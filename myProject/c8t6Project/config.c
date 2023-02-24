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
#if DH11Task==1	
	// 温湿度传感器初始化
	if(0 == DHT11_Init())
		printf("DHT11 error\r\n");
	else
		printf("DH11 success\r\n");
#endif
#if BH1750Task==1	
	// BH1750光照传感器初始化 内含IIC初始化
	if(0 != BH1750_Init())
		printf("BH1750 error\r\n");
#endif
#if MQ2Task==1
	// 烟雾传感器初始化
	ADC_Pin_Init();
#endif
	// 上电延时
	DWT_Delay_us(100000);
	printf("all hardwares finished initialization\r\n");
}

/*****************************************
* 函数功能：LED任务
* 函数参数：无
* 函数返回值：无
*****************************************/
void ledTask(void)
{
		// 保存系统时间
	static portTickType myPreviousWakeTime;
	const volatile TickType_t xDelayms = pdMS_TO_TICKS( 789UL );
	
	while(1)
	{

		vTaskDelayUntil(&myPreviousWakeTime,xDelayms);
	}
}  

/*****************************************
* 函数功能：DHT11温湿度任务
* 函数参数：无
* 函数返回值：无
*****************************************/
void dht11Task(void)
{
	// 保存系统时间
	static portTickType myPreviousWakeTime;
	const volatile TickType_t xDelayms = pdMS_TO_TICKS( 789UL );
	char str[20] ;
	while(1)
	{
		u8 temp = 0,humi = 0;
		taskENTER_CRITICAL();
		DHT11_Read_Data(&temp,&humi);
		sprintf(str,"\"t1\":%d,\"h1\":%d",temp,humi);
		LoRa_SendData((char*)str);
		taskEXIT_CRITICAL(); 
		vTaskDelayUntil(&myPreviousWakeTime,xDelayms);
	}
}  


/*****************************************
* 函数功能：BH1750光照传感器任务
* 函数参数：无
* 函数返回值：无
*****************************************/
void bh1750Task(void)
{
	// 保存系统时间
	static portTickType myPreviousWakeTime;
	const volatile TickType_t xDelayms = pdMS_TO_TICKS( 784UL );
	char str[20] ;
	while(1)
	{
		int light;
		taskENTER_CRITICAL();
		light = LIght_Intensity();
		sprintf(str,"\"l1\":%d",light);
		LoRa_SendData((char*)str);
		taskEXIT_CRITICAL(); 
		vTaskDelayUntil(&myPreviousWakeTime,xDelayms);
	}
}  


/*****************************************
* 函数功能：风扇任务
* 函数参数：无
* 函数返回值：无
*****************************************/
void fanTask(void)
{
	// 保存系统时间
	static portTickType myPreviousWakeTime;
	const volatile TickType_t xDelayms = pdMS_TO_TICKS( 789UL );
	
	while(1)
	{

		vTaskDelayUntil(&myPreviousWakeTime,xDelayms);
	}
}  


/*****************************************
* 函数功能：LORA通信任务
* 函数参数：无
* 函数返回值：无
*****************************************/
void loraTask(void)
{
	// 保存系统时间
	static portTickType myPreviousWakeTime;
	const volatile TickType_t xDelayms = pdMS_TO_TICKS( 789UL );
	
	while(1)
	{
		u16 i=0;
		u16 len=0;
		//有数据来了
		if(USART3_RX_STA&0x8000)
		{
			len = USART3_RX_STA&0X7FFF;
			USART3_RX_BUF[len]=0;//添加结束符
			USART3_RX_STA=0;

			for(i=0;i<len;i++)
			{
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
				USART_SendData(USART1,USART3_RX_BUF[i]); 
			}
			if(LoRa_CFG.mode_sta==LORA_STA_Tran)//透明传输
			{	
				//ESP8266_IpSend((char *)USART3_RX_BUF,strlen((const char *)USART3_RX_BUF));
				lora_at_response(0);//显示接收到的数据
			}
			
			// 处理数据
			
			memset((char*)USART3_RX_BUF,0x00,len);//串口接收缓冲区清0
		}
		vTaskDelayUntil(&myPreviousWakeTime,xDelayms);
	}
}  

/*****************************************
* 函数功能：M2Q烟雾传感器任务
* 函数参数：无
* 函数返回值：无
*****************************************/
void M2QTask(void)
{
	// 保存系统时间
	static portTickType myPreviousWakeTime;
	const volatile TickType_t xDelayms = pdMS_TO_TICKS( 779UL );
	char str[20] ;
	while(1)
	{
		taskENTER_CRITICAL();
		u16 m = ADC_Trans();
		taskEXIT_CRITICAL(); 
		sprintf(str,"\"m1\":%d",m);
		LoRa_SendData((char*)str);
		vTaskDelayUntil(&myPreviousWakeTime,xDelayms);
	}
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
#if LEDTASK==1		
		xTaskCreate((TaskFunction_t)task,(char*)"led task",128,(void *)NULL,2,&taskControl[1]);
#endif	
#if DH11Task==1		
		xTaskCreate((TaskFunction_t)dht11Task,(char*)"dh11 task",128,(void *)NULL,2,&taskControl[2]);
#endif	
#if BH1750Task==1		
		xTaskCreate((TaskFunction_t)bh1750Task,(char*)"bht1750 task",128,(void *)NULL,2,&taskControl[3]);
#endif	
#if FANTask==1		
		xTaskCreate((TaskFunction_t)fanTask,(char*)"fan task",128,(void *)NULL,2,&taskControl[4]);
#endif	
#if LORATASK==1		
		xTaskCreate((TaskFunction_t)loraTask,(char*)"lora task",128,(void *)NULL,2,&taskControl[5]);
#endif	
#if MQ2Task==1		
		xTaskCreate((TaskFunction_t)M2QTask,(char*)"MQ2 task",128,(void *)NULL,2,&taskControl[6]);
#endif	
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



