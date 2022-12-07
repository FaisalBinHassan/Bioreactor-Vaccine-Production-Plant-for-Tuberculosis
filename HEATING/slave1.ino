#define PIN_SWITCH 3
 
// #define INP_PIN A1
#define SLAVE_ADDR 9
 
#include <Wire.h>
 
int V0;
float volts;
float temp;
float wanted_t = 0; // temp we want. TODO - change later
char str_send[5];
 
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
 dtostrf(temp, 5, 2, str_send);
 Wire.write(str_send);
}

void receiveEvent(int HowMany) {
  String received_string = "";
  while (Wire.available()) {
    char c = Wire.read();
    received_string += c;
  }

  //Serial.print("received: ");
  //Serial.println(received_string);
  wanted_t = received_string.toFloat();
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
 delay(1500); // delay in between reads for stability
}

