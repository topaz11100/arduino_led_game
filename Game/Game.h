#ifndef Game_h
#define Game_h

#include "Arduino.h"

#include "Parts_base.h"

#include "Adafruit_NeoPixel.h"

#include "MemoryFree.h"

struct Vec {
	Vec() :x{ 0 }, y{ 0 } {}
	Vec(int xx, int yy) :x{ xx }, y{ yy } {}
	void set_x(int xx) { x = xx; }
	void set_y(int yy) { y = yy; }
	void set(int xx, int yy) { set_x(xx); set_y(yy); }
	
	int x;
	int y;

	Vec& operator+=(const Vec& u) { x += u.x; y += u.y; return *this; }
	Vec& operator-=(const Vec& u) { x -= u.x; y -= u.y; return *this; }
	Vec& operator*=(int a)        { x *= a; y *= a; return *this; }
	Vec  operator+ (const Vec& u) { return Vec{ x + u.x, y + u.y }; }
	Vec  operator- (const Vec& u) { return Vec{ x - u.x, y - u.y }; }
	Vec  operator* (int a)        { return Vec{ x * a, y * a }; }
	bool operator==(const Vec& u) { return (x == u.x) && (y == u.y); }
	bool operator!=(const Vec& u) { return !((*this) == u); }
};

class Entity {
public:
	Entity() :shape{nullptr} {}
	Entity(Vec design[], int n, const Vec& start, int color_input[]);
	Entity(const Entity& E);
	Entity& operator=(const Entity& E);
	void move(const Vec& v);
	
	~Entity() { delete[] shape; }

	Vec get_v() const { return velocity; }
	void set_v(const Vec& v) { velocity = v; }

	int get_size() const { return size; }

	int* get_color() const { return color; }

	Vec& operator[](int index) { return shape[index]; }

	bool is_over();
	
private:
	Vec* shape;
	Vec velocity;
	int size;
	
	int color[3];
};

bool collision(const Entity& a, const Entity& b);

class Entity_vector {
public:
	Entity_vector() : data{ nullptr }, size{ 0 } {}
	Entity_vector(const Entity_vector& E_v);
	Entity_vector& operator=(const Entity_vector& E_v);
	~Entity_vector() { delete[] data; }
	
	Entity& operator[](int index) { return data[index]; }
	int get_size() const { return size; }

	int overatfront();

	void push_back(const Entity& value);
	void del(int index);
	void clear();

private:
	Entity* data;
	int size;
};

bool truenper(int n);

int led_position(const Vec& v);
void led_print(Adafruit_NeoPixel* led, const Entity& a);

class Game_base {
public:
	virtual void start() {}
	virtual bool frame() {}
	virtual void reset() {}

	virtual String get_name() {}

	void init(Adafruit_NeoPixel* le, Joystick* j1, Joystick* j2) {
		led = le; joy1 = j1; joy2 = j2;
	}

protected:
	Game_base() {}
	void print();

	Entity_vector now_entity;

	Adafruit_NeoPixel* led = nullptr;
	Joystick* joy1 = nullptr;
	Joystick* joy2 = nullptr;
};

class Game_Manager {
public:
	Game_Manager() : lcd{ nullptr }, joy1{ nullptr }, joy2{ nullptr } {}
	void init(Game_base* g[], int s, Adafruit_NeoPixel* le, LiquidCrystal_I2C* lc, Joystick* j1, Joystick* j2);
	~Game_Manager() { delete[] games; }

	int game_select();
	void game_play();

private:
	int validnum(int n1, int n2);

	Game_base** games;
	int size;

	LiquidCrystal_I2C* lcd;
	Joystick* joy1;
	Joystick* joy2;
};

class Game_1 : public Game_base {
public:
	void start();
	bool frame();
	void reset();
	
	String get_name() { return game_name; }

private:
	void player_move();
	void create_enemy();
	void enemy_move();
	bool del_over();
	bool collision_check();

	String game_name = "dodge";

	Vec player_shape[4] = { Vec{-1,0}, Vec{0,1}, Vec{1,0}, Vec{0,-1} };
	int player_color[3] = { 0,255,255 };

	Vec enemy_shape[1] = { Vec{0,0} };
	int enemy_color[3] = { 255,0,255 };
};


#endif
