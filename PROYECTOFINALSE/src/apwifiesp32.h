#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP_Mail_Client.h>

#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_465
#define AUTHOR_EMAIL "pembebidos@gmail.com"
#define AUTHOR_PASSWORD "ssmiuowpdtgtdgdf"
boolean conexion = false;
WebServer server(80);
Preferences preferences;
SMTPSession smtp;
void enviarCorreo()
{
  preferences.begin("Wifi", false);
  String email = preferences.getString("email", "");
  preferences.end();
  if (email != "")
  {
    smtp.debug(1);
    ESP_Mail_Session session;
    session.server.host_name = SMTP_HOST;
    session.server.port = SMTP_PORT;
    session.login.email = AUTHOR_EMAIL;
    session.login.password = AUTHOR_PASSWORD;

    SMTP_Message message;
    message.sender.name = "Twister Educativo";
    message.sender.email = AUTHOR_EMAIL;
    message.subject = "Estado del Sistema Twister Educativo";
    message.addRecipient("Usuario", email);
    String mensaje = "El sistema se encuentra encendido";
    message.text.content = mensaje.c_str();
    message.text.charSet = "us-ascii";
    message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
    message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_normal;
    if (!smtp.connect(&session))
    {
      Serial.print("No se logro conectar ");
    }
    if (!MailClient.sendMail(&smtp, &message))
    {
      Serial.print("Error al enviar el email ");
    }
    ESP_MAIL_PRINTF("Liberar Memoria: %d", MailClient.getFreeHeap());
  }
}

void handleRoot()
{
  String html = "<html><body>";
  html += "<form method='POST' action='/wifi'>";
  html += "Red Wi-Fi: <input type='text' name='ssid'><br>";
  html += "Contraseña: <input type='password' name='password'><br>";
  html += "Gmail: <input type='text' name='email'><br>";
  html += "<input type='submit' value='Conectar'>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleWifi()
{
  preferences.begin("Wifi", false);
  String email = server.arg("email");
  if (conexion == false)
  {
    String ssid = server.arg("ssid");
    String password = server.arg("password");

    Serial.print("Conectando a la red Wi-Fi ");
    Serial.println(ssid);

    Serial.println(password);
    Serial.print("...");

    WiFi.disconnect(); // Desconectar la red Wi-Fi anterior, si se estaba conectado
    WiFi.begin(ssid.c_str(), password.c_str(), 6);
    int cnt = 0;
    while (WiFi.status() != WL_CONNECTED and cnt < 8)
    {
      delay(1000);
      Serial.print(".");
      cnt++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Conexión establecida");
      server.send(200, "text/plain", "Conexión establecida");
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
    }
    else
    {
      Serial.println("Conexión no establecida");
      server.send(200, "text/plain", "Conexión no establecida");
    }
  }
  if (email != "")
  {
    preferences.putString("email", email);
    server.send(200, "text/plain", "Correo cambiado");
  }
  preferences.end();
}

void initAP(const char *apSsid, const char *apPassword)
{ // Nombre de la red Wi-Fi y  Contraseña creada por el ESP32
  Serial.begin(115200);
  preferences.begin("Wifi", false);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSsid, apPassword);

  server.on("/", handleRoot);
  server.on("/wifi", handleWifi);

  server.begin();
  Serial.print("Ip de esp32...");
  Serial.println(WiFi.softAPIP());
  // Serial.println(WiFi.localIP());
  Serial.println("Servidor web iniciado");
  preferences.putString("ssid", "");
  preferences.putString("email", "");
  preferences.putString("password", "");
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  preferences.end();
  Serial.print("Conectando a la red Wi-Fi ");
  Serial.println(ssid);
  Serial.print("Clave Wi-Fi ");
  Serial.println(password);
  Serial.print("...");
  WiFi.disconnect();
  WiFi.begin(ssid.c_str(), password.c_str(), 6);
  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED and cnt < 8)
  {
    delay(1000);
    Serial.print(".");
    cnt++;
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    enviarCorreo();
    Serial.println("Conexión establecida");
    server.send(200, "text/plain", "Conexión establecida");
    conexion = true;
  }

  else
  {
    Serial.println("Conexión no establecida");
    server.send(200, "text/plain", "Conexión no establecida");
    conexion = false;
  }
}

void loopAP()
{

  server.handleClient();
}
