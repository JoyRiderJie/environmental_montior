#include "config.h"

// 定义变量存储照片信息
char*picture[10] = {"backgroud.bmp","myPlane.bmp","targetPlane.bmp","bullet.bmp"};
// 定义变量存储触摸屏触摸时的起始位置与终止位置
int*myLocation;
//  定义变量 记录初始化的事件
int fd = 0;
int fcid = 0;
int myPipe[2];

int sonFlag = 0;

/********************************************
*  函数功能:信号量服务函数
*  函数参数:无
*  函数返回值:无
********************************************/
void sysInit(void)
{
    int i=0;
    //  初始化LCD 为显示做准备
    lcd_init();


    // 初始化位置信息
    myLocation = (int*)malloc(sizeof(int)*4);
    for(i=0;i<4;i++)
        myLocation[i] = 0;

    // 捕捉SIGINT信号(Ctrl+C),当用户按下 Ctrl+C结束父进程时,会自动去执行 my_signal 函数
    signal(SIGINT,my_sig_handler);    

    //  清屏  避免上次显示的图片影响本次显示
    lcd_draw_clear(0,0,480,800,0xffffff);
    //  显示游戏背景
    bmp_show(picture[0],0,0);
}

/********************************************
*  函数功能:信号量服务函数
*  函数参数:无
*  函数返回值:无
********************************************/
void sysWork(void)
{
    //  创建管道  用于父子进程通信
    while(pipe(myPipe) == -1)
        printf("failtrue of pipe-ing");

    //  创建父子进程
    while((fcid = fork()) < 0)
        printf("failture of fork-ing\n");

    if(fcid == 0)
        //  子进程工作
        sonWork();
    else if(fcid > 0)
        //  父进程工作
        fatherWork();
    //  解除屏幕，不再需要LCD显示时，解除屏幕    
    lcd_uninit(); //  解除屏幕
    //   解除触摸屏
    close(fd);
}

/********************************************
*  函数功能:父进程函数
*  函数参数:无
*  函数返回值:无
********************************************/
void fatherWork(void)
{
    unsigned int count = 0;
    while(1)
    {
        printf("我已经正常工作了 %d 次\n",++count);
        // close(myPipe[1]);
        // read(myPipe[0],myLocation,sizeof(myLocation)+sizeof(int));
        printf("f:%d %d %d %d\n",myLocation[0],myLocation[1],myLocation[2],myLocation[3]);

        sleep(1);
    }
}

/********************************************
*  函数功能:子进程函数
*  函数参数:无
*  函数返回值:无
********************************************/
void sonWork(void)
{
    int flags = 0;
    int i=0;


    // 注册信号处理函数
	signal(SIGIO, my_sig_handler);
    //  打开驱动程序
	while((fd = open("/dev/input/event0", O_RDWR | O_NONBLOCK)) == -1)// | O_NONBLOCK)) == -1)
        perror("opening failture\n");
    printf(" initializetion of son was finished\n");
    //  把APP的进程号告诉驱动程序 
	fcntl(fd, F_SETOWN, getpid());
	//  使能"异步通知"
	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags | FASYNC);

    while(1) 
    {    
        if(sonFlag)
        {
            // // 触摸事件触发  读取触摸的始末位置信息
            get_inputevent(myLocation,fd);
            // // close(myPipe[0]);

            // // write(myPipe[1],myLocation,sizeof(myLocation)+sizeof(int));
            printf("s:%d %d %d %d\n",myLocation[0],myLocation[1],myLocation[2],myLocation[3]);
            sonFlag = 0;
        }
    }
}

/********************************************
*  函数功能:信号量服务函数
*  函数参数:
*           int sing:进程号
*  函数返回值:无
********************************************/
void my_sig_handler(int sing)
{
    //  触摸屏的信号量服务函数
    if(sing == SIGIO)
    {
        printf("get data\n");
        sonFlag = 1;
    }
    //  子进程 contr+c 后的服务函数
    else if(sing == SIGINT && fcid == 0)
    {
        printf("子进程结束\n");
        exit(0);
    }
    //  父进程 contr+c 后的服务函数
    else if(sing == SIGINT && fcid > 0)
    {
        printf(" 关闭父进程\n");
        //  避免产生僵尸进程  此处要等待子进程资源你释放后结束
        wait(NULL);
        exit(0);
    }
}
