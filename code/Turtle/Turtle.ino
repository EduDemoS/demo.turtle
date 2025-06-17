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
