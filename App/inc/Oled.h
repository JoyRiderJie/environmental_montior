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
//-----------------OLED�˿ڶ���----------------  					   

#define OLED_RST_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_4)//RES
#define OLED_RST_Set() GPIO_SetBits(GPIOD,GPIO_Pin_4)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_5)//DC
#define OLED_DC_Set() GPIO_SetBits(GPIOD,GPIO_Pin_5)

//ʹ��4�ߴ��нӿ�ʱʹ�� 

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_6)//CLK
#define OLED_SCLK_Set() GPIO_SetBits(GPIOD,GPIO_Pin_6)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOD,GPIO_Pin_7)//DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOD,GPIO_Pin_7)

 		     
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

/* 
	* @��������	��SSD1106д��һ���ֽ�
	* @���� 
	*				@dat: ��д�������
	*				@cmd: ����/�����־ 0-order,1-date
	*	@����ֵ ��
*/
void OLED_WR_Byte(u8 dat,u8 cmd);

/* 
	* @��������	
	* @���� 
	*				@x: 
	*				@y:  
	*	@����ֵ ��
*/
void OLED_Set_Pos(unsigned char x,unsigned char y);

/* 
	* @��������	��OLED����ʾ����
	* @���� ��
	*	@����ֵ ��
*/
void OLED_Display_On(void);

/* 
	* @��������	 �ر�OLED����ʾ����
	* @���� ��
	*	@����ֵ ��
*/
void OLED_Display_OFF(void);

/* 
	* @�������� �����ʾ������������	
	* @���� ��
	*	@����ֵ ��
*/
void OLED_Clear(void);

/* 
	* @��������	��ָ��λ����ʾһ���ַ����߲����ַ�
	* @���� 
	*				@x: 0-127
	*				@y: 0-7
*					@chr:
	*	@����ֵ ��
*/
void OLED_ShowChar(u8 x,u8 y,u8 chr);

/* 
	* @��������	��ʾһ���ַ���
	* @���� 
	*				@x: 0-127
	*				@y: 0-7
*					@chr:
	*	@����ֵ ��
*/
void OLED_ShowString(u8 x,u8 y,u8* chr);

/* 
	* @�������� ��ʾ����	
	* @���� 
	*     @x:
	*     @y:
	*     @no:
	*	@����ֵ ��
*/
void OLED_ShowCHinese(u8 x,u8 y,u8 no);

/* 
	* @�������� ��ʾ32X32������	
	* @���� 
	*     @x:
	*     @y:
	*     @no:
	*	@����ֵ ��
*/
void OLED_Show_CHinese32X32(u8 x,u8 y,u8 no);

/* 
	* @�������� ��ʼ��SSD1306		
	* @���� ��
	*	@����ֵ ��
*/
void OLED_Init(void);




#endif
