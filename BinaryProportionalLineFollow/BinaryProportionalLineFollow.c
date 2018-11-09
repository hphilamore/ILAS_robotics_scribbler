#include "simpletools.h" 
#include "s3.h" 
#include "scribbler.h"

static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

void encoder_update(void);

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
 




                

int main()                                    
{
  s3_setup();
  
    

    if (s3_resetButtonCount() == 1) {
      // Calibrate IR line sensors for WHITE
      // Turn on spot  
      
      encoder_update();                          // get new encoder values
      float left_count_start = encoder_vals[0];  // left encoder count at start 
      
      int left = 0;                              // variables for IR sensor values      
      int right = 0;
      int count = 0;
      
      int bot_diameter = 145;                    // distance between two drive wheels
      
      // turn until full circle reached
      while(fabs(encoder_vals[0] - left_count_start) < bot_diameter * 3.142){     
        
        s3_motorSet(50, -50, 0);                 // turn on spot
        left += s3_lineSensor(S3_LEFT);          // cumulative sum left IR sensor
        right += s3_lineSensor(S3_RIGHT);        // cumulative sum right IR sensor
        count += 1;                              // cumulative sum while loops
        encoder_update();                        // get new encoder values
      }
      s3_motorSet(0, 0, 0);                  // stop moving         
      
      int leftave_white = left/count;        // average left IR sensor
      int rightave_white = right/count;      // average right IR sensor
      
      s3_memoryWrite(1, leftave_white);      // store in non-volatile memory 
      s3_memoryWrite(2, rightave_white);

      
    }  
    
    
    if (s3_resetButtonCount() == 2) {
      // Calibrate IR line sensors for BLACK
      // Drive 5cm in straight line 
      
      encoder_update();                          // get new encoder values
      float left_count_start = encoder_vals[0];  // left encoder count at start 
      
      int left = 0;                              // variables for IR sensor values      
      int right = 0;
      int count = 0;
      
      int bot_diameter = 145;                    // distance between two drive wheels
      
      // turn until 5cm linear travel reached
      while(fabs(encoder_vals[0] - left_count_start) < 50){     
        
        s3_motorSet(50, 50, 0);                 // turn on spot
        left += s3_lineSensor(S3_LEFT);         // cumulative sum left IR sensor
        right += s3_lineSensor(S3_RIGHT);       // cumulative sum right IR sensor
        count += 1;                             // cumulative sum while loops
        encoder_update();                       // get new encoder values
      }
      s3_motorSet(0, 0, 0);                  // stop moving         
      
      int leftave_black = left/count;        // average left IR sensor
      int rightave_black = right/count;      // average right IR sensor
      
      s3_memoryWrite(3, leftave_black);      // store in non-volatile memory 
      s3_memoryWrite(4, rightave_black); 
      
      
    }     
    
    if (s3_resetButtonCount() == 3) {
      // line following : binary controller
      
      int leftave_white = s3_memoryRead(1);                      // stored average IR sensor calibration values
      int rightave_white = s3_memoryRead(2);
      int leftave_black = s3_memoryRead(3);
      int rightave_black = s3_memoryRead(4);
      
      int mid_left = (leftave_white - leftave_black) / 2;        // midpoint between black and white
      int mid_right = (rightave_white - rightave_black) / 2;
      
      int v_basic = 30;                                          // basic speed
      
      while(1){
      
      if(s3_lineSensor(S3_LEFT) > mid_left && s3_lineSensor(S3_RIGHT) < mid_right){       // left=white, right=black --> turn right
        s3_motorSet(40, -20, 0);
        pause(200);}
      
      else if(s3_lineSensor(S3_RIGHT) > mid_right && s3_lineSensor(S3_LEFT) < mid_left){  // left=black, right=white --> turn left
        s3_motorSet(-20, 40, 0);
        pause(200);}
       
      else{                              // left = right = white/black --> go straight
        s3_motorSet(v_basic, v_basic, 0);     
        pause(200);}
      
      } 

      
      
    } 
        
        
     if (s3_resetButtonCount() == 4) {
      // line following : binary controller
      
      int leftave_white = s3_memoryRead(1);                      // stored average IR sensor calibration values
      int rightave_white = s3_memoryRead(2);
      int leftave_black = s3_memoryRead(3);
      int rightave_black = s3_memoryRead(4);
      
      int mid_left = (leftave_white - leftave_black) / 2;        // midpoint between black and white
      int mid_right = (rightave_white - rightave_black) / 2;
      
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
   
          

}

