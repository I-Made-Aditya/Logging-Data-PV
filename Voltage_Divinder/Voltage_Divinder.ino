const int pin = 35;
int ADC_VALUE = 0;
float voltage_value = 0; 
void setup() {
  Serial.begin(115200);
}
void loop() {
  ADC_VALUE = analogRead(pin);
  Serial.print("ADC VALUE = ");
  Serial.println(ADC_VALUE);
  float x = f_map(ADC_VALUE, 0, 4095, 0, 18);
  //voltage_value = (ADC_VALUE * 5.0 ) / (1023.0);
  Serial.print("Voltage = ");
  Serial.print(x);
  Serial.println(" volts");
  Serial.println(" ==============");
  delay(1000);
}

float f_map(float x, float x1, float x2, float y1, float y2){
  return (x - x1) * (y2 - y1) / (x2 - x1) + y1;
}
