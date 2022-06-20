#define timeSeconds 2

// PIR OUTPUT端口 D6 GPIO12
const int PIR_sensor = 12;

// 获取当前时间
unsigned long now = millis();
// 上一次触发中断的时间
unsigned long lastTrigger = 0;
// 是否开启计时器的标志
boolean startTimer = false;
//  定时时间周期 2s
unsigned long detectDuration = 1000 * timeSeconds;

// 检测人体感应的中断触发函数，将LED置为高电平，且给开启计时器的标志startTimer置为true, 表示开始计时，
// 给lastTrigger赋值触发中断的时间
ICACHE_RAM_ATTR void detectHuman() {
  Serial.println("Human Detected!!!");
  digitalWrite(LED_BUILTIN, HIGH);
  startTimer = true;
  lastTrigger = millis();
}

void setup() {
  Serial.begin(115200);
  
  // 将人体感应数据口连接的IO口设置为输入上拉模式，使引脚即使在未连接外部组件的时候也能保持确定的逻辑电平。
  pinMode(PIR_sensor, INPUT_PULLUP);
  // 将人体感应数据口连接的IO口设置为中断模式, 赋值中断函数和将中断模式设置为RISING
  //（当引脚由低电平变为高电平时，触发中断）
  attachInterrupt(digitalPinToInterrupt(PIR_sensor), detectHuman, RISING);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // 获取当前时间
  now = millis();
  // 如果已经开始了计时，且当前时间减去上一次触发中断时间大于人体感应模块检测的时间周期 2s，将LED电平置为低，
  // 且将计时器标志置为false，等待下一次中断
  if(startTimer && (now - lastTrigger > detectDuration)) {
    Serial.println("Human detected stopped...");
    digitalWrite(LED_BUILTIN, LOW);
    startTimer = false;
  }
}
