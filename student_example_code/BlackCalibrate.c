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