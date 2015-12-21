#include "Arduino.h"
int RXLED = 17;
void setup()
{
  pinMode(RXLED, OUTPUT);     // set pin as output
}

void loop()
{
  digitalWrite(RXLED, HIGH);  // set the LED on
  delay(1000);                  // wait for a second
  digitalWrite(RXLED, LOW);   // set the LED off
  delay(1000);                  // wait for a second
}
