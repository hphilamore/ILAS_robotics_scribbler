a/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h" 
#include "s3.h" 
#include "scribbler.h"


static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

void encoder_update(void);
void TurnToLight(int basic_speed);



int main()                                    // Main function
{
  s3_setup();

 
  while(1)
  {
    TurnToLight(50);
  }  
}



void TurnToLight(int basic_speed){
  // If brightest light detected but not centered, turn until light is directly ahead.
  int low_speed = basic_speed * 0.7;
  int high_speed = basic_speed / 0.7;
  
  // Light is left, turn on spot towaards right
  if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){
  while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){

      //s3_motorSet(low_speed, high_speed, 0);  
      s3_motorSet(-high_speed, high_speed, 0); 
      if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) 
          | !s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){ 
        break;}   
    } //while
   } // if      
  
  // Light is right, turn on spot towards right
  else if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){
  while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){

      //s3_motorSet(high_speed, low_speed, 0);
      s3_motorSet(high_speed, -high_speed, 0);
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) | 
          !s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){ 
        break;}
    } //while
  } // if
  
  else {s3_motorSet(0, 0, 0);} 

}