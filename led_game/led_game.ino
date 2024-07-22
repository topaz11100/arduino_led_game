#include <Game_base.h>

Adafruit_NeoPixel led(256, 10, NEO_GRB + NEO_KHZ800);

LiquidCrystal_I2C lcd(0x27, 16, 2);

Joystick joy[2];
const int joy_pin[2][3] = { {14, 15, 13}, {16, 17, 12}};
const int joy_sense = 200;

const Vec vec_move[9] = { Vec{-1, 1}, Vec{0, 1}, Vec{1, 1},
                          Vec{-1, 0}, Vec{0, 0}, Vec{1, 0},
                          Vec{-1,-1}, Vec{0,-1}, Vec{1,-1} };

void led_print(const Entity& a) {
  for (int i = 0; i < a.get_size(); i += 1){
    //위치 부분 추가할것
    led.setPixelColor(i, a.get_color()[0], a.get_color()[1], a.get_color()[2]);
  }
}

class Game_1 {
public:
	Game_1() {
		now_entity.push_back(Entity{ player_shape, 8, player_start, player_color, "player" });
	}
	void print() {
		led.clear();
		for (int i = 0; i < now_entity.get_size(); i += 1) led_print(now_entity[i]);
		led.show();
	}
	void player_move() {
		now_entity[0].move(Vec{ joy[0].x_triread(joy_sense), joy[0].y_triread(joy_sense) });
	}
	void create_enemy() {
		Vec a;
		int x = random(0, 4);
		if (x == 0)       a = Vec{ random(1), random(16) };
		else if (x == 1) a = Vec{ random(14,16), random(16) };
		else if (x == 2) a = Vec{ random(16), random(14,16) };
		else if (x == 3) a = Vec{ random(16), random(1) };
		now_entity.push_back(Entity{ enemy_shape, 1, a, enemy_color, "enemy" });
	}
	void enemy_move() {
		for (int i = 1; i < now_entity.get_size(); i += 1) {
			now_entity[i].move(now_entity[i].get_v());
		}
	}
	bool del_over() {
		while (true) {
			int index = now_entity.overatfront();
			if (index == 0) return true;
			if (index == -1) break;
			now_entity.del(index);
		}
		return false;
	}
	bool collision_check() {
		for (int i = 1; i < now_entity.get_size(); i += 1) {
			if (collision(now_entity[0], now_entity[i])) return true;
		}
		return false;
	}
	bool frame() {
		player_move();
		if (truenper(5)) create_enemy();
		enemy_move();
		print();
		if (del_over()) return false;
		return !collision_check();
	}

private:
	Vec player_shape[8] = { Vec{-1, 1}, Vec{0, 1}, Vec{1, 1},
							Vec{-1, 0},            Vec{1, 0},
							Vec{-1,-1}, Vec{0,-1}, Vec{1,-1} };
	Vec player_start = Vec{ 7, 7 };
	int player_color[3] = { 181, 178, 255 };

	Vec enemy_shape[1] = { Vec{0,0} };
	int enemy_color[3] = { 255, 0, 0 };

	Entity_vector now_entity;
};



void setup()
{
  Serial.begin(9600);
  randomSeed(analogRead(0));

  led.begin();         
  led.show();            
  led.setBrightness(255); // 밝기를 설정합니다. (최대값 : 255)

  lcd.init();
  lcd.backlight();

  joy[0].init(joy_pin[0][0], joy_pin[0][1], joy_pin[0][2]);
  joy[1].init(joy_pin[1][0], joy_pin[1][1], joy_pin[1][2]);

  Game_1 game_1;
}

void loop()
{
  
}














