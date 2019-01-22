#include <Arduino.h>
#include <SD.h>
// #include <SPI.h>

// #define FIRE_PIN A0
#define TILT_PIN A1
#define SD_PIN 7
#define TEMP_PIN A2
// using a delay in main loop of 100ms and wanting to sample temp every 5sec so
// every loop iteration is LOG_TEMP_TIME/counter+1 = (delay+100 * 50) = 5000
#define LOG_TEMP_TIME 0 || 50

#define STOP "stop"
#define START "start"
#define OPEN "open"
#define CLOSE "close"
#define EXIT "exit"
#define CLEAR "clear"

const char TERMINATOR = ';';
File sensorData;

void readFile(String serialInput) {
    if (sensorData) {
        Serial.println("{data:");
        while (sensorData.available()) {
            Serial.write(sensorData.read());
        }
        Serial.println("\n}");
        while(true) {
            serialInput = Serial.readString();
            if (serialInput == CLOSE) {
                break;
            }
        }
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening data.txt");
    }
    Serial.flush();
}

void closeFile() {
    while(sensorData) {
        sensorData.close();
    }
    Serial.println("data.txt file closed.");
}

void openFileWrite() {
    // open file
    while (!sensorData) {
        sensorData = SD.open("data.txt", FILE_WRITE);
    }
    Serial.println("data.txt file opened for logging.");
}

void openFileRead() {
    // open file
    while (!sensorData) {
        sensorData = SD.open("data.txt", FILE_READ);
        // this is back up incase the file was deleted
        if (!sensorData) {
            openFileWrite();
            closeFile();
        }
    }
    Serial.println("data.txt file opened for reading.");
}

void clearDataFile() {
    while (SD.exists("data.txt")) {
        SD.remove("data.txt");
    }
    Serial.println("data.txt has been removed");
}

void readSensorsAndLogLoop() {
    // for counting when to sample
    uint8_t counter = 0;

    while(true) {
        String serialInput(Serial.readString());

        // all this logging can be removed at some point
        Serial.println();
        Serial.println(serialInput == STOP);
        Serial.println(serialInput == START);
        Serial.println(serialInput == OPEN);
        Serial.println(serialInput == CLOSE);
        Serial.println(serialInput == CLEAR);
        Serial.println();

        // convert to switch statement at some point
        if (serialInput == START) {
            openFileWrite();
            if (sensorData) {
                while(true) {
                    // get some input
                    serialInput = Serial.readString();
                    // check if we need to stop
                    if (serialInput == STOP) {
                        break;
                    }

                    // figure out best voltage to check
                    if (analogRead(TILT_PIN) > 25) {
                        sensorData.println("\t{tilt: true}");

                        // remove this log at some point
                        Serial.println("tilt true");
                    }

                    if (LOG_TEMP_TIME % counter == 0) {
                        float val = analogRead(TEMP_PIN);
                        float cel = (5.0 * val * 100.0)/1024.0;
                        float f = ((cel*9)/5) + 32;

                        sensorData.println("\t{tempF: " + (String)f + "}");

                        counter = (counter == 50) ? 0 : counter;
                        // remove this log at some point
                        Serial.println("farh: " + (String)f);
                    }
                    // delay 100ms and increament counter
                    delay(100);
                    counter++;
                }
            }
            closeFile();

        } else if (serialInput == OPEN) {
            openFileRead();
            readFile(serialInput);
            closeFile();

        } else if (serialInput == CLEAR) {
            SD.remove("data.txt");
        }
    }
}

int main() {
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

    // main loop
    readSensorsAndLogLoop();
}
