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
      

                

int main()                                    
{
  s3_setup();
  
    
    while(1)
    {
      
      encoder_update();
      print("%f\t", encoder_vals[0]);
      print("%f\t", encoder_vals[1]);
      print("%f\t", encoder_vals[2]);
      print("%f\t", encoder_vals[3]);
      print("%f\t", encoder_vals[4]);
      print("%d\n", stall_sensor());
    }  


    /*
    // turn on the spot full circle
    encoder_update();
    float left_count_start = encoder_vals[0];
    int bot_diameter = 145
    while(fabs(encoder_vals[0] - left_count_start) < bot_diameter * 3.142){
      s3_motorSet(50, -50, 0);
      encoder_update();
      print("%f\n", encoder_vals[0] - left_count_start);
    }
    s3_motorSet(0, 0, 0); 
    */    

}

