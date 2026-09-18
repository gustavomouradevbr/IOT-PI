/*
  Aula 3 — Lendo o sensor DHT22 (simulador Wokwi)

  Objetivo: sair do simples ligado/desligado e ler um sensor com protocolo
  próprio. O DHT22 não é lido com digitalRead(): ele "fala" um protocolo de
  fio único (single-wire), por isso usamos a biblioteca "DHT sensor library"
  (Adafruit) em vez de ler o pino diretamente.

  Limitação importante: só pode ser lido a cada ~2 segundos — ler mais
  rápido que isso devolve valores inválidos ou repetidos.
*/

#include <DHT.h>

#define DHT_PIN 15     // linha DATA do sensor
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  float temperatura = dht.readTemperature();  // sempre float (ex: 24.7), nunca int
  float umidade = dht.readHumidity();

  // isnan = "is not a number": o DHT22 às vezes falha (ruído, timing, fio
  // mal conectado) e devolve um valor especial (NaN) em vez de um número.
  // Sem essa checagem, o programa imprimiria "Temperatura: nan C" sem
  // avisar que algo deu errado — um erro de lógica silencioso.
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Erro ao ler o sensor DHT22!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" C");
    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");
    Serial.println("---");
  }

  delay(2000);  // respeita o limite mínimo de leitura do DHT22

  // Desafio da aula:
  // - Converter a temperatura de Celsius para Fahrenheit e exibir os dois
  // - (extra) Classificar a "sensação" do ambiente (Abafado/Frio/Agradável)
  //   combinando temperatura e umidade
}
