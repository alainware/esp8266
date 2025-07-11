/******************************************************************************
 * Project:     ESP8266 FIREBASE LED
 * File:        led.ino
 * Author:      Irving Aguilar, M.Sc. Software Engineering
 * Created:     2025-07-10
 * Version:     1.0
 * Description: Basic program to show the connection between the ESP8266 board and Firebase platform.
 * 
 * Copyright (c) [2025] Irving Aguilar
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
 ******************************************************************************/

// ESP8266 LED
#define LED_PIN 2

// Load correct library, depending on the ESP board type
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif

// Load Firebase Arduino Client Library for ESP8266 and ESP32 (v.4.4.14)
#include <Firebase_ESP_Client.h>

// Define WiFi Credentials
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "Password*"

// Globals
bool bulb1;

// Provide token generation process information
#include "addons/TokenHelper.h"

// Provide RTDB payload printout information and other support functions
#include "addons/RTDBHelper.h"

// Firebase Project API Key
#define API_KEY "apikey"

// RTDB URL
#define DATABASE_URL "yourprojectrtdb.firebaseio.com"

// Firebase Data Object
FirebaseData fbdo;

// Firebase Authentication Object
FirebaseAuth auth;

// Firebase Config Object
FirebaseConfig config;

// Millis variable to send/store data in Firebase database
unsigned long sendDataPrevMillis = 0;
const long timerDelay = 1000; // 1 sec delay

// Boolean variable for registration status
bool signupOK = false;

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    // Connect to WiFi Network
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.println("------- WiFi connection in process ------");
    Serial.print("Connected to: ");
    Serial.println(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        digitalWrite(LED_PIN, HIGH);
        delay(250);
        digitalWrite(LED_PIN, LOW);
        delay(250);
    }
    digitalWrite(LED_PIN, LOW);
    Serial.println();

    // Verify if the connection was successful
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Unable to connect to WiFi.");
    }
    else
    {
        Serial.println("Connected to WiFi. IP address: ");
        Serial.println(WIFI_SSID);
        Serial.print("Assigned IP Address: ");
        Serial.println(WiFi.localIP()); // Print IP Address
        Serial.println("---------------");
    }

    // Assign API Key
    config.api_key = API_KEY;

    // Assign RTDB URL
    config.database_url = DATABASE_URL;

    // Register to Firebase
    Serial.println();
    Serial.println("---- Firebase registration in process ----");
    Serial.print("New user... ");
    if (Firebase.signUp(&config, &auth, "", ""))
    {
        Serial.println("Ok");
        signupOK = true;
    }
    else
    {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
    }
    Serial.println("---------------");

    // Assign callback function for token state
    config.token_status_callback = tokenStatusCallback;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void loop()
{
    // Send data to Firebase if it is ready, registered and the specified time has elapsed.
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0))
    {
        sendDataPrevMillis = millis();

        // Get boolean value from RTDB
        Firebase.RTDB.getBool(&fbdo, F("/casa/foco1"), &bulb1);

        if (bulb1 == false)
        {
            // Turn off the board LED
            digitalWrite(LED_PIN, HIGH);
        }
        else
        {
            // Turn on the board LED
            digitalWrite(LED_PIN, LOW);
        }
    }
}