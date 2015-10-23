#include "gpio.h"
#include "gpio_from_sdk.h"

//************************ start ******************************************//
//chris.choi : should ask china's driver team that it can be used like this
// this code is come from keil driver version code
//************************ start ******************************************//
/*
#ifdef CHIPVERSION_B0
#define UART1_RX_VECTOR_TABLE_INDEX		16
#define UART1_TX_VECTOR_TABLE_INDEX		17
#define UART2_RX_VECTOR_TABLE_INDEX		18
#define UART2_TX_VECTOR_TABLE_INDEX		19
#else
#define UART1_RX_VECTOR_TABLE_INDEX		16
#define UART1_TX_VECTOR_TABLE_INDEX		17
#define UART2_RX_VECTOR_TABLE_INDEX		18
#define UART2_TX_VECTOR_TABLE_INDEX		19
#endif	//CHIPVERSION_B0
*/

extern void (*handle_ext_wakeup_isr)(void);
void gpio1_combined_isr_handler(void);





// chris.choi : jeffy's code 150908, so it's better code i think so use this
void gpio1_combined_isr_handler(void)
{
	uint8_t index = 0;
	//portint_callback_t callback;
	uint8_t port = LPGPIO_16;
	uint32_t intstatus = GPIO1->INTSTATUSCLEAR.reg;//jeffy
	GPIO1->INTTYPESET.reg |= (1<<15); //jeffy
	
	do {
		#ifdef CHIPVERSION_B0
		intstatus &= 0x1FF;
		#else
		intstatus &= 0x1F;
		#endif
		index = 0;
		if(intstatus != 0) {
			port = (intstatus & ~(intstatus - 1));
			while(!(port == 1)) {
				port = port >> 1;
				index++;
			}
			
			//chris.choi : i think that port_int_callback is always null but if not, then it makes error because callback is not called
			//if(port_int_callback[index+LPGPIO_16] != NULL) {
			//callback = port_int_callback[index+LPGPIO_16];
			//callback();
			//}
			//CMSDK_GPIO1->INTCLEAR = (1 << index);
			///REG_GPIO1_INTSTATUSCLEAR = (1 << index);
			GPIO1->INTSTATUSCLEAR.reg = (1 << index);/// jeffy
		}
		else {
			//CMSDK_GPIO1->INTCLEAR = CMSDK_GPIO1->INTSTATUS;
			GPIO1->INTSTATUSCLEAR.reg |= (1<<15);//jeffy
			//REG_GPIO1_INTSTATUSCLEAR = REG_GPIO1_INTENSET;
		}
		intstatus = GPIO1->INTSTATUSCLEAR.reg;
		#ifdef CHIPVERSION_B0
		intstatus &= 0x1FF;
		#else
		intstatus &= 0x1F;
		#endif
	}while(intstatus != 0);
	
	NVIC_ClearPendingIRQ(8);
}


void gpio0_combined_isr_handler(void)
{
	uint8_t index = 0;
	//portint_callback_t callback;
	uint8_t port = LPGPIO_0;
	uint32_t intstatus = GPIO0->INTSTATUSCLEAR.reg;//jeffy
	GPIO0->INTTYPESET.reg |= (1<<15); //jeffy
	
	do {
		#ifdef CHIPVERSION_B0
		intstatus &= 0x1FF;
		#else
		intstatus &= 0x1F;
		#endif
		index = 0;
		if(intstatus != 0) {
			port = (intstatus & ~(intstatus - 1));
			while(!(port == 1)) {
				port = port >> 1;
				index++;
			}
			
			//chris.choi : i think that port_int_callback is always null but if not, then it makes error because callback is not called
			//if(port_int_callback[index+LPGPIO_16] != NULL) {
			//callback = port_int_callback[index+LPGPIO_16];
			//callback();
			//}
			//CMSDK_GPIO1->INTCLEAR = (1 << index);
			///REG_GPIO1_INTSTATUSCLEAR = (1 << index);
			GPIO0->INTSTATUSCLEAR.reg = (1 << index);/// jeffy
		}
		else {
			//CMSDK_GPIO1->INTCLEAR = CMSDK_GPIO1->INTSTATUS;
			GPIO0->INTSTATUSCLEAR.reg |= (1<<15);//jeffy
			//REG_GPIO1_INTSTATUSCLEAR = REG_GPIO1_INTENSET;
		}
		intstatus = GPIO0->INTSTATUSCLEAR.reg;
		#ifdef CHIPVERSION_B0
		intstatus &= 0x1FF;
		#else
		intstatus &= 0x1F;
		#endif
	}while(intstatus != 0);
	
	NVIC_ClearPendingIRQ(7);
}

#ifdef CHIPVERSION_B0
void PORT1_COMB_Handler(void)
{
	//if(CMSDK_GPIO1->INTSTATUS & ((1<<15) | (1<<14) | (1<<13))) {
	if(GPIO1->INTSTATUSCLEAR.reg & ((1<<15) | (1<<14) | (1<<13))) {
		
		handle_ext_wakeup_isr();
		
		// clear specific int pin status that caused the Interrupt
		//CMSDK_GPIO1->INTCLEAR |= CMSDK_GPIO1->INTSTATUS & ((1<<15) | (1<<14) | (1<<13));
		GPIO1->INTSTATUSCLEAR.reg |= GPIO1->INTSTATUSCLEAR.reg & ((1<<15) | (1<<14) | (1<<13));
		//NVIC_ClearPendingIRQ(PORT1_COMB_IRQn);
		NVIC_ClearPendingIRQ(8);
	}
	else
	{
		gpio1_combined_isr_handler();
	}
}

#endif	//CHIPVERSION_B0






















// chris.choi : it's my code, so i don't want to use it.
//void gpio1_combined_isr_handler(void)
//{
//uint8_t index = 0;
////portint_callback_t callback;
//uint8_t port = LPGPIO_16;
//uint32_t intstatus = REG_GPIO1_INTENSET;
//
//do {
//#ifdef CHIPVERSION_B0
//intstatus &= 0x1FF;
//#else
//intstatus &= 0x1F;
//#endif
//index = 0;
//if(intstatus != 0) {
//port = (intstatus & ~(intstatus - 1));
//while(!(port == 1)) {
//port = port >> 1;
//index++;
//}
//
////chris.choi : i think that port_int_callback is always null but if not, then it makes error because callback is not called
////if(port_int_callback[index+LPGPIO_16] != NULL) {
////callback = port_int_callback[index+LPGPIO_16];
////callback();
////}
////CMSDK_GPIO1->INTCLEAR = (1 << index);
//REG_GPIO1_INTSTATUSCLEAR = (1 << index);
//
//}
//else {
////CMSDK_GPIO1->INTCLEAR = CMSDK_GPIO1->INTSTATUS;
//REG_GPIO1_INTSTATUSCLEAR = REG_GPIO1_INTENSET;
//}
//intstatus = REG_GPIO1_INTENSET;
//}while(intstatus != 0);
//
//}
//void gpio0_combined_isr_handler(void)
//{
//uint8_t port = LPGPIO_0;
//uint8_t index = 0;
////portint_callback_t callback;
//uint32_t intstatus = REG_GPIO0_INTENSET;
//do {
//intstatus &= 0xFFFF;
//if(intstatus != 0) {
//index = 0;
//port = (intstatus & ~(intstatus - 1));
//while(!(port == 1)) {
//port = port >> 1;
//index++;
//}
////chris.choi : i think that port_int_callback is always null but if not, then it makes error because callback is not called
////if(port_int_callback[index] != NULL) {
////callback = port_int_callback[index];
////callback();
////}
//REG_GPIO0_INTSTATUSCLEAR = (1 << index);
//}
//else {
//REG_GPIO0_INTSTATUSCLEAR = REG_GPIO0_INTENSET;
//}
//intstatus = REG_GPIO0_INTENSET;
//}while(intstatus != 0);
//}
//************************ end ******************************************//
//chris.choi : should ask china's driver team that it can be used like this
// this code is come from keil driver version code
//************************ end ******************************************//


