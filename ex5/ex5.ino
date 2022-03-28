const int LEDPIN = 11;

void setup()
{
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Intensidade do led(0-255):");
}

void loop()
{
  if (Serial.available() > 0) {
    int intensidade = Serial.parseInt();

    //porque é que ele depois não entra aqui?
    // put your main code here, to run repeatedly:
    if (intensidade >= 0 || intensidade <= 255) {
      analogWrite(LEDPIN, intensidade);
    } else {
      Serial.println("Valor não suportado. Insira um valor entre 0 e 255");
    }
  }
}
