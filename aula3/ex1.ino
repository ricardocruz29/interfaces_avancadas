const int ledPin = 6;
String ledState;

void setup() {

  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}
// Quando for criar On/Off trocar no terminal de "nova linha" para "nenhum final-de-linha"
void loop() {

  if (Serial.available() > 0) {
    ledState = Serial.readString();
    if (ledState == "On") {
      digitalWrite(ledPin, HIGH);
    }
    if (ledState == "Off") {
      digitalWrite(ledPin, LOW);
    }
  }
}
