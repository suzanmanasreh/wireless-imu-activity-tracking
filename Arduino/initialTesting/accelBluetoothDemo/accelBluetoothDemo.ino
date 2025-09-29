#include <bluefruit.h>
#include <Adafruit_LSM6DS33.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXELPIN 8
#define NODENUMBER 1

BLEUart bleuart;
Adafruit_LSM6DS33 lsm6ds33;
Adafruit_LIS3MDL lis3mdl;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
int brightness = 10;


void setup() {
  Serial.begin(115200);
  //while (!Serial);

  if (NODENUMBER == 1) {
    strip.setPixelColor(0, 0, 0, 255);
  } else {
    strip.setPixelColor(0, 255, 0, 0);
  }
  strip.setBrightness(brightness);
  strip.show();

  // Initialize IMU
  if (!lsm6ds33.begin_I2C()) {
    Serial.println("Failed to find LSM6DS33!");
    while (1);
  }

  if (! lis3mdl.begin_I2C()) {          // hardware I2C mode, can pass in address & alt Wire
  //if (! lis3mdl.begin_SPI(LIS3MDL_CS)) {  // hardware SPI mode
  //if (! lis3mdl.begin_SPI(LIS3MDL_CS, LIS3MDL_CLK, LIS3MDL_MISO, LIS3MDL_MOSI)) { // soft SPI
    Serial.println("Failed to find LIS3MDL chip");
    while (1) { delay(10); }
  }

  // Set IMU range to a higher level
  lsm6ds33.setAccelRange(LSM6DS_ACCEL_RANGE_8_G);
  lsm6ds33.setAccelDataRate(LSM6DS_RATE_208_HZ);

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

  lsm6ds33.getEvent(&accel, &gyro, &temp);
  lis3mdl.getEvent(&mag);
  /* Display the results (magnetic field is measured in uTesla) */

  // Format as ASCII string
  char buf0[128];
  snprintf(buf0, sizeof(buf0), "%d,T,%d\n",
           NODENUMBER,
           millis()
           );
  
  char buf1[128];
  snprintf(buf1, sizeof(buf1), "%d,A,%.1f,%.1f,%.1f\n",
           NODENUMBER,
           accel.acceleration.x,
           accel.acceleration.y,
           accel.acceleration.z
           );

  char buf2[128];
  snprintf(buf2, sizeof(buf2), "%d,G,%.1f,%.1f,%.1f\n",
           NODENUMBER,
           gyro.gyro.x,
           gyro.gyro.y,
           gyro.gyro.z
           );

  char buf3[128];
  snprintf(buf3, sizeof(buf3), "%d,M,%.1f,%.1f,%.1f\n",
           NODENUMBER,
           mag.magnetic.x,
           mag.magnetic.y,
           mag.magnetic.z
           );

  // Send via BLE UART
  if (Bluefruit.connected() && bleuart.notifyEnabled()) {
    bleuart.print(buf0);
    bleuart.print(buf1);
    bleuart.print(buf2);
    bleuart.print(buf3);
  }

  if (brightness == 10) {
    strip.setBrightness(5);
    brightness = 5;
  } else {
    strip.setBrightness(10);
    brightness = 10;
  }
  strip.show();

  delay(5); // trying to match 208 Hz update
}
