/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "s3.h"
#include "scribbler.h"

void s3_calibrate_line_sensor(void);

int main()                                    // Main function
{
  s3_setup();
  s3_calibrate_line_sensor();

  // Add startup code here.

 
  while(1)
  {
    // Add main loop code here.
    int left = s3_simpleLine(S3_IS, S3_LEFT, S3_BLACK);
    int right = s3_simpleLine(S3_IS, S3_RIGHT, S3_BLACK);
    print("%d", left);
    print("%d \n", right);
    
    if(left){
      s3_motorSet(50, 100, 0);
    }
    
    else if(right){
      s3_motorSet(100, 50, 0);
    }      
      
    else{
      s3_motorSet(50, 50, 0);   
    }        
    
  }  
}

void s3_calibrate_line_sensor(void) {
  int __lineSenCal[4];
  __lineSenCal[0] = s3_lineSensor(S3_LEFT);
  __lineSenCal[1] = s3_lineSensor(S3_RIGHT);
  __lineSenCal[2] = s3_lineSensor(S3_LEFT);
  __lineSenCal[3] = s3_lineSensor(S3_RIGHT);
  int __calibrate_timer = CNT + (CLKFREQ / 1000) * 3672;
  s3_motorSet(75, -75, 0);
  while (CNT < __calibrate_timer) {
    int __tempLineSen = s3_lineSensor(S3_LEFT);
    if (__tempLineSen < __lineSenCal[0]) __lineSenCal[0] = __tempLineSen;
    if (__tempLineSen > __lineSenCal[2]) __lineSenCal[2] = __tempLineSen;
    __tempLineSen = s3_lineSensor(S3_RIGHT);
    if (__tempLineSen < __lineSenCal[1]) __lineSenCal[1] = __tempLineSen;
    if (__tempLineSen > __lineSenCal[3]) __lineSenCal[3] = __tempLineSen;
  }
  s3_motorSet(0, 0, 0);
  if (__lineSenCal[2] > __lineSenCal[0]) __lineSenCal[0] = __lineSenCal[2];
  if (__lineSenCal[3] < __lineSenCal[1]) __lineSenCal[1] = __lineSenCal[3];
  scribbler_set_line_threshold((__lineSenCal[0] + __lineSenCal[1]) / 2);
  scribbler_write_line_threshold();
}
