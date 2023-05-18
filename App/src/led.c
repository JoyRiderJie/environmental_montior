#include "led.h"

struct ledParam {
    // GPIO的分组
    GPIO_TypeDef* GPIOx;
    // GPIO的引脚
    uint16_t GPIO_Pin;
};


struct ledParam LEDDivece = {GPIOB,GPIO_Pin_5|GPIO_Pin_6};

/**********************************
* 函数功能：LED的初始化函数
* 函数参数：无
* 函数返回值：无
***********************************/
void ledInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    // 打开时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // GPIO参数设置
    GPIO_InitStructure.GPIO_Pin = LEDDivece.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LEDDivece.GPIOx, &GPIO_InitStructure);
}

/**********************************
* 函数功能：LED的初始化函数
* 函数参数：无
* 函数返回值：无
***********************************/
void ledControl(uint16_t location,BitAction state)
{
    GPIO_WriteBit(LEDDivece.GPIOx,location,state);
}

