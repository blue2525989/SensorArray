#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
// #include <dht.h>

#define FIRE_MIN = 0
#define FIRE_MAX = 1024
File sensorData;
// DHT dht;

int main() {
    init();
    // dht dht11;


    Serial.begin(9600);
    while (!Serial) {
    }

    uint8_t counter = 0;

    // pinMode(8, INPUT);
    //
    // dht.setup(7);

    // fire sensor
    pinMode(A0, INPUT);
    // mercury switch
    pinMode(A1, INPUT);
    while(true) {
        Serial.println(analogRead(A0));
        Serial.println(analogRead(A1));
        // works on uno but not nano
        // if (dht.getStatus() == 0) {
        //     Serial.println("ahaha 0 status");
        // }
        // if (dht.getStatusString() == "OK") {
        //     Serial.println("ahahahha OK status");
        // }
        // Serial.println(dht.getStatus());
        // float farh = dht.toFahrenheit(dht.getTemperature());
        // float hum = dht.getHumidity();
        // Serial.println(isnan(farh));
        //
        // Serial.println(farh);
        // Serial.println(hum);


      //   Serial.println("high");
      //   digitalWrite(8, HIGH);
      //
      // delay(1000);
      //     Serial.println("low");
      // digitalWrite(8, LOW);
      // delay(dht.getMinimumSamplingPeriod() * 2);

      delay(1000);
        counter++;
        if (counter == 60) {
            break;
        }
    }

    while (!SD.begin(7)) {
    }
    sensorData = SD.open("data.txt", FILE_WRITE);



    // if the file opened okay, write to it:
    if (sensorData) {
        // close the file
        sensorData.close();
      } else {
          // if the file didn't open, print an error:
          Serial.println("error opening sensorData.txt");
    }

    // re-open the file for reading:
    sensorData = SD.open("data.txt");
    if (sensorData) {
        Serial.println("data.txt:");
        while (sensorData.available()) {
            Serial.write(sensorData.read());
        }
        sensorData.close();
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening sensorData.txt");
    }
    Serial.flush();
}
