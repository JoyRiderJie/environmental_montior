#include "event.h"

/*
 *  get_inputevent: 用来获取触摸屏输入事件
 *  arg：
 *      @int*getlocation:保存触摸屏的始末位置
 *  return：
 *      成功返回0
 *      失败返回-1
**/
int get_inputevent(int*getlocation,int fd)
{
    int ret = 0;
    int abs_x = -1,abs_y = -1;//记录触摸点的(x,y)坐标
    struct input_event ev;	//保存获取到的输入事件

    /* 输入子系统往往会将一个设备的一次动作分成几个输入事件上传给应用层
        比如： 触摸屏的一次触摸 就会被分成：
            ABS_X	触摸点的X轴事件
            ABS_Y	触摸点的Y轴事件
            等内容
        因此获取输入事件时，通常会采用循环的方式获取，以确保将一次动作的所有事件全部获取
    */
    // while(read(fd,&ev,sizeof(ev)) == sizeof(ev))
    while(1)
    {
        //读取一整个输入事件的数据量，存入结构体ev中
        ret = read(fd,&ev,sizeof(ev)); 
        // printf("type:%d code:%d value:%d\n\n",ev.type,ev.code,ev.value);
        //确保读取到的就是一整个输入事件
        if(sizeof(ev) == ret) 
        {
            //表示产生了触摸屏事件
    		if(ev.type == EV_ABS)	
            {
                //事件编码为X轴触发
                if(ev.code == ABS_X)	
                {
                    //此时的value就是触摸点X轴的坐标
                    abs_x =	ev.value;
                }
                //事件编码为Y轴触发
                else if(ev.code == ABS_Y)	
                {
                    //此时的value就是触摸点y轴的坐标
                    abs_y =	ev.value;
                }
            }
            //触摸屏触摸事件：将触摸屏当成一个按键来识别（触摸开关）
            else if(ev.type == EV_KEY && ev.code == BTN_TOUCH)	
            {
                if(ev.value == 1)
                {
                    //表示手指触摸触摸屏的瞬间
                    getlocation[0] = abs_x;
                    getlocation[1] = abs_y;
                }
                else if(ev.value == 0)
                {
                    //表示手指从触摸屏离开的瞬间
                    getlocation[2] = abs_x;
                    getlocation[3] = abs_y;
                    return 0;
                }
            }
                
        }
    }
    return -1;
}

