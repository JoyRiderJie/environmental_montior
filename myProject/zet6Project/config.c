#include "config.h"

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

	// �ϵ���ʱ
	DWT_Delay_us(100000);
	printf("all hardwares finished initialization\r\n");
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

