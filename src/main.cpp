#include "Arduino.h"
bool has_display = false; //is there a display or not
volatile bool show_speed;
int speed_pin = 3;
int power_pin = 7;
int motor_relay_pin = 18;
int esc_pin = 10;
int led_pin = 17;
uint8_t off_level = 0;
uint8_t on_min_level = 96;
volatile unsigned long last_time;
uint16_t circumference = 1000; //circumference of the wheel in mm
uint16_t damping_factor = 2;
volatile double speed;
volatile unsigned long prev_show_time;
volatile bool motor_state;
volatile double motor_speed;
double max_speed = 10;
volatile bool button_ok = true;
void display_speed() {
  if (has_display) {
    prev_show_time = millis();
  }
}
void update_speed() {
  unsigned long new_time = millis();
  unsigned long diff = new_time - last_time;
  uint16_t new_speed = circumference/diff; //speed in mm per millisecond
  speed += new_speed / damping_factor; //LPF damping
  last_time = new_time;
  if (show_speed) {
    display_speed();
  }
}

int get_esc_out() {
  uint16_t outval = map((int) motor_speed, 0, (int) max_speed, 127, 130);
  return outval;
}

void power_motor() {
  digitalWrite(led_pin, HIGH);
  motor_state = true;
  motor_speed = min(speed, max_speed);
  //turn relay switch to motor
  digitalWrite(motor_relay_pin, HIGH);
  delay(100);
  analogWrite(esc_pin, 130);
}

void stop_motor() {
  digitalWrite(led_pin, LOW);
  motor_state = false;
  motor_speed = 0;
  //turn relay switch off.
  //I think this should be done immediately to start regen
  digitalWrite(motor_relay_pin, LOW);
  analogWrite(esc_pin, 100);
}
void test_fall() {
  if (button_ok) {
    button_ok = false;
    Serial.print("test_fall triggered...");
    analogWrite(esc_pin, 90);
    Serial.print("writing analog esc value...");
    Serial.println(millis());
  }
}
void test_rise() {
  Serial.println("test rise");
  if (!button_ok) {
    Serial.print("test_rise triggered...");
    button_ok = true;
    Serial.println(millis());
  }
}
void test_change() {
  Serial.println("test change");
  if (digitalRead(power_pin)) {
    if (!button_ok) {
      Serial.print("test_rise triggered...");
      analogWrite(esc_pin, 90);
      button_ok = true;
      Serial.println(millis());
    }
  } else {
    if (button_ok) {
      button_ok = false;
      Serial.print("test_fall triggered...");
      analogWrite(esc_pin, 130);
      Serial.print("writing analog esc value...");
      Serial.println(millis());
    }
  }
}
void setup()
{
  pinMode(speed_pin, INPUT);
  pinMode(power_pin, INPUT);
  pinMode(motor_relay_pin, OUTPUT);
  pinMode(esc_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
  digitalWrite(power_pin, HIGH);
  //digitalWrite(motor_relay_pin, LOW);
  digitalWrite(motor_relay_pin, HIGH);
  //digitalWrite(motor_relay_pin, LOW);
  analogWrite(esc_pin, 80);
  // If digitalPinToInterrupt breaks, here is the mapping:
  // p->i: 3->0
  attachInterrupt(digitalPinToInterrupt(speed_pin), update_speed, RISING);
  //attachInterrupt(digitalPinToInterrupt(power_pin), power_motor, RISING);
  //attachInterrupt(digitalPinToInterrupt(power_pin), stop_motor, FALLING);
  //attachInterrupt(4, test_rise, RISING);
  //attachInterrupt(4, test_fall, FALLING);
  attachInterrupt(4, test_change, CHANGE);
  last_time = 0;
  speed = 0;
  motor_state = false;
  show_speed = false;
  motor_speed = 0;
  display_speed();
  Serial.println("finished setup");
}

void loop()
{
  if (has_display) {
    unsigned long cur_time = millis();
    if (cur_time - prev_show_time >= 700) {
      show_speed = true;
    }
  }
}

