#include <SoftwareSerial.h>

/*
THIS IS CODE FOR THE QUANTUM BB84 PROTOCOL AT STONYBROOK
    UNIVERSITY. THIS CODE IS FOR THE ARDUINO IN THE BOB SECTION.
*/

// USER COMMANDS

#define SET_BASIS_00 "S000"
#define SET_BASIS_90 "S090"
#define SET_BASIS_45 "S045"
#define SET_BASIS_315 "S315"
#define CLEAR_MEMORY "CMR"
#define SET_MANUAL_MODE "SMM"
#define PRINT_HIST "PHS"
#define MOVE_REL1 "MR1"
#define MOVE_REL2 "MR2"

// TO COMMUNICATE WITH ELLB BUS CONTROLLER
// 22.5 deg -> 00002000 in the ELL14's terms
// 0PO0000B9FF
#define DEGREE_0  "0ma00008FF6"   // THIS IS THE OFFSET ANGLE FOUND VIA MR1/2
// #define DEGREE_45 "0ma00009FF1"  // DEGREE_0 + 22.5deg in hex
// #define DEGREE_90 "0ma0000BFF1"  // DEGREE_45 + 22.5deg in hex
#define DEGREE_N45 "0ma0000EFF6" // DEGREE_90 + 22.5deg in hex

#define rxPin 5
#define txPin 6
SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);

bool waitForResponse = true;

String commandData = "";
String busData = "";

// LASE SETTINGS
const long pulseTime = 5000; // uS
const int lasePin = 13;
const long waitTime = 200; // mS

// SEND HISTORY STORAGE
const int READ_MEMORY_SIZE = 256;
char readBasis[READ_MEMORY_SIZE]; // D or R
int readBits[READ_MEMORY_SIZE];   // 0 or 1
int readIndex = 0;

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
    resetBitsAndBaseMemory();
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

            if (commandData == CLEAR_MEMORY)
            {
                resetBitsAndBaseMemory();
            }
            if (commandData == "HOME")
            {
                mySerial.println("0ho0");
            }
            else if (commandData == PRINT_HIST)
            {
                printHistoryToSerial();
            }
            else if (commandData == SET_MANUAL_MODE)
            {
                manualMode = true;
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
            else if (commandData == SET_BASIS_00)
            {
                changeReadBase(0);
            }
            else if (commandData == SET_BASIS_315)
            {
                changeReadBase(-45);
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

void printHistoryToSerial()
{
    Serial.println("BASE");
    Serial.println("STARTING BIT DUMP");
    for (int i = 0; i < READ_MEMORY_SIZE; i++)
    {

        char basei = readBasis[i];
        int biti = readBits[i];
        if (basei != 'N' && biti != 8)
        {
            Serial.print(basei);
            Serial.print(",");
        }
    }
    Serial.println("BIT");
    for (int i = 0; i < READ_MEMORY_SIZE; i++)
    {

        char basei = readBasis[i];
        int biti = readBits[i];
        if (basei != 'N' && biti != 8)
        {
            Serial.print(biti);
            Serial.print(",");
        }
    }
    Serial.println("");
    Serial.println("END BIT DUMP");
}

void resetBitsAndBaseMemory()
{

    for (int i = 0; i < READ_MEMORY_SIZE; i++)
    {
        readBasis[i] = 'N';
        readBits[i] = 8;
    }
    readIndex = 0;
}
// HEHREHRHER
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
    while (!busData.endsWith("\n") && (millis()-holdTime < 20))
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
            // // Serial.println("delV");
            // Serial.println(currentMax0);
            // Serial.println(currentMax1);
            readBasis[readIndex] = angleToBase(currentReadBase);
            readBits[readIndex] = readBit;
            // Serial.println(readBit);
            currentMax1 = 0;
            currentMax0 = 0;
            readIndex++;
            if (readIndex >= READ_MEMORY_SIZE)
            {
                printHistoryToSerial();
                resetBitsAndBaseMemory();
            }

            if (!manualMode)
            {
                changeReadBase(generateRandomReadBase());
            }
        }
    }
}