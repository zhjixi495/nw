#ifndef DLT645_1997_FRAME_H__
#define DLT645_1997_FRAME_H__

#define FRAME_START_CHAR     0x68 //帧起始字符
#define FRAME_END_CHAR       0x16 //帧结束字符
#define ADD_VALUE_CHAR       0x33 //数据域加33H处理


struct dlt6451997_ammeter_tx_frame
{
    unsigned char startch[4];
    unsigned char sync1;
    unsigned char addr[6];
    unsigned char sync2;
    unsigned char control;
    unsigned char len;
    unsigned char data[2];
    unsigned char sum;
    unsigned char endsync;
};

struct Dlt645_1997_head
{
    unsigned char sync1;
    unsigned char addr[6];
    unsigned char sync2;
    unsigned char ctrl;
    unsigned char len;
};


#define BCD_TO_BIN(val) ((val)=(unsigned char)(((val)&15) + ((val)>>4)*10))
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)

#endif  // DLT645_1997_FRAME_H__