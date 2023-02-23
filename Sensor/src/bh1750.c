#if 1

#include "bh1750.h"
#include "sys.h"
 
/*
	Ӧ��˵����
	�ڷ���I2C�豸ǰ�����ȵ��� i2c_CheckDevice() ���I2C�豸�Ƿ��������ú���������GPIO
*/
 
 
static void I2C_BH1750_GPIOConfig(void);
 
#define i2c_Delay() DWT_Delay_us(1)
/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C���������ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
	BH1750_I2C_SDA_1();
	BH1750_I2C_SCL_1();
	i2c_Delay();
	BH1750_I2C_SDA_0();
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();
}
/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
	BH1750_I2C_SDA_0();
	BH1750_I2C_SCL_1();
	i2c_Delay();
	BH1750_I2C_SDA_1();
}
/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;
 
	/* �ȷ����ֽڵĸ�λbit7 */
	for (i = 0; i < 8; i++)
	{		
		if (_ucByte & 0x80)
		{
			BH1750_I2C_SDA_1();
		}
		else
		{
			BH1750_I2C_SDA_0();
		}
		i2c_Delay();
		BH1750_I2C_SCL_1();
		i2c_Delay();	
		BH1750_I2C_SCL_0();
		if (i == 7)
		{
			 BH1750_I2C_SDA_1(); // �ͷ�����
		}
		_ucByte <<= 1;	/* ����һ��bit */
		i2c_Delay();
	}
}
/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;
	/* ������1��bitΪ���ݵ�bit7 */
	value = 0;
	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		BH1750_I2C_SCL_1();
		i2c_Delay();
		if (BH1750_I2C_SDA_READ())
		{
			value++;
		}
		BH1750_I2C_SCL_0();
		i2c_Delay();
	}
	return value;
}
/*
*********************************************************************************************************
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;
 
	BH1750_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	i2c_Delay();
	BH1750_I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
	i2c_Delay();
	if (BH1750_I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
		re = 1;
	else
		re = 0;
	BH1750_I2C_SCL_0();
	i2c_Delay();
	return re;
}
/*
*********************************************************************************************************
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	BH1750_I2C_SDA_0();	/* CPU����SDA = 0 */
	i2c_Delay();
	BH1750_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();
	BH1750_I2C_SDA_1();	/* CPU�ͷ�SDA���� */
}
/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	BH1750_I2C_SDA_1();	/* CPU����SDA = 1 */
	i2c_Delay();
	BH1750_I2C_SCL_1();	/* CPU����1��ʱ�� */
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();	
}

/*
*********************************************************************************************************
*	�� �� ��: I2C_BH1750_GPIOConfig
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void I2C_BH1750_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(BH1750_RCC_I2C_PORT, ENABLE);	/* ��GPIOʱ�� */
	GPIO_InitStructure.GPIO_Pin = BH1750_I2C_SCL_PIN | BH1750_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* ��©��� */
	GPIO_Init(BH1750_GPIO_PORT_I2C, &GPIO_InitStructure);
	/* ��һ��ֹͣ�ź�, ��λI2C�����ϵ������豸������ģʽ */
	i2c_Stop();
}
/*
*********************************************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
	i2c_Start();		/* ���������ź� */
	/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
	i2c_SendByte(_Address | BH1750_I2C_WR);
	ucAck = i2c_WaitAck();	/* ����豸��ACKӦ�� */
	i2c_Stop();			/* ����ֹͣ�ź� */
	return ucAck;
}
//BH1750дһ���ֽ�
//����ֵ	�ɹ���0		ʧ�ܣ���0 
uint8_t BH1750_Byte_Write(uint8_t data)
{
	i2c_Start();
	//����д��ַ
	i2c_SendByte(BH1750_Addr|0);
	if(i2c_WaitAck()==1)
		return 1;
	//���Ϳ�������
	i2c_SendByte(data);
	if(i2c_WaitAck()==1)
		return 2;
	i2c_Stop();
	return 0;
}
//BH1750��ȡ��������
//����ֵ �ɹ������ع���ǿ�� 	ʧ�ܣ�����0
uint16_t BH1750_Read_Measure(void)
{
	uint16_t receive_data=0; 
	i2c_Start();
	//���Ͷ���ַ
	i2c_SendByte(BH1750_Addr|1);
	if(i2c_WaitAck()==1)
		return 0;
	//��ȡ�߰�λ
	receive_data=i2c_ReadByte();
	i2c_Ack();
	//��ȡ�Ͱ�λ
	receive_data=(receive_data<<8)+i2c_ReadByte();
	i2c_NAck();
	i2c_Stop();
	return receive_data;	//���ض�ȡ��������
}
 
//BH1750s�ϵ�
void BH1750_Power_ON(void)
{
	BH1750_Byte_Write(POWER_ON);
}
//BH1750s�ϵ�
void BH1750_Power_OFF(void)
{
	BH1750_Byte_Write(POWER_OFF);
}
//BH1750��λ	�����ϵ�ʱ��Ч
void BH1750_RESET(void)
{
	BH1750_Byte_Write(MODULE_RESET);
}
 
//BH1750��ʼ��
uint8_t BH1750_Init(void)
{
	I2C_BH1750_GPIOConfig();		/* ����GPIO */
	
	BH1750_Power_ON();	//BH1750s�ϵ�
	//BH1750_RESET();			//BH1750��λ
	return BH1750_Byte_Write(Measure_Mode);
	//SysTick_Delay_ms(120);
}
//��ȡ����ǿ��
int LIght_Intensity(void)
{
	float v;
	v = (float)(BH1750_Read_Measure()/1.1f*Resolurtion);
	return (int) v;
}
#endif


#if 0
#include "bh1750.h"

#define delay_ms(ms) DWT_Delay_us(ms*100)

static void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;
	//��ʼ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	//����IO�ڹ���
	GPIO_InitStructure.GPIO_Pin = BH1750_I2C_SCL_PIN | BH1750_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BH1750_GPIO_PORT_I2C,&GPIO_InitStructure);
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//IICģʽ
	I2C_InitStructure.I2C_OwnAddress1 = 0x0A;//��Ϊ�ӻ���ַ
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//��ַ��λ��
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//ѡ����Ӧ���ź�
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;//���øߵ͵�ƽ����
	I2C_InitStructure.I2C_ClockSpeed = 200000;//����ʱ��
	I2C_Init(I2C1,&I2C_InitStructure);
	
	I2C_Cmd(I2C1,ENABLE);
//	I2C_DeInit(I2C1);
//	I2C_AcknowledgeConfig(I2C2,ENABLE);	
}

typedef   unsigned char BYTE;
typedef   unsigned short WORD;
#define SlaveAddress BH1750_Addr
 
static void Single_Write_BH1750(u8 REG_Address)
{
	printf("oz1\r\n");
    while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	printf("oz2\r\n");
	//��ʼ�ź�
	I2C_GenerateSTART(I2C1,ENABLE);
	printf("oz3\r\n");
    while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	printf("oz4\r\n");
	
	//�����豸��ַ+д�ź�
	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Transmitter);
	printf("oz5\r\n");
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
	printf("oz6\r\n");
	
	//�ڲ��Ĵ�����ַ
	I2C_SendData(I2C1,REG_Address);
	printf("oz7\r\n");
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);
	printf("oz8\r\n");
	//����ֹͣ�ź�
	I2C_GenerateSTOP(I2C1,ENABLE);               
}
 
//*********************************************************
//
//��������BH1750�ڲ�����
//
//*********************************************************
BYTE    BUF[8];                         //�������ݻ�����   
int     dis_data;                       //����   	
static void Multiple_read_BH1750(void)
{   
	u8 i;
	printf("sw1\r\n");
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	printf("sw2\r\n");
	//��ʼ�ź�
	I2C_GenerateSTART(I2C1,ENABLE);
	printf("sw3\r\n");
    while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	printf("sw4\r\n");  
	//�����豸��ַ+���ź�
	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Receiver);
	printf("sw5\r\n");
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);
	printf("sw6\r\n");
   
	for (i=0; i<3-1; i++)                      //������ȡ2����ַ���ݣ��洢��BUF
    {
		while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
		BUF[i] = I2C_ReceiveData(I2C1);
    }
	printf("sw7\r\n");
    I2C_AcknowledgeConfig(I2C1,DISABLE);
	printf("sw8\r\n");
    while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_RECEIVED) != SUCCESS);
	printf("sw9\r\n");
	BUF[i] = I2C_ReceiveData(I2C1);
	printf("sw10\r\n");
	
	 //ֹͣ�ź�
	I2C_GenerateSTOP(I2C1,ENABLE);
	printf("sw11\r\n");
	
	I2C_AcknowledgeConfig(I2C1,ENABLE);  
	printf("sw12\r\n");                      
    delay_ms(5);
	printf("get data finish\r\n");
}
 
//��ʼ��BH1750��������Ҫ��ο�pdf�����޸�****
uint8_t BH1750_Init(void)
{
	printf("step1\r\n");
	IIC_Init();
	printf("step2\r\n");
	Single_Write_BH1750(0x01); // powerON
	printf("step3\r\n");
	Single_Write_BH1750(0x10);   // H- resolution mode
	printf("step4\r\n");
	delay_ms(180);
	printf("step5\r\n");
	
	return 0;
}
 
int LIght_Intensity(void)
{
	printf("handle0\r\n");
	Multiple_read_BH1750();		 //�����������ݣ��洢��BUF��
	printf("handle1\r\n");
	dis_data=BUF[0];
	dis_data=(dis_data<<8)+BUF[1];//�ϳ����ݣ�����������
	printf("handle2\r\n");
	return (int)dis_data/1.2;
	
}
#endif



