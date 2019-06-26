/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "stm32f10x.h"

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void LED_Init(void);

void Delay(u32 count);

/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
int main(void)
{	
	LED_Init();		  	//???LED??
	
	while(1)
	{		
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		GPIO_SetBits(GPIOE,GPIO_Pin_5);
		Delay(8000000);
		GPIO_ResetBits(GPIOE,GPIO_Pin_5);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		Delay(8000000);
	};
}

/*
*********************************************************************************************************
*                                         FUNCTION DEFINITION
*********************************************************************************************************
*/

void Delay(u32 count)
{
	u32 i=0;
	for(;i<count;i++);
}

void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	    //使能PB,PE端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			    //LED0-->PB.5端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO通信速率50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			     //初始化GPIOB.5
	GPIO_SetBits(GPIOB,GPIO_Pin_5);					//PB.5输出高电平
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	            //LED1-->PE.5端口配置
	GPIO_Init(GPIOE, &GPIO_InitStructure);	  	       //初始化GPIOE.5
	GPIO_SetBits(GPIOE,GPIO_Pin_5); 			 //PE.5输出高电平
}
