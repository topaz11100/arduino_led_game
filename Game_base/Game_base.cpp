#include "Arduino.h"

#include "Game_base.h"

void Entity::init(Vec design[], int n, const Vec& start, int color_input[], String t) {
	shape = new Vec[n];
	size = n;
	tag = t;
	velocity = Vec{ random(-2,3), random(-2,3) };

	for (int i = 0; i < size; i += 1) shape[i] = design[i];
	for (int i = 0; i < 3;    i += 1) color[i] = color_input[i];

	move(start);
}

bool Entity::is_over() {
	for (int i = 0; i < size; i += 1) {
		if (shape[i].x <= 15 && shape[i].x >= 0 && shape[i].y <= 15 && shape[i].y >= 0) {
			return false;
		}
		return true;
	}
}

void Entity::move(Vec v) {
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

void Entity_vector::push_back(const Entity& value) {
	if (size == capacity) {
		int new_capacity = (capacity == 0) ? 1 : capacity * 2;
		resize(new_capacity);
	}
	data[size] = value;
	++size;
}

void Entity_vector::resize(int new_capacity) {
	Entity* new_data = new Entity[new_capacity];
	for (int i = 0; i < size; ++i) {
		new_data[i] = data[i];
	}
	delete[] data;
	data = new_data;
	capacity = new_capacity;
}

void Entity_vector::del(int index) {
	for (int i = index; i < size - 1; i += 1) {
		data[i] = data[i + 1];
	}
	size -= 1;
}

int Entity_vector::overatfront() {
	for (int i = 0; i < size; i+=1) {
		if (data[i].is_over()) return i;
	}
	return -1;
}

bool truenper(int n) {
	int x = random(1, 101);
	if (x <= n) return true;
	else return false;
}


