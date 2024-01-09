#ifndef ELL14_h
#define ELL14_h

#include "Arduino.h"
#include <SoftwareSerial.h>

class ELL14
{
public:
  ELL14(String address, SoftwareSerial mySerial);
  bool changeBase(int angle);
  void move_rel(String amount);
  String angle_to_hex(float angle);

private:
  String _address;
  SoftwareSerial _mySerial;
};
#endif
