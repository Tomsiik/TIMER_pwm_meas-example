/**
 ******************************************************************************
 * @file    main.c


 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */
/**
 * Tomáš Matìjka
 * 27.2.2018
 */

/**

Program pro realizaci èítaèe a odesílání hodnoty frekvence na seriový port.

 * Vstup pro signál je na pinu GPIOA pin 1.
 * Tento pin je pøes alternativní funkci pøipojen k timeru (TIM2) kde je rozveden do dvou kanálù.
 * Kanály jsou nakonfigurovány tak že jeden snímá nábìžnou hranu a druhý sestupnou,
 * navíc signál sestupné hrany je pøiveden do trigger controleru, kde pøes slavemode resetuje hlavní èítaè timeru.
 * Oba kanály jsou samozøejmì ve vstupním režimu, tedy Capture.
 * Konfigurace timeru je provedena tak, že každou 1us inkrementuje hodnotu v èítaèi.
 * Funguje to tak, že timer pokud nemá pøiveden signál do vstupu èítá až do hodnoty autoreload a zde se vyresetuje.
 * Po pøivedení periodického signálu (0-3V3, SQUARE), zaènou kanály reagovat na nastavené hrany.
 * Pokud první kanál detekuje nábìžnou stranu vepíše hodnotu èítaèe do svého Capture registru,
 * to samé dìlá kanál reagující na sestupnou hranu s tím že zároveò resetuje èítaè.
 * Výsledek je takový že v prvním capture registru se nachází délka stavu log.0 a v druhém registru délka log.1.
 * Souèet tìchto hodnot je periodou signálu.
 * Jednoduchým výpoètem se z periody získá frekvence, která je pomocí seriového portu odesílána z mikrokontroléru.
 *
 *
 * UART je samozøejmì nutné nakonfigurovat a povolit ho. To samé je nutné udìlat s piny pøiøazenými k tomuto UARTU.
 *
 * Použité dokumenty:
 * AN4776: General-purpose timer cookbook
 * AN4013: STM32 cross-series timer overview
 * RM0394: STM32L43xxx STM32L44xxx STM32L45xxx STM32L46xxx advanced ARM®-based 32-bit MCUs
 *
 */

#include"main.h"

extern uint32_t t1,t2;
uint32_t capture_time;
float freq=0.00;
char uart_data[];
int main(void) {

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	LL_GPIO_SetPinMode(GPIOB,LL_GPIO_PIN_13,LL_GPIO_MODE_OUTPUT);

	SystemClock_Config();
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	LL_GPIO_SetPinMode(GPIOA,LL_GPIO_PIN_1,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetAFPin_0_7(GPIOA,LL_GPIO_PIN_1,LL_GPIO_AF_1);
uart_init();
	LL_GPIO_SetPinMode(GPIOA,LL_GPIO_PIN_2,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinMode(GPIOA,LL_GPIO_PIN_3,LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetPinSpeed(GPIOA,LL_GPIO_PIN_2,LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetPinSpeed(GPIOA,LL_GPIO_PIN_3,LL_GPIO_SPEED_FREQ_VERY_HIGH);
	LL_GPIO_SetAFPin_0_7(GPIOA,LL_GPIO_PIN_2,LL_GPIO_AF_7);
	LL_GPIO_SetAFPin_0_7(GPIOA,LL_GPIO_PIN_3,LL_GPIO_AF_7);

	general_purpose_tim();


	while (1) {
		capture_time=LL_TIM_IC_GetCaptureCH2(TIM2);
		freq = (float)(1/((float)capture_time/1000000));
		sprintf(uart_data,"Freqency: %.2f Hz \n\r",freq);
		TL_USART_printf(uart_data);
		TL_mDelay(100);

	}
}


void general_purpose_tim(){
	//povolení clocku do TIM2
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	//nastavení prescalleru k získání 1us periody
	LL_TIM_SetPrescaler(TIM2,31);
	//Autoreload hodnota na max
	LL_TIM_SetAutoReload(TIM2,65535);
	//volba módu èítaèe na inkrementaci
	LL_TIM_SetCounterMode(TIM2,LL_TIM_COUNTERMODE_UP);
	//konfigurace druhého kanálu, pøímý vstup,sígnál bez pøeddìlení, bez filtrace, snímá sestupnou hranu.
	LL_TIM_IC_Config(TIM2,LL_TIM_CHANNEL_CH2,
			LL_TIM_ACTIVEINPUT_DIRECTTI|LL_TIM_ICPSC_DIV1|LL_TIM_IC_FILTER_FDIV1|LL_TIM_IC_POLARITY_FALLING);
	//konfigurace prvního kanálu, nepøímý vstup,sígnál bez pøeddìlení, bez filtrace, snímá nábìžnou hranu.
	LL_TIM_IC_Config(TIM2,LL_TIM_CHANNEL_CH1,
			LL_TIM_ACTIVEINPUT_INDIRECTTI|LL_TIM_ICPSC_DIV1|LL_TIM_IC_FILTER_FDIV1|LL_TIM_IC_POLARITY_RISING);

	//nastevení z jakého vstupu bude trigger controller pøijímat trigger(signál ze sestupné hrany
	LL_TIM_SetTriggerInput(TIM2,LL_TIM_TS_TI2FP2);
	//nastevení události pøi triggeru
	LL_TIM_SetSlaveMode(TIM2,LL_TIM_SLAVEMODE_RESET);
	//povolení kanálu èítaèù
	LL_TIM_CC_EnableChannel(TIM2,LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);
	//povolení samotného timeru
	LL_TIM_EnableCounter(TIM2);


	//NVIC_SetPriority(TIM2_IRQn,2);
	///NVIC_EnableIRQ(TIM2_IRQn);
	//LL_TIM_EnableIT_CC2(TIM2);

}
void uart_init(){
	LL_USART_InitTypeDef USART_InitStruct;
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

	USART_InitStruct.BaudRate=115200;
	USART_InitStruct.DataWidth=LL_USART_DATAWIDTH_8B;
	USART_InitStruct.HardwareFlowControl=LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling=LL_USART_OVERSAMPLING_8;
	USART_InitStruct.Parity=LL_USART_PARITY_NONE;
	USART_InitStruct.StopBits=LL_USART_STOPBITS_1;
	USART_InitStruct.TransferDirection=LL_USART_DIRECTION_TX_RX;
	LL_USART_DisableSCLKOutput(USART2);
	LL_USART_Init(USART2,&USART_InitStruct);
	LL_USART_Enable(USART2);
}

