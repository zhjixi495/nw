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

#include "config_file.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>


namespace charger 
{
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    ConfigFile::ConfigFile()
    {
        fp = 0;
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    ConfigFile::~ConfigFile()
    {
        close();
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    bool ConfigFile::open(const char* config)
    {
        if ((fp != 0) || (config == 0) || (strlen(config) <= 0))
        {
            return false;
        }
        
        fp = fopen(config, "r+");
        if (fp == 0)
        {
            return false;
        }    
        fseek (fp, 0, SEEK_SET);
        
        return true;
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    void ConfigFile::close()
    {
        if (fp != 0)
        {
            fclose(fp);
            fp = 0;
        }
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    bool ConfigFile::find(const char* section)
    {
        if ((fp == 0) || (section == 0) || (strlen(section) <= 0))
        {
            printf("ConfigFile::find fp is error \r\n");
            return false;
        }
        
        char string[512], word[512];
                
        while (true)
        {
            if (!get_valid_line(string, sizeof(string), 0x09))
            {
                printf("ConfigFile::find get_valid_line %s is error \r\n", section);
                return false;
            }
            
            get_word(string, word, sizeof(word), 0x09);
            
            if (strcmp(word, section) == 0)
            {
                return true;
            }
        }
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    bool ConfigFile::read(const char* format, ...)
    {
        if ((fp == 0) || (format == 0) || (strlen(format) <= 0))
        {
            printf("ConfigFile::read fp is error \r\n");
            return false;
        }
        
        char string[512], word[512], tokenWord[512];
        if (!get_valid_line(string, sizeof(string), 0x09))
        {
            printf("ConfigFile::read get_valid_line is error \r\n");
            return false;
        }
        
        va_list arg_ptr;    
        va_start(arg_ptr, format);
        
        char* pString;
        int* pInt;
        float* pDouble;
        int i = 0;
        bool result = true;
        while (format[i] != 0)
        {
            if (format[i] == '%')
            {
                get_word(string, word, sizeof(word), 0x09);
                i++;
            }
            
            if (format[i] == '/') 
            {
                i++;
                if (format[i] == 'd') 
                {
                    pInt = va_arg(arg_ptr, int*);
                    if (pInt == 0)
                    {
                        printf("ConfigFile::read pInt d is error \r\n");
                        result = false;
                        break;
                    }
                    
                    get_word(word, tokenWord, sizeof(tokenWord), '/');
                    
                    *pInt = atoi(tokenWord);
                }
                else if (format[i] == 'x')
                {
                    pInt = va_arg(arg_ptr, int*);
                    if (pInt == 0)
                    {
                        printf("ConfigFile::read pInt x is error \r\n");
                        result = false;
                        break;
                    }
                    
                    get_word(word, tokenWord, sizeof(tokenWord), '/');
                    *pInt = (int)strtol(tokenWord, 0, 16);
                }
                else if (format[i] == 'f') 
                {
                    pDouble = va_arg(arg_ptr, float*);
                    if (pDouble == 0)
                    {
                        printf("ConfigFile::read pInt f is error \r\n");
                        result = false;
                        break;
                    }
        
                    get_word(word, tokenWord, sizeof(tokenWord), '/');
                    *pDouble = (float )atof(tokenWord);
                }
                else if (format[i] == 's') 
                {
                    pString = va_arg(arg_ptr, char*);
                    if (pString == 0)
                    {
                        printf("ConfigFile::read pInt s is error \r\n");
                        result = false;
                        break;
                    }
                    
                    get_word(word, tokenWord, sizeof(tokenWord), '/');
                    strcpy(pString, tokenWord);
                }
            }
            else if (format[i] == 'd') 
            {
                pInt = va_arg(arg_ptr, int*);
                if (pInt == 0)
                {
                    printf("ConfigFile::read pInt dd is error \r\n");
                    result = false;
                    break;
                }
        
                *pInt = atoi(word);
            }
            
            else if (format[i] == 'x') 
            {
                pInt = va_arg(arg_ptr, int*);
                if (pInt == 0)
                {
                    printf("ConfigFile::read pInt xx is error \r\n");
                    result = false;
                    break;
                }
                
                *pInt = (int)strtol(word, 0, 16);
            }
            else if (format[i] == 'f') 
            {
                pDouble = va_arg(arg_ptr, float*);
                if (pDouble == 0)
                {
                    printf("ConfigFile::read pInt ff is error \r\n");
                    result = false;
                    break;
                }
        
                *pDouble = (float)atof(word);
            }
            else if (format[i] == 's') 
            {
                pString = va_arg(arg_ptr, char*);
                if (pString == 0)
                {
                    printf("ConfigFile::read pInt ss is error \r\n");
                    result = false;
                    break;
                }
                            
                strcpy(pString, word);
            }
        
            i++;
        }
        
        va_end(arg_ptr);
        return result;
    }
    
        /*<FUNC>***********************************************************************
    * 函数名称:
    * 功能描述:
    * 输入参数:
    * 输出参数:
    * 返 回 值:
    * 作    者:
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录:
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    bool ConfigFile::replace_line(const char *line)
    {
        char get_str[512];
        int get_lenth;
        int line_lenth;
        int current_location;
        read_line(get_str,sizeof(get_str));
        get_lenth = strlen(get_str);
        line_lenth = strlen(line);
        if (get_lenth>=line_lenth)
        {
            memset(get_str,32,get_lenth);
            strcpy(get_str,line);
            if (get_lenth>line_lenth)
            {
                get_str[line_lenth]=' ';
            }
            current_location = ftell(fp);
            fseek(fp,current_location-get_lenth-1,SEEK_SET);
            fprintf(fp,"%s",get_str);
        }
        else
        {
            printf("The input line is too long!");
            return false;
        }
        return true;
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    int ConfigFile::split(char* src, char* separator, char** dest, int destLen)
    {
        if ((src == 0) || (strlen(src) <= 0) || (separator == 0) || (strlen(separator) <= 0) || (dest == 0) || (destLen <= 0))
        {            
            return -1;
        }
                
        int size, i, length = strlen(separator);
        dest[0] = src;
        for (size = 1; *src; src++) 
        {
            for (i = 0; i < length; i++)
            {
                if (*src == separator[i])
                {
                    *src = 0;
                    if (size >= destLen)
                    {
                        return size;
                    }
                    dest[size++] = src + 1;
                    break;
                }
            }
        }
        
        dest[size] = 0;
        
        return size;
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    int ConfigFile::split(char* src, char* separator, int* dest, int destLen, int base)
    {
        if ((src == 0) || (strlen(src) <= 0) || (separator == 0) || (strlen(separator) <= 0) || (dest == 0) 
            || (destLen <= 0) || (base <= 0))
        {
            return -1;
        }
        
        memset(dest, 0, destLen);
        int size, i, length = strlen(separator);
        char* string = src;
        for (size = 0; *src; src++) 
        {
            for (i = 0; i < length; i++)
            {
                if (*src == separator[i])
                {
                    *src = 0;
                    dest[size++] = (int)strtol(string, 0, base);
                    
                    if (size >= (int)(destLen / sizeof(int)))
                    {
                        return size;
                    }
                    
                    string = src + 1;
                    break;
                }
            }
        }

        if (*src == 0)
        {
            dest[size++] = (int)strtol(string, 0, base);
        }
        
        return size;
    }

    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    void ConfigFile::del_prefix(char* string, int c)
    {
        if ((string == 0) || (strlen(string) <= 0))
        {
            return;
        }
        
        int i, pos, length = strlen(string);
        for (i = 0, pos = 0; i < length; i++)
        {
            if ((string[i] == c) || (string[i] == '\r'))
            {
                pos++;
            }
            else
            {
                break;
            }
        }
        
        for (i = pos; i < length; i++)
        {
            string[i - pos] = string[i];
        }
        
        string[i - pos] = 0;
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    void ConfigFile::del_comment(char* string)
    {
        if ((string == 0) || (strlen(string) <= 0))
        {
            return;
        }
        
        int length = strlen(string);
        for (int i = 0; i < length - 1; i++)
        {
            if ((string[i] == '/') && (string[i + 1] == '/'))
            {
                string[i] = 0;
                break;
            }
        }
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    bool ConfigFile::read_line(char* string, int size)
    {
        if ((fp == 0) || (string == 0) || (size <= 0))
        {
            printf("ConfigFile::read_line fp is error \r\n");
            return false;
        }
        
        char ch;
        int i = 0;
        
        while (true)
        {
            //一个字符一个字符地读
            if (fread(&ch, 1, 1, fp) != 1)
            {
                //一个字符都没读取到
                if (i == 0)
                    return false ;
            
                //读完了
                break ;
            }
    
            //0x0d+0x0a=回车
            if (ch == '\r')
                continue ;
            if (ch == '\n')
                break ;
            
            else
            {
                if (i < size)
                    string[i++] = ch ;
                else
                    return false;
            }
        }
            
        //结尾符
        string[i] = 0 ;
    
        return true;
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    bool ConfigFile::get_valid_line(char* string, int size, int separator)
    {
        if ((fp == 0) || (string == 0) || (size <= 0) || (separator <= 0))
        {
            printf("ConfigFile::get_valid_line fp is error \r\n");
            return false;
        }
        
        memset(string, 0, size);
        
        while (true)
        {
            if (fscanf(fp, "%[^\n]\n", string) < 0)
            //if (!(read_line(string, size)))
            {
                printf("ConfigFile::get_valid_line fscanf is error \r\n");
                return false;
            }
            
            //删除分隔符
            del_prefix(string, separator);
                        
            //删除注释
            del_comment(string);
            
            if (strlen(string) <= 0)
            {
                continue;
            }
            
            return true;
        }
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2008-12-30 17:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    void ConfigFile::get_word(char* string, char* word, int size, int separator)
    {
        if ((fp == 0) || (string == 0) || (strlen(string) <= 0) || (word == 0) || (size <= 0) || (separator <= 0))
        {
            return;
        }
        
        memset(word, 0, size);
        
        //删除分隔符
        del_prefix(string, separator);
        
        //查找分隔符
        int i, pos, length = strlen(string);
        for (i = 0, pos = length; i < length; i++)
        {
            if ((string[i] == separator) || (string[i] == '\r'))
            {
                pos = i;
                break;
            }
        }
        
        //获得第一个字
        strncpy(word, string, pos);
        word[pos] = 0;
        
        //删除第一个字
        for (i = pos; i < length; i++)
        {
            string[i - pos] = string[i];
        }
        string[i - pos] = 0;
    }
    
    /*<FUNC>***********************************************************************
    * 函数名称: 
    * 功能描述: 
    * 输入参数: 
    * 输出参数: 
    * 返 回 值: 
    * 作    者: 
    * 完成日期: 2014-09-04 16:28
    * 版    本: V1.0
    * 修改记录: 
    *     修改日期         版 本         修改人               修改摘要
    * -------------------------------------------------------------------------
    **<FUNC>**********************************************************************/
    bool ConfigFile::add_string(const char *fieldName,const char *fieldVal)
    {
        if ((fp == 0) || (fieldName == 0)|| (fieldVal == 0))
        {
            return false;
        }
        
        int len = strlen(fieldName);
        
        if(len > 128)
        {
            return false;
        }
        
        len = strlen(fieldVal);
        
        if(len > 128)
        {
            return false;
        }
        
        char nstring[256];
        
        fseek (fp, 0, SEEK_END);
        sprintf(nstring,"\n%s\n",fieldName);
        fprintf(fp,"%s",nstring);

        sprintf(nstring,"%s                    \n",fieldVal);
        fprintf(fp,"%s",nstring);
    
        return true;
    }
}

