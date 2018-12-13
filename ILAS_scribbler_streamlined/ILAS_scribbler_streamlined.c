// Libraries
#include "simpletools.h"
#include "s3.h" 
#include "scribbler.h"
#include "math.h"


// Variables
#define pi 3.14159265358979323846


static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

// Functions
void encoder_update(void);
void GoToGoal(float dxI, float dyI, float theta, int basic_speed);
//void GoToGoal_New(float dxI, float dyI, float theta, int basic_speed);
float angle2pi(float x, float y);
float sgn(float v);

/*

float DistanceTest(int trig, int echo);
float DistanceSense_test(int trig_pin, int echo_pin);
float DistanceSense(int trig_pin, int echo_pin);
*/
                     

int main()                                    // Main function
{
  // Startup code here.
  s3_setup();
 
  if (s3_resetButtonCount() == 1) {
     //while(1){
       encoder_update();
       //print("%f, %f", encoder_vals[0], encoder_vals[1]);
       //s3_motorSet(100, 100, 0);
       //GoToGoal(200, 100, pi/4, 30); 
       /*
       GoToGoal(1, 1, pi/4, 30); 
       GoToGoal(-1, 1, pi/4, 30); 
       GoToGoal(-1, -1, pi/4, 30); 
       GoToGoal(1, -1, pi/4, 30); 
       */

       GoToGoal(200, 100, pi/4, 30); 
       //GoToGoal(-200, 100, pi/4, 30); 
       //GoToGoal(-200, -100, pi/4, 30); 
       //GoToGoal(200, -100, pi/4, 30); 
       
       //GoToGoal(200, 100, pi/4, 30);
       //GoToGoal(-200, -100, pi/4, 30);
       //GoToGoal(-200, 100, pi/4, 30);
       //GoToGoal_New(200, -100, pi/4, 30);
     //}         
   }  
    
    
  if (s3_resetButtonCount() == 2) { 
    while(1){
      encoder_update();
      s3_motorSet(30, -30, 0);
      //print("%f \n", encoder_vals[1]);
    }            
   }     
    
  if (s3_resetButtonCount() == 3) {
    s3_motorSet(70, 70, 0);
    while(1){
       encoder_update();
       int time = CNT/st_usTicks;
       //print("%d, %f, %f \n", time, encoder_vals[0], encoder_vals[1]);
     }       
     
   } 
        
        
  if (s3_resetButtonCount() == 4) {
    float ang = angle2pi(1, 1);
    //print("%f\n", ang);
    //print("%f\n", ang);
    ang = angle2pi(-1, 1);
    //print("%f\n", ang);
    //print("%f\n", ang);
    ang = angle2pi(-1, -1);
    //print("%f\n", ang);
    //print("%f\n", ang);
    ang = angle2pi(1, -1);
    //print("%f\n", ang);
    //print("%f\n", ang);

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
    
    //int32_t e0 = (value >> 24);           
    //encoder_vals[7] = e0 * 0.25;  //new 1 
    encoder_vals[7] = (value >> 24) * 0.25;   
              
    
    //int32_t e1 = (value >> 16) & 0xff;           
    //encoder_vals[8] = e1 * 0.25;  //new 2  
    encoder_vals[8] = ((value >> 16) & 0xff) * 0.25;  //new 2 
      
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
    
    //int32_t e2 = (value >> 8)  & 0xff;           
    encoder_vals[2] = (value >> 8)  & 0xff;
    
    //int32_t e3 = value & 0xfc;           
    encoder_vals[3] = value & 0xfc;
    
    //int32_t e4 = value & 0x3;           
    encoder_vals[4] = value & 0x3;               

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






/*
void GoToGoal(float dxI, float dyI, float theta, int basic_speed){
  
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
  
  //print("dxR, %f\n", dxR);
  //print("dyR %f\n", dyR);
  
  // convert local x,y to polar coordinates
  //float aR = atan( dyR / dxR );                         
  float aR = angle2pi(dxR, dyR);                             // angle
  float dR = powf((powf(dxR,2) + powf(dyR,2)), 0.5);      // distance
  //print("aR");
  
  
  // calculate length of arc to travel by angle aR
  float lenR = aR * L / 2;
  

  // distance and angle elapsed in local frame of reference
  float aR_ = 0.0;
  float dR_ = 0.0;
 
  encoder_update();
  float left_count_start = encoder_vals[0]; 
  
  while(turn_flag == 0 || straight_flag == 0){
    
  //print("left encoder %f, start count %f, lenR %f, dR %f \n", 
  //encoder_vals[0], left_count_start, lenR, dR);
    
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
*/

void GoToGoal(float dxI, float dyI, float theta, int basic_speed){
  
  // this is a test
  
  // flags to show which part of path is complete
  int turn_flag = 0;
  int straight_flag = 0;
  
  // distance between robot wheels (mm)
  int L = 145;
  
  
  // convert goal global --> local
  float dxR = dxI * cos(theta) + dyI * sin(theta);
  float dyR = dxI * -sin(theta)+ dyI * cos(theta); 
  
  
  
  // convert local x,y to polar coordinates
  //float aR = atan( dyR / dxR );                         
  float aR = angle2pi(dxR, dyR);                             // angle
  float dR = powf((powf(dxR,2) + powf(dyR,2)), 0.5);         // distance
  
  /*
  print("dxR, %f\n", dxR);
  print("dyR %f\n", dyR);
  print("aR %f\n", aR);
  print("\n");
  */

  //print("aR %f\n", aR);
  
  // calculate length of arc to travel by angle aR
  float lenR = aR * L / 2;
  
  
  // distance and angle elapsed in local frame of reference
  //float aR_ = 0.0;
  //float dR_ = 0.0;
 
  encoder_update();
  float left_count_start = encoder_vals[0]; 
  
  while(turn_flag == 0 || straight_flag == 0){
    
    // test code
    //s3_motorSet( basic_speed, -basic_speed, 0);  // remove
    //encoder_update();
    print("left encoder %f, start count %f, lenR %f, dR %f \n",   encoder_vals[0], left_count_start, lenR, dR);
        //if(fabs(encoder_vals[0] - left_count_start) >= fabs(lenR)){
          //left_count_start = encoder_vals[0];}

          

    
    // angle not reached and distance not reached
    if(turn_flag == 0){
        
        /*
        // negative angle
        if(aR < 0){
          s3_motorSet( basic_speed, -basic_speed, 0);} 
        
        // positive angle    
        else{
          s3_motorSet( -basic_speed, basic_speed, 0);} 
          */
                  
        ///pause(50);
        
        s3_motorSet( -basic_speed, basic_speed, 0);// remove 
        
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




float angle2pi(float x, float y){
    // Finds angle in range 2pi
    
   // pi - (pi/2) * (1 + sgn(x)) 
   
       /*
        float angle = (
        pi - (pi/2) * (1 + sgn(x)) * (1 - sgn(powf(y, 2))) - 
        (pi/4) * (2 + sgn(x)) * sgn(y) - 
        sgn(x * y) * 
        asin( (fabs(x) - fabs(y)) / powf((2 * powf(x, 2) + 2 * powf(y, 2)), 0.5
        );
        */

        float angle = (
        pi - (pi/2) * (1 + sgn(x)) * (1 - sgn(powf(y, 2))) - 
        (pi/4) * (2 + sgn(x)) * sgn(y)- 
        sgn(x * y) * 
        asin( (fabs(x) - fabs(y)) / powf((2 * powf(x, 2) + 2 * powf(y, 2)), 0.5) )
        );
        
        //print("%f", angle);
        
        return angle;
        
}        
  
    
    
float sgn(float v) {
  if (v < 0){ return -1; }
  if (v > 0){ return 1; }
  return 0;
}