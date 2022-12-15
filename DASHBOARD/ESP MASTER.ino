// MASTER CODE

#include <Wire.h>
#include <WiFi.h>
#include "esp_wpa2.h"
#include <ThingsBoard.h>


// Wifi
#define EAP_IDENTITY "Your Email"
#define EAP_PASSWORD "Your password"

const char* ssid = "eduroam";
WiFiClient espClient; //Initialize ThingsBoard client
int status = WL_IDLE_STATUS;
ThingsBoard client(espClient);


// send and receive data
#define SLAVE_ADDR 9
#define I2C_SDA 21
#define I2C_SCL 22
float temp = 25;
float ph = 3;
float stir = 500;

// char str_send[5];

// Thingsboard
// #define TOKEN "7aLsfYuYcCDlWFvp5BNk"
#define TOKEN "QNj0Cb8tQOCRJi7ulMi5"
#define THINGSBOARD_SERVER "engf0001.cs.ucl.ac.uk"
// #define THINGSBOARD_SERVER "thingsboard.cloud"


ThingsBoard tb(espClient);
static uint16_t messageCounter = 0;



// RPC
bool subscribed = false;
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

float SetTemperature = 25;
float SetPHv = 5;
int SetStirring = 500;
RPC_Response getTemperature(const RPC_Data &data)
{
  // Serial.println("Received the get Position Method");
  return RPC_Response(NULL, SetTemperature);
}

RPC_Response setTemperature(const RPC_Data &data)
{
  // Serial.print("Received the Set Position method: ");
  SetTemperature = data;
  // Serial.println(SetPosition);
  return RPC_Response(NULL, SetTemperature);
}


RPC_Response setStirring(const RPC_Data &data)
{
  // Serial.print("Received the Set Position method: ");
  SetStirring = data;
  // Serial.println(SetPosition);
  return RPC_Response(NULL, SetStirring);
}
RPC_Response getStirring(const RPC_Data &data)
{
  // Serial.println("Received the get Position Method");
  return RPC_Response(NULL, SetStirring);
}


RPC_Response setPHv(const RPC_Data &data)
{
  // Serial.print("Received the Set Position method: ");
  SetPHv = data;
  // Serial.println(SetPosition);
  return RPC_Response(NULL, SetPHv);
}

RPC_Response getPHv(const RPC_Data &data)
{
  // Serial.println("Received the get Position Method");
  return RPC_Response(NULL, SetPHv);
}
RPC_Callback callbacks[] = {
  { "setTemp", setTemperature },
  { "getTemp", getTemperature },
  { "setStir", setStirring },
  { "getStir", getStirring },
  { "setPH", setPHv },
  { "getPH", getPHv },
};


void connectWifi() {
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(ssid);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_wpa2_ent_enable();

  WiFi.begin(ssid);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println("");
  Serial.println(F("WiFi is connected!"));
  Serial.println(F("IP address set: "));
  Serial.println(WiFi.localIP());
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin(I2C_SDA, I2C_SCL);
  connectWifi();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:


  // Check wifi connection
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
    return;
  }



  // Receive stuff
  char actual_value[17];
  Wire.requestFrom(SLAVE_ADDR, 15);

  // char identity_value = (char)Wire.read();
  int i = 0;
  while (Wire.available()) {
    char c = Wire.read();
    // Serial.println(c);
    actual_value[i] = c;
    i++;
  }

  // Serial.println(actual_value);


  // Serial.println(Wire.read());
  // Serial.println(actual_value);

  // send to thingsboard

  // reconnect to thingsboard if needed
  if (!tb.connected()) {
    // connect to thingsboard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.print(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
      Serial.println("Failed to connect");
      return;
    }
  }


  if (!subscribed) {
    Serial.println("Subscribing to RPC");
  

    if (!tb.RPC_Subscribe(callbacks, COUNT_OF(callbacks))) {
      Serial.println("Failed to sub");
      return;
    }

    Serial.println("Subscribe done");
    subscribed = true;
  }

  float r = (float)random(1000)/1000;
  messageCounter++;

  // Serial.print("Sending data...[");
  // Serial.print(messageCounter);
  // Serial.print("]...");
  // Serial.println(r);
  // Serial.println(SetTemperature);
  // Serial.println(SetPHv);
  // Serial.println(SetStirring);
  float tempGoal = SetTemperature;
  float phGoal = SetPHv;
  float stirGoal = SetStirring;

  if (temp != tempGoal || ph != phGoal  || stir != stirGoal) {
    temp = tempGoal;
    ph = phGoal;
    stir = stirGoal;
    char str_send[17];
    String t1 = String(temp, 2);
    String ph1 = String(ph, 2);
    String s1 = String(stir);
    String fin = "T" + t1 + "P" + ph1 + "S" + s1;
    for (int i = 0; i < 16; i++){
      str_send[i] = fin[i];
    }
    Wire.beginTransmission(SLAVE_ADDR);
    Wire.write(str_send);
    Wire.endTransmission();
  }

  // decoding

  String temp_value, ph_value, stir_value;

  for (int i = 1; i < 6; i++){
    temp_value += actual_value[i];
  }
  for (int i = 7; i < 11; i++){
    ph_value += actual_value[i];
  }
  for (int i = 12; i < 16; i++){
    stir_value += actual_value[i];
  }

  //test
  // Serial.println(temp_value);
  // Serial.println(ph_value);
  // Serial.println(stir_value);
  Serial.println(SetTemperature);
  Serial.println(SetPHv);
  Serial.println(SetStirring);

  tb.sendTelemetryFloat("Temperature", temp_value.toFloat());
  tb.sendTelemetryFloat("pH", ph_value.toFloat());
  tb.sendTelemetryFloat("Stiring", stir_value.toInt());
  tb.sendTelemetryFloat("float", 1.269);
  tb.sendTelemetryInt("int", 1.269);
  tb.sendTelemetryFloat("randomVal", r);

  tb.loop();

  delay(500);
}

// void requestEvent() {
//   float temp = SetTemperature;
//   Serial.println(temp);
//   dtostrf(temp, 5,2, str_send);
//   Wire.write(str_send);
// }
