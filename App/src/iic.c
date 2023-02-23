#include "iic.h"

//BH1750�ĵ�ַ
#define	BH1750_ADDR_WRITE	0x46	//01000110
#define	BH1750_ADDR_READ	0x47	//01000111
typedef enum
{
	POWER_OFF_CMD	=	0x00,	//�ϵ磺�޼���״̬
	POWER_ON_CMD	=	0x01,	//ͨ�磺�ȴ�����ָ��
	RESET_REGISTER	=	0x07,	//�������ּĴ������ڶϵ�״̬�²������ã�
	CONT_H_MODE		=	0x10,	//����H�ֱ���ģʽ����11x�ֱ����¿�ʼ����������ʱ��120ms
	CONT_H_MODE2	=	0x11,	//����H�ֱ���ģʽ2����0.51x�ֱ����¿�ʼ����������ʱ��120ms
	CONT_L_MODE		=	0x13,	//����L�ֱ���ģʽ����411�ֱ����¿�ʼ����������ʱ��16ms
	ONCE_H_MODE		=	0x20,	//һ�θ߷ֱ���ģʽ����11x�ֱ����¿�ʼ����������ʱ��120ms���������Զ�����Ϊ�ϵ�ģʽ
	ONCE_H_MODE2	=	0x21,	//һ�θ߷ֱ���ģʽ2����0.51x�ֱ����¿�ʼ����������ʱ��120ms���������Զ�����Ϊ�ϵ�ģʽ
	ONCE_L_MODE		=	0x23	//һ�εͷֱ���ģʽ����411x�ֱ����¿�ʼ����������ʱ��16ms���������Զ�����Ϊ�ϵ�ģʽ
} BH1750_MODE;



static void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;
	//��ʼ��ʱ��
	RCC_APB2PeriphClockCmd(BH1750_RCC_I2C_PORT,ENABLE);
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
//	I2C_AcknowledgeConfig(I2C2,ENABLE);	
}

/**
 * @brief	��BH1750����һ��ָ��
 * @param	cmd ���� BH1750����ģʽָ���BH1750_MODE��ö�ٶ��壩
 * @retval	�ɹ�����HAL_OK
*/
void	BH1750_Send_Cmd(BH1750_MODE cmd)
{
	I2C_Send7bitAddress(I2C1, BH1750_ADDR_WRITE, (uint8_t)cmd);
}

/**
 * @brief	��BH1750����һ�ι�ǿ����
 * @param	dat ���� �洢����ǿ�ȵĵ�ַ�������ֽ����飩
 * @retval	�ɹ� ���� ����HAL_OK
*/
uint8_t BH1750_Read_Dat(uint8_t* dat)
{
	return I2C_ReceiveData(I2C1);
}



void bh1750Init(void)
{
	IIC_Init();	
}

