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

  uint32_t time = millis();

  lsm6ds33.getEvent(&accel, &gyro, &temp);
  lis3mdl.getEvent(&mag);
  
  /* Display the results (magnetic field is measured in uTesla) */

  // Format as ASCII string
  char buf0[32];
  snprintf(buf0, sizeof(buf0), "%d,AX,%d,%.1f\n",
           NODENUMBER,
           time,
          accel.acceleration.x
           );
  
  char buf1[32];
  snprintf(buf1, sizeof(buf1), "%d,AY,%d,%.1f\n",
           NODENUMBER,
           time,
          accel.acceleration.y
           );

    char buf2[32];
  snprintf(buf2, sizeof(buf2), "%d,AZ,%d,%.1f\n",
           NODENUMBER,
           time,
          accel.acceleration.z
           );

    char buf3[32];
  snprintf(buf3, sizeof(buf3), "%d,MX,%d,%.1f\n",
           NODENUMBER,
           time,
          mag.magnetic.x
           );

    char buf4[32];
  snprintf(buf4, sizeof(buf4), "%d,MY,%d,%.1f\n",
           NODENUMBER,
           time,
          mag.magnetic.y
           );

      char buf5[32];
  snprintf(buf5, sizeof(buf5), "%d,MZ,%d,%.1f\n",
           NODENUMBER,
           time,
          mag.magnetic.z
           );

          char buf6[32];
  snprintf(buf6, sizeof(buf6), "%d,GX,%d,%.1f\n",
           NODENUMBER,
           time,
          gyro.gyro.x
           );

          char buf7[64];
  snprintf(buf7, sizeof(buf7), "%d,GY,%d,%.1f\n",
           NODENUMBER,
           time,
          gyro.gyro.y
           );


              char buf8[32];
  snprintf(buf8, sizeof(buf8), "%d,GZ,%d,%.1f\n",
           NODENUMBER,
           time,
          gyro.gyro.z
           );

  // Send via BLE UART
  if (Bluefruit.connected() && bleuart.notifyEnabled()) {
    bleuart.print(buf0);
    bleuart.print(buf1);
    bleuart.print(buf2);
    bleuart.print(buf3);
    bleuart.print(buf4);
    bleuart.print(buf5);
    bleuart.print(buf6);
    bleuart.print(buf7);
    bleuart.print(buf8);
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
