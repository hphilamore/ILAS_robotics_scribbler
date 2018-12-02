void ReachLight(void){
  // If brightest light detected directly in front of robot,
  // move in direction of light with speed inversely proportional to brightness.
	if (s3_simpleLight(S3_IS, SCRIBBLER_CENTER)){
    int light = s3_lightSensor(SCRIBBLER_CENTER);
    int motor_speed = mapInt(light, 0, 255, 100, 0);
    s3_motorSet(motor_speed, motor_speed, 0);
    pause(200);}
}