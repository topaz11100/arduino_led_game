#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal_I2C.h>

const int led_pin = 10;
const int led_size = 256;
 
Adafruit_NeoPixel led(led_size, led_pin, NEO_GRB + NEO_KHZ800);

int board[16][16];

struct Vec{
  Vec():x{0}, y{0} {}
  Vec(int xx, int yy):x{xx}, y{yy} {}
  void set_x(int xx) { x = xx; }
  void set_y(int yy) { y = yy; }
  void set(int xx, int yy) {set_x(xx); set_y(yy);}
  Vec& operator+=(const Vec& u){ x+=u.x; y+=u.y; return *this; }
  Vec& operator-=(const Vec& u){ x-=u.x; y-=u.y; return *this; }
  Vec& operator*=(int a){ x*=a; y*=a; return *this; }
  Vec operator+(const Vec& u){ return Vec{x + u.x, y + u.y}; }
  Vec operator-(const Vec& u){ return Vec{x - u.x, y - u.y}; }
  Vec operator*(int a){ return Vec{x * a, y * a}; }
  bool operator==(const Vec& u){ return (x == u.x)&&(y == u.y); }
  bool operator!=(const Vec& u){ return !((*this)==u); }
  int x;
  int y;
};

const Vec vec_move[9] = { Vec{-1, 1}, Vec{0, 1}, Vec{1, 1},
                          Vec{-1, 0}, Vec{0, 0}, Vec{1, 0},
                          Vec{-1,-1}, Vec{0,-1}, Vec{1,-1} };

class Entity{
public:
  Entity() {}
  Entity(Vec design[], int n, const Vec& start, int color_input[], String t): pos{new Vec[n]}, size{n}, tag{t} {
    for(int i=0; i<size; i+=1) pos[i] = design[i];
    for(int i=0; i<3; i+=1) color[i] = color_input[i];
    move(start);
  }
  void init(Vec design[], int n, const Vec& start, int color_input[], String t){
    pos = new Vec[n]; size = n; tag = t;
    for(int i=0; i<size; i+=1) pos[i] = design[i];
    for(int i=0; i<3; i+=1) color[i] = color_input[i];
    move(start);
  }
  ~Entity() {delete[] pos;}
  int get_size() const { return size; }
  String get_tag() const { return tag; }
  const int* get_color() const { return color; }
  const Vec& operator[](int i) const { return pos[i]; }
  void move(Vec v){ for (int i=0; i<size; i+=1) pos[i] += v; }
private:
  Vec* pos;
  int size;
  String tag;
  int color[3];
};

class Entvec {
public:
    Entvec() : data(nullptr), capacity(0), size(0) {}
    ~Entvec() { delete[] data; }
    void push_back(const Entity& value) {
        if (size == capacity) {
            int new_capacity = (capacity == 0) ? 1 : capacity * 2;
            resize(new_capacity);
        }
        data[size] = value;
        ++size;
    }
    Entity& operator[](int index) { return data[index]; }
    int get_size() const { return size; }
private:
    Entity* data;
    int capacity;
    int size;
    void resize(int new_capacity) {
        Entity* new_data = new Entity[new_capacity];
        for (int i = 0; i < size; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }
};

class Joy{
public:
  void init(int x, int y, int z){ xPin = x; yPin = y; zPin = z;
       pinMode(xPin, INPUT); pinMode(yPin, INPUT); pinMode(zPin, INPUT_PULLUP); }
  int x_read() { return analogRead(xPin); }
  int x_triread(int s){
    int temp = x_read();
    if      (temp < (512 - s)) return -1;
    else if (temp > (512 + s)) return 1;
    else return 0;
  }
  int y_read() { return analogRead(yPin); }
  int y_triread(int s){
    int temp = y_read();
    if      (temp < (512 - s)) return -1;
    else if (temp > (512 + s)) return 1;
    else return 0;
  }
  int z_read() { return digitalRead(xPin); }
private:
  int xPin;
  int yPin;
  int zPin;
};

Vec operator*(int a, const Vec& v){ return Vec{v.x * a, v.y * a}; }

void led_print(const Entity& a) {
  for (int i = 0; i < a.get_size(); i += 1){
    //위치 부분 추가할것
    led.setPixelColor(i, a.get_color()[0], a.get_color()[1], a.get_color()[2]);
  }
}

bool collision(const Entity& a, const Entity& b){
  for (int i = 0; i < a.get_size(); i += 1){
    for (int j = 0; j < b.get_size(); j += 1){ if(a[i] == b[j]) return true; }
  }
  return false;
}

class Game_1{
public:
  Game_1() { now_entity.push_back( Entity{player_shape, 8, player_start, player_color, "player"} ); };
  void print() {
    led.clear(); led.show();
    for(int i=0; i<now_entity.get_size(); i+=1) led_print(now_entity[i]);
    led.show();
  }
private:
  Vec player_shape[8] = { Vec{-1, 1}, Vec{0, 1}, Vec{1, 1}, 
                          Vec{-1, 0},            Vec{1, 0}, 
                          Vec{-1,-1}, Vec{0,-1}, Vec{1,-1} };
  Vec player_start = Vec{7, 7};
  int player_color[3] = {181, 178, 255};

  Vec enemy_shape[1] = { Vec{0,0} };
  int enemy_color[3] = {255, 0, 0};

  Entvec now_entity;
};

Joy joy[2];
const int joy_pin[2][3] = { {14, 15, 13}, {16, 17, 12}};


LiquidCrystal_I2C lcd(0x27,16,2);
void liquid_print(String str, int x, int y){
  lcd.noBacklight();
  lcd.setCursor(x, y);
  lcd.print(str);
  lcd.backlight();
}

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
}

void loop()
{
  
}

char receive_char(){
    if(Serial.available()) return Serial.read();
    else return '?';
}
String receive(){
  String result = "";
  if( !(Serial.available()) ) return result;
  while(true){
    char temp = receive_char();
    if( temp == '!' ) break;
    if( temp == '?' ) continue;
    result += String(temp);
  }
  return result;
}














