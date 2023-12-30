/*
TO DO:
FIX ULTRASONIC
FIX GEMISMA ERROR
*/





#define bottlePump 7
#define PumpAdeiasma 11
#define DosingPump 10
#define vana 12
#define SmpotLow 2
#define SmpotHigh 3
#define trigPin 9
#define echoPin 8

const int numMeasurements = 5;      // Number of measurements for the median filter
int remainder = 0;                  // variable to determine median position
int numValidMeasurements = 0;       // Number of valid measurements taken
int waterMpotHigh = 0;              // value of mpotilia high water sensor
int antixloDelay = 1750;            // delay for antixlorio drop
int mpotiliaLow = 0;                // value of mpotilia low water sensor
int time1 = 0;                      // variable to calculate 10 min delay
int time2 = 0;                      // variable to calculate 10 min delay
float validMeasurements[numMeasurements]; // Array to store valid measurements  
float measurements[numMeasurements]; // Array to store measurements
float finalDistance = 0;            // variable to store after all ultrasonic measurements processing
float distance;                     // variable where the distance of the measured object is stored
float UL = 6.5;                       // upper limit for water of aquarium
float SL;                           // level of water at start
float BL;                           // level of water that will be removed
long duration;                      // variable where the the reflection time of the ultrasound is stored                 
bool adeiasmaTelos = false;         // variable to control when mpotilia emptying is finished
bool gemismaMpotiliasTelos = false; // variable to control if mpotilia is full
bool gemismaTankTelos = false;      // variable to control if tank is full
bool dontstart = false;             // variable to stop program if something wasn't correct

void setup()
{
  pinMode(bottlePump, OUTPUT);
  pinMode(PumpAdeiasma, OUTPUT);
  pinMode(DosingPump, OUTPUT);
  pinMode(vana, OUTPUT);
  pinMode(SmpotLow, INPUT);
  pinMode(SmpotHigh, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(bottlePump, HIGH);
  digitalWrite(PumpAdeiasma, HIGH);
  digitalWrite(DosingPump, HIGH);
  digitalWrite(vana, HIGH);
  Serial.begin(9600);
}

void loop()
{
  //-----------------------------------------  
  Serial.flush();
  Serial.println("Starting programm");
  serial_wait();
  firstMeasurement();
  SL = finalDistance;
  BL = SL + 6; 
  Serial.println("--- NEW START ---");
  Serial.println("Distance is:"+ String(SL));
  Serial.println("Mpotilia up sensor is:"+String(digitalRead(SmpotHigh)));
  Serial.println("Mpotilia low sensor is:"+String(digitalRead(SmpotLow)));
  goto skip;
  serial_wait();
  check_one(); 
  gemismaMpotilias();
  Serial.println("--- END GEMISMA 1 ---");
  serial_wait();
  antixlo(); 
  Serial.println("--- END ANTIXLO 1 ---");
  serial_wait();
  time1 = millis();
  adeiasma();
  Serial.println("--- END ADEISMA ---");
  Serial.flush();
  check_two();
  time2 = millis() - time1;
  Serial.print("Start delay: "+String(((600000 - time2) / 1000) / 60));
  delay(600000 - time2);
  Serial.print("End 10 min delay");
  serial_wait();
  waterToTank(); // first bottle  
  check_three();
  skip:
  serial_wait();
  gemismaMpotilias();
  serial_wait();
  antixlo();
  delay(600000);
  serial_wait();
  waterToTank();
  Serial.flush();
  metrhshUltarsonic();
  Serial.print("Final water level = "+String(finalDistance));
  Serial.flush();
  delay(50);
  exit(0);
}
//---------------------------------------------------------------------------------------------------------------
void serial_wait()
{
  Serial.println("Please enter 1 to continue or 2 to stop the program:");
  while (true)
  {
    if (Serial.available() > 0)
    {
      int input = Serial.parseInt();
      if (input == 1)
      {
        Serial.println("Continuing program...");
        break;
      }
      else if (input == 2)
      {
        Serial.println("Stopping program...");
        delay(50);
        exit(0);
      }
      else
      {
        Serial.println("Invalid input. Please enter 1 to continue or 2 to stop the program:");
      }
    }
  }
}
//---------------------------------------------------------------------------------------------------------------
void check_one()
{
  metrhshUltarsonic();
  if (SL < UL or SL > finalDistance + 2)
  {
    dontstart = true;
    if (SL < UL)
    {
      Serial.println("Water level too high");
    }
    else if (SL > 9.5)
    {
      Serial.println("Water level too low");
    }
  }
}
//---------------------------------------------------------------------------------------------------------------
void check_three()
{
  metrhshUltarsonic();
  if (finalDistance - 3 < UL)
  {
    Serial.println("Too much water would be added");
    dontstart = true;
  }
}
//---------------------------------------------------------------------------------------------------------------
void check_two()
{
  metrhshUltarsonic();
  if (finalDistance < SL + 5.5)
  {
    Serial.println("Not enough water removed");
    dontstart = true;
  }
}
//-----------------------------------------------------------------------------------------------------------------
void firstMeasurement(){
 for (int i = 0; i < numMeasurements; i++)
  {
    delay(500);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    measurements[i] = distance; // Store the measurement in the array
    //Serial.println(String(i) + ": " + String(measurements[i]));
  }
  for (int j = 0; j < numMeasurements; ++j) {
    for (int p = j + 1; p < numMeasurements; ++p) {
      if (measurements[j] > measurements[p]) {
        float temp = measurements[j];
        measurements[j] = measurements[p];
        measurements[p] = temp;
      }
    }
  }
  int remainder = numMeasurements % 2;
  if (remainder != 0) {
    finalDistance = measurements[(numMeasurements / 2) + 1];
  }
  else {
    finalDistance = measurements[numMeasurements / 2];
  }
  Serial.println("First median is:" + String(finalDistance));
}
//-----------------------------------------------------------------------------------------------------------------
void metrhshUltarsonic()
{
  for (int i =0; i< numValidMeasurements;i++){
    validMeasurements[i] = 0;
  }
  numValidMeasurements=0;
  for(int i = 0;i < numMeasurements; i++){
    measurements[i] = 0;
  }
  for (int i = 0; i < numMeasurements; i++)
  {
    delay(500);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    measurements[i] = distance; // Store the measurement in the array
    //Serial.println(String(i) + ": " + String(measurements[i]));
  }
  for (int k = 0; k < numMeasurements;++k) {
    if (measurements[k] > SL - 0.5 and measurements[k] < BL + 0.5) {
      numValidMeasurements++;
      int number = numValidMeasurements;
      validMeasurements[numValidMeasurements] = measurements[k];
    }
  }
  for (int j = 0; j < numValidMeasurements; ++j) {
    for (int p = j + 1; p < numValidMeasurements; ++p) {
      if (validMeasurements[j] > validMeasurements[p]) {
        float temp = validMeasurements[j];
        validMeasurements[j] = validMeasurements[p];
        validMeasurements[p] = temp;
      }
    }
  }
  if (numValidMeasurements != 0) {
    int remainder = numValidMeasurements % 2;
    if (remainder != 0) {
      finalDistance = validMeasurements[int((numValidMeasurements / 2)) + 1];
    }
    else {
      finalDistance = validMeasurements[int(numValidMeasurements / 2)];
    }
    Serial.println("Median is: " + String(finalDistance));
  }
  else{
    Serial.println("No valid measurements");
  }
}
//----------------------------------------------------------------------------------------------------------------
void adeiasma()
{
  if (not dontstart)
  {
    Serial.println("Water starting level is:"+String(SL));
    Serial.println("start adeiasma");
    while (adeiasmaTelos == false)
    {
      metrhshUltarsonic();
      delay(2000);
      digitalWrite(PumpAdeiasma, LOW);
      if (finalDistance >= BL)
      {
        adeiasmaTelos = true;
        Serial.println("Final distance is:"+String(finalDistance));
      }
    }
    digitalWrite(PumpAdeiasma, HIGH);
    Serial.println("End adeiasma");
  }
}
//------------------------------------------------------------------------------------------------------------------
void gemismaMpotilias()
{
    if (not dontstart)
  {
    Serial.println(digitalRead(SmpotHigh));
    gemismaMpotiliasTelos = false;
    Serial.flush();   
    Serial.println("start gemisma mpotilias");
    while (gemismaMpotiliasTelos == false)
    {       
      digitalWrite(vana, LOW);
      int MpotHigh = digitalRead(SmpotHigh);
      if (MpotHigh == HIGH)
      {
        Serial.println("full");
        gemismaMpotiliasTelos = true;
        delay(2000);
      }
    }
    digitalWrite(vana, HIGH);
    Serial.print("end gemisma");
    Serial.flush();
  }
}
//------------------------------------------------------------------------------------------------------------------
void antixlo()
{
  if (not dontstart)
  {
    if (gemismaMpotiliasTelos = true)
    {
      Serial.println("start antixlorio");
      digitalWrite(DosingPump, LOW);
      delay(antixloDelay);
      digitalWrite(DosingPump, HIGH);
      Serial.println("end antixlorio");
    }
  }
}
//------------------------------------------------------------------------------------------------------------------
void waterToTank()
{
  if (not dontstart)
  {
    gemismaTankTelos = false;
    Serial.println("Start water to tank");
    while (gemismaTankTelos == false and dontstart == false)
    {
      mpotiliaLow = digitalRead(SmpotLow);
      digitalWrite(bottlePump, LOW);
      if (digitalRead(SmpotLow) == LOW)
      {
        gemismaTankTelos = true;
      }
    }
    digitalWrite(bottlePump, HIGH);
    Serial.println("End water to tank");
    
  }
}
