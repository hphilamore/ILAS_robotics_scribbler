

#include "simpletools.h"                      // Include simpletools
#include "s3.h" 
#include "scribbler.h"
#include "math.h"

void adder(void *par);                        // Forward declaration
void encoder_update(void);                        // Forward declaration


static volatile int t, n;                     // Global vars for cogs to share
static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder dat
unsigned int stack[40 + 25];                  // Stack vars for other cog

int main()                                    // main function
{
  s3_setup();
  t = 50;                                     // Set values of t & n
  n = 5000;

  // Launch encoder_update function into another cog (processor).
  cogstart(encoder_update, NULL, stack, sizeof(stack));

  // Watch what the other cog is doing to the value of n.
  while(1)
  {
    //print("n = %d\n", n);                     // Display result
    //pause(100); 
                                  // Wait 1/10 of a second 
    s3_motorSet( 70, -70, 0);
    print("%f \n", encoder_vals[0]);   
    //cog_run(encoder_update, 128);                  // Run blink in other cog
  }    
}







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