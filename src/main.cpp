#include "Arduino.h"
#include <Servo.h>
Servo esc;
bool has_display = false; //is there a display or not
volatile bool show_speed;
int speed_pin = 3;
int power_pin = 7;
volatile unsigned long last_time;
uint16_t circumference = 1000; //circumference of the wheel in mm
uint16_t damping_factor = 2;
volatile double speed;
volatile unsigned long prev_show_time;
volatile bool motor_state;
volatile double motor_speed;
double max_speed = 15;

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
  uint16_t outval = map((int) motor_speed, 0, (int) max_speed, 0, 179);
  return outval;
}

void power_motor() {
  motor_state = true;
  motor_speed = min(speed, max_speed);
  esc.write(get_esc_out());
}

void setup()
{
  esc.attach(9);
  pinMode(speed_pin, INPUT);
  pinMode(power_pin, INPUT);
  attachInterrupt(speed_pin, update_speed, RISING); // no digitalPinToInterrupt function
  attachInterrupt(power_pin, power_motor, RISING);
  last_time = 0;
  speed = 0;
  motor_state = false;
  show_speed = false;
  motor_speed = 0;
  display_speed();
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

