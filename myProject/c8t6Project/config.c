#include "config.h"

/*  FreeRTOS��������ֵ */
BaseType_t res[10];

/*  ������ƾ��  */
// 0����������Ŀ��ƾ��
TaskHandle_t taskControl[10];
/*  �¼����ƾ��   */

/*  ��ʱ�����   */

/*  �������� */


/*   �ⲿ��������   */
extern NVIC_InitTypeDef usart3NVIC_InitStructure;
extern struct uartParam usart3Param;

/*****************************************
* �������ܣ�STM32�����ʼ��
* ������������
* ��������ֵ����
*****************************************/
void hardwareInit(void)
{
#if USARTFUNCTION==1
	// ���ڱ�Ҫ�Ĳ���
	struct uartParam usart1Param = {USART1,RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA,NULL,GPIOA,GPIO_Pin_10,GPIO_Pin_9,
	{115200,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Rx | USART_Mode_Tx,USART_HardwareFlowControl_None}};
	NVIC_InitTypeDef usart1NVIC_InitStructure = {USART1_IRQn,3,3,ENABLE};
#endif
	
	// ��ʱ��2��Ҫ�Ĳ���
//	struct timeParam tim2 = {TIM2,RCC_APB1Periph_TIM2,{71,TIM_CKD_DIV1,2,TIM_CounterMode_Up}};
//	NVIC_InitTypeDef tim2NVIC_InitStructure = {TIM2_IRQn,1,1,ENABLE};
	
	// �����ж�����NVIC�ķ���
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
#if USARTFUNCTION==1
	// USART1��ʼ��
	usartInit(&usart1Param,&usart1NVIC_InitStructure);
#endif
	// ��ʱ��2��ʼ��
//	timInit(&tim2,&tim2NVIC_InitStructure);
	// ��ʱ������ʼ��
//	delay_init();
	
	// �ʺ���FreeRTOS��ʹ�õ�΢�뼶��ʱ����
	DWT_Dalay_Init();
	// LED��ʼ��
	ledInit();
	// LORA ��ʼ��
	loraInit();
#if DH11Task==1	
	// ��ʪ�ȴ�������ʼ��
	if(0 == DHT11_Init())
		printf("DHT11 error\r\n");
	else
		printf("DH11 success\r\n");
#endif
#if BH1750Task==1	
	// BH1750���մ�������ʼ�� �ں�IIC��ʼ��
	if(0 != BH1750_Init())
		printf("BH1750 error\r\n");
#endif
#if MQ2Task==1
	// ����������ʼ��
	ADC_Pin_Init();
#endif
	// �ϵ���ʱ
	DWT_Delay_us(100000);
	printf("all hardwares finished initialization\r\n");
}

/*****************************************
* �������ܣ�LED����
* ������������
* ��������ֵ����
*****************************************/
void ledTask(void)
{
		// ����ϵͳʱ��
	static portTickType myPreviousWakeTime;
	const volatile TickType_t xDelayms = pdMS_TO_TICKS( 789UL );
	
	while(1)
	{

		vTaskDelayUntil(&myPreviousWakeTime,xDelayms);
	}
}  

/*****************************************
* �������ܣ�DHT11��ʪ������
* ������������
* ��������ֵ����
*****************************************/
void dht11Task(void)
{
	// ����ϵͳʱ��
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
* �������ܣ�BH1750���մ���������
* ������������
* ��������ֵ����
*****************************************/
void bh1750Task(void)
{
	// ����ϵͳʱ��
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
* �������ܣ���������
* ������������
* ��������ֵ����
*****************************************/
void fanTask(void)
{
	// ����ϵͳʱ��
	static portTickType myPreviousWakeTime;
	const volatile TickType_t xDelayms = pdMS_TO_TICKS( 789UL );
	
	while(1)
	{

		vTaskDelayUntil(&myPreviousWakeTime,xDelayms);
	}
}  


/*****************************************
* �������ܣ�LORAͨ������
* ������������
* ��������ֵ����
*****************************************/
void loraTask(void)
{
	// ����ϵͳʱ��
	static portTickType myPreviousWakeTime;
	const volatile TickType_t xDelayms = pdMS_TO_TICKS( 789UL );
	
	while(1)
	{
		u16 i=0;
		u16 len=0;
		//����������
		if(USART3_RX_STA&0x8000)
		{
			len = USART3_RX_STA&0X7FFF;
			USART3_RX_BUF[len]=0;//��ӽ�����
			USART3_RX_STA=0;

			for(i=0;i<len;i++)
			{
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
				USART_SendData(USART1,USART3_RX_BUF[i]); 
			}
			if(LoRa_CFG.mode_sta==LORA_STA_Tran)//͸������
			{	
				//ESP8266_IpSend((char *)USART3_RX_BUF,strlen((const char *)USART3_RX_BUF));
				lora_at_response(0);//��ʾ���յ�������
			}
			
			// ��������
			
			memset((char*)USART3_RX_BUF,0x00,len);//���ڽ��ջ�������0
		}
		vTaskDelayUntil(&myPreviousWakeTime,xDelayms);
	}
}  

/*****************************************
* �������ܣ�M2Q������������
* ������������
* ��������ֵ����
*****************************************/
void M2QTask(void)
{
	// ����ϵͳʱ��
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
* �������ܣ�����freertos������
* ������������
* ��������ֵ����
*****************************************/
void createAllTask(void)
{
	
	while(1)
	{
		// ���ٽ���
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
		// ���ٽ���	
		taskEXIT_CRITICAL(); 
		vTaskDelete(NULL);	
		vTaskDelay(100);
	}
}

/*******************************************
* �������ܣ�ϵͳ���� �����ʼ��
* ������������
* ��������ֵ����
*******************************************/
void sysInit(void)
{
	// STM32�����ʼ��
	hardwareInit();
	
	// ����һ��FreeRTOS��������
	xTaskCreate((TaskFunction_t)createAllTask,(char*)"createTask",512,(void *)NULL,2,&taskControl[0]);
	
	vTaskStartScheduler();
}

/*******************************************
* �������ܣ�ϵͳ���� �ڲ�����while����
* ������������
* ��������ֵ����
*******************************************/
void sysWork(void)
{
		
}

/*****************************************
* �������ܣ�freertos�Ŀ������� �ڲ���ò�Ҫ��while(1)ѭ��
* ������������
* ��������ֵ����
*****************************************/
void vApplicationIdleHook (void)
{
	
}  



