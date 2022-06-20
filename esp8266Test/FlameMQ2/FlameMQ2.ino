//Flame Sensor and MQ-2 Test
//火焰传感器&烟雾传感器MQ-2测试
const int analogPin = A0;   //定义模拟引脚A0
int astate=0;   //存放模拟引脚A0的数值
void setup()
{
  pinMode(2, INPUT);
  Serial.begin(115200);
}

void loop()
{
  //火焰传感
  int temp = digitalRead(2);
  if(temp==1){
    Serial.print("GPIO2为高电平-->");
    Serial.println(temp);
    Serial.println("无火情...");
  }else{
    Serial.print("GPIO2为低电平-->");
    Serial.print(temp);
    Serial.println("有火！！！");
  }
  //烟雾传感MQ-2
  astate = analogRead(analogPin);//读取A0模拟引脚的数值
  Serial.print("烟雾含量：");
  Serial.println(astate);
  delay(2000);
}
