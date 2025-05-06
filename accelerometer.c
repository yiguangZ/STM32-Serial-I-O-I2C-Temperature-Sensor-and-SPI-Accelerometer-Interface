#include "SPI.h"
#include "SysTimer.h"
#include "accelerometer.h"

void accWrite(uint8_t addr, uint8_t val){
	// TODO access SPI_Transfer_Data
  SPI_Transfer_Data(((addr & 0x3F) << 8) | val);

}

uint8_t accRead(uint8_t addr){
    return SPI_Transfer_Data(((addr & 0x3F) | 0x80) << 8);
}

void initAcc(void){
	accWrite(0x2C, 0x0A);     // Set 100Hz output data rate
  accWrite(0x31, 0x08);     // Set full resolution mode
  accWrite(0x2D, 0x08);     // Enable measurement mode
}

void readValues(double* x_out, double* y_out, double* z_out) {
    uint8_t x_low = accRead(0x32);
    uint8_t x_high = accRead(0x33);
    uint8_t y_low = accRead(0x34);
    uint8_t y_high = accRead(0x35);
    uint8_t z_low = accRead(0x36);
    uint8_t z_high = accRead(0x37);

    int16_t x_raw_value = (x_high << 8) | x_low;
    int16_t y_raw_value = (y_high << 8) | y_low;
    int16_t z_raw_value = (z_high << 8) | z_low;

    double scale_factor = 0.0039;  // Scale factor in g/LSB for full-resolution mode
    *x_out = x_raw_value * scale_factor;
    *y_out = y_raw_value * scale_factor;
    *z_out = z_raw_value * scale_factor;
}


