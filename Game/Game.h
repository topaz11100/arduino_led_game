#ifndef Game_h
#define Game_h

#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include "lcd_helper.h"
#include "base_part.h"
#include "MemoryFree.h"

//게임 제작 위한 자료구조 ----------------------------------------------------------------------
struct Vec
{
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

class Entity
{
public:
	Entity() :shape{nullptr} {}
	Entity(Vec design[], int n, const Vec& start, int color_input[]);
	Entity(const Entity& E);
	Entity& operator=(const Entity& E);
	void move(const Vec& v);
	
	~Entity() { delete[] shape; }

	const Vec& get_v() const { return velocity; }
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

class Entity_vector
{
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
//게임 제작 위한 자료구조 ----------------------------------------------------------------------

//확률 ---------------------------------------------------------------------------------------
bool truenper(int n);
//확률 ---------------------------------------------------------------------------------------

//led매트릭스 제어 ----------------------------------------------------------------------------
int led_position(const Vec& v);
void led_print(Adafruit_NeoPixel* led, const Entity& a);
//led매트릭스 제어 ----------------------------------------------------------------------------

//게임 관리자 --------------------------------------------------------------------------------
class Game_Manager
{
public:
	Game_Manager() : led{ nullptr }, lcd{ nullptr } {}
	void init(Game_base* g[], int s, Adafruit_NeoPixel* le, LiquidCrystal_I2C* lc, Joystick* j[]);
	~Game_Manager() { delete[] games; }

	int game_select();
	void game_play();

private:
	int validnum(int n1, int n2);
	void clear_led() { led->clear(); led->show(); }

	Game_base** games = nullptr;
	int size;

	Adafruit_NeoPixel* led = nullptr;
	LiquidCrystal_I2C* lcd = nullptr;
	Joystick* joy[2] = { nullptr, nullptr };
};
//게임 관리자 --------------------------------------------------------------------------------

//게임 공통조상 클래스 --------------------------------------------------------------------------------
class Game_base
{
public:
	virtual void start() {}
	virtual bool frame() {}
	virtual void end() {}

	virtual void playing_print() {}
	virtual String get_name() {}

	void init(Adafruit_NeoPixel* le, LiquidCrystal_I2C* lc, Joystick* j[]) {
		led = le; lcd = lc; joy[0] = j[0]; joy[1] = j[1];
	}

protected:
	Game_base() {}
	void print();

	Entity_vector now_entity;

	Adafruit_NeoPixel* led = nullptr;
	LiquidCrystal_I2C* lcd = nullptr;
	Joystick* joy[2] = { nullptr, nullptr };
};
//게임 공통조상 클래스 --------------------------------------------------------------------------------

//닷지 게임 --------------------------------------------------------------------------------
class Game_1 : public Game_base
{
public:
	void start();
	bool frame();
	void end();

	void playing_print();
	String get_name() { return game_name; }

private:
	void player_move();
	void create_enemy();
	void enemy_move();
	bool del_over();
	bool collision_check();

	int game_score = 0;

	String game_name = "dodge";

	Vec player_shape[4] = { Vec{-1,0}, Vec{0,1}, Vec{1,0}, Vec{0,-1} };
	int player_color[3] = { 0,255,255 };

	Vec enemy_shape[1] = { Vec{0,0} };
	int enemy_color[3] = { 255,0,255 };
};
//닷지 게임 --------------------------------------------------------------------------------

//퐁 게임 --------------------------------------------------------------------------------
class Game_2 : public Game_base
{
public:
	void start();
	bool frame();
	void end();

	void playing_print();
	String get_name() { return game_name; }

private:
	void player_move();
	void ball_move();
	void change_ball(int n);
	void player_collision();
	void wall_collision();
	int ball_gameover_check();
	
	int win_user = 0;

	String game_name = "pong";

	Vec p_shape[4] = { Vec{0,0}, Vec{0,1}, Vec{0,2}, Vec{0,3} };
	int p1_color[3] = { 0,255,255 };
	int p2_color[3] = { 255,0,255 };

	Vec ball_shape[1] = { Vec{0,0} };
	int ball_color[3] = { 0,255,0 };

	int ball_cat = 0;
	int ball_flip = 0;
	Vec ball_route[3][4] = { { Vec{ 1, 0 }, Vec{ -1, 0 }, Vec{ 1, 0 }, Vec{ -1, 0 } },
							 { Vec{ 1, 1 }, Vec{ -1, 1 }, Vec{ 1,-1 }, Vec{ -1,-1 } },
							 { Vec{ 1, 2 }, Vec{ -1, 2 }, Vec{ 1,-2 }, Vec{ -1,-2 } } };
};
//퐁 게임 --------------------------------------------------------------------------------


#endif
