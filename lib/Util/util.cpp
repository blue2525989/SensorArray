#include <Arduino.h>
#include <SD.h>
#include <Util.h>

#define STOP "stop"
#define START "start"
#define OPEN "open"
#define CLOSE "close"
#define EXIT "exit"
#define CLEAR "clear"

#define TILT_PIN A1
#define SD_PIN 7
#define TEMP_PIN A2
#define LOG_TEMP_TIME 20

File _sensorData;
String _serialInput;

void Util::readFile(Util* u) {
    if (u->_sensorData) {
        Serial.println("{data:");
        while (u->_sensorData.available()) {
            Serial.write(u->_sensorData.read());
        }
        Serial.println("}");
        while(true) {
            u->_serialInput = Serial.readString();
            if (u->_serialInput == CLOSE) {
                break;
            }
        }
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening data.txt");
    }
    Serial.flush();
}

void Util::closeFile(Util* u) {
    while(u->_sensorData) {
        u->_sensorData.close();
    }
    Serial.println("data.txt file closed.");
}

void Util::openFileWrite(Util* u) {
    // open file
    while (!u->_sensorData) {
        u->_sensorData = SD.open("data.txt", FILE_WRITE);
    }
    Serial.println("data.txt file opened for logging.");
}

void Util::openFileRead(Util* u) {
    // open file
    while (!u->_sensorData) {
        u->_sensorData = SD.open("data.txt", FILE_READ);
        // this is back up incase the file was deleted
        if (!u->_sensorData) {
            u->openFileWrite(u);
            u->closeFile(u);
        }
    }
    Serial.println("data.txt file opened for reading.");
}

void Util::clearDataFile() {
    while (SD.exists("data.txt")) {
        SD.remove("data.txt");
    }
    Serial.println("data.txt has been removed");
}

void Util::readSensorsAndLogLoop() {
    Util* u = new Util();

    // for counting when to sample
    int counter = 0;

    while(true) {
        u->_serialInput = Serial.readString();

        // all this logging can be removed at some point
        Serial.println();
        Serial.println("command entered STOP " + u->_serialInput == STOP);
        Serial.println("command entered START " + u->_serialInput == START);
        Serial.println("command entered OPEN " + u->_serialInput == OPEN);
        Serial.println("command entered CLOSE " + u->_serialInput == CLOSE);
        Serial.println("command entered CLEAR " + u->_serialInput == CLEAR);
        Serial.println();

        // convert to switch statement at some point
        if (u->_serialInput == START) {
            u->openFileWrite(u);
            u->_sensorData.println("{open: true},");
            if (u->_sensorData) {
                while(true) {
                    // get some input
                    u->_serialInput = Serial.readString();
                    // check if we need to stop
                    if (u->_serialInput == STOP) {
                        break;
                    }

                    // figure out best voltage to check
                    if (analogRead(TILT_PIN) > 25) {
                        u->_sensorData.println("\t{tilt: true},");

                        // remove this log at some point
                        Serial.println("tilt true");
                    }
                    delay(50);
                    if (counter == LOG_TEMP_TIME) {
                        /*float dummyRead = */analogRead(TEMP_PIN);
                        delay(50);
                        float val = analogRead(TEMP_PIN);
                        float cel = (5.0 * val * 100.0)/1024.0;
                        float f = ((cel*9)/5) + 32;

                        u->_sensorData.println("\t{tempF: " + (String)f + "},");

                        counter = 0;
                        // remove this log at some point
                        Serial.println("farh: " + (String)f);
                    }
                    // delay 50ms and increament counter
                    delay(50);
                    counter++;
                }
            }

            u->_sensorData.println("{close: true}");
            u->closeFile(u);

        } else if (u->_serialInput == OPEN) {
            u->openFileRead(u);
            u->readFile(u);
            u->closeFile(u);

        } else if (u->_serialInput == CLEAR) {
            u->clearDataFile();
        }
    }
}

void Util::setup() {
    init();
    // init serial port
    Serial.begin(9600);
    while (!Serial) {
    }

    // temp analog
    pinMode(TEMP_PIN, INPUT);
    // tilt switch
    pinMode(TILT_PIN, INPUT);
    // init SD
    while (!SD.begin(SD_PIN)) {
    }
}
