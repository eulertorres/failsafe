// Defina o pino de controle do ESC
#define esc  6 // substitua por seu pino GPIO desejado
#define LEDG 4
#define LEDR 5
#define trigger 7

// Defina os limites de largura de pulso
const int pulseWidthStart = 960; // PWM inicial (throttle mínimo, para o ESC armar)
const int pulseWidthEnd = 1550;  // Velocidade final antes da perda de sinal
const int pulseAfterFS = 1888;   // PWM em que o ESC volta após perda de sinal

const int delayLowSignal = 10; // Tempo em ms em que haverá perda de sinal. Simulação Failsafe

// Defina os parâmetros de temporização e aceleração
//const int dt = 100;      // Delta t incremento aceleração/desaceleração
const int ds = 2; 	       // Incremento largura de pulso
const int accel = 1;      // Dt Aceleração do ESC.

//const int delayAfterMax = 5000; // 5 segundos

//const int delayBeforeDecrease = 5000; // 5 segundos

const int periodo = 10000; // Período em us. Frequência = 10^6/T

int esc_1, counter, start = 0, request = 0, last;

unsigned long zero_timer, timer_channel_1, esc_loop_timer, lock = 0;

void setup() {
  Serial.begin(57600);                                                                  //Inicia comunicação serial

  //Portas de saída do arduino
  DDRD |= B11011000;                                                                    // Portas 6, 1 saída. 6 ESC 7 trigger 4 e 5 LED
  esc_1 = pulseWidthStart;
  esc_pulse_output();
  PORTD |= B10000000;               // Trigger começa em alto
}

void loop() {
  //digitalWrite(LED, LOW);
  while(zero_timer + periodo > micros());        // Garantir que o loop esteja na frequência desejada
  zero_timer = micros();                         // Zera o timer para os próximos 4000 us
  esc_pulse_output();

  // Caso start = 1, Vamos acelerar o ESC gradualmente
  if(counter > accel){
    // Comando para acelerar e desacelerar o ESC
    if(start == 1 && esc_1 < pulseWidthEnd){    
      esc_1 = esc_1 + ds;
      counter  = 0;
      //PORTD |= B00001000;
    } else if (start == 3 && esc_1 > pulseWidthStart){
      esc_1 = esc_1 - (ds*2);
      counter = 0;
      //PORTD &= B11110111;
    }
  } else counter++;

 //Serial.println(start);

  if(PIND & B00000100 && millis() > lock + 5000){
    start++;
    if (start == 1) PORTD |= B00010000;
    if (start == 3) PORTD &= B11001111; 
    if(start == 2){
      PORTD &= B00110111;                 // Desliga sinal do Throttle e Trigger (DEBUG)
      delay(delayLowSignal);              // Permanece em baixo pelo tempo determinado
      PORTD |= B11100000;                 // Retomada do PWM e borda de subida trigger
      //delayMicroseconds(pulseAfterFS);    // Primeiro PWM depois do Fail safe
      //PORTD &= B10111111;
      esc_1 = pulseAfterFS;               // Define velocidade que o ESC permanecerá após retomar
      esc_pulse_output();
      }
    lock = millis();
 }
}

void esc_pulse_output(){
  PORTD |= B01000000;                                            // Set port 5, 6 and 7 high at once
  timer_channel_1 = esc_1 + zero_timer;                          //Calculate the time when digital port 4 is set low.

  while(PIND & B01000000){                        // Entre neste loop até que todos os sinais estejam em BIAXO
    esc_loop_timer = micros();                                   // Lê o tempo atual
    if(timer_channel_1 <= esc_loop_timer)PORTD &= B10111111;     // Se o tempo atual for >= do tempo definido, o pino 2 é colocado em BAIXO
  }
}

ISR(INT0_vect){
  if(PIND & B00000100){
    if(!last){
      request = 1;
      last = 1;
    }
  } else if(last){
    last = 0;
  }
}