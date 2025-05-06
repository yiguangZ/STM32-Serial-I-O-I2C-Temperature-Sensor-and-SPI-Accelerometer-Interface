#include "UART.h"

void UART1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // Enable USART1 clock
    RCC->CCIPR &= ~(RCC_CCIPR_USART1SEL); // Clear USART1 clock source selection
    RCC->CCIPR |= RCC_CCIPR_USART1SEL_0;  // Set system clock as source for USART1}
}
void UART2_Init(void) {
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;//USART 2 clock
	RCC->CCIPR &= ~(RCC_CCIPR_USART2SEL); //cleart USART2 clock source selection
	RCC->CCIPR |= RCC_CCIPR_USART2SEL_0; //set system clock as source for USART2
	
	
	
	
}

void UART1_GPIO_Init(void) {
	 RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;  // Enable GPIO clock for Port B

    // Set PB6 and PB7 to alternate function mode for USART1 (AF7)
    GPIOB->MODER &= ~(3UL << (12)); // Clear PB6 mode
    GPIOB->MODER |= (2UL << (12));  // Set PB6 to AF mode
    GPIOB->MODER &= ~(3UL << (14)); // Clear PB7 mode
    GPIOB->MODER |= (2UL << (14));  // Set PB7 to AF mode

    // Set alternate function for PB6 and PB7 to USART1 (AF7)
    GPIOB->AFR[0] |= (7UL << (24)); // Set PB6 AF to AF7
    GPIOB->AFR[0] |= (7UL << (28)); // Set PB7 AF to AF7

   //Set I/O output speed value as very high speed for PB6 PB7
	GPIOB->OSPEEDR &= ~(3UL<<12);
	GPIOB->OSPEEDR |= 3UL<<12;
	GPIOB->OSPEEDR &= ~(3UL<<14);
	GPIOB->OSPEEDR |= 3UL<<14;
	// GPIO Pull up
	GPIOB->PUPDR &= ~(3UL<<12);
	GPIOB->PUPDR &= ~(3UL<<14);
	GPIOB->PUPDR |= (1UL<<12);
	GPIOB->PUPDR &= (1UL<<14);
	//GPIO Push pull
	GPIOB->OTYPER &= ~(1UL<<6);
	GPIOB->OTYPER &= ~(1UL<<7);
}

void UART2_GPIO_Init(void) {
	// Enable GPIO Clock for Port A
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	 // Set PA2 and PA3 to alternate function mode (AF7 for USART2)
    GPIOA->MODER &= ~(3UL << (4));  // Clear PA2 mode
    GPIOA->MODER |= (2UL << (4));   // Set PA2 to AF mode
    GPIOA->MODER &= ~(3UL << (6));  // Clear PA3 mode
    GPIOA->MODER |= (2UL << (6));   // Set PA3 to AF mode

    // Set alternate function for PA2 and PA3 to USART2 (AF7)
    GPIOA->AFR[0] |= (7UL << 8);  // Set PA2 AF to AF7
    GPIOA->AFR[0] |= (7UL << 12);  // Set PA3 AF to AF7
	//Set I/O output speed value as very high speed for PA2 PA3
	GPIOA->OSPEEDR &= ~(3UL<<4);
	GPIOA->OSPEEDR |= 3UL<<4;
	GPIOA->OSPEEDR &= ~(3UL<<6);
	GPIOA->OSPEEDR |= 3UL<<6;
	// GPIO Pull up
	GPIOA->PUPDR &= ~(3UL<<4);
	GPIOA->PUPDR &= ~(3UL<<6);
	GPIOA->PUPDR |= (1UL<<4);
	GPIOA->PUPDR &= (1UL<<6);
	//GPIO Push pull
	GPIOA->OTYPER &= ~(1UL<<2);
	GPIOA->OTYPER &= ~(1UL<<3);

}

void USART_Init(USART_TypeDef* USARTx) {
  USARTx->CR1 &= ~USART_CR1_UE;
	USARTx->CR1 &= ~USART_CR1_M;   // Set word length to 8 bits
  USARTx->CR1 &= ~USART_CR1_OVER8; // Set oversampling mode to 16
  USARTx->CR2 &= ~USART_CR2_STOP;  // Set number of stop bits to 1

  // Calculate and set baud rate to 9600
  uint32_t USARTDIV = 80000000 / 9600;
  USARTx->BRR = USARTDIV;

  USARTx->CR1 |= USART_CR1_TE | USART_CR1_RE;  // Enable transmitter and receiver
  USARTx->CR1 |= USART_CR1_UE;  // Enable USART
}

uint8_t USART_Read (USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(40000);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}
