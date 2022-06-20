/********火焰传感器Test*******/
//Flame Sensor Test

void setup()
{
  pinMode(2, INPUT);  //GPIO2-->D4
  Serial.begin(115200);
}

void loop()
{
  int temp = digitalRead(2);
  if(!temp){  
     Serial.println("有火!!!");
     Serial.println(temp);
  }else{
     Serial.println("无火...");
     Serial.println(temp);
  }
  delay(2000);
}
