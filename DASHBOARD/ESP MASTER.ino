// MASTER CODE

#include <Wire.h>
#include <WiFi.h>
#include "esp_wpa2.h"
#include <ThingsBoard.h>


// Wifi
#define EAP_IDENTITY "zcabmaq@ucl.ac.uk"
#define EAP_PASSWORD "Muyyaq.333802"

const char* ssid = "eduroam";
WiFiClient espClient; //Initialize ThingsBoard client
int status = WL_IDLE_STATUS;
ThingsBoard client(espClient);


// send and receive data
#define SLAVE_ADDR 9
#define I2C_SDA 21
#define I2C_SCL 22
float temp = 0;

char str_send[5];

// Thingsboard
#define TOKEN "7aLsfYuYcCDlWFvp5BNk"
#define THINGSBOARD_SERVER "thingsboard.cloud"


ThingsBoard tb(espClient);
static uint16_t messageCounter = 0;



// RPC
bool subscribed = false;
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

float SetTemperature = 0;
float SetPHv = 0;
int SetStirring = 0;
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
  String received_string = "";
  Wire.requestFrom(SLAVE_ADDR, 5);

  while (Wire.available()) {
    char c = (char)Wire.read();
    received_string += c;
  }
  // Serial.println(Wire.read());
  Serial.println(received_string);

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
  float tempGoal = SetTemperature;

  if (temp != tempGoal) {
    temp = tempGoal;
    Wire.beginTransmission(SLAVE_ADDR);
    char str_send[5];
    dtostrf(temp, 5, 2, str_send);
    Wire.write(str_send);
    Wire.endTransmission();
  }


  //test



  tb.sendTelemetryFloat("Temperature", received_string.toFloat());
  tb.sendTelemetryFloat("float", 1.269);
  tb.sendTelemetryInt("int", 1.269);
  tb.sendTelemetryFloat("randomVal", r);

  tb.loop();

  delay(1000);
}

void requestEvent() {
  float temp = SetTemperature;
  Serial.println(temp);
  dtostrf(temp, 5,2, str_send);
  Wire.write(str_send);
}
