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
    {0,"�����ļ�","Ԥ��",50,0,1000,80,"sdf1sadfds"},
    {1,"�ƷѲ���","����1",50,0,1000,80},
    {2,"�ƷѲ���","����2",60,0,1000,80},
    {3,"�ƷѲ���","����3",60,0,1000,80},
    {4,"�ƷѲ���","����4",60,0,1000,80},
    {5,"�ƷѲ���","����5",60,0,1000,80},
    
    {6,"����������","��Ƭ����",0,0,10,2},
    {7,"����������","����������",0,0,10,0},
    {8,"����������","������",0,0,115200,115200},
    
    {9,"��̨ͨ�Ų���","Э������",0,0,10,5},
    {10,"��̨ͨ�Ų���","�ϱ����",0,0,10,1},
    {11,"�ƷѲ���","����6",51,0,1000,62},
    {12,"����������","�Ʒ�",0,0,10,2},
    {13,"����������","�Ʒ�2",0,0,10,2},
    {14,"����������","�Ʒ�3",0,0,10,2,"for test"},
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
            printf("��������%d:%d ���ݱ��޸�\n",i,value);
        }
        else if(ret == RET_OK)
        {
            printf("��������%d:%d �ɹ�\n",i,value);
        }
        else if(ret == RET_ERR)
        {
            printf("��������%d:%d ��ȡʧ��\n",i,value);
        }
    }
    
    value = getIntValue(8,ret)+1;
    
    sprintf(tempstring,"%d",value);
    sValue = string(tempstring);
    iniFile.setValue(configInit[8].section,configInit[8].key, sValue);
    
    //if (modifyFlag)
    {
        printf("\n----------�ļ����޸ģ����浽�ļ�-------------\n");
        //���浽�ļ�  
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
    
    //��ȡָ���ε�ָ�����ֵ  
    ret = iniFile.getValue("COMMON","DB",value);  
    printf("ret=%d value=%s\n",ret,value.c_str());
    
    //������ֵ��ע��  
    iniFile.setValue("TEST","NAME","root","�û�����111");  
    
    ret = iniFile.getValue("sys_table","max_column_id",value);  
    printf("max_column_id ret=%d value=%d\n",ret,atoi(value.c_str()));
    
    ret = iniFile.getValue("sys_table","baudrads",value); 
    int idata =  atoi(value.c_str());
    printf("baudrads ret=%d value=%d\n",ret,idata);
    
    ret = iniFile.getValue("sys_table","serverIp",value);  
    printf("serverIp ret=%d value=%s\n",ret,value.c_str());
    value.append("1");
    iniFile.setValue("sys_table","serverIp",value,"������IP"); 
    
    ret = iniFile.getValue("sys_table","serverIp2",value);  
    printf("serverIp2 ret=%d value=%s\n",ret,value.c_str());
    
    ret = iniFile.getValue("sys_table","http",value);
    if(ret !=0)
    {
        sprintf(tempstring,"http://www.baidu.com/%d",50);
        value = string(tempstring);
        iniFile.setValue("sys_table","http", value,"��ά��");
    }

    
    
    sprintf(tempstring,"%d",idata+1);
    value = string(tempstring);
    iniFile.setValue("sys_table","baudrads", value,"������");
    iniFile.setValue("sys_table","baudrads2", value,"������2");
    
    iniFile.getIntValue("sys_table","baudrads2", ret,0,10000,30);
    iniFile.getIntValue("sys_table22","baudrads3", ret,0,10000,30);
    iniFile.getIntValue("sys_table22","baudrads4", ret,0,10000,30);
    iniFile.getIntValue("sys_table22","baudrads5", ret,0,10000,500);
    iniFile.setSectionComment("sys_table22","���Բ�����");
    //���浽�ļ�  
    iniFile.save();  

    return 0;
}
*/