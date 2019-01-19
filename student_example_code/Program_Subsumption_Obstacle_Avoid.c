/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "s3.h" 
#include "scribbler.h"
#include "servo.h"
#include "ping.h"

#define PI 3.14159265358979323846
# define bot_diameter 145
# define b_speed 50
# define white 0
# define black 1
# define n_servo_pos 7

// motor variables
int basic_speed = b_speed;
int low_speed = b_speed * 0.7;
int high_speed = b_speed / 0.7;
int left_speed;
int right_speed;

// print variables
char *readout;

// random walk variables
int turn_direction;
int turn_distance;
int straight_distance;

// ultrasound obstacle avioidance variables
int obj_distance_cm;
int ping_pin = 1;           // I/O pin on which ping is attached
int th_ping_avoid_cm = 20;  // distnace below which an obstacle is detected
static float ping_dist_cm[n_servo_pos]; //= {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

// line avoid variables
float line_left_count_start;
int TurnFromLineComplete;
int SenseLineRight;
int SenseLineLeft;

// servo_variables
//int servo_time_start; 
//int servo_time_interval = 400; // ms
//int n_servo_pos = 7;           // number of servo positions per sweep
int servo_pos;
int servo_dir = 1;
int servo_pin = 0;             // I/O pin on which servo is attached
static float servo_pos_array[n_servo_pos];

// light sense variables
int SenseLightLeft;
int SenseLightRight;
int SenseLightFront;

// random walk variables
int RandomTurn;
int RandomStraight;
int RandomReset;
float hunt_left_count_start;
int RandomTurnComplete; 
int RandomStraightComplete; 

// LED variables
int32_t left_colour;
int32_t center_colour;
int32_t right_colour;


// Line follower variables
int leftave_white; 
int rightave_white; 
int leftave_black; 
int rightave_black; 
int mid_left; 
int mid_right;  

// encoder count variables
static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

void encoder_update(void);
void encoder_update_cog(void);
void TurnToLight(int basic_speed);
void ReachLight(void);
void IR_ObstacleAvoid(int basic_speed);
void HuntLight(int basic_speed);
void resetRandomWalk(void);
void RandomWalk(int basic_speed);
void arbitrate(void);
void resetTurnFromLine(void);
void Calibrate(int colour);



int main()                                    // Main function
{
  s3_setup();
  cog_run(encoder_update_cog, 65);
  resetRandomWalk();
  servo_time_start = CNT/st_msTicks;
  
  if (s3_resetButtonCount() == 1) {
    // Calibrate IR line sensors for WHITE   
    Calibrate(white);      
  }  
  
  
  if (s3_resetButtonCount() == 2) {
    // Calibrate IR line sensors for BLACK 
    Calibrate(black);     
  }  


  if (s3_resetButtonCount() == 3) {
    
    // Line follower variables
    leftave_white = s3_memoryRead(1);                      // stored average IR sensor calibration values
    rightave_white = s3_memoryRead(2);
    leftave_black = s3_memoryRead(3);
    rightave_black = s3_memoryRead(4);
    mid_left = (leftave_white + leftave_black) / 5;        // midpoint between black and white
    mid_right = (rightave_white + rightave_black) / 5;  
   
    while(1)
    {
      //sense();
      servo_sweep();
      ping_distance_cm();
      
      print("\n");
      obstacle_avoid_ping();
      //arbitrate();
      pause(100);
    }  
  }
  
  if (s3_resetButtonCount() == 4) {
    
    // Line follower variables
    leftave_white = s3_memoryRead(1);                      // stored average IR sensor calibration values
    rightave_white = s3_memoryRead(2);
    leftave_black = s3_memoryRead(3);
    rightave_black = s3_memoryRead(4);
    mid_left = (leftave_white + leftave_black) / 5;        // midpoint between black and white
    mid_right = (rightave_white + rightave_black) / 5;  
   
    while(1)
    {
      print("%d, %d, %d, %d\n", mid_left, mid_right, s3_lineSensor(S3_LEFT), s3_lineSensor(S3_RIGHT));
    }  
  } 
  
  if (s3_resetButtonCount() == 5) {
    while(1){
      turn_direction = random(0, 1);      // distance to turn
      print("%d", turn_direction); 
    }    
  } 
  
  if (s3_resetButtonCount() == 6) {  
    servo_sweep_v1();
  } 
  
  servo_pos += servo_dir * 180 / (n_servo_pos - 1);

  if(servo_pos >= 180 || servo_pos <= 0){
    servo_dir *= -1;
  } 
  
  if (s3_resetButtonCount() == 7) {
    while(1){
    servo_pos += servo_dir * 180 / (n_servo_pos - 1);  
  
    if(servo_pos >= 180 || servo_pos <= 0){
      servo_dir *= -1;
    }
       
    servo_angle(servo_pin, 10 * servo_pos);
    }     
  } 
   
     
}



void sense(void){
 if (s3_lineSensor(S3_LEFT) < mid_left) {
    print("Line detected left!\n");    
    resetTurnFromLine();    
    SenseLineLeft = 1; } 
    
    
  if (s3_lineSensor(S3_RIGHT) < mid_right){
    print("Line detected right!\n");    
    resetTurnFromLine();    
    SenseLineRight = 1; } 
      

  TurnFromLineComplete = fabs(encoder_vals[0] - line_left_count_start) > 50 ;
  
  SenseLightLeft = s3_simpleLight(S3_IS, SCRIBBLER_LEFT);
  SenseLightRight = s3_simpleLight(S3_IS, SCRIBBLER_RIGHT);
  SenseLightFront = s3_simpleLight(S3_IS, SCRIBBLER_CENTER);
  
  RandomTurnComplete = fabs(encoder_vals[0] - hunt_left_count_start) > turn_distance;
  RandomStraightComplete = fabs(encoder_vals[0] - hunt_left_count_start - turn_distance) > straight_distance;
  
}


void arbitrate()
{    
  left_colour = S3_COLOR_000000;  center_colour = S3_COLOR_000000;  right_colour = S3_COLOR_000000;
  
  if(turn_direction == 0){ 
    readout = ("random turn right"); 
    left_speed = basic_speed;    
    right_speed = -basic_speed;
    left_colour = S3_COLOR_000000;     center_colour = S3_COLOR_000000;     right_colour = S3_COLOR_FF7F00 ;} 
    
  if(turn_direction == 1){
    readout = ("random turn left");  
    left_speed = basic_speed;    
    right_speed = -basic_speed;
    left_colour = S3_COLOR_FF7F00 ;     center_colour = S3_COLOR_000000;     right_colour = S3_COLOR_000000;}  

  if( RandomTurnComplete){
    readout = "random straight"; 
    left_speed = basic_speed;
    right_speed = basic_speed;
    left_colour = S3_COLOR_000000 ;     center_colour = S3_COLOR_FF7F00;     right_colour = S3_COLOR_000000;  
  }
 
  
  if( RandomStraightComplete ){
    print("random reset\n");
    readout = "random reset";
    resetRandomWalk();
    left_colour = S3_COLOR_000000 ;     center_colour = S3_COLOR_000000;     right_colour = S3_COLOR_000000;  
  }
  

  if( SenseLightLeft ){
    readout = "Light Left"; 
    left_speed = -low_speed;
    right_speed = low_speed;
    resetRandomWalk();
    left_colour = S3_COLOR_00FF00  ;     center_colour = S3_COLOR_000000;     right_colour = S3_COLOR_000000;  
  }  
  
  
  if( SenseLightRight ){
    readout = "Light Right"; 
    left_speed = basic_speed;
    right_speed = -basic_speed;
    left_colour = S3_COLOR_000000 ;     center_colour = S3_COLOR_000000;     right_colour = S3_COLOR_00FF00 ;  
    resetRandomWalk(); 
  }  
    
  if( SenseLightFront ){
    readout = "Light Front"; 
    int light = s3_lightSensor(SCRIBBLER_CENTER);
    int motor_speed = mapInt(light, 0, 255, 100, 0);
    left_speed = motor_speed;
    right_speed = motor_speed;
    left_colour = S3_COLOR_000000 ;     center_colour = S3_COLOR_00FF00 ;     right_colour = S3_COLOR_000000;  
    resetRandomWalk();
  } 

  if( SenseLineLeft ){
    readout = "Turning from Line left"; 
    resetTurnFromLine();
    left_speed = basic_speed;
    right_speed = -basic_speed;
    left_colour = S3_COLOR_FF0000  ;  
  } 
  
  if( SenseLineRight ){
    readout = "Turning from Line right"; 
    resetTurnFromLine();
    left_speed = basic_speed;
    right_speed = -basic_speed;
    right_colour = S3_COLOR_FF0000;  
  }
  
  
  if( SenseLineLeft * TurnFromLineComplete || SenseLineRight * TurnFromLineComplete ){
    readout = "Turn from Line Complete"; 
    resetTurnFromLine();
  }


  servo_sweep();
  s3_motorSet(left_speed, right_speed, 0);  
  s3_setLED(S3_LEFT, left_colour );     s3_setLED(S3_CENTER, center_colour );     s3_setLED(S3_RIGHT, right_colour); 
  print("%s", readout);
      

}

void resetRandomWalk(void){
  // Resets the random walk distances
    hunt_left_count_start = encoder_vals[0];   // left encoder count at start 
    turn_distance = random(50, 100);      // distance to turn 
    turn_direction = random(0, 1);      // distance to turn 
    straight_distance = random(100, 200);  // distance to go straight
  }  
  
void resetTurnFromLine(void){
  // Restarts the turn away from line
    line_left_count_start = encoder_vals[0];   // left encoder count at start 
    //SenseLine = 0;
    SenseLineLeft = 0;
    SenseLineRight = 0;
  }  
  





void encoder_update_cog(void) { 
    while(1){  
    // Value : a 32 bit integer containing registers describig the behavious of drive and idler wheel encoders
    // Updates an array of 9 values
    // 0 Left wheel distance count (mm)
    // 1 Right wheel distance count (mm)
    // 2 Time in 1/10 second since the last idler encoder edge
    // 3 Idler wheel velocity
    // 4 Non-zero if one or more motors are turning.  
    // 5-8 Variables used in encoder calcs     

    
    int32_t value = scribbler_motion();
    
    //int32_t e0 = (value >> 24);           
    //encoder_vals[7] = e0 * 0.25;  //new 1 
    encoder_vals[7] = (value >> 24) * 0.25;  //new 1           
    
    int32_t e1 = (value >> 16) & 0xff; 
    if(e1 > 100){e1 -= 256;}           
    encoder_vals[8] = e1 * 0.25;  //new 2 
    //encoder_vals[8] = ((value >> 16) & 0xff) * 0.25;  //new 2
       
      
      //if (new > old){
      //if (encoder_vals[7] >= encoder_vals[5]){
      if (abs(encoder_vals[7]) >= abs(encoder_vals[5])){
        encoder_vals[0] += encoder_vals[7] - encoder_vals[5];
      }
      else {  
        encoder_vals[0] += encoder_vals[7]; 
      }    
      
      //if (encoder_vals[8] >= encoder_vals[6]){
      if (abs(encoder_vals[8]) >= abs(encoder_vals[6])){
        encoder_vals[1] += encoder_vals[8] - encoder_vals[6];
      }
      else {  
        encoder_vals[1] += encoder_vals[8]; 
      }             
        
    //old = new;
    encoder_vals[5] = encoder_vals[7];  // old1 = new1
    encoder_vals[6] = encoder_vals[8];  // old2 = new2
    
    //int32_t e2 = (value >> 8)  & 0xff;           
    encoder_vals[2] = (value >> 8)  & 0xff;
    
    //int32_t e3 = value & 0xfc;           
    encoder_vals[3] = value & 0xfc;
    
    //int32_t e4 = value & 0x3;           
    encoder_vals[4] = value & 0x3; 
    
    //print("%d  \t", (value >> 24)); 
    //print("%f  \n", encoder_vals[0]);  
  }  
}  
  
  
void Calibrate(int colour){
    // Calibrate IR line sensors 
    // colour = "w" (white) or "b" (black) 
    
    int left = 0;                              // variables for IR sensor values      
    int right = 0;
    int count = 0;
    
    //int bot_diameter = 145;                    // distance between two drive wheels 
    
    int encoder_limit;                         // encoder count at which motion stops
    int vl;                                    // left velocity
    int vr;                                    // right velocity
    
    //print("%s", colour);
    
    
    if(colour == white){
      encoder_limit = bot_diameter * PI;    
      vl = 50;                             
      vr = -50;                            
    }   
    
    else if(colour == black){
      encoder_limit = 100;                  
      vl = 50;                             
      vr = 50;                             
    } 
       
    
    float left_count_start = encoder_vals[0];  // left encoder count at start 
    
    // turn until set limit reached
    while(fabs(encoder_vals[0] - left_count_start) < encoder_limit){     
      
      s3_motorSet(vl, vr, 0);                 // turn on spot
      left += s3_lineSensor(S3_LEFT);          // cumulative sum left IR sensor
      right += s3_lineSensor(S3_RIGHT);        // cumulative sum right IR sensor
      count += 1;                              // cumulative sum while loops

    }
    s3_motorSet(0, 0, 0);                  // stop moving         
    
    int leftave = left/count;        // average left IR sensor
    int rightave = right/count;      // average right IR sensor
    print("left ave = %d, right ave = %d", leftave, rightave);
    
    
    if(colour == white){
      s3_memoryWrite(1, leftave);      // store in non-volatile memory 
      s3_memoryWrite(2, rightave);                           
    }   
    
    else if(colour == black){
      s3_memoryWrite(3, leftave);      // store in non-volatile memory 
      s3_memoryWrite(4, rightave);                           
    }    
    
}  


void IR_ObstacleAvoid(int basic_speed){
  // Performs escape maneuver if IR sensors detect obstacle
  // Otherwise drives straight at input basic speed
  
  int left = s3_simpleObstacle(S3_IS, S3_LEFT);
  int right = s3_simpleObstacle(S3_IS, S3_RIGHT);
  int centre = s3_simpleObstacle(S3_IS, S3_CENTER);
  int detected = s3_simpleObstacle(S3_IS, S3_DETECTED);
  /*
  print("%d\t", detected);
  print("\t");
  print("%d\t", left);
  print("%d\t", centre);
  print("%d\n", right);
  */
  
  if(detected){
    print("Obstacle Detected \n");
    if(left && !right){         // obstacle to left
      print("Obstacle left \n");
      s3_motorSet(-10, 70, 0);  // turn right
    }
    else if(right && !left){    // obstacle to right
      print("Obstacle right \n");
      s3_motorSet(70, -10, 0);  // turn left
    } 
    else{                       // obstacle directly in front
      print("Obstacle front \n");
      s3_simpleSpin(60, 50, 0); // clockwise turn
    } 
  }
  else{
    print("Obstacle Not Detected \n");
    s3_motorSet(basic_speed, basic_speed, 0);
  }                
}   

void servo_sweep(void) {
  // Sweeps servo from side to side, range of motion 180 degrees
  
    
  //print("%d\t", CNT/st_msTicks - servo_time_start);
  
  servo_pos += servo_dir * 180 / (n_servo_pos - 1);

  if(servo_pos >= 180 || servo_pos <= 0){
    servo_dir *= -1;
  } 
  
  servo_angle(servo_pin, 10 * servo_pos);
  //servo_angle(servo_pin, 0);
  
  //obj_distance_cm = ping_cm(ping_pin);
  
  print("%d\t", servo_pos);
  //print("%d\t", obj_distance_cm);

  //print("\n");

} 

void ping_distance_cm(void){
  // Reads Distance to nearest object 
  obj_distance_cm = ping_cm(ping_pin);
  print("%d\t", obj_distance_cm);
}  
             


void obstacle_avoid_ping(void){
  // turn light to red if obstacle detected 
  if(obj_distance_cm < th_ping_avoid_cm){
    s3_setLED(S3_LEFT, S3_COLOR_FF0000 );     s3_setLED(S3_CENTER, S3_COLOR_FF0000 );     s3_setLED(S3_RIGHT, S3_COLOR_FF0000); 


    if(servo_pos < 90){
      // obstacle to right of robot
      s3_motorSet(-70, -20, 0);  // turn left
      
    } 
    
    else if(servo_pos > 90){
      // obstacle to left of robot
      s3_motorSet(-20, 70, 0);  // turn right
      
    } 
    
    else{
      // obstacle is in front of robot
      s3_simpleSpin(180, 70, 0);

    }            
    
    
  }
  
  else{
    s3_setLED(S3_LEFT, S3_COLOR_000000 );     s3_setLED(S3_CENTER, S3_COLOR_000000 );     s3_setLED(S3_RIGHT, S3_COLOR_000000); 
    s3_motorSet(50, 50, 0);  // turn right
  }        
}  


void servo_sweep_v1(void) {
  
  //#include "servo.h"
  
  while(1){
    
    servo_angle(0, 10 * 180);
    
    
    for (int pos = 0; pos <= 180; pos += 36) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      servo_angle(servo_pin, 10 * pos);   // tell servo to go to position in variable 'pos'
      pause(100);                          // waits 15ms for the servo to reach the position
    }
    
    
    
    for (int pos = 180; pos >= 0; pos -= 36) { // goes from 180 degrees to 0 degrees
      servo_angle(servo_pin, 10 * pos);   // tell servo to go to position in variable 'pos'
      pause(100);                          // waits 15ms for the servo to reach the position
    }
  
  }  
}
  
