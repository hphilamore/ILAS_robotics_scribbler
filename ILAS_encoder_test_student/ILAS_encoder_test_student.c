#include "simpletools.h" // Include simpletools
#include "s3.h" 
#include "scribbler.h"
#include "math.h"

// functions
void encoder_update_cog(void); 
void GoToGoal(float dxI, float dyI, float theta, int basic_speed);

// variables

static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0}; 
unsigned int stack[40 + 25];    // Stack var for other cog

#define pi 3.14159265358979323846

int main() // main function
{
s3_setup();

cogstart(encoder_update_cog, NULL, stack, sizeof(stack));

if (s3_resetButtonCount() == 1) {
	GoToGoal(200, -100, pi/4, 50); 
    }
}


void GoToGoal(float dxI, float dyI, float theta, int basic_speed){

int turn_flag = 0;
int straight_flag = 0;

// distance between robot wheels (mm)
int L = 145;

// convert goal global --> local
float dxR = dxI * cos(theta) + dyI * sin(theta);
float dyR = dxI * -sin(theta)+ dyI * cos(theta); 

// plan path
float aR = atan2(dyR, dxR);                         // angle
float dR = powf((powf(dxR,2) + powf(dyR,2)), 0.5);  // distance
float lenR = aR * L / 2;                            // arc length  

float left_count_start = encoder_vals[0];
 
while(1){
if(turn_flag == 0){   // turn to angle

if(aR < 0){      // negative angle
	s3_motorSet( basic_speed, -basic_speed, 0);} 

else{            // positive angle
	s3_motorSet( -basic_speed, basic_speed, 0);} 

if(fabs(encoder_vals[0] - left_count_start) >= fabs(lenR)){ // angle reached
turn_flag = 1;                       // put the flag up
left_count_start = encoder_vals[0];}  // reset the count
} // if 

else if(straight_flag == 0){    // drive to goal

s3_motorSet( basic_speed, basic_speed, 0);
if(fabs(encoder_vals[0] - left_count_start) >= fabs(dR)){ // goal reached
// put the flag up
straight_flag = 1;
//print("reached!");
s3_simpleStop(); 
} 
} // else if


else if(straight_flag == 1){break;} 

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
