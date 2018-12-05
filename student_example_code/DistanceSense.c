float DistanceSense(int trig_pin, int echo_pin);

float DistanceSense(int trig_pin, int echo_pin){
  // Measures the distance of travel of an ultrasonic pulse from HC-SR04 sensor 
  
  // setup pins
  set_direction(trig_pin, 0); // output
  set_direction(echo_pin, 1);    // input  
  
  
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