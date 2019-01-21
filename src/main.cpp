#include <Arduino.h>
#include <SD.h>
#include <SPI.h>

#define FIRE_MIN 0
#define FIRE_MAX 1024
#define FIRE_PIN A0
#define TILT_PIN A1
#define SD_PIN 7
#define TEMP_PIN A2
// using a delay in main loop of 100ms and wanting to sample temp every 5sec so
// every loop iteration is LOG_TEMP_TIME/counter+1 = (delay+100 * 50) = 5000
#define LOG_TEMP_TIME 50

File sensorData;


void readSensorsAndLogLoop() {
    // for counting when to sample
    uint8_t counter = 0;



    while(true) {
        if (analogRead(TILT_PIN) == 0) {
            sensorData.print("{tilt: TRUE");
            sensorData.println("}");
        }

        float firePinVal = analogRead(FIRE_PIN);
        if (firePinVal < 100) {
            sensorData.print("{fire: {range: ");
            sensorData.print(firePinVal);
            sensorData.println(", active: TRUE} }");
        }

        if (counter == LOG_TEMP_TIME) {
            float val = analogRead(TEMP_PIN);
            float cel = (5.0 * val * 100.0)/1024.0;
            float f = ((cel*9)/5) + 32;

            sensorData.print("{tempF: ");
            sensorData.print(f);
            sensorData.println("}");
        }
        // delay 100ms and increament counter
        delay(100);
        counter++;


        // close the file
        sensorData.close();
    }
}



void readFile() {
    if (sensorData) {
        Serial.println("{data:");
        while (sensorData.available()) {
            Serial.write(sensorData.read());
        }
        Serial.println("\n}");
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening data.txt");
    }
    Serial.flush();
}

void openFile() {
    // open file
    while (!sensorData) {
        sensorData = SD.open("data.txt", FILE_WRITE);
    }
    Serial.println("SD initialized, data.txt file opened for logging.");
}

void closeFile() {
    sensorData.close();
}

int main() {
    init();
    // init serial port
    Serial.begin(9600);
    while (!Serial) {
    }

    // temp analog
    pinMode(TEMP_PIN, INPUT);
    // fire sensor
    pinMode(FIRE_PIN, INPUT);
    // reed switch
    pinMode(TILT_PIN, INPUT);
    // init SD
    while (!SD.begin(SD_PIN)) {
    }

    // open file
    openFile();

    // todo work on this loop more
    while(true) {
        if (Serial.read() == "Stop") {
            Serial.println("Reading file.");
            readFile();
        } else {
            Serial.println("logging data.");
            readSensorsAndLogLoop();
        }
    }
    Serial.flush();
}
