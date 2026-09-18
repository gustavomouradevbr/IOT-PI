/*
  Aula 1 — Blink no ESP32 (simulador Wokwi)

  Objetivo: primeiro contato com GPIO de saída.
  Um sistema IoT sempre tem 3 papéis: Dispositivo, Comunicação e
  Processamento. Aqui exploramos só o "Dispositivo": o ESP32 controlando
  um LED simples.
*/

#define LED_PIN 2   // GPIO2: pino digital comum, ligado ao LED azul embutido na maioria das placas ESP32 DevKit

void setup() {
  // setup() roda uma única vez, quando o ESP32 liga.
  pinMode(LED_PIN, OUTPUT);   // configura o pino como SAÍDA digital
}

void loop() {
  // loop() roda repetidamente, para sempre.
  digitalWrite(LED_PIN, HIGH);  // liga o LED
  delay(1000);                  // aguarda 1 segundo
  digitalWrite(LED_PIN, LOW);   // desliga o LED
  delay(1000);                  // aguarda 1 segundo

  // Desafio da aula: alterar o delay() para 250 (mais rápido) e 2000 (mais devagar)
}
