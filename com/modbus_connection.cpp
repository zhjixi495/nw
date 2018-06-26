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
#include <sys/time.h>
#include "modbus_connection.h"
#include "config_file.h"
#include "serial.h"
#include "modbus_constants.h"
#include "modbus_frame.h"
#include "timer.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>





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
    ModbusConnection::ModbusConnection(const char* _config, Serial* _serial, int _address, int _debug)
    {
        state = STATE_CLOSE;
        
        debug = _debug;
        
        memset(config, 0, sizeof(config));
        strcpy(config, _config);
        serial = _serial;
        memset(information, 0, sizeof(information));
        strcpy(information, "Modbus Protocol");
        
        dataFrame.clear();
        requestFrame = new ModbusFrame();
        
        commState = false;
        commStateId = -1;
        deviceStateId = -1;
        
        address = _address;
        crc = 1;
        memset(dataFun, 0, sizeof(dataFun));
        memset(pDataFun, 0, sizeof(pDataFun));
        memset(dataId, 0, sizeof(dataId));
        memset(pDataId, 0, sizeof(pDataId));
        memset(dataNr, 0, sizeof(dataNr));
        memset(pDataNr, 0, sizeof(pDataNr));
        memset(dataComm, 0, sizeof(dataComm));
        
        dataSignals.clear();
        dataMap.clear();
        waveMap.clear();
        waveSizeMap.clear();
        
        ctrlCid2 = 0;
        ctrlSignals.clear();
        
        settingCid2 = 0;
        settingSignals.clear();
        
        eventSignals.clear();
        
        commFailureTime = 10;
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
    ModbusConnection::~ModbusConnection()
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
    bool ModbusConnection::open()
    {
        if (state != STATE_CLOSE)
        {
            return false;
        }
        
        ConfigFile configFile;
        if (!configFile.open(config))
        {
            return false;
        }
        
        //数据扩展属性
        int dataStartId[8];
        int dataEndId[8];
        int dataExpandNum[8];
        int dataExpandType[8];
        for(int i = 0; i < 8; i++)
        {
            dataStartId[i] = -1;
            dataEndId[i] = -1;
            dataExpandNum[i] = -1;
            dataExpandType[i] = -1;    
        }
        int num;
        configFile.find("[DATA_EXPAND_NUM]");
        if (!configFile.read("%d", &num))
        {
            return false;
        }
        configFile.find("[DATA_EXPAND]");
        for (int i = 0; i < num; i++)
        {
            if (!configFile.read("%d%d%d%d", &dataStartId[i], &dataEndId[i], &dataExpandNum[i], &dataExpandType[i]))
            {
                return false;
            }
        }
        
        //实时数据
        configFile.find("[DATA_SIGNAL_INFO]");
        if (!configFile.read("%s%s%s%d", dataFun, dataId, dataNr, &crc))
        {
            return false;
        }
        if (ConfigFile::split(dataFun, (char *)(char *)"/", pDataFun, sizeof(pDataFun)) <= 0)
        {
            return false;
        }
        if (ConfigFile::split(dataId, (char *)(char *)"/", pDataId, sizeof(pDataId)) <= 0)
        {
            return false;
        }
        if (ConfigFile::split(dataNr, (char *)(char *)"/", pDataNr, sizeof(pDataNr)) <= 0)
        {
            return false;
        }
        int frameCount = 0;
        for (int i = 0; i < (int)(sizeof(pDataFun)); i++)
        {
            if (pDataFun[i] == 0)
            {
                break;
            }
            
            frameCount++;
            
            dataFrame[i] = new ModbusFrame();
        }
                
        configFile.find("[DATA_SIGNAL_NUM]");
        if (!configFile.read("%d", &num))
        {
            return false;
        }
                
        int id;
        char localName[128];
        char expression[256];
        
        //建立一个id和字符串映射的map
        std::map<int, std::string> expressionMap;
        DataSignal* dataSignal;
        configFile.find("[DATA_SIGNAL]");
        for (int i = 0; i < num; i++)
        {
            dataSignal = new DataSignal;
            
            dataSignal->value = 0;
            if (!configFile.read("%d%d%s%/d/d/d/d%s", &id, &(dataSignal->index), localName, &(dataSignal->frmId), 
                &(dataSignal->frmIndex), &(dataSignal->frmType), &(dataSignal->frmScale), expression))
            {
                return false;
            }
            expressionMap[id] = expression;
            //如果帧中数据类型frmType为1，表示位类型，此时精度frmScale应为frmBit
            if ((dataSignal->frmType == 1)||(dataSignal->frmType == 7))
            {
                dataSignal->frmBit = dataSignal->frmScale;
                dataSignal->frmScale = 1;
            }
            else
            {
                dataSignal->frmBit = -1;
            }
                        
            dataSignals[id] = dataSignal;
            if ((dataSignal->index) >= 0)
            {
                dataMap[dataSignal->index] = 0;
            }
            /*
            //倒数第二个数据为设备通讯状态
            if (i == (num - 2))
            {
                commStateId = id;
                printf("before expand----commStateId:%d ", commStateId);
            }
            
            //最后一个数据为设备状态
            if (i == (num - 1))
            {
                deviceStateId = dataSignal->index;
                printf("deviceStateId:%d\n", deviceStateId);
            }
            */
            //解析表达式
            parseExpression(expression, dataSignal->expression, sizeof(dataSignal->expression), id, 16);
        }
        
        //根据扩展属性扩展配置文件内容,扩展DATA_SIGNAL_INFO字段内容
        for(int i = 0; i < 8; i++)
        {
            if((dataStartId[i] != -1) && (dataEndId[i] != -1) && (dataExpandNum[i] != -1) && (dataExpandType[i] != -1))
            {
                int expandOffset = dataEndId[i] - dataStartId[i]+1;
                for(int j = 1; j <= dataExpandNum[i]; j++)
                {
                    for(int k = dataStartId[i]; k <= dataEndId[i]; k++)
                    {
                        DataSignal *expandDataSignal = dataSignals[k];
//                        printf("get--id:%d index:%d frmId:%d frmIndex:%d\n", k, expandDataSignal->index, expandDataSignal->frmId, expandDataSignal->frmIndex);
                        dataSignal = new DataSignal;
                        dataSignal->value = 0;
                        if(dataExpandType[i] == 0)    //有不同的扩展规则，通过类型来区分
                        {
                            //type=0,扩展index和frmId字段
                            dataSignal->index = expandDataSignal->index+expandOffset*j;
                            dataSignal->frmId = expandDataSignal->frmId+j;
                            dataSignal->frmIndex = expandDataSignal->frmIndex;
                            dataSignal->frmType = expandDataSignal->frmType;
                            dataSignal->frmBit = expandDataSignal->frmBit;
                            dataSignal->frmScale = expandDataSignal->frmScale;
                            
                            char tmpExpression[256];
                            sprintf(tmpExpression, "%s", expressionMap[k].c_str());
                            
                            //解析表达式
                            parseExpression(tmpExpression, dataSignal->expression, sizeof(dataSignal->expression), k, 16);
                        }
                        else if(dataExpandType[i] == 1)    //有不同的扩展规则，通过类型来区分
                        {
                            //type=1,扩展index和frmIndex
                            dataSignal->index = expandDataSignal->index+expandOffset*j;    
                            dataSignal->frmId = expandDataSignal->frmId;
                            dataSignal->frmIndex = expandDataSignal->frmIndex+2*j;
                            dataSignal->frmType = expandDataSignal->frmType;
                            dataSignal->frmBit = expandDataSignal->frmBit;
                            dataSignal->frmScale = expandDataSignal->frmScale;
                            
                            char tmpExpression[256];
                            sprintf(tmpExpression, "%s", expressionMap[k].c_str());
                            
                            //解析表达式
                            parseExpression(tmpExpression, dataSignal->expression, sizeof(dataSignal->expression), k, 16);
                        }
                        else if(dataExpandType[i] == 2)    //有不同的扩展规则，通过类型来区分
                        {
                            //type=2,扩展index,frmIndex和表达式ID
                            if(expandDataSignal->index == -1)
                            {
                                dataSignal->index = -1;    
                            }
                            else
                            {
                                dataSignal->index = expandDataSignal->index+4*j;        
                            }
                            dataSignal->frmId = expandDataSignal->frmId;
                            
                            if(expandDataSignal->frmIndex == -1)
                            {
                                dataSignal->frmIndex = -1;
                            }
                            else
                            {
                                dataSignal->frmIndex = expandDataSignal->frmIndex+2*j;
                            }
                            dataSignal->frmType = expandDataSignal->frmType;
                            dataSignal->frmBit = expandDataSignal->frmBit;
                            dataSignal->frmScale = expandDataSignal->frmScale;
                            
                            std::string tmpExpression;
                            if(strcmp((char *)expressionMap[k].c_str(), "-1") == 0)
                            {
                                tmpExpression = "-1";
                            }
                            else
                            {
                                expandExpression((char *)expressionMap[k].c_str(), tmpExpression, j*12);
                            }
                            
                            //解析表达式
                            char tmpExpression1[128];
                            sprintf(tmpExpression1, "%s", tmpExpression.c_str());
                            parseExpression((char *)tmpExpression1, dataSignal->expression, sizeof(dataSignal->expression), k, 16);
                        }
//                        printf("add--id:%d index:%d frmId:%d frmIndex:%d\n", k+expandOffset*j, dataSignal->index, dataSignal->frmId, dataSignal->frmIndex);
                        dataSignals[k+expandOffset*j] = dataSignal;
                        
                        if ((dataSignal->index) >= 0)
                        {
                            dataMap[dataSignal->index] = 0;
                        }
                    }
                }
            }    
        }
        
        //第一个数据为设备通讯状态 modify by zjx
        commStateId = 0;
        
        //第二个数据为设备状态 modify by zjx
        deviceStateId = 1;
        //printf("after expand----commStateId:%d deviceStateId:%d\n", commStateId, deviceStateId);
                
        //控制命令
        configFile.find("[CTRL_SIGNAL_INFO]");
        if (!configFile.read("%x", &ctrlCid2))
        {
            return false;
        }
                
        configFile.find("[CTRL_SIGNAL_NUM]");
        if (!configFile.read("%d", &num))
        {
            return false;
        }
        
        int index;
        CtrlSignal* ctrlSignal;
        configFile.find("[CTRL_SIGNAL]");
        for (int i = 0; i < num; i++)
        {
            ctrlSignal = new CtrlSignal;
            
            if (!configFile.read("%d%d%s%x%x%x", &id, &index, localName, &(ctrlSignal->cmdFunc), &(ctrlSignal->cmdAddr), &(ctrlSignal->cmdValue)))
            {
                return false;
            }
            
            ctrlSignals[index] = ctrlSignal;
        }
        
        //设置命令
        configFile.find("[SETTING_SIGNAL_INFO]");
        if (!configFile.read("%x", &settingCid2))
        {
            return false;
        }
        
        configFile.find("[SETTING_SIGNAL_NUM]");
        if (!configFile.read("%d", &num))
        {
            return false;
        }
        
        char cmdParam[128];
        SettingSignal* settingSignal;
        configFile.find("[SETTING_SIGNAL]");
        for (int i = 0; i < num; i++)
        {
            settingSignal = new SettingSignal;
            
            if (!configFile.read("%d%d%s%x%x%d%s%d%d", &id, &index, localName, &(settingSignal->cmdFunc), &(settingSignal->cmdAddr),
                &(settingSignal->cmdLen), cmdParam, &(settingSignal->paramType), &(settingSignal->scale)))
            {
                return false;
            }
            
            //解析命令参数
            if(strcmp(cmdParam, "-1") == 0)
            {
                settingSignal->cmdSize = 0;    
            }
            else
            {
                settingSignal->cmdSize = ConfigFile::split(cmdParam, (char *)"/", settingSignal->cmdParam, sizeof(settingSignal->cmdParam), 10);
            }
            
            settingSignals[index] = settingSignal;
        }

        //事件扩展属性
        int eventStartId[8];
        int eventEndId[8];
        int eventExpandNum[8];
        int eventExpandType[8];
        int eventJudgeIdOffset[8];
        for(int i = 0; i < 8; i++)
        {
            eventStartId[i] = -1;
            eventEndId[i] = -1;
            eventExpandNum[i] = -1;
            eventExpandType[i] = -1;
            eventJudgeIdOffset[i] = -1;    
        }
        configFile.find("[EVENT_EXPAND_NUM]");
        if (!configFile.read("%d", &num))
        {
            return false;
        }
        configFile.find("[EVENT_EXPAND]");
        for (int i = 0; i < num; i++)
        {
            if (!configFile.read("%d%d%d%d%d", &eventStartId[i], &eventEndId[i], &eventExpandNum[i], &eventExpandType[i], &eventJudgeIdOffset[i]))
            {
                return false;
            }
        }
        
        //事件
        configFile.find("[EVENT_SIGNAL_NUM]");
        if (!configFile.read("%d", &num))
        {
            return false;
        }
        
        //清空map，存储事件的id和字符串映射
        expressionMap.clear();
        
        EventSignal* eventSignal;
        configFile.find("[EVENT_SIGNAL]");
        for (int i = 0; i < num; i++)
        {
            eventSignal = new EventSignal;
            eventSignal->startTime = 0;
            eventSignal->eventStateCount = 0;
            
            if (!configFile.read("%d%d%s%s%d", &id, &index, localName, expression, &(eventSignal->level)))
            {
                return false;
            }
            expressionMap[index] = expression;
            if (eventSignal->level == 1)
            {
                eventSignal->value = 1;
            }
            else
            {
                eventSignal->value = 0;
            }
            eventSignals[index] = eventSignal;
            
            //解析表达式
            parseExpression(expression, eventSignal->expression, sizeof(eventSignal->expression), id, 10);
        }
        
        //根据扩展属性扩展配置文件内容,扩展EVENT_SIGNAL字段内容
        for(int i = 0; i < 8; i++)
        {
            if((eventStartId[i] != -1) && (eventEndId[i] != -1) && (eventExpandNum[i] != -1) && (eventExpandType[i] != -1) && (eventJudgeIdOffset[i] != -1))
            {
                int eventIdOffset = eventEndId[i] - eventStartId[i]+1;
                for(int j = 1; j <= eventExpandNum[i]; j++)
                {
                    for(int k = eventStartId[i]; k <= eventEndId[i]; k++)
                    {
                        EventSignal *expandEventSignal = eventSignals[k];
//                        printf("get--id:%d expr.id:%d expr.symbol:%d expr.value:%d\n", k, (expandEventSignal->expression[0])->id, (expandEventSignal->expression[0])->symbol, (expandEventSignal->expression[0])->value);
                        eventSignal = new EventSignal;
                        eventSignal->startTime = 0;
                        eventSignal->eventStateCount = 0;
                        if(eventExpandType[i] == 0)    //有不同的扩展规则，通过类型来区分
                        {
                            //type=0扩展expression.id部分
                            eventSignal->level = expandEventSignal->level;
                            eventSignal->value = expandEventSignal->value;
                            
                            char tmpExpression[128];
                            sprintf(tmpExpression, "%s", expressionMap[k].c_str());
                            
                            //解析表达式
                            parseExpression(tmpExpression, eventSignal->expression, sizeof(eventSignal->expression), k, 10);
                            (eventSignal->expression[0])->id += eventJudgeIdOffset[i]*j;
                        }
//                        printf("add--id:%d expr.id:%d expr.symbol:%d expr.value:%d\n", k+eventIdOffset*j, (eventSignal->expression[0])->id, (eventSignal->expression[0])->symbol, (eventSignal->expression[0])->value);
                        eventSignals[k+eventIdOffset*j] = eventSignal;
                    }
                }
            }    
        }
        
        configFile.close();
        state = STATE_OPEN;
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
    void ModbusConnection::close()
    {
        state = STATE_CLOSE;
        memset(config, 0, sizeof(config));
        serial = 0;
        memset(information, 0, sizeof(information));
        
        for (ModbusFrameMap::iterator i = dataFrame.begin(); i != dataFrame.end(); ++i)
        {
            delete ((*i).second);
        }
        dataFrame.clear();
        
        if (requestFrame != 0)
        {
            delete requestFrame;
            requestFrame = 0;
        }
        
        commState = false;
        commStateId = -1;
        deviceStateId = -1;
        
        address = 0;        
        crc = 1;
        memset(dataFun, 0, sizeof(dataFun));
        memset(pDataFun, 0, sizeof(pDataFun));
        memset(dataId, 0, sizeof(dataId));
        memset(pDataId, 0, sizeof(pDataId));
        memset(dataNr, 0, sizeof(dataNr));
        memset(pDataNr, 0, sizeof(pDataNr));
        
        memset(dataComm, 0, sizeof(dataComm));
        for (DataSignalMap::iterator i = dataSignals.begin(); i != dataSignals.end(); ++i)
        {
            DataSignal* dataSignal = (*i).second;
            for (int j = 0; j < (int)(sizeof(dataSignal->expression) / sizeof(Expression*)); j++)
            {
                if (dataSignal->expression[j] != 0)
                {
                    delete[] dataSignal->expression[j];
                }
            }
            
            delete ((*i).second);
        }
        dataSignals.clear();
        dataMap.clear();
        waveMap.clear();
        waveSizeMap.clear();
        
        ctrlCid2 = 0;
        for (CtrlSignalMap::iterator i = ctrlSignals.begin(); i != ctrlSignals.end(); ++i)
        {
            delete ((*i).second);
        }
        ctrlSignals.clear();
        
        settingCid2 = 0;
        for (SettingSignalMap::iterator i = settingSignals.begin(); i != settingSignals.end(); ++i)
        {
            delete ((*i).second);
        }
        settingSignals.clear();
        
        for (EventSignalMap::iterator i = eventSignals.begin(); i != eventSignals.end(); ++i)
        {
            EventSignal* eventSignal = (*i).second;
            for (int j = 0; j < (int)(sizeof(eventSignal->expression) / sizeof(Expression*)); j++)
            {
                if (eventSignal->expression[j] != 0)
                {
                    delete[] eventSignal->expression[j];
                }
            }
            
            delete ((*i).second);
        }
        eventSignals.clear();
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
    bool ModbusConnection::process(bool waveFlag)
    {
        if (state == STATE_CLOSE)
        {
            return true;
        }
        
        if (state == STATE_OPEN)
        {
            state = STATE_CONNECT;
        }
                        
        //依顺序发送数据采集帧
        for (int i = 0; i < (int)(sizeof(pDataFun)); i++)
        {
            if (pDataFun[i] == 0)
            {
                break;
            }
            
            int fun = (int)strtol(pDataFun[i], 0, 10);
            int id = (int)strtol(pDataId[i], 0, 10);
            int nr = (int)strtol(pDataNr[i], 0, 10);
                        
            ModbusFrame* frame = dataFrame[i];
            frame->clearBuffer();
            frame->setCrc(crc);
            frame->setNode(address);
            frame->setFun(fun);
            frame->setId(id);
            frame->setNr(nr);
            if (send(frame))
            {
                dataComm[i] = true;
            }
            else
            {
                dataComm[i] = false;
                break;
            }
        }    
        
        //从数据采集帧中获得数据
        int cnt = 0;
        for (DataSignalMap::iterator i = dataSignals.begin(); i != dataSignals.end(); ++i)
        {
            cnt++;
            
            if(cnt % 200 == 0)
            {
                //休息50毫秒
                Timer::msleep(50);
            }
            
            DataSignal* dataSignal = (*i).second;
            if (((dataSignal->frmId) >= 0) && (dataComm[dataSignal->frmId])) //通讯成功
            {
                ModbusFrame* frame = dataFrame[dataSignal->frmId];
                dataSignal->value = frame->getInfoData(dataSignal->frmIndex, dataSignal->frmType, dataSignal->frmBit);

                if (dataSignal->value >= -9999)
                {
                    dataSignal->value = (float)((dataSignal->value)) / (float)(dataSignal->frmScale);
                }
                //dataSignal->value = (float)(frame->getInfoData(dataSignal->frmIndex, dataSignal->frmType, dataSignal->frmBit)) / (dataSignal->frmScale);
            }
            
            //根据expression进行数据处理
            if ((dataSignal->index) >= 0)
            {
                int result = evalExpression(dataSignal->expression, sizeof(dataSignal->expression), false);
                //printf("value:%.2f result:%d\n", dataSignal->value, result);
                if (result <= -10000)
                {
                    dataMap[dataSignal->index] = dataSignal->value;
                }
                else
                {
                    dataMap[dataSignal->index] = (result >= 0) ? (float)result : dataMap[dataSignal->index];
                }
            }
            
            //printf("index:%d value:%.2f\n", dataSignal->index, dataMap[dataSignal->index]);
            
            //给虚拟量赋值
            if (((dataSignal->frmId) < 0) && ((dataSignal->index) >= 0))
            {
                dataSignal->value = dataMap[dataSignal->index];
            }
        }
        
        eventProcess();
        
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
    const char* ModbusConnection::info()
    {
        return information;
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
    bool ModbusConnection::init()
    {
        return false;
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
    void ModbusConnection::getWave(WaveMap& waveMap)
    {
        waveMap = this->waveMap;
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
    int ModbusConnection::getWaveSize(WaveSizeMap& waveSizeMap)
    {
        waveSizeMap = this->waveSizeMap;
        
        return 0;
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
    void ModbusConnection::getData(DataMap& dataMap)
    {
        dataMap = this->dataMap;
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
    void ModbusConnection::setEvent(EventMap& eventMap)
    {
        for (EventMap::iterator i = eventMap.begin(); i != eventMap.end(); ++i)
        {
            if ((*i).second == 1)
            {
                EventSignalMap::iterator j = eventSignals.find((*i).first);
                if (j != eventSignals.end())
                {
                    EventSignal* eventSignal = (*j).second;
                    eventSignal->value = 1;
                    eventSignal->startTime = 0;
                    eventSignal->eventStateCount = 0;
                    
                    if ((*i).first == 1) //通讯失败
                    {
                        DataSignalMap::iterator k = dataSignals.find(commStateId);
                        if (k != dataSignals.end())
                        {
                            DataSignal* dataSignal = (*k).second;
                            dataSignal->value = 1;
                        }
                    }
                }
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
    void ModbusConnection::getEvent(EventMap& eventMap)
    {
        //for (DataMap::iterator i = dataMap.begin(); i != dataMap.end(); ++i)
        //{
        //    printf("ModbusConnection--- dataMap(%d)=%.2f\n", (*i).first, (*i).second);
        //}
        
        for (EventSignalMap::iterator i = eventSignals.begin(); i != eventSignals.end(); ++i)
        {
            EventSignal* eventSignal = (*i).second;
            
            eventMap[(*i).first] = eventSignal->value;
            
            //printf("ModbusConnection--- eventMap(%d)=%d\n", (*i).first, eventSignal->value);
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
    int ModbusConnection::getEventLevel(int index)
    {
        if (index < 0)
        {
            return -1;
        }
        
        EventSignal* eventSignal = 0;
        EventSignalMap::iterator i = eventSignals.find(index);
        if (i != eventSignals.end())
        {
            eventSignal = (*i).second;
            
            return eventSignal->level;
        }
        
        return -1;
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
    bool ModbusConnection::control(int index, float var, char* batch, int length,bool broadcastFlag)
    {
        /*
        if (state != STATE_CONNECT)
        {
            return false;
        }
        */
            
        if (index < 0)
        {
            return false;
        }
        
        CtrlSignalMap::iterator i = ctrlSignals.find(index); //查找是否为控制命令
        SettingSignalMap::iterator j = settingSignals.find(index); //查找是否为设置命令
        if ((i == ctrlSignals.end()) && (j == settingSignals.end()))
        {
            return false;
        }
        
        requestFrame->clearBuffer();
        requestFrame->setCrc(crc);
        
        if(!batch)
        {    
            if (i != ctrlSignals.end()) //控制命令
            {
                CtrlSignal* ctrlSignal = (*i).second;
                if(broadcastFlag==true)
                {
                    requestFrame->setNode(0xff);
                }
                else
                {        
                    requestFrame->setNode(address);
                }
                requestFrame->setFun(ctrlSignal->cmdFunc);
                requestFrame->setId(ctrlSignal->cmdAddr);
                requestFrame->setValue(ctrlSignal->cmdValue);
            }
            else if (j != settingSignals.end()) //设置命令
            {
                SettingSignal* settingSignal = (*j).second;
                if(broadcastFlag==true)
                {
                    requestFrame->setNode(0xff);
                }
                else
                {        
                    requestFrame->setNode(address);
                }
                requestFrame->setFun(settingSignal->cmdFunc);
                requestFrame->setId(settingSignal->cmdAddr);
                requestFrame->setNr(settingSignal->cmdLen);
                requestFrame->setBytes(settingSignal->cmdLen*2);
                requestFrame->setValue((int)((int)var * settingSignal->scale));
                //printf("var= %d \n",(int)(var * settingSignal->scale));
                if(settingSignal->cmdSize == 0)
                {
                    //普通设置数据，占用1个寄存器
                    requestFrame->addInfoData((float)(var * (float)settingSignal->scale), settingSignal->paramType);
                }
                else
                {
                    for(int kk = 0; kk < settingSignal->cmdSize; kk++)
                    {
                        if(settingSignal->cmdParam[kk] == 0)
                        {
                            requestFrame->addInfoData((float)(var * (float)settingSignal->scale), settingSignal->paramType);
                        }
                        else if(settingSignal->cmdParam[kk] == -1)
                        {
                            requestFrame->addInfoData((float)0x00, settingSignal->paramType);
                        }
                        else
                        {
                            requestFrame->addInfoData((float)(dataMap[settingSignal->cmdParam[kk]]), settingSignal->paramType);    
                        }
                    }    
                }
            }
            else
            {
                return false;
            }
        }
        else //test fix zjx 2013-1
        {
            if (j != settingSignals.end()) //设置命令
            {
                SettingSignal* settingSignal = (*j).second;
                if(broadcastFlag==true)
                {
                    requestFrame->setNode(0xff);
                }
                else
                {        
                    requestFrame->setNode(address);
                }
                requestFrame->setFun(settingSignal->cmdFunc);//功能码
                requestFrame->setId(settingSignal->cmdAddr);//寄存器开始地址
                requestFrame->setNr(length/2);//寄存器数量
                requestFrame->setBytes(length);//数据数量
                requestFrame->addInfoData((unsigned char*)batch,length);
            }
            else
            {    
                return false;
            }
        }
        return send(requestFrame);
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
    bool ModbusConnection::send(ModbusFrame* frame)
    {
        
        if (state == STATE_CLOSE)
        {
            return false;
        }
        
        if (frame == 0)
        {
            return false;
        }
         
        char requestBuffer[ModbusConstants::FRAME_BUFFER_SIZE];
        memset(requestBuffer, 0, sizeof(requestBuffer));
        const char* buffer = (const char*)(frame->getBuffer());
        int requestLength = frame->getLength();
        memcpy((void*)requestBuffer, (const void*)buffer, requestLength);
        
                
        bool commOK = false;
        char responseBuffer[ModbusConstants::FRAME_BUFFER_SIZE];
        memset(responseBuffer, 0, sizeof(responseBuffer));
        for (int i = 0; i < 3; i++)
        {
            if (!serial->open())
            {
                continue;
            }
            //sleep(3);
            

            
            if (serial->send(requestBuffer, requestLength) != requestLength)
            {
                serial->close();
                continue;
            }
            
            //广播时，只发一次就自动退出
            if(frame->getNode()==0xff)
            {
                if (debug)
                {
                    printf("Success:Tx: ");
                    for(int k1 = 0; k1 < requestLength; k1++)
                    {
                        printf("%02X ", (requestBuffer[k1]) & 0xFF);    
                    }
                    printf("\n\n");
                }
                break;
            }    
                        
            int responseLength = serial->recv(responseBuffer, sizeof(responseBuffer));
            if (responseLength < 0)
            {
                if (debug)
                {
                    printf("Error---Tx len=%d: ", requestLength);
                    for(int k1 = 0; k1 < requestLength; k1++)
                    {
                        printf("%02X ", (requestBuffer[k1]) & 0xFF);    
                    }
                    printf("\n");
                }
                
                serial->close();
                continue;
            }
            
            if (frame->parse((const unsigned char*)responseBuffer, responseLength) != 0)
            {
                if (debug)
                {
                    printf("Error---Tx len=%d: ", requestLength);
                    for(int k1 = 0; k1 < requestLength; k1++)
                    {
                        printf("%02X ", (requestBuffer[k1]) & 0xFF);    
                    }
                    printf("\n");
                    
                    printf("Error---Rx len=%d : ", responseLength);
                    for(int k2 = 0; k2 < responseLength; k2++)
                    {
                        printf("%02X ", (responseBuffer[k2]) & 0xFF);    
                    }
                    printf("\n\n");
                }
                
                serial->close();
                continue;
            }
            
            if (debug)
            {
                printf("Success:Tx: ");
                for(int k1 = 0; k1 < requestLength; k1++)
                {
                    printf("%02X ", (requestBuffer[k1]) & 0xFF);    
                }
                printf("\n");
                
                printf("Success:Rx: ");
                for(int k2 = 0; k2 < responseLength; k2++)
                {
                    printf("%02X ", (responseBuffer[k2]) & 0xFF);    
                }
                printf("\n\n");
            }
                
            commOK = true;
            break;
        }
        
        //设置通讯状态
        commState = commOK;
                
        DataSignalMap::iterator i = dataSignals.find(commStateId);
        if (i != dataSignals.end())
        {
            DataSignal* dataSignal = (*i).second;
            if (commState) //通讯成功
            {
                dataSignal->value = 0;
                
                commFailureTime = 0;
            }
            else //通讯失败
            {
                //去掉误告警
                //通讯失败：1，有；0，无
                if (((int)(dataSignal->value)) == 0)
                {
                    if (commFailureTime < 1)
                    {
                        commFailureTime = Timer::get_current_time_millis();
                    }
                    
                    if (fabs(Timer::get_current_time_millis() - commFailureTime) >= (double)(25 * 1000))
                    {
                        dataSignal->value = 1;
                    }
                }
                
                return false;
            }
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
    int ModbusConnection::parseExpression(char* src, Expression** dest, int destLen, int id, int base)
    {
        if ((src == 0) || (strlen(src) <= 0) || (dest == 0) || (destLen <= 0) || (id < 0) || (base <= 0))
        {
            return -1;
        }
        
        memset(dest, 0, destLen);
        char szExpression[128];
        char* expression[32];
        char* subExpression[16];
        char szId[8], szValue[8];
        
        //把表达式（如(ID9=E9)/(ID9=01)&(ID10=E8)/(ID9=01)&(ID10=E7)/(ID9=02)）通过/分割
        int length = ConfigFile::split(src, (char *)"/", expression, sizeof(expression));
        
        for (int i = 0; i < length; i++)
        {
            if (i >= (int)(destLen / sizeof(Expression*)))
            {
                break;
            }
            
            if ((strlen(expression[i]) <= 0) || (strcmp(expression[i], "*") == 0))
            {
                Expression* pExpression = new Expression[1];
                memset(pExpression, 0, sizeof(Expression) * 1);
                dest[i] = pExpression;
                
                pExpression[0].id = 10000; //表示不需要后续表达式，始终为真
                
                return 0;
            }
                        
            strcpy(szExpression, expression[i]);
            
            //把子表达式（如(ID9=01)&(ID10=E8)）通过&|分割
            int subLength = ConfigFile::split(expression[i], (char *)"&|", subExpression, sizeof(subExpression));
            if (subLength <= 0)
            {
                continue;
            }
            
            Expression* pExpression = new Expression[subLength];
            memset(pExpression, 0, sizeof(Expression) * subLength);
            dest[i] = pExpression;
            for (int j = 0; j < subLength; j++)
            {
                //在子表达式（如(ID9=01)）中寻找=
                if (strchr(subExpression[j], '=') != 0)
                {
                    if (sscanf(subExpression[j], "%*[^D]D%[^=]=%[^)]", szId, szValue) < 0)
                    {
                        pExpression[j].id = -1; //表示不需要后续表达式
                        break;
                    }
                    
                    pExpression[j].id = atoi(szId);
                    pExpression[j].symbol = getSymbol(szExpression, (char *)"&|", j);
                    pExpression[j].value = (int)strtol(szValue, 0, base);
                }
                else if (strcmp(subExpression[j], "-1") == 0)
                {
                    pExpression[j].id = -10000; //表示不需要表达式
                    
                    return 0;
                }
                else
                {
                    pExpression[j].id = id;
                    pExpression[j].symbol = getSymbol(szExpression, (char *)"&|", j);
                    pExpression[j].value = (int)strtol(subExpression[j], 0, base);
                }
            }
        }
                
        return 0;
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
    void ModbusConnection::expandExpression(char *src, std::string &dest, int offset)
    {
        int startIndex = 0;
        int endIndex = 0;
        int size = 0;
        int value[32];
        int valueLen[32];
        
        for(unsigned int i = 0; i < strlen(src); i++)
        {
            if(src[i] == 'D')
            {
                startIndex = i;
                continue;
            }
    
            if(src[i] == '=')
            {
                endIndex = i;
                char tmp[8];
                memset(tmp, 0, sizeof(tmp));
                int index = 0;
                for(int j = startIndex+1; j < endIndex; j++)
                {
                    tmp[index++] = src[j];
                }
                
                //将表达式中ID段后的数字截取出来，添加offset
                valueLen[size] = index;
                value[size] = atoi(tmp);
                value[size] += offset;
                size++;
                continue;
            }
        }
        
        int index = 0;
        for(unsigned int i = 0; i < strlen(src);)
        {
            if(src[i] == 'D')
            {
                dest += src[i];
                i++;
                char tmp[8];
                memset(tmp, 0, sizeof(tmp));
                sprintf(tmp, "%d", value[index]);
                dest += tmp;
                i += valueLen[index];
                index++;
                continue;
            }
            dest += src[i];
            i++;
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
    int ModbusConnection::getSymbol(char* src, char* separator, int index)
    {
        if ((src == 0) || (strlen(src) <= 0) || (separator == 0) || (strlen(separator) <= 0) || (index < 0))
        {
            return -1;
        }
        
        int i, j, count, length = strlen(src), sepLength = strlen(separator);
        for (i = 0, count = 0; i < length; i++)
        {
            for (j = 0; j < sepLength; j++)
            {
                if (src[i] == separator[j])
                {
                    break;
                }
            }
            
            if (j != sepLength)
            {
                if (count == index)
                {
                    return src[i];
                }
                
                count++;
            }
        }
        
        return -1;
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
    int ModbusConnection::evalExpression(Expression** dest, int destLen, bool event)
    {
        if ((dest == 0) || (destLen <= 0))
        {
            return -1;
        }
        
        for (int i = 0; i < (int)(destLen / sizeof(Expression*)); i++)
        {
            Expression* pExpression = dest[i];
            if (pExpression == 0)
            {
                break;
            }
            
            for (int j = 0; ; j++)
            {
                if (pExpression[j].id <= -10000) //不需要表达式
                {
                    return -10000;
                }
                
                if (pExpression[j].id < 0) //不需要后续表达式
                {
                    break;
                }
                
                if (pExpression[j].id >= 10000) //表示不需要后续表达式，始终为真
                {
                    return i;
                }
                
                DataSignalMap::iterator k = dataSignals.find(pExpression[j].id); //查找信号
                if (k == dataSignals.end())
                {
                    break;
                }
                                
                DataSignal* dataSignal = (*k).second;
                int currValue = (int)(dataSignal->value);
                if (((dataSignal->index) >= 0) && event)
                {
                    currValue = (int)(dataMap[dataSignal->index]);
                }        
                bool result = (currValue == pExpression[j].value) ? true : false;
                                
                if (pExpression[j].symbol < 0) //表示不需要下一个表达式
                {
                    if (result)
                    {
                        return i;
                    }
                    else
                    {
                        break;
                    }
                }
                else if (pExpression[j].symbol == '&') //与下一个表达式进行与操作
                {
                    if (!result)
                    {
                        break;
                    }
                }
                else if (pExpression[j].symbol == '|') //与下一个表达式进行或操作
                {
                    if (result)
                    {
                        return i;
                    }
                }
                else
                {
                    break;
                }
            }
        }
        
        return -1;
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
    void ModbusConnection::eventProcess()
    {
        //根据expression进行事件处理
        int alarmCnt = 0;
        int warningCnt = 0;
        int cnt = 0;
        for (EventSignalMap::iterator i = eventSignals.begin(); i != eventSignals.end(); ++i)
        {
            cnt++;
            
            if(cnt % 200 == 0)
            {
                //休息50毫秒
                Timer::msleep(50);
            }
            
            EventSignal* eventSignal = (*i).second;
            
            if ((eventSignal->level > 1) && ((*i).first != 1))
            {
                if (commState) //只有在通讯正常时，才分析告警
                {
                    int result = evalExpression(eventSignal->expression, sizeof(eventSignal->expression));
                    
                    //去掉误告警
                    //告警：1，有；0，无
                    int eventStateTmp = (result == 0) ? 1 : 0;
                    if (eventStateTmp != eventSignal->value)
                    {
                        if ((eventSignal->eventStateCount) == 0)
                        {
                            eventSignal->startTime = Timer::get_current_time_millis();
                        }
                        
                        (eventSignal->eventStateCount)++;
                    }
                    else
                    {
                        eventSignal->startTime = 0;
                        eventSignal->eventStateCount = 0;
                    }
                    
                    if (eventStateTmp == 1)
                    {
                        if (((eventSignal->eventStateCount) >= 2) 
                            && (fabs(Timer::get_current_time_millis() - eventSignal->startTime) >= (double)(5 * 1000)))
                        {
                            eventSignal->value = eventStateTmp;
                            eventSignal->eventStateCount = 0;
                        }
                    }
                    else
                    {
                        eventSignal->value = eventStateTmp;
                        eventSignal->eventStateCount = 0;
                    }
                }
            }
            else
            {
                int result = evalExpression(eventSignal->expression, sizeof(eventSignal->expression));
                eventSignal->value = (result == 0) ? 1 : 0;
            }
            
            //增加设备状态计数器，记录严重告警和一般告警的个数
            if ((eventSignal->level >= 3) && (eventSignal->value != 0))
            {
                alarmCnt++;
            }
            else if ((eventSignal->level == 2) && (eventSignal->value != 0))
            {
                warningCnt++;
            }
        }
                        
        //设置设备状态
/*      bool alarmState = false;
        bool failState = false;
        for (EventSignalMap::iterator i = eventSignals.begin(); i != eventSignals.end(); ++i)
        {
            EventSignal* eventSignal = (*i).second;
            if ((eventSignal->level >= 3) && (eventSignal->value != 0))
            {
                failState = true;
                break;
            }
            else if ((eventSignal->level == 2) && (eventSignal->value != 0))
            {
                alarmState = true;
                break;
            }
        }
        if (failState)
        {
            dataMap[deviceStateId] = 2; //故障状态
        }
        else
        {
            if (alarmState)
            {
                dataMap[deviceStateId] = 1; //告警状态
            }
            else
            {
                dataMap[deviceStateId] = 0; //正常状态
            }
        }*/
        if((alarmCnt == 0) && (warningCnt == 0))
        {
            dataMap[deviceStateId] = 0; //正常状态    
        }
        else if((alarmCnt == 0) && (warningCnt != 0))
        {
            dataMap[deviceStateId] = 1; //告警状态
        }
        else
        {
            dataMap[deviceStateId] = 2; //故障状态    
        }
    }
    
    void ModbusConnection::setDebugMode(int val)
    {
        debug = val;
    }
}

