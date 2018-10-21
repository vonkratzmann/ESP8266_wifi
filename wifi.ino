#include <ESP8266WiFi.h>

// Used to enable/disable debug text to serial monitor
const boolean DEBUG_WIFI = false;

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

  // Shutdown serial comms, so can use the led
  disableSerialComms();
  // Enable led hardware so we can turn led on and off
  enableLedHw();
} //----------------------------


// Main loop
void loop() {
  // Listen for incoming clients
  WiFiClient wiFiClient = server.available();
  if (wiFiClient) {
    if (DEBUG_WIFI) Serial.println("Client Connected");

    while (wiFiClient.connected()) {
      processIncomingData(wiFiClient);
    }
    delay(100); // Give the web server time to receive the data
  }
  // Close the connection
  wiFiClient.stop();
  if (DEBUG_WIFI) Serial.println("Client disconnected");
} //----------------------------


void initHardware() {
  Serial.begin(115200);
  pinMode(DIGITAL_PIN, OUTPUT);
  digitalWrite(DIGITAL_PIN, LOW);
} //----------------------------


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
} //----------------------------


void disableSerialComms() {
  Serial.println("Disabling serial comms");
  // Give the serial monitor time to transmit the data
  delay(500);
  Serial.flush();
  Serial.end();
} //----------------------------


void enableLedHw() {
  pinMode(LED_PIN, OUTPUT);
  // Turn led off
  digitalWrite(LED_PIN, HIGH);
} //----------------------------


void processIncomingData(WiFiClient connectedClient) {
  // Get the number of bytes available for reading by calling available(),
  // that is, the amount of data that has been written by the connectedclient to the server.
  if (connectedClient.available()) {
    String req = connectedClient.readStringUntil('\r');
    if (DEBUG_WIFI) Serial.println(req);

    int val;
    // Check if a valid command and value, update I/O and client if debugging enabled
    if (getCommand(req, &val)) {
      digitalWrite(LED_PIN, !val);    // Complement val as writing a zero turns led on
      // connectedClient.print(genPage(val));
      // connectedClient.print(getPage());
    }
    else {
      if (DEBUG_WIFI) Serial.println("Invalid request");
    }

    // Discard any bytes that have been written to the client browser but not yet read.
    connectedClient.flush();
  }
} //----------------------------


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
} //----------------------------


/*
    Create HTML page that contains the status.
    If valid return true other wise return false,
    the value of the command is returned in val.
*/
String genPage(int val) {

  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now ";
  s += (val) ? "high" : "low";

  return s;
} //----------------------------


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
} //----------------------------


//End of file
