#include <SoftwareSerial.h>

/*
THIS IS CODE FOR THE QUANTUM BB84 PROTOCOL AT STONYBROOK
    UNIVERSITY. THIS CODE IS FOR THE ARDUINO IN THE EVE SECTION.
*/

// USER COMMANDS

#define SET_BASIS_00 "S000"
#define SET_BASIS_90 "S090"
#define SET_BASIS_45 "S045"
#define SET_BASIS_315 "S315"
#define CLEAR_MEMORY "CMR"
#define START_ALIGN "STA"
#define END_ALIGN "ENA"
#define TEST_SEND "TS"
#define CHANGE_ADRESS_TO_1 "CATA"
#define CHANGE_ADRESS_TO_0 "CAT0"

// TO COMMUNICATE WITH ELLB BUS CONTROLLER
// TO RECEIVING WAVEPLATE
// 22.5 deg -> 00002000 in the ELL14's terms
#define DEGREE_0 "0ma00000200"   // THIS IS THE OFFSET ANGLE FOUND VIA MR1/2
#define DEGREE_N45 "0ma00003680" // DEGREE_0 + 22.5deg in hex

// TO SENDING WAVEPLATE
#define SEGREE_0 "Ama00000200"   // THIS IS THE OFFSET ANGLE FOUND VIA MR1/2
#define SEGREE_45 "Ama00002200"  // DEGREE_0 + 22.5deg in hex
#define SEGREE_90 "Ama00004200"  // DEGREE_45 + 22.5deg in hex
#define SEGREE_N45 "Ama00003680" // DEGREE_90 + 22.5deg in hex

#define rxPin 5
#define txPin 6
SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);
bool waitForResponse = false;

String commandData = "";
String busData = "";

// LASE SETTINGS
const long pulseTime = 1000; // uS
const int lasePin = 11;
const long waitTime = 1000; // mS

// SEND HISTORY STORAGE
const int SENT_MEMORY_SIZE = 256;
char readBasis[SENT_MEMORY_SIZE]; // D or R
int readBits[SENT_MEMORY_SIZE];   // 0 or 1
int readIndex = 0;

// FOR READING BITS
int input0 = A0;
int input1 = A2;
unsigned long startTime;
float currentMax0 = 0.0;
float currentMax1 = 0.0;
bool readingPulse;
float delV = 0.5;
float triggerLevel = 0.5;
bool manualMode = false; // WILL NEW BASES BE GENERATED AND SET AFTER EACH READ?

int currentReadBase;

void setup()
{
    // Define pin modes for TX and RX
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    // Set the baud rate for the SoftwareSerial object
    randomSeed(analogRead(A5));
    mySerial.begin(9600);
    // Set the baud rate for the Serial object
    Serial.begin(9600);
    digitalWrite(lasePin, LOW);
    currentReadBase = generateRandomReadBase();
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
        Serial.println(commandData);
        if (incomingChar == '\n')
        {
            // End of message, process the commandData string
            commandData.trim(); // Remove leading/trailing whitespace and newlines

            if (commandData == CLEAR_MEMORY)
            {
                resetBitsAndBaseMemory();
            }
            else if (commandData == SET_BASIS_00)
            {
                changeReadBase(0);
            }
            else if (commandData == TEST_SEND)
            {
                sendPulse();
            }
            else if (commandData == SET_BASIS_90)
            {
                changeReadBase(90);
            }
            else if (commandData == SET_BASIS_45)
            {
                changeReadBase(45);
            }
            else if (commandData == SET_BASIS_315)
            {
                changeReadBase(-45);
            }
            else if (commandData == START_ALIGN)
            {
                digitalWrite(lasePin, HIGH);
            }
            else if (commandData == END_ALIGN)
            {
                digitalWrite(lasePin, LOW);
            }
            else if (commandData == CHANGE_ADRESS_TO_1)
            {
                mySerial.println("0ca1");
                delay(500);
                mySerial.println("1ca1");
                delay(500);
                mySerial.println("2ca1");
                delay(500);
                mySerial.println("3ca1");
                delay(500);
                mySerial.println("4ca1");
                delay(500);
                mySerial.println("5ca1");
                delay(500);
                mySerial.println("6ca1");
                delay(500);
                mySerial.println("7ca1");
                delay(500);
                mySerial.println("8ca1");
                delay(500);
                mySerial.println("9ca1");
                delay(500);
                mySerial.println("Aca1");
                delay(500);
                mySerial.println("Bca1");
                delay(500);
                mySerial.println("Cca1");
                delay(500);
                mySerial.println("Dca1");
                delay(500);
                mySerial.println("Eca1");
                delay(500);
                mySerial.println("Fca1");
                delay(500);
            }
            else if (commandData == CHANGE_ADRESS_TO_0)
            {
                mySerial.println("0ca0");
                delay(500);
                mySerial.println("1ca0");
                delay(500);
                mySerial.println("2ca0");
                delay(500);
                mySerial.println("3ca0");
                delay(500);
                mySerial.println("4ca0");
                delay(500);
                mySerial.println("5ca0");
                delay(500);
                mySerial.println("6ca0");
                delay(500);
                mySerial.println("7ca0");
                delay(500);
                mySerial.println("8ca0");
                delay(500);
                mySerial.println("9ca0");
                delay(500);
                mySerial.println("Aca0");
                delay(500);
                mySerial.println("Bca0");
                delay(500);
                mySerial.println("Cca0");
                delay(500);
                mySerial.println("Dca0");
                delay(500);
                mySerial.println("Eca0");
                delay(500);
                mySerial.println("Fca0");
                delay(500);
            }
            else
            {
                Serial.println("NOT A VALID COMMAND");
            }

            commandData = ""; // Clear the commandData string for the next message
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

// return uniform random choice of angle and corresponding base and bit
// THIS IS THE BASE FOR READING
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

void printHistoryToSerial()
{
    Serial.println("");
    Serial.println("STARTING BIT DUMP");
    for (int i = 0; i < SENT_MEMORY_SIZE; i++)
    {
        Serial.print(readBasis[i]);
        Serial.print(",");
        Serial.print(readBits[i]);
        Serial.println("");
    }
    Serial.println("END BIT DUMP");
}

bool changeReadBase(int angle)
{

    switch (angle)
    {
    case 0:
        // 0 => Hex => 00000000
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

bool changeSendBase(int angle)
{

    switch (angle)
    {
    case 0:
        // 0 => Hex => 00000000
        mySerial.println(SEGREE_0);
        break;
    case 90:
        mySerial.println(SEGREE_90);
        break;
    case 45:
        // PPR/8 = 32768 => Hex => 00008000
        mySerial.println(SEGREE_45);
        break;
    case -45:
        mySerial.println(SEGREE_N45);
        break;
    default:
        // Do something when angle doesn't match any case
        break;
    }
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

void resetBitsAndBaseMemory()
{

    for (int i = 0; i < SENT_MEMORY_SIZE; i++)
    {
        readBasis[i] = 'N';
        readBits[i] = 8;
    }
    readIndex = 0;
}

void mirrorPulse(int readBit)
{

    if (readBit == 0)
    {
        if (currentReadBase == 0)
        {
            changeSendBase(0);
        }
        else if (currentReadBase == -45)
        {
            changeSendBase(-45);
        }
    }
    else if (readBit == 1)
    {
        if (currentReadBase == 0)
        {
            changeSendBase(90);
        }
        else if (currentReadBase == -45)
        {
            changeSendBase(45);
        }
    }
    // also check if change was success
    Serial.println("MIRRORING");
    sendPulse();
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
            Serial.println("FOUND");
            // WE FOUND A PULSE
            readingPulse = false;

            int readBit;
            if (currentMax1 - currentMax0 > delV)
            {
                // ONE

                readBit = 1;
            }
            else if (currentMax0 - currentMax1 > delV)
            {
                // ZERO

                readBit = 0;
            }
            else
            {
                // RANDOMLY PICK
                readBit = random(2);
            }
            readBasis[readIndex] = angleToBase(currentReadBase);
            readBits[readIndex] = readBit;

            mirrorPulse(readBit);

            currentMax1 = 0;
            currentMax0 = 0;
            readIndex++;

            if (readIndex >= SENT_MEMORY_SIZE)
            {
                printHistoryToSerial();
                resetBitsAndBaseMemory();
            }
            if (manualMode)
            {
                changeReadBase(generateRandomReadBase());
            }
        }
    }
}
int angleToBase(int angle)
{
    switch (angle)
    {
    case 0:
        return "R";
        break;
    case 90:
        return "R";
        break;
    case 45:
        return "D";
        break;
    case -45:
        return "D";
        break;
    default:
        // Do something when angle doesn't match any case
        break;
    }
}