#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 7

char temperatureF[6];

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

float tempC = 0;
float tempF = 0;

void setup() {
  // put your setup code here, to run once:
  sensors.begin();
  Serial.begin(9600);
  Serial.println("Dallas Temperature Test");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  sensors.requestTemperatures();
  
  Serial.println(sensors.getTempCByIndex(0));
  
  tempC = sensors.getTempCByIndex(0);
  tempF = sensors.toFahrenheit(tempC);
  
  convertToString(tempF);
  //convertToString(tempC);
}

void convertToString(float number)
{
  dtostrf(number, 3, 1, temperatureF);
}
