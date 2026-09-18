/*
  Aula 2 — Entrada digital + Serial Monitor

  Objetivo: usar digitalRead() para ler um botão e Serial.println() para debug.
  INPUT_PULLUP liga um resistor interno que mantém o pino em HIGH por padrão,
  evitando que ele "flutue" (leia valores aleatórios) quando nada está
  pressionando o botão.
*/

#define LED_PIN 2
#define BUTTON_PIN 4   // botão conectado ao GND quando pressionado

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // pino fica HIGH por padrão, vai a LOW quando pressionado
  Serial.begin(115200);               // abre o canal serial (baud rate = velocidade de comunicação)
}

void loop() {
  int estadoBotao = digitalRead(BUTTON_PIN);
  // digitalRead() devolve um int (1 = HIGH, 0 = LOW), não um "bool" de verdade.
  // HIGH e LOW são só constantes inteiras (HIGH = 1, LOW = 0) nos bastidores.

  if (estadoBotao == LOW) {          // LOW = botão pressionado (contraintuitivo, mas é assim com INPUT_PULLUP)
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Botao pressionado e LED ligado");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Botao solto e LED desligado");
  }

  delay(200);

  // Desafio da aula:
  // - Inverter o comportamento (LED começa ligado, apaga ao pressionar)
  // - Fazer o LED alternar de estado a cada aperto (toggle), em vez de
  //   acender só enquanto pressionado
}
