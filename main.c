//
//
// PB5 -> LED  on board
// PC4 -> Trig on HC-SR04
// PD4 -> Echo on HC_SR04
// PD5 -> RX   on USBtoUART
//

#include "stm8s.h"
#include <stdio.h>

// Instead of putchar() which is called by printf(). 
char putchar(char c) {
	while(!UART1_GetFlagStatus(UART1_FLAG_TXE));
	UART1_SendData8(c);
	return c;
}

// CPU must runs on 16MHz.
void delay_10us(void)
{
	uint16_t i;
	for (i=0;i<3;i++) {
	}
}

// CPU must runs on 16MHz.
void delay_1ms(void)
{
	uint16_t i;
	for (i=0;i<1070;i++) ;
}

// CPU must runs on 16MHz.
void delay_ms(uint16_t n)
{
	uint16_t i;
	for (i=0;i<n;i++) {
		delay_1ms();
	}
}

uint16_t overflow_cnt;

void main()
{
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); // CPU: 16MHz
	
	// UART1 is used for printf().
	UART1_DeInit();
	UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TX_ENABLE|UART1_MODE_RX_DISABLE);
	
	printf("hcsr04 start...\n");
	
	GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST); // LED on board.
	GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
	
	TIM2_DeInit();
	TIM2_TimeBaseInit(TIM2_PRESCALER_16, 1000);
	TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);

	TIM2_CCxCmd(TIM2_CHANNEL_1, DISABLE);
	TIM2_CCxCmd(TIM2_CHANNEL_2, DISABLE);

	// No APIs in STM8S_StdPeriph_Lib can support to do the follows.
	TIM2->CCMR1 &= ~0x03; // CC1S = 00
	TIM2->CCMR1 |=  0x01; // CC1S = 01 , IC1 is mapped on TI1FP1
	TIM2->CCER1 &= ~0x02; // CC1P=0, rising of TI1F  

	TIM2->CCMR2 &= ~0x03; // CC1S = 00
	TIM2->CCMR2 |=  0x02; // CC2S = 10 , IC2 is mapped on TI1FP2
	TIM2->CCER1 |=  0x20; // CC2P=1, falling of TI1F
	
	TIM2_CCxCmd(TIM2_CHANNEL_1, ENABLE);
	TIM2_CCxCmd(TIM2_CHANNEL_2, ENABLE);

	TIM2_SetIC1Prescaler(TIM2_ICPSC_DIV1);
	TIM2_SetIC2Prescaler(TIM2_ICPSC_DIV1);
	
	TIM2_ITConfig(TIM2_IT_CC1, ENABLE);
	TIM2_ITConfig(TIM2_IT_CC2, ENABLE);
	
	enableInterrupts();
	
	while (1) {
		delay_ms(5000);
		GPIO_WriteHigh(GPIOC, GPIO_PIN_4);
		delay_10us();
		delay_10us();
		GPIO_WriteLow(GPIOC, GPIO_PIN_4);
	}
}

@far @interrupt void TIM2_UPD_OVF_IRQHandler(void)
{
	if (TIM2_GetITStatus(TIM2_IT_UPDATE)) {
		TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
		overflow_cnt++;
	}	
}

@far @interrupt void TIM2_CAP_COM_IRQHandler(void)
{
	if (TIM2_GetITStatus(TIM2_IT_CC1)) {
		TIM2_ClearITPendingBit(TIM2_IT_CC1);
		TIM2_SetCounter(0);
		overflow_cnt = 0;
		TIM2_Cmd(ENABLE);
	}
	
	if (TIM2_GetITStatus(TIM2_IT_CC2)) {
		TIM2_ClearITPendingBit(TIM2_IT_CC2);
		TIM2_Cmd(DISABLE);
		printf("%ucm\n", ( overflow_cnt * 1000 + TIM2_GetCounter() ) / 58 );
	}
}

void assert_failed(u8* file, u32 line)
{
	while (1)
	{
	}
}

