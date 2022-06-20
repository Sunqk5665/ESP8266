//#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,/* reset=*/ U8X8_PIN_NONE);//构造器

void setup() {
  // put your setup code here, to run once:
  u8g2.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  u8g2.clearBuffer();         // 清除内部缓冲区
  u8g2.drawTriangle(20,5, 27,50, 5,32);
//  u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
//  u8g2.drawStr(0,10,"Hello World!");  // write something to the internal memory
//  u8g2.drawStr(0,20,"This is Sunqker!");  // write something to the internal memory
//  u8g2.setFont(u8g2_font_wqy12_t_gb2312);
//  u8g2.drawStr(0,40,"一二三");
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(1000);
}
