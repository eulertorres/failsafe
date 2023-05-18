#include <ESP32Servo.h>

// Defina o pino de controle do ESC
const int escPin = 4; // substitua por seu pino GPIO desejado
const int LED = 5;

// Defina os limites de largura de pulso
const int pulseWidthStart = 1000; // 1000 us
const int pulseWidthEnd = 2000; // substitua por seu valor Fw desejado

// Defina os parâmetros de temporização
const int dt = 100; // Delta t incremento aceleração/desaceleração
const int ds = 10; 	// Incremento largura de pulso
const int delayAfterMax = 5000; // 5 segundos
const int delayLowSignal = 10000; // substitua por seu valor X desejado
const int delayBeforeDecrease = 5000; // 5 segundos

Servo esc;

void setup() {
  esc.attach(escPin);
  esc.writeMicroseconds(pulseWidthStart);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(LED, LOW);
  // Espere o comando X via Serial como um valor em milissegundos
  if (Serial.available() > 0) {
    int delayLowSignal = Serial.parseInt();

    // Começa a incrementar a largura do pulso
    for (int i = pulseWidthStart; i <= pulseWidthEnd; i += ds) {
      esc.writeMicroseconds(i);
      delay(dt);
    }

    // Espera 5 segundos
    delay(delayAfterMax);

    // Coloca o sinal em nível lógico baixo
    digitalWrite(escPin, LOW);
	digitalWrite(LED, HIGH);
    delay(delayLowSignal);
	
    // Volta ao PWM com largura de pulso Fw
    esc.writeMicroseconds(pulseWidthEnd);
    delay(delayBeforeDecrease);

    // Começa a decrementar a largura do pulso
    for (int i = pulseWidthEnd; i >= pulseWidthStart; i -= ds) {
      esc.writeMicroseconds(i);
      delay(dt);
    }
  }
}
