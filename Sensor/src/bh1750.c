#if 1

#include "bh1750.h"
#include "sys.h"
 
/*
	应用说明：
	在访问I2C设备前，请先调用 i2c_CheckDevice() 检测I2C设备是否正常，该函数会配置GPIO
*/
 
 
static void I2C_BH1750_GPIOConfig(void);
 
#define i2c_Delay() DWT_Delay_us(1)
/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
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
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线停止信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	BH1750_I2C_SDA_0();
	BH1750_I2C_SCL_1();
	i2c_Delay();
	BH1750_I2C_SDA_1();
}
/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参：_ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;
 
	/* 先发送字节的高位bit7 */
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
			 BH1750_I2C_SDA_1(); // 释放总线
		}
		_ucByte <<= 1;	/* 左移一个bit */
		i2c_Delay();
	}
}
/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
	uint8_t i;
	uint8_t value;
	/* 读到第1个bit为数据的bit7 */
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
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参：无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t re;
 
	BH1750_I2C_SDA_1();	/* CPU释放SDA总线 */
	i2c_Delay();
	BH1750_I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
	i2c_Delay();
	if (BH1750_I2C_SDA_READ())	/* CPU读取SDA口线状态 */
		re = 1;
	else
		re = 0;
	BH1750_I2C_SCL_0();
	i2c_Delay();
	return re;
}
/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	BH1750_I2C_SDA_0();	/* CPU驱动SDA = 0 */
	i2c_Delay();
	BH1750_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();
	BH1750_I2C_SDA_1();	/* CPU释放SDA总线 */
}
/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{
	BH1750_I2C_SDA_1();	/* CPU驱动SDA = 1 */
	i2c_Delay();
	BH1750_I2C_SCL_1();	/* CPU产生1个时钟 */
	i2c_Delay();
	BH1750_I2C_SCL_0();
	i2c_Delay();	
}

/*
*********************************************************************************************************
*	函 数 名: I2C_BH1750_GPIOConfig
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void I2C_BH1750_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(BH1750_RCC_I2C_PORT, ENABLE);	/* 打开GPIO时钟 */
	GPIO_InitStructure.GPIO_Pin = BH1750_I2C_SCL_PIN | BH1750_I2C_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	/* 开漏输出 */
	GPIO_Init(BH1750_GPIO_PORT_I2C, &GPIO_InitStructure);
	/* 给一个停止信号, 复位I2C总线上的所有设备到待机模式 */
	i2c_Stop();
}
/*
*********************************************************************************************************
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参：_Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
	i2c_Start();		/* 发送启动信号 */
	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(_Address | BH1750_I2C_WR);
	ucAck = i2c_WaitAck();	/* 检测设备的ACK应答 */
	i2c_Stop();			/* 发送停止信号 */
	return ucAck;
}
//BH1750写一个字节
//返回值	成功：0		失败：非0 
uint8_t BH1750_Byte_Write(uint8_t data)
{
	i2c_Start();
	//发送写地址
	i2c_SendByte(BH1750_Addr|0);
	if(i2c_WaitAck()==1)
		return 1;
	//发送控制命令
	i2c_SendByte(data);
	if(i2c_WaitAck()==1)
		return 2;
	i2c_Stop();
	return 0;
}
//BH1750读取测量数据
//返回值 成功：返回光照强度 	失败：返回0
uint16_t BH1750_Read_Measure(void)
{
	uint16_t receive_data=0; 
	i2c_Start();
	//发送读地址
	i2c_SendByte(BH1750_Addr|1);
	if(i2c_WaitAck()==1)
		return 0;
	//读取高八位
	receive_data=i2c_ReadByte();
	i2c_Ack();
	//读取低八位
	receive_data=(receive_data<<8)+i2c_ReadByte();
	i2c_NAck();
	i2c_Stop();
	return receive_data;	//返回读取到的数据
}
 
//BH1750s上电
void BH1750_Power_ON(void)
{
	BH1750_Byte_Write(POWER_ON);
}
//BH1750s断电
void BH1750_Power_OFF(void)
{
	BH1750_Byte_Write(POWER_OFF);
}
//BH1750复位	仅在上电时有效
void BH1750_RESET(void)
{
	BH1750_Byte_Write(MODULE_RESET);
}
 
//BH1750初始化
uint8_t BH1750_Init(void)
{
	I2C_BH1750_GPIOConfig();		/* 配置GPIO */
	
	BH1750_Power_ON();	//BH1750s上电
	//BH1750_RESET();			//BH1750复位
	return BH1750_Byte_Write(Measure_Mode);
	//SysTick_Delay_ms(120);
}
//获取光照强度
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
	//初始化时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
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
	//起始信号
	I2C_GenerateSTART(I2C1,ENABLE);
	printf("oz3\r\n");
    while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	printf("oz4\r\n");
	
	//发送设备地址+写信号
	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Transmitter);
	printf("oz5\r\n");
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) != SUCCESS);
	printf("oz6\r\n");
	
	//内部寄存器地址
	I2C_SendData(I2C1,REG_Address);
	printf("oz7\r\n");
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTING) != SUCCESS);
	printf("oz8\r\n");
	//发送停止信号
	I2C_GenerateSTOP(I2C1,ENABLE);               
}
 
//*********************************************************
//
//连续读出BH1750内部数据
//
//*********************************************************
BYTE    BUF[8];                         //接收数据缓存区   
int     dis_data;                       //变量   	
static void Multiple_read_BH1750(void)
{   
	u8 i;
	printf("sw1\r\n");
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	printf("sw2\r\n");
	//起始信号
	I2C_GenerateSTART(I2C1,ENABLE);
	printf("sw3\r\n");
    while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS);
	printf("sw4\r\n");  
	//发送设备地址+读信号
	I2C_Send7bitAddress(I2C1,SlaveAddress,I2C_Direction_Receiver);
	printf("sw5\r\n");
	while(I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) != SUCCESS);
	printf("sw6\r\n");
   
	for (i=0; i<3-1; i++)                      //连续读取2个地址数据，存储中BUF
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
	
	 //停止信号
	I2C_GenerateSTOP(I2C1,ENABLE);
	printf("sw11\r\n");
	
	I2C_AcknowledgeConfig(I2C1,ENABLE);  
	printf("sw12\r\n");                      
    delay_ms(5);
	printf("get data finish\r\n");
}
 
//初始化BH1750，根据需要请参考pdf进行修改****
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
	Multiple_read_BH1750();		 //连续读出数据，存储在BUF中
	printf("handle1\r\n");
	dis_data=BUF[0];
	dis_data=(dis_data<<8)+BUF[1];//合成数据，即光照数据
	printf("handle2\r\n");
	return (int)dis_data/1.2;
	
}
#endif



