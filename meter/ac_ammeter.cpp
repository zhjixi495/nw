// Copyright (c) 2011
// All rights reserved.
//
// Author:  ZhengJixiang
//

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <iostream>


#include "glog/logging.h"
#include "shared.h"
#include "Serial.hpp"
#include "ModbusConnection.hpp"
#include "PThreads.hpp"
#include "dlt645-1997Connection.h"

using namespace google;
using namespace esf;
using namespace cp;
using namespace std;


extern float  fCollectData[100];
extern unsigned char addr[6];


static bool running = true;

void MainSignalHandler(int signal_number, siginfo_t* signal_info, void* ucontext)
{
    signal_number = signal_number;
    
    if (signal_info->si_signo == SIGUSR1)
	{
		running = false;
		ucontext = ucontext;
		return;
	}
	#if defined(__arm__)
    int f = 0;
    Dl_info dlinfo;

    unsigned int **fp = 0; 
    unsigned int *lr = 0;
    unsigned int *pc = 0;

    printf("signal_info.si_signo = %d\n", signal_info->si_signo);  /* 打印信号 */
    printf("signal_info.si_errno = %d\n", signal_info->si_errno);  /* 打印信号错误码 */
    

    fp = (unsigned int **)((ucontext_t*)ucontext)->uc_mcontext.arm_fp;  /*读取异常时FP寄存器*/
    lr = (unsigned int *)((ucontext_t*)ucontext)->uc_mcontext.arm_lr;  /*读取异常时LR寄存器*/
    pc = (unsigned int *)((ucontext_t*)ucontext)->uc_mcontext.arm_pc;  /*读取异常时PC寄存器*/

    printf("Stack trace:\n");  /*开始分析堆栈数据*/

    if(!dladdr(pc, &dlinfo))  /*首先分析异常时的PC寄存器*/
    {
    	printf("Can not get symbol info\n");
    }
    else
    {
    	const char *symname_pc = dlinfo.dli_sname;
    	printf("error addr is : %p <%s+0x%x> (%s)\n", pc, symname_pc, (unsigned)((unsigned)lr - (unsigned)dlinfo.dli_saddr), dlinfo.dli_fname);  /*打印出错的语句的地址和函数名*/
    }
    
    fp = (unsigned int **)(*(fp -3));

    /*分析堆栈中的回溯结构*/
    while (fp && lr)
    {
        if (!dladdr(lr, &dlinfo))
        {
            printf("Can not get symbol info\n");
            break;
        }
        const char *symname_lr = dlinfo.dli_sname;
        ++f;
        printf("backtrace %3d : %p <%s+0x%x> (%s)\n", f, lr, symname_lr, (unsigned)((unsigned)lr - (unsigned)dlinfo.dli_saddr), dlinfo.dli_fname);  /*打印调用函数的名称和地址*/

         if ((dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main")) || (!dlinfo.dli_sname))
        {
        	break;  /*如果回到主函数，则结束分析*/
        }
        
        //计算下一帧堆栈地址信息
        lr = *(unsigned int **)(fp - 1);
        fp = (unsigned int **)(*(fp -3));
    }
#endif
    printf("End of stack trace\n");
    exit(1);
}



/*

//Name			      Port				Settings	   Timeout(ms)		   Address			Config 	          Delay
//ac_ammeter1997		ttySAC1 		9600,e,8,1 		1000			    NULL 		 ac_ammeter.pdu 		25 


argv[0]程序名称     ./ac_ammeter1997             
argv[1]设备         ttySAC1
argv[2]设置         9600,n,8,1
argv[3]超时         1000
argv[4]设备地址     NULL
argv[5]配置文件     ac_ammeter.pdu
argv[6]delay        25 
*/



int main(int argc, char* argv[])
{
    char buffer[128] = "/dev/";
    int id=-1;
    
    if (argc!=7)
    {
        printf("param error!\n");
        for(int i=0;i<argc;i++)
        {
        	printf("argv[%d]=%s \n",i,argv[i]);
        }
        printf("\n");
        return -1;
    }
    printf("\nbuild date:"__DATE__);
    printf("\nbuild time:"__TIME__);
    printf("\n");
    
    FLAGS_log_dir = "../log";
    FLAGS_max_log_size = 1;
    FLAGS_stop_logging_if_full_disk = true;
    InitGoogleLogging(argv[0]);
    SetLogDestination(INFO, "../log/ac_ammeter");
    //FLAGS_logtostderr = true;
    
    struct sigaction sig_action;
    memset(&sig_action, 0, sizeof(sig_action));
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_handler = SIG_IGN;
    sigaction(SIGHUP, &sig_action, NULL);
    sigaction(SIGINT, &sig_action, NULL);
    sigaction(SIGQUIT, &sig_action, NULL);
    sigaction(SIGPIPE, &sig_action, NULL);
    sigaction(SIGALRM, &sig_action, NULL);
    sigaction(SIGTSTP, &sig_action, NULL);
    sigaction(SIGTTIN, &sig_action, NULL);
    sigaction(SIGTTOU, &sig_action, NULL);
    sigaction(SIGURG, &sig_action, NULL);
    
    memset(&sig_action, 0, sizeof(sig_action));
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags |= SA_SIGINFO;
    sig_action.sa_sigaction = &MainSignalHandler;
    sigaction(SIGUSR1, &sig_action, NULL);
    sigaction(SIGSEGV, &sig_action, NULL);
    sigaction(SIGILL, &sig_action, NULL);
    sigaction(SIGFPE, &sig_action, NULL);
    sigaction(SIGABRT, &sig_action, NULL);
    sigaction(SIGBUS, &sig_action, NULL);
    sigaction(SIGTERM, &sig_action, NULL);
    	
	LOG(INFO) << "ac_ammeter now is starting...";
	    

	
    if(open_system_share()<0)
    {    
	    LOG(FATAL) << "Open system share fail";
	    ShutdownGoogleLogging();
	    return -1;
	}
	    
	id=register_device(AC_DEVICE);
	printf("%s id=%d\n",argv[0],id);
	if(id<0)
	{    
	    LOG(FATAL) << "Register ac_ammeter device fail";
	    ShutdownGoogleLogging();
	    return -1;
	}
	
	        
	if(open_ac_ammeter_sharemem()<0)
	{
	    LOG(FATAL) << "Can't open ac_ammeter_sharemem";
	    ShutdownGoogleLogging();
	    return -1;
	}
	    
	
	printf("register_device ok!\n");
	//设备
	strcat(buffer, argv[1]);		
	//Serial::Serial(const char* _port, const char* _settings, bool _rs485, int _timeout, int _delay)
	Serial *serial = new Serial(buffer, argv[2], false,atoi(argv[3]),atoi(argv[6]));
	if (!(serial->open()))
	{
		serial->close();
		delete serial;
		serial = 0;
		
		LOG(FATAL) << "Fails on creating the serial(" << buffer << " " << argv[2] << " " << argv[3] << "delay" << argv[6]<< ").";
		
		ShutdownGoogleLogging();		
		return -2;
	}
	
    //检查配置文件
	sprintf(buffer, "../conf/%s", argv[5]);
    std::fstream fs(buffer, std::ios::in);
    if (!fs.is_open())
    {
        std::cerr << "use default device file(" << buffer << ") for read" << std::endl;
        
        //拷贝默认文件
        sprintf(buffer, "rm -f ../conf/%s;cp ../conf/default/%s ../conf/%s", argv[5], argv[5], argv[5]);
        CHECK_GE(system(buffer), 0) << " error";
        fs.close();
    }
    fs.close();
    sprintf(buffer, "../conf/%s", argv[5]);
    	
	DLT645Connection *dlt645Connection;
    dlt645Connection = new DLT645Connection(buffer,serial,id,true);
	if(!dlt645Connection->open())
	{
	        dlt645Connection->close();
    		delete dlt645Connection;
    		dlt645Connection = 0;
    		
    		LOG(FATAL) << "Fails on creating the dlt645Connection(" << buffer<< ").";
    		
    		ShutdownGoogleLogging();
    		
    		return -3;
	}    
	
	while(1)
	{    	
	    dlt645Connection->process();	
        //检测线程是否需要退出
		if(!running)
		{
		    LOG(INFO) << "Received user stop command, ac_ammeter is now quiting...";
		    printf("ac_meter1997 exit\n");
		    sleep(1);
			break;
	   	}
	   	keepalive(id);
	   	sleep(5);
    }	
    
	ShutdownGoogleLogging();	
    return 0;
}