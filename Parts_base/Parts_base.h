#ifndef Parts_base_h
#define Parts_base_h

#include "Arduino.h"

#include "Servo.h"

#include "LiquidCrystal_I2C.h"

class Button {
public:
    void init(int p) { pin = p; pinMode(p, INPUT_PULLUP); }

    bool is_pushed() { return !digitalRead(pin); }
    void button_neutral() { while (true) { if (!is_pushed()) return; } }
private:
    int pin;
};

class Joystick {
public:
    void init(int x, int y, int z, int s);
    void set_sense(int s) { sense = s; }

    int x_read() { return 1023 - analogRead(xPin); }
    int x_triread();

    int y_read() { return 1023 - analogRead(yPin); }
    int y_triread();

    bool z_read() { return !digitalRead(zPin); }

    bool is_zero() { return !(x_triread() || y_triread() || z_read()); }
    void joystick_neutral() { while (true) { if (is_zero()) return; } }
private:
    int xPin;
    int yPin;
    int zPin;
    int sense;
};

class Ultrasonic {
public:
    void init(int trig, int echo);
    float distance();
private:
    int trigPin;
    int echoPin;
};

class Servo_vector {
public:
    Servo_vector(int s) : sv{ new Servo[s] }, size{ s } {}
    ~Servo_vector() { delete[] sv; }
    Servo& operator[](int n) { return sv[n]; }
    void attach(int pin[]);
    void move_arr(int angle[], int speed);
    void move_one(int n, int angle, int speed);
private:
    Servo* sv;
    int size;
    bool issame(int angle[]);
};

void lcd_print(const LiquidCrystal_I2C& lcd, const String& str, int x, int y);
String make_space(const String& str, int lcd_width, int mode = 0);

char receive_char(char error_char);
String receive_String(char terminal_char);

class Protocol {
public:
    Protocol(char c, int n) :s{ new String[n + 1] }, sep{ c }, sep_count{ n } {}
    void strip(String str);
    const String& operator[](int n) { return s[n]; }
    ~Protocol() { delete[] s; }
private:
    String* s;
    char sep;
    int sep_count;
};

#endif
