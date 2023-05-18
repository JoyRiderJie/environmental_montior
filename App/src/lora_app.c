#include "lora_app.h"
#include "lora_ui.h"
#include "lora_cfg.h"
#include "usart.h"
#include "string.h"
#include "delay.h"
#include "stdio.h"

#define delay_ms(ms) DWT_Delay_us(ms*1000)

#define Dire_DateLen sizeof(Dire_Date)/sizeof(Dire_Date[0])

extern __IO u16 ADC_ConvertedValue;

//设备参数初始化(具体设备参数见lora_cfg.h定义)
_LoRa_CFG LoRa_CFG=
{
    .addr = LORA_ADDR,       //设备地址
    .power = LORA_POWER,     //发射功率
    .chn = LORA_CHN,         //信道
    .wlrate = LORA_RATE,     //空中速率
    .wltime = LORA_WLTIME,   //睡眠时间
    .mode = LORA_MODE,       //工作模式
    .mode_sta = LORA_STA,    //发送状态
    .bps = LORA_TTLBPS,      //波特率设置
    .parity = LORA_TTLPAR    //校验位设置
};

//全局参数
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

// 串口3必要参数
struct uartParam usart3Param = {USART3,RCC_APB2Periph_GPIOB,RCC_APB1Periph_USART3,GPIOB,GPIO_Pin_11,GPIO_Pin_10,
    {115200,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Rx | USART_Mode_Tx,USART_HardwareFlowControl_None}
};
NVIC_InitTypeDef usart3NVIC_InitStructure = {USART3_IRQn,2,3,ENABLE};

// 定时器3必要参数
struct timeParam tim3Param = {TIM3,RCC_APB1Periph_TIM3,{7199,TIM_CKD_DIV1,99,TIM_CounterMode_Up}};
NVIC_InitTypeDef tim3NVIC_InitStructure = {TIM3_IRQn,1,2,ENABLE};

//设备工作模式(用于记录设备状态)
u8 Lora_mode=0; //0:配置模式 1:接收模式 2:发送模式
u8 Dire_Date[]= {0x11,0x22,0x33,0x44,0x55}; //定向传输数据
u8 date[30]= {0}; //定向数组
u32 obj_addr=0;//记录目标地址和信道（用于显示测试框右上角）（用户通过键盘数据输入时最大出现99999,所以需要定义u32类型）
u8 obj_chn=0;//记录用户输入目标信道
u8 Tran_Data[200]= {0}; //透传数组
u8 wlcd_buff[10]= {0}; //LCD显示字符串缓冲区


//记录中断状态
static u8 Int_mode=0;//0:关闭 1:上升沿 2:下降沿

//AUX中断设置
//mode:配置的模式 0:关闭 1:上升沿 2:下降沿
void Aux_Int(u8 mode)
{
    if(!mode)
    {
        EXTI_InitStructure.EXTI_LineCmd = DISABLE;//关闭中断
        NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    } else
    {
        if(mode==1)
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿
        else if(mode==2)
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿

        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    }
    Int_mode = mode;//记录中断模式
    EXTI_Init(&EXTI_InitStructure);
    NVIC_Init(&NVIC_InitStructure);

}

//LORA_AUX中断服务函数
void EXTI4_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line4))
    {
        if(Int_mode==1)//上升沿(发送:开始发送数据 接收:数据开始输出)
        {
            if(Lora_mode==1)//接收模式
            {
                USART3_RX_STA=0;//数据计数清0
            }
            Int_mode=2;//设置下降沿触发
        }
        else if(Int_mode==2)//下降沿(发送:数据已发送完 接收:数据输出结束)
        {
            if(Lora_mode==1)//接收模式
            {
                USART3_RX_STA|=1<<15;//数据计数标记完成
            } else if(Lora_mode==2)//发送模式(串口数据发送完毕)
            {
                Lora_mode=1;//进入接收模式
            }
            Int_mode=1;//设置上升沿触发
        }
        Aux_Int(Int_mode);//重新设置中断边沿
        EXTI_ClearITPendingBit(EXTI_Line4); //清除LINE4上的中断标志位
    }
}

//LoRa模块初始化
//返回值: 0,检测成功
//        1,检测失败
static u8 _loRaInit(void)
{
    u8 retry=0;
    u8 temp=1;

    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PA端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//禁止JTAG,从而PA15可以做普通IO使用,否则PA15不能做普通IO!!!

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	    		 //LORA_MD0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //LORA_AUX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		     //下拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOA.4

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);

    EXTI_InitStructure.EXTI_Line=EXTI_Line4;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = DISABLE;              //中断线关闭(先关闭后面再打开)
    EXTI_Init(&EXTI_InitStructure);//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//LORA_AUX
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级2，
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE; //关闭外部中断通道（后面再打开）
    NVIC_Init(&NVIC_InitStructure);

    LORA_MD0=0;
    LORA_AUX=0;

    while(LORA_AUX)//确保LORA模块在空闲状态下(LORA_AUX=0)
    {
        printf("模块正忙,请稍等!!\n");
        delay_ms(1000);
    }
    //初始化串口3
    usartInit(&usart3Param,&usart3NVIC_InitStructure);
    // 初始化定时器3
    timInit(&tim3Param,&tim3NVIC_InitStructure);
    USART3_RX_STA=0;		//清零
    TIM_Cmd(TIM3,DISABLE);	//关闭定时器3

    LORA_MD0=1;//进入AT模式
    delay_ms(1000);
    retry=3;

    while(retry--)
    {
        if(!lora_send_cmd((u8 *)"ATE0",(unsigned char *)"OK",70)) //OK
        {
            temp=0;//检测成功
            printf("检测成功\n");
            break;
        }
    }
    if(retry==0) temp=1;//检测失败
    return temp;
}

//Lora模块参数配置
void LoRa_Set(void)
{
    u8 sendbuf[20];
    u8 lora_addrh,lora_addrl=0;
    //进入配置模式前设置通信波特率和校验位(115200 8位数据 1位停止 无数据校验）
    setUsartParam(&usart3Param,LORA_TTLBPS_115200,LORA_TTLPAR_8N1);
    //开启串口3收发模式
    setUsartMod(&usart3Param,0);

    while(LORA_AUX);//等待模块空闲
    LORA_MD0=1; //进入配置模式
    delay_ms(1000);
    Lora_mode=0;//标记"配置模式"

    lora_addrh =  (LoRa_CFG.addr>>8)&0xff;
    lora_addrl = LoRa_CFG.addr&0xff;
    sprintf((char*)sendbuf,"AT+ADDR=%02x,%02x",lora_addrh,lora_addrl);//设置设备地址
    lora_send_cmd(sendbuf,(u8 *)"OK",50);
    sprintf((char*)sendbuf,"AT+WLRATE=%d,%d",LoRa_CFG.chn,LoRa_CFG.wlrate);//设置信道和空中速率
    lora_send_cmd(sendbuf,(u8 *)"OK",50);
    sprintf((char*)sendbuf,"AT+TPOWER=%d",LoRa_CFG.power);//设置发射功率
    lora_send_cmd(sendbuf,(u8 *)"OK",50);
    sprintf((char*)sendbuf,"AT+CWMODE=%d",LoRa_CFG.mode);//设置工作模式
    lora_send_cmd(sendbuf,(u8 *)"OK",50);
    sprintf((char*)sendbuf,"AT+TMODE=%d",LoRa_CFG.mode_sta);//设置发送状态
    lora_send_cmd(sendbuf,(u8 *)"OK",50);
    sprintf((char*)sendbuf,"AT+WLTIME=%d",LoRa_CFG.wltime);//设置睡眠时间
    lora_send_cmd(sendbuf,(u8 *)"OK",50);
    sprintf((char*)sendbuf,"AT+UART=%d,%d",LoRa_CFG.bps,LoRa_CFG.parity);//设置串口波特率、数据校验位
    lora_send_cmd(sendbuf,(u8 *)"OK",50);

    LORA_MD0=0;//退出配置,进入通信
    delay_ms(400);
    while(LORA_AUX);//判断是否空闲(模块会重新配置参数)
    USART3_RX_STA=0;
    Lora_mode=1;//标记"接收模式"
    //返回通信,更新通信串口配置(波特率、数据校验位)
    setUsartParam(&usart3Param,LoRa_CFG.bps,LoRa_CFG.parity);
    Aux_Int(1);//设置LORA_AUX上升沿中断
}

void LoRa_SendData(char* message)//Lora模块发送数据
{
    u16 addr;
    u8 chn;
    u16 i=0;

    Lora_mode=2;//标记"发送状态"

    if(LoRa_CFG.mode_sta == LORA_STA_Tran)//透明传输
    {
        sprintf((char*)Tran_Data,"%s",message);
        u3_printf("%s\r\n",Tran_Data);
        printf("Send：%s\r\n",Tran_Data);//显示发送的数据
    }
    else if(LoRa_CFG.mode_sta == LORA_STA_Dire)//定向传输
    {
        addr = (u16)obj_addr;//目标地址
        chn = obj_chn;//目标信道

        date[i++] =(addr>>8)&0xff;//高位地址
        date[i++] = addr&0xff;//低位地址
        date[i] = chn;//无线信道

        for(i=0; i<Dire_DateLen; i++) //数据写到发送BUFF
        {
            date[3+i] = Dire_Date[i];
        }
        for(i=0; i<(Dire_DateLen+3); i++)
        {
            while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);//循环发送,直到发送完毕
            USART_SendData(USART3,date[i]);
        }

        //将十六进制的数据转化为字符串打印在lcd_buff数组
        sprintf((char*)wlcd_buff,"%x %x %x %x %x %x %x %x",date[0],date[1],date[2],date[3],date[4],date[5],date[6],date[7]);

        Dire_Date[4]++;//Dire_Date[4]数据更新
    }
}

/*******************************************************************
*函数：char *USER_GetSteeringEngineJsonValue(char *cJson, char *Tag)
*功能：json为字符串序列，将json格式中的目标对象Tag对应的值字符串转换为数值
*输入：
		char *cJson json字符串
		char *Tag 要操作的对象标签
*输出：返回数值
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
uint8_t USER_GetSteeringEngineJsonValue(char *cJson, char *Tag)
{
    char *target = NULL;
    //char *str = NULL;
    static char temp[10];
    uint8_t len=0;
    uint8_t value=0;
    int8_t i=0;

    memset(temp, 0x00, 128);
    sprintf(temp,"\"%s\":\"",Tag);
    target=strstr((const char *)cJson, (const char *)temp);
    if(target == NULL)
    {
        printf("空字符！\r\n");
        return NULL;
    }
    i=strlen((const char *)temp);
    target=target+i;
    memset(temp, 0x00, 128);
    for(i=0; i<10; i++, target++)//数值超过10个位为非法，由于2^32=4294967296
    {
        if((*target<='9')&&(*target>='0'))
        {
            temp[i]=*target;
        }
        else
        {
            break;
        }
    }
    //str=strstr((const char *)target, (const char *)"\",");

    temp[i+1] = '\0';
// printf("数值str=%s\r\n",temp);
//	printf("数值str=%s\r\n",target);
// len=strlen((const char *)target);
//	for(i=0; i<len;i++)
//    {
//	 printf("数值str[%d]=0x%x  \r\n",i,*(target+i));
//	}
    len=strlen((const char *)temp);
    switch(len)
    {
    case(1):
        value=temp[0]-'0';
        break;
    case(2):
        value=temp[1]-'0'+(temp[0]-'0')*10;
        break;
    case(3):
        value=temp[2]-'0'+(temp[0]-'0')*100+(temp[1]-'0')*10;
        break;
    default:
        break;
    }
    //printf("数值value=%d\r\n",value);
    return value;
}

//Lora模块接收数据
void LoRa_ReceData(void)
{
    u16 i=0;
    u16 len=0;
    //有数据来了
    if(USART3_RX_STA&0x8000)
    {
        len = USART3_RX_STA&0X7FFF;
        USART3_RX_BUF[len]=0;//添加结束符
        USART3_RX_STA=0;

        for(i=0; i<len; i++)
        {
            while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕
            USART_SendData(USART1,USART3_RX_BUF[i]);
        }
        if(LoRa_CFG.mode_sta==LORA_STA_Tran)//透明传输
        {
            //ESP8266_IpSend((char *)USART3_RX_BUF,strlen((const char *)USART3_RX_BUF));
            lora_at_response(0);//显示接收到的数据
        }

        //int recFlag = 0;
        //数据解析
#if FAN
        //arr+1~0
        //占空比计算：fan0pwmval/arr+1;  最大也就是5500/7200
        //FAN占空比大于%50差不多就不转了
        //LIGHT占空比可以从%0~%100
        //FAN1
        if(strstr((const char *)USART3_RX_BUF, (const char *)"fanStatus1") != NULL)
        {
            recFlag++;
            fanStatus1 = USER_GetSteeringEngineJsonValue((char *)USART3_RX_BUF, (char *)"fanStatus1");

            if(fanStatus1 == 0)
            {
                TIM_SetCompare1(TIM1,100*72);
                printf("FAN1 OFF\r\n");
            }
            else if(fanStatus1 == 1)
            {
                TIM_SetCompare1(TIM1,fanVal1);
                printf("FAN1 ON\r\n");
            }
        }
        if(strstr((const char *)USART3_RX_BUF, (const char *)"fanSpeed1") != NULL) {
            recFlag++;
            fanSpeed1 = USER_GetSteeringEngineJsonValue((char *)USART3_RX_BUF, (char *)"fanSpeed1");

            fanVal1 = (int)(100 - fanSpeed1)*72;
            if(fanStatus1 == 1)
                TIM_SetCompare1(TIM1,fanVal1);	  //通道比较值   修改TIM1_CCR1 占空比

            printf("FAN1 = %d%%\r\n", fanSpeed1);
        }

        //FAN2
        if(strstr((const char *)USART3_RX_BUF, (const char *)"fanStatus2") != NULL) {
            recFlag++;
            fanStatus2 = USER_GetSteeringEngineJsonValue((char *)USART3_RX_BUF, (char *)"fanStatus2");

            if(fanStatus2 == 0) {
                TIM_SetCompare4(TIM1,100*72);
                printf("FAN2 OFF\r\n");
            }
            else if(fanStatus2 == 1) {
                TIM_SetCompare4(TIM1,fanVal2);
                printf("FAN2 ON\r\n");
            }
        }
        if(strstr((const char *)USART3_RX_BUF, (const char *)"fanSpeed2") != NULL) {
            recFlag++;
            fanSpeed2 = USER_GetSteeringEngineJsonValue((char *)USART3_RX_BUF, (char *)"fanSpeed2");

            fanVal2 = (int)(100 - fanSpeed2)*72;

            if(fanStatus2 == 1)
                TIM_SetCompare4(TIM1,fanVal2);	  //通道比较值   修改TIM1_CCR1 占空比
            printf("FAN2 = %d%%\r\n", fanSpeed2);
        }

        if(recFlag > 0) {
            //确认消息回发
            LoRa_SendData("FanRecOK");
        }
#elif LIGHT
        if(strstr((const char *)USART3_RX_BUF, (const char *)"lightStatus") != NULL) {
            recFlag++;
            lightStatus = USER_GetSteeringEngineJsonValue((char *)USART3_RX_BUF, (char *)"lightStatus");

            if(lightStatus == 0) {
                TIM_SetCompare1(TIM1,100*9);
                printf("LIGHT OFF\r\n");
            }
            else if(lightStatus == 1) {
                TIM_SetCompare1(TIM1,lightVal);
                printf("LIGHT ON\r\n");
            }
        }
        if(strstr((const char *)USART3_RX_BUF, (const char *)"lightPWM") != NULL)
        {
            recFlag++;
            lightPWM = USER_GetSteeringEngineJsonValue((char *)USART3_RX_BUF, (char *)"lightPWM");

            lightVal = (int)(100 - lightPWM)*9;

            if(lightStatus == 1)
                TIM_SetCompare1(TIM1,lightVal);	  //通道比较值   修改TIM1_CCR1 占空比
            printf("LIGHT = %d%%\r\n", lightPWM);
        }
        if(recFlag > 0)
        {
            //确认消息回发
            LoRa_SendData("LightRecOK");
        }
#endif

        memset((char*)USART3_RX_BUF,0x00,len);//串口接收缓冲区清0
    }

}

/**************************************************
* 函数功能：LORA初始化函数
* 函数参数：无
* 函数返回值：无
**************************************************/
void loraInit(void)
{
    //初始化ATK-LORA-01模块 初始化为无写回模式
    while(_loRaInit())
    {
        printf("未检测到模块!!!\r\n");
        printf("\r\n");
        delay_ms(300);
    }
    printf("检测到模块!!!\r\n");
    delay_ms(500);

    //1.进入配置模式
    LORA_MD0=1; //进入配置模式
    delay_ms(40);
    //关闭串口接收
    setUsartMod(&usart3Param,1);
    Aux_Int(0);//关闭中断

    LoRa_CFG.addr = 0x5410;
    LoRa_CFG.chn = 0x10;   //0x20
    LoRa_CFG.power = LORA_PW_11dBm;
    LoRa_CFG.wlrate = LORA_RATE_19K2;
    LoRa_CFG.wltime = LORA_WLTIME_1S;
    LoRa_CFG.mode = LORA_MODE_GEN;
    LoRa_CFG.mode_sta = LORA_STA_Tran;   //LORA_STA_Dire
    LoRa_CFG.bps = LORA_TTLBPS_115200;
    LoRa_CFG.parity = LORA_TTLPAR_8N1;
    LoRa_Set();
}


