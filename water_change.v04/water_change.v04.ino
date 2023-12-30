#define bottlePump 13
#define PumpAdeiasma 11
#define DosingPump 10
#define vana 12
#define SmpotLow 2
#define SmpotHigh 3
#define trigPin 9
#define echoPin 8

const int numMeasurements = 5;       // Number of measurements for the median filter
int delayTime = 0;                   // variable to remove any time passed from the 10 min delay
int validMeasurements = 0;           // Number of valid measurements
int waterMpotHigh = 0;               // value of mpotilia high water sensor
int antixloDelay = 2500;             // delay for antixlorio drop
int mpotiliaLow = 0;                 // value of mpotilia low water sensor
int time1 = 0;                       // variable to calculate 10 min delay
int time2 = 0;                       // variable to calculate 10 min delay
float measurements[numMeasurements]; // Array to store measurements
float result = 0;                    // temp variable for average distance
float averageDistance = 0;           // variable to store the average distance
float finalDistance = 0;             // variable to store after all ultrasonic measurements processing
float distance;                      // variable where the distance of the measured object is stored
float UL = 7;                        // upper limit for water of aquarium
float SL;                            // level of water at start
float BL;                            // level of water that will be removed
long duration;                       // variable where the the reflection time of the ultrasound is stored
bool firstGemismaMesurment = false;  // temp variable for first mpotilia measurement
bool adeiasmaTelos = false;          // variable to control when mpotilia emptying is finished
bool gemismaMpotiliasTelos = false;  // variable to control if mpotilia is full
bool gemismaTankTelos = false;       // variable to control if tank is full
bool dontstart = false;              // variable to stop program if something wasn't correct

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
  // BASIC MEASURMENTS
  metrhshUltarsonic();
  SL = finalDistance;
  BL = SL + 6;
  Serial.println("--- NEW START ---");
  Serial.println("Starting water level is:" + String(SL));
  Serial.println("Mpotilia up sensor is:");
  Serial.print(digitalRead(SmpotHigh));
  Serial.println("Mpotilia low sensor is:");
  Serial.print(digitalRead(SmpotLow));

  skip(); // ASKS TO SKIP RPOGRAM
}
//---------------------------------------------------------------------------------------------------------------
void serial_wait() // DONE(needs testing)
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
  if (SL < UL or SL > 9.5)
  {
    dontstart = true;
    if (SL < 7)
    {
      Serial.print("Water level too high");
    }
    else if (SL > 9.5)
    {
      Serial.print("Water level too low");
    }
    Serial.println("Stopping the program");
    delay(100);
    exit(0);
  }
}
//---------------------------------------------------------------------------------------------------------------
void check_three()
{
  metrhshUltarsonic();
  Serial.println("Water bottom level distance is:" + String(finalDistance));
  if (finalDistance - 3 < UL)
  {
    Serial.println("Too much water would be added");
    Serial.println("Stopping the program");
    delay(100);
    exit(0);
  }
}
//---------------------------------------------------------------------------------------------------------------
void check_two()
{
  metrhshUltarsonic();
  Serial.print("Water bottom level distance is:");
  Serial.println(finalDistance);
  if (finalDistance < SL + 5.5)
  {
    Serial.println("Not enough water removed");
    Serial.println("Stopping the program");
    delay(100);
    exit(0);
  }
}
//-----------------------------------------------------------------------------------------------------------------
void metrhshUltarsonic() // DONE NEEDS TESTING
{
  for (int i = 0; i < numMeasurements; i++)
  {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    measurements[i] = distance; // Store the measurement in the array

    if (i > 0)
    {
      // Sort the measurements in ascending order using insertion sort
      for (int j = i; j > 0 && measurements[j] < measurements[j - 1]; j--)
      {
        // Swap measurements[j] and measurements[j-1]
        float temp = measurements[j];
        measurements[j] = measurements[j - 1];
        measurements[j - 1] = temp;
      }
    }

    // Increment the count of valid measurements
    validMeasurements++;
    finalDistance = measurements[validMeasurements / 2]; // Median value
  }
}
//----------------------------------------------------------------------------------------------------------------
void adeiasma()
{
  Serial.print("Water starting level is:");
  Serial.println(SL);
  Serial.println("start empty aquarium");
  while (adeiasmaTelos == false)
  {
    metrhshUltarsonic();
    digitalWrite(PumpAdeiasma, LOW);
    if (finalDistance >= BL)
    {
      adeiasmaTelos = true;
      Serial.print("Final distance is:");
      Serial.println(finalDistance);
    }
  }
  digitalWrite(PumpAdeiasma, HIGH);
  Serial.print("End empty aquarium");
}
//------------------------------------------------------------------------------------------------------------------
void gemismaMpotilias()
{

  gemismaMpotiliasTelos = false;
  firstGemismaMesurment = false;
  Serial.println("start gemisma mpotilias");
  Serial.println("VANA ON");
  digitalWrite(vana, LOW);
  while (gemismaMpotiliasTelos == false)
  {
    waterMpotHigh = digitalRead(SmpotHigh);
    if (waterMpotHigh == HIGH)
    {
      delay(1000);
      waterMpotHigh = digitalRead(SmpotHigh);
      if (waterMpotHigh == HIGH)
      {
        gemismaMpotiliasTelos = true;
      }
    }
  }
  digitalWrite(vana, HIGH);
  Serial.println("gemisma end mpotilias");
}
//------------------------------------------------------------------------------------------------------------------
void antixlo()
{
  if (gemismaMpotiliasTelos == true)
  {
    Serial.println("start antixlorio");
    digitalWrite(DosingPump, LOW);
    delay(antixloDelay);
    digitalWrite(DosingPump, HIGH);
    Serial.println("end antixlorio");
  }
}
//------------------------------------------------------------------------------------------------------------------
void waterToTank()
{
  Serial.print("Start water to tank");
  while (gemismaTankTelos == false)
  {
    mpotiliaLow = digitalRead(SmpotLow);
    digitalWrite(bottlePump, LOW);
    if (mpotiliaLow == LOW)
    {
      delay(500);
      mpotiliaLow = digitalRead(SmpotLow);
      if (mpotiliaLow == LOW)
      {
        gemismaTankTelos = true;
      }
    }
  }
  digitalWrite(bottlePump, HIGH);
  Serial.println("End water to tank");
  gemismaTankTelos = false;
}
//------------------------------------------------------------------------------------------------------------------
void skip()
{ // NOT DONE

  bool validInput = false;

  while (!validInput)
  {
    Serial.println("Menu:");
    Serial.println("1. Run program from the beginning");
    Serial.println("2. Skip to Part first adeiasma");
    Serial.println("3. Skip to Part first water to tank");
    Serial.println("4. Skip to Part second gemisma mpotilias");
    Serial.println("5. Skip to Part second water to tank");
    Serial.println("Enter your choice: ");

    while (!Serial.available())
    {
      // Wait for user input
    }

    int choice = Serial.parseInt();

    while (Serial.available())
    {
      Serial.read();
    }

    switch (choice)
    {
    case 1:
      // Run the program from the beginning
      validInput = true;
      runProgram();
      break;
    case 2:
      // Skip to first adeiasma
      validInput = true;
      skipToPartA();
      break;
    case 3:
      // Skip to Part first water to tank
      validInput = true;
      skipToPartB();
      break;
    case 4:
      // Skip to Part second gemisma mpotilias
      validInput = true;
      skipToPartC();
      break;
    case 5:
      // Skip to Part second water to tank
      validInput = true;
      skipToPartD();
      break;
    default:
      Serial.println("Invalid choice. Please enter a valid option.");
      break;
    }
  }
}

void runProgram()
{ // runs the program from the start
  check_one();
  Serial.println("--- END CHECK ONE ---");
  gemismaMpotilias();
  Serial.println("--- END GEMISMA 1 ---");
  serial_wait();
  antixlo();
  Serial.println("--- END ANTIXLO 1 ---");
  serial_wait();
  time1 = millis();
  adeiasma();
  Serial.println("--- END ADEISMA ---");
  serial_wait();
  time2 = millis() - time1;
  Serial.print("Start delay: ");
  Serial.println(((600000 - time2) / 1000) / 60);
  delay(600000 - time2);
  Serial.print("End 10 min delay");
  check_two();
  serial_wait();
  waterToTank(); // first bottle
  check_three();
  Serial.println("FIRST BOTTLE COMPLETE. CLEARING THE SERIAL PORT IN 10 SECONDS");
  delay(10000);
  Serial.flush();
  serial_wait();
  gemismaMpotilias();
  serial_wait();
  antixlo();
  delay(600000);
  serial_wait();
  waterToTank();
  metrhshUltarsonic();
  Serial.println("Final water level is:" + String(finalDistance));
  exit(0);
}

void skipToPartA()
{ // starts the program from first adeiasma
  Serial.println("How many minutes has the new water been in the mpotilia with the antixlorio?");
  while (true)
  {
    if (Serial.available() >= 0 && Serial.available() <= 10)
    {
      int input = Serial.parseInt();
      delayTime = 600000 - (input * 600000);
      break;
    }
    else
    {
      Serial.println("Invalid input");
    }
  }

  time2 = millis() - time1;
  Serial.print("Start delay: ");
  Serial.println(((600000 - time2) / 1000) / 60);
  if (time2 + delayTime > 600000)
  {
    time2 = 0;
    delayTime = 600000;
  }
  delay(600000 - time2 - delayTime);
  Serial.print("End 10 min delay");
  check_two();
  serial_wait();
  waterToTank(); // first bottle
  check_three();
  Serial.println("FIRST BOTTLE COMPLETE. CLEARING THE SERIAL PORT IN 10 SECONDS");
  delay(10000);
  Serial.flush();
  serial_wait();
  gemismaMpotilias();
  serial_wait();
  antixlo();
  delay(600000);
  serial_wait();
  waterToTank();
  metrhshUltarsonic();
  Serial.println("Final water level is:" + String(finalDistance));
  exit(0);
}

void skipToPartB()
{ // starts the program from first water to tank

  check_two();
  serial_wait();
  waterToTank(); // first bottle
  check_three();
  Serial.println("FIRST BOTTLE COMPLETE. CLEARING THE SERIAL PORT IN 10 SECONDS");
  delay(10000);
  Serial.flush();
  serial_wait();
  gemismaMpotilias();
  serial_wait();
  antixlo();
  delay(600000);
  serial_wait();
  waterToTank();
  metrhshUltarsonic();
  Serial.println("Final water level is:" + String(finalDistance));
  exit(0);
}

void skipToPartC()
{ //  starts the program from second gemisma mpotilias
  gemismaMpotilias();
  serial_wait();
  antixlo();
  delay(600000);
  serial_wait();
  waterToTank();
  metrhshUltarsonic();
  Serial.println("Final water level is:" + String(finalDistance));
  exit(0);
}

void skipToPartD()
{ // starts the program from second water to tank
  waterToTank();
  metrhshUltarsonic();
  Serial.println("Final water level is:" + String(finalDistance));
  exit(0);
}