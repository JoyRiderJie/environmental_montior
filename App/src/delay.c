#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//使用SysTick的普通计数模式对延迟进行管理（适合STM32F10x系列）
//包括delay_us,delay_ms
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2010/1/1
//版本：V1.8
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

static u8  fac_us=0;							//us延时倍乘数
static u16 fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数


//初始化延迟函数
//当使用OS的时候,此函数会初始化OS的时钟节拍
//SYSTICK的时钟固定为HCLK时钟的1/8
//SYSCLK:系统时钟
void delay_init()
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
    fac_us=SystemCoreClock/8000000;				//为系统时钟的1/8

    fac_ms=(u16)fac_us*1000;					//非OS下,代表每个ms需要的systick时钟数
}

//延时nus
//nus为要延时的us数.
void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD=nus*fac_us; 					//时间加载
    SysTick->VAL=0x00;        					//清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数
    do
    {
        temp=SysTick->CTRL;
    } while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
    SysTick->VAL =0X00;      					 //清空计数器
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864
void delay_ms(u16 nms)
{
    u32 temp;
    SysTick->LOAD=(u32)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
    SysTick->VAL =0x00;							//清空计数器
    SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数
    do
    {
        temp=SysTick->CTRL;
    } while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达
    SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
    SysTick->VAL =0X00;       					//清空计数器
}

// 使用定时器2完成的延时函数
void Tim2Delay_us(__IO uint32_t nTime)
{
    while(nTime)
        if(time2Count%4 < 2)
            nTime -= 2;

//	u32 temp = SysTick->VAL;        					//清空计数器
//	temp -= nTime * SystemCoreClock/1000000;
//	temp = (temp< 0) ? (SysTick->LOAD+temp) : temp;
//
//	while(SysTick->VAL == temp);      					 //清空计数器
}

uint32_t DWT_Dalay_Init(void)
{
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk;
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    if(DWT->CYCCNT)
    {
        return 0;
    }
    else {
        return 1;
    }
}

//延时函数,参数是n微秒
void DWT_Delay_us(volatile uint32_t nus)
{
    uint32_t clk_cycle_start = DWT->CYCCNT;  // 获得当前次数
    nus *= 72; // 72MHZ时，1us的时钟脉冲为72，1us计数72次，nus则是nus *= 72

    while((DWT->CYCCNT - clk_cycle_start < nus));  // 当计数次数达到预期时，跳出循环
}



