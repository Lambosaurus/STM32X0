
#include "COMP.h"

#ifdef COMP_ENABLED

/*
 * PRIVATE DEFINITIONS
 */

#ifndef COMP_IRQn
#define COMP_IRQn		ADC1_COMP_IRQn
#endif

#define WRITE_BIT(reg, bit, set)  (reg = (set) ? (reg | (bit)) : (reg & ~(bit)))

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

/*
 * PRIVATE VARIABLES
 */

#ifdef COMP1_ENABLE
static COMP_t gCOMP_1 = {
	.Instance = COMP1,
};
COMP_t * COMP_1 = &gCOMP_1;
#endif
#ifdef COMP2_ENABLE
static COMP_t gCOMP_2 = {
	.Instance = COMP2,
};
COMP_t * COMP_2 = &gCOMP_2;
#endif

/*
 * PUBLIC FUNCTIONS
 */

void COMP_Init(COMP_t * comp, COMP_Input_t inputs)
{
	comp->Instance->CSR = inputs
			| COMP_POWERMODE_MEDIUMSPEED
			| COMP_CSR_COMPxEN;
}

void COMP_Deinit(COMP_t * comp)
{
#ifdef COMP_USE_IRQS
	uint32_t exti = COMP_GET_EXTI_LINE(comp->Instance);
	CLEAR_BIT(EXTI->IMR, exti);
#endif
	comp->Instance->CSR = 0;
}

bool COMP_Read(COMP_t * comp)
{
	return comp->Instance->CSR & COMP_CSR_COMPxOUTVALUE;
}

#ifdef COMP_USE_IRQS
void COMP_OnChange(COMP_t * comp, GPIO_IT_Dir_t dir, VoidFunction_t callback)
{
	comp->callback = callback;

	__HAL_RCC_SYSCFG_CLK_ENABLE();
    uint32_t exti = COMP_GET_EXTI_LINE(comp->Instance);
    WRITE_BIT(EXTI->RTSR, exti, (dir & GPIO_IT_Rising));
    WRITE_BIT(EXTI->FTSR, exti, (dir & GPIO_IT_Falling));
	// Clear EXTI pending bit - in case its already set
	WRITE_REG(EXTI->PR, exti);
	SET_BIT(EXTI->IMR, exti);

	HAL_NVIC_EnableIRQ(COMP_IRQn);
}
#endif //COMP_USE_IRQS

/*
 * PRIVATE FUNCTIONS
 */

/*
 * INTERRUPT ROUTINES
 */

#ifdef COMP_USE_IRQS
static inline void COMP_IRQHandler(COMP_t * comp, uint32_t exti)
{
	if(EXTI->PR & exti)
	{
		WRITE_REG(EXTI->PR, exti);
		comp->callback();
	}
}

void ADC1_COMP_IRQHandler(void)
{
	// This will have to be shared with the ADC handler at some point...
#ifdef COMP1_ENABLE
	COMP_IRQHandler(COMP_1, COMP_EXTI_LINE_COMP1);
#endif
#ifdef COMP2_ENABLE
	COMP_IRQHandler(COMP_2, COMP_EXTI_LINE_COMP2);
#endif
}
#endif


#endif // COMP_ENABLED
