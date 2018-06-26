/*<FH>*************************************************************************
* ��Ȩ����: Copyright (C) 2007-2012, Sinexcel Electric Co., Ltd.
* �ļ�����: 
* �ļ���ʶ: 
* ����ժҪ: 
* ����˵��: 
* ��ǰ�汾: V1.0
* ��    ��: 
* �������: 2012-5-2 11:53:47
* �޸ļ�¼1: 
*     �޸�����: 
*     �� �� ��: 
*     �� �� ��: 
*     �޸�����:
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
