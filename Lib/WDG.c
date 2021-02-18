
#include "WDG.h"


/*
 * PRIVATE DEFINITIONS
 */

// Note: if the IWDG_PR_MIN is adjusted, so must the IWDG_FREQ_BASE
#define IWDG_PR_MIN				IWDG_PRESCALER_4
#define IWDG_PR_MAX				IWDG_PRESCALER_256
#define IWDG_FREQ_BASE			(LSI_VALUE / 4)

#define IWDG_RELOAD_MAX		0xFFF

// Using div 128, this gives a max period of ~14s
// And a resolution of ~3ms

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

static uint32_t WDG_SelectPrescalar(uint32_t target_period, uint32_t * frequency);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void WDG_Init(uint32_t period)
{
	IWDG->KR = IWDG_KEY_ENABLE;
	IWDG->KR = IWDG_KEY_WRITE_ACCESS_ENABLE;

	uint32_t frequency;
	IWDG->PR = WDG_SelectPrescalar(period, &frequency);

	// +1 to prevent timer being shorter than expected.
	uint32_t reload = (frequency * period / 1000) + 1;
	if (reload > IWDG_RELOAD_MAX) { reload = IWDG_RELOAD_MAX; }

	IWDG->RLR = reload;

	while (IWDG->SR != 0x00); // Wait for the IWDG to start

	// Trigger a reload to kick things off.
	WDG_Kick();

#ifdef DEBUG
	// This prevents a watchdog timeout while halted under debug
	__HAL_DBGMCU_FREEZE_IWDG();
#endif
}

void WDG_Kick(void)
{
	IWDG->KR = IWDG_KEY_RELOAD;
}

/*
 * PRIVATE FUNCTIONS
 */

static uint32_t WDG_SelectPrescalar(uint32_t target_period, uint32_t * frequency)
{
	uint32_t period_ticks = IWDG_FREQ_BASE * target_period / 1000;
	uint32_t k = 0;
	while (k <= IWDG_PR_MAX && period_ticks >= IWDG_RELOAD_MAX)
	{
		k++;
		period_ticks >>= 1;
	}
	*frequency = IWDG_FREQ_BASE >> k;
	return k << IWDG_PR_PR_Pos;
}

/*
 * INTERRUPT ROUTINES
 */
