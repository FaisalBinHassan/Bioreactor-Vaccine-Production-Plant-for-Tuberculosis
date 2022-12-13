#define PIN_SWITCH 3
 
// #define INP_PIN A1
#define SLAVE_ADDR 9
 
#include <Wire.h>
 

int pwrPin = 3;
int snsrPin = 7;
//Tempr
int V0;
float volts;
float temp;
float wanted_temp = 29.6; // temp we want. TODO - change later
float wanted_ph = 5; // temp we want. TODO - change later
float wanted_stir = 1155; // temp we want. TODO - change later
char to_send[17];

 
// pH
const int red_pH_sensorValue = 29;    // pH 4
const int yellow_pH_sensorValue = 0;  //pH 7
const int blue_pH_sensorValue = -36;  //pH 10
 
// calibration
const double acid_slope = -0.1034;
const double alkali_slope = -0.0833;
 
// Pumps
const int pumpA = 9;   // acid pump
const int pumpB = 10;  // alkali pump
 
//pH sensor
float pH = 0;
int stir = 0;
float difference = abs(pH - wanted_ph);
 
void setup() {
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);

  pinMode(pwrPin,OUTPUT);
  pinMode(snsrPin, INPUT);
 
 
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // int wanted_t = 28;
  pinMode(PIN_SWITCH, OUTPUT);
}

// sending to esp
void requestEvent() {
 
  String t1 = String(temp, 2);
  String ph1 = String(pH, 2);
  String s1 = String(stir,2);
  String fin = "T" + t1 + "P" + ph1 + "S" + s1;
 
  for (int i = 0; i < 16; i++){
    to_send[i] = fin[i];
  }
  Serial.println(to_send);
  Wire.write(to_send);
}

// receiving from esp
void receiveEvent() {
  char rec[17];
  int i = 0;
  while (Wire.available()) {
  char c = Wire.read();
  // Serial.println(c);
    rec[i] = c;
    i++;
  }
  String rt, rp, rs;
  for (int i = 1; i < 6; i++){
    rt += rec[i];
  }
  for (int i = 7; i < 11; i++){
    rp += rec[i];
  }
  for (int i = 12; i < 16; i++){
    rs += rec[i];
  }
  wanted_temp = rt.toFloat();
  // Serial.println(wanted_temp);
  wanted_ph = rp.toFloat();
  wanted_stir = rs.toInt();
 
  //Serial.print("received: ");
  //Serial.println(received_string);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  V0 = analogRead(A0);
  // print out the value you read:
  // int diff = sensorValue1 - sensorValue0;
  volts = (5.0 * V0) / 1023.0;
  temp = (volts - 1.2586) / 0.0535;
 
  // Serial.println(temp);
  // Serial.println(wanted_t);
  
  if (temp < wanted_temp - 0.4){ // stop 0.4 before reaching temp
    analogWrite(PIN_SWITCH, 60);
    Serial.println("heating");
  }
  else
    digitalWrite(PIN_SWITCH, LOW);
 
 // Serial.println(voltage);
 
 // Serial.println(" a0");
 // Serial.print(sensorValue1);
 // Serial.println(" a1");
 
 //pH
  float pH1_sensorValue = analogRead(A1);
  float rV_sensorValue = analogRead(A2);
  float pH2_sensorValue = pH1_sensorValue - rV_sensorValue;


 
  if (pH2_sensorValue < 0) {
    pH = pH2_sensorValue * alkali_slope + 7;
  }
  if (pH2_sensorValue > 0) {
    pH = pH2_sensorValue * acid_slope + 7;
  }
  if (pH2_sensorValue == 0) {
    pH = 7;
  }
  // output
  // Serial.println(pH);
 
  // acid pump activation
  if (wanted_ph < pH && difference >= 0.5) {
    analogWrite(pumpA, 255);
  } else {
    analogWrite(pumpA, 0);
  }
 
  // akali pump activation
  if (wanted_ph > pH && difference >= 0.5) {
    analogWrite(pumpB, 255);
  } else {
    analogWrite(pumpB, 0);
  }
  // stir

  float newSpeed_mapped = (wanted_stir + 1486) / 24.473;
  analogWrite(pwrPin,newSpeed_mapped);
 
  int snsrVal = digitalRead(snsrPin);
  float t = pulseIn(snsrPin, HIGH);
  float period = t*0.00001;
  float freq = 10/(period*2);
  int stir = (freq*60);
  // Serial.println(stir);
  // rpm = 24.473 x pwd_volt - 1486
  




 delay(1500); // delay in between reads for stability
}