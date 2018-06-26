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

#ifndef _CP_MODBUSCONNECTION_HPP
#define _CP_MODBUSCONNECTION_HPP

#include "connection.h"
#include <string>



namespace charger 
{
    class Serial;
    class ModbusFrame;
    
    class ModbusConnection : public Connection
    {
    public:
        ModbusConnection(const char* _config, Serial* _serial, int _address, int _debug = 0);
        virtual ~ModbusConnection();
                                
        virtual bool open();
        virtual void close();
        virtual bool process(bool waveFlag);
        virtual const char* info();
        virtual bool init();
        
        virtual void setDebugMode(int val);
        
        virtual void getWave(WaveMap& waveMap);
        virtual int getWaveSize(WaveSizeMap& waveSizeMap);
        virtual void getData(DataMap& dataMap);
        virtual void setEvent(EventMap& eventMap);
        virtual void getEvent(EventMap& eventMap);
        virtual int getEventLevel(int index);
        virtual bool control(int index, float var, char* batch, int length,bool broadcastFlag = false);
                
    private:
        ModbusConnection(const ModbusConnection& other);
        ModbusConnection& operator=(const ModbusConnection& other);
        
        int debug;
        
        typedef struct
        {
            int id;
            int symbol;
            int value;
        }Expression;
        
        bool send(ModbusFrame* frame);
        int parseExpression(char* src, Expression** dest, int destLen, int id, int base);
        int getSymbol(char* src, char* separator, int index);
        int evalExpression(Expression** dest, int destLen, bool event = true);
        void eventProcess();
        void expandExpression(char *src, std::string &dest, int offset);
        
        typedef enum 
        { 
            STATE_CLOSE = 0,
            STATE_OPEN = 1,
            STATE_CONNECT = 2
        } State;
        
        State state;
        char config[128];
        Serial* serial;
        char information[32];
        
        typedef std::map<int, ModbusFrame*> ModbusFrameMap;
        ModbusFrameMap dataFrame;
        ModbusFrame* requestFrame;
        
        bool commState;
        int commStateId;
        int deviceStateId;
        
        int address;
        
        int crc;
        char dataFun[128];
        char* pDataFun[32];
        char dataId[128];
        char* pDataId[32];
        char dataNr[128];
        char* pDataNr[32];
        
        bool dataComm[32];        
        typedef struct
        {
            int index;
            int frmId;
            int frmIndex;
            int frmType;
            int frmScale;
            int frmBit;
            Expression* expression[32];
            float value;
        }DataSignal;
        typedef std::map<int, DataSignal*> DataSignalMap;
        DataSignalMap dataSignals;
        DataMap dataMap;
        WaveMap waveMap;
        WaveSizeMap waveSizeMap;
        
        int ctrlCid2;
        typedef struct
        {
            int cmdFunc;
            int cmdAddr;
            int cmdValue;
        }CtrlSignal;
        typedef std::map<int, CtrlSignal*> CtrlSignalMap;
        CtrlSignalMap ctrlSignals;
        
        int settingCid2;
        typedef struct
        {
            int cmdFunc;
            int cmdAddr;
            int cmdLen;
            int cmdSize;
            int cmdParam[32];
            int paramType;
            int scale;
        }SettingSignal;
        typedef std::map<int, SettingSignal*> SettingSignalMap;
        SettingSignalMap settingSignals;
        
        typedef struct
        {
            Expression* expression[32];
            int level;
            int value;
            int eventStateCount;
            double startTime;
        }EventSignal;
        typedef std::map<int, EventSignal*> EventSignalMap;
        EventSignalMap eventSignals;
        
        double commFailureTime;
    };
}

#endif /*_CP_MODBUSCONNECTION_HPP*/
