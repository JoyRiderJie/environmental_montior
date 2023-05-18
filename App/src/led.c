#include "led.h"

struct ledParam {
    // GPIO�ķ���
    GPIO_TypeDef* GPIOx;
    // GPIO������
    uint16_t GPIO_Pin;
};


struct ledParam LEDDivece = {GPIOB,GPIO_Pin_5|GPIO_Pin_6};

/**********************************
* �������ܣ�LED�ĳ�ʼ������
* ������������
* ��������ֵ����
***********************************/
void ledInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    // ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // GPIO��������
    GPIO_InitStructure.GPIO_Pin = LEDDivece.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LEDDivece.GPIOx, &GPIO_InitStructure);
}

/**********************************
* �������ܣ�LED�ĳ�ʼ������
* ������������
* ��������ֵ����
***********************************/
void ledControl(uint16_t location,BitAction state)
{
    GPIO_WriteBit(LEDDivece.GPIOx,location,state);
}

