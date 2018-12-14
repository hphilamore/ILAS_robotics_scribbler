#include "simpletools.h" // Include simpletools
#include "s3.h" 
#include "scribbler.h"
#include "math.h"

// functions
void encoder_update_cog(void); 
void GoToGoalOr(float dxI, float dyI, float th_i, float th_f, int basic_speed);
float sgn(float v);
void encoder_counter(void);
void wheel_motors(int left_speed, int right_speed, int duration);
void IR_ObstacleAvoid(int basic_speed);

// constants
#define pi 3.14159265358979323846
#define L 145 // Length between robot wheel centres

// variables
static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; 
static float motor_speed[4] = {0, 0, 0, 0}; 
static float encoder_count[2] = {0, 0}; 
//unsigned int stack[40 + 25];            // Stack var for other cog
//int L = 145;                            // distance between robot wheels (mm)


int main()                                    // Main function
{
  s3_setup();
  cog_run(encoder_update_cog, 40);
 
  if (s3_resetButtonCount() == 1) {
    GoToGoal_O(200, 800, 0, -pi, 50); 
  }
}



void GoToGoalOr(float dxI, float dyI, float th_i, float th_f, int basic_speed){
  
  // Drives the robot by turning on the spot then driving in a straight line.  
  // dxI : x distance to goal, global frame
  // dyI : y distance to goal, global frame
  // th_f : final angle of orientation, global frame
  // th_i : initial angle of orientation, global frame 

  int turn_flag = 0;
  int straight_flag = 0;
  int orientation_flag = 0;
  
  // convert goal global --> local
  float dxR = dxI * cos(th_i) + dyI * sin(th_i);
  float dyR = dxI * -sin(th_i)+ dyI * cos(th_i); 
  
  // plan path
  float a1 = atan2(dyR, dxR);                           // angle
  float len_a1 = a1 * L / 2;                             // convert angle to arc of wheel travel
  
  float d = powf((powf(dxR,2) + powf(dyR,2)), 0.5);    // distance
  
  float a2 = th_f - th_i - a1;                                // angle of orientation 
  float len_a2 = a2 * L / 2;                             // convert angle to arc of wheel travel
  
  
  float left_count_start = encoder_vals[0];
   
  while(1){
    if(turn_flag == 0){   // turn to angle
    s3_motorSet(-sgn(a1)*basic_speed, sgn(a1)*basic_speed, 0);
    if(fabs(encoder_vals[0] - left_count_start) >= fabs(len_a1)){ // angle reached
      turn_flag = 1;                                              // put the flag up
      left_count_start = encoder_vals[0];}                        // reset the count
    } // if 
    
    
    else if(straight_flag == 0){  // drive to goal                             
      s3_motorSet( basic_speed, basic_speed, 0);               
      if(fabs(encoder_vals[0] - left_count_start) >= fabs(d)){ // goal reached
        straight_flag = 1;                                     // put the flag up
        left_count_start = encoder_vals[0];
      } 
    } // else if
    
    
    
    else if(orientation_flag == 0){  // turn to orientation                               
      s3_motorSet(-sgn(a2)*basic_speed, sgn(a2)*basic_speed, 0); 
      if(fabs(encoder_vals[0] - left_count_start) >= fabs(len_a2)){ // orientation reached
        orientation_flag = 1;                                       // put the flag up
      } 
    } // else if
    
    
    else if(orientation_flag == 1){
      s3_motorSet(0, 0, 0); 
      break;} 
  
  }// while 
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


float sgn(float v) {
  if      (v < 0){ return -1; }
  else if (v > 0){ return 1; }
  else           { return 0; }
  return 0;
}
