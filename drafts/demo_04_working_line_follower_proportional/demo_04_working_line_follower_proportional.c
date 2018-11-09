#include "simpletools.h" 
#include "s3.h" 
#include "scribbler.h"

static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data
int leftave_white; 
int rightave_white;
int leftave_black;
int rightave_black; 
    

                

int main()                                    
{
  s3_setup();
  
    

      if (s3_resetButtonCount() == 1) {
      // turn on spot to calibrate white
      encoder_update();
      float left_count_start = encoder_vals[0];
      print("left count start , %f", left_count_start);
      
      int left = 0;
      int right = 0;
      int count = 0;
      encoder_update();
      int bot_diameter = 145;
      while(fabs(encoder_vals[0] - left_count_start) < bot_diameter * 3.142){
        s3_motorSet(-50, 50, 0);
        print("%f\n", encoder_vals[0] - left_count_start);
        print("%f\n", fabs(encoder_vals[0] - left_count_start));
        
        left += s3_lineSensor(S3_LEFT);
        right += s3_lineSensor(S3_RIGHT);
        count += 1;
        
        print("count, %d\n", count);
        print("left, %d\n", left); 
        print("right, %d\n", right);
        print("\n");
        
        encoder_update();      
      }
      
      s3_motorSet(0, 0, 0); 
      leftave_white = left/count;
      rightave_white = right/count;
      s3_memoryWrite(1, leftave_white);
      s3_memoryWrite(2, rightave_white);

      print("leftave_white, %d\n", leftave_white); 
      print("rightave_white, %d\n", rightave_white);
      
    } 
    
    if (s3_resetButtonCount() == 2) {
      // drive 5cm to calibrate black
      encoder_update();
      float left_count_start = encoder_vals[0];
      print("left count start , %f", left_count_start);
      
      int left = 0;
      int right = 0;
      int count = 0;
      encoder_update();
      while(fabs(encoder_vals[0] - left_count_start) < 50){
        s3_motorSet(50, 50, 0);
        print("%f\n", encoder_vals[0] - left_count_start);
        print("%f\n", fabs(encoder_vals[0] - left_count_start));
        
        left += s3_lineSensor(S3_LEFT);
        right += s3_lineSensor(S3_RIGHT);
        count += 1;
        
        print("count, %d\n", count);
        print("left, %d\n", left); 
        print("right, %d\n", right);
        print("\n");
        
        encoder_update();      
      }
      
      s3_motorSet(0, 0, 0); 
      leftave_black = left/count;
      rightave_black = right/count;
      print("leftave_black, %d\n", leftave_black); 
      print("rightave_black, %d\n", rightave_black);
      s3_memoryWrite(3, leftave_black);
      s3_memoryWrite(4, rightave_black);
      
    }     
    
    if (s3_resetButtonCount() == 3) {

      print("leftave_black, %d\n", s3_memoryRead(3)); 
      print("rightave_black, %d\n", s3_memoryRead(4));
      print("leftave_white, %d\n", s3_memoryRead(1)); 
      print("rightave_white, %d\n", s3_memoryRead(2));
      
      leftave_white = s3_memoryRead(1);
      rightave_white = s3_memoryRead(2);
      leftave_black = s3_memoryRead(3);
      rightave_black = s3_memoryRead(4);
      
      int mid_left = (leftave_white - leftave_black) * 2 / 3;
      int mid_right = (rightave_white - rightave_black) * 2 / 3;
      
      int leftIR;
      int rightIR;
      
      while(1){
        
        if(s3_lineSensor(S3_LEFT) > mid_left){
          leftIR = 1;
        }          
        else{
          leftIR = 0;
        }
        
        if(s3_lineSensor(S3_RIGHT) > mid_right){
          rightIR = 1;
        }          
        else{
          rightIR = 0;
        } 
        /*
        print("%d\t", mid_left);
        print("%d\t", mid_right);
        
        print("%d\t", s3_lineSensor(S3_LEFT));
        print("%d\t", s3_lineSensor(S3_RIGHT));
        
        print("%d\t", leftIR);
        print("%d\n", rightIR);
        */
        
        int v_basic = 30;
        
        if(leftIR < rightIR){
          /*
          s3_motorSet(-20, 40, 0);
          pause(200);
          */
          
          
          int l_error = leftave_white - s3_lineSensor(S3_LEFT);
          int v_r = v_basic + (100 - v_basic) * l_error / (leftave_white - leftave_black);
          s3_motorSet(-v_r/2, v_r, 0);
          print("v_r = %d\n", v_r);
          pause(200);
          
          
        }
        
        else if(rightIR < leftIR){
          /*
          s3_motorSet(40, -20, 0);
          pause(200);
          */
          
          
          int r_error = rightave_white - s3_lineSensor(S3_RIGHT);
          int v_l = v_basic + (100 - v_basic) * r_error / (rightave_white - rightave_black);
          s3_motorSet(v_l, -v_l/2, 0);
          print("v_l = %d\n", v_l);
          pause(200);
          
        }      
          
        else{
          /*
          s3_motorSet(30, 30, 0);
          pause(200); 
          */ 
          
          s3_motorSet(v_basic, v_basic, 0);
          pause(200); 
        }       
                     
          }        
          
        }   
   
          

}


void encoder_update() {   
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


int stall_sensor(void){
//   Compares the idler wheel velocity to the wheel encoders to check if the robot is stuck
  int stall;
  if(encoder_vals[4] && !encoder_vals[3]){
    stall = 1;
  }    
  else{
    stall = 0;
  } 
  return stall;
}     
  

