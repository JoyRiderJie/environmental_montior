#include "config.h"

#include "config.h"

/*  FreeRTOS函数返回值 */
BaseType_t res[10];

/*  任务控制句柄  */
// 0号是主任务的控制句柄
TaskHandle_t taskControl[10];
/*  事件控制句柄   */

/*  定时器句柄   */

/*  其他变量 */


/*   外部声明变量   */
extern NVIC_InitTypeDef usart3NVIC_InitStructure;
extern struct uartParam usart3Param;

/*****************************************
* 函数功能：STM32外设初始化
* 函数参数：无
* 函数返回值：无
*****************************************/
void hardwareInit(void)
{
#if USARTFUNCTION==1
	// 串口必要的参数
	struct uartParam usart1Param = {USART1,RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA,NULL,GPIOA,GPIO_Pin_10,GPIO_Pin_9,
	{115200,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Rx | USART_Mode_Tx,USART_HardwareFlowControl_None}};
	NVIC_InitTypeDef usart1NVIC_InitStructure = {USART1_IRQn,3,3,ENABLE};
#endif
	
	// 定时器2必要的参数
//	struct timeParam tim2 = {TIM2,RCC_APB1Periph_TIM2,{71,TIM_CKD_DIV1,2,TIM_CounterMode_Up}};
//	NVIC_InitTypeDef tim2NVIC_InitStructure = {TIM2_IRQn,1,1,ENABLE};
	
	// 设置中断向量NVIC的分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#if USARTFUNCTION==1
	// USART1初始化
	usartInit(&usart1Param,&usart1NVIC_InitStructure);
#endif
	// 定时器2初始化
//	timInit(&tim2,&tim2NVIC_InitStructure);
	// 延时函数初始化
//	delay_init();
	
	// 适合在FreeRTOS中使用的微秒级延时函数
	DWT_Dalay_Init();
	// LED初始化
	ledInit();
	// LORA 初始化
	loraInit();

	// 上电延时
	DWT_Delay_us(100000);
	printf("all hardwares finished initialization\r\n");
}

/*****************************************
* 函数功能：创建freertos的任务
* 函数参数：无
* 函数返回值：无
*****************************************/
void createAllTask(void)
{
	
	while(1)
	{
		// 入临界区
		taskENTER_CRITICAL();
		printf("start create main task\r\n");

		// 出临界区	
		taskEXIT_CRITICAL(); 
		vTaskDelete(NULL);	
		vTaskDelay(100);
	}
}

/*******************************************
* 函数功能：系统外设 储存初始化
* 函数参数：无
* 函数返回值：无
*******************************************/
void sysInit(void)
{
	// STM32外设初始化
	hardwareInit();
	
	// 创建一个FreeRTOS创建函数
	xTaskCreate((TaskFunction_t)createAllTask,(char*)"createTask",512,(void *)NULL,2,&taskControl[0]);
	
	vTaskStartScheduler();
}

/*******************************************
* 函数功能：系统工作 内部不含while函数
* 函数参数：无
* 函数返回值：无
*******************************************/
void sysWork(void)
{
		
}

/*****************************************
* 函数功能：freertos的空闲任务 内部最好不要有while(1)循环
* 函数参数：无
* 函数返回值：无
*****************************************/
void vApplicationIdleHook (void)
{
	
}  

