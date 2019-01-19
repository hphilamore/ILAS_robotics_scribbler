void TurnToLight(int basic_speed){
// If brightest light detected but not centered, turn until light is directly ahead.

  int low_speed = basic_speed * 0.7;
  int high_speed = basic_speed / 0.7;
    
    // Light is left, turn on spot towaards right
    if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){
      while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){

          s3_motorSet(-high_speed, high_speed, 0);  
          if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT) 
              | !s3_simpleLight(S3_IS, SCRIBBLER_LEFT)){ 
            break;}   
        } //while
     } // if      
    
    // Light is right, turn on spot towards right
    else if (s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){
      while(!s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){

          s3_motorSet(high_speed, -high_speed, 0);
          if (s3_simpleLight(S3_IS, SCRIBBLER_LEFT) | 
              !s3_simpleLight(S3_IS, SCRIBBLER_RIGHT)){ 
            break;}
        } //while
    } // if

  else {s3_motorSet(0, 0, 0);} 
}



