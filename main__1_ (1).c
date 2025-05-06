/*
 * ECE 153B
 *
 * Name(s):
 * Section:
 * Lab: 4A
 */


#include "stm32l476xx.h"
#include "LED.h"
#include "SysClock.h"
#include "UART.h"
#include <string.h>
#include <stdio.h>

// Initializes USARTx
// USART2: UART Communication with Termite
// USART1: Bluetooth Communication with Phone
void Init_USARTx(int x) {
	if(x == 1) {
		UART1_Init();
		UART1_GPIO_Init();
		USART_Init(USART1);
	} else if(x == 2) {
		UART2_Init();
		UART2_GPIO_Init();
		USART_Init(USART2);
	} else {
		// Do nothing...
	}
}

int main(void) {
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	// Initialize UART -- change the argument depending on the part you are working on
	Init_USARTx(1);
	printf("UART initialized and ready. Please enter a command.\r\n");

	char rxByte;
	while(1) {
 // Send prompt message to the terminal
        printf("Enter command (Y/y to turn on LED, N/n to turn off LED):\r\n");

        // Receive a response from Termite
        scanf("%c", &rxByte);

        if (rxByte == 'Y' || rxByte == 'y') {
            Green_LED_On();
            printf("Green LED has been turned ON.\r\n");
        } else if (rxByte == 'N' || rxByte == 'n') {
            Green_LED_Off();
            printf("Green LED has been turned OFF.\r\n");
        } else {
            printf("Unrecognized command. Please enter Y/y or N/n.\r\n");	}
}
}