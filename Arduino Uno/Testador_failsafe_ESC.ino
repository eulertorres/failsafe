// Defina o pino de controle do ESC
#define esc  6 // substitua por seu pino GPIO desejado
#define LED 1

// Defina os limites de largura de pulso
const int pulseWidthStart = 1000; // 1000 us
const int pulseWidthEnd = 1500; // substitua por seu valor Fw desejado

// Defina os parâmetros de temporização
const int dt = 100; // Delta t incremento aceleração/desaceleração
const int ds = 10; 	// Incremento largura de pulso
const int delayAfterMax = 5000; // 5 segundos
const int delayLowSignal = 10000; // substitua por seu valor X desejado
const int delayBeforeDecrease = 5000; // 5 segundos

int esc_1;

unsigned long zero_timer, timer_channel_1, esc_loop_timer;

void setup() {
  Serial.begin(57600);                                                                  //Inicia comunicação serial

  //Portas de saída do arduino
  DDRD |= B01000010;                                                                    // Portas 6, 1 saída. 6 ESC 1 LED
  esc_1 = pulseWidthStart;
  esc_pulse_output();
}

void loop() {
  digitalWrite(LED, LOW);
  // Espere o comando X via Serial como um valor em milissegundos
  if (Serial.available() > 0) {
    int delayLowSignal = Serial.parseInt();

    // Começa a incrementar a largura do pulso
    for (int i = pulseWidthStart; i <= pulseWidthEnd; i += ds) {
	  esc_1 = i;
	  esc_pulse_output();
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

void esc_pulse_output(){
  zero_timer = micros();
  PORTD |= B01000000;                                            // Set port 5, 6 and 7 high at once
  timer_channel_1 = esc_1 + zero_timer;                          //Calculate the time when digital port 4 is set low.

  while(PORTD >= 63){                        // Entre neste loop até que todos os sinais estejam em BIAXO
    esc_loop_timer = micros();                                   // Lê o tempo atual
    if(timer_channel_1 <= esc_loop_timer)PORTD &= B10111111;     // Se o tempo atual for >= do tempo definido, o pino 2 é colocado em BAIXO
  }
}