/* SERIAL_TERMINAL USED */


// ------ Libraries and Definitions ------
#include "simpletools.h"
#include "s3.h"
#include "scribbler.h"



// ------ Main Program ------
int main() {
  s3_setup();
  pause(100);

  if (s3_resetButtonCount() == 1) {
    // stright line
    while(1){
      s3_motorSet(50, 50, 0);
      pause(200);
      if(!s3_tailWheelMoving()){
        recovery_maneuver();
      }
    }
  }      
 

  if (s3_resetButtonCount() == 2) {
    // drive in a circle
    for (int __n = 0; __n < 4; __n++) {
      s3_motorSetRotate(360, 30 * 10000 / 491, 50);
    }
  }
  
  
  if (s3_resetButtonCount() == 3) {
    // ta-da
    ta_da();
  }
  
  
  if (s3_resetButtonCount() == 4) {
    // uh-oh
    uh_oh();
  }
  
  
  if (s3_resetButtonCount() == 5) {
    // uh-oh
    beethoven(40);
  }
 
}


void ta_da(){
  s3_setVolume(20);
  s3_playNote(659, 1046, 152);
  pause(35);
  s3_playNote(659, 1046, 562);
}  



void uh_oh(){
  s3_setVolume(20);
  s3_playNote(200, 0, 175);
  pause(100);
  s3_playNote(160, 0, 562);
}  


void beethoven(int vol){
  s3_simplePlay((6272 / 16), 250, vol);
  s3_simplePlay((6272 / 16), 250, vol);
  s3_simplePlay((6272 / 16), 250, vol);
  s3_simplePlay((4978 / 16), 1000, vol);
  pause(300);
  s3_simplePlay((5588 / 16), 250, vol);
  s3_simplePlay((5588 / 16), 250, vol);
  s3_simplePlay((5588 / 16), 250, vol);
  s3_simplePlay((4699 / 16), 1000, vol);
}



void recovery_maneuver(){
  s3_motorSetRotate(-60, 100 * 1000 / 491, 200);
}
