/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools

int main()                                    // Main function
{
  // Add startup code here.

 
  //while(1)
  //{
   int x = 2; 
   
   x = 3; 
   
   switch (x) 
   { 
       case 1: printf("Choice is 1"); 
               break; 
       case 2: printf("Choice is 2"); 
                break; 
       case 3: printf("Choice is 3"); 
               break; 
       default: printf("Choice other than 1, 2 and 3"); 
                break;   
   } 
   return 0;
    
  //}  
}
