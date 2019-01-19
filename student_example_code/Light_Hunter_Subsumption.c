/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "s3.h" 
#include "scribbler.h"

# define PI 3.14159265358979323846
# define bot_diameter 145
# define b_speed 50
# define white 0
# define black 1

int basic_speed = b_speed;
int low_speed = b_speed * 0.7;
int high_speed = b_speed / 0.7;
int left_speed;
int right_speed;
float hunt_left_count_start;
float line_left_count_start;
int SenseLightLeft;
int SenseLightRight;
int SenseLightFront;
int TurnComplete; 
int TurnFromLineComplete;

int32_t left_colour;
int32_t center_colour;
int32_t right_colour;


// Line follower variables
int leftave_white; 
int rightave_white; 
int leftave_black; 
int rightave_black; 
int mid_left; 
int mid_right;  

char* readout;

static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data


void encoder_update_cog(void);
void TurnToLight(int basic_speed);
void resetHuntLight(void);
void resetTurnFromLine(void);
void arbitrate(void);
void sense(void);
void Calibrate(int colour);



int main()                                    // Main function
{
  s3_setup();
  cog_run(encoder_update_cog, 65);
  resetHuntLight();
  
  if (s3_resetButtonCount() == 1) {
    // Calibrate IR line sensors for WHITE   
    Calibrate(white);      
  }  
  
  
  if (s3_resetButtonCount() == 2) {
    // Calibrate IR line sensors for BLACK 
    Calibrate(black);     
  }  


  if (s3_resetButtonCount() == 3) {
    
    // Line follower variables
    leftave_white = s3_memoryRead(1);                      // stored average IR sensor calibration values
    rightave_white = s3_memoryRead(2);
    leftave_black = s3_memoryRead(3);
    rightave_black = s3_memoryRead(4);
    mid_left = (leftave_white + leftave_black) / 5;        // midpoint between black and white
    mid_right = (rightave_white + rightave_black) / 5;  
    
   
    while(1)
    {
      sense();
      arbitrate();
      pause(200);
    }  
  }
  
   
     
}



void sense(void){
  // Represent all sensory inputs as boolean variable 
  
  SenseLightLeft = s3_simpleLight(S3_IS, SCRIBBLER_LEFT);
  SenseLightRight = s3_simpleLight(S3_IS, SCRIBBLER_RIGHT);
  SenseLightFront = s3_simpleLight(S3_IS, SCRIBBLER_CENTER);
  
  TurnComplete = fabs(encoder_vals[0] - hunt_left_count_start) > bot_diameter * PI;
}



void arbitrate()
{    
   
  left_speed = basic_speed;    
  right_speed = -basic_speed; 
  readout = "Hunting Light";  

  if( TurnComplete){
    readout = "hunt light reset";
    resetHuntLight(); 
  } 
  
  if( SenseLightLeft ){
    readout = "Light Left"; 
    left_speed = -basic_speed;
    right_speed = basic_speed;
    resetHuntLight();
  }   
  
  if( SenseLightRight ){
    readout = "Light Right"; 
    left_speed = basic_speed;
    right_speed = -basic_speed;
    resetHuntLight();    
  }  
    
  if( SenseLightFront ){
    readout = "Light Front"; 
    int light = s3_lightSensor(SCRIBBLER_CENTER);
    int motor_speed = mapInt(light, 0, 255, 100, 0);
    left_speed = motor_speed;
    right_speed = motor_speed;
    resetHuntLight(); 
  } 


  s3_motorSet(left_speed, right_speed, 0);   
  print("%s\n", readout); 
      

}

void resetHuntLight(void){
  // Resets the random walk distances
    hunt_left_count_start = encoder_vals[0];   // left encoder count at start 
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
  
  
void Calibrate(int colour){
    // Calibrate IR line sensors 
    // colour = "w" (white) or "b" (black) 
    
    int left = 0;                              // variables for IR sensor values      
    int right = 0;
    int count = 0;
    
    //int bot_diameter = 145;                    // distance between two drive wheels 
    
    int encoder_limit;                         // encoder count at which motion stops
    int vl;                                    // left velocity
    int vr;                                    // right velocity
    
    //print("%s", colour);
    
    
    if(colour == white){
      encoder_limit = bot_diameter * PI;    
      vl = 50;                             
      vr = -50;                            
    }   
    
    else if(colour == black){
      encoder_limit = 100;                  
      vl = 50;                             
      vr = 50;                             
    } 
       
    
    float left_count_start = encoder_vals[0];  // left encoder count at start 
    
    // turn until set limit reached
    while(fabs(encoder_vals[0] - left_count_start) < encoder_limit){     
      
      s3_motorSet(vl, vr, 0);                 // turn on spot
      left += s3_lineSensor(S3_LEFT);          // cumulative sum left IR sensor
      right += s3_lineSensor(S3_RIGHT);        // cumulative sum right IR sensor
      count += 1;                              // cumulative sum while loops

    }
    s3_motorSet(0, 0, 0);                  // stop moving         
    
    int leftave = left/count;        // average left IR sensor
    int rightave = right/count;      // average right IR sensor
    print("left ave = %d, right ave = %d", leftave, rightave);
    
    
    if(colour == white){
      s3_memoryWrite(1, leftave);      // store in non-volatile memory 
      s3_memoryWrite(2, rightave);                           
    }   
    
    else if(colour == black){
      s3_memoryWrite(3, leftave);      // store in non-volatile memory 
      s3_memoryWrite(4, rightave);                           
    }    
    
}                 




