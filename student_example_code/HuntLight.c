void HuntLight(int basic_speed){
  // If no brightest light detected, turn on spot until light found
  if (!s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_LEFT) && 
      !s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){  
    
    print("hunting light \n");
    encoder_update();                          // get new encoder values
    float left_count_start = encoder_vals[0];  // left encoder count at start 
  
    while(fabs(encoder_vals[0] - left_count_start) < L * 3.142){     
    
      s3_motorSet(basic_speed, -basic_speed, 0);    
      
      encoder_update();                             // get new encoder values
      
      if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) |   // stop if light detected 
          s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) | 
          s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
            break;}     
         
    }
  }    
}   