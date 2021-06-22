#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int tempSensorPin = A0;
const int blueLedPin = 7;
const int greenLedPin = 8;
const int redLedPin = 9;

const float setTemp = 25.0;
const float setTempRange = 2.0;
const int tempBufferLength = 50;
float tempBuffer[tempBufferLength];
int tempBufferPosition = 0;

void setup() {
  for (int i = 0; i < tempBufferLength; i++) {
    tempBuffer[i] = readTemp();
  }

  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(redLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
}

void loop() {
  float temp = calcTemp();
  setLeds(temp);
  writeTemp(temp);
}

float calcTemp() {
  saveTempToBuffer(readTemp());
  float avgTemp = calcAvgTemp();

  return avgTemp;
}

void setLeds(float temp) {
  digitalWrite(blueLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);

  int activeLed = greenLedPin;
  if (temp < setTemp - setTempRange) {
    activeLed = blueLedPin;
  } else if (temp > setTemp + setTempRange) {
    activeLed = redLedPin;
  }

  digitalWrite(activeLed, HIGH);
}

void writeTemp(float temp) {
  lcd.setCursor(0, 0);
  lcd.print("Tc:     " + String(temp, 1));
  lcd.setCursor(0, 1);
  lcd.print("set Tc: " + String(setTemp, 1));
}

float readTemp() {
  int sensorVal = analogRead(tempSensorPin);
  float voltage = (sensorVal / 1024.0) * 5.0;
  float temp = (voltage - 0.5) * 100.0;

  Serial.print("Sensor value: " + String(sensorVal)
               + ", Volts: " + String(voltage, 3)
               + ", temperature: " + String(temp, 2) + "\n");

  return temp;
}

void saveTempToBuffer(float temp) {
  tempBuffer[tempBufferPosition] = temp;

  tempBufferPosition++;
  if (tempBufferPosition >= tempBufferLength) {
    tempBufferPosition = 0;
  }

  Serial.print("Buffer: [");
   for (int i = 0; i < tempBufferLength; i++) {
     Serial.print(String(tempBuffer[i],2) + ", ");
  }
  Serial.print("]\n");
}

float calcAvgTemp() {
  float tempSum = 0.0;
  for (int i = 0; i < tempBufferLength; i++) {
    tempSum += tempBuffer[i];
  }

  float avgTemp = tempSum / tempBufferLength;

  Serial.print(String(avgTemp) + "\n");

  return avgTemp;
}
