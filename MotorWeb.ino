

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// These are the pins used to control the motor shield

#define LEFT_F 15 // Motor B
#define LEFT_B 13

#define RIGHT_F 2 // Motor A
#define RIGHT_B 0

#define ENA D5
#define ENB D6

// Can be set between 0 and 1023
int drivePower = 1023;

uint8_t driveDirection = HIGH;

uint8_t steerDirection = HIGH;

// ----------------
// Set *YOUR* (pre-existing) WiFi SSID and Password here
// ----------------
const char* ssid = "J_WICK";
const char* password = "bitemebitemebiteme";

ESP8266WebServer server(80);

const char *webpage = 
  #include "motorPage.h"
;

void handleRoot() {
  server.send(200, "text/html", webpage);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){

  pinMode(LEFT_F, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  pinMode(LEFT_B, OUTPUT);
  pinMode(RIGHT_F, OUTPUT);
  pinMode(RIGHT_B, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  analogWrite(ENA, drivePower);
  analogWrite(ENB, drivePower);

  

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("WifiCar")) {
    Serial.println("MDNS Responder Started");
  }

  server.on("/", handleRoot);

  server.on("/forward", [](){
    Serial.println("forward");
    
    analogWrite(LEFT_F, drivePower);
    analogWrite(RIGHT_F, drivePower);
    
    server.send(200, "text/plain", "forward");
  });

  server.on("/driveStop", [](){

    Serial.println("driveStop");
    
    digitalWrite(LEFT_F, 0);
    digitalWrite(RIGHT_F, 0);
    digitalWrite(LEFT_B, 0);
    digitalWrite(RIGHT_B, 0);
    
    server.send(200, "text/plain", "driveStop");
  });

  server.on("/back", [](){
    Serial.println("back");
    
    analogWrite(LEFT_B, drivePower);
    analogWrite(RIGHT_B, drivePower);
    
    server.send(200, "text/plain", "back");
  });

  server.on("/right", [](){
    
    Serial.println("right");
    
    analogWrite(LEFT_F, drivePower);
    analogWrite(RIGHT_B, drivePower);
    
    server.send(200, "text/plain", "right");
  });

  server.on("/left", [](){
    
    Serial.println("left");
    
    analogWrite(RIGHT_F, drivePower);
    analogWrite(LEFT_B, drivePower);
    
    server.send(200, "text/plain", "left");
  });

  server.on("/steerStop", [](){
    
    Serial.println("steerStop");
    
    digitalWrite(LEFT_F, 0);
    digitalWrite(RIGHT_F, 0);
    digitalWrite(LEFT_B, 0);
    digitalWrite(RIGHT_B, 0);
    
    server.send(200, "text/plain", "steerStop");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Server Started");
}

void loop(void){
  server.handleClient();
}
