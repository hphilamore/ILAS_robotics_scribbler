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
      
      // Add your own functions for obstacle avoidance
      //IR_ObstacleAvoid(basic_speed); 
      //StallAvoid(basic_speed);
                        
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected    
          s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
          s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
            break;}     
    } 
  } //else 
                
} //function
