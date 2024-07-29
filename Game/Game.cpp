#include "Arduino.h"

#include "Game.h"

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

void Entity::init(Vec design[], int n, const Vec& start, int color_input[]) {
	shape = new Vec[n];
	size = n;
	
	velocity = Vec{ random(-2,3), random(-2,3) };

	for (int i = 0; i < size; i += 1) shape[i] = design[i];
	for (int i = 0; i < 3;    i += 1) color[i] = color_input[i];

	move(start);
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

void Game_base::print() {
	led->clear();
	for (int i = 0; i < now_entity.get_size(); i += 1) {
		led_print(led, now_entity[i]);
	}
	led->show();
}

void Game_Manager::init(Game_base* g[], int s, Adafruit_NeoPixel* le, LiquidCrystal_I2C* lc, Joystick* j1, Joystick* j2) {
	games = new Game_base* [s];
	for (int i = 0; i < s; i += 1) {
		games[i] = g[i];
	}

	size = s;
	lcd = lc; joy1 = j1; joy2 = j2;

	for (int i = 0; i < s; i += 1) {
		games[i]->init(le, j1, j2);
	}
}

int Game_Manager::validnum(int n1, int n2) {
	int result = n1 + n2;
	if (result < 0) return 0;
	else if (result >= size) return size - 1;
	else return result;
}
int Game_Manager::game_select() {
	int num = 0;
	while (true) {
		if (joy1->z_read()) return num;
		num = validnum(num, joy1->y_triread());
		
		lcd_print(*lcd, "push to select", 0, 0);
		lcd_print(*lcd, games[num]->get_name(), 0, 1);
	}
}

void Game_Manager::game_play() {
	int game_num = game_select();
	
	while (games[game_num]->frame()) {
		lcd_print(*lcd, "     play game", 0, 0);
	}

	games[game_num]->reset();
	lcd_print(*lcd, "   game over", 0, 0);
	lcd_print(*lcd, "push to continue", 0, 1);

	while (!joy1->z_read()) {}
}

Game_1::Game_1() {
	now_entity.push_back(Entity{ player_shape, 4, player_start, player_color});
}

void Game_1::player_move() {
	Vec temp{ joy1->x_triread(), joy1->y_triread() };
	now_entity[0].move(temp);
}

void Game_1::create_enemy() {
	Vec a;
	int x = random(0, 4);
	if (x == 0)      a = Vec{ random(0, 1), random(0, 16) };
	else if (x == 1) a = Vec{ random(14,16), random(0, 16) };
	else if (x == 2) a = Vec{ random(0, 16), random(14,16) };
	else if (x == 3) a = Vec{ random(0, 16), random(0, 1) };
	now_entity.push_back(Entity{ enemy_shape, 1, a, enemy_color});
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
		if (collision(now_entity[0], now_entity[i])) return true;
	}
	return false;
}

void Game_1::reset() {
	now_entity.clear();
	now_entity.push_back(Entity{ player_shape, 4, player_start, player_color});
}

bool Game_1::frame() {
	if (truenper(100)) create_enemy();
	player_move();
	enemy_move();
	print();
	delay(200);

	Serial.println(freeMemory());

	if (del_over()) return false;
	bool result = !collision_check();
	return result;
}




                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             