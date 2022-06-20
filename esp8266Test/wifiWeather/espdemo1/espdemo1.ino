#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <U8g2lib.h>   //U8g2库
#include <Wire.h>


//------------Wifi信息------//
const char* ssid = "mi";
const char* password = "11111111";
//-------------------------//
//api.seniverse.com/v3/weather/now.json?key=SbCTo-Zgsj2X_44Er&location=Xian&language=en&unit=c
const int httpPort = 80; //(http协议默认)端口号
const char* host = "api.seniverse.com"; //服务器地址

String reqUserKey = "SiPP_-AVxO1Cmi17r";//知心天气API私钥
String reqLocation = "Taian";//地址
String reqUnit = "c";//温度单位：摄氏度

//-------------------http请求-----------------------------//
String reqRes = "/v3/weather/now.json?key=" + reqUserKey +
                + "&location=" + reqLocation +
                "&language=en&unit=" + reqUnit;
String httprequest = String("GET ") + reqRes + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: close\r\n\r\n";


U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,/* reset=*/ U8X8_PIN_NONE);//构造器

//------------------------函数声明--------------------------------//
void OLEDDispaly(String cityName, String weather, String temperature);
void wifi_connect();
void httpRequest();
void parseJson(WiFiClient client);
//---------------------------------------------------------------//
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  u8g2.begin();  //初始化U8g2库
  wifi_connect();
}

void loop() {
  // put your main code here, to run repeatedly:
  httpRequest();
  delay(3000);
}

void wifi_connect() {
  //提示信息
  Serial.print("Wifi connecting");
  /*******绘制缓冲区内容到oled屏********/
  u8g2.clearBuffer();  //清除缓冲区
  u8g2.setFont(u8g2_font_unifont_t_chinese2);  //设置字体集
  u8g2.setCursor(0, 16);
  u8g2.print("Wifi connecting ...");
  u8g2.sendBuffer();
  /**********************************/
  //连接Wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { //判断当前的WiFi连接状态
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWifi Connected!");//WiFi连接成功
  
  u8g2.clearBuffer();
  u8g2.setCursor(0, 16);
  u8g2.print("Wifi connected!!!");
  u8g2.sendBuffer();
  
}

void httpRequest() {
  WiFiClient client;
  /**************连接服务器******************/
  if (client.connect(host, httpPort)) {   //服务器连接
    Serial.println("连接成功，接下来发送请求");
    client.print(httprequest);//访问API接口,(发送数据到已连接的服务器)
    String response_status = client.readStringUntil('\n');//从设备接收到的数据中读取信息(字符串)，即接收服务器返回的数据
    Serial.println(response_status);

    if (client.find("\r\n\r\n")) { //从设备接收到的数据中寻找指定字符串信息
      Serial.println("响应报文体找到，开始解析");
    }
    parseJson(client);  //数据解析-->
  }
  else {
    Serial.println("连接服务器失败");  //链接失败
  }
  client.stop();
}

void OLEDDispaly(String cityName, String weather, String temperature) {
  u8g2.clearBuffer();
  u8g2.setCursor(0, 16);
  u8g2.print("City: ");
  u8g2.print(cityName);
  //u8g2.print("济南");
  u8g2.setCursor(0, 36);
  u8g2.print("Weather: ");
  u8g2.print(weather);
  u8g2.setCursor(0, 52);
  u8g2.print("temperature: ");
  u8g2.print(temperature);
  //u8g2.print("度");
  u8g2.sendBuffer();
}

void parseJson(WiFiClient client) {
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 230;
  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, client);

  JsonObject obj1 = doc["results"][0];
  String cityName = obj1["location"]["name"].as<String>();
  String weather = obj1["now"]["text"].as<String>();
  String temperature = obj1["now"]["temperature"].as<String>();
  OLEDDispaly(cityName, weather, temperature);
  Serial.println(cityName);
  Serial.println(weather);
  Serial.println(temperature);
}
