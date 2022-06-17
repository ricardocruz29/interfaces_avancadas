const int redPin = 5;
const int greenPin = 3;
const int bluePin = 4;

int ldrPin = 0; //LDR no pino analígico 0
int ldrValor = 0; //Valor lido do LDR

//Feedback professores
//Para a luminosidade, secalhar faz sentido fazer tipo uma media ao longo de x tempo, para termos em conta o caso de passar por exemplo uma nuvem, e a luminosidade ficar baixa
//Verificar também bases de dados para verificar que valores e que sao aceitaveis
//Para o sensor de movimento, fazer um timer para a planta não estar sempre a falar qnd estivesse alguem
//Falar no relatorio da possivel imlpementação do painel solar, como é que fariamos (tem que ver isso pq só funcionaria em platas de luz direta haha)
//Armazenar informações de todos os registos dos modelos para ter uma media das informações (mas não sei se acho isso necessário) -> Basicamente se tivessemos 5 mil produtos vendidos, o publico começar a dizer que valores é que achavam melhores
//não estar sempre ligado na cor sólida quando se está tudo bem, tendo uma variação na intensidade

void setup() {
  // put your setup code here, to run once:
  pinMode(bluePin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
  
  Serial.begin(9600); //Inicia a comunicação serial
}

void loop() {
  // put your main code here, to run repeatedly:
  ldrValor = analogRead(ldrPin);
  
  Serial.println(ldrValor);
  Serial.println("---");
  
  //falta de luz
  if (ldrValor >= 550) {
    //analogWrite(redPin, 43);
    //analogWrite(greenPin, 43);
    //analogWrite(bluePin, 43);
    analogWrite(redPin, 255);
    analogWrite(greenPin, 255);
    analogWrite(bluePin, 0);
  }
  
  //excesso luz
  if (ldrValor < 50) {
    //analogWrite(redPin, 237);
    //analogWrite(greenPin, 51);
    //analogWrite(bluePin, 14);
    analogWrite(redPin, 255);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
  }

  if (ldrValor >= 100 && ldrValor < 650) {
    analogWrite(redPin, 0);
    analogWrite(greenPin, 255);
    analogWrite(bluePin, 0);
  }
  
  
  delay(200);
}
