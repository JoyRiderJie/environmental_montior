#include "usart.h"

//串口接收缓存区
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//发送缓冲,最大USART3_MAX_SEND_LEN字节

/******************************
* 函数功能：串口初始化
* 函数参数：
*			struct uartParam*usart:串口信息结构体 内含串口引脚分组及引脚波特率等必要信息
*			NVIC_InitTypeDef*NVIC_InitStructure：NIVC信息结构体 内含NVIC通道、抢占优先级等必要信息
* 函数返回值：无
*******************************/
void usartInit(struct uartParam*usart,NVIC_InitTypeDef*NVIC_InitStructure)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 外设时钟初始化
    if(usart->RCC_APB1Periph)
        RCC_APB1PeriphClockCmd(usart->RCC_APB1Periph,ENABLE);
    if(usart->RCC_APB2Periph)
        RCC_APB2PeriphClockCmd(usart->RCC_APB2Periph,ENABLE);

    // 发送引脚初始化
    GPIO_InitStructure.GPIO_Pin = usart->TxGPIOPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(usart->GPIOx,&GPIO_InitStructure);
    // 接收引脚初始化
    GPIO_InitStructure.GPIO_Pin = usart->RxGPIOPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(usart->GPIOx,&GPIO_InitStructure);

    //串口初始化
    USART_Init(usart->usartName, &usart->USART_InitStructure);
    //使能串口
    USART_Cmd(usart->usartName, ENABLE);
    //开启中断
    USART_ITConfig(usart->usartName, USART_IT_RXNE, ENABLE);

    // NVIC初始化
    NVIC_Init(NVIC_InitStructure);
}


/***********************************************
* 函数功能：串口波特率和校验位配置
* 函数参数：
*			struct uartParam*usart：串口信息结构体
*			bps:波特率（1200~115200）
*			parity:校验位（无、偶、奇）
* 函数返回值：无
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

    USART_Cmd(USART3, DISABLE); //关闭串口
    usart->USART_InitStructure.USART_BaudRate = bound;
    usart->USART_InitStructure.USART_StopBits = USART_StopBits_1;

    if(parity==LORA_TTLPAR_8N1)//无校验
    {
        usart->USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        usart->USART_InitStructure.USART_Parity = USART_Parity_No;
    } else if(parity==LORA_TTLPAR_8E1)//偶校验
    {
        usart->USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        usart->USART_InitStructure.USART_Parity = USART_Parity_Even;
    } else if(parity==LORA_TTLPAR_8O1)//奇校验
    {
        usart->USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        usart->USART_InitStructure.USART_Parity = USART_Parity_Odd;
    }
    USART_Init(USART3, &usart->USART_InitStructure); //初始化串口3
    USART_Cmd(USART3, ENABLE); //使能串口
}

/**********************************************************
* 函数功能：设置串口收发模式
* 函数参数：
*			struct uartParam*usart：传入串口结构体
*			u8 state：传入串口模式 0-收发模式 1-只发送模式 2-只接收模式
* 函数参数：无
**********************************************************/
void setUsartMod(struct uartParam*usart,u8 mod)
{
    USART_Cmd(USART3, DISABLE); //失能串口

    if(mod == 0)
        usart->USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
    else if(mod == 1)
        usart->USART_InitStructure.USART_Mode = USART_Mode_Tx;//只发送
    else if(mod == 2)
        usart->USART_InitStructure.USART_Mode = USART_Mode_Rx;//只接收

    USART_Init(usart->usartName, &usart->USART_InitStructure); //初始化串口3
    USART_Cmd(usart->usartName, ENABLE); //使能串口
}

/******************************************** USART1相关 ********************************************/

/*  使用printf函数相关(串口发送)  */
struct __FILE
{
    int handle;

};
FILE __stdout;

//重定义fputc函数  用于printf串口1输出
int fputc(int ch, FILE *f)
{
    //循环发送,直到发送完毕
    while((USART1->SR&0X40)==0)
        ;
    USART1->DR = (u8) ch;
    return ch;
}

/*  串口接收相关(中断服务函数) */
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//记录接收状态 bit15-接收完成标志  bit14-接收到0x0d  bit13~0-接收到的有效字节数目
u16 USART_RX_STA=0;
void USART1_IRQHandler(void)
{
    u8 Res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res =USART_ReceiveData(USART1);	//读取接收到的数据

        if((USART_RX_STA&0x8000)==0)//接收未完成
        {
            if(USART_RX_STA&0x4000)//接收到了0x0d
            {
                if(Res!=0x0a)
                    USART_RX_STA=0;//接收错误,重新开始
                else
                    USART_RX_STA|=0x8000;	//接收完成了
            }
            else //还没收到0X0D
            {
                if(Res==0x0d)
                    USART_RX_STA|=0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
                    USART_RX_STA++;
                    if(USART_RX_STA > (USART_REC_LEN-1))
                        USART_RX_STA=0;//接收数据错误,重新开始接收
                }
            }
        }
    }
}


/******************************************** USART3相关 ********************************************/

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过timer,则认为不是1次连续数据.也就是超过timer没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART3_RX_STA=0;

void USART3_IRQHandler(void)
{
    u8 res;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
    {
        res =USART_ReceiveData(USART3);
        if((USART3_RX_STA&(1<<15))==0)              //接收完的一批数据,还没有被处理,则不再接收其他数据
        {
            if(USART3_RX_STA<USART3_MAX_RECV_LEN)	//还可以接收数据
            {
                if(!Lora_mode)//配置功能下(启动定时器超时)
                {
                    TIM_SetCounter(TIM3,0);             //计数器清空
                    if(USART3_RX_STA==0) 				//使能定时器7的中断
                    {
                        TIM_Cmd(TIM3,ENABLE);           //使能定时器3
                    }
                }
                USART3_RX_BUF[USART3_RX_STA++]=res;	//记录接收到的值
            } else
            {
                USART3_RX_STA|=1<<15;				//强制标记接收完成
            }
        }
    }
}


//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART3_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
    for(j=0; j<i; j++)							//循环发送数据
    {
        while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
        USART_SendData(USART3,USART3_TX_BUF[j]);
    }
}

/***************** 串口4相关   *******************************************/
/******************************************************************************
* Function Name --> 串口4发送一个字节数据
* Description   --> nonevoid USART1_SendData(uint16_t Data)
* Input         --> bound：波特率	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_SendData(uint16_t Data)
{
	while((UART4->SR & 0x40) == 0);	//等待发送完毕
	USART_SendData(UART4, Data);
}

/******************************************************************************
* Function Name --> 串口4发送一串数据
* Description   --> none
* Input         --> *Data:字符串	
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
* Function Name --> 串口4发送一串数据，指定数据长度
* Description   --> none
* Input         --> *Data:字符串，len长度	
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

//串口4中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误 

uint8_t UART4_RX_BUF[UART4_REC_LEN];	//接收缓冲,最大UART4_REC_LEN个字节

//接收状态
uint16_t UART4_RX_COUNT=0;	//接收到的有效字节数目，最大USART1_REC_LEN字节
uint8_t F_UART4_RX_FINISH=0;//接收状态标记,1:UART4接收完成，0：UART4接收未完成
uint8_t F_UART4_RX_RECEIVING=0;//串口接收正在进行中标记,1:进行中，0：暂停或结束
uint8_t UART4_RX_TIMEOUT_COUNT=0;	//串口接收超时计数。串口超过3ms未接收到数据，视为超时。单位：ms

/******************************************************************************
* Function Name --> 串口4接收中断服务程序
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_IRQHandler(void)
{
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收到数据
	{
		F_UART4_RX_RECEIVING=1;//正在接收串口数据
		UART4_RX_TIMEOUT_COUNT=0;//超时计数清零
		UART4_RX_BUF[UART4_RX_COUNT] = USART_ReceiveData(UART4);	//读取接收到的数据
		UART4_RX_COUNT++;
		if(UART4_RX_COUNT>UART4_REC_LEN)//缓存区溢出
		{UART4_RX_COUNT = 0x0000;}	
		//end 接收未完成   		 		 
	}	//end 接收到数据
}

