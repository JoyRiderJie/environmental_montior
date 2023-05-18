#ifndef _OLED_H
#define _OLED_H

#include "stm32f10x.h"

#define SIZE 16
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED端口定义----------------  					   

#define OLED_RST_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_4)//RES
#define OLED_RST_Set() GPIO_SetBits(GPIOD,GPIO_Pin_4)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_5)//DC
#define OLED_DC_Set() GPIO_SetBits(GPIOD,GPIO_Pin_5)

//使用4线串行接口时使用 

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_6)//CLK
#define OLED_SCLK_Set() GPIO_SetBits(GPIOD,GPIO_Pin_6)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_7)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOD,GPIO_Pin_7)

 		     
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

/* 
	* @函数作用	向SSD1106写入一个字节
	* @参数 
	*				@dat: 待写入的数据
	*				@cmd: 数据/命令标志 0-order,1-date
	*	@返回值 无
*/
void OLED_WR_Byte(u8 dat,u8 cmd);

/* 
	* @函数作用	
	* @参数 
	*				@x: 
	*				@y:  
	*	@返回值 无
*/
void OLED_Set_Pos(unsigned char x,unsigned char y);

/* 
	* @函数作用	打开OLED的显示功能
	* @参数 无
	*	@返回值 无
*/
void OLED_Display_On(void);

/* 
	* @函数作用	 关闭OLED的显示功能
	* @参数 无
	*	@返回值 无
*/
void OLED_Display_OFF(void);

/* 
	* @函数作用 清除显示屏中所有内容	
	* @参数 无
	*	@返回值 无
*/
void OLED_Clear(void);

/* 
	* @函数作用	在指定位置显示一个字符或者部分字符
	* @参数 
	*				@x: 0-127
	*				@y: 0-7
*					@chr:
	*	@返回值 无
*/
void OLED_ShowChar(u8 x,u8 y,u8 chr);

/* 
	* @函数作用	显示一个字符串
	* @参数 
	*				@x: 0-127
	*				@y: 0-7
*					@chr:
	*	@返回值 无
*/
void OLED_ShowString(u8 x,u8 y,u8* chr);

/* 
	* @函数作用 显示汉字	
	* @参数 
	*     @x:
	*     @y:
	*     @no:
	*	@返回值 无
*/
void OLED_ShowCHinese(u8 x,u8 y,u8 no);

/* 
	* @函数作用 显示32X32点阵汉字	
	* @参数 
	*     @x:
	*     @y:
	*     @no:
	*	@返回值 无
*/
void OLED_Show_CHinese32X32(u8 x,u8 y,u8 no);

/* 
	* @函数作用 初始化SSD1306		
	* @参数 无
	*	@返回值 无
*/
void OLED_Init(void);




#endif
