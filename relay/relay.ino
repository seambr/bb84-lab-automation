#include <SoftwareSerial.h>
// 0ma00010000
/*
THIS IS CODE FOR THE QUANTUM BB84 PROTOCOL AT STONYBROOK
    UNIVERSITY. THIS CODE IS FOR THE ARDUINO IN THE ALICE SECTION.
*/

// TO COMMUNICATE WITH ELLB BUS CONTROLLER
#define rxPin 3
#define txPin 5
SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);

String commandData = "";

String busData = "";


void setup() {
    // Define pin modes for TX and RX
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    // Set the baud rate for the SoftwareSerial object
    mySerial.begin(9600);
    // Set the baud rate for the Serial object
    Serial.begin(9600);
}

void loop() {
    checkSerial();


    if (mySerial.available() > 0) {
        
        char incomingChar = mySerial.read(); // Read the incoming character
        busData += incomingChar;     // Append the character to the busData string
        if (incomingChar == '\n') {
            // End of message, process the busData string
            busData.trim(); // Remove leading/trailing whitespace and newlines
            Serial.println(busData);
            busData = ""; // Clear the commandData string for the next message
        }
    }
}

void checkSerial(){
    if (Serial.available() > 0) {
        
        char incomingChar = Serial.read(); // Read the incoming character
        commandData += incomingChar;     // Append the character to the commandData string
        if (incomingChar == '\n') {
            // End of message, process the commandData string
            commandData.trim(); // Remove leading/trailing whitespace and newlines
            mySerial.println(commandData);
            commandData = ""; // Clear the commandData string for the next message
        }
    }

    
    
      
}