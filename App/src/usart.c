#include "usart.h"

//���ڽ��ջ�����
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�

/******************************
* �������ܣ����ڳ�ʼ��
* ����������
*			struct uartParam*usart:������Ϣ�ṹ�� �ں��������ŷ��鼰���Ų����ʵȱ�Ҫ��Ϣ
*			NVIC_InitTypeDef*NVIC_InitStructure��NIVC��Ϣ�ṹ�� �ں�NVICͨ������ռ���ȼ��ȱ�Ҫ��Ϣ
* ��������ֵ����
*******************************/
void usartInit(struct uartParam*usart,NVIC_InitTypeDef*NVIC_InitStructure)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // ����ʱ�ӳ�ʼ��
    if(usart->RCC_APB1Periph)
        RCC_APB1PeriphClockCmd(usart->RCC_APB1Periph,ENABLE);
    if(usart->RCC_APB2Periph)
        RCC_APB2PeriphClockCmd(usart->RCC_APB2Periph,ENABLE);

    // �������ų�ʼ��
    GPIO_InitStructure.GPIO_Pin = usart->TxGPIOPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(usart->GPIOx,&GPIO_InitStructure);
    // �������ų�ʼ��
    GPIO_InitStructure.GPIO_Pin = usart->RxGPIOPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(usart->GPIOx,&GPIO_InitStructure);

    //���ڳ�ʼ��
    USART_Init(usart->usartName, &usart->USART_InitStructure);
    //ʹ�ܴ���
    USART_Cmd(usart->usartName, ENABLE);
    //�����ж�
    USART_ITConfig(usart->usartName, USART_IT_RXNE, ENABLE);

    // NVIC��ʼ��
    NVIC_Init(NVIC_InitStructure);
}


/***********************************************
* �������ܣ����ڲ����ʺ�У��λ����
* ����������
*			struct uartParam*usart��������Ϣ�ṹ��
*			bps:�����ʣ�1200~115200��
*			parity:У��λ���ޡ�ż���棩
* ��������ֵ����
***********************************************/
void setUsartParam(struct uartParam*usart,u8 bps,u8 parity)
{
    static u32 bound=0;

    switch(bps)
    {
    case LORA_TTLBPS_1200:
        bound=1200;
        break;
    case LORA_TTLBPS_2400:
        bound=2400;
        break;
    case LORA_TTLBPS_4800:
        bound=4800;
        break;
    case LORA_TTLBPS_9600:
        bound=9600;
        break;
    case LORA_TTLBPS_19200:
        bound=19200;
        break;
    case LORA_TTLBPS_38400:
        bound=38400;
        break;
    case LORA_TTLBPS_57600:
        bound=57600;
        break;
    case LORA_TTLBPS_115200:
        bound=115200;
        break;
    }

    USART_Cmd(USART3, DISABLE); //�رմ���
    usart->USART_InitStructure.USART_BaudRate = bound;
    usart->USART_InitStructure.USART_StopBits = USART_StopBits_1;

    if(parity==LORA_TTLPAR_8N1)//��У��
    {
        usart->USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        usart->USART_InitStructure.USART_Parity = USART_Parity_No;
    } else if(parity==LORA_TTLPAR_8E1)//żУ��
    {
        usart->USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        usart->USART_InitStructure.USART_Parity = USART_Parity_Even;
    } else if(parity==LORA_TTLPAR_8O1)//��У��
    {
        usart->USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        usart->USART_InitStructure.USART_Parity = USART_Parity_Odd;
    }
    USART_Init(USART3, &usart->USART_InitStructure); //��ʼ������3
    USART_Cmd(USART3, ENABLE); //ʹ�ܴ���
}

/**********************************************************
* �������ܣ����ô����շ�ģʽ
* ����������
*			struct uartParam*usart�����봮�ڽṹ��
*			u8 state�����봮��ģʽ 0-�շ�ģʽ 1-ֻ����ģʽ 2-ֻ����ģʽ
* ������������
**********************************************************/
void setUsartMod(struct uartParam*usart,u8 mod)
{
    USART_Cmd(USART3, DISABLE); //ʧ�ܴ���

    if(mod == 0)
        usart->USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ
    else if(mod == 1)
        usart->USART_InitStructure.USART_Mode = USART_Mode_Tx;//ֻ����
    else if(mod == 2)
        usart->USART_InitStructure.USART_Mode = USART_Mode_Rx;//ֻ����

    USART_Init(usart->usartName, &usart->USART_InitStructure); //��ʼ������3
    USART_Cmd(usart->usartName, ENABLE); //ʹ�ܴ���
}

/******************************************** USART1��� ********************************************/

/*  ʹ��printf�������(���ڷ���)  */
struct __FILE
{
    int handle;

};
FILE __stdout;

//�ض���fputc����  ����printf����1���
int fputc(int ch, FILE *f)
{
    //ѭ������,ֱ���������
    while((USART1->SR&0X40)==0)
        ;
    USART1->DR = (u8) ch;
    return ch;
}

/*  ���ڽ������(�жϷ�����) */
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//��¼����״̬ bit15-������ɱ�־  bit14-���յ�0x0d  bit13~0-���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;
void USART1_IRQHandler(void)
{
    u8 Res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
    {
        Res =USART_ReceiveData(USART1);	//��ȡ���յ�������

        if((USART_RX_STA&0x8000)==0)//����δ���
        {
            if(USART_RX_STA&0x4000)//���յ���0x0d
            {
                if(Res!=0x0a)
                    USART_RX_STA=0;//���մ���,���¿�ʼ
                else
                    USART_RX_STA|=0x8000;	//���������
            }
            else //��û�յ�0X0D
            {
                if(Res==0x0d)
                    USART_RX_STA|=0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
                    USART_RX_STA++;
                    if(USART_RX_STA > (USART_REC_LEN-1))
                        USART_RX_STA=0;//�������ݴ���,���¿�ʼ����
                }
            }
        }
    }
}


/******************************************** USART3��� ********************************************/

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������timer,����Ϊ����1����������.Ҳ���ǳ���timerû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART3_RX_STA=0;

void USART3_IRQHandler(void)
{
    u8 res;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
    {
        res =USART_ReceiveData(USART3);
        if((USART3_RX_STA&(1<<15))==0)              //�������һ������,��û�б�����,���ٽ�����������
        {
            if(USART3_RX_STA<USART3_MAX_RECV_LEN)	//�����Խ�������
            {
                if(!Lora_mode)//���ù�����(������ʱ����ʱ)
                {
                    TIM_SetCounter(TIM3,0);             //���������
                    if(USART3_RX_STA==0) 				//ʹ�ܶ�ʱ��7���ж�
                    {
                        TIM_Cmd(TIM3,ENABLE);           //ʹ�ܶ�ʱ��3
                    }
                }
                USART3_RX_BUF[USART3_RX_STA++]=res;	//��¼���յ���ֵ
            } else
            {
                USART3_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
            }
        }
    }
}


//����3,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u3_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART3_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART3_TX_BUF);		//�˴η������ݵĳ���
    for(j=0; j<i; j++)							//ѭ����������
    {
        while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������
        USART_SendData(USART3,USART3_TX_BUF[j]);
    }
}

/***************** ����4���   *******************************************/
/******************************************************************************
* Function Name --> ����4����һ���ֽ�����
* Description   --> nonevoid USART1_SendData(uint16_t Data)
* Input         --> bound��������	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_SendData(uint16_t Data)
{
	while((UART4->SR & 0x40) == 0);	//�ȴ��������
	USART_SendData(UART4, Data);
}

/******************************************************************************
* Function Name --> ����4����һ������
* Description   --> none
* Input         --> *Data:�ַ���	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_SendStr(uint8_t *Data)
{
	while(*Data!='\0')
	{
	    UART4_SendData((uint16_t )(*Data));
		Data++;
	}
}

/******************************************************************************
* Function Name --> ����4����һ�����ݣ�ָ�����ݳ���
* Description   --> none
* Input         --> *Data:�ַ�����len����	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_SendStrlen(uint8_t *Data, uint8_t len)
{
	while(len--)
	{
		UART4_SendData((uint16_t )(*Data));
		Data++;
	}
}

//=========================================================
 
//=========================================================

//����4�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ��� 

uint8_t UART4_RX_BUF[UART4_REC_LEN];	//���ջ���,���UART4_REC_LEN���ֽ�

//����״̬
uint16_t UART4_RX_COUNT=0;	//���յ�����Ч�ֽ���Ŀ�����USART1_REC_LEN�ֽ�
uint8_t F_UART4_RX_FINISH=0;//����״̬���,1:UART4������ɣ�0��UART4����δ���
uint8_t F_UART4_RX_RECEIVING=0;//���ڽ������ڽ����б��,1:�����У�0����ͣ�����
uint8_t UART4_RX_TIMEOUT_COUNT=0;	//���ڽ��ճ�ʱ���������ڳ���3msδ���յ����ݣ���Ϊ��ʱ����λ��ms

/******************************************************************************
* Function Name --> ����4�����жϷ������
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //���յ�����
	{
		F_UART4_RX_RECEIVING=1;//���ڽ��մ�������
		UART4_RX_TIMEOUT_COUNT=0;//��ʱ��������
		UART4_RX_BUF[UART4_RX_COUNT] = USART_ReceiveData(UART4);	//��ȡ���յ�������
		UART4_RX_COUNT++;
		if(UART4_RX_COUNT>UART4_REC_LEN)//���������
		{UART4_RX_COUNT = 0x0000;}	
		//end ����δ���   		 		 
	}	//end ���յ�����
}

