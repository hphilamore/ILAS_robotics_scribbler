int PID_time_old; 
float PID_err_old;
float P, I, D;
float Feedback, Setpoint;
float kp, ki, kd;


void PID(float Setpoint, float Feedback, int Bias){
  // Change motor speed by to correct error between feedback and set point
  
   int out_max = 100;
   int out_min = 10;
   float in_max = 2 * Setpoint;
   float in_min = 0;
  
   kp = 0;
   kd = 0;
   ki = 0;
  
   /*How long since we last calculated*/
   int PID_time_new = CNT/st_usTicks;
   int timeChange = (PID_time_new - PID_time_old);
  
   // Find error variables
   float err = -1 * (Setpoint - Feedback);
   P =   err; 
   I +=  err; //(err * timeChange);
   D =  (fabs(err) - fabs(PID_err_old)); // (err - PID_err_old) / timeChange;
  
   /*Remember some variables for next time*/
   PID_err_old = err;
   PID_time_old = PID_time_new;

   /*Compute PID Output*/
   //return (int)(kp * P + ki * I + kd * D);
   int PIDvalue = (int)(kp * P + ki * I + kd * D);
   
   int leftMotorSpeed = Bias + PIDvalue;
   int rightMotorSpeed = Bias - PIDvalue;
   s3_motorSet( leftMotorSpeed , rightMotorSpeed , 0);
   
   if(leftMotorSpeed > rightMotorSpeed){ 
     s3_setLED(S3_CENTER, S3_COLOR_00FF00);} // green  
     
       
   else if(leftMotorSpeed < rightMotorSpeed){ 
     s3_setLED(S3_CENTER, S3_COLOR_FF0000);} // red   

     
   else{
     s3_setLED(S3_CENTER, S3_COLOR_000000);} // off
   
   pause(100); // loop rate
}

