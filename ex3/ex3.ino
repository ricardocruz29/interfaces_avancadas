const int LEDPIN = 11;
int sinal = 1; int intensidade = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
}

void checkIntensidade() {
  Serial.println(intensidade);
  Serial.println(sinal);
  if (intensidade < 0) {
    intensidade = 0;
    sinal *= -1;
  } else if (intensidade > 255){
    intensidade = 255;
    sinal *= -1;
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(LEDPIN, intensidade);
  delay(1000);
  //so para piscar
  analogWrite(LEDPIN, 0);
  delay(1000);
  //---------
  checkIntensidade();
  intensidade += 5 * sinal;
}
