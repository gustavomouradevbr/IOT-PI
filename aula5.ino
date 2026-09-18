/*
  Aula 5 — Estruturando os dados em JSON (preparação pro ThingSpeak)

  Objetivo: montar uma string JSON com sprintf(), pra já nomear os dados
  como o ThingSpeak espera (field1, field2, ...), adiantando o trabalho da
  Aula 6. O \" dentro da string coloca aspas duplas literais dentro de
  outra string com aspas duplas — sem isso, o compilador acharia que a
  string terminou ali.

  Esta versão já resolve o desafio da aula, adicionando:
  field3 = temperatura máxima já registrada
  field4 = 1 se está em alerta, 0 se não está
  (TEMP_ALERTA é só um valor de exemplo — ajuste como preferir)
*/

#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11
#define TEMP_ALERTA 30.0  // limite de exemplo para disparar o alerta

DHT dht(DHT_PIN, DHT_TYPE);
float temperaturaMaxima = -1000.0; // guarda o maior valor já lido nesta sessão

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
    if (temperatura > temperaturaMaxima) {
      temperaturaMaxima = temperatura; // atualiza o recorde da sessão
    }

    int emAlerta = (temperatura >= TEMP_ALERTA) ? 1 : 0;

    char jsonBuffer[100];
    sprintf(jsonBuffer, "{\"field1\":%.2f,\"field2\":%.2f,\"field3\":%.2f,\"field4\":%d}",
            temperatura, umidade, temperaturaMaxima, emAlerta);

    Serial.println(jsonBuffer);
  }

  delay(2000);
}
