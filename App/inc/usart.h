#ifndef _USART_H
#define _USART_H

#include "stm32f10x.h"  
#include "stdio.h"
#include "stdarg.h"	 
#include "string.h"
#include "lora_cfg.h"

/*********************************     LORA ����3���      **************************/
#define USART_REC_LEN 10
#define USART3_MAX_RECV_LEN		1024			    //�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		600					//����ͻ����ֽ���

extern u8 Lora_mode;
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern vu16 USART3_RX_STA;   						//��������״̬
extern u8 USART_RX_BUF[USART_REC_LEN];


/*********************************     ESP8266 ����4���      **************************/
#define UART4_REC_LEN					255	//�����������ֽ���  ��ʼ��255
#define EN_UART4_RX					1	//���ڽ���ʹ�ܡ�
											//1���������
											//0����ֹ����

/******************************************************************************
                              �ⲿ���ù��ܺ���
******************************************************************************/
extern uint8_t  UART4_RX_BUF[UART4_REC_LEN];	//���ջ��壬���UART4_REC_LEN���ֽڣ�ĩ�ֽ�Ϊ���з� 
extern uint16_t UART4_RX_COUNT;				//UART4���յ����ݸ���
extern uint8_t F_UART4_RX_FINISH;				//����״̬���,1:UART4������ɣ�0��UART4����δ���
extern uint8_t F_UART4_RX_RECEIVING;			//���ڽ������ڽ����б��,1:�����У�0����ͣ�����
extern uint8_t UART4_RX_TIMEOUT_COUNT;			//���ڽ��ճ�ʱ���������ڳ���3msδ���յ����ݣ���Ϊ��ʱ����λ��ms

/***************   ͨ�ýṹ��  **********************************************/
struct uartParam{
	// ��������
	USART_TypeDef* usartName;
	// ʱ�ӷ���2
	uint32_t RCC_APB2Periph;
	// ʱ�ӷ���1
	uint32_t RCC_APB1Periph;
	// GPIO����
	GPIO_TypeDef* GPIOx;
	// ��������
	uint16_t RxGPIOPin;
	// ��������
	uint16_t TxGPIOPin;
	// ����������Ϣ�ṹ��
	USART_InitTypeDef USART_InitStructure;
};

/******************************
* �������ܣ����ڳ�ʼ��
* ����������
*			struct uartParam*usart:������Ϣ�ṹ�� �ں��������ŷ��鼰���Ų����ʵȱ�Ҫ��Ϣ
*			NVIC_InitTypeDef*NVIC_InitStructure��NIVC��Ϣ�ṹ�� �ں�NVICͨ������ռ���ȼ��ȱ�Ҫ��Ϣ
* ��������ֵ����
*******************************/
void usartInit(struct uartParam*usart,NVIC_InitTypeDef*NVIC_InitStructure);

/***********************************************
* �������ܣ����ڲ����ʺ�У��λ����
* ����������
*			struct uartParam*usart��������Ϣ�ṹ��
*			bps:�����ʣ�1200~115200��
*			parity:У��λ���ޡ�ż���棩
* ��������ֵ����
***********************************************/
void setUsartParam(struct uartParam*usart,u8 bps,u8 parity);

/**********************************************************
* �������ܣ����ô����շ�ģʽ
* ����������
*			struct uartParam*usart�����봮�ڽṹ��
*			u8 state�����봮��ģʽ 0-�շ�ģʽ 1-ֻ����ģʽ 2-ֻ����ģʽ
* ������������
**********************************************************/
void setUsartMod(struct uartParam*usart,u8 mod);

void u3_printf(char* fmt,...);

void UART4_SendData(uint16_t Data);
void UART4_SendStr(uint8_t *Data);
void UART4_SendStrlen(uint8_t *Data, uint8_t len);



#endif

