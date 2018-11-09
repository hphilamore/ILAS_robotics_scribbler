void ta_da(){
  s3_setVolume(20);
  s3_playNote(659, 1046, 152);
  pause(35);
  s3_playNote(659, 1046, 562);
}  


void uh_oh(){
  s3_setVolume(20);
  s3_playNote(200, 0, 175);
  pause(100);
  s3_playNote(160, 0, 562);
}  

void beethoven(int vol){
  s3_simplePlay((6272 / 16), 250, vol);
  s3_simplePlay((6272 / 16), 250, vol);
  s3_simplePlay((6272 / 16), 250, vol);
  s3_simplePlay((4978 / 16), 1000, vol);
  pause(300);
  s3_simplePlay((5588 / 16), 250, vol);
  s3_simplePlay((5588 / 16), 250, vol);
  s3_simplePlay((5588 / 16), 250, vol);
  s3_simplePlay((4699 / 16), 1000, vol);
}