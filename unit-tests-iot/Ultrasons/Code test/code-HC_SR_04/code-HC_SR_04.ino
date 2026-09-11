const int pinTrig = 9;
const int pinEcho = 8;

long duree;
int distance;

void setup() {
  Serial.begin(9600);
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT);
  digitalWrite(pinTrig, LOW);
}

void loop() {
  digitalWrite(pinTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(pinTrig, LOW);
  
  duree = pulseIn(pinEcho, HIGH);
  distance = duree * 0.034 / 2;
  
  Serial.print("Distance : ");
  Serial.print(distance);
  Serial.println(" cm");
  
  delay(500);
}