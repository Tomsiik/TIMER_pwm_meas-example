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
 * Tom� Mat�jka
 * 27.2.2018
 */

/**

Program pro realizaci ��ta�e a odes�l�n� hodnoty frekvence na seriov� port.

 * Vstup pro sign�l je na pinu GPIOA pin 1.
 * Tento pin je p�es alternativn� funkci p�ipojen k timeru (TIM2) kde je rozveden do dvou kan�l�.
 * Kan�ly jsou nakonfigurov�ny tak �e jeden sn�m� n�b�nou hranu a druh� sestupnou,
 * nav�c sign�l sestupn� hrany je p�iveden do trigger controleru, kde p�es slavemode resetuje hlavn� ��ta� timeru.
 * Oba kan�ly jsou samoz�ejm� ve vstupn�m re�imu, tedy Capture.
 * Konfigurace timeru je provedena tak, �e ka�dou 1us inkrementuje hodnotu v ��ta�i.
 * Funguje to tak, �e timer pokud nem� p�iveden sign�l do vstupu ��t� a� do hodnoty autoreload a zde se vyresetuje.
 * Po p�iveden� periodick�ho sign�lu (0-3V3, SQUARE), za�nou kan�ly reagovat na nastaven� hrany.
 * Pokud prvn� kan�l detekuje n�b�nou stranu vep�e hodnotu ��ta�e do sv�ho Capture registru,
 * to sam� d�l� kan�l reaguj�c� na sestupnou hranu s t�m �e z�rove� resetuje ��ta�.
 * V�sledek je takov� �e v prvn�m capture registru se nach�z� d�lka stavu log.0 a v druh�m registru d�lka log.1.
 * Sou�et t�chto hodnot je periodou sign�lu.
 * Jednoduch�m v�po�tem se z periody z�sk� frekvence, kter� je pomoc� seriov�ho portu odes�l�na z mikrokontrol�ru.
 *
 *
 * UART je samoz�ejm� nutn� nakonfigurovat a povolit ho. To sam� je nutn� ud�lat s piny p�i�azen�mi k tomuto UARTU.
 *
 * Pou�it� dokumenty:
 * AN4776: General-purpose timer cookbook
 * AN4013: STM32 cross-series timer overview
 * RM0394: STM32L43xxx STM32L44xxx STM32L45xxx STM32L46xxx advanced ARM�-based 32-bit MCUs
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
	//povolen� clocku do TIM2
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	//nastaven� prescalleru k z�sk�n� 1us periody
	LL_TIM_SetPrescaler(TIM2,31);
	//Autoreload hodnota na max
	LL_TIM_SetAutoReload(TIM2,65535);
	//volba m�du ��ta�e na inkrementaci
	LL_TIM_SetCounterMode(TIM2,LL_TIM_COUNTERMODE_UP);
	//konfigurace druh�ho kan�lu, p��m� vstup,s�gn�l bez p�edd�len�, bez filtrace, sn�m� sestupnou hranu.
	LL_TIM_IC_Config(TIM2,LL_TIM_CHANNEL_CH2,
			LL_TIM_ACTIVEINPUT_DIRECTTI|LL_TIM_ICPSC_DIV1|LL_TIM_IC_FILTER_FDIV1|LL_TIM_IC_POLARITY_FALLING);
	//konfigurace prvn�ho kan�lu, nep��m� vstup,s�gn�l bez p�edd�len�, bez filtrace, sn�m� n�b�nou hranu.
	LL_TIM_IC_Config(TIM2,LL_TIM_CHANNEL_CH1,
			LL_TIM_ACTIVEINPUT_INDIRECTTI|LL_TIM_ICPSC_DIV1|LL_TIM_IC_FILTER_FDIV1|LL_TIM_IC_POLARITY_RISING);

	//nasteven� z jak�ho vstupu bude trigger controller p�ij�mat trigger(sign�l ze sestupn� hrany
	LL_TIM_SetTriggerInput(TIM2,LL_TIM_TS_TI2FP2);
	//nasteven� ud�losti p�i triggeru
	LL_TIM_SetSlaveMode(TIM2,LL_TIM_SLAVEMODE_RESET);
	//povolen� kan�lu ��ta��
	LL_TIM_CC_EnableChannel(TIM2,LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2);
	//povolen� samotn�ho timeru
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

