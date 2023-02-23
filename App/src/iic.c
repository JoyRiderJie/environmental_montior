#include "iic.h"

//BH1750的地址
#define	BH1750_ADDR_WRITE	0x46	//01000110
#define	BH1750_ADDR_READ	0x47	//01000111
typedef enum
{
	POWER_OFF_CMD	=	0x00,	//断电：无激活状态
	POWER_ON_CMD	=	0x01,	//通电：等待测量指令
	RESET_REGISTER	=	0x07,	//重置数字寄存器（在断电状态下不起作用）
	CONT_H_MODE		=	0x10,	//连续H分辨率模式：在11x分辨率下开始测量，测量时间120ms
	CONT_H_MODE2	=	0x11,	//连续H分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms
	CONT_L_MODE		=	0x13,	//连续L分辨率模式：在411分辨率下开始测量，测量时间16ms
	ONCE_H_MODE		=	0x20,	//一次高分辨率模式：在11x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
	ONCE_H_MODE2	=	0x21,	//一次高分辨率模式2：在0.51x分辨率下开始测量，测量时间120ms，测量后自动设置为断电模式
	ONCE_L_MODE		=	0x23	//一次低分辨率模式：在411x分辨率下开始测量，测量时间16ms，测量后自动设置为断电模式
} BH1750_MODE;



static void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef  I2C_InitStructure;
	//初始化时钟
	RCC_APB2PeriphClockCmd(BH1750_RCC_I2C_PORT,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	//配置IO口功能
	GPIO_InitStructure.GPIO_Pin = BH1750_I2C_SCL_PIN | BH1750_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BH1750_GPIO_PORT_I2C,&GPIO_InitStructure);
	
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;//IIC模式
	I2C_InitStructure.I2C_OwnAddress1 = 0x0A;//作为从机地址
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;//地址的位数
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;//选择有应答信号
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_16_9;//配置高低电平比例
	I2C_InitStructure.I2C_ClockSpeed = 200000;//配置时钟
	I2C_Init(I2C1,&I2C_InitStructure);
	
	I2C_Cmd(I2C1,ENABLE);
//	I2C_AcknowledgeConfig(I2C2,ENABLE);	
}

/**
 * @brief	向BH1750发送一条指令
 * @param	cmd —— BH1750工作模式指令（在BH1750_MODE中枚举定义）
 * @retval	成功返回HAL_OK
*/
void	BH1750_Send_Cmd(BH1750_MODE cmd)
{
	I2C_Send7bitAddress(I2C1, BH1750_ADDR_WRITE, (uint8_t)cmd);
}

/**
 * @brief	从BH1750接收一次光强数据
 * @param	dat —— 存储光照强度的地址（两个字节数组）
 * @retval	成功 —— 返回HAL_OK
*/
uint8_t BH1750_Read_Dat(uint8_t* dat)
{
	return I2C_ReceiveData(I2C1);
}



void bh1750Init(void)
{
	IIC_Init();	
}

