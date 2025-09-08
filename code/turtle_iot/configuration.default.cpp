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

#define LCD_ENABLE          1                       // Enable LCD display functionality

/******************************* WIFI **************************************/
/* @todo Update the WiFi settings according to your secrets-sheet */
#define WIFI_SSID           "SECRET_WIFI_SSID"      //  Name of your WiFi network
#define WIFI_PASSWORD       "SECRET_WIFI_PASSWORD"  //  Password of your WiFi network
#define WIFI_MAX_ATTEMPTS   20                      //  How many times to attempt to connect
                                                    //  before resuming without WiFi

/************************ MQTT Broker Config *******************************/
/* @todo Update the MQTT settings according to your secrets-sheet */
#define MQTT_WORKSHOP_ID        "undefined"                 // Workshop ID as provided 
#define MQTT_TEAM_ID            "xx"                        // Team ID (two digits, leading zero - e.g. "01" or "10")
#define MQTT_USERNAME           "MQTT_USERNAME"             // MQTT Username
#define MQTT_PASSWORD           "MQTT_PASSWORD"             // MQTT Password

#define MQTT_SERVER             "iot-mqtt-broker.gbssg.ch"  // MQTT Broker Address
#define MQTT_PORT               8883                        // MQTT Broker Port
#define MQTT_UPDATE_PERIOD      1000                        // [ms] Time to wait before sending data to MQTT
#define MQTT_MAX_ATTEMPTS       10
