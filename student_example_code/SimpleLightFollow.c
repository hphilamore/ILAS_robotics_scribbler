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