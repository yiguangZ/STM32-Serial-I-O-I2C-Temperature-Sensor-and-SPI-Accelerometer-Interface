# 📡 STM32 Serial I/O, I2C Temperature Sensor, and SPI Accelerometer Interface (ECE 153B Labs 4A–6)

This project collection demonstrates interfacing with peripheral devices using UART, I2C, and SPI on the STM32L476 microcontroller. Each lab introduces a new communication protocol and its integration with sensors or output control, culminating in a full SPI accelerometer reader with UART output.

## 🔍 Lab Overview

| Lab  | Topic                          | Description                                                                 |
|------|--------------------------------|-----------------------------------------------------------------------------|
| 4A   | UART + GPIO                    | Use UART (USART1) to receive terminal input and control the on-board LED   |
| 4B   | I2C + UART                     | Read temperature from TMP102 sensor via I2C and print to terminal via UART |
| 6    | SPI + UART + Accelerometer     | Interface with an ADXL345 accelerometer using SPI and display values via UART |

## 🧰 Features

- **Lab 4A: UART Input/Output**
  - UART1 connected to Bluetooth (or Termite via jumper)
  - Terminal command turns on/off on-board green LED (PA5)

- **Lab 4B: I2C Temperature Sensing**
  - Reads temperature from TMP102 sensor using I2C protocol (PB6, PB7)
  - UART2 prints temperature data to serial monitor

- **Lab 6: SPI Accelerometer**
  - Initializes and reads acceleration data (x, y, z) from ADXL345 via SPI1
  - Outputs real-time measurements via UART2 at 9600 baud
  - SysTick-based delay for periodic sampling

## 📁 File Structure

| File Name          | Function                                          |
|--------------------|---------------------------------------------------|
| `main__1_ (1).c`    | Lab 4A – UART LED control                        |
| `main (3).c`        | Lab 4B – I2C temperature sensor + UART output   |
| `main (4).c`        | Lab 6 – SPI accelerometer + UART output         |
| `I2C.c`             | I2C bus initialization and data transfer logic  |
| `SPI.c`             | SPI bus setup and ADXL345 communication         |
| `accelerometer.c`   | ADXL345 initialization and data reading         |
| `UART (2).c`        | UART initialization and basic USART functions   |
| `LED (1|2).c`       | GPIO LED setup and toggle                       |
| `SysTimer (1).c`    | SysTick timer configuration and delay function  |

## 📦 Dependencies

- STM32CubeIDE or Keil MDK (CMSIS-based build environment)
- STM32L476 Discovery board
- TMP102 I2C temperature sensor (Lab 4B)
- ADXL345 SPI accelerometer (Lab 6)

## 🚀 How to Run

1. Select the corresponding `main*.c` file for your lab.
2. Build and flash to your STM32 board.
3. For UART:
   - Use Termite or PuTTY (baud rate: 9600)
4. For I2C:
   - Connect TMP102 to PB6 (SCL) and PB7 (SDA)
5. For SPI:
   - Connect ADXL345 to SPI1 (PA4, PB3–PB5)

## 👤 Author

Yiguang Zhu — ECE 153B Labs 4A, 4B, and 6 (Winter 2024)
