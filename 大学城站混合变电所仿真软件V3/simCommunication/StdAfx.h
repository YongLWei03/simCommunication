// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__04129C82_3A73_4A54_BCE6_931560C71AF2__INCLUDED_)
#define AFX_STDAFX_H__04129C82_3A73_4A54_BCE6_931560C71AF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC Automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#if _MSC_VER >= 1200
#include <afxhtml.h> // MFC HTML view support
#else
#define CHtmlView CView // not availiable with versions earlier that VC6
#endif

#include <tlhelp32.h>
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxsock.h>		// MFC socket extensions
#include <afxmt.h>
#include "message.h"
#include "math.h"
#include "global.h"
#include "NewString.h"
#include "NewSocket.h"
#include "DataExchangeSocket.h"
#include "NewComputer.h"
#include "NewProtection.h"
#include "ProtocolTH104.h"
#include "CommThread.h"
#include "PowerThread.h"
#include "ProtectionCalculater.h"

extern CString g_SystemFilePath;
extern CString g_ProjectFilePath;
extern WORD    g_PowerID;
extern CString g_PowerNameList[];

typedef struct TagVarStruct //变量
{
  WORD UnitID;
  WORD VarGroupID;
  WORD VarID;

  CString VarName;
  CString TextFormat;
  float Value;
  float LastValue;
}VARSTRUCT;

typedef struct TagVarGroupStruct //变量组
{
  WORD UnitID;
  WORD VarGroupID;
  CString VarGroupName;
  WORD VarGroupType; //类型  遥测组、遥信组、电度组
  TagVarStruct Var[VARCOUNT];
}VARGROUPSTRUCT;

typedef struct TagParameterGroupStruct //定值组
{
  WORD UnitID;
  WORD VarGroupID;
  CString VarGroupName;
  WORD VarGroupType; //类型  遥测组、遥信组、电度组
  TagVarStruct Var[PARAMETERCOUNT];
}PARAMETERGROUPSTRUCT;

typedef struct TagUnitStruct //装置
{
  CString UnitName;
  WORD UnitType; //类型  0保留、1线路等
  WORD UnitID;
  WORD UnitState;//装置状态 //正常、故障
  float UnitCT;
  float UnitPT;
  TagVarGroupStruct VarGroup[VARGROUPCOUNT];
  TagParameterGroupStruct ParameterGroup[PARAMETERGROUPCOUNT];
  TagVarGroupStruct PreDownParameterGroup;
  WORD CurrentParamaterGroupID;   //当前定值组号
  float CurrentValue;     //给定电流
  float VotlageValue;     //给定电压
  float AngleValue;       //给定角度
  BOOL  TransmissionState;//传动状态
  BOOL  TrainState;       //行车状态

  WORD  VarLock[VARCOUNT];
  WORD  VarAbnormal[VARCOUNT];//异常状态
  CString VarSoundName[VARCOUNT][2];

  float SampleWaveValue[12][320];//32点x10周波采样数值
  float FaultWaveValue[12][320]; //32点x10周波故障数值

  CPtrList SOEReportList;   //SOE报告列表

  BYTE m_CommAllDatabuf[1024]; //通讯全数据缓冲区
  BYTE m_CommRecordWaveDatabuf[8192]; //通讯录波数据缓冲区

}UNITSTRUCT;

typedef struct TagSOEReportStruct
{
  WORD UnitID;
  WORD SOEID;
  CTime SOETime;
  WORD SOEMSEL;
  BOOL SOEValue;           //事件数值
  BOOL SendState;   //通讯传送状态 0:未发 1:已发
  float ActionValue[VARCOUNT]; //动作数值
  float RecordWaveValue[12][320];//32点x10周波数值
}SOEREPORTSTRUCT;

typedef struct TagVarShareStruct //变量
{
  float Value;
}VARSHARESTRUCT;

typedef struct TagVarGroupShareStruct //变量组
{
  TagVarShareStruct Var[VARCOUNT];
}VARGROUPSHARESTRUCT;

typedef struct TagParameterGroupShareStruct //定值组
{
  TagVarShareStruct Var[PARAMETERCOUNT];
}PARAMETERSHARESTRUCT;

typedef struct TagUnitShareStruct
{
  WORD UnitID;
  WORD UnitState;//装置状态 //正常、故障
  TagVarGroupShareStruct VarGroup[VARGROUPCOUNT];
  TagParameterGroupShareStruct ParameterGroup[PARAMETERGROUPCOUNT];
  WORD CurrentParamaterGroupID;   //当前定值组号
  float CurrentValue;     //给定电流
  float VotlageValue;     //给定电压
  float AngleValue;       //给定角度
  BOOL  TransmissionState;//传动状态
}UNITSHARESTRUCT;

typedef struct TagComputerShareStruct
{
  BOOL simCommunicationNewData;
  BOOL simProtectionNewData;
  BOOL simCommunicationNewSOEReport;
  BOOL simProtectionNewSOEReport;
  TagSOEReportStruct SOEReport;
  TagUnitShareStruct CommunicationUnitList[UNITCOUNT];
  TagUnitShareStruct ProtectionUnitList[UNITCOUNT];
}COMPUTERSHARESTRUCT;

typedef struct TagDataExchangeStruct//数据交换
{
  BOOL PowerState;//供电状态
  BOOL TrainState;//行车状态
  WORD UnitID;
  WORD VarGroupID;
  WORD VarID;
  float   AverCurrent;              //行车电流
  float   AverAngle;                //行车角度
	float   CurrentChangeRate;        //电流抖动
}DATAEXCHANGESTRUCT;

typedef struct TagInlineVotlageStruct
{
	float Ua1;
	float Ub1;
	float Uc1;
	float Ua2;
	float Ub2;
	float Uc2;
  float Reserved1;
  float Reserved2;

	float T1HiVoltageChangeRate;
	float T1LoVoltageChangeRate;
	float T2HiVoltageChangeRate;
	float T2LoVoltageChangeRate;
	WORD  StartupDelaySecond;
  CTime BeginTime;
}INLINEVOTLAGEVALUESTRUCT;

typedef struct TagPTVotlageStruct
{
  float T1HiVoltageValueK;
  float T1HiCurrentMaxValue;
  float T2HiVoltageValueK;
  float T2HiCurrentMaxValue;
  float T1LoVoltageValueK;
  float T1LoCurrentMaxValue;
  float T2LoVoltageValueK;
  float T2LoCurrentMaxValue;
  WORD  T1Gear;
  WORD  T2Gear;
  WORD  T1Temp1;
  WORD  T1Temp2;
  WORD  T2Temp1;
  WORD  T2Temp2;
  float Reserved;
	WORD  StartupDelaySecond;
  CTime BeginTime;
}PTVOTLAGESTRUCT;

typedef struct TagCurrentStruct
{
  float Reserved1;
  float Reserved2;
  float Reserved3;
  float Reserved4;
	float NoloadCurrentChangeMaxValue;
	WORD  StartupDelaySecond;
  CTime BeginTime;
}CURRENTSTRUCT;

typedef struct TagCompensationStruct
{
	WORD CompensationType;    //补偿形式  0：电容器 1：AVC 2：AVG 
	WORD CompensationUnitID;  //补偿保护

  float CompensationBranchValueZ[4];              //补偿支路阻抗值
	float CompensationBranchValueK[4];              //补偿支路系数值
  char CompensationBranchVarI[4][8];              //补偿支路电流变量指示
  char CompensationBranchVarQ[4][8];              //补偿支路无功变量指示
  char CompensationBranchPowerPathVarList[4][128];//补偿支路路径指示

}COMPENSATIONSTRUCT;

typedef struct TagPowerPathStruct
{
  WORD UnitID;
  WORD VarID;
  float Uab;
  float Ubc;
  float Uca;
  float Ua;
  float Ub;
  float Uc;
}POWERPATHSTRUCT;

typedef struct TagBypassFeedStruct
{
 TagVarStruct Var[3];
}BYPASSFEEDSTRUCT;

typedef struct TagFreeTransmissionStruct  //自由传动
{
  WORD    FeederUnitID;             //传动馈线编号

  float   AverCurrent;              //传动电流
  float   AverAngle;                //传动角度
	float   CurrentChangeRate;        //电流抖动
  char    CurrentChangeRateList[512];//电流变化率表

  WORD    ContinuedSeconds;         //传动持续时间
	WORD    StartupDelaySecond;       //传动启动时间

  CTime   BeginTime;                //传动起始时间
  CTime   FinishTime;               //传动终止时间
}FREETRANSMISSIONTRUCT;

typedef struct TagFreeTrainStruct     //自由行车
{
  WORD    InFeederUnitID;             //驶入馈线编号
  WORD    OutFeederUnitID;            //驶出馈线编号

  float   RealTimeCurrent;            //实时电流
  float   RealTimeAngle;              //实时角度
  float   AverCurrent;                //行车电流
  float   AverAngle;                  //行车角度
	float   CurrentChangeRate;          //电流抖动
  char    InCurrentChangeRateList[512];//驶入电流变化率表
  char    OutCurrentChangeRateList[512];//驶出电流变化率表

  WORD    InContinuedSeconds;         //驶入持续时间
  WORD    ChangeContinuedSeconds;     //换相持续时间
  WORD    OutContinuedSeconds;        //驶出持续时间
	WORD    StartupDelaySecond;         //行车启动时间

  CTime   InBeginTime;                //驶入起始时间
  CTime   InFinishTime;               //驶入终止时间
  CTime   ChangeBeginTime;            //换相起始时间
  CTime   ChangeFinishTime;           //换相终止时间
  CTime   OutBeginTime;               //驶出起始时间
  CTime   OutFinishTime;              //驶出终止时间

}FREETRAINSTRUCT;

typedef struct TagFaultTransmissionStruct //故障传动
{
  BOOL    FaultTransmissionState;         //故障传动状态 0:空闲 1:准备读文件 2: 传动中
  char    FaultTransmissionDataFileName[256];
	WORD    StartupDelaySecond;             //传动启动时间
  CTime   BeginTime;                      //行车起始时间
}FAULTTRANSMISSIONTRUCT;

typedef struct TagSimTrainStruct          //仿真行车
{
  BOOL    SimTrainState;                  //仿真行车状态 0:空闲 1:准备读文件 2: 行车中
  char    SimTrainDataFileName[256];
	WORD    SimTrainDataCurrentPos;
	WORD    SimTrainDataCount;
  WORD    StartupDelaySecond;             //行车启动时间
  CTime   BeginTime;                      //行车起始时间
}SIMTRAINSTRUCT;

typedef struct TagProtectionSwitchStruct
{
  CString VarName[PROTECTION_SWITCH_COUNT];
  CString TextFormat[PROTECTION_SWITCH_COUNT];
  float Value[PROTECTION_SWITCH_COUNT];
}PROTECTIONSWITCHSTRUCT;//保护压板结构

typedef struct TagProtectionParameterStruct
{
  CString VarName[PROTECTION_PARAMETER_COUNT];
  CString TextFormat[PROTECTION_PARAMETER_COUNT];
  float Value[PROTECTION_PARAMETER_COUNT];
}PROTECTIONPARAMETERSTRUCT;//保护定值结构

typedef struct TagProtectionActionValueStruct
{
  CString VarName[PROTECTION_ACTION_VALUE_COUNT];
  CString TextFormat[PROTECTION_ACTION_VALUE_COUNT];
  float Value[PROTECTION_ACTION_VALUE_COUNT];
}PROTECTIONACTIONVALUESTRUCT;//保护动作值结构

typedef struct TagProtectionSampleStruct
{
  CString ChannelName[WAVE_CHANNEL_COUNT];
  float ChannelValue[WAVE_CHANNEL_COUNT][WAVE_SAMPLE_COUNT];
}PROTECTIONSAMPLESTRUCT;//保护采样结构

typedef struct TagProtectionCalculaterStruct
{
  WORD ProtectionID;          //保护装置编号
  WORD ProtectionType;        //保护装置类型
  WORD ProtectionActionType;  //保护动作类型 0为无动作
  WORD ProtectionActionTime;  //保护动作时间

  TagProtectionSampleStruct ProtectionSample;           //保护采样波形数据
  TagProtectionSwitchStruct ProtectionSwitch;           //保护压板
  TagProtectionParameterStruct ProtectionParameter;     //保护定值
  TagProtectionActionValueStruct ProtectionActionValue;  //保护动作值
  BOOL ProtectionActionBreakerTrip[3];                    //保护动作跳闸输出
  BOOL ProtectionActionBreakerReclose;                    //保护动作重合闸输出
}PROTECTIONCALCULATERSTRUCT;//保护计算结构

typedef struct TagPresetVarStruct //预置变量
{
  WORD UnitID;
  WORD VarGroupID;
  WORD VarID;
  float PresetValue;
  CTime PresetTime;
}PRESETVARSTRUCT;

typedef struct TagSceneOperateStruct //场景操作
{
  CString SceneName;
  CString SceneDescribe;
  
  BOOL  EnableFreeTrain;        //自由行车操作有效
  BOOL  EnableSimTrain;         //仿真行车操作有效
  BOOL  EnableFaultTransmission;//仿真传动操作有效
  BOOL  EnableFreeTransmission; //自由传动操作有效
  BOOL  EnableVar[UNITCOUNT][VARCOUNT]; //遥信变量操作有效
  BOOL  EnableParameterVar[UNITCOUNT][PARAMETERGROUPCOUNT][PARAMETERCOUNT]; //定值变量操作有效

  TagInlineVotlageStruct      InlineVoltage;
  TagPTVotlageStruct          PTVoltage;
  TagCurrentStruct            Current;
  TagCompensationStruct       Compensation[2];

  TagFreeTransmissionStruct   FreeTransmission;   //自由传动
  TagFreeTrainStruct          FreeTrain;          //自由行车
  TagFaultTransmissionStruct  FaultTransmission;  //故障传动
  TagSimTrainStruct           SimTrain;           //仿真行车

  WORD CurrentParamaterGroupID[UNITCOUNT];  //当前定值组号
  WORD VarLock[UNITCOUNT][VARCOUNT];        //拒动
  WORD VarAbnormal[VARCOUNT][VARCOUNT];     //异常
  float VarValue[UNITCOUNT][VARCOUNT];
  float ParameterVarValue[UNITCOUNT][PARAMETERGROUPCOUNT][PARAMETERCOUNT];

  CPtrList ScenePictureList;
  CPtrList SceneSoundList;

  LONG TotalScore;
  LONG CorrectScore;

}SCENEOPERATESTRUCT;

typedef struct TagScenePictureStruct  //场景图示
{
  CString ScenePrompt;            //提示
  CString SceneNote;              //说明
  CString ScenePictureName[2];
  CString SceneVideoName;
  CString VarOperate;             //遥信操作
  CString VarWait;                //遥信等待
  CString VarConfirm;             //遥信确认
  CString SceneMultiSelectOperate;//多选操作
  CString SceneMessageShowOperate;//提示操作
  CString SceneDelayOperate;      //延时操作
  CString SceneParameterOperate;  //定值操作
  CString SceneInlineVotlage;     //电压设置
  CString ScenePTVotlage;         //压互设置
  CString SceneCurrent;           //电流设置
  CString SceneCompensation;      //补偿设置
  CString SceneTransmission;      //传动设置
  CString SceneTrain;             //行车设置
  CString SceneFileLoad;          //场景设置

  WORD    SceneType;              //0:遥控操作 1:就地操作 2:确认操作
  WORD    SceneUnitID;
  WORD    SceneVarID;
  WORD    SceneTarge;             //0:分选择/操作 1:合选择/操作
  WORD    SceneOperateError;      //操作错误
  WORD    SceneCorrectScore;      //正确操作得分
  WORD    SceneErrorScore;        //错误操作扣分
  WORD    SceneOperateUsedSeconds;//操作用时
}SCENEPICTURE;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__04129C82_3A73_4A54_BCE6_931560C71AF2__INCLUDED_)
