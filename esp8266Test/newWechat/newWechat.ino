//esp8266头文件，需要先安装esp8266开发环境
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


/******************************************************************************/
#define DEFAULT_STASSID  "mi"                              //WIFI名称
#define DEFAULT_STAPSW   "11111111"                        //WIFI密码

String uid = "8a1350d0c73d254c726ae6dab79b4fc3";             // 用户私钥，巴法云控制台获取
String title = "你好，开发者";                           // 消息标题
String msg = "我很好，谢谢";                             //发送的消息
String ApiUrl = "http://go.bemfa.com/v1/sendwechatcard";        //默认 api 网址,http 默认端口80，https默认端口443

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
      if (WiFi.status() == WL_CONNECTED) { //检查WIFI是否连接
         
          http.begin(client,ApiUrl+"?uid="+uid+"&title="+title+"&msg="+msg);  //初始化请求
          int httpCode = http.GET();                                  //发送请求
       
          if (httpCode > 0) { // 检查状态码
       
            String payload = http.getString();   //获取响应信息
            Serial.println(payload);             //打印响应信息
          }
          http.end();   //关闭http
    }
  }
