#include <FS.h>
#include <LittleFS.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "index.h" 

#define EEPROM_SIZE 9
ESP8266WebServer server(80);
int relay7 = 14, relay8 = 16;
int state7 = HIGH, state8 = HIGH;

char ssid[30] = "Space";
char pass[30] = "SpaceSpace";
const char* Apssid = "ESP8266-Relay";
const char* Appassword = "SpaceSpace";
  IPAddress ip(10, 74, 8, 15);
  IPAddress dns(94, 140, 14, 141);
  IPAddress gateway(10, 74, 8, 1);
  IPAddress gatewayap(10, 74, 8, 1);
  IPAddress subnet(255, 255, 252, 0);

File myfile;
String text;
long savednum = 0 ,passnum = 0; 

void(* resetFunc) (void) = 0;//declare reset function at address 0

void getWifi() {
  WiFi.config(ip, dns, gateway, subnet); 
  WiFi.begin(ssid, pass);
  int xc = 0;
  while (WiFi.status() != WL_CONNECTED && xc < 10) {
    delay(500);
    xc++;
  }
    Serial.println("");
  if (WiFi.status() == WL_CONNECTED) { 
    Serial.print("CONNECTED To: ");
    Serial.println(ssid);
    Serial.print("IP Address: http://");
    Serial.println(WiFi.localIP().toString().c_str());
    WiFi.softAPdisconnect(true);
    } else { 
    WiFi.softAPConfig(ip, gatewayap, subnet);
    WiFi.softAP(Apssid, Appassword);
    Serial.print("CONNECTED To: ");
    Serial.print(Apssid);
    Serial.print(" Password: ");
    Serial.println(Appassword);
    Serial.print("IP Address: http://");
    Serial.println(WiFi.softAPIP().toString().c_str());
    }
}
void handleRoot() {
    String s = MAIN_page; //Read HTML contents
    server.send(200, "text/html", s); //Send web page
}
void handleWifi() { 
  handleRoot();
  text = server.arg("usname");
  myfile = LittleFS.open("/Ssid.txt", "w");
    if(myfile){ 
      myfile.print(text); 
    }
    myfile.close();
  int len = text.length() + 1; 
  text.toCharArray(ssid, len);
  text = server.arg("pssw");
  myfile = LittleFS.open("/Password.txt", "w");
    if(myfile){ 
      myfile.print(text);
    }
    myfile.close();
  len = text.length() + 1;
  text.toCharArray(pass, len); 
  getWifi();
}

void handlestate7() {
    handleRoot();
    state7 = !state7;
    digitalWrite(relay7, state7);
    EEPROM.write(7, state7);
    EEPROM.commit();
  }
void handlestate8() {
    handleRoot();
    state8 = !state8;
    digitalWrite(relay8, state8);
    EEPROM.write(8, state8);
    EEPROM.commit();
  }
void handlestate() {
  String content = "<?xml version = \"1.0\" ?>";
  content += "<inputs><analog>";
  content += state7;
  content += "</analog><analog>";
  content += state8;
  content += "</analog></inputs>";
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/xml", content); //Send web page
}
void handleRestesp() {  
    handleRoot();
    delay(1000);     
    resetFunc();
}
void getssid() { 
myfile = LittleFS.open("/Ssid.txt", "r");
 if(!myfile){
   return;
 }
int nb = 0;
while (myfile.available()) {
  savednum = myfile.read();
  ssid[nb] = (char) savednum;
  nb += 1;
}
myfile.close();
}
void getpass() { 
myfile = LittleFS.open("/Password.txt", "r");
 if(!myfile){
   return;
 }
int nb = 0;
while (myfile.available()) {
  savednum = myfile.read();
  pass[nb] = (char) savednum;
  nb += 1;
}
myfile.close();
}
void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  LittleFS.begin();
  getssid();
  getpass();
  WiFi.mode(WIFI_STA); 
  getWifi();
  pinMode(relay7, OUTPUT);
  pinMode(relay8, OUTPUT);
  server.on("/", handleRoot);   
  server.on("/Mywifi", handleWifi);
  server.on("/LED7", handlestate7); 
  server.on("/LED8", handlestate8);  
  server.on("/redstate", handlestate); 
  server.on("/restesp", handleRestesp);
  server.begin();
  getstate();
}
void loop() {
  server.handleClient(); 
  delay(50); 
}
void setrelaystate() { 
    digitalWrite(relay7, state7);
    digitalWrite(relay8, state8);
}
void getstate() { 
    state7 = EEPROM.read(7);
    state8 = EEPROM.read(8);
    setrelaystate();
}
