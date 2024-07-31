#include <Game.h>

Adafruit_NeoPixel led{256, 10, NEO_GRB + NEO_KHZ800};

LiquidCrystal_I2C lcd{0x27, 16, 2};

Joystick joy1;
Joystick joy2;
const int joy_pin[2][3] = { {14, 15, 11}, {16, 17, 12} };
const int joy_sense = 200;

Game_1 g1;
Game_base* game_list[1] = { &g1 };
Game_Manager gm;

void setup()
{
  randomSeed(analogRead(A5));
  Serial.begin(9600);

  led.begin();         
  led.show();            
  led.setBrightness(1);

  lcd.init();
  lcd.backlight();

  joy1.init(joy_pin[0][0], joy_pin[0][1], joy_pin[0][2], joy_sense);
  joy2.init(joy_pin[1][0], joy_pin[1][1], joy_pin[1][2], joy_sense);

  gm.init(game_list, 1, &led, &lcd, &joy1, &joy2);
}

void loop()
{
  gm.game_play();
}














