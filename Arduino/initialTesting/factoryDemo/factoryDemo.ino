/*********************************************************************
This is an example for our nRF52 based Bluefruit LE modules
Pick one up today in the adafruit shop!
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
MIT license, check LICENSE for more information
All text above, and the splash screen below must be included in
any redistribution
*********************************************************************/
#include <bluefruit.h>
#include <Adafruit_NeoPixel.h>
#define NEOPIXELPIN 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.setPixelColor(0, 0, 50, 0);
  strip.setBrightness(20);
  strip.show();  // Initialize all pixels to 'off'
  Serial.println("Bluefruit52 Central Scan Example");
  Serial.println("--------------------------------\n");
  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
    Bluefruit.begin(0, 1);
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName("Bluefruit52");
  // Start Central Scan
  Bluefruit.setConnLedInterval(250);
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.start(0);
  Serial.println("Scanning ...");
}
void scan_callback(ble_gap_evt_adv_report_t *report) {
  Serial.println("Timestamp Addr Rssi Data");
  Serial.printf("%09d ", millis());
  // MAC is in little endian --&gt; print reverse
  Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
  Serial.print(" ");
  Serial.print(report->rssi);
  Serial.print(" ");
  Serial.printBuffer(report->data.p_data, report->data.len, '-');
  Serial.println();
  // Check if advertising contain BleUart service
  if (Bluefruit.Scanner.checkReportForUuid(report, BLEUART_UUID_SERVICE)) {
    Serial.println(" BLE UART service detected");
  }
  Serial.println();
  // For Softdevice v6: after received a report, scanner will be paused
  // We need to call Scanner resume() to continue scanning
  Bluefruit.Scanner.resume();
}
uint8_t i = 0;
void loop() {
  strip.setPixelColor(0, Wheel(i++));
  strip.show();
  delay(10);
}
  // Input a value 0 to 255 to get a color value.
  // The colours are a transition r - g - b - back to r.
  uint32_t
  Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
