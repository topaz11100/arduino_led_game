#include "Arduino.h"

#include "Game.h"

Entity::Entity(Vec design[], int n, const Vec& start, int color_input[]) {
	shape = new Vec[n];
	size = n;

	velocity = Vec{ random(-2,3), random(-2,3) };
	//while (velocity == Vec{0,0}) velocity = Vec{ random(-2,3), random(-2,3) };

	for (int i = 0; i < size; i += 1) shape[i] = design[i];
	for (int i = 0; i < 3; i += 1) color[i] = color_input[i];

	move(start);
}

Entity::Entity(const Entity& E) {
	velocity = E.velocity;
	size = E.size;
	
	for (int i = 0; i < 3; i += 1) color[i] = E.color[i];

	shape = new Vec[ E.size ];
	for (int i = 0; i < E.size; i += 1) shape[i] = E.shape[i];
}

Entity& Entity::operator=(const Entity& E) {
	velocity = E.velocity;
	size = E.size;
	
	for (int i = 0; i < 3; i += 1) color[i] = E.color[i];

	delete[] shape;
	shape = new Vec[E.size];
	for (int i = 0; i < E.size; i += 1) shape[i] = E.shape[i];
	return *this;
}

bool Entity::is_over() {
	for (int i = 0; i < size; i += 1) {
		if ( (0 <= shape[i].x) && (shape[i].x <= 15) &&
			 (0 <= shape[i].y) && (shape[i].y <= 15)   ) {
			return false;
		}
	}
	return true;
}

void Entity::move(const Vec& v) {
	for (int i = 0; i < size; i += 1) {
		shape[i] += v;
	}
}

bool collision(const Entity& a, const Entity& b) {
	for (int i = 0; i < a.get_size(); i += 1) {
		for (int j = 0; j < b.get_size(); j += 1) {
			if (a[i] == b[j]) return true;
		}
	}
	return false;
} 

Entity_vector::Entity_vector(const Entity_vector& E_v) {
	size = E_v.size;

	data = new Entity[E_v.size];
	for (int i = 0; i < size; i += 1) {
		data[i] = E_v.data[i];
	}
}
Entity_vector& Entity_vector::operator=(const Entity_vector& E_v) {
	size = E_v.size;

	delete[] data;
	data = new Entity[E_v.size];
	for (int i = 0; i < size; i += 1) {
		data[i] = E_v.data[i];
	}
}

int Entity_vector::overatfront() {
	for (int i = 0; i < size; i += 1) {
		if (data[i].is_over()) return i;
	}
	return -1;
}

void Entity_vector::push_back(const Entity& value) {
	int new_size = size + 1;
	Entity* new_data = new Entity[new_size];
	for (int i = 0; i < size; i += 1) {
		new_data[i] = data[i];
	}
	new_data[size] = value;
	delete[] data;
	data = new_data;
	size = new_size;
}

void Entity_vector::del(int index) {
	int new_size = size - 1;
	Entity* new_data = new Entity[new_size];
	for (int i = 0; i < index; i += 1) {
		new_data[i] = data[i];
	}
	for (int i = index + 1; i < size; i += 1) {
		new_data[i - 1] = data[i];
	}
	delete[] data;
	data = new_data;
	size = new_size;
}

void Entity_vector::clear() {
	delete[] data;
	data = nullptr;
	size = 0;
}

bool truenper(int n) {
	int x = random(1, 101);
	if (x <= n) return true;
	else return false;
}

int led_position(const Vec& v) {
	if (v.x < 0 || v.x > 15 || v.y < 0 || v.y > 15) return -1;
	int temp[2] = { 16 * v.x, 16 * v.x + 15 };
	if (v.x % 2 == 0) return temp[1] - v.y;
	else              return temp[0] + v.y;
}
void led_print(Adafruit_NeoPixel* led, const Entity& a) {
	for (int i = 0; i < a.get_size(); i += 1) {
		int temp = led_position(a[i]);
		if (temp == -1) continue;
		led->setPixelColor(temp, a.get_color()[0], a.get_color()[1], a.get_color()[2]);
	}
}

void Game_Manager::init(Game_base* g[], int s, Adafruit_NeoPixel* le, LiquidCrystal_I2C* lc, Joystick* j[]) {
	games = g;
	size = s;
	led = le; lcd = lc; joy[0] = j[0]; joy[1] = j[1];

	for (int i = 0; i < s; i += 1) {
		games[i]->init(led, lcd, joy);
	}
}

int Game_Manager::validnum(int n1, int n2) {
	int result = n1 + n2;
	if (result < 0) return 0;
	else if (result >= size) return size - 1;
	else return result;
}
int Game_Manager::game_select() {
	lcd_print(*lcd, make_space("push to select", 16, 2), 0, 0);
	lcd_print(*lcd, "joystick neutral", 0, 1);
	for (int i = 0; i < 2; i += 1) joy[i]->joystick_neutral();

	int num = 0;
	while (true) {
		if (joy[0]->z_read()) return num;
		num = validnum(num, joy[0]->y_triread());
		lcd_print(*lcd, make_space(games[num]->get_name(), 16, 2), 0, 1);
	}
}

void Game_Manager::game_play() {
	clear_led();

	int game_num = game_select();
	games[game_num]->start();
	
	while (games[game_num]->frame()) {
		games[game_num]->playing_print();
		Serial.println(freeMemory());
	}

	games[game_num]->end();

	lcd_print(*lcd, "push to continue", 0, 1);
	while (!joy[0]->z_read()) {}
}

void Game_base::print() {
	led->clear();
	for (int i = 0; i < now_entity.get_size(); i += 1) {
		led_print(led, now_entity[i]);
	}
	led->show();
}

//´åÁö

void Game_1::player_move() {
	Vec temp{ joy[0]->x_triread(), joy[0]->y_triread()};
	now_entity[0].move(temp);
}

void Game_1::create_enemy() {
	Vec enemy_start{ random(0,16),random(0,16) };
	while ( (3 <= enemy_start.x && enemy_start.x <= 12) ||
			(3 <= enemy_start.y && enemy_start.y <= 12)   ) {
		enemy_start.set(random(0,16),random(0,16));
	}
	now_entity.push_back(Entity{ enemy_shape, 1, enemy_start, enemy_color});
}

void Game_1::enemy_move() {
	for (int i = 1; i < now_entity.get_size(); i += 1) {
		now_entity[i].move(now_entity[i].get_v());
	}
}

bool Game_1::del_over() {
	while (true) {
		int index = now_entity.overatfront();
		if      (index ==  0) return true;
		else if (index == -1) return false;
		now_entity.del(index);
	}
}

bool Game_1::collision_check() {
	for (int i = 1; i < now_entity.get_size(); i += 1) {
		if ( collision( now_entity[0], now_entity[i] ) ) return true;
	}
	return false;
}

void Game_1::start() {
	Vec player_start{ random(7,9), random(7,9) };
	now_entity.push_back(Entity{ player_shape, 4, player_start, player_color });
}

bool Game_1::frame() {
	if (truenper(100)) create_enemy();
	player_move(); enemy_move();

	print();
	delay(150);

	game_score += 1;

	if (del_over()) return false;
	return !collision_check();
}

void Game_1::end() {
	lcd_print(*lcd, make_space("score : " + String(game_score), 16, 2), 0, 0);
	game_score = 0;
	now_entity.clear();
}

void Game_1::playing_print() {
	lcd_print(*lcd, "playing    score", 0, 0);
	lcd_print(*lcd, "dodge      " + String(game_score), 0, 1);
}

//Æþ

void Game_2::start() {
	Vec p1_start{ 0, 6 };
	now_entity.push_back(Entity{ p_shape, 4, p1_start, p1_color });
	Vec p2_start{ 15, 6 };
	now_entity.push_back(Entity{ p_shape, 4, p2_start, p2_color });

	Vec ball_start{ random(7,9), random(7,9) };
	now_entity.push_back(Entity{ ball_shape, 1, ball_start, ball_color });
	
	ball_cat = 0;
	ball_flip = random(0, 4);
}

void Game_2::player_move() {
	for (int i = 0; i < 2; i += 1) {
		now_entity[i].set_v(Vec{ 0, joy[i]->y_triread()});
		now_entity[i].move(now_entity[i].get_v());
	}

	for (int i = 0; i < now_entity[0].get_size(); i += 1) {
		if		(now_entity[0][i].y < 0)  now_entity[0][i] = Vec{ 0,15 };
		else if (now_entity[0][i].y > 15) now_entity[0][i] = Vec{ 0,0 };
		if		(now_entity[1][i].y < 0)  now_entity[1][i] = Vec{ 15,15 };
		else if (now_entity[1][i].y > 15) now_entity[1][i] = Vec{ 15,0 };
	}
}

void Game_2::ball_move() {
	now_entity[2].move( ball_route[ball_cat][ball_flip] );
	player_collision();
	wall_collision();
}

void Game_2::change_ball(int n) {
	if (ball_cat == 0) {
		ball_cat += 1;
		ball_flip = 1 + n - joy[n]->y_triread();
	}
	else if (ball_cat == 1) {
		int temp = (n == 0) ? 2 : 1;
		ball_cat += (temp - ball_flip) * joy[n]->y_triread();
		ball_flip += (n == 0) ? -1 : 1;
	}
	else if (ball_cat == 2) {
		int temp = (n == 0) ? 2 : 1;
		if ((temp - ball_flip) * joy[n]->y_triread() < 0) ball_cat -= 1;
		ball_flip += (n == 0) ? -1 : 1;
	}
}

void Game_2::player_collision() {
	for (int i = 0; i < 2; i += 1) {
		if (collision(now_entity[i], now_entity[2])) {
			change_ball(i);
		}
	}
}

void Game_2::wall_collision() {
	if (now_entity[2][0].x == 0 || now_entity[2][0].x == 15) return;

	if		(now_entity[2][0].y >= 15) { print(); ball_flip += 2; }
	else if (now_entity[2][0].y <= 0)  { print(); ball_flip -= 2; }
}

int Game_2::ball_gameover_check() {
	if		(now_entity[2][0].x > 15) return 1;
	else if (now_entity[2][0].x < 0)  return 2;
	else return 0;
}

bool Game_2::frame() {
	player_move();
	ball_move();
	print();
	delay(150);
	win_user = ball_gameover_check();
	if (win_user) return false;
	return true;
}

void Game_2::end() {
	lcd_print(*lcd, make_space("player " + String(win_user) + " win!", 16, 2), 0, 0);
	win_user = 0;
	now_entity.clear();
}

void Game_2::playing_print() {
	lcd_print(*lcd, make_space("playing", 16, 2), 0, 0);
	lcd_print(*lcd, make_space("pong", 16, 2), 0, 1);
}






                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             