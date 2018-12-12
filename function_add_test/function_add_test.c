/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "s3.h"
#include "scribbler.h"
#include "test_lib.h"


int main()                                    // Main function
{
  // Add startup code here.
  s3_setup();
 
  while(1)
  {
    // Add main loop code here.
    
    s3_lineSensor(S3_LEFT);
    char X = my_func();
    
  }  
}
