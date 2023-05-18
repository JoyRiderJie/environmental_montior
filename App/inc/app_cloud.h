/******************************************************************************
******************************************************************************
* @file    app/app_cloud.h 
* @author  zhao
* @version V1.0.0
* @date    2018.07.17
* @update  2018.07.17 
* @brief   云服务
******************************************************************************
******************************************************************************/
#ifndef __APP_CLOUD_H__
#define __APP_CLOUD_H__
#ifdef __cplusplus  /* C++支持 */
	extern "C"{
#endif

#include "sys.h"

void cloud_task(void);
uint8_t USER_GetSteeringEngineJsonValue(char *cJson, char *Tag);
void cloudTest(void);

#endif


