/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2013; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : EHS
*                 DC
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

static   OS_STK      AppTaskStartStk[APP_TASK_START_STK_SIZE];

OS_EVENT *led_sem;

OS_STK 							LED0_TASK_STK[LED0_STK_SIZE];
OS_STK 							LED1_TASK_STK[LED1_STK_SIZE];
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
void LED_Init(void);

static  void  AppTaskCreate  (void);
static	void	AppEventCreate (void);

static  void  AppTaskStart   (void *p_arg);

void led0_task(void *pdata);
void led1_task(void *pdata);
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

int  main (void)
{
    LED_Init();		  	//????LED???????

    OSInit();                                                   /* Initialize "uC/OS-II, The Real-Time Kernel"              */

		OSTaskCreate(AppTaskStart,(void *)0,(OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1],APP_TASK_START_PRIO );//??????

    OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)       */
}


/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
	OS_CPU_SR cpu_sr=0;

  RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	OS_CPU_SysTickInit(rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC);

	OS_ENTER_CRITICAL();			//?????(???????)  
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                               /* Determine CPU capacity                                   */
#endif

    //APP_TRACE_INFO(("Creating Application Events...\n\r"));
    AppEventCreate();                                           /* Create Application Kernel objects                        */

    //APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    AppTaskCreate();                                            /* Create application tasks                                 */

    OSTaskSuspend(APP_TASK_START_PRIO);	//??????.
		OS_EXIT_CRITICAL();				//?????(???????)
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION TASKS
*
* Description:  This function creates the application tasks.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{  
 	OSTaskCreate(led0_task,(void *)0,(OS_STK*)&LED0_TASK_STK[LED0_STK_SIZE-1],LED0_TASK_PRIO);						   
 	OSTaskCreate(led1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);
}



/*
*********************************************************************************************************
*                                          AppEventCreate()
*
* Description : Create application kernel objects tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static void AppEventCreate(void)
{
	led_sem = OSSemCreate(0);
}

/*
*********************************************************************************************************
*                                         FUNCTION DEFINITION
*********************************************************************************************************
*/

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
