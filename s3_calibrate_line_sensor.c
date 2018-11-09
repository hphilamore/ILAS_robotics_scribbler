void s3_calibrate_line_sensor(void) {
  int __lineSenCal[4];
  __lineSenCal[0] = s3_lineSensor(S3_LEFT);
  __lineSenCal[1] = s3_lineSensor(S3_RIGHT);
  __lineSenCal[2] = s3_lineSensor(S3_LEFT);
  __lineSenCal[3] = s3_lineSensor(S3_RIGHT);
  int __calibrate_timer = CNT + (CLKFREQ / 1000) * 3672;
  s3_motorSet(75, -75, 0);
  while (CNT < __calibrate_timer) {
    int __tempLineSen = s3_lineSensor(S3_LEFT);
    if (__tempLineSen < __lineSenCal[0]) __lineSenCal[0] = __tempLineSen;
    if (__tempLineSen > __lineSenCal[2]) __lineSenCal[2] = __tempLineSen;
    __tempLineSen = s3_lineSensor(S3_RIGHT);
    if (__tempLineSen < __lineSenCal[1]) __lineSenCal[1] = __tempLineSen;
    if (__tempLineSen > __lineSenCal[3]) __lineSenCal[3] = __tempLineSen;
  }
  s3_motorSet(0, 0, 0);
  if (__lineSenCal[2] > __lineSenCal[0]) __lineSenCal[0] = __lineSenCal[2];
  if (__lineSenCal[3] < __lineSenCal[1]) __lineSenCal[1] = __lineSenCal[3];
  scribbler_set_line_threshold((__lineSenCal[0] + __lineSenCal[1]) / 2);
  scribbler_write_line_threshold();
}