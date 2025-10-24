#include "MyBoschSensor.h"
#include "SignalQueue.h"
#include "features.h"
#include "xgb_mlgen.h"

Eloquent::ML::Port::XGBClassifier xgb;

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
  
  Serial.println("time,predicted-class,feature-extraction-time-us,prediction-time-us");
}

float ax[WINDOW], ay[WINDOW], az[WINDOW], gx[WINDOW], gy[WINDOW], gz[WINDOW];
float features[120]; // 20 features * 6 axes
char output[128];
void loop() {
  while (signal_queue.is_full()) {
    signal_queue.peek(ax, ay, az, gx, gy, gz);

    ulong start = micros();
    extract_features(ax, WINDOW, &features[0]);
    extract_features(ay, WINDOW, &features[20]);
    extract_features(az, WINDOW, &features[40]);
    extract_features(gx, WINDOW, &features[60]);
    extract_features(gy, WINDOW, &features[80]);
    extract_features(gz, WINDOW, &features[100]);
    
    ulong start2 = micros();
    int y_pred = xgb.predict(features);
    ulong end = micros();
    snprintf(output, sizeof(output), "%lu,%d,%lu,%lu", start, y_pred, start2 - start, end - start2);
    Serial.println(output);

    // delay(1000);
  }
}
