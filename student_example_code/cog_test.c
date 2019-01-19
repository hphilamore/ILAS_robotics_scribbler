/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "s3.h" 
#include "scribbler.h"
#include "servo.h"
#include "ping.h"


#define servo_pin 0

volatile float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

volatile int dt;                              // Declare dt for both cogs
volatile int angle;                              // Declare dt for both cogs

void blink1(void);
void blink2(void);
void blink3(void);


void servo_sweep(void);
void encoder_update_cog(void);
void dist_sense(void);
void angle_dist(void);


int main()                                    // Main function
{
  s3_setup();
  s3_motorSet(50, 50, 0);
  
  //cog_run(blink1, 128);
  //cog_run(angle_dist, 20);
  cog_run(blink2, 128);
  //cog_run(blink3, 128);
  
  //cog_run(encoder_update_cog, 20);
  
  servo_angle(0, 900); 
  
  while(1){
    
    servo_angle(servo_pin, 10 * 180); 
    
    print("%d\n", dt);
    print("%f\n", encoder_vals[0]);
    print("%d\n", angle);
    print("\n");
    
    /*
    int dist = ping_cm(1);
    print("%d\n", dist);
    //simpleterm_close(); 
    print("%f\n", encoder_vals[0]);
    print("\n");
    //simpleterm_close(); 
    */
  }    
  
  
  
  
  
  //cogstart(encoder_update_cog, NULL, stack1, sizeof(stack1));
  //cogstart(servo_sweep, NULL, stack2, sizeof(stack2));
   

    
}

void blink1(void) {
  
  while(1){
    
    //print("%f", encoder_vals[0]);
    
    s3_setLED(S3_LEFT, S3_COLOR_FF0000);
    pause(500);
    s3_setLED(S3_LEFT, S3_COLOR_000000);
    pause(500);
    
    
  }    
}

void blink2(void) {
  
  while(1){
  
    s3_setLED(S3_RIGHT, S3_COLOR_00FF00 );
    pause(1000);
    s3_setLED(S3_RIGHT, S3_COLOR_000000);
    pause(1000);
    
  }    
}


void blink3(void) {
  
  while(1){
  
    s3_setLED(S3_CENTER, S3_COLOR_00FF00 );
    pause(200);
    s3_setLED(S3_CENTER, S3_COLOR_000000);
    pause(200);
    
  }    
}



/*
void servo_sweep(void) {
  
  //#include "servo.h"
  
  while(1){
    
    servo_angle(0, 10 * 180);
    
    
    for (int pos = 0; pos <= 180; pos += 10) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      servo_angle(servo_pin, 10 * pos);   // tell servo to go to position in variable 'pos'
      pause(100);                          // waits 15ms for the servo to reach the position
    }
    
    
    
    for (int pos = 180; pos >= 0; pos -= 10) { // goes from 180 degrees to 0 degrees
      servo_angle(servo_pin, 10 * pos);   // tell servo to go to position in variable 'pos'
      pause(100);                          // waits 15ms for the servo to reach the position
    }
  
  }  
}
*/


void dist_sense(void){
  while(1){
    dt = ping_cm(1);
    
    //simpleterm_close(); 
  }  
} 


void angle_dist(void){
  while(1){
    for (int pos = 0; pos <= 180; pos += 10) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      angle = pos;
      dt = ping_cm(2);
      pause(75);                          // waits 15ms for the servo to reach the position
    }
    
    
    
    for (int pos = 180; pos >= 0; pos -= 10) { // goes from 180 degrees to 0 degrees
      angle = pos;
      dt = ping_cm(2);
      pause(75);                          // waits 15ms for the servo to reach the position
    } 
  }    
    
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
    
    servo_angle(0, 10 * 90);  

    
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
