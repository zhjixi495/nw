#include<stdio.h>
#include<iostream>
#include "inifile.h"
#include<string.h>
#include<math.h>
#include <stdlib.h>
#include <sstream>
#include <ctype.h>

using namespace inifile;

struct ConfigInit {
    int indexId;
    string section;
    string key;
    int value;
    int minValue;
    int maxValue;
    int defaultValue;
    string comment;
    int ret;
};

struct ConfigInit configInit[]=
{
    {0,"配置文件","预留",50,0,1000,80,"sdf1sadfds"},
    {1,"计费部分","费率1",50,0,1000,80},
    {2,"计费部分","费率2",60,0,1000,80},
    {3,"计费部分","费率3",60,0,1000,80},
    {4,"计费部分","费率4",60,0,1000,80},
    {5,"计费部分","费率5",60,0,1000,80},
    
    {6,"读卡器部分","卡片类型",0,0,10,2},
    {7,"读卡器部分","读卡器类型",0,0,10,0},
    {8,"读卡器部分","波特率",0,0,115200,115200},
    
    {9,"后台通信部分","协议类型",0,0,10,5},
    {10,"后台通信部分","上报间隔",0,0,10,1},
    {11,"计费部分","费率6",51,0,1000,62},
    {12,"读卡器部分","计费",0,0,10,2},
    {13,"读卡器部分","计费2",0,0,10,2},
    {14,"读卡器部分","计费3",0,0,10,2,"for test"},
};

int getIntValue(int index,int &ret)
{
    ret = -1;
    for(int i= 0;i<sizeof(configInit)/sizeof(ConfigInit);i++)
    {
        if (index == configInit[i].indexId)
        {
            ret = configInit[i].ret;
            return configInit[i].value;
        }
    }
    
    
    return 0;
}

int main(int argc, char *argv[])
{
    IniFile iniFile;
    int index = 0;
    bool modifyFlag=0;
    int ret,value;
    string sValue;
    char tempstring[128];
    
    iniFile.load("a2.txt");
    
    for(int i= 0;i<sizeof(configInit)/sizeof(ConfigInit);i++)
    {
        configInit[i].value = iniFile.getIntValue(configInit[i].section,configInit[i].key, configInit[i].ret,configInit[i].minValue,configInit[i].maxValue,configInit[i].defaultValue,configInit[i].comment);
        
        if(RET_MODIFY==configInit[i].ret)
        {
            modifyFlag = true;
        }
    }
    
    for(int i= 0;i<sizeof(configInit)/sizeof(ConfigInit);i++)
    {
        value = getIntValue(i,ret);
        
        if(ret == RET_MODIFY)
        {
            configInit[i].ret = RET_OK;
            printf("数据索引%d:%d 数据被修改\n",i,value);
        }
        else if(ret == RET_OK)
        {
            printf("数据索引%d:%d 成功\n",i,value);
        }
        else if(ret == RET_ERR)
        {
            printf("数据索引%d:%d 获取失败\n",i,value);
        }
    }
    
    value = getIntValue(8,ret)+1;
    
    sprintf(tempstring,"%d",value);
    sValue = string(tempstring);
    iniFile.setValue(configInit[8].section,configInit[8].key, sValue);
    
    //if (modifyFlag)
    {
        printf("\n----------文件被修改，保存到文件-------------\n");
        //保存到文件  
        iniFile.save();  
    }

    return 0;
}

/*
int main(int argc, char *argv[])
{
    IniFile iniFile;
    
    iniFile.load("aa.txt");  
    iniFile.print();
    string value;
    char tempstring[128];
    int ret;
    
    //获取指定段的指定项的值  
    ret = iniFile.getValue("COMMON","DB",value);  
    printf("ret=%d value=%s\n",ret,value.c_str());
    
    //设置新值和注释  
    iniFile.setValue("TEST","NAME","root","用户名称111");  
    
    ret = iniFile.getValue("sys_table","max_column_id",value);  
    printf("max_column_id ret=%d value=%d\n",ret,atoi(value.c_str()));
    
    ret = iniFile.getValue("sys_table","baudrads",value); 
    int idata =  atoi(value.c_str());
    printf("baudrads ret=%d value=%d\n",ret,idata);
    
    ret = iniFile.getValue("sys_table","serverIp",value);  
    printf("serverIp ret=%d value=%s\n",ret,value.c_str());
    value.append("1");
    iniFile.setValue("sys_table","serverIp",value,"服务器IP"); 
    
    ret = iniFile.getValue("sys_table","serverIp2",value);  
    printf("serverIp2 ret=%d value=%s\n",ret,value.c_str());
    
    ret = iniFile.getValue("sys_table","http",value);
    if(ret !=0)
    {
        sprintf(tempstring,"http://www.baidu.com/%d",50);
        value = string(tempstring);
        iniFile.setValue("sys_table","http", value,"二维码");
    }

    
    
    sprintf(tempstring,"%d",idata+1);
    value = string(tempstring);
    iniFile.setValue("sys_table","baudrads", value,"波特率");
    iniFile.setValue("sys_table","baudrads2", value,"波特率2");
    
    iniFile.getIntValue("sys_table","baudrads2", ret,0,10000,30);
    iniFile.getIntValue("sys_table22","baudrads3", ret,0,10000,30);
    iniFile.getIntValue("sys_table22","baudrads4", ret,0,10000,30);
    iniFile.getIntValue("sys_table22","baudrads5", ret,0,10000,500);
    iniFile.setSectionComment("sys_table22","测试波特率");
    //保存到文件  
    iniFile.save();  

    return 0;
}
*/