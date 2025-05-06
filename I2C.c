#include "I2C.h"

extern void Error_Handler(void);

// Inter-integrated Circuit Interface (I2C)
// up to 100 Kbit/s in the standard mode, 
// up to 400 Kbit/s in the fast mode, and 
// up to 3.4 Mbit/s in the high-speed mode.

// Recommended external pull-up resistance is 
// 4.7 kOmh for low speed, 
// 3.0 kOmh for the standard mode, and 
// 1.0 kOmh for the fast mode
	
//===============================================================================
//                        I2C GPIO Initialization
//===============================================================================
void I2C_GPIO_Init(void) {
// Turn on GPIO clock
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; // Enable clock for GPIOB 

// Set GPIOB pins 6 and 7 to alternate function mode
GPIOB->MODER &= ~(3UL << 12); // Clear mode for PB6
GPIOB->MODER |= (2UL << 12);  // Set PB6 to Alternate Function mode
GPIOB->MODER &= ~(3UL << 14); // Clear mode for PB7
GPIOB->MODER |= (2UL << 14);  // Set PB7 to Alternate Function mode

// Set GPIOB pins 6 and 7 to I2C mode (AF4)
GPIOB->AFR[0] &= ~(0xFUL << 24); // Clear alternate function for PB6
GPIOB->AFR[0] |= (4UL << 24);    // Set PB6 to AF4 (I2C1 SCL)
GPIOB->AFR[0] &= ~(0xFUL << 28); // Clear alternate function for PB7
GPIOB->AFR[0] |= (4UL << 28);    // Set PB7 to AF4 (I2C1 SDA)

// Set GPIOB pins 6 and 7 to Open Drain
GPIOB->OTYPER |= (1UL << 6); // Set PB6 to open-drain
GPIOB->OTYPER |= (1UL << 7); // Set PB7 to open-drain

// Set GPIOB pins 6 and 7 to very high speed
GPIOB->OSPEEDR |= (3UL << 12); // Set very high speed for PB6
GPIOB->OSPEEDR |= (3UL << 14); // Set very high speed for PB7

// Set GPIOB pins 6 and 7 to pull-up
GPIOB->PUPDR &= ~(3UL << 12); // Clear pull-up/down for PB6
GPIOB->PUPDR |= (1UL << 12);  // Enable pull-up for PB6
GPIOB->PUPDR &= ~(3UL << 14); // Clear pull-up/down for PB7
GPIOB->PUPDR |= (1UL << 14);  // Enable pull-up for PB7


}
	
#define I2C_TIMINGR_PRESC_POS	28
#define I2C_TIMINGR_SCLDEL_POS	20
#define I2C_TIMINGR_SDADEL_POS	16
#define I2C_TIMINGR_SCLH_POS	8
#define I2C_TIMINGR_SCLL_POS	0

//===============================================================================
//                          I2C Initialization
//===============================================================================
void I2C1_Init(void) {
    uint32_t deviceAddress = 0x52;

    // Enable clock for I2C1 peripheral
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    // Configure I2C1 clock source to system clock
    RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL_1;
    RCC->CCIPR |= RCC_CCIPR_I2C1SEL_0;

    // Reset and re-enable I2C1
    RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST;
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;

    // Disable I2C1 for configuration
    I2C1->CR1 &= ~I2C_CR1_PE;

    // Enable analog noise filter and disable digital noise filter
    I2C1->CR1 &= ~I2C_CR1_ANFOFF;
    I2C1->CR1 &= ~I2C_CR1_DNF;

    // Enable interrupt on error events
    I2C1->CR1 |= I2C_CR1_ERRIE;

    // Allow clock stretching
    I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;

    // Configure for 7-bit addressing mode
    I2C1->CR2 &= ~I2C_CR2_ADD10;

    // Enable auto-stop condition after all bytes are sent
    I2C1->CR2 |= I2C_CR2_AUTOEND;

    // Enable NACK after the last byte
    I2C1->CR2 |= I2C_CR2_NACK;

    // Set timing prescaler to 7
    I2C1->TIMINGR |= (7UL << I2C_TIMINGR_PRESC_POS);

    // Configure SCL low period and high period
    I2C1->TIMINGR |= (50UL << I2C_TIMINGR_SCLL_POS);
    I2C1->TIMINGR |= (42UL << I2C_TIMINGR_SCLH_POS);

    // Set SDA delay and SCL delay
    I2C1->TIMINGR |= (14UL << I2C_TIMINGR_SDADEL_POS);
    I2C1->TIMINGR |= (12UL << I2C_TIMINGR_SCLDEL_POS);

    // Configure own address 1 and disable 10-bit addressing
    I2C1->OAR1 &= ~I2C_OAR1_OA1EN;   // Disable to configure
    I2C1->OAR1 &= ~I2C_OAR1_OA1MODE; // 7-bit addressing mode
    I2C1->OAR1 &= ~I2C_OAR1_OA1;
    I2C1->OAR1 |= (deviceAddress << 1);  // Set own address

    // Enable own address 1
    I2C1->OAR1 |= I2C_OAR1_OA1EN;

    // Enable the I2C1 peripheral
    I2C1->CR1 |= I2C_CR1_PE;
}


//===============================================================================
//                           I2C Start
// Master generates START condition:
//    -- Secondary address: 7 bits
//    -- Automatically generate a STOP condition after all bytes have been transmitted 
// Direction = 0: Master requests a write transfer
// Direction = 1: Master requests a read transfer
//=============================================================================== 
int8_t I2C_Start(I2C_TypeDef * I2Cx, uint32_t DevAddress, uint8_t Size, uint8_t Direction) {	
	
	// Direction = 0: Master requests a write transfer
	// Direction = 1: Master requests a read transfer
	
	uint32_t tmpreg = 0;
	
	// This bit is set by software, and cleared by hardware after the Start followed by the address
	// sequence is sent, by an arbitration loss, by a timeout error detection, or when PE = 0.
	tmpreg = I2Cx->CR2;
	
	tmpreg &= (uint32_t)~((uint32_t)(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));
	
	if (Direction == READ_FROM_SECONDARY)
		tmpreg |= I2C_CR2_RD_WRN;  // Read from Secondary
	else
		tmpreg &= ~I2C_CR2_RD_WRN; // Write to Secondary
		
	tmpreg |= (uint32_t)(((uint32_t)DevAddress & I2C_CR2_SADD) | (((uint32_t)Size << 16 ) & I2C_CR2_NBYTES));
	
	tmpreg |= I2C_CR2_START;
	
	I2Cx->CR2 = tmpreg; 
	
   	return 0;  // Success
}

//===============================================================================
//                           I2C Stop
//=============================================================================== 
void I2C_Stop(I2C_TypeDef * I2Cx){
	// Master: Generate STOP bit after the current byte has been transferred 
	I2Cx->CR2 |= I2C_CR2_STOP;								
	// Wait until STOPF flag is reset
	while( (I2Cx->ISR & I2C_ISR_STOPF) == 0 ); 
}

//===============================================================================
//                           Wait for the bus is ready
//=============================================================================== 
void I2C_WaitLineIdle(I2C_TypeDef * I2Cx){
	// Wait until I2C bus is ready
	while( (I2Cx->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY );	// If busy, wait
}

//===============================================================================
//                           I2C Send Data
//=============================================================================== 
int8_t I2C_SendData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if (Size <= 0 || pData == NULL) return -1;
	
	I2C_WaitLineIdle(I2Cx);
	
	if (I2C_Start(I2Cx, DeviceAddress, Size, WRITE_TO_SECONDARY) < 0 ) return -1;

	// Send Data
	// Write the first data in DR register
	// while((I2Cx->ISR & I2C_ISR_TXE) == 0);
	// I2Cx->TXDR = pData[0] & I2C_TXDR_TXDATA;  

	for (i = 0; i < Size; i++) {
		// TXE is set by hardware when the I2C_TXDR register is empty. It is cleared when the next
		// data to be sent is written in the I2C_TXDR register.
		// while( (I2Cx->ISR & I2C_ISR_TXE) == 0 ); 

		// TXIS bit is set by hardware when the I2C_TXDR register is empty and the data to be
		// transmitted must be written in the I2C_TXDR register. It is cleared when the next data to be
		// sent is written in the I2C_TXDR register.
		// The TXIS flag is not set when a NACK is received.
		while((I2Cx->ISR & I2C_ISR_TXIS) == 0 );
		I2Cx->TXDR = pData[i] & I2C_TXDR_TXDATA;  // TXE is cleared by writing to the TXDR register.
	}
	
	// Wait until TC flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0 && (I2Cx->ISR & I2C_ISR_NACKF) == 0);
	
	if( (I2Cx->ISR & I2C_ISR_NACKF) != 0 ) return -1;

	I2C_Stop(I2Cx);
	return 0;
}


//===============================================================================
//                           I2C Receive Data
//=============================================================================== 
int8_t I2C_ReceiveData(I2C_TypeDef * I2Cx, uint8_t DeviceAddress, uint8_t *pData, uint8_t Size) {
	int i;
	
	if(Size <= 0 || pData == NULL) return -1;

	I2C_WaitLineIdle(I2Cx);

	I2C_Start(I2Cx, DeviceAddress, Size, READ_FROM_SECONDARY); // 0 = sending data to the secondary, 1 = receiving data from the secondary.
						  	
	for (i = 0; i < Size; i++) {
		// Wait until RXNE flag is set 	
		while( (I2Cx->ISR & I2C_ISR_RXNE) == 0 );
		pData[i] = I2Cx->RXDR & I2C_RXDR_RXDATA;
	}
	
	// Wait until TCR flag is set 
	while((I2Cx->ISR & I2C_ISR_TC) == 0);
	
	I2C_Stop(I2Cx);
	
	return 0;
}
