# 1 "c:\\Users\\Sunqker\\Desktop\\esp8266Test\\DHT11Test\\DHT11Test.ino"
# 1 "c:\\Users\\Sunqker\\Desktop\\esp8266Test\\DHT11Test\\DHT11Test.ino"
/**********************************************************************

  项目名称/Project          : 零基础入门学用物联网

  程序名称/Program name     : dht11esp8266温湿度读取

  团队/Team                 : 太极创客团队 / Taichi-Maker (www.taichi-maker.com)

  作者/Author               : 小黑

  日期/Date（YYYYMMDD）     : 20200617

  程序目的/Purpose          :

  演示用esp8266读取DHT11采集的温湿度值，并用串口打印采集到的温湿度数值

  使用的第三方库/Library

  DHT11库

  下载请前往

  http://www.taichi-maker.com/homepage/download/#library-download

***********************************************************************/
# 15 "c:\\Users\\Sunqker\\Desktop\\esp8266Test\\DHT11Test\\DHT11Test.ino"
/********************###库文件声明###********************/
# 17 "c:\\Users\\Sunqker\\Desktop\\esp8266Test\\DHT11Test\\DHT11Test.ino" 2

/********************###定义###********************/
dht11 DHT11;//定义传感器类型


/********************###子函数###********************/
double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32; //摄氏温度度转化为华氏温度
}

double Kelvin(double celsius)
{
  return celsius + 273.15; //摄氏温度转化为开氏温度
}

/********************###程序初始化###********************/
void setup()
{
  Serial.begin(115200);//设置波特率为115200
  Serial.println("\n");
  Serial.println("esp8266读取DHT11传感器数值 ");
  Serial.println("串口会分别打印当前湿度（%），当前摄氏度温度，当前华氏度温度和当前开式温度 ");
  Serial.print("DHT11库文件版本: ");
  Serial.println("0.4.1");
  Serial.println();
}

/********************###主函数###********************/
void loop()
{
  Serial.println("\n");
  DHT11.read(2/*定义传感器连接引脚。此处的PIN2在NodeMcu8266开发板上对应的引脚是D4*/); //更新传感器所有信息

  Serial.print("当前湿度 (%): ");
  Serial.println((float)DHT11.humidity, 2);

  Serial.print("当前温度 (℃): ");
  Serial.println((float)DHT11.temperature, 2);

  Serial.print("当前温度 (℉): ");
  Serial.println(Fahrenheit(DHT11.temperature), 2);

  Serial.print("当前温度 (K): ");
  Serial.println(Kelvin(DHT11.temperature), 2);


  delay(2000);//每两秒打印一次
}
