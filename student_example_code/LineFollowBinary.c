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
