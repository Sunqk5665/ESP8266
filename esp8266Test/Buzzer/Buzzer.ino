/***********蜂鸣器警报***************/
const int PWM = 12; //定义蜂鸣器I/O引脚-->GPIO12(D6)~
void setup() {
  pinMode(PWM,OUTPUT);
}

void loop() {
  for(int i=200;i<=800;i++){
    tone(PWM,i);
    delay(5);
  }
  delay(2000);
  for(int i=800;i>=200;i--){
    tone(PWM,i);
    delay(10);
  }
}
