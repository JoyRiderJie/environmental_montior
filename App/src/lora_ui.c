#include "lora_ui.h"
#include "usart.h"		
#include "delay.h"		
#include "string.h"    
#include "usart.h" 
#include "lora_cfg.h"
#include "lora_app.h"

#define delay_ms(ms) DWT_Delay_us(ms*1000)

 	
//usmart֧�ֲ���
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������USART3_RX_STA;
//     1,����USART3_RX_STA;
void lora_at_response(u8 mode)
{
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		printf("send result:%s\r\n",USART3_RX_BUF);	//���͵�����
		if(mode)USART3_RX_STA=0;
	}
}
//lora���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
u8* lora_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART3_RX_BUF[USART3_RX_STA&0X7FFF]=0;//��ӽ�����
		lora_at_response(0);
//		printf("%s",USART3_RX_BUF);
		strx=strstr((const char*)USART3_RX_BUF,(const char*)str);
	}
	return (u8*)strx;
}
//lora��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 lora_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	USART3_RX_STA=0;
	printf("���ͣ�%s\r\n",cmd);
	if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);//�ȴ���һ�����ݷ������  
		USART3->DR=(u32)cmd;
	}else u3_printf("%s\r\n",cmd);//��������
	
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
	   while(--waittime)	//�ȴ�����ʱ
	   { 
			delay_ms(10);
		  if(USART3_RX_STA&0X8000)//���յ��ڴ���Ӧ����
		  {
			  if(lora_check_cmd(ack))break;//�õ���Ч���� 
			  USART3_RX_STA=0;
		  }
	   }
	   if(waittime==0)res=1; 
	}
	return res;
} 

//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}

//�ַ�ת��16��������
//test[] �洢λ��
//buffer[] ��ת��������
//len�� ����
void char2hex_buff(u8 test[],u8 buff[],u8 len)
{
	u8 i=0;
	for(i=0;i<len;i++)
	{
 		test[i] = chr2hex(buff[i]);
	}
}

//��16��������ת��������
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

//��������λ��
//��:100,3λ����10,1λ��
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


