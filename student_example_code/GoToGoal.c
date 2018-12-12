void GoToGoal(float dxI, float dyI, float theta, int basic_speed){

int turn_flag = 0;
int straight_flag = 0;

// distance between robot wheels (mm)
int L = 145;

// convert goal global --> local
float dxR = dxI * cos(theta) + dyI * sin(theta);
float dyR = dxI * -sin(theta)+ dyI * cos(theta); 

// plan path
float aR = atan2(dyR, dxR);                         // angle
float dR = powf((powf(dxR,2) + powf(dyR,2)), 0.5);  // distance
float lenR = aR * L / 2;                            // arc length  

float left_count_start = encoder_vals[0];
 
while(1){
if(turn_flag == 0){   // turn to angle

if(aR < 0){      // negative angle
	s3_motorSet( basic_speed, -basic_speed, 0);} 

else{            // positive angle
	s3_motorSet( -basic_speed, basic_speed, 0);} 

if(fabs(encoder_vals[0] - left_count_start) >= fabs(lenR)){ // angle reached
turn_flag = 1;                       // put the flag up
left_count_start = encoder_vals[0];}  // reset the count
} // if 

else if(straight_flag == 0){    // drive to goal 
s3_motorSet( basic_speed, basic_speed, 0);
if(fabs(encoder_vals[0] - left_count_start) >= fabs(dR)){ // goal reached
// put the flag up
straight_flag = 1;
//print("reached!");
s3_simpleStop(); 
} 
} // else if


else if(straight_flag == 1){break;} 

}// while 
} 
