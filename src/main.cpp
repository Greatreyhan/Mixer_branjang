#include <Arduino.h>
#include "max6675.h"
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define Pin Thermocouple
#define thermoDO 4
#define thermoCS 5
#define thermoCLK 6

// Instance of MAX6675
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

// Define potentiometer pin
#define potentiometer A3

// Define Buzz Sound
#define buzzer 7

// Define Motor Driver
#define EN_R 8
#define EN_L 9
#define PWM_R 11
#define PWM_L 10

// Define Button
#define Button 2

// Define Button State
volatile bool State = LOW;
const unsigned long DEBOUNCE_DELAY = 500; // Adjust this value as needed
volatile unsigned long lastDebounceTime = 0;

// Prototype
void state_motor();

void setup()
{
  // Initialize buzzzer
  pinMode(buzzer, OUTPUT);

  // Initialize BTS7960 Driver Motor
  pinMode(EN_L, OUTPUT);
  pinMode(EN_R, OUTPUT);
  pinMode(PWM_L, OUTPUT);
  pinMode(PWM_R, OUTPUT);

  // Initialize Button Arduino
  pinMode(Button, INPUT_PULLUP);
  // attachInterrupt(digitalPinToInterrupt(Button), state_motor, CHANGE);

  Serial.begin(9600);
  lcd.begin(2, 16);
  lcd.backlight();
}

void loop()
{
  // Read thermocouple data
  float temperature = thermocouple.readCelsius();

  // Turn Buzzer ON/OFF
  if (temperature > 50.0)
  {
    digitalWrite(temperature, HIGH);
  }
  else
  {
    digitalWrite(temperature, LOW);
  }

  // Control LCD Output
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T = ");
  lcd.setCursor(4, 0);
  lcd.print(temperature);
  lcd.setCursor(10, 0);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("SPD = ");
  lcd.setCursor(6, 1);

  // Read analog value
  int pot = analogRead(potentiometer);
  lcd.print(map(pot, 0, 1023, -10, 10));

  // Control Motor
  if (pot > 512)
  {
    analogWrite(PWM_R, map(pot, 512, 1023, 0, 255));
    analogWrite(PWM_L, 0);
  }
  if (pot < 512)
  {
    analogWrite(PWM_L, map(pot, 512, 0, 0, 255));
    analogWrite(PWM_R, 0);
  }

  if(!digitalRead(Button)) state_motor();

  // Delay for Thermocouple
  delay(200);
}

void state_motor()
{
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime >= DEBOUNCE_DELAY)
  {
    State = !State;
    // Enable Driver
    digitalWrite(EN_L, State);
    digitalWrite(EN_R, State);
    Serial.println(State);
    lastDebounceTime = currentTime;
  }
}