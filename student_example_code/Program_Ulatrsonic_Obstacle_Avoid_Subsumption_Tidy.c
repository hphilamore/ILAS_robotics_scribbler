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
# define n_servo_pos 5

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
int AvoidFrontObstacle_Start;
int AvoidFrontObstacle_dist = 50; //0.25 * bot_diameter * PI;
int AvoidFrontObstacle;
int AvoidFrontObstacle_Complete;
int obstacle;
int closest_is_right;
int closest_is_left;
int closest_is_front;
int min_ping_index;
static int ping_dist_cm[n_servo_pos];

// line avoid variables
float line_left_count_start;
int TurnFromLineComplete;
int SenseLineRight;
int SenseLineLeft;
int leftave_white; 
int rightave_white; 
int leftave_black; 
int rightave_black; 
int mid_left; 
int mid_right;

// servo variables
int servo_pos;
int servo_dir = 1;
int servo_pin = 0;             // I/O pin on which servo is attached
static int servo_pos_array[n_servo_pos];
int servo_index;
int servo_change_dir;

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
  
// encoder count variables
static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

void encoder_update(void);
void encoder_update_cog(void);
void TurnToLight(int basic_speed);
void ReachLight(void);
void IR_ObstacleAvoid(int basic_speed);
void HuntLight(int basic_speed);

void RandomWalk(int basic_speed);
void arbitrate(void);




int main()                                    // Main function
{
  s3_setup();
  cog_run(encoder_update_cog, 65);
  

  if (s3_resetButtonCount() == 3) {
    
    initialise_ping();
    initialise_servo_angles();

    while(1)
    {
      
      // old method
      /*
      ping_distance_cm_array();
      servo_sweep_array();
      min_ping_index = min_ping_array();
      obstacle_avoid_ping_array();
      print("\n");
      */
      
      // subsumption method
      sense();
      arbitrate();
      print("\n");
      //pause(200);
      
      
      // old method revised
      /*
      ping_scan();
      obstacle_avoid_ping_array();
      print("\n");
      */
    }  
  }
}



void sense(void){
 
  min_ping_index = min_ping_array();
  
  obstacle = ping_dist_cm[min_ping_index] < th_ping_avoid_cm;

  closest_is_right = servo_pos_array[min_ping_index] < 90;
  
  closest_is_left = servo_pos_array[min_ping_index] > 90;
  
  closest_is_front = servo_pos_array[min_ping_index] == 90;

  AvoidFrontObstacle_Complete = fabs(encoder_vals[0] - AvoidFrontObstacle_Start) > AvoidFrontObstacle_dist;
  
}


void arbitrate()
{    
  left_colour = S3_COLOR_000000;  center_colour = S3_COLOR_000000;  right_colour = S3_COLOR_000000;
  left_speed = 50;    
  right_speed = 50; 
  readout = "No Obstacle";
  
  if( obstacle * closest_is_right ){
    left_speed = -20;    
    right_speed = 70;
    right_colour = S3_COLOR_FF0000;
    readout = "Obstacle is Right";                  
  }
  
  
  if( obstacle * closest_is_left ){  
    left_speed = 70;    
    right_speed = -20;
    left_colour = S3_COLOR_FF0000;
    readout = "Obstacle is Left"; 
  }
  
  
  if( obstacle * closest_is_front ){
    AvoidFrontObstacle = 1;
    AvoidFrontObstacle_Start = encoder_vals[0];
    AvoidFrontObstacle = 1;
  }
  
  if( AvoidFrontObstacle ){
    left_speed = 90;    
    right_speed = -90;
    center_colour = S3_COLOR_FF0000;
    readout = "Obstacle is Front"; 
    
  }    

  if( AvoidFrontObstacle * AvoidFrontObstacle_Complete ){
    AvoidFrontObstacle = 0;
    initialise_ping(); 
    readout = "Avoid Front Obstacle Complete"; 
  }
  
  ping_scan();
  
  
  s3_motorSet(left_speed, right_speed, 0);  
  s3_setLED(S3_LEFT, left_colour );     s3_setLED(S3_CENTER, center_colour );     s3_setLED(S3_RIGHT, right_colour); 
  
  print("\t %d,", ping_dist_cm[min_ping_index]);
  print("\t %d,", min_ping_index);
  print("%s", readout);
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


void servo_sweep_array(void) {
  // Sweeps servo from side to side, range of motion 180 degrees
  
  servo_pos = servo_pos_array[servo_index];

  servo_angle(servo_pin, 10 * servo_pos);
  
  servo_index += servo_dir;

  if(servo_index >= (n_servo_pos - 1) || servo_index <= 0){
    servo_dir *= -1;
  } 
} 


void ping_distance_cm_array(void){
  // Reads Distance to nearest object 

  ping_dist_cm[servo_index] = ping_cm(ping_pin);
  print("%d\t", ping_dist_cm[servo_index]);
} 



void ping_scan(void) {
  // Sweeps servo from side to side, range of motion 180 degrees
  // Records range sensed by ping at each angle
  
  servo_angle(servo_pin, 10 * servo_pos_array[servo_index]);
  
  pause(150); // pause while servo reaches position
  
  ping_dist_cm[servo_index] = ping_cm(ping_pin);
  
  servo_index += servo_dir;
  
  if(servo_index >= (n_servo_pos - 1) || servo_index <= 0){
    servo_dir *= -1;
  } 

} 


void obstacle_avoid_ping_array(void){
  
  /*
  int min_index = 0;
  
  print("[");
  
  print("%d,", ping_dist_cm[min_index]);
  
  for(int i=1; i < n_servo_pos; i++){
    
    print("%d,", ping_dist_cm[i]);
    
    if(ping_dist_cm[i] < ping_dist_cm[min_index]){
			min_index = i;
    }
  } 
  print("]");
  
  print("\t %d \t", servo_pos_array[min_index]); 
  */
  

   
  if(ping_dist_cm[min_ping_index] < th_ping_avoid_cm){
    print(" %d \t", ping_dist_cm[min_ping_index]);

    if(servo_pos_array[min_ping_index] < 90){
      // obstacle to right of robot
      s3_setLED( S3_RIGHT, S3_COLOR_FF0000 ); 
      s3_motorSet(-20, 70, 0);  // turn left
       
    } 
    
    else if(servo_pos_array[min_ping_index] > 90){
      s3_setLED( S3_LEFT, S3_COLOR_FF0000 );
      s3_motorSet(70, -20, 0);  // turn right
      
    } 
    
    else if(servo_pos_array[min_ping_index] == 90){
      s3_setLED( S3_CENTER, S3_COLOR_FF0000 );
      s3_simpleSpin(90, 70, 0);
      initialise_ping();
      
      
      
    }            

  }
  
  else{
    s3_setLED(S3_LEFT, S3_COLOR_000000 );     s3_setLED(S3_CENTER, S3_COLOR_000000 );     s3_setLED(S3_RIGHT, S3_COLOR_000000); 
    s3_motorSet(50, 50, 0);  // turn right
  }        
}  


void servo_sweep_basic(void) {
  
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

void initialise_ping(void){
  for(int i=0; i < n_servo_pos; i++){
        ping_dist_cm[i] = 300;
      }
} 

void initialise_servo_angles(void){
  for(int p=0; p < n_servo_pos; p++){
      servo_pos_array[p] = p * 180 / (n_servo_pos - 1);
  }
}



int min_ping_array(void){

  int min_index = 0;
  
  print("[");
  
  print("%d,", ping_dist_cm[min_index]);
  
  for(int i=1; i < n_servo_pos; i++){
    
    //print("%d,", ping_dist_cm[i]);
    
    if(ping_dist_cm[i] < ping_dist_cm[min_index]){
      min_index = i;
    }			
   
    //print("(min_i=%d, %d),", min_index, ping_dist_cm[i]);
    print("%d,", ping_dist_cm[i]);
    
   
  } 
  print("]");
  
  //print("\t %d \t", servo_pos_array[min_index]); 
  
  return min_index; 
}         
  
