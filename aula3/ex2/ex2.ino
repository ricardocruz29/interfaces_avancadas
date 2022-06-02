const int ledPin = 6;
int atraso;
int nr_piscas;

void setup() {

  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.println(" ");
  Serial.println("Quantas vezes quer piscar o led?");
  while (Serial.available() == 0) {}
  nr_piscas = Serial.parseInt();

  //Clean cache
  Serial.read();

  Serial.println("Qual o atraso que quer dar entre piscadelas?");
  while (Serial.available() == 0) {}
  atraso = Serial.parseInt();

  //Clean cache
  Serial.read();
  for (int i = 0; i < nr_piscas; i++) {
    digitalWrite(ledPin, HIGH);
    delay(atraso);
    digitalWrite(ledPin, LOW);
    delay(atraso);
  }
}
