const int pin_x = A0;
const int pin_y = A1;
const int pin_btn = 3;
int sensorValue;
bool pressed = false;

void setup() {
  Serial.begin(9600);
}

void loop() {
  sensorValue = analogRead(pin_x);
  Serial.print("X");
  Serial.println(sensorValue);
  sensorValue = analogRead(pin_y);
  Serial.print("Y");
  Serial.println(sensorValue);
  bool readv = digitalRead(pin_btn);
  if (!readv && !pressed){
    Serial.println("bp");
    pressed = true;
  }else if (readv && pressed){
    Serial.println("br");
    pressed = false;
  }
}
