#ifndef COMP_H
#define COMP_H

#include "STM32X.h"
#include "GPIO.h"

// Use this bit to detect whether the comparator modules are available for this MCU
#ifdef COMP_CSR_COMPxEN
#define COMP_ENABLED

/*
 * FUNCTIONAL TESTING
 * STM32L0: N
 * STM32F0: N
 */

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

// Note - some options based on the internal VREF exist, and could be supported in future.
typedef enum {
	COMP_Pos_IO1 = COMP_INPUT_PLUS_IO1,
	COMP_Pos_IO2 = COMP_INPUT_PLUS_IO2,
	COMP_Pos_IO3 = COMP_INPUT_PLUS_IO3,
	COMP_Pos_IO4 = COMP_INPUT_PLUS_IO4,
	COMP_Pos_IO5 = COMP_INPUT_PLUS_IO5,
#ifdef COMP_INPUT_PLUS_IO6
	COMP_Pos_IO6 = COMP_INPUT_PLUS_IO6,
#endif
	COMP_Neg_IO1 = COMP_INPUT_MINUS_IO1,
	COMP_Neg_IO2 = COMP_INPUT_MINUS_IO2,
	COMP_Input_Inverted = COMP_OUTPUTPOL_INVERTED,
} COMP_Input_t;

typedef struct {
	COMP_TypeDef * Instance;
#ifdef COMP_USE_IRQS
	VoidFunction_t callback;
#endif
}COMP_t;

/*
 * PUBLIC FUNCTIONS
 */

void COMP_Init(COMP_t * comp, COMP_Input_t inputs);
void COMP_Deinit(COMP_t * comp);
bool COMP_Read(COMP_t * comp);

#ifdef COMP_USE_IRQS
void COMP_OnChange(COMP_t * comp, GPIO_IT_Dir_t dir, VoidFunction_t callback);
#endif

/*
 * EXTERN DECLARATIONS
 */

#ifdef COMP1_ENABLE
extern COMP_t * COMP_1;
#endif
#ifdef COMP2_ENABLE
extern COMP_t * COMP_2;
#endif

#endif //COMP_ENABLED
#endif //COMP_H
