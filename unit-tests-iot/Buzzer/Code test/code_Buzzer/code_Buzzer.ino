void setup(){
  Serial.begin(9600);
  pinMode(13, OUTPUT);
}

void loop(){
  digitalWrite(13, LOW);
  Serial.println("Alarme éteinte : 0");
  delay(2000);
  digitalWrite(13, HIGH);
  Serial.println("Alarme alumée : 1");
  delay(2000);
}