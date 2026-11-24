/*
 * ESP8266 Wi-Fi signal strength to ThingSpeak
 *
 * Sends the Wi-Fi signal strength (RSSI, in dBm) to Field 1 of a ThingSpeak channel every 20 seconds.
 * Nothing to wire, so it is a good first test that your board, your network, and your channel all work.
 * It uses the official ThingSpeak library by MathWorks and sends over HTTPS, checking ThingSpeak's certificate.
 *
 * You need:
 *   - Board: any ESP8266, such as a NodeMCU 1.0 (ESP-12E) or a WeMos D1 mini
 *   - Board package: "esp8266 by ESP8266 Community" (Boards Manager URL:
 *     https://arduino.esp8266.com/stable/package_esp8266com_index.json), version 3.1.2
 *   - Library: "ThingSpeak by MathWorks" (Arduino Library Manager), version 2.1.1
 *   - Nothing else: no wiring
 *
 * Set up:
 *   1. Make a free account at https://thingspeak.mathworks.com and create a channel with Field 1 turned on.
 *   2. Put your Wi-Fi name and password, the channel number, and the channel's Write API Key in secrets.h.
 *   3. Upload, then open the Serial Monitor at 115200 baud.
 *
 * Tutorial: https://nothans.com/thingspeak-tutorials/esp8266/building-the-internet-of-things-with-the-esp8266-wi-fi-module-and-thingspeak
 * Examples: https://github.com/nothans/thingspeak-esp-examples
 * By Hans Scharler (https://nothans.com). MIT License.
 */

#define TS_ENABLE_SSL   // send to ThingSpeak over HTTPS
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>
#include "secrets.h"
#include "thingspeak_root_ca.h"

#include "ThingSpeak.h"   // include after the client headers and TS_ENABLE_SSL

const unsigned long channelNumber = SECRET_CH_ID;
const char *writeAPIKey = SECRET_WRITE_APIKEY;

// ThingSpeak's free plan accepts one update every 15 seconds; 20 leaves room.
const unsigned long updateIntervalMs = 20000;

WiFiClientSecure client;
BearSSL::X509List thingspeakRootCa(THINGSPEAK_ROOT_CA);

void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.print("Connecting to ");
  Serial.print(SECRET_SSID);
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 30000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? " connected." : " not connected yet, will retry.");
}

// Certificates have start and end dates, so the board needs the real time before it can check one.
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Setting the clock");
  unsigned long start = millis();
  while (time(nullptr) < 1700000000 && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(time(nullptr) >= 1700000000 ? " done." : " no answer from the time servers yet.");
}

void setup() {
  Serial.begin(115200);
  delay(100);
  WiFi.mode(WIFI_STA);
  connectWiFi();
  if (WiFi.status() == WL_CONNECTED) setClock();
  client.setTrustAnchors(&thingspeakRootCa);   // only trust ThingSpeak's certificate authority
  ThingSpeak.begin(client);
}

void loop() {
  connectWiFi();
  if (WiFi.status() == WL_CONNECTED) {
    if (time(nullptr) < 1700000000) setClock();
    long value = WiFi.RSSI();   // signal strength in dBm: -50 is strong, -80 is weak

    // Write the value to Field 1 of your channel. 200 means ThingSpeak accepted it.
    int status = ThingSpeak.writeField(channelNumber, 1, value, writeAPIKey);
    if (status == 200) {
      Serial.print("Wi-Fi signal (dBm): ");
      Serial.print(value);
      Serial.println(" sent to ThingSpeak.");
    } else {
      // -301 could not connect (check the network, or the certificate), -401 sent too soon, 400 wrong Write API Key.
      Serial.print("ThingSpeak write failed, code ");
      Serial.println(status);
    }
  }
  delay(updateIntervalMs);
}
