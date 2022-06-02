#include <Servo.h>
Servo servo;
int angulo = 0;
int sinal = 1;


void setup() {
Serial.begin(9600);
servo.attach(9);

}

void loop() {
  servo.write(angulo);
  delay(2000);
  angulo += 15 * sinal;

  Serial.println(angulo);

  if (angulo >= 180 || angulo <= 0){
    sinal *= -1;
  }
  
}
