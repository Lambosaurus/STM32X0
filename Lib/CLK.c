
#include "CLK.h"

/*
 * PRIVATE DEFINITIONS
 */

/*
 * PRIVATE TYPES
 */

/*
 * PRIVATE PROTOTYPES
 */

#ifdef CLK_USE_LSE
static void CLK_ResetBackupDomain(void)
#endif
static void CLK_AccessBackupDomain(void);

/*
 * PRIVATE VARIABLES
 */

/*
 * PUBLIC FUNCTIONS
 */

void CLK_InitSYSCLK(void)
{
	RCC_OscInitTypeDef osc = {0};
#ifdef CLK_USE_HSE
	osc.OscillatorType 		= RCC_OSCILLATORTYPE_HSE;
	osc.HSEState 			= RCC_HSE_ON;
	osc.PLL.PLLState 		= RCC_PLL_ON;
	osc.PLL.PLLSource 		= RCC_PLLSOURCE_HSE;
	osc.PLL.PLLMUL 			= RCC_PLL_MUL2;
#ifdef STM32F0
	osc.PLL.PREDIV			= RCC_PREDIV_DIV1;
#else
	osc.PLL.PLLDIV 			= RCC_PLL_DIV1;
#endif
#else
	osc.OscillatorType 		= RCC_OSCILLATORTYPE_HSI;
	osc.HSIState 			= RCC_HSI_ON;
	osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	osc.PLL.PLLState 		= RCC_PLL_ON;
	osc.PLL.PLLSource 		= RCC_PLLSOURCE_HSI;
	osc.PLL.PLLMUL 			= RCC_PLL_MUL4;
#ifdef STM32F0
	osc.PLL.PREDIV			= RCC_PREDIV_DIV2;
#else
	osc.PLL.PLLDIV 			= RCC_PLL_DIV2;
#endif
#endif //CLK_USE_HSE
	HAL_RCC_OscConfig(&osc);

	RCC_ClkInitTypeDef clk = {0};
	clk.ClockType 		= RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	clk.SYSCLKSource 	= RCC_SYSCLKSOURCE_PLLCLK;
	clk.AHBCLKDivider 	= RCC_SYSCLK_DIV1;
	clk.APB1CLKDivider 	= RCC_HCLK_DIV1;
#ifdef STM32L0
	clk.ClockType 		|= RCC_CLOCKTYPE_PCLK2;
	clk.APB2CLKDivider  = RCC_HCLK_DIV1;
#endif
	HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_1);
}


#ifdef USB_ENABLE
void CLK_EnableUSBCLK(void)
{
	__HAL_RCC_USB_CONFIG(RCC_USBCLKSOURCE_HSI48);
	__HAL_RCC_HSI48_ENABLE();
	while(!__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY));
}
void CLK_DisableUSBCLK(void)
{
	__HAL_RCC_HSI48_DISABLE();
	// No need to wait for disable.
}
#endif


void CLK_EnableLSO(void)
{
#ifdef CLK_USE_LSE
#ifdef CLK_LSE_BYPASS
	__HAL_RCC_LSE_CONFIG(RCC_LSE_BYPASS);
#else
	__HAL_RCC_LSE_CONFIG(RCC_LSE_ON);
#endif
	while(!__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY));

	CLK_AccessBackupDomain();
	CLK_ResetBackupDomain();
	__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
#else
	__HAL_RCC_LSI_ENABLE();
	while (!__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY));
	CLK_AccessBackupDomain();
	__HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSI);
#endif
}

void CLK_DisableLSO(void)
{
#ifdef CLK_USE_LSE
	__HAL_RCC_LSE_CONFIG(RCC_LSE_OFF);
#else
	__HAL_RCC_LSI_DISABLE();
#endif
}

/*
 * PRIVATE FUNCTIONS
 */

#ifdef CLK_USE_LSE
static void CLK_ResetBackupDomain(void)
{
	uint32_t csr = (RCC->CSR & ~(RCC_CSR_RTCSEL));
	// RTC Clock selection can be changed only if the Backup Domain is reset
	__HAL_RCC_BACKUPRESET_FORCE();
	__HAL_RCC_BACKUPRESET_RELEASE();
	RCC->CSR = csr;
}
#endif

static void CLK_AccessBackupDomain(void)
{
	if (HAL_IS_BIT_CLR(PWR->CR, PWR_CR_DBP))
	{
		// Get access to backup domain
		SET_BIT(PWR->CR, PWR_CR_DBP);
		while(HAL_IS_BIT_CLR(PWR->CR, PWR_CR_DBP));
	}
}

void CLK_OscConfig(void)
{
#ifdef CLK_USE_HSE
#ifdef CLK_HSE_BYPASS
	__HAL_RCC_HSE_CONFIG(RCC_HSE_BYPASS);
#else
	__HAL_RCC_HSE_CONFIG(RCC_HSE_ON);
#endif
	while(!__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY));
#else
	// If not using HSE, use HSI. Logic to change in future....
	__HAL_RCC_HSI_CONFIG(RCC_HSI_ON);
	__HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_HSICALIBRATION_DEFAULT);
	while(!__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY));
#endif

#ifdef CLK_USE_MSI
	__HAL_RCC_MSI_ENABLE();
	while(!__HAL_RCC_GET_FLAG(RCC_FLAG_MSIRDY));
	__HAL_RCC_MSI_RANGE_CONFIG(RCC_MSIRANGE_5);
	__HAL_RCC_MSI_CALIBRATIONVALUE_ADJUST(RCC_MSICALIBRATION_DEFAULT);
#endif

  /*-------------------------------- PLL Configuration -----------------------*/
  /* Check the parameters */
  assert_param(IS_RCC_PLL(RCC_OscInitStruct->PLL.PLLState));
  if ((RCC_OscInitStruct->PLL.PLLState) != RCC_PLL_NONE)
  {
    /* Check if the PLL is used as system clock or not */
    if(sysclk_source != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
    {
      if((RCC_OscInitStruct->PLL.PLLState) == RCC_PLL_ON)
      {
        /* Check the parameters */
        assert_param(IS_RCC_PLLSOURCE(RCC_OscInitStruct->PLL.PLLSource));
        assert_param(IS_RCC_PLL_MUL(RCC_OscInitStruct->PLL.PLLMUL));
        assert_param(IS_RCC_PLL_DIV(RCC_OscInitStruct->PLL.PLLDIV));

        /* Disable the main PLL. */
        __HAL_RCC_PLL_DISABLE();

        /* Get Start Tick */
        tickstart = HAL_GetTick();

        /* Wait till PLL is disabled */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  != 0U)
        {
          if((HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }

        /* Configure the main PLL clock source, multiplication and division factors. */
        __HAL_RCC_PLL_CONFIG(RCC_OscInitStruct->PLL.PLLSource,
                             RCC_OscInitStruct->PLL.PLLMUL,
                             RCC_OscInitStruct->PLL.PLLDIV);
        /* Enable the main PLL. */
        __HAL_RCC_PLL_ENABLE();

        /* Get Start Tick */
        tickstart = HAL_GetTick();

        /* Wait till PLL is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  == 0U)
        {
          if((HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else
      {
        /* Disable the main PLL. */
        __HAL_RCC_PLL_DISABLE();

        /* Get Start Tick */
        tickstart = HAL_GetTick();

        /* Wait till PLL is disabled */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  != 0U)
        {
          if((HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
    else
    {
      /* Check if there is a request to disable the PLL used as System clock source */
      if((RCC_OscInitStruct->PLL.PLLState) == RCC_PLL_OFF)
      {
        return HAL_ERROR;
      }
      else
      {
        /* Do not return HAL_ERROR if request repeats the current configuration */
        pll_config = RCC->CFGR;
        if((READ_BIT(pll_config, RCC_CFGR_PLLSRC) != RCC_OscInitStruct->PLL.PLLSource) ||
           (READ_BIT(pll_config, RCC_CFGR_PLLMUL) != RCC_OscInitStruct->PLL.PLLMUL) ||
           (READ_BIT(pll_config, RCC_CFGR_PLLDIV) != RCC_OscInitStruct->PLL.PLLDIV))
        {
          return HAL_ERROR;
        }
      }
    }
  }

  return HAL_OK;
}

/*
 * INTERRUPT ROUTINES
 */

