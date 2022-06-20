/*
 * 微信通知提醒
 * 2020-02-26
 * QQ交流群：824273231
 * 官网https://bemfa.com 
 *  * 注意：由于微信更新的原因，此版本可能停用，可在 https://cloud.bemfa.com/tcp/wechat.html 页面查看新接口教程
 */

#include <SoftwareSerial.h>
 
SoftwareSerial mySerial(13, 12); // RX, TX  通过软串口连接esp8266，


/******************************************************************************/
String ssid ="newhtc";                                       //WIFI名称

String password="qq123456";                                  //WIFI密码

String uid = "4d9ec352e0376f2110a0c601a28xxxx";             // 用户私钥，巴法云控制台获取
String type = "1";                                           // 1表示是预警消息，默认即可
String device = "人体红外传感器设备";                           // 设备名称
String msg = "检测到班主任已站在窗户边，立即放下手机假装学习";       //发送的消息
int delaytime = 0;   


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
 
  mySerial.begin(115200);
  mySerial.println("AT+RST");   // 初始化重启一次esp8266
  delay(1500);
  echo();
  mySerial.println("AT");
  echo();
  delay(500);
  mySerial.println("AT+CWMODE=3");  // 设置Wi-Fi模式
  echo();
  mySerial.println("AT+CWJAP=\"" +ssid+"\",\"" + password + "\"");  // 连接Wi-Fi
  echo();
  delay(10000);
}
 
void loop() {
 
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
  post();
}
 
void echo(){
  delay(50);
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}
 
void post(){
  String postData;
  //Post Data
  postData = "uid="+uid+"&type=" + type +"&time="+delaytime+"&device="+device+"&msg="+msg+"&msg2="+msg2;
  mySerial.println("AT+CIPMODE=1");
  echo();
  mySerial.println("AT+CIPSTART=\"TCP\",\"api.bemfa.com\",80");  // 连接服务器的80端口
  delay(1000);
  echo();
  mySerial.println("AT+CIPSEND"); // 进入TCP透传模式，接下来发送的所有消息都会发送给服务器
  echo();
  mySerial.print("POST /api/wechat/v1/"); // 开始发送post请求
  mySerial.print(" HTTP/1.1\r\nHost: api.bemfa.com\r\nContent-Type: application/x-www-form-urlencoded\r\nConnection:close\r\nContent-Length:"); // post请求的报文格式
  mySerial.print(postData.length()); // 需要计算post请求的数据长度
  mySerial.print("\r\n\r\n"); 
  mySerial.println(postData); // 结束post请求
  delay(3000);
  echo();
  mySerial.print("+++"); // 退出tcp透传模式，用println会出错
  delay(2000);
}
