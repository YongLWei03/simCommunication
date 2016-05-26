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

typedef struct TagVarStruct //����
{
  WORD UnitID;
  WORD VarGroupID;
  WORD VarID;

  CString VarName;
  CString TextFormat;
  float Value;
  float LastValue;
}VARSTRUCT;

typedef struct TagVarGroupStruct //������
{
  WORD UnitID;
  WORD VarGroupID;
  CString VarGroupName;
  WORD VarGroupType; //����  ң���顢ң���顢�����
  TagVarStruct Var[VARCOUNT];
}VARGROUPSTRUCT;

typedef struct TagParameterGroupStruct //��ֵ��
{
  WORD UnitID;
  WORD VarGroupID;
  CString VarGroupName;
  WORD VarGroupType; //����  ң���顢ң���顢�����
  TagVarStruct Var[PARAMETERCOUNT];
}PARAMETERGROUPSTRUCT;

typedef struct TagUnitStruct //װ��
{
  CString UnitName;
  WORD UnitType; //����  0������1��·��
  WORD UnitID;
  WORD UnitState;//װ��״̬ //����������
  float UnitCT;
  float UnitPT;
  TagVarGroupStruct VarGroup[VARGROUPCOUNT];
  TagParameterGroupStruct ParameterGroup[PARAMETERGROUPCOUNT];
  TagVarGroupStruct PreDownParameterGroup;
  WORD CurrentParamaterGroupID;   //��ǰ��ֵ���
  float CurrentValue;     //��������
  float VotlageValue;     //������ѹ
  float AngleValue;       //�����Ƕ�
  BOOL  TransmissionState;//����״̬
  BOOL  TrainState;       //�г�״̬

  WORD  VarLock[VARCOUNT];
  WORD  VarAbnormal[VARCOUNT];//�쳣״̬
  CString VarSoundName[VARCOUNT][2];

  float SampleWaveValue[12][320];//32��x10�ܲ�������ֵ
  float FaultWaveValue[12][320]; //32��x10�ܲ�������ֵ

  CPtrList SOEReportList;   //SOE�����б�

  BYTE m_CommAllDatabuf[1024]; //ͨѶȫ���ݻ�����
  BYTE m_CommRecordWaveDatabuf[8192]; //ͨѶ¼�����ݻ�����

}UNITSTRUCT;

typedef struct TagSOEReportStruct
{
  WORD UnitID;
  WORD SOEID;
  CTime SOETime;
  WORD SOEMSEL;
  BOOL SOEValue;           //�¼���ֵ
  BOOL SendState;   //ͨѶ����״̬ 0:δ�� 1:�ѷ�
  float ActionValue[VARCOUNT]; //������ֵ
  float RecordWaveValue[12][320];//32��x10�ܲ���ֵ
}SOEREPORTSTRUCT;

typedef struct TagVarShareStruct //����
{
  float Value;
}VARSHARESTRUCT;

typedef struct TagVarGroupShareStruct //������
{
  TagVarShareStruct Var[VARCOUNT];
}VARGROUPSHARESTRUCT;

typedef struct TagParameterGroupShareStruct //��ֵ��
{
  TagVarShareStruct Var[PARAMETERCOUNT];
}PARAMETERSHARESTRUCT;

typedef struct TagUnitShareStruct
{
  WORD UnitID;
  WORD UnitState;//װ��״̬ //����������
  TagVarGroupShareStruct VarGroup[VARGROUPCOUNT];
  TagParameterGroupShareStruct ParameterGroup[PARAMETERGROUPCOUNT];
  WORD CurrentParamaterGroupID;   //��ǰ��ֵ���
  float CurrentValue;     //��������
  float VotlageValue;     //������ѹ
  float AngleValue;       //�����Ƕ�
  BOOL  TransmissionState;//����״̬
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

typedef struct TagDataExchangeStruct//���ݽ���
{
  BOOL PowerState;//����״̬
  BOOL TrainState;//�г�״̬
  WORD UnitID;
  WORD VarGroupID;
  WORD VarID;
  float   AverCurrent;              //�г�����
  float   AverAngle;                //�г��Ƕ�
	float   CurrentChangeRate;        //��������
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
	WORD CompensationType;    //������ʽ  0�������� 1��AVC 2��AVG 
	WORD CompensationUnitID;  //��������

  float CompensationBranchValueZ[4];              //����֧·�迹ֵ
	float CompensationBranchValueK[4];              //����֧·ϵ��ֵ
  char CompensationBranchVarI[4][8];              //����֧·��������ָʾ
  char CompensationBranchVarQ[4][8];              //����֧·�޹�����ָʾ
  char CompensationBranchPowerPathVarList[4][128];//����֧··��ָʾ

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

typedef struct TagFreeTransmissionStruct  //���ɴ���
{
  WORD    FeederUnitID;             //�������߱��

  float   AverCurrent;              //��������
  float   AverAngle;                //�����Ƕ�
	float   CurrentChangeRate;        //��������
  char    CurrentChangeRateList[512];//�����仯�ʱ�

  WORD    ContinuedSeconds;         //��������ʱ��
	WORD    StartupDelaySecond;       //��������ʱ��

  CTime   BeginTime;                //������ʼʱ��
  CTime   FinishTime;               //������ֹʱ��
}FREETRANSMISSIONTRUCT;

typedef struct TagFreeTrainStruct     //�����г�
{
  WORD    InFeederUnitID;             //ʻ�����߱��
  WORD    OutFeederUnitID;            //ʻ�����߱��

  float   RealTimeCurrent;            //ʵʱ����
  float   RealTimeAngle;              //ʵʱ�Ƕ�
  float   AverCurrent;                //�г�����
  float   AverAngle;                  //�г��Ƕ�
	float   CurrentChangeRate;          //��������
  char    InCurrentChangeRateList[512];//ʻ������仯�ʱ�
  char    OutCurrentChangeRateList[512];//ʻ�������仯�ʱ�

  WORD    InContinuedSeconds;         //ʻ�����ʱ��
  WORD    ChangeContinuedSeconds;     //�������ʱ��
  WORD    OutContinuedSeconds;        //ʻ������ʱ��
	WORD    StartupDelaySecond;         //�г�����ʱ��

  CTime   InBeginTime;                //ʻ����ʼʱ��
  CTime   InFinishTime;               //ʻ����ֹʱ��
  CTime   ChangeBeginTime;            //������ʼʱ��
  CTime   ChangeFinishTime;           //������ֹʱ��
  CTime   OutBeginTime;               //ʻ����ʼʱ��
  CTime   OutFinishTime;              //ʻ����ֹʱ��

}FREETRAINSTRUCT;

typedef struct TagFaultTransmissionStruct //���ϴ���
{
  BOOL    FaultTransmissionState;         //���ϴ���״̬ 0:���� 1:׼�����ļ� 2: ������
  char    FaultTransmissionDataFileName[256];
	WORD    StartupDelaySecond;             //��������ʱ��
  CTime   BeginTime;                      //�г���ʼʱ��
}FAULTTRANSMISSIONTRUCT;

typedef struct TagSimTrainStruct          //�����г�
{
  BOOL    SimTrainState;                  //�����г�״̬ 0:���� 1:׼�����ļ� 2: �г���
  char    SimTrainDataFileName[256];
	WORD    SimTrainDataCurrentPos;
	WORD    SimTrainDataCount;
  WORD    StartupDelaySecond;             //�г�����ʱ��
  CTime   BeginTime;                      //�г���ʼʱ��
}SIMTRAINSTRUCT;

typedef struct TagProtectionSwitchStruct
{
  CString VarName[PROTECTION_SWITCH_COUNT];
  CString TextFormat[PROTECTION_SWITCH_COUNT];
  float Value[PROTECTION_SWITCH_COUNT];
}PROTECTIONSWITCHSTRUCT;//����ѹ��ṹ

typedef struct TagProtectionParameterStruct
{
  CString VarName[PROTECTION_PARAMETER_COUNT];
  CString TextFormat[PROTECTION_PARAMETER_COUNT];
  float Value[PROTECTION_PARAMETER_COUNT];
}PROTECTIONPARAMETERSTRUCT;//������ֵ�ṹ

typedef struct TagProtectionActionValueStruct
{
  CString VarName[PROTECTION_ACTION_VALUE_COUNT];
  CString TextFormat[PROTECTION_ACTION_VALUE_COUNT];
  float Value[PROTECTION_ACTION_VALUE_COUNT];
}PROTECTIONACTIONVALUESTRUCT;//��������ֵ�ṹ

typedef struct TagProtectionSampleStruct
{
  CString ChannelName[WAVE_CHANNEL_COUNT];
  float ChannelValue[WAVE_CHANNEL_COUNT][WAVE_SAMPLE_COUNT];
}PROTECTIONSAMPLESTRUCT;//���������ṹ

typedef struct TagProtectionCalculaterStruct
{
  WORD ProtectionID;          //����װ�ñ��
  WORD ProtectionType;        //����װ������
  WORD ProtectionActionType;  //������������ 0Ϊ�޶���
  WORD ProtectionActionTime;  //��������ʱ��

  TagProtectionSampleStruct ProtectionSample;           //����������������
  TagProtectionSwitchStruct ProtectionSwitch;           //����ѹ��
  TagProtectionParameterStruct ProtectionParameter;     //������ֵ
  TagProtectionActionValueStruct ProtectionActionValue;  //��������ֵ
  BOOL ProtectionActionBreakerTrip[3];                    //����������բ���
  BOOL ProtectionActionBreakerReclose;                    //���������غ�բ���
}PROTECTIONCALCULATERSTRUCT;//��������ṹ

typedef struct TagPresetVarStruct //Ԥ�ñ���
{
  WORD UnitID;
  WORD VarGroupID;
  WORD VarID;
  float PresetValue;
  CTime PresetTime;
}PRESETVARSTRUCT;

typedef struct TagSceneOperateStruct //��������
{
  CString SceneName;
  CString SceneDescribe;
  
  BOOL  EnableFreeTrain;        //�����г�������Ч
  BOOL  EnableSimTrain;         //�����г�������Ч
  BOOL  EnableFaultTransmission;//���洫��������Ч
  BOOL  EnableFreeTransmission; //���ɴ���������Ч
  BOOL  EnableVar[UNITCOUNT][VARCOUNT]; //ң�ű���������Ч
  BOOL  EnableParameterVar[UNITCOUNT][PARAMETERGROUPCOUNT][PARAMETERCOUNT]; //��ֵ����������Ч

  TagInlineVotlageStruct      InlineVoltage;
  TagPTVotlageStruct          PTVoltage;
  TagCurrentStruct            Current;
  TagCompensationStruct       Compensation[2];

  TagFreeTransmissionStruct   FreeTransmission;   //���ɴ���
  TagFreeTrainStruct          FreeTrain;          //�����г�
  TagFaultTransmissionStruct  FaultTransmission;  //���ϴ���
  TagSimTrainStruct           SimTrain;           //�����г�

  WORD CurrentParamaterGroupID[UNITCOUNT];  //��ǰ��ֵ���
  WORD VarLock[UNITCOUNT][VARCOUNT];        //�ܶ�
  WORD VarAbnormal[VARCOUNT][VARCOUNT];     //�쳣
  float VarValue[UNITCOUNT][VARCOUNT];
  float ParameterVarValue[UNITCOUNT][PARAMETERGROUPCOUNT][PARAMETERCOUNT];

  CPtrList ScenePictureList;
  CPtrList SceneSoundList;

  LONG TotalScore;
  LONG CorrectScore;

}SCENEOPERATESTRUCT;

typedef struct TagScenePictureStruct  //����ͼʾ
{
  CString ScenePrompt;            //��ʾ
  CString SceneNote;              //˵��
  CString ScenePictureName[2];
  CString SceneVideoName;
  CString VarOperate;             //ң�Ų���
  CString VarWait;                //ң�ŵȴ�
  CString VarConfirm;             //ң��ȷ��
  CString SceneMultiSelectOperate;//��ѡ����
  CString SceneMessageShowOperate;//��ʾ����
  CString SceneDelayOperate;      //��ʱ����
  CString SceneParameterOperate;  //��ֵ����
  CString SceneInlineVotlage;     //��ѹ����
  CString ScenePTVotlage;         //ѹ������
  CString SceneCurrent;           //��������
  CString SceneCompensation;      //��������
  CString SceneTransmission;      //��������
  CString SceneTrain;             //�г�����
  CString SceneFileLoad;          //��������

  WORD    SceneType;              //0:ң�ز��� 1:�͵ز��� 2:ȷ�ϲ���
  WORD    SceneUnitID;
  WORD    SceneVarID;
  WORD    SceneTarge;             //0:��ѡ��/���� 1:��ѡ��/����
  WORD    SceneOperateError;      //��������
  WORD    SceneCorrectScore;      //��ȷ�����÷�
  WORD    SceneErrorScore;        //��������۷�
  WORD    SceneOperateUsedSeconds;//������ʱ
}SCENEPICTURE;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__04129C82_3A73_4A54_BCE6_931560C71AF2__INCLUDED_)
