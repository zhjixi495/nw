/*************************************************************************************************** 
                                   
  
                  版板所有 2000-2100

文件:   base_type.c    
作者:   
说明:   

修改记录：
版本：V1 20180412
修改内容：新创建


***************************************************************************************************/
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "base_type.h"

void cur_version_info(char *stringversion)
{
    printf("Build Time:%s  %s\n",__DATE__,__TIME__);
    printf("version:%s\n",stringversion);
}



/***************************************************************************************************
文件结束.
***************************************************************************************************/


