#include <WiFi.h>

const char* ssid = "";
const char* password = "";

const int LDR_PIN = 32;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado! IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  while (!client.available()) delay(1);

  String req = client.readStringUntil('\r');
  client.read(); // limpa o '\n'

  // Lê o sensor
  int valorLDR = analogRead(LDR_PIN);
  float lux = map(valorLDR, 0, 4095, 0, 1000);

  // Rota /lux
  if (req.indexOf("GET /lux") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println(lux);
    return;
  }

  // Página com gráfico
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=UTF-8");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta charset='UTF-8'><title>Gráfico em Tempo Real</title>");
  client.println("<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>");
  client.println("<style>body{text-align:center;font-family:Arial;}canvas{max-width:600px;margin:auto;}</style>");
  client.println("</head><body>");
  client.println("<h2>Luminosidade (lux)</h2>");
  client.println("<canvas id='grafico'></canvas>");
  client.println("<script>");
  client.println("const ctx = document.getElementById('grafico').getContext('2d');");
  client.println("const dados = { labels: [], datasets: [{ label: 'Lux', data: [], borderColor: 'blue', fill: false }] };");
  client.println("const config = { type: 'line', data: dados, options: { scales: { y: { beginAtZero: true } } } };");
  client.println("const grafico = new Chart(ctx, config);");

  client.println("function atualizarGrafico() {");
  client.println("  fetch('/lux').then(r => r.text()).then(valor => {");
  client.println("    const lux = parseFloat(valor);");
  client.println("    const agora = new Date().toLocaleTimeString();");
  client.println("    if (dados.labels.length >= 20) { dados.labels.shift(); dados.datasets[0].data.shift(); }");
  client.println("    dados.labels.push(agora);");
  client.println("    dados.datasets[0].data.push(lux);");
  client.println("    grafico.update();");
  client.println("  });");
  client.println("}");
  client.println("setInterval(atualizarGrafico, 1000);");
  client.println("</script>");
  client.println("</body></html>");
}
