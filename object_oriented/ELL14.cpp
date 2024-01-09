#include "Arduino.h"
#include "ELL14.h"
#include <SoftwareSerial.h>

ELL14::ELL14(String address, SoftwareSerial mySerial){
  _address = address;
  _mySerial = mySerial;
}

void ELL14