/*
  超声波传感器（HC-SR04）Test
*/
//超声波传感器引脚定义
const int trigPin = 2;  //GPIO2-->D4
const int echoPin = 0;  //GPIO0-->D3
//间隔时间和距离
long duration,cm;
//int distance;

void setup() {
  Serial.begin(115200);
  //设置超声模块连接的引脚模式
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  cm = calculateDistance();
  Serial.print("测得距离为：");
  Serial.print(cm);
  Serial.println("cm");
  delay(1000);
}

/************超声测距函数************/
int calculateDistance(){
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // 传感器由 10 微秒或更多微秒的 HIGH 脉冲触发。
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  // 预先给出一个短的 LOW 脉冲以确保一个干净的 HIGH 脉冲：
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  // 从传感器读取信号：一个高脉冲，其持续时间为从发出ping到接收到物体回波的时间（以微秒为单位）。
  duration = pulseIn(echoPin, HIGH);
 
  // convert the time into a distance
  // 将时间转换为距离
  cm = (duration*0.034)/2;
  return cm;
}   
