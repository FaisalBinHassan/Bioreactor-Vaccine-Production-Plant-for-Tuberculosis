float temp = 27.26;
float ph = 6.52;
int stir = 442;
// String fin = "";

// 16 chars + newline (very important)
char to_send[17];
char rec[17];


void setup() {
  Serial.begin(9600);

}

void loop() {
  
  // fin - final message to be sent, before converting to array of char
  String t1 = String(temp, 2);
  String ph1 = String(ph, 2);
  String s1 = String(stir);
  String fin = "T" + t1 + "P" + ph1 + "S" + s1;

  // endoding
  // to_send will be sent
  for (int i = 0; i < 16; i++){
    to_send[i] = fin[i];
  }
  // decoding
  for (int i = 0; i < 16; i++){
    rec[i] = to_send[i];
  }
  
  // splitting on letter characters
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

  // values split
  float current_temp = rt.toFloat();
  float current_ph = rp.toFloat();
  int current_stir = rs.toInt();

  Serial.println(current_temp);
  Serial.println(current_ph);
  Serial.println(current_stir);
  delay(1200);
}
