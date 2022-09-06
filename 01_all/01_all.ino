#include "FS.h"
#include "SD.h"
#include "max6675.h"
#include <SPI.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

uint64_t uS_TO_S_FACTOR = 1000000;
uint64_t TIME_TO_SLEEP = 30;

const char* ssid     = "WS Clown Project 2.4G";
const char* password = "membadutbersama";

RTC_DATA_ATTR int sensor_data = 0;

#define LUX_CALC_SCALAR 12518931
#define LUX_CALC_EXPONENT -1.405
#define REF_RESISTANCE 5030

String Data;

#define LDR_PIN 36
#define voldivPin 35
#define acs_pin 34

#define thermoSO 32
#define thermoSCK 25

//sensor thermocouple 1
#define thermo1CS 33
float temp1 = 0;
MAX6675 thermocouple1(thermoSCK, thermo1CS, thermoSO);

//sensor thermocouple 2
#define thermo2CS 15
float temp2 = 0;
MAX6675 thermocouple2(thermoSCK, thermo2CS, thermoSO);

//sensor thermocouple 3
#define thermo3CS 4
float temp3 = 0;
MAX6675 thermocouple3(thermoSCK, thermo3CS, thermoSO);

  //sensor thermocouple 4
#define thermo4CS 27
float temp4 = 0;
MAX6675 thermocouple4(thermoSCK, thermo4CS, thermoSO);

float valueCurrent = 0;
float valueVoltage = 0.0; 
float ACSValue = 0.0, acs_samples = 0.0, avgACS = 0.0, BaseVol = 1.52;
 
int   ldrRawData;
float resistorVoltage, ldrVoltage;
float ldrResistance;
float ldrLux;
float irr;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String Date;
String day;
String Time;

void setup() {
  pinMode(34, INPUT);
  Serial.begin(115200);
  SPI.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");

  timeClient.begin();
  timeClient.setTimeOffset(25200);
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    return;
  }

  File file = SD.open("/Logging.txt");
  if (!file) {
    Serial.println("File does not exist");
    Serial.println("Creating file...");
    writeFile(SD, "/Logging.txt", "Reading Number, Date, Hour, ValueLDR \r\n");
  }
  else {
    Serial.println("File exists");
  }
  file.close();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  obtainReadings();
  obtain_Date_Time();
  data_logging();
  sensor_data++;
  Serial.println("Sensor data logged successfully! Going to sleep");
  esp_deep_sleep_start();

}

void loop() {
  // put your main code here, to run repeatedly:

}

void obtainReadings() {
  //ACS-Sensor
  for (int y = 0; y < 500; y++) { //This would take 500 Samples
    ACSValue = analogRead(acs_pin);
    acs_samples = acs_samples + ACSValue;
    delay (3);
  }
  avgACS = acs_samples/500;
  valueCurrent=((((avgACS) * (3.3 / 4095.0)) - BaseVol ) / 0.100 );
  absolute(valueCurrent);
  if (valueCurrent < 0.67) {
    valueCurrent = 0;
  }
  delay(100);
  
  //Voltage_Divinder
  int adc_volt = analogRead(voldivPin);
  valueVoltage = f_map(adc_volt, 0, 4095, 0, 18);
  delay(100);

  //LDR to iRadiance
  ldrRawData = analogRead(LDR_PIN);
  int ADCmap = map(ldrRawData, 0, 4095, 0, 1023);
  resistorVoltage = (float)ADCmap / 1023 * 5.0;
  ldrVoltage = 5.0 - resistorVoltage;
  ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
  ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
  irr = (ldrLux*0.0079);
  if(ADCmap>=1023) irr = 0;

  delay(100);

  //thermocouple
  temp1 = thermocouple1.readCelsius();
  delay(100);
  temp2 = thermocouple2.readCelsius();
  delay(100);
  temp3 = thermocouple3.readCelsius();
  delay(100);
  temp4 = thermocouple4.readCelsius();
  delay(100);

  
  Serial.print("LDR iRadiance  : "); Serial.print(irr); Serial.println(" W/m2");
  Serial.print("Current : "); Serial.print(valueCurrent); Serial.println("A");
  Serial.print("Voltage : "); Serial.print(valueVoltage); Serial.println("V");
  Serial.print("Thermocouple 1 = "); Serial.print(temp1); Serial.println("C");
  Serial.print("Thermocouple 2 = "); Serial.print(temp2); Serial.println("C");
  Serial.print("Thermocouple 3 = "); Serial.print(temp3); Serial.println("C");
  Serial.print("Thermocouple 4 = "); Serial.print(temp4); Serial.println("C");
  Serial.println("===========================");
}

void absolute(int x){
  if(x<0){
    x=(-1) * x;
  }
}

float f_map(float x, float x1, float x2, float y1, float y2){
  return (x - x1) * (y2 - y1) / (x2 - x1) + y1;
}

void obtain_Date_Time() {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Date = timeClient.getFormattedTime();
  Serial.println(Date);

  int split = Date.indexOf(" T ");
  day = Date.substring(0, split);
  Serial.println(day);
  Time = Date.substring(split + 1, Date.length() - 1);
  Serial.println(Time);
}


void data_logging() {
  Data = String(sensor_data) + " | " + String(day) + " | " + String(Time) + " | " +
         String(irr) + " W/m2 | " + String(valueCurrent) + " A | " + 
         String(valueVoltage) + " V | " + String(temp1) + " C | " + 
         String(temp2) + " C | " + String(temp3) + " C | " + 
         String(temp4) + " C" "\r\n";
  Serial.print("Save data: ");
  Serial.println(Data);
  appendFile(SD, "/Logging.txt", Data.c_str());
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
