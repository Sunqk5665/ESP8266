
#include <ESP8266WiFi.h>               //默认，加载WIFI头文件
#include <PubSubClient.h>              //默认，加载MQTT库文件

//********************需要修改的部分*******************//
const char* ssid = "mi";           //修改，你的路由器WIFI名字
const char* password = "11111111";     //你的WIFI密码
#define ID_MQTT  "8a1350d0c73d254c726ae6dab79b4fc3"     //用户私钥，控制台获取
const char* topic = "bafaTopicTest01";        //主题名字，可在巴法云控制台自行创建，名称随意
//const int B_led = D2;       //单片机LED引脚值，D系列是NodeMcu引脚命名方式，其他esp8266型号将D2改为自己的引脚
//**************************************************//

const char* mqtt_server = "bemfa.com"; //默认，MQTT服务器
const int mqtt_server_port = 9501;     //默认，MQTT服务器端口号
WiFiClient espClient;                  //新建WiFiClient对象
PubSubClient client(espClient);        //建立PubSubClient对象

//灯光函数及引脚定义
void turnOnLed();
void turnOffLed();


void setup_wifi() {//连接WiFi函数
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

void callback(char* topic, byte* payload, unsigned int length) {//MQTT消息处理函数
  Serial.print("Topic:");
  Serial.println(topic);
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("Msg:");
  Serial.println(msg);
  if (msg == "on") {//如果接收字符on，亮灯
    turnOnLed();//开灯函数
  } else if (msg == "off") {//如果接收字符off，亮灯
    turnOffLed();//关灯函数
  }
  msg = "";
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID_MQTT)) {
      Serial.println("connected");
      Serial.print("subscribe:");
      Serial.println(topic);
      //订阅主题，如果需要订阅多个主题，可发送多条订阅指令client.subscribe(topic2);client.subscribe(topic3);
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);           //设置波特率115200
  pinMode(LED_BUILTIN, OUTPUT);   //设置引脚为输出模式
  digitalWrite(LED_BUILTIN, HIGH);//默认引脚上电高电平
  setup_wifi();                   //连接wifi
  client.setServer(mqtt_server, mqtt_server_port);//设置mqtt服务器
  client.setCallback(callback);   //mqtt消息处理
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

//打开
void turnOnLed() {
  Serial.println("turn on light");
  digitalWrite(LED_BUILTIN, LOW);
}
//关闭灯泡
void turnOffLed() {
  Serial.println("turn off light");
  digitalWrite(LED_BUILTIN, HIGH);
}
