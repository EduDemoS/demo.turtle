/*
* This file is part of the EduDemoS Solar Turtle which is
* co-funded by the European Union.
* Copyright (C) 2025  Gerda Stetter Stiftung - Technik macht Spaß!
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


/* To setup the project:
    1. Create a copy of configuration.default.cpp
    2. Name the copy "configuration.cpp"
    3. Adjust the settings in configuration.cpp according to your needs 
       (look for @todo-comments) */
#include "configuration.cpp"

// Include libraries
#include <stdlib.h>

#if LCD_ENABLE
# include <LiquidCrystal_I2C.h>// include the library for the LCD
#endif

#include <WiFiSecureClientProvider.h>
#include <MqttClient.h>
#include <SimpleSoftTimer.h>

#define V_REF 1.0 // scale for the analog pin of the ESP
#define SENSOR_MAX_OUTPUT_VOLTAGE 1.0 // max. output of the voltage-sensor
#define SENSOR_INPUT_MAX_VOLTAGE 5.0 // max. input into the voltage-sensor

using namespace HolisticSolutions;
using namespace HolisticSolutions::WiFi;
using namespace HolisticSolutions::Mqtt;

#define DEMONSTRATOR_TYPE_NAME  "TURTLE"
#define MQTT_DEVICE_NAME        DEMONSTRATOR_TYPE_NAME MQTT_TEAM_ID

//////////////////////// Local functions //////////////////////////////////////
static void SetupWiFi(WiFiManager &wifi);
static void SetupMqtt(MqttClient &mqtt);

static void TaskCommunication();
static void TaskHeartbeat();

static void LogMessage(const char *topic, const void *data, size_t len);

// Message handler for updates through MQTT
static MQTT_MESSAGE_HANDLER_DECLARE(OnMirrorReceived);

static LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address and the number of columns and rows

static WiFiSecureClientProvider wifi;
static MqttClient               mqtt(wifi);

static SimpleSoftTimer  _timeout(MQTT_UPDATE_PERIOD);
static SimpleSoftTimer  _readTimeout(200);
static bool             _toggle = false;
static bool             _mirror = false;

// convert the analog value given by the ESP (0-1023) into the output voltage of the voltage sensor
float convertToVoltage(int analogValue) {
  return analogValue * (V_REF / 1023) * 3.3;
}

// convert the output voltage of the voltage sensor into the output voltage of the solar panel
float calculateVoltageSolar(float outputVoltage) {
  return outputVoltage * (SENSOR_INPUT_MAX_VOLTAGE / SENSOR_MAX_OUTPUT_VOLTAGE);
}

template <typename T>
void UpstreamSendDatapoint(const char *topic, const T& value, bool retain = false) {
  JsonDocument data;
  data["value"] = value;
  mqtt.publish(topic, data, retain);
}

void setup() {
  Serial.begin(9600); // only needed for debugging
  
  // Let some of the hardware settle.
  delay(10);

  Serial.println("Booting EduDemoS Turtle IoT demo");

  Serial.print("Resetting WiFi...");
  wifi.reset();
  Serial.println("done");

  Serial.print("Resetting MQTT client...");
  mqtt.reset();
  Serial.println("done");
    
  SetupWiFi(wifi);
  SetupMqtt(mqtt);

  /* Register topic handlers. As The simplified MQTT client does not support 
     wild cards, we have to subscribe for every single LED explicitly. */
  {
    mqtt.subscribe("ctrl/mirror", MQTT_MESSAGE_HANDLER_NAME(OnMirrorReceived), 0);
  }

#if LCD_ENABLE
  lcd.begin();
#endif 
}

void loop() {
  TaskCommunication();
  TaskHeartbeat();

  if (_readTimeout.isTimeout()) {
    int analogValue = analogRead(A0); //read the current value given by the analog pin of the ESP
    float outputVoltage = convertToVoltage(analogValue);
    float voltage_solar = calculateVoltageSolar(outputVoltage);

    /* only needed for debugging
    Serial.print("voltage_solar: ");
    Serial.print(voltage_solar, 2);
    Serial.println(" V");
    */

    UpstreamSendDatapoint("data/voltage", voltage_solar);

#if LCD_ENABLE
    lcd.clear(); // Clears the display
    lcd.setCursor(0,0);  // set the cursor to column 0, line 0
    lcd.print("Current Voltage");
    lcd.setCursor(0,1);  // set the cursor to column 0, line 1
    lcd.print(voltage_solar, 2);
    lcd.setCursor(5,1);
    lcd.print("V");
#endif

    _readTimeout.restart();
  }
}

static void TaskCommunication() {
  wifi.run();
  if (wifi.connected()) {
    mqtt.run();
  }
}

static void TaskHeartbeat() {
  if (_timeout.isTimeout()
      && mqtt.connected()) {
    JsonDocument heartbeat;

    heartbeat["state"] = _toggle ? "on" : "off";
    heartbeat["mirror"] = _mirror;

    mqtt.publish("data/heartbeat", heartbeat);
    
    _toggle = !_toggle;
    _timeout.restart();
  }
}

static MQTT_MESSAGE_HANDLER_DECLARE(OnMirrorReceived) {
  const char *payload = (const char *)data;

  LogMessage(topic, data, len);

  _mirror = strncmp("on", payload, 2) == 0;
  mqtt.publish("data/mirror", _mirror ? "off" : "on");
}

static void LogMessage(const char *topic, const void *data, size_t len) {
  const char *message = (const char *)data;
  String text;
  
  text.concat(message, len);
  
  Serial.print("Topic: '");
  Serial.print(topic);
  Serial.println("' Message: '" + text + "'");
}

static void SetupWiFi(WiFiManager &wifi)
{
  Serial.print("Connecting to WiFi network '");
  Serial.print(WIFI_SSID);
  Serial.print("' .");

  wifi.reset();
  wifi.connect(WIFI_SSID, WIFI_PASSWORD);
}

static void SetupMqtt(MqttClient &mqtt)
{
  Serial.println("Initiating connection to MQTT broker");
  // Do not validate the server certificate
  // @warning Only for experimental environments, not for production use.
  mqtt.InsecureAccept();
  
  if ((strlen(MQTT_WORKSHOP_ID) > 0) 
      && (strcmp(MQTT_WORKSHOP_ID, "undefined") != 0))
  {
    mqtt.TopicPrefixSet("EduDemoS/" MQTT_WORKSHOP_ID "/" MQTT_DEVICE_NAME);
  }
  else 
  {
    Serial.println("WARNING: Workshop ID not set");
    mqtt.TopicPrefixSet("EduDemoS/WSxxx/" MQTT_DEVICE_NAME);
  }

  mqtt.CredentialsSet(MQTT_USERNAME, MQTT_PASSWORD);
  mqtt.connect(MQTT_DEVICE_NAME, MQTT_SERVER, MQTT_PORT);
}
