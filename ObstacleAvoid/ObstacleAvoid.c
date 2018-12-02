#include "simpletools.h" 
#include "s3.h" 
#include "scribbler.h"
#include "servo.h"
#include "ping.h"

int PID_time_old; 
float PID_err_old;
float P, I, D;
float Feedback, Setpoint;
float kp, ki, kd;

float DistanceSense(int trig_pin, int echo_pin);
int calculatePID(float Setpoint, float Feedback);

static float encoder_vals[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};   // an array to hold encoder data
void encoder_update(void);

   
        

int main()                                    
{
  s3_setup();
  PID_time_old = CNT/st_usTicks;
  


    if (s3_resetButtonCount() == 1) {
      // Calibrate IR line sensors for WHITE
      // Turn on spot
      //WhiteCalibrate();   
      //Calibrate("w"); 
      while(1){   
           TurnToLight(70);}      
    }  
    
    
    if (s3_resetButtonCount() == 2) {
      // Calibrate IR line sensors for BLACK
      // Drive 5cm in straight line 
      //BlackCalibrate(); 
      //Calibrate("b");    
      float d = DistanceSense(0, 1); 
      print("%.2f\n", d);
      s3_motorSet(50, 50, 0);
      pause(1000);
      while(1){
        //WallFollow(d, 70, 0, 1);
        WallFollow(d, 0, 1);
      }  
      
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
        //IR_ObstacleAvoid(70);
        
        DigitalLightFollow(70);
        //SimpleLightFollow(70);
        
        pause(200);
        IR_ObstacleAvoid(70);
        StallAvoid(70);
        
        
      }                    
    }  
    
    if (s3_resetButtonCount() == 7) {
      /*
      // line following : proportional controller
      while(1){
        s3_motorSet(70, 70, 0);
        StallAvoid(70);
      }  
      */
      while(1){
        float d = DistanceSense(0, 1); 
        print("%.2f\n", d);
      }      
      //PingTest();  
                    
    } 
    
    if (s3_resetButtonCount() == 8) {
      // line following : proportional controller
      
      unsigned int time_old = CNT/st_usTicks;
      
      while(1){
        /*
              servo_angle(0, 180 * 10);
              pause(500);
              servo_angle(0, 90 * 10); 
              pause(500);
              */
            int count = CNT;
            //unsigned int int_time = CNT/CLKFREQ;
            //unsigned int int_time = CNT/st_iodt;
            unsigned int time_new = CNT/st_usTicks;
            //print("%d\t", int_time);
            //print("%d\n", time);
            //print("%d\t", CNT/1000000);
            //print("%d\t", CLKFREQ/1000000);
            unsigned int time_delta = time_new - time_old;
            print("%d\t", CNT/CLKFREQ);
            print("%d\t", time_old);
            print("%d\t", time_new);
            print("%d\n", time_delta);          
            }
              
       
      /*
      servo_angle(0, 180 * 10);
      pause(500);
      servo_angle(0, 90 * 10); 
      pause(500); 
      */   
                    
    }        

}

int calculatePID(float Setpoint, float Feedback){
  // Returns a value to chnage motor speed by to correct error between feedbacl and set point
  
   /*How long since we last calculated*/
   int PID_time_new = CNT/st_usTicks;
   int timeChange = (PID_time_new - PID_time_old);
  
   // Find error variables
   float err = Setpoint - Feedback;
   P =   err; 
   I += (err * timeChange);
   D =  (err - PID_err_old) / timeChange;
  
   /*Remember some variables for next time*/
   PID_err_old = err;
   PID_time_old = PID_time_new;

   /*Compute PID Output*/
   return (int)(kp * P + ki * I + kd * D);
   
}

void motorPIDcontrol(int PIDvalue, int bias){
  // sets the motor speed based on a PID control variable and bias 
  int leftMotorSpeed = bias - PIDvalue;
  int rightMotorSpeed = bias + PIDvalue;
  s3_motorSet( leftMotorSpeed , rightMotorSpeed , 0);
}



void AnalogLight(void){
  // Robot follows the brightest light in its path
  int left = s3_lightSensor(SCRIBBLER_LEFT);
  int right = s3_lightSensor(SCRIBBLER_RIGHT);
  int centre = s3_lightSensor(SCRIBBLER_CENTER);
  print("%d\t", left);
  print("%d\t", centre);
  print("%d\n", right);
 
}

void StallAvoid(int basic_speed){
  // escape maneuver if stall detected
  //while (1) {
    //pause(200);
    if (!s3_tailWheelMoving()) {
      print("Stalled \n");
      s3_motorSet(-basic_speed, -basic_speed, 1500);   // move backwards
      s3_simpleSpin(90, basic_speed, 0);               // quarter clockwise turn
    }
    else
    {
      print("Not Stalled \n");
    }
    //s3_motorSet(-basic_speed, -basic_speed, 1500);   // move backwards
    //s3_simpleSpin(90, basic_speed, 0);               // quarter clockwise turn
  //}
}      
    
 




void IR_ObstacleAvoid(int basic_speed){
  // Performs escape maneuver if IR sensors detect obstacle
  // Otherwise drives straight at input basic speed
  
  int left = s3_simpleObstacle(S3_IS, S3_LEFT);
  int right = s3_simpleObstacle(S3_IS, S3_RIGHT);
  int centre = s3_simpleObstacle(S3_IS, S3_CENTER);
  int detected = s3_simpleObstacle(S3_IS, S3_DETECTED);
  
  print("%d\t", detected);
  print("\t");
  print("%d\t", left);
  print("%d\t", centre);
  print("%d\n", right);
  
  if(detected){
    if(left && !right){         // obstacle to left
      s3_motorSet(-10, 70, 0);  // turn right
    }
    else if(right && !left){    // obstacle to right
      s3_motorSet(70, -10, 0);  // turn left
    } 
    else{                       // obstacle directly in front
      s3_simpleSpin(60, 50, 0); // clockwise turn
    } 
  }
  else{
    s3_motorSet(basic_speed, basic_speed, 0);
  }                
}

  
  



void SimpleLightFollow(int basic_speed){
  // Robot follows the brightest light in its path
  // If brightest not detected, goes straight
  
  int low_speed = basic_speed * 0.7;
  int high_speed = basic_speed / 0.7;
  
  if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){
    while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
      s3_motorSet(low_speed, high_speed, 0);  
      if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | !s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){ 
        break;}                
    } //while
   } // if      
  
  else if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){
    while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
      s3_motorSet(high_speed, low_speed, 0);
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) | !s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){ 
        break;}
    } //while
  } // if
  
  else{
    s3_motorSet(basic_speed, basic_speed, 0);}   
}




void DigitalLightFollow(int basic_speed){
  // Robot follows the brightest light in its path
  // If brightest not detected, turns on spot
  
  int bot_diameter = 145;                      // distance between two drive wheels 
  
  if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |  // brightest detected
      s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
      s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
        
    SimpleLightFollow(basic_speed);
  } 
     
  
  else{   // brightest not detected
  encoder_update();                          // get new encoder values
  float left_count_start = encoder_vals[0];  // left encoder count at start 
  
  // turn on spot until full circle reache  d
    while(fabs(encoder_vals[0] - left_count_start) < bot_diameter * 3.142){     
      
      s3_motorSet(basic_speed, -basic_speed, 0);    
      
      encoder_update();                             // get new encoder values
      
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected 
          s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
          s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
            break;}      
    }
    
    // go straight
    while(1){
      s3_motorSet(basic_speed, basic_speed, 0);
      
      IR_ObstacleAvoid(basic_speed); 
      StallAvoid(basic_speed);
                        
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected    
          s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
          s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
            break;}     
    } 
  } //else 
                
} //function


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
    // colour = "w" (white) or "b" (black) 
    
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
  
  int left = 0;                              // variables for IR sensor values      
  int right = 0;
  int count = 0;
  
  int bot_diameter = 145;                    // distance between two drive wheels
  
  encoder_update();                          // get new encoder values
  float left_count_start = encoder_vals[0];  // left encoder count at start 
  
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
      
      int left = 0;                              // variables for IR sensor values      
      int right = 0;
      int count = 0;
      
      int bot_diameter = 145;                    // distance between two drive wheels
      
      encoder_update();                          // get new encoder values
      float left_count_start = encoder_vals[0];  // left encoder count at start 
      
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
      // line following 
      // conroller = "b" (binary) or "p" (proportional) 
      
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

/*
void WallFollow(float set_point, 
                int basic_speed, 
                int trig_pin, 
                int echo_pin){
  // Follows a wall by maintaining a distance defined by setpoint (cm)
  // 
  float d = DistanceSense(trig_pin, echo_pin); 
  print("set point %d\t", basic_speed); 
  print("%.2f\n", d);               
      

  if(d > set_point){       
    //s3_motorSet(basic_speed - 20, basic_speed + 20, 0);
    s3_motorSet(-basic_speed, basic_speed, 0);
    s3_setLED(S3_LEFT, S3_COLOR_FF0000);
    s3_setLED(S3_CENTER, S3_COLOR_FF0000);
    s3_setLED(S3_RIGHT, S3_COLOR_FF0000);
    pause(200);
    s3_motorSet(basic_speed, basic_speed, 0);
    pause(500);}
     
  else{                                     
    //s3_motorSet(basic_speed + 20, basic_speed - 20, 0);
    s3_motorSet(basic_speed, -basic_speed, 0);
    s3_setLED(S3_LEFT, S3_COLOR_00FF00 );
    s3_setLED(S3_CENTER, S3_COLOR_00FF00 );
    s3_setLED(S3_RIGHT, S3_COLOR_00FF00 );    
    pause(200);
    s3_motorSet(basic_speed, basic_speed, 0);
    pause(500);}                   
}
*/

void WallFollow(float set_point, 
                //int basic_speed, 
                int trig_pin, 
                int echo_pin){
  // Follows a wall by maintaining a distance defined by setpoint (cm)
  // Wall on Right side of robot
  float d = DistanceSense(trig_pin, echo_pin);  
  print("set point = %.2f\n", d);  
  int high = 70;
  int low = 40;
  int dt = 100; //200;             
      

  if(d > set_point){       
    //s3_motorSet(basic_speed - 20, basic_speed + 20, 0);
    //s3_motorSet(40, basic_speed, 0);
    s3_motorSet(high, low, 0);
    s3_setLED(S3_LEFT, S3_COLOR_FF0000);
    s3_setLED(S3_CENTER, S3_COLOR_FF0000);
    s3_setLED(S3_RIGHT, S3_COLOR_FF0000);
    pause(dt);
    //s3_motorSet(basic_speed, basic_speed, 0);
    //pause(500);
    }
     
  else{                                     
    //s3_motorSet(basic_speed + 20, basic_speed - 20, 0);
    //s3_motorSet(basic_speed, 40, 0);
    s3_motorSet(low, high, 0);
    s3_setLED(S3_LEFT, S3_COLOR_00FF00 );
    s3_setLED(S3_CENTER, S3_COLOR_00FF00 );
    s3_setLED(S3_RIGHT, S3_COLOR_00FF00 );    
    pause(dt);
    //s3_motorSet(basic_speed, basic_speed, 0);
    //pause(500);
    }                   
}


void PingTest(void){
  // Measures the distance of travel of an ultrasonic pulse from HC-SR04 sensor 
  // setup pins
  int trigger_pin = 0;
  int echo_pin = 1;
  set_direction(trigger_pin, 0); // output
  set_direction(echo_pin, 1);    // input  
  
  // send a pulse
  int time_old = CNT/st_usTicks;
  int time_new = CNT/st_usTicks;
  int time_delta = 10;    
  while(time_new - time_old < time_delta){    
     high(trigger_pin);       
     time_new = CNT/st_usTicks;}
  low(trigger_pin); 
         
  for(int i = 0; i < 100; i++){
    print("%d\n", input(echo_pin));
  }  
}



float DistanceSense(int trig_pin, int echo_pin){
  // Measures the distance of travel of an ultrasonic pulse from HC-SR04 sensor 
  
  // setup pins
  set_direction(trig_pin, 0); // output
  set_direction(echo_pin, 1);    // input  
  
  // variables for measuring echo pulse
  
  
  // send a pulse
  int time_old = CNT/st_usTicks;
  int time_new = CNT/st_usTicks;
  int time_delta = 10;    
  while(time_new - time_old < time_delta){    
     high(trig_pin);       
     time_new = CNT/st_usTicks;}
  low(trig_pin);
  
  
  int echo, previousEcho, lowHigh, highLow;
  int startTime, stopTime, difference;
  float rangeCm;
  
  
  // check for received pulse
  lowHigh = highLow = echo = previousEcho = 0;
  
  while(0 == lowHigh || highLow == 0) {
    previousEcho = echo;
    echo = input(echo_pin);
    
    if(0 == lowHigh && 0 == previousEcho && 1 == echo) {
      lowHigh = 1;
      startTime = CNT/st_usTicks;
    }
    
    if(1 == lowHigh && 1 == previousEcho && 0 == echo) {
      highLow = 1;
      stopTime = CNT/st_usTicks;
    }
  }
  difference = stopTime - startTime;
  rangeCm = difference / 58;
  return rangeCm;
  //print("Start: %d, stop: %d, difference: %d, range: %.2f cm\n", startTime, stopTime, difference, rangeCm);
  //print("differece: %d, Distance: %.2f cm\n", difference, rangeCm);
 
} 
  
  
 
/*
float DistanceSense(void){
  // Measures the distance of travel of an ultrasonic pulse from HC-SR04 sensor 
  
  // setup pins
  int trigger_pin = 0;
  int echo_pin = 1;
  set_direction(trigger_pin, 0); // output
  set_direction(echo_pin, 1);    // input  
  
  // variables for measuring echo pulse
  
  
  // send a pulse
  int time_old = CNT/st_usTicks;
  int time_new = CNT/st_usTicks;
  int time_delta = 10;    
  while(time_new - time_old < time_delta){    
     high(trigger_pin);       
     time_new = CNT/st_usTicks;}
  low(trigger_pin);
  
  
  int echo, previousEcho, lowHigh, highLow;
  int startTime, stopTime, difference;
  float rangeCm;
  
  
  // check for received pulse
  lowHigh = highLow = echo = previousEcho = 0;
  
  while(0 == lowHigh || highLow == 0) {
    previousEcho = echo;
    echo = input(echo_pin);
    
    if(0 == lowHigh && 0 == previousEcho && 1 == echo) {
      lowHigh = 1;
      startTime = CNT/st_usTicks;
    }
    
    if(1 == lowHigh && 1 == previousEcho && 0 == echo) {
      highLow = 1;
      stopTime = CNT/st_usTicks;
    }
  }
  difference = stopTime - startTime;
  rangeCm = difference / 58;
  return rangeCm;
  //print("Start: %d, stop: %d, difference: %d, range: %.2f cm\n", startTime, stopTime, difference, rangeCm);
  //print("differece: %d, Distance: %.2f cm\n", difference, rangeCm);
 
} 
 */ 



void OnOffPulse(void){
  // Sends an ON OFF pulse 
  
  int trigger_pin = 0;
  int time_old = CNT/st_usTicks;
  int time_new = CNT/st_usTicks;
  unsigned int time_delta = 10;
  
  while (1){  
  
    while(time_new - time_old < time_delta){
    
      high(trigger_pin); 
      
      time_new = CNT/st_usTicks;
      
    }
    
    time_old = time_new;    
      
    while(time_new - time_old < time_delta){
    
      low(trigger_pin); 
      
      time_new = CNT/st_usTicks;
      
    }
    
    time_old = time_new;
  }  
  
}  


void ReachLight(void){
  // If brightest light detected directly in front of robot,
  // move in direction of light with speed inversely proportional to brightness.
	if (s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
    int light = s3_lightSensor(SCRIBBLER_CENTER);
    int motor_speed = mapInt(light, 0, 255, 100, 0);
    s3_motorSet(motor_speed, motor_speed, 0);
    pause(200);}
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

float DistanceTest(int trig, int echo){
  print("%d", trig);
  print("%d", echo);
}  
  


  
  
  
  
  
  