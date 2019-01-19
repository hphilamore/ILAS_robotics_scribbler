/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h" 
#include "s3.h" 
#include "scribbler.h"
#include "servo.h"
#include "ping.h"
#include "math.h"

static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

void encoder_update(void);
void StallAvoid(int basic_speed);
void IR_ObstacleAvoid(int basic_speed);
void SimpleLightFollow(int basic_speed);
void DigitalLightFollow(int basic_speed);



int main()                                    // Main function
{
  s3_setup();

 
  while(1)
  {
    IR_ObstacleAvoid(50);
    StallAvoid(50);
    DigitalLightFollow(50);
    
  }  
}



void SimpleLightFollow(int basic_speed){
  // Robot follows the brightest light in its path
  // If brightest not detected, goes straight
  
  int low_speed = basic_speed * 0.7;
  int high_speed = basic_speed / 0.7;
  
  if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){
    while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
      s3_motorSet(low_speed, high_speed, 0);  
      if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | !s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){ 
        break;}                
    } //while
   } // if      
  
  else if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){
    while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
      s3_motorSet(high_speed, low_speed, 0);
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) | !s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){ 
        break;}
    } //while
  } // if
  
  else{
    s3_motorSet(basic_speed, basic_speed, 0);}   
}




void DigitalLightFollow(int basic_speed){
  // Robot follows the brightest light in its path
  // If brightest not detected, turns on spot
  
  int bot_diameter = 145;                      // distance between two drive wheels 
  
  if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |  // brightest detected
      s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
      s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
    print("Light Detected \n");
    SimpleLightFollow(basic_speed);
  } 
     
  
  else{   //   brightest not detected
    print("Light NotDetected \n");
    encoder_update();                          // get new encoder values
    float left_count_start = encoder_vals[0];  // left encoder count at start 
    
    // turn on spot until full circle reache  d
      while(fabs(encoder_vals[0] - left_count_start) < bot_diameter * 3.142){     
        
        s3_motorSet(basic_speed, -basic_speed, 0);    
        
        encoder_update();                             // get new encoder values
        
        if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected 
            s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
            s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
              break;}      
    }
    
    // go straight
    while(1){
      s3_motorSet(basic_speed, basic_speed, 0);
      
      IR_ObstacleAvoid(basic_speed); 
      StallAvoid(basic_speed);
                        
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected    
          s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
          s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
            break;}     
    } 
  } //else 
                
} //function


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
    //s3_motorSet(-basic_speed, -basic_speed, 1500);   // move backwards
    //s3_simpleSpin(90, basic_speed, 0);               // quarter clockwise turn
  //}
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
      s3_motorSet(-10, 70, 0);  // turn right
    }
    else if(right && !left){    // obstacle to right
      s3_motorSet(70, -10, 0);  // turn left
    } 
    else{                       // obstacle directly in front
      s3_simpleSpin(60, 50, 0); // clockwise turn
    } 
  }
  else{
    print("Obstacle Not Detected \n");
    s3_motorSet(basic_speed, basic_speed, 0);
  }                
}


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