#define PIN_SWITCH 3
 
// #define INP_PIN A1
#define SLAVE_ADDR 9
 
#include <Wire.h>

//Temp
int V0;
float volts;
float temp;
float wanted_t = 0; // temp we want. TODO - change later
float wanted_ph = 0; // temp we want. TODO - change later
float wanted_stir = 0; // temp we want. TODO - change later
char to_send[17];
char rec[17];

 
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
int stir = 697;
float user_input;
float difference = abs(pH - user_input);

void setup() {
  Wire.begin(SLAVE_ADDR);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);


 
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // int wanted_t = 28;
  pinMode(PIN_SWITCH, OUTPUT);
}
 
void requestEvent() {

  String t1 = String(temp, 2);
  String ph1 = String(pH, 2);
  String s1 = String(stir);
  String fin = "T" + t1 + "P" + ph1 + "S" + s1;

  for (int i = 0; i < 16; i++){
    to_send[i] = fin[i];
  }
  Wire.write(to_send);
}

void receiveEvent(int HowMany) {
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
  float wanted_temp = rt.toFloat();
  float wanted_ph = rp.toFloat();
  int wanted_stir = rs.toInt();

  Serial.println("Wanted temp: ", wanted_temp);
  Serial.println("Wanted ph: ", wanted_ph);
  Serial.println("Wanted stir: ". wanted_stir);

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
 
  Serial.println(temp);
  Serial.println(wanted_t);
  
  if (temp < wanted_t - 0.4){ // stop 0.4 before reaching temp
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
  Serial.println(pH);

  // acid pump activation
  if (user_input < pH && difference >= 0.5) {
    analogWrite(pumpA, 255);
  } else {
    analogWrite(pumpA, 0);
  }

  // akali pump activation
  if (user_input > pH && difference >= 0.5) {
    analogWrite(pumpB, 255);
  } else {
    analogWrite(pumpB, 0);
  }
 delay(1500); // delay in between reads for stability
}
