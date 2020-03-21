/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "LEGO Control"
#define APPSK  "lgctrl12345"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
const String head = "<!DOCTYPE html><html><head><title>LEGO Control</title><meta charset=\"UTF-8\"/><style>table{width:100%;} div{width:100%;height:140px;padding-top:60px;font-size:400%;text-align:center;vertical-align:middle;} .status0{background-color:#F88;} .status1{background-color:#8F8;} iframe,body,html{width:100%;height:100%;}</style></head><body>";
const int PIN_NR[] = { 16, 0, 2, 14, 12, 13, 15 };
const String PIN_NAME[] = { "D0", "D3", "D4", "D5", "D6", "D7", "D8" };


bool PIN_VALUE[] = { false, false, false, false, false, false, false };

ESP8266WebServer server(80);
IPAddress local_IP(192,168,168,1);
IPAddress gateway(192,168,168,1);
IPAddress subnet(255,255,255,0);

void sendPage() {
  Serial.print("Sending page...");
  String page = "<!DOCTYPE html>";
  page += "<html><head><title>LEGO Control</title><meta charset=\"UTF-8\"/>";
  page += "<style>table{width:100%;} div{width:100%;height:140px;padding-top:60px;font-size:400%;text-align:center;vertical-align:middle;} .stat0{background-color:#F88;} .stat1{background-color:#8F8;} iframe,body,html{width:100%;height:100%;}</style>";
  page += "<script type=\"text/javascript\">";
  page += "function send(w){var r=new XMLHttpRequest();r.onreadystatechange=function(){if (this.readyState==4){var s=this.responseText;for(var i=0;i<7;i++){document.getElementById('s'+i).setAttribute('class','stat'+s.substr(i,1));}}};r.open('GET',w,true);r.send();}";
  page += "window.setInterval(function(){send('/s');},7500);";
  page += "</script>";
  page += "</head><body>";
  
  page += "<h1>LEGO&copy; Control</h1>";
  
  page += "<table><tr>";
  for (int i = 0; i < 7; i++) {
    if (i == 4) page += "</tr><tr>";
    page += "<td><div onclick=\"send('/t";
    page += i;
    page += "')\" id=\"s";
    page += i;
    page += "\" class=\"stat";
    if (PIN_VALUE[i])
      page += "1";
    else
      page += "0";
    page += "\">" + PIN_NAME[i] + "</div></td>";
  }
  page += "</tr></table>";

  page += "<table><tr><td><div onclick=\"send('/a0')\" class=\"stat0\">All off</div></td><td><div onclick=\"send('/a1')\" class=\"stat1\">All on</div></td></tr></table>";

  page += "</body></html>";
  
  server.send(200, "text/html", page);
  Serial.println("Done.");
}

void handleRoot() {
  sendPage();
}

void handleStatus() {
  Serial.print("Sending status...");
  char stat[7];
  for (int i = 0; i < 7; i++) {
    if (PIN_VALUE[i])
      stat[i] = '1';
    else
      stat[i] = '0';
  }
  server.send(200, "text/plain", stat);
  Serial.println("Done.");
}

void handleToggle(int nr) {
  Serial.print("Toggeling ");
  Serial.print(nr);
  Serial.println(".");
  if (nr >= 0) {
    PIN_VALUE[nr] = !PIN_VALUE[nr];
    digitalWrite(PIN_NR[nr], PIN_VALUE[nr] ? HIGH : LOW);
  }
  handleStatus();
}

void handleToggle0() { handleToggle(0); }
void handleToggle1() { handleToggle(1); }
void handleToggle2() { handleToggle(2); }
void handleToggle3() { handleToggle(3); }
void handleToggle4() { handleToggle(4); }
void handleToggle5() { handleToggle(5); }
void handleToggle6() { handleToggle(6); }

void handleAllOff() {
  Serial.println("Turing all off.");
  for (int i = 0; i < 7; i++) {
    PIN_VALUE[i] = false;
    digitalWrite(PIN_NR[i], LOW);
  }
  handleStatus();
}

void handleAllOn() {
  Serial.println("Turing all on.");
  for (int i = 0; i < 7; i++) {
    PIN_VALUE[i] = true;
    digitalWrite(PIN_NR[i], HIGH);
  }
  handleStatus();
}

void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(PIN_NR[i], OUTPUT);
  }
  
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/s", handleStatus);
  server.on("/t0", handleToggle0);
  server.on("/t1", handleToggle1);
  server.on("/t2", handleToggle2);
  server.on("/t3", handleToggle3);
  server.on("/t4", handleToggle4);
  server.on("/t5", handleToggle5);
  server.on("/t6", handleToggle6);
  server.on("/a0", handleAllOff);
  server.on("/a1", handleAllOn);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
