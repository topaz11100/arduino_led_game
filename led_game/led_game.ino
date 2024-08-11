#include <Game.h>

const int led_pin = 7;
Adafruit_NeoPixel led{256, led_pin, NEO_GRB + NEO_KHZ800};

LiquidCrystal_I2C lcd{0x27, 16, 2};

Joystick* joy[2] = {new Joystick, new Joystick};
const int joy_pin[2][3] = { {A0, A1, 11}, { A2, A3, 10} };
const int joy_sense = 200;

const int game_count = 2;
Game_base* game_list[game_count] = { new Game_2, new Game_1 };
Game_Manager gm;

void setup()
{
  randomSeed(analogRead(A8));
  Serial.begin(9600);

  led.begin();
  led.show();
  led.setBrightness(1);

  lcd.init();
  lcd.backlight();

  for(int i=0; i<2; i+=1){
    joy[i]->init(joy_pin[i][0], joy_pin[i][1], joy_pin[i][2], joy_sense);
  }

  gm.init(game_list, game_count, &led, &lcd, joy);
}

void loop()
{
  gm.game_play();
}














