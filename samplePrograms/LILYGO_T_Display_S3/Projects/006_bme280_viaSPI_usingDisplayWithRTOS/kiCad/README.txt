T-Display S3 + BME280 SPI schematic

Files:
  TDisplayS3_BME280_SPI.sch   KiCad legacy schematic (editable; KiCad 6/7/8/9 can import/open and save as .kicad_sch)
  TDisplayS3_BME280_SPI.pro   minimal legacy project file

Connections:
  3V3    -> BME280 3V3/VDD
  GND    -> BME280 GND
  GPIO10 -> SCK
  GPIO11 -> SDI/MOSI
  GPIO12 <- SDO/MISO
  GPIO13 -> CS/CSB

The standard T-Display S3 onboard LCD uses GPIO5-9, 15, 38-48; GPIO10-13 are exposed and suitable choices for an external SPI device.
ESP32-S3 uses a flexible GPIO matrix, so the Arduino SPI bus can be initialized with these pins explicitly.
