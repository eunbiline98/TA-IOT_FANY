#include "DHT.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <MQ135.h>

#define valve 9
#define fan 10
#define DHTPIN 2

const int sens_CO2 = A0;
const int sens_Smoke = A2;

#define DHTTYPE DHT11

float dtCO2, dtSmoke, dtTemp, dtHumd;
float SmokePercentage;
float t, h;

String stRuangan;

DHT dht(DHTPIN, DHTTYPE);
MQ135 mq135_sensor(sens_CO2);
SoftwareSerial nodemcu(5, 6);

void readDHT() {
  delay(2000);
  h = dht.readHumidity();
  t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
}

// MQ-2 Calibration
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void func() {
  readDHT();

  //MQ-135 Calibration
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(t, h);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(t, h);

  //MQ-2 Calibration
  SmokePercentage = mapfloat(analogRead(sens_Smoke), 30, 350, 0, 100);
  if (SmokePercentage >= 100) {
    SmokePercentage = 100;
  }
  if (SmokePercentage <= 0) {
    SmokePercentage = 0;
  }
  // data collect
  dtCO2 = correctedPPM;
  dtSmoke = SmokePercentage;
  dtTemp = t;
  dtHumd = h;


  if (dtCO2 >= 80 || dtSmoke >= 780 || dtTemp >= 30) {
    digitalWrite(fan, LOW);
    digitalWrite(valve, LOW);
  } else {
    digitalWrite(fan, HIGH);
    digitalWrite(valve, HIGH);
  }
  Serial.print("Read CO2 Sensor: ");
  Serial.println(dtCO2);
  Serial.print("Read Smoke Sensor: ");
  Serial.println(dtSmoke);
  Serial.print("Read Temperature Sensor: ");
  Serial.println(dtTemp);
  Serial.print("Read Humdity Sensor: ");
  Serial.println(dtHumd);
  Serial.print("===========================");
  Serial.println();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  nodemcu.begin(9600);
  dht.begin();

  pinMode(fan, OUTPUT);
  pinMode(valve, OUTPUT);

  digitalWrite(fan, HIGH);
  digitalWrite(valve, HIGH);

  Serial.println("Program started");
  delay(2000);
  Serial.println("Program ready...!!!");
}

void loop() {
  // put your main code here, to run repeatedly:

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();

  func();

  //Assign collected data to JSON Object
  data["data_1"] = dtCO2;
  data["data_2"] = dtSmoke;
  data["data_3"] = dtTemp;
  data["data_4"] = dtHumd;

  //Send data to NodeMCU
  data.printTo(nodemcu);
  jsonBuffer.clear();
  delay(2000);
}
