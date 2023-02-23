#ifndef __IIC_H
#define __IIC_H

#include "sys.h"

/* 定义I2C总线连接的GPIO端口, 只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define BH1750_GPIO_PORT_I2C	GPIOB			/* GPIO端口 */
#define BH1750_RCC_I2C_PORT 	RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define BH1750_I2C_SCL_PIN		GPIO_Pin_6			/* 连接到SCL时钟线的GPIO */
#define BH1750_I2C_SDA_PIN		GPIO_Pin_7			/* 连接到SDA数据线的GPIO */

void bh1750Init(void);


#endif
