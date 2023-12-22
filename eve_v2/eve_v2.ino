#include <SoftwareSerial.h>

/*
THIS IS CODE FOR THE QUANTUM BB84 PROTOCOL AT STONYBROOK
    UNIVERSITY. THIS CODE IS FOR THE ARDUINO IN THE BOB SECTION.
*/

// USER COMMANDS

#define SET_READ_00 "R000"
#define SET_READ_315 "R315"

#define SET_SEND_00 "S000"
#define SET_SEND_90 "S090"
#define SET_SEND_45 "S045"
#define SET_SEND_315 "S315"

#define CLEAR_MEMORY "CMR"
#define SET_MANUAL_MODE "SMM"
#define PRINT_HIST "PHS"
#define MOVE_REL1 "MR1"
#define MOVE_REL2 "MR2"

#define SEND_REL1 "SR1"
#define SEND_REL2 "SR2"

#define START_ALIGN "STA"
#define END_ALIGN "ENA"

#define CHANGE_ADRESS_TO_1 "CATA"
#define CHANGE_ADRESS_TO_0 "CAT0"

// TO COMMUNICATE WITH ELLB BUS CONTROLLER
// 22.5 deg -> 00002000 in the ELL14's terms

#define DEGREE_0 "0ma00008FF6"   // THIS IS THE OFFSET ANGLE FOUND VIA MR1/2
#define DEGREE_N45 "0ma0000EFF6" // DEGREE_90 + 22.5deg in hex

// TO SENDING WAVEPLATE
#define SEGREE_0 "Ama00000200"   // THIS IS THE OFFSET ANGLE FOUND VIA MR1/2
#define SEGREE_45 "Ama00002200"  // DEGREE_0 + 22.5deg in hex
#define SEGREE_90 "Ama00004200"  // DEGREE_45 + 22.5deg in hex
#define SEGREE_N45 "Ama00003680" // DEGREE_90 + 22.5deg in hex

#define rxPin 5
#define txPin 6
SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);
int busTimeout = 20;
bool waitForResponse = true;

String commandData = "";
String busData = "";

// LASE SETTINGS
const long pulseTime = 5000; // uS
const int lasePin = 8;
const long waitTime = 400; // mS

// FOR READING BITS
int input0 = A0;
int input1 = A2;
unsigned long startTime;
unsigned long holdTime;
float currentMax0 = 0.0;
float currentMax1 = 0.0;
bool readingPulse;
float delV = 0.2;
float triggerLevel = 0.5;
bool manualMode = false; // Controls if new base is auto selected

int currentReadBase; // 0 or -45

void setup()
{
  // Define pin modes for TX and RX
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  randomSeed(analogRead(A5));
  // Set the baud rate for the SoftwareSerial object
  mySerial.begin(9600);
  // Set the baud rate for the Serial object
  Serial.begin(9600);
  currentReadBase = generateRandomReadBase();
  changeReadBase(currentReadBase);
}

void loop()
{
  readPulseAsync();
  checkSerial();
}

void checkSerial()
{
  while (Serial.available() > 0)
  {

    char incomingChar = Serial.read(); // Read the incoming character
    commandData += incomingChar;       // Append the character to the commandData string
    if (incomingChar == '\n')
    {
      Serial.println(commandData);
      // End of message, process the commandData string
      commandData.trim(); // Remove leading/trailing whitespace and newlines
      if (commandData == "HOME")
      {
        mySerial.println("0ho0");
      }
      else if (commandData == SET_MANUAL_MODE)
      {
        manualMode = !manualMode;
      }
      // MOVING RELATIVE
      else if (commandData == MOVE_REL1)
      {
        mySerial.println("0mr00001000");
        while (!busData.endsWith("\n"))
        {
          if (mySerial.available() > 0)
          {
            busData += char(mySerial.read()); // Read incoming characters
          }
        }

        busData.trim();
        Serial.println(busData);
        busData = "";
      }
      else if (commandData == MOVE_REL2)
      {
        mySerial.println("0mr00000800");
        while (!busData.endsWith("\n"))
        {
          if (mySerial.available() > 0)
          {
            busData += char(mySerial.read()); // Read incoming characters
          }
        }

        busData.trim();
        Serial.println(busData);
        busData = "";
      }
      else if (commandData == SEND_REL1)
      {
        mySerial.println("Amr00001000");
        while (!busData.endsWith("\n"))
        {
          if (mySerial.available() > 0)
          {
            busData += char(mySerial.read()); // Read incoming characters
          }
        }

        busData.trim();
        Serial.println(busData);
        busData = "";
      }
      else if (commandData == SEND_REL2)
      {
        mySerial.println("Amr00000800");
        while (!busData.endsWith("\n"))
        {
          if (mySerial.available() > 0)
          {
            busData += char(mySerial.read()); // Read incoming characters
          }
        }

        busData.trim();
        Serial.println(busData);
        busData = "";
      }
      else if (commandData == START_ALIGN)
      {
        digitalWrite(lasePin, HIGH);
      }
      else if (commandData == END_ALIGN)
      {
        digitalWrite(lasePin, LOW);
      }
      else if (commandData == SET_READ_00)
      {
        changeReadBase(0);
      }
      else if (commandData == SET_READ_315)
      {
        changeReadBase(-45);
      }
      else if (commandData == SET_SEND_00)
      {
        changeSendBase(0);
      }
      else if (commandData == SET_SEND_315)
      {
        changeSendBase(-45);
      }
      else if (commandData == SET_SEND_00)
      {
        changeSendBase(0);
      }
      else if (commandData == SET_SEND_315)
      {
        changeSendBase(-45);
      }
      else if (commandData == CHANGE_ADRESS_TO_1)
      {
        mySerial.println("0ca1");
        delay(100);
        mySerial.println("1ca1");
        delay(100);
        mySerial.println("2ca1");
        delay(100);
        mySerial.println("3ca1");
        delay(100);
        mySerial.println("4ca1");
        delay(100);
        mySerial.println("5ca1");
        delay(100);
        mySerial.println("6ca1");
        delay(100);
        mySerial.println("7ca1");
        delay(100);
        mySerial.println("8ca1");
        delay(100);
        mySerial.println("9ca1");
        delay(100);
        mySerial.println("Aca1");
        delay(100);
        mySerial.println("Bca1");
        delay(100);
        mySerial.println("Cca1");
        delay(100);
        mySerial.println("Dca1");
        delay(100);
        mySerial.println("Eca1");
        delay(100);
        mySerial.println("Fca1");
        delay(100);
      }
      else if (commandData == CHANGE_ADRESS_TO_0)
      {
        mySerial.println("0ca0");
        delay(100);
        mySerial.println("1ca0");
        delay(100);
        mySerial.println("2ca0");
        delay(100);
        mySerial.println("3ca0");
        delay(100);
        mySerial.println("4ca0");
        delay(100);
        mySerial.println("5ca0");
        delay(100);
        mySerial.println("6ca0");
        delay(100);
        mySerial.println("7ca0");
        delay(100);
        mySerial.println("8ca0");
        delay(100);
        mySerial.println("9ca0");
        delay(100);
        mySerial.println("Aca0");
        delay(100);
        mySerial.println("Bca0");
        delay(100);
        mySerial.println("Cca0");
        delay(100);
        mySerial.println("Dca0");
        delay(100);
        mySerial.println("Eca0");
        delay(100);
        mySerial.println("Fca0");
        delay(100);
      }
      else
      {
        Serial.println("NOT A VALID COMMAND/MODE");
      }
      Serial.println("READ");

      commandData = ""; // Clear the commandData string for the next message
    }
  }
}

bool changeReadBase(int angle)
{
  switch (angle)
  {
  case 0:
    // 0 => Hex
    mySerial.println(DEGREE_0);
    currentReadBase = 0;
    break;
  case -45:
    mySerial.println(DEGREE_N45);
    currentReadBase = -45;
    break;
  default:
    // Do something when angle doesn't match any case
    break;
  }
  // DELAY FOR A BIT
  if (!waitForResponse)
  {
    return true;
  }
  // kksds
  holdTime = millis();
  while (!busData.endsWith("\n") && (millis() - holdTime < busTimeout))
  {

    while (mySerial.available() > 0)
    {
      busData += char(mySerial.read()); // Read incoming characters
    }
  }

  busData.trim();

  if (busData.charAt(1) == 'P' && busData.charAt(2) == 'O')
  {
    // success
    busData = "";
    return true;
  }
  else if (busData.charAt(1) == 'G' && busData.charAt(2) == 'S')
  {
    busData = "";
    return false;
  }
}

int generateRandomReadBase()
{

  int randomNumber = random(2);

  // Map the random number to your choices
  switch (randomNumber)
  {
  case 0:
    return 0;
    break;
  case 1:
    return -45;
    break;
  }
}
int angleToBase(int angle)
{
  switch (angle)
  {
  case 0:
    return 'R';
    break;
  case 90:
    return 'R';
    break;
  case 45:
    return 'D';
    break;
  case -45:
    return 'D';
    break;
  default:
    // Do something when angle doesn't match any case
    break;
  }
}
void readPulseAsync()
{
  float input1Voltage = 5.0 * (analogRead(A2) / 1023.0);
  float input0Voltage = 5.0 * (analogRead(A0) / 1023.0);

  bool triggered = (input1Voltage >= triggerLevel || input0Voltage >= triggerLevel);

  if (triggered && !readingPulse)
  {
    // Record the start time of the HIGH state
    startTime = micros();
    readingPulse = true;
  }
  if (readingPulse)
  {
    currentMax1 = fmax(currentMax1, input1Voltage);
    currentMax0 = fmax(currentMax0, input0Voltage);

    if (micros() - startTime >= pulseTime * 0.8)
    {
      // WE FOUND A PULSE
      readingPulse = false;
      int readBit;
      if (currentMax1 - currentMax0 > delV)
      {
        // ONE
        Serial.print("1");
        readBit = 1; // 0 or 1
      }
      else if (currentMax0 - currentMax1 > delV)
      {
        // ZERO
        Serial.print("0");
        readBit = 0; // 0 or 1
      }
      else
      {
        // RANDOMLY PICK
        Serial.print("2");
        readBit = random(2); // 0 or 1
      }

      // HERE IS WHERE WE DIFFER FROM BOB WE NOW WANT TO SEND A PULSE
      // KEEPING TRACK OF OUR HISTORY IS NOT NECESSARY

      // * Change send base to currentReadBase
      changeSendBase(currentReadBase);
      // * Send Pulse
      sendPulse();
      // * New read base, and update currentReadBase, and start scanning again
      if (!manualMode)
      {
        changeReadBase(generateRandomReadBase());
      }
    }
  }
}
void sendPulse()
{
  // Set Output Pin to 5V
  digitalWrite(lasePin, HIGH);

  delayMicroseconds(pulseTime);
  // Set Output Pin to 0V
  digitalWrite(lasePin, LOW);
}

bool changeSendBase(int angle)
{
  busData = "";

  switch (angle)
  {
  case 0:

    mySerial.println(SEGREE_0);
    break;
  case 90:
    mySerial.println(SEGREE_90);
    break;
  case 45:
    mySerial.println(SEGREE_45);
    break;
  case -45:
    mySerial.println(SEGREE_N45);
    break;
  default:
    break;
  }
  // DELAY FOR A BIT
  delay(300);
  if (!waitForResponse)
  {

    return true;
  }

  while (!busData.endsWith("\n"))
  {

    while (mySerial.available() > 0)
    {
      busData += char(mySerial.read()); // Read incoming characters
    }
  }

  busData.trim();
  busData = "";
  return true;
  if (busData.charAt(1) == 'P' && busData.charAt(2) == 'O')
  {
    // success

    busData = "";
    return true;
  }
  else if (busData.charAt(1) == 'G' && busData.charAt(2) == 'S')
  {

    busData = "";
    return false;
  }
}