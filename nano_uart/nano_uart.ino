#include <SoftwareSerial.h>

// Pins: D8 = RX (← CrowPanel TX), D9 = TX (→ CrowPanel RX)
SoftwareSerial commSerial(8, 9);  // RX, TX

#define ANALOG_DISCHARGE_PIN A0
#define ANALOG_CHARGE_PIN A1
#define ANALOG_TEMP_PIN A2
#define ANALOG_VOLTAGE_SOLAR_PIN A3
#define ANALOG_VOLTAGE_CHARGER_PIN A4
#define ANALOG_VOLTAGE_GUN_PIN A5

#define NTC_NOMINAL 10000.0       // Resistance at 25°C
#define TEMPERATURE_NOMINAL 25.0  // Temp for nominal resistance
#define B_COEFFICIENT 3950.0      // Beta coefficient (most common is 3950)
#define SERIES_RESISTOR 10000.0   // Value of the fixed resistor
#define ADC_MAX 1023.0            // 10-bit ADC

void setup() {
  Serial.begin(115200);       // USB debugging
  commSerial.begin(115200);   // To CrowPanel
  delay(300);

  Serial.println("Arduino Nano sender started (A-F Format)");
}

float read_adc(int pin)
{
  int adcVal = analogRead(pin);
  if (adcVal <= 0) adcVal = 1;
  if (adcVal >= 1023) adcVal = 1022;
  return (adcVal / ADC_MAX) * 3.3;
}

float convert_steinhart(float pinVoltage)
{
  float resistance = SERIES_RESISTOR * ((3.3 - pinVoltage) / pinVoltage);

  // 4. Steinhart-Hart Equation
  float temp = resistance / NTC_NOMINAL;        // (R/Ro)
  temp = log(temp);                             // ln(R/Ro)
  temp /= B_COEFFICIENT;                        // 1/B * ln(R/Ro)
  temp += 1.0 / (TEMPERATURE_NOMINAL + 273.15); // + (1/To)
  temp = 1.0 / temp;                            // Invert to get Kelvin
  temp -= 273.15;                               // Convert to Celsius

  return temp;
}

void loop() {
  char buf[32];
  char floatStr[10];

  commSerial.println("\n");
  
  //discharge calc
  float disAmp = read_adc(ANALOG_DISCHARGE_PIN) * 11.0 / 25.0;
  // charge calc
  float chAmp = read_adc(ANALOG_CHARGE_PIN) * 11.0;
  //temp calc
  float tempVoltage = read_adc(ANALOG_TEMP_PIN);
  float temp = convert_steinhart(tempVoltage);

  //voltages
  float v_out_solar = read_adc(ANALOG_VOLTAGE_SOLAR_PIN) * 11.0;
  float v_out_charger = read_adc(ANALOG_VOLTAGE_CHARGER_PIN) * 11.0;
  float v_out_gun= read_adc(ANALOG_VOLTAGE_GUN_PIN) * 11.0;

  // A: Discharge Current
  dtostrf(disAmp, 4, 2, floatStr);
  snprintf(buf, sizeof(buf), "A%s A", floatStr);     
  commSerial.println(buf);
  Serial.println(buf);

  // B: Charge Current
  dtostrf(chAmp, 4, 2, floatStr);
  snprintf(buf, sizeof(buf), "B%s A", floatStr);   
  commSerial.println(buf);
  Serial.println(buf);

  // C: Temperature
  dtostrf(temp, 4, 1, floatStr); 
  snprintf(buf, sizeof(buf), "C%s C", floatStr);  
  commSerial.println(buf);
  Serial.println(buf);

  // D: Voltage Solar
  dtostrf(v_out_solar, 4, 2, floatStr); 
  snprintf(buf, sizeof(buf), "D%s V", floatStr);  
  commSerial.println(buf);
  Serial.println(buf);

  // E: Voltage Charger
  dtostrf(v_out_charger, 4, 2, floatStr);
  snprintf(buf, sizeof(buf), "E%s V", floatStr);   
  commSerial.println(buf);
  Serial.println(buf);

  // F: Voltage Gun
  dtostrf(v_out_gun, 4, 2, floatStr);
  snprintf(buf, sizeof(buf), "F%s V", floatStr);  
  commSerial.println(buf);
  Serial.println(buf);

  delay(1200);   
}