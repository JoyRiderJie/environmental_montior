#ifndef __IIC_H
#define __IIC_H

#include "sys.h"

/* ����I2C�������ӵ�GPIO�˿�, ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define BH1750_GPIO_PORT_I2C	GPIOB			/* GPIO�˿� */
#define BH1750_RCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define BH1750_I2C_SCL_PIN		GPIO_Pin_6			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define BH1750_I2C_SDA_PIN		GPIO_Pin_7			/* ���ӵ�SDA�����ߵ�GPIO */

void bh1750Init(void);


#endif
