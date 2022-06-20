//Family Security and Environmental Monitoring System
//------------------------头文件引用------------------------------//
#include <ESP8266WiFi.h>          //默认，加载WIFI头文件
#include <PubSubClient.h>         //默认，加载MQTT库文件
#include <Ticker.h>               //Ticker库，用来定时调用函数或多任务处理
#include <U8g2lib.h>   //U8g2库
#include <Wire.h>
#include <ESP8266HTTPClient.h>  //ESP8266的HTTP协议通讯库

//------------------------函数声明--------------------------------//
void connectWiFi(); //WiFi连接函数
void callback(char* topic, byte* payload, unsigned int length);//MQTT消息回调函数
void reconnect();  //MQTT重连函数
void temphumiValue(); //获取温湿度
int FlameValue();  //火焰检测函数
int calculateDistance();//超声波测距函数
void hongwai();    //红外模块函数
void tickerCount();//定时器计数函数
/*******oled显示函数*******/
void OLEDDispaly(String temp, String rh, String gas, String fire, String human);
void doHttpStick();  //微信通知提醒函数
void buzzerAlarm();  //蜂鸣器函数
//--------------------------------------------------------------//

//********************WiFi名称&密码*******************//
const char* ssid = "mi";               //WIFI名字
const char* password = "11111111";     //你的WIFI密码

//-----------------------巴法云MQTT相关配置---------------------------//
#define ID_MQTT "8a1350d0c73d254c726ae6dab79b4fc3"   //用户私钥ID，控制台获取
const char* topic = "bafaTopicTest01";    //主题名字
const char* mqtt_server = "bemfa.com";    //默认，MQTT服务器地址
const int mqtt_server_port = 9501;        //默认，MQTT服务器端口号
WiFiClient wifiClient;             //新建WiFiClient对象，初始化wifi客户端，用于MQTT
WiFiClient wifiClient2;    //新建WiFiClient2，用于HTTP微信消息推送使用
HTTPClient http;  //初始化http
PubSubClient mqttclient(wifiClient);   //建立PubSubClient对象mqttclient
//------------------------------------------------------------------//
long lastMsg = 0;    //时间戳

long timeval = 3 * 1000;     //数据上传的传感器时间间隔，默认3秒

//------------------------Ticker定时设置----------------------------//
Ticker ticker;   //建立Ticker对象，实现定时

//-------------------DHT11温湿度传感器配置---------------------//
#include<dht11.h>      //引入DHT11库
dht11 DHT11;           //定义传感器类型对象
const int DHT11PIN = 16;    //定义DHT11传感器连接引脚-->GPIO16(D0)
double temp,humi; //温湿度变量
//---------------MQ2烟雾传感器配置---------------//
const int analogPin = A0; //引脚定义为整型常量
unsigned int mqData=0;    //烟雾含量

//-------------------火焰传感器配置------------------------//
const int FlamePIN = 0;   //定义火焰传感器引脚-->GPIO0(D3)
int fv;                   //存放 火焰检测函数返回值
 
//------------------超声波(HR-SR04)模块配置--------------//
const int trigPin = 15;     //trig引脚-->GPIO15(D8)
const int echoPin = 13;     //echo引脚-->GPIO13(D7)
long duration,cm;          //间隔时间和距离

//------------------人体红外感应模块配置---------------------//
const int LED = 2;        //GPIO2(D4)和esp8266板载LED相通，低电平有效
const int PIR_sensor = 14;//GPIO14(D5)
int pirVal;               //红外模块out引脚返回值

//----------oled屏配置--------------//
#define SCL 5   //串行时钟线-->GPIO5(D1)
#define SDA 4   //串行数据线-->GPIO4(D2)
/***利用构造器对oled进行初始化****/
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /*clock=*/SCL, /*data=*/SDA, /*reset=*/U8X8_PIN_NONE);
//------------蜂鸣器配置---------------//
const int buzzerPin = 12; //定义蜂鸣器I/O引脚-->GPIO12(D6)

//--------------微信消息推送配置--------------------//
String uid = "8a1350d0c73d254c726ae6dab79b4fc3";
String title = "人体红外传感器设备";                  // 设备名称
String wxmsg = "检测陌生人闯入家门，请立即确认！！！";   //发送的消息
String ApiUrl = "http://go.bemfa.com/v1/sendwechatcard"; //默认 api 网址


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);    //设置串口波特率为115200
  
  pinMode(DHT11PIN, INPUT); //定义DHT11PIN为输入(INPUT)模式

  pinMode(FlamePIN,INPUT);  //定义FlamePIN为输入(INPUT)模式
  
  //设置超声模块连接的引脚模式
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //---------LED引脚---------//
  pinMode(LED_BUILTIN, OUTPUT);    //设置引脚为输出模式
  digitalWrite(LED_BUILTIN, LOW);  //默认引脚上电高电平
  //-------蜂鸣器引脚--------//
  pinMode(buzzerPin,OUTPUT);
  //人体红外感应模块引脚设置
  pinMode(PIR_sensor, INPUT); 
  
  ticker.attach(2,hongwai);//每隔2秒执行一次hongwai函数，获取红外模块检测数据
  
  connectWiFi();           //连接wifi
  mqttclient.setServer(mqtt_server, mqtt_server_port);//设置mqtt服务器和端口号
  mqttclient.setCallback(callback);                   //设置消息回调函数，进行mqtt消息处理
  /**********u8g2***********/
  u8g2.begin();  //u8g2库初始化
  u8g2.enableUTF8Print(); //enable UTF8 support for the Arduino print() function
//  http.begin(wifiClient,ApiUrl+"?uid="+uid+"&title="+title+"&msg="+wxmsg);  //初始化请求
}

void loop() {
  if (!mqttclient.connected()){//判断mqtt是否连接
    reconnect();
  }
  mqttclient.loop();  //保持MQTT客户端心跳
  
  /***温湿度***/
  temphumiValue();
  //int intTemp = (int)temp;
  String strTemp = (String)temp;
  int intHumi = (int)humi;
  String strHumi = (String)intHumi;
  
  /****烟雾*****/
  mqData = analogRead(analogPin);//读取A0模拟引脚的数值
  String strsmoke = (String)mqData;
  if(mqData>300){
    Serial.println("可燃气泄露！！！");
    title = "可燃气状况";        // 设备名称
    wxmsg = "可燃气泄露！！！";   //发送的消息
    digitalWrite(LED, HIGH);
    doHttpStick();
    buzzerAlarm();
  }
  /***火焰检测***/
  fv = FlameValue();
  String staFire;
  if(fv){
    Serial.println("无火...");
    staFire = "nofire";
  }else{
    Serial.println("有火!!!");
    staFire = "fire";
    
    title = "火灾警报";   // 设备名称
    wxmsg = "有火！！！";   //发送的消息
    digitalWrite(LED, HIGH);
    doHttpStick();
    buzzerAlarm();
  }
  /***超声波测距***/
  cm = calculateDistance();
  double dis = (double)cm;
  int intDis = (int)dis;
  String strDis = (String)intDis;
  if(intDis > 4){
    title = "非法闯入警报";   // 设备名称
    wxmsg = "家门被打开！！！";   //发送的消息
    doHttpStick();
    digitalWrite(LED, HIGH);
    buzzerAlarm();
  }
  
  //红外模块监测是否有人靠近
  String staMan;
  if(pirVal){
    Serial.println("有人");
    staMan = "Someone";
    title = "家中人体红外感应";                  // 设备名称
    wxmsg = "检测家中人员活动，请立即确认！！！";   //发送的消息
    digitalWrite(LED, HIGH);
    doHttpStick(); //微信消息推送
    buzzerAlarm();
  }else{
    Serial.println("无人");
    staMan = "Nobody";
  }
  /****OLEDDispaly传参*****/
  OLEDDispaly(strTemp, strHumi, strsmoke, staMan, intDis, staFire);
  //OLEDDispaly(String temp, String rh, String gas, String human,int Dis,String fire)
  
  long now = millis();//获取当前时间戳
  if (now - lastMsg > timeval) {//如果达到3s，进行数据上传
    lastMsg = now;
    Serial.print("当前温度 (℃): ");
    Serial.println(temp,2);
    Serial.print("当前湿度 (%): ");
    Serial.println(humi,2);
    Serial.print("当前烟雾含量: ");
    Serial.println(strsmoke);
    Serial.print("测得距离为：");
    Serial.print(dis,2);
    Serial.println("cm");

    // 定义数据格式：#温度#湿度#烟雾#红外监测人#距离#火情#
    String  msg = "#"+strTemp + "#"+strHumi + "#"+strsmoke + "#"+staMan + "#"+strDis + "#"+staFire;
    mqttclient.publish(topic, msg.c_str());  //数据上传（向主题发布消息）
  }
  digitalWrite(buzzerPin, HIGH); //关闭蜂鸣器
  digitalWrite(LED, LOW);
  delay(3000);
}


/***WiFi连接函数***/
void connectWiFi() {//连接WiFi函数
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());//输出 IP
}
//callback回调函数
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String Mqtt_Buff = ""; //缓冲区字符串变量
  for (int i = 0; i < length; i++) {
    Mqtt_Buff += (char)payload[i]; //存储缓冲区数据
  }
  Serial.print(Mqtt_Buff);
  Serial.println();

  Mqtt_Buff = ""; //清空缓冲区字符串
}

//MQTT重连函数
void reconnect() {
  // 循环判断MQTT连接状态
  while (!mqttclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttclient.connect(ID_MQTT)) {  //连接mqtt服务端
      Serial.println("connected");
      mqttclient.subscribe(topic);      //订阅topic
    } else {
      //若连接失败，则打印connack报文中的returnCode(连接返回码)，具体见第一章 MQTT协议基础中的1-3
      Serial.print("failed, rc=");
      Serial.print(mqttclient.state());
      Serial.println(" try again in 5 seconds");
      // 在重新尝试连接MQTT之前等待5秒
      delay(5000);
    }
  }
}
/***温湿度返回函数***/
void temphumiValue(){
  DHT11.read(DHT11PIN); //更新DHT11传感器所有信息
  temp = DHT11.temperature; //获取温度
  humi = DHT11.humidity; //获取湿度
}

/*****火焰检测函数******/
int FlameValue(){//返回值为0-->有火
  int fvalue = digitalRead(FlamePIN);
  return fvalue;
}
/******超声测距函数******/
int calculateDistance(){
  // 传感器由 10 微秒或更多微秒的 HIGH 脉冲触发。
  // 预先给出一个短的 LOW 脉冲以确保一个干净的 HIGH 脉冲：
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(12);
  digitalWrite(trigPin, LOW);

 
  // 从传感器读取信号：一个高脉冲，其持续时间为从发出ping到接收到物体回波的时间（以微秒为单位）。
  duration = pulseIn(echoPin, HIGH);
 
  // 将时间转换为距离
  cm = (duration*0.034)/2;
  return cm;
}
/*************红外监测函数**********/
void hongwai(){
  pirVal = digitalRead(PIR_sensor);//返回红外模块引脚值
}

//oled屏显函数
void OLEDDispaly(String temp, String rh, String gas, String human,int dis,String fire){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.setCursor(0,15);
  u8g2.print("T:");
  u8g2.print(temp);
  u8g2.print("℃");
  
  u8g2.setCursor(68,15);
  u8g2.print("H:");
  u8g2.print(rh);
  u8g2.print("%");
  
  u8g2.setCursor(0,30);
  u8g2.print("Gas:");
  u8g2.print(gas);
  u8g2.print("%");
  
  u8g2.setCursor(56,30);
  u8g2.print("Man:");
  if(human=="Someone"){
    u8g2.print("yes!!!");
  }else{
    u8g2.print("no");
  }

  u8g2.setCursor(0,45);
  u8g2.print("Door:");
  if(dis>4){
    u8g2.print("opened !!!");
  }else{
    u8g2.print("closed");
  }
  
  u8g2.setCursor(0,60);
  u8g2.print("Fire:");
  u8g2.print(fire);
  u8g2.print("!");
  
  u8g2.sendBuffer();
}
//******微信消息推送函数********//
void doHttpStick(){  //微信消息推送函数
  if(WiFi.status() == WL_CONNECTED) { //检查WIFI是否连接
    //初始化请求
    http.begin(wifiClient2,ApiUrl+"?uid="+uid+"&title="+title+"&msg="+wxmsg);
    int httpCode = http.GET();   //发送GET请求  
    if (httpCode > 0) { // 检查返回的状态码
      String payload = http.getString();   //获取取响应体数据并且以字符串形式返回
      Serial.println(payload);             //打印响应信息
    }
    http.end();   //关闭http
  }else{
      connectWiFi();  //若WiFi断连则进行重连
  }
}
//**********蜂鸣器报警***********//
void buzzerAlarm(){
  digitalWrite(buzzerPin, LOW);//低电平触发
}
