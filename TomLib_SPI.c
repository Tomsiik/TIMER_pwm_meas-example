/*
 * TomLib_SPI.c
 *
 *  Created on: 14. 10. 2017
 *      Author: Tomsik
 */

#include <Periph_Init.h>
#include"main.h"
#include"TomLib_NRF24L01.h"
#include"TomLib_SPI.h"



void TL_SPI_Set_CSN(){
	//while(LL_SPI_IsActiveFlag_BSY(TL_SPI)==1){}
	LL_GPIO_SetOutputPin(GPIOB,TL_CSN);
}

void TL_SPI_Reset_CSN(){
	//while(LL_SPI_IsActiveFlag_BSY(TL_SPI)==1){}
	LL_GPIO_ResetOutputPin(GPIOB,TL_CSN);
}


void TL_SPI_Transmit_Byte(uint8_t dataIn){
	while(LL_SPI_IsActiveFlag_TXE(TL_SPI)==0){}
	LL_SPI_TransmitData8(TL_SPI,dataIn);

	while(LL_SPI_IsActiveFlag_BSY(TL_SPI)==1){}
	LL_SPI_ReceiveData8(TL_SPI);
}


void TL_SPI_Transmit_Multi(uint8_t dataIn[],uint32_t size){

	for(uint8_t count=0; count<size; count++){
		while(LL_SPI_IsActiveFlag_TXE(TL_SPI)==0){}
		LL_SPI_TransmitData8(TL_SPI,dataIn[count]);
		while(LL_SPI_IsActiveFlag_BSY(TL_SPI)==1){}
		LL_SPI_ReceiveData8(TL_SPI);
	}


}

uint8_t TL_SPI_ReceiveTransmit_Byte(uint8_t dataIn){
	while(LL_SPI_IsActiveFlag_TXE(TL_SPI)==0){}
	LL_SPI_TransmitData8(TL_SPI,dataIn);
	while(LL_SPI_IsActiveFlag_RXNE(TL_SPI)==0){}
	while(LL_SPI_IsActiveFlag_BSY(TL_SPI)==1){}
	return LL_SPI_ReceiveData8(TL_SPI);
}

void TL_SPI_ReceiveTransmit_Multi(uint8_t dataIn[],uint8_t dataOut[],uint16_t size){
	uint8_t count=0;
	for(count=0;count<size;count++){
		while(LL_SPI_IsActiveFlag_TXE(TL_SPI)==0){}
		LL_SPI_TransmitData8(TL_SPI,dataIn[count]);

		while(LL_SPI_IsActiveFlag_RXNE(TL_SPI)==0){}
		dataOut[count]=LL_SPI_ReceiveData8(TL_SPI);
		while(LL_SPI_IsActiveFlag_BSY(TL_SPI)==1){}


	}
}

