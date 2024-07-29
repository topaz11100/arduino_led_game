#include <Game.h>

const int led_pin = 10;
const int led_size = 256;

Adafruit_NeoPixel led(led_size, led_pin, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(9600);
  led.begin();         
  led.show();            
  led.setBrightness(1); // 밝기를 설정합니다. (최대값 : 255)
}
void loop() {
  Adafruit_NeoPixel& a = led;
  Adafruit_NeoPixel& b = a;
  b.setPixelColor(10, 255,0,255);
  a.show();
}
