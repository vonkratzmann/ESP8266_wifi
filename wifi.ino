#include <ESP8266WiFi.h>
// WiFi Definitions
const char* ssid = "iiNet22B8AF";
const char* password = "F314F397F7";

// Pin Definitions
const int LED_PIN = 1;          // Onboard, blue LED, clashes with Txmt
const int DIGITAL_PIN = 2;      //GPIO-2

WiFiServer server(80);
//----------------------------

void setup() {        // run once
  initHardware();
  setupWiFi();
  server.begin();
  Serial.println("Server started");
}
//----------------------------

// Main loop
void loop() {
  // Listen for incoming clients
  WiFiClient client = server.available();
  if (client.connected()) {
    Serial.println("New client");

    // Get the number of bytes available for reading by calling available(),
    // that is, the amount of data that has been written to the client by the server it is connected to.
    while (!client) {
      delay(1);
      yield();
    }
    String req = client.readStringUntil('\r');
    Serial.println(req);

    int val;
    // Check if a valid command and value, update I/O and client
    if (getCommand(req, &val)) {
      //   digitalWrite(DIGITAL_PIN, val);
      //    client.print(genPage(val));
      client.print(getPage());
    }
    else {
      Serial.println("Invalid request");
    }

    // Discard any bytes that have been written to the client but not yet read.
    client.flush();
  }
}

//----------------------------

void initHardware() {
  Serial.begin(115200);
  pinMode(DIGITAL_PIN, OUTPUT);
  digitalWrite(DIGITAL_PIN, LOW);
}
//----------------------------

void setupWiFi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());
}
//----------------------------

/*
    Get the command
    If valid return true other wise return false,
    the value of the command is returned in val.
*/
boolean getCommand(String s, int* val) {

  boolean result = true;

  if (s.indexOf("/gpio/0") != -1)
    *val = 0;
  else if (s.indexOf("/gpio/1") != -1)
    *val = 1;
  else
    result = false;

  return result;
}
//----------------------------

/*
    Create HTML page that contains the status.
    If valid return true other wise return false,
    the value of the command is returned in val.
*/
String genPage(int val) {

  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val) ? "high" : "low";

  return s;
}
//----------------------------



String getPage() {
  // Prepare the response. Start with the common header:
  String page;
  page += "<html lang=en_AU>";
  page += "<head>";
  page += "<meta http-equiv='refresh' content='10'/>";
  page += "<title>ESP8266 Demo</title>";
  page += "<style> body {background-color: #ffffff; font-family: Arial, Helvetica, Sans-Serif; Color: 000088;  }</style>";
  page += "</head>";

  page += "<body>";
  page += "<h1>ESP8266 Demo</h1>";
  page += "<h3>GPIO</h3>";
  page += "<form action='/' method='POST'>";
  page += "<ul><li>D3 (Current status: LedStatus)";
  page += "<INPUT type='radio' name='LED' value='1'>ON";
  page += "<INPUT type='radio' name='LED' value='0'>OFF";
  page += "</ul></li>";
  page += "<INPUT type='submit' value='Actualiser'>";
  page += "</body>";
  page += "</html>";
  return page;
}
//----------------------------



//End of file
