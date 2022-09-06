#include "FS.h"
#include "SD.h"
#include <SPI.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

uint64_t uS_TO_S_FACTOR = 1000000; 
uint64_t TIME_TO_SLEEP = 10;

const char* ssid     = "WS Clown Project 2.4G";
const char* password = "membadutbersama";

RTC_DATA_ATTR int sensor_data = 0;
String Data;

#define LIGHT_SENSOR_PIN 36
float valueLDR;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String Date;
String day;
String Time;


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  
  timeClient.begin();
  timeClient.setTimeOffset(25200);

  if(!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("SD card initialization failed!");
    return;    
  }

  File file = SD.open("/Logging.txt");
  if(!file) {
    Serial.println("File does not exist");
    Serial.println("Creating file...");
    writeFile(SD, "/Logging.txt", "Reading Number, Date, Hour, ValueLDR \r\n");
  }
  else {
    Serial.println("File exists");  
  }
  file.close();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  //sensors.begin(); 
  obtainReadings();
  obtain_Date_Time();
  data_logging();
  
  sensor_data+=1;
  Serial.println("Sensor data logged successfully! Going to sleep");
  esp_deep_sleep_start(); 
 
}

void loop() {
  // put your main code here, to run repeatedly:

}

void obtainReadings(){
  valueLDR = analogRead(LIGHT_SENSOR_PIN);
  //sensors.requestTemperatures(); 
  //temperature = sensors.getTempCByIndex(0); 
  Serial.print("LDR Value: ");
  Serial.println(valueLDR);
}

void obtain_Date_Time() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  Date = timeClient.getFormattedTime();
  Serial.println(Date);
  
  int split = Date.indexOf("T");
  day = Date.substring(0, split);
  Serial.println(day);
  Time = Date.substring(split+1, Date.length()-1);
  Serial.println(Time);
}

void data_logging() {
  Data = String(sensor_data) + "," + String(day) + "," + String(Time) + "," + 
                String(valueLDR) + "\r\n";
  Serial.print("Save data: ");
  Serial.println(Data);
  appendFile(SD, "/Logging.txt", Data.c_str());
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
