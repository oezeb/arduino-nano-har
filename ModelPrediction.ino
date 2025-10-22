#include "MyBoschSensor.h"
#include "SignalQueue.h"
#include "features.h"

void handle_signal() {
  float ax, ay, az, gx, gy, gz;
  if (myIMU.readAcceleration(ax, ay, az) && myIMU.readGyroscope(gx, gy, gz)) {
    signal_queue.push(ax, ay, az, gx, gy, gz);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Starting");

  myIMU.debug(Serial);
  myIMU.onInterrupt(handle_signal);
  if (!myIMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  
  Serial.println("Started");
  Serial.print("Accelerometer sample rate = ");
  Serial.print(myIMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println("Acceleration in G's");
  Serial.print("Gyroscope sample rate = ");
  Serial.print(myIMU.gyroscopeSampleRate());
  Serial.println(" Hz");
  Serial.println("Gyroscope in degrees/second");
  Serial.println();
}

float ax[WINDOW], ay[WINDOW], az[WINDOW], gx[WINDOW], gy[WINDOW], gz[WINDOW];
float features[120]; // 20 features * 6 axes
void loop() {
  if (signal_queue.is_full()) {
    signal_queue.peek(ax, ay, az, gx, gy, gz);
    extract_features(ax, WINDOW, &features[0]);
    extract_features(ay, WINDOW, &features[20]);
    extract_features(az, WINDOW, &features[40]);
    extract_features(gx, WINDOW, &features[60]);
    extract_features(gy, WINDOW, &features[80]);
    extract_features(gz, WINDOW, &features[100]);
    Serial.println("Extracted features:");
    for (int i = 0; i < 120; i++) {
      Serial.print(features[i], 6);
      if (i < 119) {
        Serial.print(", ");
      }
    }
    Serial.println();

    delay(1000);
  }
}
