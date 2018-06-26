/*<FH>*************************************************************************
* 版权所有: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* 文件名称: 
* 文件标识: 
* 内容摘要: 
* 其它说明: 
* 当前版本: V1.0
* 作    者: 
* 完成日期: 2012-5-2 11:53:47
* 修改记录1: 
*     修改日期: 
*     版 本 号: 
*     修 改 人: 
*     修改内容:
**<FH>************************************************************************/

#ifndef _ESF_CONFIGFILE_HPP
#define _ESF_CONFIGFILE_HPP

#include <stdio.h>

namespace charger
{
    class ConfigFile
    {
    public:
        ConfigFile();
        virtual ~ConfigFile();
                        
        virtual bool open(const char* config);
        virtual void close();
        virtual bool find(const char* section);
        virtual bool read(const char* format, ...);
        
        virtual bool replace_line(const char *line);
        
        static int split(char* src, char* separator, char** dest, int destLen);
        static int split(char* src, char* separator, int* dest, int destLen, int base);
        static void del_prefix(char* string, int c);
        static void del_comment(char* string);
        
        bool add_string(const char *fieldName,const char *fieldVal);
        
    private:
        ConfigFile(const ConfigFile& other);
        ConfigFile& operator=(const ConfigFile& other);
        
        bool read_line(char* string, int size);
        bool get_valid_line(char* string, int size, int separator);
        void get_word(char* string, char* word, int size, int separator);
        
        FILE* fp;
    };
}

#endif /*_ESF_CONFIGFILE_HPP*/
