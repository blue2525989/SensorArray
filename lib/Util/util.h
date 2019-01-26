#ifndef Util_h
#define Util_h

#include <Arduino.h>
#include <Util.h>
#include <SD.h>

class Util
{
public:
  static void readSensorsAndLogLoop();
  static void setup();
private:
  File _sensorData;
  String _serialInput;
  void readFile(Util* u);
  void closeFile(Util* u);
  void openFileWrite(Util* u);
  void openFileRead(Util* u);
  void clearDataFile();
};

#endif
