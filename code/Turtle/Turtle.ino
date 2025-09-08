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

#include <stdlib.h>
#include <LiquidCrystal_I2C.h>// include the library for the LCD

#define V_REF 1.0 // scale for the analog pin of the ESP
#define SENSOR_MAX_OUTPUT_VOLTAGE 1.0 // max. output of the voltage-sensor
#define SENSOR_INPUT_MAX_VOLTAGE 5.0 // max. input into the voltage-sensor

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address and the number of columns and rows

// convert the analog value given by the ESP (0-1023) into the output voltage of the voltage sensor
float convertToVoltage(int analogValue) {
  return analogValue * (V_REF / 1023) * 3.3;
}

// convert the output voltage of the voltage sensor into the output voltage of the solar panel
float calculateVoltageSolar(float outputVoltage) {
  return outputVoltage * (SENSOR_INPUT_MAX_VOLTAGE / SENSOR_MAX_OUTPUT_VOLTAGE);
}


void setup() {
  //Serial.begin(9600); // only needed for debugging
  lcd.begin();
}

void loop() {
  int analogValue = analogRead(A0); //read the current value given by the analog pin of the ESP
  float outputVoltage = convertToVoltage(analogValue);
  float voltage_solar = calculateVoltageSolar(outputVoltage);
  /* only needed for debugging
  Serial.print("voltage_solar: ");
  Serial.print(voltage_solar, 2);
  Serial.println(" V");
  */
  lcd.clear(); // Clears the display
  lcd.setCursor(0,0);  // set the cursor to column 0, line 0
  lcd.print("Current Voltage");
  lcd.setCursor(0,1);  // set the cursor to column 0, line 1
  lcd.print(voltage_solar, 2);
  lcd.setCursor(5,1);
  lcd.print("V");
  delay(200); // wait 0.2 seconds before refreshing

}
