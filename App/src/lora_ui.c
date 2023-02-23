#include "lora_ui.h"
#include "usart.h"		
#include "delay.h"		
#include "string.h"    
#include "usart.h" 
#include "lora_cfg.h"
#include "lora_app.h"

#define delay_ms(ms) DWT_Delay_us(ms*1000)

 	
//usmart支持部分
//将收到的AT指令应答数据返回给电脑串口
//mode:0,不清零USART3_RX_STA;
//     1,清零USART3_RX_STA;
void lora_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		printf("send result:%s\r\n",USART3_RX_BUF);	//发送到串口
		if(mode)USART3_RX_STA=0;
	}
}
//lora发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
u8* lora_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//接收到一次数据了
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//添加结束符
		lora_at_response(0);
//		printf("%s",USART3_RX_BUF);
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	}
	return (u8*)strx;
}
//lora发送命令
//cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
u8 lora_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	printf("发送：%s\r\n",cmd);
	if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);//发送命令
	
	if(ack&&waittime)		//需要等待应答
	{
	   while(--waittime)	//等待倒计时
	   { 
			delay_ms(10);
		  if(USART3_RX_STA&0X8000)//接收到期待的应答结果
		  {
			  if(lora_check_cmd(ack))break;//得到有效数据 
			  USART3_RX_STA=0;
		  }
	   }
	   if(waittime==0)res=1; 
	}
	return res;
} 

//将1个字符转换为16进制数字
//chr:字符,0~9/A~F/a~F
//返回值:chr对应的16进制数值
u8 chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}

//字符转成16进制数据
//test[] 存储位置
//buffer[] 需转换的数组
//len： 长度
void char2hex_buff(u8 test[],u8 buff[],u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
 		test[i] = chr2hex(buff[i]);
	}
}

//将16进制数据转换成数字
u32 hexbuff_num(u8 test[],u8 buff[],u8 len)
{
	u8 i=0;
	u32 num=0; 
	
	for(i=0;i<len;i++) test[i] = chr2hex(buff[i]);
    if(len==5)  num = test[0]*10000+test[1]*1000+test[2]*100+test[3]*10+test[4];
	else if(len==4) num = test[0]*1000+test[1]*100+test[2]*10+test[3];		
	else if(len==3) num = test[0]*100+test[1]*10+test[2];		
	else if(len==2) num = test[0]*10+test[1];
	else if(len==1) num = test[0];
	
	return num;
}

//计算数字位数
//如:100,3位数、10,1位数
u8 num_count(u32 num)
{
	u32 date=0;
	u8 count=0;
	
	date = num;
	do
	{
		count++;
		date/=10;
	}while(date>0);
	
	return count;
}


