#include "FastLED.h"
#include <Servo.h>
#include <SoftwareSerial.h>
//#include <Adafruit_Sensor.h>
#include <dht11.h>

#define NUM_LEDS 58
#define DATA_PIN 5//灯带接口
#define pushButton 2 //按钮接口1
#define pushButtonall 8 //接口按钮2
#define LightPin 4 //光线传感器接口
#define PIN_SERVO 3//舵机接口
const int GP2D12 = A0;       //红外端口（模拟）
const int sensorPin = A2;    //温湿度传感器接口
dht11 DHT;
Servo myservo; //舵机变量

//灯带设置
#define LED_TYPE WS2812
#define CLOUR_ORDER GRB
#define UPDATES_PER_SECOND 100
CRGB leds[NUM_LEDS];
CRGB Lightning(50, 10, 200);
CRGB Lightning1(200, 200, 200);
uint8_t max_bright = 255;

int pos = 0;//舵机初始角度
float val;//存储红外传感器读到的值
int allFlag0 = 1;//存储模式变量
int buttonStateall;//存储台灯档位



void setup() {
  Serial.begin(9600);
  delay(1000);
  myservo.attach(PIN_SERVO);  //定义舵机端口
  LEDS.addLeds<LED_TYPE, DATA_PIN, CLOUR_ORDER>(leds, NUM_LEDS);
}

void loop() {
  int buttonStateall = digitalRead(pushButtonall);
  if (buttonStateall == HIGH) {
        allFlag0++;
        if (allFlag0 == 6) {
          allFlag0 = 0;
        }
        delay(500);
        return;
      }
  switch (allFlag0) {
    case 0: break;
    case 1:
      Light();//小夜灯模式
      break;
    case 2:
      temperature();//温度计
      break;
    case 3:
      humidity();//湿度计
      break;
    case 4:
      table_lamp();//台灯模式
      break;
    case 5:
      detect_people();//互动模式
      break;
    default: break;
  }
}
void Light() {
  Serial.print("begin");
  int val2;
  val2 = analogRead(LightPin);  //connect grayscale sensor to Analog 0
  Serial.println(val2, DEC);    //print the value to serial
  delay(100);
  if (val2 < 60) {
    for (int t = 0; t <= 255; t += 10) {
      fill_solid(leds, 58, CRGB::Orange);
      FastLED.setBrightness(t);
      FastLED.show();
      delay(50);
      delay(100);
    }
    while (1) {
      fill_solid(leds, 58, CRGB::Orange);
      FastLED.setBrightness(255);
      FastLED.show();
      if (analogRead(LightPin) > 200)
        break;
    }
  }
  if (val2 > 200) {
    fill_solid(leds, 58, CRGB::Black);
    FastLED.show();
  }
}

void temperature() {
  if (DHT.temperature >= 20) {
    while (1) {
      for (int i = 0; i <= 43; i++) {
        fill_solid(leds, NUM_LEDS, CRGB(255, 163 - 2 * i, 23));
        FastLED.show();
        delay(100);
      }
    }
  }
}
void humidity() {
  if (DHT.humidity <= 10) {
    while (1) {
      for (int j = 0; j < 3; j++) {
        fill_solid(leds, NUM_LEDS, Lightning);
        FastLED.show();
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        delay(500 - 150 * j);
      }
      int i = random() % 30;
      int m = random() % 15;
      fill_solid(leds + i, m, Lightning);
      FastLED.show();
      delay(100);
      fill_solid(leds + i, m, CRGB::Black);
      FastLED.show();
      fill_solid(leds + 10, m + 3, Lightning1);
      FastLED.show();
      delay(100);
      fill_solid(leds + 10, m + 3, CRGB::Black);
      FastLED.show();
      fill_solid(leds + NUM_LEDS - i, m, CRGB::Blue);
      FastLED.show();
      delay(150);
      fill_solid(leds + NUM_LEDS - i, m, CRGB::Black);
      FastLED.show();
      fill_solid(leds + i + 20, 15, Lightning);
      FastLED.show();
      delay(150);
      fill_solid(leds + i + 20, 15, CRGB::Black);
      FastLED.show();
      fill_solid(leds + i - 10, 10, Lightning1);
      FastLED.show();
      delay(150);
      fill_solid(leds + i - 10, 10, CRGB::Black);
      FastLED.show();
      fill_solid(leds + 40 - i, 7, CRGB::Blue);
      FastLED.show();
      delay(150);
      fill_solid(leds + 40 - i, 7, CRGB::Black);
      FastLED.show();
    }
  }
}

void table_lamp() {
  int allFlag = 0;
  int buttonState = digitalRead(pushButton);  //读数字引脚2的状态
  //int outpurvalue=map(sensorValue,0,1023,0,255);
  if (buttonState == HIGH) {
    allFlag++;
    if (allFlag == 4) {
      allFlag = 0;
    }
  }
  delay(500);
  if (allFlag == 0)  //打开状态的分支
  {
    fill_solid(leds, 58, CRGB::Black);
    FastLED.show();
  }
  if (allFlag == 1) {
    fill_solid(leds, 58, CRGB(255, 163, 23));
    FastLED.setBrightness(35);
    FastLED.show();
  }
  if (allFlag == 2) {
    fill_solid(leds, 58, CRGB(255, 163, 23));
    FastLED.setBrightness(120);
    FastLED.show();
  }
  if (allFlag == 3) {
    fill_solid(leds, 58, CRGB(255, 163, 23));
    FastLED.setBrightness(255);
    FastLED.show();
  }
  if (buttonStateall == HIGH) {
    allFlag0++;
    if (allFlag0 == 6) {
      allFlag0 = 0;
    }
    delay(500);
    return;
  }
}

void detect_people() {
  val = 2547.8 / ((float)analogRead(GP2D12) * 0.49 - 10.41) - 0.42;  //计算人离云朵灯的距离
  if (10 < val && val < 80) {
    PersonComing();  //检测到有人进入特定范围，调用函数
  }
}

void PersonComing() {
  fill_solid(leds, 30, CRGB::Blue);  //设置灯带颜色
  FastLED.show();                    //点亮灯带
  //改变pos,控制舵机转动
  for (pos = 0; pos <= 180; pos += 1) {
    myservo.write(pos);
    delay(15);
  }
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
    delay(15);
  }
  fadeToBlackBy(leds, 30, 128);  //熄灭灯带
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}
