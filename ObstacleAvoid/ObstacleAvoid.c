#include "simpletools.h" 
#include "s3.h" 
#include "scribbler.h"

static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data

void encoder_update(void);

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
 




                

int main()                                    
{
  s3_setup();
  
    

    if (s3_resetButtonCount() == 1) {
      // Calibrate IR line sensors for WHITE
      // Turn on spot
      //WhiteCalibrate();   
      Calibrate("w");    
    }  
    
    
    if (s3_resetButtonCount() == 2) {
      // Calibrate IR line sensors for BLACK
      // Drive 5cm in straight line 
      //BlackCalibrate(); 
      Calibrate("b");     
      
    }     
    
    if (s3_resetButtonCount() == 3) {
      // line following : binary controller
      //LineFollowBinary(); 
      LineFollow("b");    
      
    } 
        
        
     if (s3_resetButtonCount() == 4) {
      // line following : proportional controller
      //LineFollowProp(); 
      LineFollow("p");           
          
    } 
    
    if (s3_resetButtonCount() == 5) {
      // line following : proportional controller
      //LineFollowProp();
      while(1){ 
        AnalogLightFollow(); 
      }                    
    }  
    
    
    if (s3_resetButtonCount() == 6) {
      // line following : proportional controller
      //LineFollowProp();
      while(1){ 
        IR_ObstacleAvoid(70);
        //AnalogLightFollow(); 
      }                    
    }         

}




void IR_ObstacleAvoid(int basic_speed){
  // Performs escape maneuver if IR sensors detect obstacle
  
  int left = s3_simpleObstacle(S3_IS, S3_LEFT);
  int right = s3_simpleObstacle(S3_IS, S3_RIGHT);
  int centre = s3_simpleObstacle(S3_IS, S3_CENTER);
  int detected = s3_simpleObstacle(S3_IS, S3_DETECTED);
 

  //int left = s3_readObstacle(SCRIBBLER_LEFT);
  //int right = s3_readObstacle(SCRIBBLER_RIGHT);
  
  
  print("%d\t", detected);
  print("\t");
  print("%d\t", left);
  print("%d\t", centre);
  print("%d\n", right);
  
  if(detected){
    if(left && !right){
      //print("left not right")
      s3_motorSet(-10, 70, 0);
    }
    else if(right && !left){
      s3_motorSet(70, -10, 0);
    } 
    else{
      //s3_motorSet(100, -10, 200);
      s3_simpleSpin(60, 50, 0);
    } 
  }
  else{
    s3_motorSet(basic_speed, basic_speed, 0);
    // test
  }             
            
          
      
  
}

  
  



void SimpleLightFollow(void){
  // Robot follows the brightest light in its path
  // If brightest not detected, goes straight
  if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){
    s3_motorSet(-70, 70, 0);}
  
  else if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){
    s3_motorSet(70, -70, 0);}
  
  else{
    s3_motorSet(70, 70, 0);}   
}




void DigitalLightFollow(void){
  // Robot follows the brightest light in its path
  // If brightest not detected, turns on spot
  
  int bot_diameter = 145;                    // distance between two drive wheels 
  

  if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){        // light to left --> turn left
    s3_motorSet(-70, 70, 0);}
  
  else if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){  // light to right --> turn right
    s3_motorSet(70, -70, 0);}
  
  else if(s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){ // light at centre --> go straight
    s3_motorSet(70, 70, 0);}
  
  
  else{   // no light         
    encoder_update();                                   // get new encoder values
    float left_count_start = encoder_vals[0];           // left encoder count at start 
    float angle_rad = 0.52;                             // angle to turn on spot
    float angle_mm = bot_diameter * angle_rad / 3.142;  // angle arc in mm
    while(fabs(encoder_vals[0] - left_count_start) < angle_mm){          
      s3_motorSet(-50, 50, 0);
      encoder_update();}      
  }              
}


void AnalogLightFollow(void){
  // Robot follows the brightest light in its path
  int left = s3_lightSensor(SCRIBBLER_LEFT);
  int right = s3_lightSensor(SCRIBBLER_RIGHT);
  int centre = s3_lightSensor(SCRIBBLER_CENTER);
  print("%d\t", left);
  print("%d\t", centre);
  print("%d\n", right);
  
  if (left > right && left > centre){             // brightest is left
   s3_motorSet(-70, 70, 0);}                      // turn left
  
  else if (right > left && right > centre){       // brightest is right 
   s3_motorSet(70, -70, 0);}                      // turn right
  
  else{                                           // brightest is centre
   s3_motorSet(70, 70, 0);}                       // go straight
}
     
  
  
  

 
  

void Calibrate(char colour){
  // Calibrate IR line sensors 
  // colour = "white" or "black"   
  
  int left = 0;                              // variables for IR sensor values      
  int right = 0;
  int count = 0;
  
  int bot_diameter = 145;                    // distance between two drive wheels 
  
  int encoder_limit;                         // encoder count at which motion stops
  int vl;                                    // left velocity
  int vr;                                    // right velocity
  
  
  //if(colour == "Lend money!"){
  if(colour == "w"){
    encoder_limit = bot_diameter * 3.142;    
    vl = 50;                             
    vr = -50;                            
  }   
  
  if(colour == "b"){
    encoder_limit = 50;                  
    vl = 50;                             
    vr = 50;                             
  } 
     
  
  encoder_update();                          // get new encoder values
  float left_count_start = encoder_vals[0];  // left encoder count at start 
  
  // turn until full circle reached
  while(fabs(encoder_vals[0] - left_count_start) < encoder_limit){     
    
    s3_motorSet(vl, vr, 0);                 // turn on spot
    left += s3_lineSensor(S3_LEFT);          // cumulative sum left IR sensor
    right += s3_lineSensor(S3_RIGHT);        // cumulative sum right IR sensor
    count += 1;                              // cumulative sum while loops
    encoder_update();                        // get new encoder values
  }
  s3_motorSet(0, 0, 0);                  // stop moving         
  
  int leftave_white = left/count;        // average left IR sensor
  int rightave_white = right/count;      // average right IR sensor
  
  s3_memoryWrite(1, leftave_white);      // store in non-volatile memory 
  s3_memoryWrite(2, rightave_white);
  
  
}  



void WhiteCalibrate(void){
  // Calibrate IR line sensors for WHITE
  // Turn on spot  
  
  encoder_update();                          // get new encoder values
  float left_count_start = encoder_vals[0];  // left encoder count at start 
  
  int left = 0;                              // variables for IR sensor values      
  int right = 0;
  int count = 0;
  
  int bot_diameter = 145;                    // distance between two drive wheels
  
  // turn until full circle reached
  while(fabs(encoder_vals[0] - left_count_start) < bot_diameter * 3.142){     
    
    s3_motorSet(50, -50, 0);                 // turn on spot
    left += s3_lineSensor(S3_LEFT);          // cumulative sum left IR sensor
    right += s3_lineSensor(S3_RIGHT);        // cumulative sum right IR sensor
    count += 1;                              // cumulative sum while loops
    encoder_update();                        // get new encoder values
  }
  s3_motorSet(0, 0, 0);                  // stop moving         
  
  int leftave_white = left/count;        // average left IR sensor
  int rightave_white = right/count;      // average right IR sensor
  
  s3_memoryWrite(1, leftave_white);      // store in non-volatile memory 
  s3_memoryWrite(2, rightave_white);
}  


void BlackCalibrate(void){
      // Calibrate IR line sensors for BLACK
      // Drive 5cm in straight line
      
      encoder_update();                          // get new encoder values
      float left_count_start = encoder_vals[0];  // left encoder count at start 
      
      int left = 0;                              // variables for IR sensor values      
      int right = 0;
      int count = 0;
      
      int bot_diameter = 145;                    // distance between two drive wheels
      
      // turn until 5cm linear travel reached
      while(fabs(encoder_vals[0] - left_count_start) < 50){     
        
        s3_motorSet(50, 50, 0);                 // turn on spot
        left += s3_lineSensor(S3_LEFT);         // cumulative sum left IR sensor
        right += s3_lineSensor(S3_RIGHT);       // cumulative sum right IR sensor
        count += 1;                             // cumulative sum while loops
        encoder_update();                       // get new encoder values
      }
      s3_motorSet(0, 0, 0);                  // stop moving         
      
      int leftave_black = left/count;        // average left IR sensor
      int rightave_black = right/count;      // average right IR sensor
      
      s3_memoryWrite(3, leftave_black);      // store in non-volatile memory 
      s3_memoryWrite(4, rightave_black);
}      


void LineFollow(char controller){
      // line following : binary controller
      
      int leftave_white = s3_memoryRead(1);                      // stored average IR sensor calibration values
      int rightave_white = s3_memoryRead(2);
      int leftave_black = s3_memoryRead(3);
      int rightave_black = s3_memoryRead(4);
      
      int mid_left = (leftave_white - leftave_black) / 2;        // midpoint between black and white
      int mid_right = (rightave_white - rightave_black) / 2;
      
      int v_basic = 30;                                          // basic speed
      
      
      while(1){
      
        if (controller=="b"){
          if(s3_lineSensor(S3_LEFT) > mid_left && s3_lineSensor(S3_RIGHT) < mid_right){       // left=white, right=black --> turn right
            s3_motorSet(40, -20, 0);
            pause(200);}
          
          else if(s3_lineSensor(S3_RIGHT) > mid_right && s3_lineSensor(S3_LEFT) < mid_left){  // left=black, right=white --> turn left
            s3_motorSet(-20, 40, 0);
            pause(200);}
           
          else{                                     // left = right = white/black --> go straight
            s3_motorSet(v_basic, v_basic, 0);     
            pause(200);}
         }          
        
        if (controller=="p"){
          if(s3_lineSensor(S3_LEFT) > mid_left && s3_lineSensor(S3_RIGHT) < mid_right){          // left=white, right=black --> turn right
            int r_error = rightave_white - s3_lineSensor(S3_RIGHT);
            int v_l = v_basic + (100 - v_basic) * r_error / (rightave_white - rightave_black);
            s3_motorSet(v_l, -v_l/2, 0);
            pause(200);}
          else if(s3_lineSensor(S3_RIGHT) > mid_right && s3_lineSensor(S3_LEFT) < mid_left){     // left=black, right=white --> turn left
            int l_error = leftave_white - s3_lineSensor(S3_LEFT);
            int v_r = v_basic + (100 - v_basic) * l_error / (leftave_white - leftave_black);
            s3_motorSet(-v_r/2, v_r, 0);
            pause(200);}
          else{                                     // left = right = white/black --> go straight
            s3_motorSet(v_basic, v_basic, 0);
            pause(200);}
         }         
           
      }      
}


void LineFollowBinary(void){
      // line following : binary controller
      
      int leftave_white = s3_memoryRead(1);                      // stored average IR sensor calibration values
      int rightave_white = s3_memoryRead(2);
      int leftave_black = s3_memoryRead(3);
      int rightave_black = s3_memoryRead(4);
      
      int mid_left = (leftave_white - leftave_black) / 2;        // midpoint between black and white
      int mid_right = (rightave_white - rightave_black) / 2;
      
      int v_basic = 30;                                          // basic speed
      
      while(1){
      
      if(s3_lineSensor(S3_LEFT) > mid_left && s3_lineSensor(S3_RIGHT) < mid_right){       // left=white, right=black --> turn right
        s3_motorSet(40, -20, 0);
        pause(200);}
      
      else if(s3_lineSensor(S3_RIGHT) > mid_right && s3_lineSensor(S3_LEFT) < mid_left){  // left=black, right=white --> turn left
        s3_motorSet(-20, 40, 0);
        pause(200);}
       
      else{                              // left = right = white/black --> go straight
        s3_motorSet(v_basic, v_basic, 0);     
        pause(200);}
      
      } 
}


void LineFollowProp(void){
      // line following : proportional controller
      
      
      int leftave_white = s3_memoryRead(1);                      // stored average IR sensor calibration values
      int rightave_white = s3_memoryRead(2);
      int leftave_black = s3_memoryRead(3);
      int rightave_black = s3_memoryRead(4);
      
      int mid_left = (leftave_white - leftave_black) / 2;        // midpoint between black and white
      int mid_right = (rightave_white - rightave_black) / 2;
      
      int v_basic = 30;                                          // basic speed

      
      while(1){
      
        if(s3_lineSensor(S3_LEFT) > mid_left && s3_lineSensor(S3_RIGHT) < mid_right){          // left=white, right=black --> turn right
          int r_error = rightave_white - s3_lineSensor(S3_RIGHT);
          int v_l = v_basic + (100 - v_basic) * r_error / (rightave_white - rightave_black);
          s3_motorSet(v_l, -v_l/2, 0);
          pause(200);}
        else if(s3_lineSensor(S3_RIGHT) > mid_right && s3_lineSensor(S3_LEFT) < mid_left){     // left=black, right=white --> turn left
          int l_error = leftave_white - s3_lineSensor(S3_LEFT);
          int v_r = v_basic + (100 - v_basic) * l_error / (leftave_white - leftave_black);
          s3_motorSet(-v_r/2, v_r, 0);
          pause(200);}
        else{                                     // left = right = white/black --> go straight
          s3_motorSet(v_basic, v_basic, 0);
          pause(200);}

      
      }
}
