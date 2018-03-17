/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @author  Ac6
  * @version V1.0
  * @date    02-Feb-2015
  * @brief   Default Interrupt Service Routines.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#ifdef USE_RTOS_SYSTICK
#include <cmsis_os.h>
#endif
#include "stm32l4xx_it.h"
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            	  	    Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles SysTick Handler, but only if no RTOS defines it.
  * @param  None
  * @retval None
  */

__IO uint32_t TimmingDelay;
__IO uint32_t t1,t2;

void SysTick_Handler(void)
{	 if(TimmingDelay !=0)
	{
		TimmingDelay --;
	}


}
void _Error_Handler(char * file, int line) {
	printf("chyba");
	while (1) {
	}
}

void TIM2_IRQHandler(void){
 // nezkoumám zdroj pøerušení (vím že to je CC2, jiný jsem nepovolil)
 LL_TIM_ClearFlag_CC2(TIM2); // mažu vlajku pøerušení
 t1=LL_TIM_IC_GetCaptureCH1(TIM2); // èas pøíchodu sestupné hrany (trvání pulzu log.1)
 t2=LL_TIM_IC_GetCaptureCH2(TIM2); // èas pøíchodu vzestupné hrany (trvání celé periody)

	 GPIOB->ODR=LL_GPIO_PIN_13;


}



#ifdef USE_RTOS_SYSTICK
	osSystickHandler();
#endif

