void setup()
{
  pinMode(34, INPUT);
  Serial.begin(115200);
}
void loop()
{
  float value=0;
  float ACSValue = 0.0, Samples = 0.0, AvgACS = 0.0, BaseVol = 1.52; //Change BaseVol as per your reading in the first step.
  for (int x = 0; x < 500; x++) { //This would take 500 Samples
    ACSValue = analogRead(34);
    Samples = Samples + ACSValue;
    delay (3);
  }
  AvgACS = Samples/500;
  value=((((AvgACS) * (3.3 / 4095.0)) - BaseVol ) / 0.100 ); //0.066V = 66mVol. This is sensitivity of your ACS module.
  absolute(value);
  if (value < 0.67) {
    value = 0;
  }
  Serial.println(value);
}

void absolute(int x){
  if(x<0){
    x=(-1) * x;
  }
}
