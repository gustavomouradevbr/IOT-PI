/*
  Aula 4 — Do simulador para o circuito físico (DHT11 + ESP32-C3 Mini)

  O que muda do simulador pro físico:
  - Driver USB: o computador precisa reconhecer a placa como porta COM
  - Alimentação correta: o DHT11 vai no pino 3V3 (não em pinos de 5V)
  - Pull-up na linha de dados: se o sensor for o módulo de 3 pinos, o
    resistor já vem embutido; se for o sensor "pelado" de 4 pinos, é
    preciso um resistor de 10kΩ entre DATA e VCC

  DHT11 x DHT22 (real x simulação): leituras um pouco menos precisas
  (±2°C / ±5% vs ±0.5°C / ±2-5%) e intervalo mínimo entre leituras menor
  (~1s vs ~2s) — por isso hoje as leituras são esperadas um pouco menos
  precisas que na simulação da Aula 3.

  Ligação: DHT11 VCC -> 3V3 | DHT11 DATA -> GPIO4 | DHT11 GND -> GND
*/

#include <DHT.h>

#define DHT_PIN 4      // GPIO4 no ESP32-C3 Mini
#define DHT_TYPE DHT11 // trocado de DHT22 (Aula 3) para DHT11 (sensor físico)

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Erro ao ler o sensor DHT11!");
  } else {
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" C");
    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");
    Serial.println("---");
  }

  delay(2000); // acima do mínimo de 1s do DHT11, não precisa mexer

  // Desafio da aula: assoprar de leve no sensor ou segurá-lo na mão por
  // 30s e observar no Monitor Serial quanto tempo a leitura demora pra
  // reagir. Anotar: quantos segundos até a temperatura subir visivelmente?
}
