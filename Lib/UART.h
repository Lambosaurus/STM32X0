#ifndef UART_H
#define UART_H

#include "STM32X.h"

/*
 * FUNCTIONAL TESTING
 * STM32L0: Y
 * STM32F0: N
 */

/*
 * PUBLIC DEFINITIONS
 */

#ifndef UART_BFR_SIZE
#define UART_BFR_SIZE 64
#endif

/*
 * PUBLIC TYPES
 */

typedef struct
{
	uint8_t buffer[UART_BFR_SIZE];
	uint32_t head;
	uint32_t tail;
} UART_Buffer_t;

typedef struct {
	UART_Buffer_t tx;
	UART_Buffer_t rx;
	USART_TypeDef * Instance;
} UART_t;


/*
 * PUBLIC FUNCTIONS
 */

// Initialisation
void UART_Init(UART_t * uart, uint32_t baud);
void UART_Deinit(UART_t * uart);

// Transmit
void UART_Write(UART_t * uart, const uint8_t * data, uint32_t count);
void UART_WriteStr(UART_t * uart, const char * str);
void UART_FlushWrite(UART_t * uart);

// Recieve
uint32_t UART_ReadCount(UART_t * uart);
uint32_t UART_Read(UART_t * uart, uint8_t * data, uint32_t count);
uint8_t UART_Pop(UART_t * uart);
void UART_FlushRead(UART_t * uart);


/*
 * EXTERN DECLARATIONS
 */

#ifdef UARTLP_GPIO
extern UART_t * UART_LP;
#endif
#ifdef UART1_GPIO
extern UART_t * UART_1;
#endif
#ifdef UART2_GPIO
extern UART_t * UART_2;
#endif
#ifdef UART3_GPIO
extern UART_t * UART_3;
#endif
#ifdef UART4_GPIO
extern UART_t * UART_4;
#endif
#ifdef UART5_GPIO
extern UART_t * UART_5;
#endif

#endif //UART_H
