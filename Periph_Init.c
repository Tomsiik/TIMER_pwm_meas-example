#include <Periph_Init.h>
#include "main.h"
#include "stm32l4xx.h"

void SystemClock_Config(void) {
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	LL_RCC_HSE_Enable();
	LL_RCC_HSE_EnableBypass();

	while (LL_RCC_HSE_IsReady() != 1) {}

	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_1, 16,LL_RCC_PLLR_DIV_4);
	LL_RCC_PLL_EnableDomain_SYS();
	LL_RCC_PLL_Enable();

	while (LL_RCC_PLL_IsReady() != 1) {}
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL) {}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
	LL_RCC_SetI2CClockSource(LL_RCC_I2C2_CLKSOURCE_PCLK1);

	LL_RCC_ConfigMCO(LL_RCC_MCO1SOURCE_SYSCLK, LL_RCC_MCO1_DIV_1);

	SystemCoreClockUpdate();

	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
	SysTick_Config(SystemCoreClock / 100000);
	NVIC_SetPriority(SysTick_IRQn,NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
	LL_SYSTICK_DisableIT();

}


