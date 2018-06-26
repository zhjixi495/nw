/*************************************************************************************************** 
                                   
  
                  版板所有 2000-2100

文件:   base_type.h    
作者:   
说明:   
修改记录：
版本：V1 20180412
修改内容：新创建



***************************************************************************************************/
#ifndef __BASETYPE_H__                   //防重包含.
#define __BASETYPE_H__
/**************************************************************************************************/

#define PRINTF_LINE printf("file=%s line=%d fun=%s\n",__FILE__,__LINE__,__FUNCTION__);

//定义规范化的,通用的标准数据类型.

typedef unsigned char uint8_t;
typedef signed char int8_t;

typedef unsigned short uint16_t;
typedef signed short int16_t;

typedef unsigned int uint32_t;
typedef signed int int32_t;



//常用的枚举类型.
typedef enum
{
    BIT_RESET,
    BIT_SET
}BIT;

typedef enum
{ 
    BOOL_FALSE = 0,
    BOOL_TRUE = 1,
}BOOL;

typedef enum    //数据类型.
{
    DATA_TYPE_INT = 0,
    DATA_TYPE_STRING,//字符数组
    DATA_TYPE_STRING_ADDR,
    DATA_TYPE_FLOAT,
    DATA_TYPE_CHAR,
    DATA_TYPE_UCHAR,
    DATA_TYPE_INT16U,
    DATA_TYPE_INT16S,
    DATA_TYPE_BIGINT16U,//16位大端
    DATA_TYPE_BIGINT32U,//32位大端
    DATA_TYPE_INT_STRING,//整形数组
    DATA_TYPE_ENUM
}DATA_TYPE;

union UnionInt32
{
    unsigned int all;
    struct
    {
        unsigned int  bit0:1;
        unsigned int  bit1:1;
        unsigned int  bit2:1;
        unsigned int  bit3:1;
        unsigned int  bit4:1;
        unsigned int  bit5:1;
        unsigned int  bit6:1;
        unsigned int  bit7:1;
        unsigned int  bit8:1;
        unsigned int  bit9:1;
        unsigned int  bit10:1;
        unsigned int  bit11:1;
        unsigned int  bit12:1;
        unsigned int  bit13:1;
        unsigned int  bit14:1;
        unsigned int  bit15:1;
        unsigned int  bit16:1;
        unsigned int  bit17:1;
        unsigned int  bit18:1;
        unsigned int  bit19:1;
        unsigned int  bit20:1;
        unsigned int  bit21:1;
        unsigned int  bit22:1;
        unsigned int  bit23:1;
        unsigned int  bit24:1;
        unsigned int  bit25:1;
        unsigned int  bit26:1;
        unsigned int  bit27:1;
        unsigned int  bit28:1;
        unsigned int  bit29:1;
        unsigned int  bit30:1;
        unsigned int  bit31:1;
    }bit;
};

#define BITVALUE(x) (1<<x)

#define EVENT_SET(value,x) (value |= x)
#define EVENT_CLR(value,x) (value &= ~x)
#define EVENT_CHK(value,x) (value & x)

#ifdef __cplusplus
    extern "C" {
#endif


extern void cur_version_info(char *stringversion);

#ifdef __cplusplus
    }
#endif

/**************************************************************************************************/
#endif  //#ifndef __BASETYPE_H__
/**************************************************************************************************/
/***************************************************************************************************
文件结束.
***************************************************************************************************/
