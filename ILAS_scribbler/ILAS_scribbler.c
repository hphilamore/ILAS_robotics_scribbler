// Libraries
#include "simpletools.h"
#include "s3.h" 
#include "scribbler.h"
#include "servo.h"
#include "ping.h"
#include "math.h"


// Variables
#define pi 3.14159265358979323846

int PID_time_old; 
float PID_err_old;
float P, I, D;
float Feedback, Setpoint;
float kp, ki, kd;
static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

// Functions
void encoder_update(void);
//void GoToGoal(float dxI, float dyI, float theta, int basic_speed);
void GoToGoal_New(float dxI, float dyI, float theta, int basic_speed);
/*

float DistanceTest(int trig, int echo);
float DistanceSense_test(int trig_pin, int echo_pin);
float DistanceSense(int trig_pin, int echo_pin);
*/

float test_vals[9];
                     

int main()                                    // Main function
{
  // Startup code here.
  s3_setup();
  PID_time_old = CNT/st_usTicks;
  test_vals[0] = 1;

 
  if (s3_resetButtonCount() == 1) {
     //while(1){
       encoder_update();
       //print("%f, %f", encoder_vals[0], encoder_vals[1]);
       //s3_motorSet(100, 100, 0);
       //GoToGoal_New(200, 100, pi/4, 30); 
       //GoToGoal_New(200, 100, pi/4, 30);
       GoToGoal_New(-200, -100, pi/4, 30);
       //GoToGoal_New(-200, 100, pi/4, 30);
       //GoToGoal_New(200, -100, pi/4, 30);
     //}         
   }  
    
    
  if (s3_resetButtonCount() == 2) { 
    while(1){
      encoder_update();
      s3_motorSet(30, -30, 0);
      print("%f \n", encoder_vals[1]);
    }            
   }     
    
  if (s3_resetButtonCount() == 3) {
    s3_motorSet(70, 70, 0);
    while(1){
       encoder_update();
       int time = CNT/st_usTicks;
       print("%d, %f, %f \n", time, encoder_vals[0], encoder_vals[1]);
     }       
     
   } 
        
        
  if (s3_resetButtonCount() == 4) {

   }      
    
  if (s3_resetButtonCount() == 5) {
                    
   }  
    
    
  if (s3_resetButtonCount() == 6) {
                    
   }  
    
  if (s3_resetButtonCount() == 7) {

   } 
    
  if (s3_resetButtonCount() == 8) {
             
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


/*
void GoToGoal_New(float dxI, float dyI, float theta, int basic_speed){
  
  // distance between robot wheels (mm)
  int L = 145;
  
  // flag to indicate presence of obstacle
  int flag = 0;
  
  // convert goal global --> local
  float dxR = dxI * cos(theta) + dyI * sin(theta);
  float dyR = dxI * -sin(theta)+ dyI * cos(theta); 
  
  // convert local x,y to polar coordinates
  float aR = atan( dyR / dxR );                           // angle
  float dR = powf((powf(dxR,2) + powf(dyR,2)), 0.5);      // distance
  
  //print("dxI = %f \t", dxI );
  //print("dyI = %f \t", dyI );

  
 
   
  
  // distance and angle elapsed in local frame of reference
  float aR_ = 0.0;
  float dR_ = 0.0;
  
  // encoder count at start
  float enLnew; 
  float enRnew; 
  encoder_update();
  float enLold = encoder_vals[0];
  float enRold = encoder_vals[1]; 
  
  while( fabs(aR_) < fabs(aR) || fabs  (dR_) < fabs(dR)){ // while goal not reached
    
    
    // angle not reached and distance not reached
    if(fabs(aR_) < fabs(aR) && fabs(dR_) < fabs(dR)){
      
        // negative angle
        if(aR < 0){
          s3_motorSet( basic_speed, -basic_speed, 0);} 
        
        // positive angle    
        else{
          s3_motorSet( -basic_speed, basic_speed, 0);} 
      } 
    
      // angle reached but distance not reached
      else if(fabs(aR_) >= fabs(aR) && fabs(dR_) < fabs(dR)){
        s3_motorSet( basic_speed, basic_speed, 0);} // else if
      
      
      // goal reached
      else{ s3_motorSet( 0, 0, 0);}
     
      
      
      
      // update distance elapsed 
      encoder_update();
      enLnew = encoder_vals[0];
      enRnew = encoder_vals[1];
      print("encoderNew = %f, %f \n", encoder_vals[0], encoder_vals[1]);
  

      print("DR = %f \n", enRnew - enRold);
      
  
      
      print("\n");
  
      
      
      aR_ += ((enLnew - enLold) - (enRnew - enRold)) / L;
      dR_ += ((enLnew - enLold) + (enRnew - enRold)) / 2;
      enLold = enLnew; 
      enRold = enRnew; 
    
    
  } // while    
    
} // function

*/ 







void GoToGoal_New(float dxI, float dyI, float theta, int basic_speed){
  
  // flags to show which part of path is complete
  int turn_flag = 0;
  int straight_flag = 0;
  
  // distance between robot wheels (mm)
  int L = 145;
  
  // flag to indicate presence of obstacle
  int flag = 0;
  
  // convert goal global --> local
  float dxR = dxI * cos(theta) + dyI * sin(theta);
  float dyR = dxI * -sin(theta)+ dyI * cos(theta); 
  
  // convert local x,y to polar coordinates
  float aR = atan( dyR / dxR );                           // angle
  float dR = powf((powf(dxR,2) + powf(dyR,2)), 0.5);      // distance
  
  
  // calculate length of arc to travel by angle aR
  float lenR = aR * L / 2;
  

  // distance and angle elapsed in local frame of reference
  float aR_ = 0.0;
  float dR_ = 0.0;
 
  encoder_update();
  float left_count_start = encoder_vals[0]; 
  
  while(turn_flag == 0 || straight_flag == 0){
    
  print("left encoder %f, start count %f, lenR %f, dR %f \n", 
  encoder_vals[0], left_count_start, lenR, dR);
    
    // angle not reached and distance not reached
    if(turn_flag == 0){
        
        // negative angle
        if(aR < 0){
          s3_motorSet( basic_speed, -basic_speed, 0);} 
        
        // positive angle    
        else{
          s3_motorSet( -basic_speed, basic_speed, 0);} 
                  
        ///pause(50); 
        
        encoder_update();
        if(fabs(encoder_vals[0] - left_count_start) >= fabs(lenR)){
          // put the flag up
          turn_flag = 1;
          // reset the count
          encoder_update();
          left_count_start = encoder_vals[0];}
          
     } // if
     
     
     
     
     // angle reached but distance not reached
     else if(straight_flag == 0){
  
        s3_motorSet( basic_speed, basic_speed, 0);
        //pause(50);
        
        encoder_update();
        if(fabs(encoder_vals[0] - left_count_start) >= fabs(dR)){
          // put the flag up
          straight_flag = 1;
          // stop the motors
          s3_motorSet( 0, 0, 0); }         
     } // else if
     

        
      
   } // while 
   
   // goal reached
                              
           
} // function
