#define LDR_PIN 35
#define MAX_ADC_READING 1023
#define ADC_REF_VOLTAGE 5
#define REF_RESISTANCE 5030 // measure this for best results
#define LUX_CALC_SCALAR 12518931
#define LUX_CALC_EXPONENT -1.405

void setup(void)
{
  Serial.begin(115200);

}
void loop(void)
{
  int ldrRawData;
  float resistorVoltage, ldrVoltage;
  float ldrResistance;
  float ldrLux;
  // Perform the analog to digital conversion
  ldrRawData = analogRead(LDR_PIN);
  int ADCto=map(ldrRawData, 0, 4095, 4095, 0);
  // RESISTOR VOLTAGE_CONVERSION
  // Convert the raw digital data back to the voltage that was measured on the analog pin
  resistorVoltage = (float)ADCto / MAX_ADC_READING * ADC_REF_VOLTAGE;
  // voltage across the LDR is the 5V supply minus the 5k resistor voltage
  ldrVoltage = ADC_REF_VOLTAGE - resistorVoltage;
  // LDR_RESISTANCE_CONVERSION
  // resistance that the LDR would have for that voltage
  ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
  // LDR_LUX
  // Change the code below to the proper conversion from ldrResistance to
  // ldrLux
  ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
  // print out the results
  float irr = (ldrLux*0.0079);
  Serial.print("LDR Raw Data : "); Serial.println(ADCto);
  Serial.print("LDR Voltage : "); Serial.print(ldrVoltage); Serial.println(" volts");
  Serial.print("LDR Resistance : "); Serial.print(ldrResistance, 4); Serial.println(" Ohms");
  Serial.print("LDR Illuminance: "); Serial.print(ldrLux); Serial.println(" lux");
  Serial.print("LDR iRadiance : "); Serial.print(irr); Serial.println(" W/m2");
  Serial.println("====================================");
  delay(1000);
}
