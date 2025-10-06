#include <bluefruit.h>
#include <Adafruit_LSM6DS33.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXELPIN 8
#define NODENUMBER 2

BLEUart bleuart;
Adafruit_LSM6DS33 lsm6ds33;
Adafruit_LIS3MDL lis3mdl;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(115200);

  // Set up the fancy LED color to tell nodes apart visually
  if (NODENUMBER == 1) {
    strip.setPixelColor(0, 0, 255, 255); // Cyan
  } else {
    strip.setPixelColor(0, 255, 255, 0); // Orange/Yellow?
  }
  strip.setBrightness(10);
  strip.show();

  // Initialize IMU to get Accel/Gyro readings
  if (!lsm6ds33.begin_I2C()) {
    Serial.println("Failed to find LSM6DS33!");
    while (1);
  }

  // Initialize the device needed for magnetometer measurements
  if (! lis3mdl.begin_I2C()) {          // hardware I2C mode, can pass in address & alt Wire
    Serial.println("Failed to find LIS3MDL chip");
    while (1) { delay(10); }
  }

  // Set IMU to detect acclerometer changes up to +/- 8G's
  lsm6ds33.setAccelRange(LSM6DS_ACCEL_RANGE_8_G);

  // How quickly we update the accelerometer. 50ish is fine for us.
  lsm6ds33.setAccelDataRate(LSM6DS_RATE_52_HZ);

  // Initialize BLE
  Bluefruit.begin(1, 0);
  char name[128];
  snprintf(name, sizeof(name), "Feather Node #%.2d", NODENUMBER);
  Bluefruit.setName(name);
  bleuart.begin();

  Bluefruit.Advertising.addService(bleuart);
  Bluefruit.Advertising.addName();
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.start(0);

  Serial.println("BLE IMU peripheral started");
}

void loop() {
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sensors_event_t mag;

  // Fetch the current time for timestamp purposes
  uint32_t time = millis();

  // Get the accel/gyro/mag measurements from the sensor
  lsm6ds33.getEvent(&accel, &gyro, &temp);
  lis3mdl.getEvent(&mag);
  
  // Organize values into an array of floats/chars for easy loop access
  float values[9] = {
    accel.acceleration.x, accel.acceleration.y, accel.acceleration.z,
    gyro.gyro.x, gyro.gyro.y, gyro.gyro.z,
    mag.magnetic.x, mag.magnetic.y, mag.magnetic.z
  };

  char types[9][3] = {
    "AX", "AY", "AZ", "GX", "GY", "GZ", "MX", "MY", "MZ"
  };

  // Declare our buffer
  char buf[20];

  // Send via BLE UART
  // NOTE: REDUCED MEASUREMENT ACCURACY BEYOND 1M MILLISECS ELAPSED
  if (Bluefruit.connected() && bleuart.notifyEnabled()) {
    for (int i = 0; i < 9; i++) {
      snprintf(buf, 20, "%d,%s,%d,%.3f",
        NODENUMBER,
        types[i],
        time,
        values[i]
      );
      bleuart.print(buf);
    }
  }
}
