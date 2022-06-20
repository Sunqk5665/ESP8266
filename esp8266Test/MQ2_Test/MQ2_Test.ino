//烟雾传感器模块MQ-2测试
//
/*线路连接

*/const int analogPin = A0; //引脚定义为整型常量
unsigned int mqData=0;

void setup()
{
  Serial.begin(115200);
}

void loop() 
{
  mqData = analogRead(analogPin);//读取A0模拟引脚的数值
  //使用map函数把输出的数值进行映射()
  //int outputValue = map(mqData,0,1023,0,255);
  Serial.print("当前环境中的烟雾浓度：");
  Serial.println(mqData);
  delay(1000);
}
