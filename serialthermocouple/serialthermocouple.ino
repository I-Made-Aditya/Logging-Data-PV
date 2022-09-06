// this example is public domain. enjoy!
// https://learn.adafruit.com/thermocouple/

#include "max6675.h"

int thermoSO = 32;
int thermoSCK = 25;

/*int thermo1CS = 33;
float temp1=0;
MAX6675 thermocouple1(thermoSCK, thermo1CS, thermoSO);

int thermo2CS = 27;
float temp2 = 0;
MAX6675 thermocouple2(thermoSCK, thermo2CS, thermoSO);
*/
int thermo3CS = 22;
float temp3 = 0;
MAX6675 thermocouple3(thermoSCK, thermo3CS, thermoSO);

/*int thermo4CS = 17;
float temp4 = 0;
MAX6675 thermocouple4(thermoSCK, thermo4CS, thermoSO);
*/
void setup() {
  Serial.begin(115200);
  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  // basic readout test, just print the current temp
  /*temp1 = thermocouple1.readCelsius();
  delay(100);
  temp2 = thermocouple2.readCelsius();
  delay(100);*/
  temp3 = thermocouple3.readCelsius();
  delay(100);
  /*temp4 = thermocouple4.readCelsius();
  delay(100);
   */
   /*Serial.print("Thermocouple 1 = "); 
   Serial.print(temp1);
   Serial.println("C");
   Serial.print("Thermocouple 2 = "); 
   Serial.print(temp2);
   Serial.println("C");*/
   Serial.print("Thermocouple 3 = "); 
   Serial.print(temp3);
   Serial.println("C");
   /*Serial.print("Thermocouple 4 = "); 
   Serial.print(temp4);
   Serial.println("C");*/
   Serial.println("========================");


   // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
   delay(1000);
}
