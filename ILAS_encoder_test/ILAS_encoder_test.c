

#include "simpletools.h"                      // Include simpletools
#include "s3.h" 
#include "scribbler.h"
#include "math.h"

//void adder(void *par);                        // Forward declaration
void encoder_update(void);                        // Forward declaration
void GoToGoal(float dxI, float dyI, float theta, int basic_speed);
float angle2pi(float x, float y);
float sgn(float v);

static volatile int t, n;                     // Global vars for cogs to share
static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder dat
unsigned int stack[40 + 25];                  // Stack vars for other cog
#define pi 3.14159265358979323846


int main()                                    // main function
{
  s3_setup();
  //t = 50;                                     // Set values of t & n
  //n = 5000;
  
  cogstart(encoder_update, NULL, stack, sizeof(stack));
  
  if (s3_resetButtonCount() == 1) {
  // Launch encoder_update function into another cog (processor).
  //cogstart(encoder_update, NULL, stack, sizeof(stack));
  //cog_run(encoder_update, 65);

  // Watch what the other cog is doing to the value of n.
  //while(1)
  //{
    //print("n = %d\n", n);                     // Display result
    //pause(100); 
                                  // Wait 1/10 of a second 
    //s3_motorSet( 50, -50, 0);
    //print("%f \n", encoder_vals[0]); 
    //GoToGoal();
    
    
    //GoToGoal(200, 100, pi/4, 50); 
    //GoToGoal(200, -100, pi/4, 50);   
    GoToGoal(-200, 100, pi/4, 50);   
    //GoToGoal(-200, -100, pi/4, 50); 
    
        
    //cog_run(encoder_update, 128);                  // Run blink in other cog
  //} 
  }
  
  /*
  if (s3_resetButtonCount() == 2) {
    
    //cogstart(encoder_update, NULL, stack, sizeof(stack));
    
    while(1){
      s3_motorSet( 50, 50, 0);   
      int32_t value = scribbler_motion();
      print("%d  \t", (value >> 24)); 
      //print("%f  \n", encoder_vals[0]);  
   }     
  
  } 
  */
  
  /*
    
  if (s3_resetButtonCount() == 3) {
    s3_motorSet( 50, 50, 0);  
    cogstart(encoder_update, NULL, stack, sizeof(stack));
    
    print("%f  \n", encoder_vals[0]);  
    while(1){
      //s3_motorSet( 50, 50, 0);   
      int32_t value = scribbler_motion();
      print("%d  \t", (value >> 24)); 
      //print("%f  \n", encoder_vals[0]); 
      print("%f  \n", encoder_vals[0]);   
   }     
  
  } 
  */  


     
}


void GoToGoal(float dxI, float dyI, float theta, int basic_speed){
  int turn_flag = 0;
  int straight_flag = 0;
  
  // distance between robot wheels (mm)
  int L = 145;
  
  // convert goal global --> local
  float dxR = dxI * cos(theta) + dyI * sin(theta);
  float dyR = dxI * -sin(theta)+ dyI * cos(theta); 

  // convert local x,y to polar coordinates                        
  float aR = angle2pi(dxR, dyR);                             // angle
  float dR = powf((powf(dxR,2) + powf(dyR,2)), 0.5);         // distance
  //print("angle %f\n", aR);
  //print("%f\n", dR);
  
  
  
  float lenR = aR * L / 2;
  
  float left_count_start = encoder_vals[0]; 
  
  
  
  while(1){
  
  if(turn_flag == 0 || straight_flag == 0){
    
    //print("%f \t", encoder_vals[0]); 
    //print("left encoder %f, start count %f, lenR %f, dR %f \n",   encoder_vals[0], left_count_start, lenR, dR);
    //print("start count %f \t", left_count_start);
    //print("lenR %f \n", lenR);
    //print("dR %f \n", dR);
      
     if(turn_flag == 0){
      //s3_motorSet( -basic_speed, basic_speed, 0);// remove  
      
      
      // negative angle
      if(aR < 0){
        s3_motorSet( basic_speed, -basic_speed, 0);} 
      
      // positive angle    
      else{
        s3_motorSet( -basic_speed, basic_speed, 0);} 
      
      
          
      if(fabs(encoder_vals[0] - left_count_start) >= fabs(lenR)){
        //print("reached\n");
        // put the flag up
        turn_flag = 1;
        // reset the count
        left_count_start = encoder_vals[0];
        //s3_motorSet( 0, 0, 0);
       } //if
     } // if         
     
     
     else if(straight_flag == 0){

     s3_motorSet( basic_speed, basic_speed, 0);
     if(fabs(encoder_vals[0] - left_count_start) >= fabs(dR)){
          // put the flag up
          straight_flag = 1;
          //print("reached!");
          s3_simpleStop();
          //s3_motorSet( 0, 0, 0);  
        }                  
             
     } // else if
      
   } // if (turn_flag == 0 || straight_flag == 0)
  
      
  }// while  
  
  
}     
  
  



/*
void encoder_update(void) { 
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

}

*/

/*
void encoder_update(void) { // working version from 18_12_08
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
    
    //print("%d  \t", (value >> 24)); 
    //print("%f  \n", encoder_vals[0]);  
  }                 

}
*/


void encoder_update(void) { 
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
    
    //int32_t e1 = (value >> 16) & 0xff;           
    //encoder_vals[8] = e1 * 0.25;  //new 2 
    encoder_vals[8] = ((value >> 16) & 0xff) * 0.25;  //new 2  
      
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
    
    //print("%d  \t", (value >> 24)); 
    //print("%f  \n", encoder_vals[0]);  
  }                 

}



float angle2pi(float x, float y){
    // Finds angle in range 2pi

        /*
        float angle = (
        pi - (pi/2) * (1 + sgn(x)) * (1 - sgn(powf(y, 2))) - 
        (pi/4) * (2 + sgn(x)) * sgn(y)- 
        sgn(x * y) * 
        asin( (fabs(x) - fabs(y)) / powf((2 * powf(x, 2) + 2 * powf(y, 2)), 0.5) )
        );
        */
        
        
        float angle = atan2(y, x);
        
        //print("%f", angle);
        
        return angle;
        
}        
  
    
    
float sgn(float v) {
  if (v < 0){ return -1; }
  if (v > 0){ return 1; }
  return 0;
}