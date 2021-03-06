/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "s3.h" 
#include "scribbler.h"

# define L 145

int basic_speed = 50;
int turn_distance;
int straight_distance;
float left_count_start;

int SenseLightSide;
int SenseLightFront;
int SenseObstacle;

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



int main()                                    // Main function
{
  s3_setup();
  cog_run(encoder_update_cog, 65);
  resetRandomWalk();

  if (s3_resetButtonCount() == 1) {
   
    while(1)
    {
      //SenseObstacle = s3_simpleObstacle(S3_IS, S3_DETECTED);
      SenseLightSide = s3_simpleLight(S3_IS, SCRIBBLER_LEFT) || (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT));
      SenseLightFront = s3_simpleLight(S3_IS, SCRIBBLER_CENTER);
      arbitrate();
      
      //TurnToLight(50);
      //ReachLight();
      
      


      
      //HuntLight(50);
      //StallAvoid(50);
      //IR_ObstacleAvoid(50);
    }  
  }    
}

/*
void HuntLight(int basic_speed){
  // If no brightest light detected, turn on spot until light found
  if (!s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_LEFT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){  
    
    print("hunting light \n");
    //encoder_update();                          // get new encoder values
    float left_count_start = encoder_vals[0];  // left encoder count at start 
  
    while(fabs(encoder_vals[0] - left_count_start) < L * 3.142){     
    
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
  /*
  if(SenseObstacle){
    IR_ObstacleAvoid(basic_speed);
    resetRandomWalk();
  }  
  */ 
  
  
  if(SenseLightSide){
    TurnToLight(basic_speed);
    resetRandomWalk();
  }    
    
  else if(SenseLightFront){
    ReachLight();
    resetRandomWalk();
  }  
  
  else{
    //HuntLight(basic_speed);
    RandomWalk(basic_speed);
  }    
      

}

void resetRandomWalk(void){
  // Resets the random walk distances
    left_count_start = encoder_vals[0];   // left encoder count at start 
    turn_distance = random(50, 100);      // distance to turn 
    straight_distance = random(50, 100);  // distance to go straight
  }    
  


void RandomWalk(int basic_speed){
  // Turn on spot by random distance, then move by random distance until light found 
    print("hunting light \n");
    
    // turn a random distance
    if(fabs(encoder_vals[0] - left_count_start) < turn_distance){      
      print("%f, %d, hunting light turn\n", fabs(encoder_vals[0] - left_count_start), turn_distance);     
      s3_motorSet(basic_speed, -basic_speed, 0);    
     }
    
    
    // drive a random distance
    else if(fabs(encoder_vals[0] - left_count_start - turn_distance) < straight_distance){  
      print("%f, %d, hunting light straight\n", fabs(encoder_vals[0] - left_count_start), straight_distance);     
      s3_motorSet(basic_speed, basic_speed, 0);     
     }
     
     
   // reset distances
    else if(fabs(encoder_vals[0] - left_count_start - turn_distance) > straight_distance){ 
      resetRandomWalk();               
    }
        
}      


void HuntLight(int basic_speed){
  // If no brightest light detected, turn on spot by random distance, then move by random distance until light found
  if (!s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_LEFT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){  
    
    print("hunting light \n");

    left_count_start = encoder_vals[0];   // left encoder count at start 
    turn_distance = random(50, 100);      // distance to turn 
    straight_distance = random(50, 100);  // distance to go straight 
    
    while(fabs(encoder_vals[0] - left_count_start) < turn_distance){   // turn a random distance until light found  
      print("%f, %d, hunting light turn\n", fabs(encoder_vals[0] - left_count_start), turn_distance);   
    
      s3_motorSet(basic_speed, -basic_speed, 0);    
     
      
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected 
          s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
          s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
            break;}      
    }//while 
    
    
    
    //left_count_start = encoder_vals[0];   // left encoder count at start
    while(fabs(encoder_vals[0] - left_count_start - turn_distance) < straight_distance){ // drive a random distance until light found  
      print("%f, %d, hunting light straight\n", fabs(encoder_vals[0] - left_count_start), straight_distance);  
    
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
