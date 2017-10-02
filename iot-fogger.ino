#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80); // Port für den WebServer setzen (80 ist Standard Web Port)

int getArgValue(String name) // Zum auslesen des ?fogtime=MS Parameters
{
  for (uint8_t i = 0; i < server.args(); i++)
    if(server.argName(i) == name)
      return server.arg(i).toInt();
  return -1;
}

void startseite() // Wird bei URL Aufruf / (Startseite) ausgeführt
{
  server.send(200, "text/html", "Bedienung:\nURL: /fog (Nebel ein)\nURL: /nofog (Nebel aus)");
}

void einschalten() // Wird bei URL Aufruf /fog ausgeführt
{
  server.send(200, "text/html", "OK"); // HTML auswerfen
  digitalWrite(5, HIGH); // GPIO Pin auf 3.3V ziehen
}

void ausschalten() // Wird bei URL Aufruf /nofog ausgeführt
{
  server.send(200, "text/html", "OK"); // HTML auswerfen
  digitalWrite(5, LOW); // GPIO Pin auf 0V ziehen
}

void zeitschalter() // Wird bei URL Aufruf /fogtimer ausgeführt
{
  int fogtime = getArgValue("fogtime"); // Variable Fogtime holen
  if(fogtime > 10)
  {
   server.send(200, "text/html", "OK"); // HTML auswerfen
   digitalWrite(5, HIGH); // GPIO Pin auf 3.3V ziehen
   delay(fogtime); // Millisekunden aus Variable abwarten (Nebelt so lange)
   digitalWrite(5, LOW); // GPIO Pin auf 0V ziehen
  }
  else
  {
   server.send(200, "text/html", "ERROR"); // HTML auswerfen (wenn fogtime nicht gesetzt wird nur der Fehler ausgegeben)
  }
}

int buttonState = 0;

void setup()
{
  pinMode(15, INPUT); //
  pinMode(5, OUTPUT); // GPIO(D1) als Ausgang konfigurieren
  digitalWrite(5, LOW); // Zur Sicherheit Relais abschalten (Relais ausgeschaltet)

  Serial.begin(9600); // Serielle schnittstelle initialisieren
  Serial.println(""); // Lehere Zeile ausgeben
  Serial.println("Starte WLAN-Hotspot..."); // Meldung im Serial Monitor
  WiFi.mode(WIFI_AP);           // ESP8266 in den W-Lan Access Point (AP) Modus versetzen
  WiFi.softAP("IoT Fogger", "12345678"); // Name des W-Lan Netzes (SSID - Passwort)
  IPAddress apIP(10, 10, 10, 1);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00
  delay(500); //warte zum Start 0,5s
  Serial.print("IP Adresse "); // Server IP im Serial ausgeben (IP des WebServers zur Steuerung)
  Serial.println(WiFi.softAPIP());

  // Ereignisse setzen
  server.on("/", startseite); // Wird bei URL Aufruf / (Startseite) ausgeführt
  server.on("/fog", einschalten); // Wird bei URL Aufruf /fog ausgeführt
  server.on("/nofog", ausschalten); // Wird bei URL Aufruf /nofog ausgeführt
  server.on("/fogtimer", zeitschalter); // Wird bei URL Aufruf /fogtimer ausgeführt (?fogtime=MS muss gesetzt werden ! 1000 = 1 Sekunde)

  server.begin(); // Starte den WebServer
  Serial.println("HTTP Server gestartet"); //Im Serial melden dass wir Ready to use sind
}

void loop()
{
  server.handleClient(); // Permanent prüfen ob eine Anfrage kommt

  buttonState = digitalRead(15);
  if (buttonState == HIGH) {
    digitalWrite(5, HIGH);
  } else {
    digitalWrite(5, LOW);
  }
}
