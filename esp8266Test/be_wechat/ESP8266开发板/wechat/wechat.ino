/*
 * 微信通知提醒
 * 2021-3-26
 * QQ 1217882800
 * https://bemfa.com 
 * 
 * 注意：由于微信更新的原因，此版本可能失效，可在 https://cloud.bemfa.com/tcp/wechat.html 页面查看新接口教程
 */

//esp8266头文件，需要先安装esp8266开发环境
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>



/******************************************************************************/
#define DEFAULT_STASSID  "newhtc"                              //WIFI名称
#define DEFAULT_STAPSW   "qq123456"                        //WIFI密码

String uid = "4d9ec352e0376f2110a0c601a2857225";             // 用户私钥，巴法云控制台获取
String type = "2";                                           // 1表示是预警消息，2表示设备提醒消息
String device = "人体红外传感器设备";                           // 设备名称
String msg = "检测到班主任已站在窗户边，立即放下手机假装学习";       //发送的消息
int delaytime = 0;                                          //为了防止被设备“骚扰”，可设置贤者时间，单位是秒，如果设置了该值，在该时间内不会发消息到微信，设置为0立即推送。
String ApiUrl = "http://api.bemfa.com/api/wechat/v1/";        //默认 api 网址

/******************************************************************************/
static uint32_t lastWiFiCheckTick = 0;//wifi 重连计时
WiFiClient client;  //初始化wifi客户端
HTTPClient http;  //初始化http

//=======================================================================
//              WIFI重新连接函数
//=======================================================================
void startSTA(){
  WiFi.disconnect();//断开连接
  WiFi.mode(WIFI_STA);//设置wifi模式
  WiFi.begin(DEFAULT_STASSID, DEFAULT_STAPSW);// 开始连接
}


//=======================================================================
//              WIFI状态检测函数，如果WIFI断开自动重连
//=======================================================================
void doWiFiTick(){
    if ( WiFi.status() != WL_CONNECTED ) {//如果没连接
        if (millis() - lastWiFiCheckTick > 1000) { //未连接1s重连，检查是否大于1秒
          lastWiFiCheckTick = millis();
          startSTA();//重新连接
        }
      }
 }

//=======================================================================
//                    初始化
//=======================================================================

void setup() {
  delay(1000);
  Serial.begin(115200);     //设置串口波特率
  WiFi.mode(WIFI_OFF);        //设置wifi模式
  delay(1000);
  WiFi.mode(WIFI_STA);         //设置wifi模式
  
  WiFi.begin(DEFAULT_STASSID, DEFAULT_STAPSW);     //开始连接wifi
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {//检查是否连接成功
    delay(500);
    Serial.print(".");
  }

  //如果连接成功，打印ip等信息
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(DEFAULT_STASSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP 地址
}

//=======================================================================
//                    主循环
//=======================================================================
void loop() {


      doHttpStick();//在想推送消息的地方执行推送函数即可
      delay(20000);//20s推送一次，可删除delay,在想推送消息的地方执行推送函数即可
}


//******微信消息推送函数********//
void doHttpStick(){  //微信消息推送函数
  String postData;
  //Post Data
  postData = "uid="+uid+"&type=" + type +"&time="+delaytime+"&device="+device+"&msg="+msg;
  http.begin(client,ApiUrl);              //Specify request destination
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Specify content-type header
  int httpCode = http.POST(postData);   //Send the request
  String payload = http.getString();    //Get the response payload
  Serial.println(httpCode);   //Print HTTP return code
  Serial.println(payload);    //Print request response payload
  http.end();  //Close connection
  Serial.println("send success");  
  }
//=======================================================================
