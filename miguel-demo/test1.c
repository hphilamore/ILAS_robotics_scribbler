/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "s3.h"

int main()                                    // Main function
{
  // Add startup code here.

 
  while(1)
  {
    // Add main loop code here.
    print("%d\n", s3_lineSensor(S3_LEFT));
    
  }  
}
