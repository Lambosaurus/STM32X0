#ifndef GPIO_H
#define GPIO_H

#include "STM32X.h"

/*
 * FUNCTIONAL TESTING
 * STM32L0: Y
 * STM32F0: Y
 */

/*
 * PUBLIC DEFINITIONS
 */

#define GPIOCFG_MODE_POS		0
#define GPIOCFG_PULL_POS		4
#define GPIOCFG_SPEED_POS		8
#define GPIOCFG_FLAG_POS		12

/*
 * PUBLIC TYPES
 */

typedef GPIO_TypeDef GPIO_t;

typedef enum {
	GPIO_IT_Rising 			= 0x01,
	GPIO_IT_Falling 		= 0x02,
	GPIO_IT_Both 			= GPIO_IT_Rising | GPIO_IT_Falling,
} GPIO_IT_Dir_t;

typedef enum {
	GPIO_Pull_None 			= GPIO_NOPULL << GPIOCFG_PULL_POS,
	GPIO_Pull_Up   			= GPIO_PULLUP << GPIOCFG_PULL_POS,
	GPIO_Pull_Down 			= GPIO_PULLDOWN << GPIOCFG_PULL_POS,
	GPIO_Pull_MASK			= 0x03 << GPIOCFG_PULL_POS,
} GPIO_Pull_t;

typedef enum {
	GPIO_Mode_Input			= 0x00 << GPIOCFG_MODE_POS,
	GPIO_Mode_Output 		= 0x01 << GPIOCFG_MODE_POS,
	GPIO_Mode_Alternate 	= 0x02 << GPIOCFG_MODE_POS,
	GPIO_Mode_Analog    	= 0x03 << GPIOCFG_MODE_POS,
	GPIO_Mode_MASK			= 0x03 << GPIOCFG_MODE_POS,
} GPIO_Mode_t;

typedef enum {
	GPIO_Speed_Slow     	= 0x00 << GPIOCFG_SPEED_POS,
	GPIO_Speed_Medium		= 0x01 << GPIOCFG_SPEED_POS,
	GPIO_Speed_Fast			= 0x02 << GPIOCFG_SPEED_POS,
	GPIO_Speed_High			= 0x03 << GPIOCFG_SPEED_POS,
	GPIO_Speed_MASK			= 0x03 << GPIOCFG_SPEED_POS,
} GPIO_Speed_t;

typedef enum {
	GPIO_Flag_None			= 0,
	GPIO_Flag_OpenDrain  	= 0x01 << GPIOCFG_FLAG_POS,
} GPIO_Flag_t;

typedef bool GPIO_State_t;

/*
 * PUBLIC FUNCTIONS
 */

// Base initialisation. This does not need to be called by the user.
void GPIO_Init(GPIO_t * gpio, uint32_t pin, GPIO_Flag_t flag);

// Initialisation
static inline void GPIO_EnableOutput(GPIO_t * gpio, uint32_t pin, GPIO_State_t state);
static inline void GPIO_EnableInput(GPIO_t * gpio, uint32_t pin, GPIO_Pull_t pull);
void GPIO_EnableAlternate(GPIO_t * gpio, uint32_t pin, GPIO_Flag_t flags, uint32_t af);
static inline void GPIO_Deinit(GPIO_t * gpio, uint32_t pin);

#ifdef GPIO_USE_IRQS
void GPIO_EnableIRQ(GPIO_t * gpio, uint32_t pin, GPIO_Pull_t pull, GPIO_IT_Dir_t dir, VoidFunction_t callback);
#endif //GPIO_USE_IRQS

// Outputs
void GPIO_Write(GPIO_t * gpio, uint32_t pin, GPIO_State_t state);
static inline void GPIO_Set(GPIO_t * gpio, uint32_t pin);
static inline void GPIO_Reset(GPIO_t * gpio, uint32_t pin);

// Inputs
static inline GPIO_State_t GPIO_Read(GPIO_t * gpio, uint32_t pin);
static inline uint32_t GPIO_ReadPort(GPIO_t * gpio, uint32_t pins);


/*
 * EXTERN DECLARATIONS
 */

#include "GPIO.inl"

#endif //GPIO_H
