#ifndef _LORA_APP_H_
#define _LORA_APP_H_

#include "stm32f10x.h"
#include "lora_cfg.h"
#include "sys.h"
#include "time.h"

#define LORA_AUX  PAin(4)    //LORA模块状态引脚
#define LORA_MD0  PAout(15)  //LORA模块控制引脚

extern _LoRa_CFG LoRa_CFG;
extern u8 Lora_mode;
extern u32 obj_addr;//记录用户输入目标地址
extern u8 obj_chn;//记录用户输入目标信道

u8 LoRa_Init(void);
void Aux_Int(u8 mode);
void LoRa_Set(void);
void LoRa_SendData(char* message);
void LoRa_ReceData(void);
void LoRa_Process(void);
void loraInit(void);

#endif

