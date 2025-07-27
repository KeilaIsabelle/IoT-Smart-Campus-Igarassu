#include <WiFi.h>
#include "DHT.h"

#define DHTPIN 14        // GPIO onde o sensor está conectado (mude se usar outro)
#define DHTTYPE DHT11   // ou DHT22 se for outro modelo

const char* ssid = "";
const char* password "";

WiFiServer server(80);  // Porta padrão HTTP

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Conectando-se ao Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conectado! IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("Novo cliente conectado!");
  while (!client.available()) {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  client.flush();

  float temperatura = dht.readTemperature(); // Celsius
  float umidade = dht.readHumidity();

  // Início do HTML
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<title>Leitura do Sensor DHT11</title>");
  client.println("<style>");
  client.println("body { font-family: Arial; text-align: center; margin-top: 50px; }");
  client.println("h1 { color: #333; }");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h1>Veja como está a temperatura e a umidade hoje:</h1>");

  if (isnan(temperatura) || isnan(umidade)) {
    client.println("<p>Falha ao ler o sensor DHT11 😢</p>");
  } else {
    client.println("<p><strong>Temperatura:</strong> " + String(temperatura) + " &deg;C</p>");
    client.println("<p><strong>Umidade:</strong> " + String(umidade) + " %</p>");
  }

  client.println("</body>");
  client.println("</html>");

  delay(100);
  Serial.println("Cliente desconectado");
}