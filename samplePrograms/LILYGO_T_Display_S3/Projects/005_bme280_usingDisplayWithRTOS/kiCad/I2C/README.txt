T-Display S3 + BME280 I2C schematic
====================================

Files:
- TDisplayS3_BME280_I2C.sch : KiCad legacy schematic (KiCad 6/7/8/9 can open/import it)

Wiring:
T-Display S3 3V3      -> BME280 VCC
T-Display S3 GND      -> BME280 GND
T-Display S3 GPIO01   -> BME280 SCL
T-Display S3 GPIO02   -> BME280 SDA
BME280 CSB            -> 3.3V (I2C mode)
BME280 SDO            -> GND  (I2C address 0x76)

Notes:
- This schematic assumes a common 6-pin BME280 breakout module.
- Many breakout modules already include I2C pull-up resistors.
- If yours does not, add ~4.7 kOhm pull-ups from SDA and SCL to 3.3V.
- For SDO tied to 3.3V instead, the common BME280 I2C address is 0x77.
- T-Display S3 standard I2C pins per current LILYGO documentation: SCL=GPIO17, SDA=GPIO18.

Opening in modern KiCad:
1. Keep all files in the same directory.
2. Open TDisplayS3_BME280_I2C.sch in KiCad Schematic Editor.
3. KiCad may offer to convert/save it to the current .kicad_sch format.
