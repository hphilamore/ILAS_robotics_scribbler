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