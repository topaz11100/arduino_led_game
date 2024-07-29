#ifndef Parts_base_h
#define Parts_base_h

#include "Arduino.h"

#include "Servo.h"

#include "LiquidCrystal_I2C.h"

class Joystick {
public:
    void init(int x, int y, int z, int s);
    void set_sense(int s) { sense = s; }
    int x_read() { return analogRead(xPin); }
    int x_triread();
    int y_read() { return analogRead(yPin); }
    int y_triread();
    bool z_read() { return !digitalRead(zPin); }
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
    const Servo& operator[](int n) { return sv[n]; }
    void attach(int pin[]);
    void move_arr(int angle[], int speed);
    void move_one(int n, int angle, int speed);
private:
    Servo* sv;
    int size;
    bool issame(int angle[]);
};

void lcd_print(LiquidCrystal_I2C& lcd, String str, int x, int y);

char receive_char(char error_char);

String receive_String(char terminal_char);

class Strip {
public:
    Strip(char c, int n) :s{ new String[n + 1] }, sep{ c }, sep_count{ n } {}
    void decomposition(String str);
    const String& operator[](int n) { return s[n]; }
    ~Strip() { delete[] s; }
private:
    String* s;
    char sep;
    int sep_count;
};

#endif
