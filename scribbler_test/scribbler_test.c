/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "s3.h" 
#include "scribbler.h"

# define L 145

static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

void encoder_update(void);
void TurnToLight(int basic_speed);
void ReachLight(void);


int main()                                    // Main function
{
  s3_setup();

 
  while(1)
  {
    TurnToLight(50);
    ReachLight();
    HuntLight(50);
    
  }  
}

void HuntLight(int basic_speed){
  // If no brightest light detected, turn on spot until light found
  if (!s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_LEFT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){  
    
    print("hunting light \n");
    encoder_update();                          // get new encoder values
    float left_count_start = encoder_vals[0];  // left encoder count at start 
  
    while(fabs(encoder_vals[0] - left_count_start) < L * 3.142){     
    
    s3_motorSet(basic_speed, -basic_speed, 0);    
    
    encoder_update();                             // get new encoder values
    
    if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected 
        s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
        s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
          break;}      
    }
  }    
}      

  


void ReachLight(void){
  // If brightest light detected directly in front of robot,
  // move in direction of light with speed inversely proportional to brightness.
	if (s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
    print("light ahead\n");
    int light = s3_lightSensor(SCRIBBLER_CENTER);
    int motor_speed = mapInt(light, 0, 255, 100, 0);
    s3_motorSet(motor_speed, motor_speed, 0);
    pause(200);}
}



void TurnToLight(int basic_speed){

int low_speed = basic_speed * 0.7;
int high_speed = basic_speed / 0.7;
  
  // Light is left, turn on spot towards right
  if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){
    print("light left\n");
    while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
  
        s3_motorSet(-high_speed, high_speed, 0);  
        if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) 
            | !s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){ 
          break;}   
      } //while
   } // if      
  
  // Light is right, turn on spot towards right
  else if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){
    print("light right\n");
    while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
  
        s3_motorSet(high_speed, -high_speed, 0);
        if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) | 
            !s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){ 
          break;}
      } //while
  } // if

  else {print("no light\n");
  s3_motorSet(0, 0, 0);} 
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