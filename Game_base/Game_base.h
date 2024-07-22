#ifndef Game_base_h
#define Game_base_h

#include "Arduino.h"

#include "Adafruit_NeoPixel.h"
#include "Parts_base.h"

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
	Entity() {}
	Entity(Vec design[], int n, const Vec& start, int color_input[], String t) { init(design, n, start, color_input, t); }
	void init(Vec design[], int n, const Vec& start, int color_input[], String t);
	~Entity() { delete[] shape; }

	Vec get_v() const { return velocity; }
	void set_v(const Vec& v) { velocity = v; }

	int get_size() const { return size; }
	String get_tag() const { return tag; }
	const int* get_color() const { return color; }
	const Vec& operator[](int index) { return shape[index]; }

	bool is_over();
	void move(Vec v);
private:
	Vec* shape;
	Vec velocity;
	int size;
	String tag;
	int color[3];
};
bool collision(const Entity& a, const Entity& b);

class Entity_vector {
public:
	Entity_vector() : data(nullptr), capacity(0), size(0) {}
	~Entity_vector() { delete[] data; }
	
	Entity& operator[](int index) { return data[index]; }
	int get_size() const { return size; }

	int overatfront();

	void push_back(const Entity& value);
	void del(int index);

private:
	Entity* data;
	int capacity;
	int size;
	void resize(int new_capacity);
};

bool truenper(int n);



#endif
