/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "s3.h" 
#include "scribbler.h"

#define PI 3.14159265358979323846

# define bot_diameter 145

# define b_speed 50

# define white 0
# define black 1

int basic_speed = b_speed;
int low_speed = b_speed * 0.7;
int high_speed = b_speed / 0.7;

char *readout;

int left_speed;
int right_speed;

int turn_direction;
int turn_distance;
int straight_distance;
float hunt_left_count_start;

float line_left_count_start;

int LeftObstacle;
int RightObstacle;
int CenterObstacle;

int SenseLightLeft;
int SenseLightRight;
int SenseLightFront;

int RandomTurn;
int RandomStraight;
int RandomReset;

int RandomTurnComplete; 
int RandomStraightComplete; 
int TurnFromLineComplete;

int SenseObstacle;
int SenseLine;
int SenseLineRight;
int SenseLineLeft;
int TurnFromLine;
//int SenseStall;

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


static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

void encoder_update(void);
void encoder_update_cog(void);
void TurnToLight(int basic_speed);
void ReachLight(void);
void StallAvoid(int basic_speed);
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
    
    //resetRandomWalk();
   
    while(1)
    {
      //print("%d, %d, %d, %d\n", mid_left, mid_right, s3_lineSensor(S3_LEFT), s3_lineSensor(S3_RIGHT));
      sense();
      arbitrate();
      pause(200);
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
     
}



void sense(void){
  // Represent all sensory inputs as boolean variable 
  //pause(1000);
  //SenseStall = !s3_tailWheelMoving();
  /*
  if (s3_lineSensor(S3_LEFT) < mid_left || s3_lineSensor(S3_RIGHT) < mid_right){
    //s3_setLED(S3_LEFT, S3_COLOR_00FF00 );     s3_setLED(S3_CENTER, S3_COLOR_00FF00 );     s3_setLED(S3_RIGHT, S3_COLOR_00FF00 ); 
    //pause(100);
    //s3_setLED(S3_LEFT, S3_COLOR_000000 );     s3_setLED(S3_CENTER, S3_COLOR_000000 );     s3_setLED(S3_RIGHT, S3_COLOR_000000 ); 
    print("Line detected!\n");
    
    resetTurnFromLine();
    
    SenseLine = 1; } 
  */
 if (s3_lineSensor(S3_LEFT) < mid_left) {
    //s3_setLED(S3_LEFT, S3_COLOR_00FF00 );     s3_setLED(S3_CENTER, S3_COLOR_00FF00 );     s3_setLED(S3_RIGHT, S3_COLOR_00FF00 ); 
    //pause(100);
    //s3_setLED(S3_LEFT, S3_COLOR_000000 );     s3_setLED(S3_CENTER, S3_COLOR_000000 );     s3_setLED(S3_RIGHT, S3_COLOR_000000 ); 
    print("Line detected left!\n");
    
    resetTurnFromLine();
    
    SenseLineLeft = 1; } 
    
    
  if (s3_lineSensor(S3_RIGHT) < mid_right){
    //s3_setLED(S3_LEFT, S3_COLOR_00FF00 );     s3_setLED(S3_CENTER, S3_COLOR_00FF00 );     s3_setLED(S3_RIGHT, S3_COLOR_00FF00 ); 
    //pause(100);
    //s3_setLED(S3_LEFT, S3_COLOR_000000 );     s3_setLED(S3_CENTER, S3_COLOR_000000 );     s3_setLED(S3_RIGHT, S3_COLOR_000000 ); 
    print("Line detected right!\n");
    
    resetTurnFromLine();
    
    SenseLineRight = 1; } 
      
  //TurnFromLine = fabs(encoder_vals[0] - line_left_count_start) < bot_diameter * PI / 2;
  TurnFromLineComplete = fabs(encoder_vals[0] - line_left_count_start) > 50 ;

  LeftObstacle = s3_simpleObstacle(S3_IS, S3_LEFT) && !s3_simpleObstacle(S3_IS, S3_RIGHT);
  RightObstacle = s3_simpleObstacle(S3_IS, S3_RIGHT) && !s3_simpleObstacle(S3_IS, S3_LEFT);
  CenterObstacle = s3_simpleObstacle(S3_IS, S3_CENTER);
  
  SenseLightLeft = s3_simpleLight(S3_IS, SCRIBBLER_LEFT);
  SenseLightRight = s3_simpleLight(S3_IS, SCRIBBLER_RIGHT);
  SenseLightFront = s3_simpleLight(S3_IS, SCRIBBLER_CENTER);
  
  RandomTurnComplete = fabs(encoder_vals[0] - hunt_left_count_start) > turn_distance;
  RandomStraightComplete = fabs(encoder_vals[0] - hunt_left_count_start - turn_distance) > straight_distance;
  
  
  
  //RandomReset = fabs(encoder_vals[0] - hunt_left_count_start - turn_distance) > straight_distance;
  
  /*
  RandomTurn = fabs(encoder_vals[0] - hunt_left_count_start) < turn_distance;
  RandomStraight = fabs(encoder_vals[0] - hunt_left_count_start - turn_distance) < straight_distance;
  RandomReset = fabs(encoder_vals[0] - hunt_left_count_start - turn_distance) > straight_distance;
  */
}

/*
void HuntLight(int basic_speed){
  // If no brightest light detected, turn on spot until light found
  if (!s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_LEFT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){  
    
    print("hunting light \n");
    //encoder_update();                          // get new encoder values
    float hunt_left_count_start = encoder_vals[0];  // left encoder count at start 
  
    while(fabs(encoder_vals[0] - hunt_left_count_start) < L * 3.142){     
    
      s3_motorSet(basic_speed, -basic_speed, 0);    
      
      //encoder_update();                             // get new encoder values
      
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected 
          s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
          s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
            break;}      
    }
  }    
}      
*/


void arbitrate()
{    
  left_colour = S3_COLOR_000000;  center_colour = S3_COLOR_000000;  right_colour = S3_COLOR_000000;
  
  
  /*
  if(SenseObstacle){
    IR_ObstacleAvoid(basic_speed);
    resetRandomWalk();
  }  
  */ 
   
  /*
  if( RandomTurnComplete * RandomStraightComplete ){
    //char *name = "random";
    readout = "random reset";
    //print("%s \n", readout);
    resetRandomWalk();
  }


  if( !RandomStraightComplete ){
    //print("%f, %d, random straight\n", fabs(encoder_vals[0] - hunt_left_count_start), straight_distance); 
    readout = "random straight";
    left_speed = low_speed;
    right_speed = low_speed;     
    //s3_motorSet(basic_speed, basic_speed, 0); 
  }

  if( !RandomTurnComplete){
    //print("%f, %d, random turn\n", fabs(encoder_vals[0] - hunt_left_count_start), turn_distance); 
    readout = "random turn"; 
    left_speed = basic_speed;
    right_speed = -basic_speed;  
    //s3_motorSet(basic_speed, -basic_speed, 0); 
  }
  */


  
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
    //print("%f, %d, random turn\n", fabs(encoder_vals[0] - hunt_left_count_start), turn_distance); 
    readout = "random straight"; 
    left_speed = basic_speed;
    right_speed = basic_speed;
    left_colour = S3_COLOR_000000 ;     center_colour = S3_COLOR_FF7F00;     right_colour = S3_COLOR_000000;  
    //s3_motorSet(basic_speed, -basic_speed, 0); 
  }
 
  
  if( RandomStraightComplete ){
    //char *name = "random";
    print("random reset\n");
    readout = "random reset";
    //print("%s \n", readout);
    resetRandomWalk();
    left_colour = S3_COLOR_000000 ;     center_colour = S3_COLOR_000000;     right_colour = S3_COLOR_000000;  
  }
  
  /*
  if( TurnFromLine ){
    readout = "turning from line";
    left_speed = basic_speed;
    right_speed = -basic_speed;
  }  
   

  if( SenseLine ){
    print("line sensed");
    readout = "line sensed";
    resetTurnFromLine();
  } 
  */
  
  
  if( SenseLightLeft ){
    //print("Light Left\n");
    readout = "Light Left"; 
    left_speed = -low_speed;
    right_speed = low_speed;
    //s3_motorSet(-high_speed, high_speed, 0);
    resetRandomWalk();
    left_colour = S3_COLOR_00FF00  ;     center_colour = S3_COLOR_000000;     right_colour = S3_COLOR_000000;  
    //resetTurnFromLine();
    //TurnFromLine = 0;
  }  
  
  
  if( SenseLightRight ){
    //print("Light Right\n");
    readout = "Light Right"; 
    left_speed = basic_speed;
    right_speed = -basic_speed;
    left_colour = S3_COLOR_000000 ;     center_colour = S3_COLOR_000000;     right_colour = S3_COLOR_00FF00 ;  
    //s3_motorSet(high_speed, -high_speed, 0);
    resetRandomWalk();
    //resetTurnFromLine();
    
  }  
    
  if( SenseLightFront ){
    //print("Light Ahead\n");
    readout = "Light Front"; 
    int light = s3_lightSensor(SCRIBBLER_CENTER);
    int motor_speed = mapInt(light, 0, 255, 100, 0);
    left_speed = motor_speed;
    right_speed = motor_speed;
    left_colour = S3_COLOR_000000 ;     center_colour = S3_COLOR_00FF00 ;     right_colour = S3_COLOR_000000;  
    //s3_motorSet(motor_speed, motor_speed, 0);
    resetRandomWalk();
    //resetTurnFromLine();
    //TurnFromLine = 0;
  } 
  
  
  /*
  if( SenseLine ){
    readout = "Turning from Line"; 
    resetTurnFromLine();
    left_speed = basic_speed;
    right_speed = -basic_speed;
  }
  */ 

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
  
  
  if( SenseLine * TurnFromLineComplete ){
    readout = "Turn from Line Complete"; 
    resetTurnFromLine();
    //left_speed = 0;
    //right_speed = 0;
  }
    
  
  
  /*
  if(LeftObstacle){
    readout = "Obstacle Left"; 
    left_speed = 70;
    right_speed = -10;

  }  
  
  
  if(RightObstacle){
    readout = "Obstacle Right"; 
    left_speed = -10;
    right_speed = 70; 
  } 
  
  
  if(CenterObstacle){
    readout = "Obstacle Center"; 
    left_speed = 70;
    right_speed = -70; 
    
  } 
  */

  /*
  if( SenseStall * SenseLightFront || SenseStall * RandomStraight * !RandomTurn * !SenseLightLeft * !SenseLightRight){
    //print("Light Ahead\n");
    readout = "Stall Avoid"; 
    s3_motorSet(-basic_speed, -basic_speed, 1500);   // move backwards
    s3_simpleSpin(90, basic_speed, 0);               // quarter clockwise turn
    //resetRandomWalk();
  } 
  */


  s3_motorSet(left_speed, right_speed, 0);  
  s3_setLED(S3_LEFT, left_colour );     s3_setLED(S3_CENTER, center_colour );     s3_setLED(S3_RIGHT, right_colour); 
  //print("%s SenseLine = %d, TurnDirection = %d\n", readout, SenseLine, turn_direction);
  print("%s SenseLine = %d, TurnDirection = %d, %d, %d, %d, %d\n", readout, SenseLine, turn_direction, mid_left, mid_right, s3_lineSensor(S3_LEFT), s3_lineSensor(S3_RIGHT));
      

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
  


void RandomWalk(int basic_speed){
  // Turn on spot by random distance, then move by random distance until light found 
    print("hunting light \n");
    
    // turn a random distance
    if(fabs(encoder_vals[0] - hunt_left_count_start) < turn_distance){      
      print("%f, %d, hunting light turn\n", fabs(encoder_vals[0] - hunt_left_count_start), turn_distance);     
      s3_motorSet(basic_speed, -basic_speed, 0);    
     }
    
    
    // drive a random distance
    else if(fabs(encoder_vals[0] - hunt_left_count_start - turn_distance) < straight_distance){  
      print("%f, %d, hunting light straight\n", fabs(encoder_vals[0] - hunt_left_count_start), straight_distance);     
      s3_motorSet(basic_speed, basic_speed, 0);     
     }
     
     
   // reset distances
    else if(fabs(encoder_vals[0] - hunt_left_count_start - turn_distance) > straight_distance){ 
      resetRandomWalk();               
    }
        
}      


void HuntLight(int basic_speed){
  // If no brightest light detected, turn on spot by random distance, then move by random distance until light found
  if (!s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_LEFT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){  
    
    print("hunting light \n");

    hunt_left_count_start = encoder_vals[0];   // left encoder count at start 
    turn_distance = random(50, 100);      // distance to turn 
    straight_distance = random(50, 100);  // distance to go straight 
    
    while(fabs(encoder_vals[0] - hunt_left_count_start) < turn_distance){   // turn a random distance until light found  
      print("%f, %d, hunting light turn\n", fabs(encoder_vals[0] - hunt_left_count_start), turn_distance);   
    
      s3_motorSet(basic_speed, -basic_speed, 0);    
     
      
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected 
          s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
          s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
            break;}      
    }//while 
    
    
    
    //hunt_left_count_start = encoder_vals[0];   // left encoder count at start
    while(fabs(encoder_vals[0] - hunt_left_count_start - turn_distance) < straight_distance){ // drive a random distance until light found  
      print("%f, %d, hunting light straight\n", fabs(encoder_vals[0] - hunt_left_count_start), straight_distance);  
    
      s3_motorSet(basic_speed, basic_speed, 0);    
      
      
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected 
          s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
          s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
            break;}  
      /*      
      if (!s3_tailWheelMoving()){                    // stop if stalled
            StallAvoid(50);
            break;}
      */
            
      //if (s3_simpleObstacle(S3_IS, S3_DETECTED)){  // stop if obstacle detected
           // break;}    
               
     }// while
     
  }    
}      
  


void ReachLight(void){
  // If brightest light detected directly in front of robot,
  // move in direction of light with speed inversely proportional to brightness.
  //if (s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
    print("light ahead\n");
    int light = s3_lightSensor(SCRIBBLER_CENTER);
    int motor_speed = mapInt(light, 0, 255, 100, 0);
    s3_motorSet(motor_speed, motor_speed, 0);
    pause(200);//}
}



void TurnToLight(int basic_speed){

int low_speed = basic_speed * 0.7;
int high_speed = basic_speed / 0.7;
  
  // Light is left, turn on spot towards right
  if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){
    print("light left\n");
    s3_motorSet(-high_speed, high_speed, 0);}       
  
  // Light is right, turn on spot towards right
  else if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){
    print("light right\n");
    s3_motorSet(high_speed, -high_speed, 0);} 

  else {print("no light\n");
  s3_motorSet(0, 0, 0);} 
}



void StallAvoid(int basic_speed){
  // escape maneuver if stall detected
  //while (1) {
    //pause(200);
    if (!s3_tailWheelMoving()) {
      print("Stalled \n");
      s3_motorSet(-basic_speed, -basic_speed, 1500);   // move backwards
      s3_simpleSpin(90, basic_speed, 0);               // quarter clockwise turn
    }
    else
    {
      print("Not Stalled \n");
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

/*
void encoder_update(void) {   
    // Value : a 32 bit integer containing registers describig the behavious of drive and idler wheel encoders
    // Updates an array of 9 values
    // 0 Left wheel distance count (mm)
    // 1 Right wheel distance count (mm)
    // 2 Time in 1/10 second since the last idler encoder edge
    // 3 Idler wheel velocity
    // 4 Non-zero if one or more motors are turning.  
    // 5-8 Variables used in encoder calcs     

    
    int32_t value = scribbler_motion();
    
    int32_t e0 = (value >> 24);           
    encoder_vals[7] = e0 * 0.25;  //new 1           
    
    int32_t e1 = (value >> 16) & 0xff;           
    encoder_vals[8] = e1 * 0.25;  //new 2  
      
      //if (new > old){
      if (encoder_vals[7] >= encoder_vals[5]){
        encoder_vals[0] += encoder_vals[7] - encoder_vals[5];
      }
      else {  
        encoder_vals[0] += encoder_vals[7]; 
      }    
      
      if (encoder_vals[8] >= encoder_vals[6]){
        encoder_vals[1] += encoder_vals[8]-encoder_vals[6];
      }
      else {  
        encoder_vals[1] += encoder_vals[8]; 
      }             
        
    //old = new;
    encoder_vals[5] = encoder_vals[7];  // old1 = new1
    encoder_vals[6] = encoder_vals[8];  // old2 = new2
    
    int32_t e2 = (value >> 8)  & 0xff;           
    encoder_vals[2] = e2;
    
    int32_t e3 = value & 0xfc;           
    encoder_vals[3] = e3;
    
    int32_t e4 = value & 0x3;           
    encoder_vals[4] = e4;               

}
*/

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




void LineFollowProp(void){
      // line following : proportional controller
      
      /*
      int leftave_white = s3_memoryRead(1);                      // stored average IR sensor calibration values
      int rightave_white = s3_memoryRead(2);
      int leftave_black = s3_memoryRead(3);
      int rightave_black = s3_memoryRead(4);
      
      int mid_left = (leftave_white - leftave_black) / 2;        // midpoint between black and white
      int mid_right = (rightave_white - rightave_black) / 2;
      */
      
      int v_basic = 30;                                          // basic speed

      
      while(1){
      
        if(s3_lineSensor(S3_LEFT) > mid_left && s3_lineSensor(S3_RIGHT) < mid_right){          // left=white, right=black --> turn right
          int r_error = rightave_white - s3_lineSensor(S3_RIGHT);
          int v_l = v_basic + (100 - v_basic) * r_error / (rightave_white - rightave_black);
          s3_motorSet(v_l, -v_l/2, 0);
          pause(200);}
          
        else if(s3_lineSensor(S3_RIGHT) > mid_right && s3_lineSensor(S3_LEFT) < mid_left){     // left=black, right=white --> turn left
          int l_error = leftave_white - s3_lineSensor(S3_LEFT);
          int v_r = v_basic + (100 - v_basic) * l_error / (leftave_white - leftave_black);
          s3_motorSet(-v_r/2, v_r, 0);
          pause(200);}
          
        else{                                     // left = right = white/black --> go straight
          s3_motorSet(v_basic, v_basic, 0);
          pause(200);}

      
      }
}
