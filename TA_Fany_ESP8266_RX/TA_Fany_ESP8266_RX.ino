#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

// blynk setup
#define BLYNK_TEMPLATE_ID "TMPL6YTV7K2Fx"
#define BLYNK_TEMPLATE_NAME "Projectfany"
#define BLYNK_AUTH_TOKEN "ceHz4-_dx_-QGfBzCLnMU6xFid8Pnvcm"
#define BLYNK_PRINT Serial

char ssid[] = "BUSOL";
char pass[] = "$ut0h0m312";

//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D2, D3);
BlynkTimer timer;

float dtAir, dtSmoke, dtTemp, dtHumd;

void readData() {
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(nodemcu);

  if (data == JsonObject::invalid()) {
    //Serial.println("Invalid Json Object");
    jsonBuffer.clear();
    return;
  }

  Serial.println("JSON Object Recieved Success");

  Serial.print("Read CO2 Sensor: ");
  dtAir = data["data_1"];
  Serial.println(dtAir);
  Serial.print("Read Smoke Sensor: ");
  dtSmoke = data["data_2"];
  Serial.println(dtSmoke);
  Serial.print("Read Temperature Sensor: ");
  dtTemp = data["data_3"];
  Serial.println(dtTemp);
  Serial.print("Read Humdity Sensor: ");
  dtHumd = data["data_4"];
  Serial.println(dtHumd);
  Serial.println("-----------------------------------------");
}

void sendSensor() {
  readData();
  Blynk.virtualWrite(V0, dtAir);
  Blynk.virtualWrite(V1, dtSmoke);
  Blynk.virtualWrite(V2, dtTemp);
  Blynk.virtualWrite(V3, dtHumd);
}

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  nodemcu.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);

  timer.setInterval(1000L, sendSensor);
}

void loop() {
  Blynk.run();
  timer.run();
  readData();
}
