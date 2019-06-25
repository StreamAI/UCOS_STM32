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
OS_SEM led_sem;
/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;

static  OS_TCB   LED0TCB;
static  OS_TCB   LED1TCB;
/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

static  CPU_STK  LED0_TASK_STK[LED0_STK_SIZE];
static  CPU_STK  LED1_TASK_STK[LED1_STK_SIZE];
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskCreate (void);
static  void  AppObjCreate  (void);
static  void  AppTaskStart  (void *p_arg);

void LED_Init(void);
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
    OS_ERR  err;

    LED_Init();

    OSInit(&err);                                               /* Init uC/OS-III.                                      */

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
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
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
		CPU_SR_ALLOC();
   (void)p_arg;

    //BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();
	
    cpu_clk_freq = BSP_CPU_ClkFreq();				                     /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

		OS_CRITICAL_ENTER();
    Mem_Init();                                                 /* Initialize Memory Management Module                  */
		
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

    //CPU_IntDisMeasMaxCurReset();
 
		//APP_TRACE_INFO(("Creating Application Events...\n\r"));
    AppObjCreate();		/* Create Application Objects                           */
		
    //APP_TRACE_INFO(("Creating Application Tasks...\n\r"));
    AppTaskCreate();                                            /* Create Application Tasks                             */
		
		OSTaskSuspend((OS_TCB*)&AppTaskStartTCB,&err);
    OS_CRITICAL_EXIT();
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
	OS_ERR  err;
	
	OSTaskCreate((OS_TCB     *)&LED0TCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"LED0 Task",
                 (OS_TASK_PTR ) led0_task,
                 (void       *) 0,
                 (OS_PRIO     ) LED0_TASK_PRIO,
                 (CPU_STK    *)&LED0_TASK_STK[0],
                 (CPU_STK_SIZE) LED0_STK_SIZE / 10,
                 (CPU_STK_SIZE) LED0_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
								 
	OSTaskCreate((OS_TCB     *)&LED1TCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"LED1 Task",
                 (OS_TASK_PTR ) led1_task,
                 (void       *) 0,
                 (OS_PRIO     ) LED1_TASK_PRIO,
                 (CPU_STK    *)&LED1_TASK_STK[0],
                 (CPU_STK_SIZE) LED1_STK_SIZE / 10,
                 (CPU_STK_SIZE) LED1_STK_SIZE,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}


/*
*********************************************************************************************************
*                                      CREATE APPLICATION EVENTS
*
* Description:  This function creates the application kernel objects.
*
* Arguments  :  none
*
* Returns    :  none
*********************************************************************************************************
*/

static  void  AppObjCreate (void)
{
	OS_ERR  err;
	
	OSSemCreate((OS_SEM*)&led_sem,
							(CPU_CHAR*)"LED SEM",
							(OS_SEM_CTR)0,
							(OS_ERR*)&err);
}

/*
*********************************************************************************************************
*                                         FUNCTION DEFINITION
*********************************************************************************************************
*/

void led0_task(void *pdata)
{
	OS_ERR err;
	
	while(1)
	{		
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
		OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
		
		OSSemPost(&led_sem,OS_OPT_POST_1,&err);
	};
}

void led1_task(void *pdata)
{
	OS_ERR err;
	
	while(1)
	{
		OSSemPend(&led_sem,0,OS_OPT_PEND_BLOCKING,(CPU_TS *)0,&err);
		
		GPIO_ResetBits(GPIOE,GPIO_Pin_5);
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);
		GPIO_SetBits(GPIOE,GPIO_Pin_5);
		OSTimeDly(500,OS_OPT_TIME_DLY,&err);
	};
}

void LED_Init(void)
{
 
	GPIO_InitTypeDef  GPIO_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	    //??PB,PE????

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;			    //LED0-->PB.5????
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	 //????
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //IO????50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);			     //???GPIOB.5
	GPIO_SetBits(GPIOB,GPIO_Pin_5);					//PB.5?????
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	            //LED1-->PE.5????
	GPIO_Init(GPIOE, &GPIO_InitStructure);	  	       //???GPIOE.5
	GPIO_SetBits(GPIOE,GPIO_Pin_5); 			 //PE.5?????
}
