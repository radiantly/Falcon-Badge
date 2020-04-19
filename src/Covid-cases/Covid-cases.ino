/**
 * Initial-Firmware
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 by Securehardware@bi0s, radiantly
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
// Pin Declarations for the modules in the Kit
const int laserPin = 0;
const int dataPin = 13;   
const int loadPin = 15;  
const int clockPin = 14; 
const int cntrl_1Pin = 12; // Pin D6
const int cntrl_2Pin = 16; // Pin D0

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
// Libraries Included for modules
#include <Wire.h>
#include "SSD1306Wire.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

SSD1306Wire display(0x3c, 4, 5, GEOMETRY_128_32);

// Custom Header Files
#include "fonts.h"

String name;
unsigned long totalCases = 0;
unsigned long deaths = 0;

const char* ssid     = "WiFi Name";
const char* password = "WiFi Password";

/////////////////////////////////////////////////////////////////////
void LED_Matrix() {
  byte data = B00001111;
  digitalWrite(loadPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, data);
  digitalWrite(loadPin, HIGH);
  delay(100);  
}
/////////////////////////////////////////////////////////////////////
void disp_falcon() {
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(Orbitron_Medium_18);
  display.drawString(0,18, "Falcon");
}
/////////////////////////////////////////////////////////////////////
void disp_cases() {
  if(!totalCases) return;
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(Open_Sans_Regular_14);
  display.drawString(0, 0, "Cases in India ");
  display.setFont(Orbitron_Medium_18);
  display.drawString(0,18, String(totalCases));
  display.display();
}
/////////////////////////////////////////////////////////////////////
void disp_deaths() {
  if(!deaths) return;
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(Open_Sans_Regular_14);
  display.drawString(0, 0, "Deaths ");
  display.setFont(Orbitron_Medium_18);
  display.drawString(0,18, String(deaths));
  display.display();
}
/////////////////////////////////////////////////////////////////////
void getInfo() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  HTTPClient http;
    http.begin("http://api.apify.com/v2/key-value-stores/toDWvRj1JpTXiM8FF/records/LATEST?disableRedirect=true");
    int httpCode = http.GET();
    //Check the returning code                                                                  
    if (httpCode > 0) {
      // Get the request response payload
      String payload = http.getString();
      Serial.println(payload);

      const size_t capacity = JSON_OBJECT_SIZE(7) + 230;
      DynamicJsonDocument doc(capacity);

      deserializeJson(doc, payload);
      
      totalCases = doc["totalCases"];
      deaths = doc["deaths"];
      Serial.println(totalCases);
    } else {
      Serial.println(httpCode);
    }
    http.end();
}

//////////////////////////////////////////////////////////////////////////
void setup() {
  pinMode(laserPin,OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(loadPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  digitalWrite(laserPin, LOW);
  // Debug Enable
  Serial.begin(9600);
  // Display Init
  display.init();
  display.flipScreenVertically();

  name="bufferOverflow";

  WiFi.begin(ssid, password);


  LED_Matrix();
  
  display.clear();
  disp_falcon();
  display.display();

  getInfo();

  // Leaving the original falcon flag here
  Serial.println("falcon{g3t_start3d_w!th_h4rdw4r3}");
}

void toggleDisplay() {
  static bool cases = true;

  if(cases) {
    disp_cases();
  } else {
    disp_deaths();
  }

  cases = cases ? false : true;
}

/////////////////////////////////////////////////////////////////////
unsigned long last_refresh = 0;
unsigned long last_toggle = 0;

void loop() {
  if(millis() > last_refresh + 6000000) {
    last_refresh = millis();
    Serial.println("Running!");
    getInfo();
  }

  if(millis() > last_toggle + 5000) {
    last_toggle = millis();
    
    toggleDisplay();
  }
}
/////////////////////////////////////////////////////////////////////
