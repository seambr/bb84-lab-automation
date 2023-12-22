#include <SoftwareSerial.h>

/*
THIS IS CODE FOR THE QUANTUM BB84 PROTOCOL AT STONYBROOK
    UNIVERSITY. THIS CODE IS FOR THE ARDUINO IN THE ALICE SECTION.
*/

// USER COMMANDS
#define SEND_1_PULSE "F1"
#define SEND_8_PULSE "F8"
#define SEND_32_PULSE "F32"
#define SEND_64_PULSE "F64"
#define SEND_256_PULSE "F256"
#define SEND_8_RAND "R8"
#define SEND_32_RAND "R32"
#define SEND_64_RAND "R64"
#define SEND_128_RAND "R128"
#define CLEAR_MEMORY "CMR"
#define SET_BASIS_00 "S000"
#define SET_BASIS_90 "S090"
#define SET_BASIS_45 "S045"
#define SET_BASIS_315 "S315"
#define PRINT_HIST "PHS"
#define START_ALIGN "STA"
#define END_ALIGN "ENA"
#define GET_OFFSET "GO"

#define MOVE_REL1 "MR1"
#define MOVE_REL2 "MR2"

// 22.5 deg -> 00002000 in the ELL14's terms
// 0PO0000C9F8
#define DEGREE_0 "0ma0000830E"   // THIS IS THE OFFSET ANGLE FOUND VIA MR1/2
#define DEGREE_45 "0ma0000A30E"  // DEGREE_0 + 22.5deg in hex
#define DEGREE_90 "0ma0000C30E"  // DEGREE_45 + 22.5deg in hex
#define DEGREE_N45 "0ma0000E30E" // DEGREE_90 + 22.5deg in hex
// #define DEGREE_0 "0ma00000000"
// #define DEGREE_90 "0ma00010000"
// #define DEGREE_45 "0ma00008000"
// #define DEGREE_N45 "0ma0000D200"

String commandData = "";
String busData = "";

// LASE SETTINGS
const long pulseTime = 5000; // uS
const int lasePin = 8;
const long waitTime = 400; // mS

// SEND HISTORY STORAGE
const int SENT_MEMORY_SIZE = 256;
char sentBasis[SENT_MEMORY_SIZE]; // D or R
int sentBits[SENT_MEMORY_SIZE];   // 0 or 1
int sentIndex = 0;

unsigned long startTime;

struct baseAndBitStruct
{
    int ang;
    char base;
    int bit;
};

// TO COMMUNICATE WITH ELLB BUS CONTROLLER
#define rxPin 5
#define txPin 6
SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);
bool waitForResponse = true;

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
    digitalWrite(lasePin, LOW);
    resetBitsAndBaseMemory();
}

void loop()
{

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
            // End of message, process the commandData string
            commandData.trim(); // Remove leading/trailing whitespace and newlines
            if (commandData == SEND_1_PULSE)
            {
                sendPulse();
            }
            if (commandData == "HOME")
            {
                mySerial.println("0ho0");
            }
            else if (commandData == GET_OFFSET)
            {
                mySerial.println("0go");
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
            else if (commandData == SEND_32_PULSE)
            {
                sendNPulses(32);
            }

            else if (commandData == SEND_64_PULSE)
            {
                sendNPulses(64);
            }
            else if (commandData == SEND_256_PULSE)
            {
                sendNPulses(256);
            }
            else if (commandData == SEND_8_RAND)
            {
                sendNRandomBitBase(8);
            }
            else if (commandData == SEND_32_RAND)
            {
                sendNRandomBitBase(32);
            }
            else if (commandData == SEND_64_RAND)
            {
                sendNRandomBitBase(64);
            }
            else if (commandData == START_ALIGN)
            {
                digitalWrite(lasePin, HIGH);
            }
            else if (commandData == END_ALIGN)
            {
                digitalWrite(lasePin, LOW);
            }
            else if (commandData == SEND_128_RAND)
            {
                sendNRandomBitBase(128);
            }
            else if (commandData == CLEAR_MEMORY)
            {
                resetBitsAndBaseMemory();
            }
            else if (commandData == PRINT_HIST)
            {
                printHistoryToSerial();
            }
            else if (commandData == SET_BASIS_00)
            {
                changeBase(0);
            }
            else if (commandData == SET_BASIS_90)
            {
                changeBase(90);
            }
            else if (commandData == SET_BASIS_45)
            {
                changeBase(45);
            }
            else if (commandData == SET_BASIS_315)
            {
                changeBase(-45);
            }
            else
            {
                Serial.println("NOT A VALID COMMAND");
            }
            Serial.println("READ");

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

void sendNPulses(int N)
{
    startTime = millis();
    for (int i = 0; i < N; i++)
    {
        sendPulse();
        delay(waitTime);
        if (millis() - startTime > 600000)
        {
            break;
        }
    }
}

// return uniform random choice of angle and corresponding base and bit
int generateRandomBitBase()
{
    // random number between 0 and 3 inclusive
    int randomNumber = random(4);
    // Map the random number to your choices
    switch (randomNumber)
    {
    case 0:
        return 0;
        break;
    case 1:

        return 90;
        break;
    case 2:

        return 45;
        break;
    case 3:
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
        char basei = sentBasis[i];
        int biti = sentBits[i];
        if (basei != 'N' || biti != 8)
        {
            Serial.print(basei);
            Serial.print(",");
        }
    }
    Serial.println("BIT");
    for (int i = 0; i < SENT_MEMORY_SIZE; i++)
    {
        char basei = sentBasis[i];
        int biti = sentBits[i];
        if (basei != 'N' || biti != 8)
        {
            Serial.print(biti);
            Serial.print(",");
        }
    }
    Serial.println("END BIT DUMP");
}
// Select Random Basis, Rotate WavePlate, SendPulse, Record
void sendPulseRandomBitRandomBase()
{
    int angle = generateRandomBitBase();
    bool changeSuccess = changeBase(angle);
    delay(5);
    sendPulse();
    addBaseBitToHistory(angle);
}

void addBaseBitToHistory(int angle)
{
    char sentBase = angleToBase(angle);
    int sentBit = angleToBit(angle);
    sentBasis[sentIndex] = sentBase;
    sentBits[sentIndex] = sentBit;
    sentIndex++;

    if (sentIndex >= SENT_MEMORY_SIZE)
    {
        printHistoryToSerial();
        resetBitsAndBaseMemory();
    }
}

void sendNRandomBitBase(int N)
{
    for (int i = 0; i < N; i++)
    {
        sendPulseRandomBitRandomBase();
        delay(waitTime);
    }
}

bool changeBase(int angle)
{
    busData = "";
    // Serial.println(angle);
    switch (angle)
    {
    case 0:
        // 0 => Hex => 00000000
        mySerial.println(DEGREE_0);
        break;
    case 90:
        mySerial.println(DEGREE_90);
        break;
    case 45:
        // PPR/8 = 32768 => Hex => 00008000
        mySerial.println(DEGREE_45);
        break;
    case -45:
        mySerial.println(DEGREE_N45);
        break;
    default:
        // Do something when angle doesn't match any case
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

void resetBitsAndBaseMemory()
{

    for (int i = 0; i < SENT_MEMORY_SIZE; i++)
    {
        sentBasis[i] = 'N';
        sentBits[i] = 8;
    }
    sentIndex = 0;
}

char angleToBase(int angle)
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
int angleToBit(int angle)
{
    switch (angle)
    {
    case 0:
        return 0;
        break;
    case 90:
        return 1;
        break;
    case 45:
        return 1;
        break;
    case -45:
        return 0;
        break;
    default:
        // Do something when angle doesn't match any case
        break;
    }
}
