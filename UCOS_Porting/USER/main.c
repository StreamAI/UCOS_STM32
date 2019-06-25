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

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include <includes.h>

/////////////////////////UCOSII????///////////////////////////////////
OS_EVENT *led_sem;

void LED_Init(void);

//START ??
//???????
#define START_TASK_PRIO      			10 //?????????????
//????????
#define START_STK_SIZE  				64
//????	
OS_STK START_TASK_STK[START_STK_SIZE];
//????
void start_task(void *pdata);	
 			   
//LED0??
//???????
#define LED0_TASK_PRIO       			7 
//????????
#define LED0_STK_SIZE  		    		64
//????	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//????
void led0_task(void *pdata);


//LED1??
//???????
#define LED1_TASK_PRIO       			6 
//????????
#define LED1_STK_SIZE  					64
//????
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//????
void led1_task(void *pdata);


int main(void)
{	  
		LED_Init();		  	//????LED???????

		OSInit();   
		OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//??????
		OSStart();
}

//????
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata;
	led_sem = OSSemCreate(0);
	
	RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	OS_CPU_SysTickInit(rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC);
	
	OSStatInit();					//???????.?????1????
	
  OS_ENTER_CRITICAL();			//?????(???????)    
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(led1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
	
	OSTaskSuspend(START_TASK_PRIO);	//??????.
	OS_EXIT_CRITICAL();				//?????(???????)
}

//LED0??
void led0_task(void *pdata)
{	 	
	while(1)
	{		
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		OSTimeDly(1000);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		OSTimeDly(1000);
		
		OSSemPost(led_sem);
	};
}

//LED1??
void led1_task(void *pdata)
{
	INT8U err;
	
	while(1)
	{
		OSSemPend(led_sem,0,&err);
		
		GPIO_ResetBits(GPIOE,GPIO_Pin_5);
		OSTimeDly(500);
		GPIO_SetBits(GPIOE,GPIO_Pin_5);
		OSTimeDly(500);
	};
}

//LED IO???
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
