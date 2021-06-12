#ifndef COMP_H
#define COMP_H

#include "STM32X.h"

/*
 * FUNCTIONAL TESTING
 * STM32L0: N
 * STM32F0: NA
 */

/*
 * PUBLIC DEFINITIONS
 */

/*
 * PUBLIC TYPES
 */

typedef struct {

}COMP_t;

/*
 * PUBLIC FUNCTIONS
 */

void COMP_Init(COMP_t * comp);
void COMP_Deinit(COMP_t * comp);

void COMP_Start(COMP_t * comp);
void COMP_Stop(COMP_t * comp);

/*
 * EXTERN DECLARATIONS
 */

#endif //COMP_H
