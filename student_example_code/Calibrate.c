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
