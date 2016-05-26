////城轨牵引降压混合变电所继电保护仿真程序(AC35kV/AC400V/DC750V)
#include "stdafx.h"
#include "ProtectionCalculater.h"
#define	PI 3.141592654
#define	number_of_samples 32		//每周波采样点数,采样点间隔为0.625ms

typedef struct TagTimerStruct		//计时器
{
	BOOL  StartupState;				//启动状态
	DWORD StartupTickCount;			//启动计数
}TIMERSTRUCT;

static struct TagTimerStruct g_LossVoltageXLTimer[UNITCOUNT];		// 失压计时器(用于中压线路保护)
static struct TagTimerStruct g_OverLoadXLTimer[UNITCOUNT];			// 过负荷计时器(用于中压线路保护)
static struct TagTimerStruct g_LossVoltageDKXTimer[UNITCOUNT];		// 低电压计时器(用于直流馈线保护)
static struct TagTimerStruct g_OverLoadDKXTimer[UNITCOUNT];			// 过负荷计时器(用于直流馈线保护)
static struct TagTimerStruct g_OverCurrentDKXTimer[UNITCOUNT];		// 过电流计时器(用于直流馈线保护)
static struct TagTimerStruct g_RecloserDKXTimer[UNITCOUNT];			// 重合闸计时器(用于直流馈线保护)
static struct TagTimerStruct g_LeakVoltageKJTimer[UNITCOUNT];		// 泄漏电压计时器(用于直流框架保护)

static BOOL ActiveCurrentQuickBreakKX=FALSE;						// 用于重合闸后跳闸判断
static BOOL ActiveDDLKX=FALSE;
static BOOL ActiveOverCurrentKX=FALSE;
static BOOL ActiveBilateralBreakKX=FALSE;						
static BOOL ForeverShort=FALSE;										// 馈线发生短路故障的性质,0:瞬时性短路,1:永久性短路

BOOL StartupTimer(TagTimerStruct *pTimer)
{
	if(pTimer->StartupState==TRUE) return FALSE;
	pTimer->StartupState=TRUE;										//设置计时器启动状态
	pTimer->StartupTickCount=::GetTickCount();						//设置计时器启动时间
				
	return TRUE;
}

BOOL ClearTimer(TagTimerStruct *pTimer)
{
	if(pTimer->StartupState==FALSE) return FALSE;
	pTimer->StartupState=FALSE;										//清除计时器状态
	pTimer->StartupTickCount=0;
				
	return TRUE;
}

BOOL CalculaterValue(float SampleValueList[WAVE_SAMPLE_COUNT],WORD BeginPosition,float &FundamentalWaveValue)	//计算从BeginPosition开始的前32个采样点的基波值
{
	if(BeginPosition<32) return FALSE;
				
	float sintab[32],costab[32];
	int sampcount=32;
	int i,temp;

	for(i=0;i<sampcount;i++)
	{
		temp=i;
		sintab[i]=(float)sin(2*(double)3.14159265*(double)temp/(double)sampcount);		//sin(2*pi*k/32)
	}
	for(i=0;i<sampcount;i++)
	{
		temp=i;
		costab[i]=(float)cos(2*(double)3.14159265*(double)temp/(double)sampcount);
	}
	
	float Re=0,Im=0,Rms=0;
	int j=0;

	for(i=0;i<1;i++)
	{
		while(j/sampcount==i)
		{
			float *Value=&SampleValueList[j+BeginPosition-sampcount];
			Im+=*Value*sintab[j];
			Re+=*Value*costab[j];
			j++;
		}
	}
	Rms=(float)sqrt(Re*Re+Im*Im)/(float)sampcount;
	Rms=(float)1.41421356*Rms;
	FundamentalWaveValue=Rms;

	return TRUE;
}

BOOL CalculaterValue(float SampleValueList[WAVE_SAMPLE_COUNT],WORD BeginPosition,float &FundamentalWaveValue,float &FundamentalWaveValuejd)
{		
	if(BeginPosition<32) return FALSE;
				
	float sintab[32],costab[32];
	int sampcount=32;
	int i,temp;

	for(i=0;i<sampcount;i++)
	{
		temp=i;
		sintab[i]=(float)sin(2*(double)3.14159265*(double)temp/(double)sampcount);
	}
	for(i=0;i<sampcount;i++)
	{
		temp=i;
		costab[i]=(float)cos(2*(double)3.14159265*(double)temp/(double)sampcount);
	}
					
	float Re=0,Im=0,Rms=0,Rjd=0;
	int j=0;

	for(i=0;i<1;i++)
	{
		while(j/sampcount==i)
		{
			float *Value=&SampleValueList[j+BeginPosition-32];
			Im+=*Value*sintab[j];
			Re+=*Value*costab[j];
			j++;
		}
	}
	Rms=(float)sqrt(Re*Re+Im*Im)/(float)sampcount;
	Rms=(float)1.41421356*Rms;
	Rjd=(float)atan2(Im,Re);
	FundamentalWaveValue=Rms;
	FundamentalWaveValuejd=Rjd;
				
	return TRUE;
}

BOOL CalculaterValue(float SampleValueList[WAVE_SAMPLE_COUNT],WORD BeginPosition,int Harmonic,float &FundamentalWaveValue)	//计算从BeginPosition开始的前32个采样点的Harmonic次谐波值
{
	if(BeginPosition<32) return FALSE;

	float sintab[32],costab[32];
	int sampcount=32;	
	int i,temp;

	for(i=0;i<sampcount;i++)
	{
		temp=i;
		sintab[i]=(float)sin(2*(double)3.14159265*(double)temp*Harmonic/(double)sampcount);
	}
	for(i=0;i<sampcount;i++)
	{
		temp=i;
		costab[i]=(float)cos(2*(double)3.14159265*(double)temp*Harmonic/(double)sampcount);
	}
				
	float Re=0,Im=0,Rms=0;
	int j=0;

	for(i=0;i<1;i++)
	{
		while(j/sampcount==i)
		{
			float *Value=&SampleValueList[j+BeginPosition-sampcount];
			Im+=*Value*sintab[j];
			Re+=*Value*costab[j];
			j++;
		}
	}
	Rms=(float)sqrt(Re*Re+Im*Im)/(float)sampcount;
	Rms=(float)1.41421356*Rms;
	FundamentalWaveValue=Rms;

	return TRUE;
}

BOOL CalculaterValue(float SampleValueList[WAVE_SAMPLE_COUNT],WORD BeginPosition,float &DDValue,float &DCValue,BOOL Result)	//计算从BeginPosition开始的直流量
{
	if(BeginPosition<32) return FALSE;

	DDValue=SampleValueList[BeginPosition]-SampleValueList[BeginPosition-1];
	DCValue=SampleValueList[BeginPosition];

	return TRUE;
}

BOOL WINAPI ProtectionCalculater(TagProtectionCalculaterStruct *pProtectionCalculater)
{
	switch(pProtectionCalculater->ProtectionType)
	{
		case 1:														//中压线路保护
		case 3:														//中压变压器保护
		case 4:														//中压整流机组保护
		case 8:														//低压线路保护
			ProtectionCalculater1(pProtectionCalculater);
			break;
		case 5:														//直流进线保护
		case 6:														//直流馈线保护
			ProtectionCalculater2(pProtectionCalculater);
			break;
		case 7:														//直流框架保护
			ProtectionCalculater3(pProtectionCalculater);
			break;
	}
	
	return TRUE;
}

BOOL WINAPI ProtectionCalculater1(TagProtectionCalculaterStruct *pProtectionCalculater)		//交流保护,采样通道顺序:Ua,Ub,Uc,Ia,Ib,Ic,I0
{
//读取保护压板
	BOOL CurrentQuickBreakOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];				//电流速断保护压板
	BOOL OverCurrentSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];			//过电流1段保护压板
	BOOL OverCurrentSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[2];			//过电流2段保护压板
	BOOL SpillCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[3];					//线路纵差保护压板
	BOOL ZeroOverCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[4];				//零序过流保护压板
	BOOL LossVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[5];					//失压保护压板
	BOOL OverLoadOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[6];						//过负荷保护压板
	BOOL OverLoadCKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[7];					//0:过负荷告警,1:过负荷跳闸
	BOOL OverLoadSXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[8];					//0:过负荷定时限,1:过负荷反时限
	BOOL ZeroSequenceMeasure=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[9];				//0:零序过滤器,1:零序互感器
	BOOL PTUnusualOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[30];					//PT异常检测
//读取定值
	float PTFValue=pProtectionCalculater->ProtectionParameter.Value[0]*(float)0.001;				//馈线PT变比
	float CTFValue=pProtectionCalculater->ProtectionParameter.Value[1];								//馈线CT变比
	float CTZValue=pProtectionCalculater->ProtectionParameter.Value[2];								//零序CT变比
	float CurrentMutationValue=pProtectionCalculater->ProtectionParameter.Value[3];					//电流突变量启动定值
	float CurrentQuickBreakAValue=pProtectionCalculater->ProtectionParameter.Value[4];				//A相速断电流定值
	float CurrentQuickBreakBValue=pProtectionCalculater->ProtectionParameter.Value[5];				//B相速断电流定值
	float CurrentQuickBreakCValue=pProtectionCalculater->ProtectionParameter.Value[6];				//C相速断电流定值
	float CurrentQuickBreakTime=pProtectionCalculater->ProtectionParameter.Value[7]*1000;			//电流速断时间定值
	float OverCurrentSection1AValue=pProtectionCalculater->ProtectionParameter.Value[8];			//A相过流1段电流定值
	float OverCurrentSection1BValue=pProtectionCalculater->ProtectionParameter.Value[9];			//B相过流1段电流定值
	float OverCurrentSection1CValue=pProtectionCalculater->ProtectionParameter.Value[10];			//C相过流1段电流定值
	float OverCurrentSection1Time=pProtectionCalculater->ProtectionParameter.Value[11]*1000;		//过流1段时间定值
	float OverCurrentSection2AValue=pProtectionCalculater->ProtectionParameter.Value[12];			//A相过流2段电流定值
	float OverCurrentSection2BValue=pProtectionCalculater->ProtectionParameter.Value[13];			//B相过流2段电流定值
	float OverCurrentSection2CValue=pProtectionCalculater->ProtectionParameter.Value[14];			//C相过流2段电流定值
	float OverCurrentSection2Time=pProtectionCalculater->ProtectionParameter.Value[15]*1000;		//过流2段时间定值
	float LossVoltageValue=pProtectionCalculater->ProtectionParameter.Value[16];					//失压保护定值
	float LossVoltageTime=pProtectionCalculater->ProtectionParameter.Value[17]*1000;				//失压时间定值
	float OverLoadAValue=pProtectionCalculater->ProtectionParameter.Value[18];						//A相过负荷电流定值
	float OverLoadBValue=pProtectionCalculater->ProtectionParameter.Value[19];						//B相过负荷电流定值
	float OverLoadCValue=pProtectionCalculater->ProtectionParameter.Value[20];						//C相过负荷电流定值
	float OverLoadTime=pProtectionCalculater->ProtectionParameter.Value[21]*1000;					//过负荷时间定值
	float ZeroOverCurrentValue=pProtectionCalculater->ProtectionParameter.Value[22];				//零序过流电流定值
	float ZeroOverCurrentTime=pProtectionCalculater->ProtectionParameter.Value[23]*1000;			//零序过流时间定值
				
	BOOL ActiveCurrentQuickBreak=FALSE;		//电流速断动作标志		1
	BOOL ActiveOverCurrentSection1=FALSE;	//过电流1段动作标志		2
	BOOL ActiveOverCurrentSection2=FALSE;	//过电流2段动作标志		3
	BOOL ActiveLossVoltage=FALSE;			//失压动作标志			4
	BOOL ActiveOverLoad=FALSE;				//过负荷动作标志		5
	BOOL ActiveZeroOverCurrent=FALSE;		//零序过流动作标志		6
	BOOL ActiveSpillCurrent=FALSE;			//线路纵差动作标志		7
	BOOL ActivePT=FALSE;					//PT异常标志			18
				
	int CurrentQuickBreakJS=0;				//电流速断计数器
	int OverCurrentSection1JS=0;			//过电流1段计数器
	int OverCurrentSection2JS=0;			//过电流2段计数器
	int ZeroOverCurrentJS=0;				//零序过流计数器
	int SpillCurrentJS=0;					//馈线纵差计数器

	WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;

	if((CurrentQuickBreakOnOff==TRUE)&&(ActiveType==0))		//电流速断保护投入
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float IaValue,IbValue,IcValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);	//计算Ia
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);	//计算Ib
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);	//计算Ic
			IaValue/=CTFValue;
			IbValue/=CTFValue;
			IcValue/=CTFValue;

			if((IaValue>CurrentQuickBreakAValue)||(IbValue>CurrentQuickBreakBValue)||(IcValue>CurrentQuickBreakCValue))
			{
				if(CurrentQuickBreakJS==int(CurrentQuickBreakTime*0.1))
				{	
					ActiveCurrentQuickBreak=TRUE;
					ActiveType=1;
					ActiveTime=(WORD)CurrentQuickBreakTime;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;							//线路断路器动作跳闸
				}
				CurrentQuickBreakJS+=1;
			}
		}
	}

    if((OverCurrentSection1OnOff==TRUE)||(OverCurrentSection2OnOff==TRUE))		//过电流保护投入
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float IaValue,IbValue,IcValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);	//计算Ia
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);	//计算Ib
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);	//计算Ic
			IaValue/=CTFValue;
			IbValue/=CTFValue;
			IcValue/=CTFValue;

			if((OverCurrentSection1OnOff==TRUE)&&(ActiveType==0))										//过电流1段保护投入
			{
				if((IaValue>OverCurrentSection1AValue)||(IbValue>OverCurrentSection1BValue)||(IcValue>OverCurrentSection1CValue))
				{
					if(OverCurrentSection1JS==int(OverCurrentSection1Time*0.1))	
					{
						ActiveOverCurrentSection1=TRUE;
						ActiveType=2;
						ActiveTime=(WORD)OverCurrentSection1Time;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//线路断路器动作跳闸
					}
					OverCurrentSection1JS+=1;
				}
			}
			if((OverCurrentSection2OnOff==TRUE)&&(ActiveType==0))										//过电流2段保护投入
			{
				if((IaValue>OverCurrentSection2AValue)||(IbValue>OverCurrentSection2BValue)||(IcValue>OverCurrentSection2CValue))
				{
					if(OverCurrentSection2JS==int(OverCurrentSection2Time*0.1))
					{
						ActiveOverCurrentSection2=TRUE;
						ActiveType=3;
						ActiveTime=(WORD)OverCurrentSection2Time;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//线路断路器动作跳闸
					}
					OverCurrentSection2JS+=1;
				}
			}
		}
	}

	if((ZeroOverCurrentOnOff==TRUE)&&(ActiveType==0))		//零序过流保护投入
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float IaValue,IbValue,IcValue,I0Value;
			float IaValuejd,IbValuejd,IcValuejd; 

			if(ZeroSequenceMeasure==FALSE)
			{
				CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue,IaValuejd);
				CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue,IbValuejd);
				CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue,IcValuejd);
				I0Value=(float)(sqrt(pow(IaValue*cos(IaValuejd)+IbValue*cos(IbValuejd)+IcValue*cos(IcValuejd),2)+
					pow(IaValue*sin(IaValuejd)+IbValue*sin(IbValuejd)+IcValue*sin(IcValuejd),2))/3.0);
				I0Value/=CTZValue;
			}
			else
			{
				CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,I0Value);
				I0Value/=CTZValue;
			}

			if(I0Value>ZeroOverCurrentValue)
			{
				if(ZeroOverCurrentJS==int(ZeroOverCurrentTime*0.1))
				{	
					ActiveZeroOverCurrent=TRUE;
					ActiveType=6;
					ActiveTime=(WORD)ZeroOverCurrentTime;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;							//线路断路器动作跳闸
				}
				ZeroOverCurrentJS+=1;
			}
		}
	}

	if((SpillCurrentOnOff==TRUE)&&(ActiveType==0))			//线路纵差保护投入
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

		}
	}

	if((LossVoltageOnOff==TRUE)&&(ActiveType==0))			//失压保护投入
	{
		float UaValue,UbValue,UcValue,IaValue,IbValue,IcValue;		
		int sampno=WAVE_SAMPLE_COUNT;						//取最后一个周波

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UaValue);		//计算Ua
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UbValue);		//计算Ub
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UcValue);		//计算Uc
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);		//计算Ia
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);		//计算Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);		//计算Ic
		UaValue/=PTFValue;
		UbValue/=PTFValue;
		UcValue/=PTFValue;
		IaValue/=CTFValue;
		IbValue/=CTFValue;
		IcValue/=CTFValue;

		if(((UaValue<LossVoltageValue)&&(IaValue<0.4))||((UbValue<LossVoltageValue)&&(IbValue<0.4))||((UcValue<LossVoltageValue)&&(IcValue<0.4)))														//电压小于失压定值
		{
			if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0) ActiveLossVoltage=TRUE;	//断路器在合位
			else ActiveLossVoltage=FALSE;
		}
		else ActiveLossVoltage=FALSE;

		if(ActiveLossVoltage==FALSE)
		{
			ClearTimer(&g_LossVoltageXLTimer[pProtectionCalculater->ProtectionID]);
		}
		else
		{
			if(g_LossVoltageXLTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_LossVoltageXLTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				BOOL TimerOut=FALSE;
				if((::GetTickCount()-g_LossVoltageXLTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(LossVoltageTime)) TimerOut=TRUE;
				if(TimerOut==TRUE)
				{
					ClearTimer(&g_LossVoltageXLTimer[pProtectionCalculater->ProtectionID]);
					ActiveType=4;
					ActiveTime=(WORD)::GetTickCount()%1000;
					ActionPosition=160+(WORD)::GetTickCount()%120;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;
				}
			}
		}
	}

	if((OverLoadOnOff==TRUE)&&(ActiveType==0))				//过负荷保护投入
	{
		float IaValue,IbValue,IcValue;
		int sampno=WAVE_SAMPLE_COUNT;						//取最后一个周波

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);	//计算Ia
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);	//计算Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);	//计算Ic
		IaValue/=CTFValue;
		IbValue/=CTFValue;
		IcValue/=CTFValue;

		float TimePhasea=100000;
		float TimePhaseb=100000;
		float TimePhasec=100000;
		BOOL ActivePhasea=FALSE;
		BOOL ActivePhaseb=FALSE;
		BOOL ActivePhasec=FALSE;
		
		if(IaValue>OverLoadAValue)
		{
			ActivePhasea=TRUE;
			if(OverLoadSXOnOff==TRUE)
			{
				TimePhasea=(float)0.14*OverLoadTime*(IaValue/OverLoadAValue-(float)1.0);
			}
		}
		if(IbValue>OverLoadBValue)
		{
			ActivePhaseb=TRUE;
			if(OverLoadSXOnOff==TRUE)
			{
				TimePhaseb=(float)0.14*OverLoadTime*(IbValue/OverLoadBValue-(float)1.0);
			}
		}
		if(IcValue>OverLoadCValue)
		{
			ActivePhasec=TRUE;
			if(OverLoadSXOnOff==TRUE)
			{
				TimePhasec=(float)0.14*OverLoadTime*(IcValue/OverLoadCValue-(float)1.0);
			}
		}
		if(ActivePhasea||ActivePhaseb||ActivePhasec)
		{
			ActiveOverLoad=TRUE;
			if(OverLoadSXOnOff==TRUE) OverLoadTime=min(min(TimePhasea,TimePhaseb),TimePhasec);
			else OverLoadTime=pProtectionCalculater->ProtectionParameter.Value[21]*1000;
		}
		else ActiveOverLoad=FALSE;

		if(ActiveOverLoad==FALSE)
		{
			ClearTimer(&g_OverLoadXLTimer[pProtectionCalculater->ProtectionID]);
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=0;
		}
		else
		{
			if(g_OverLoadXLTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_OverLoadXLTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				BOOL TimerOut=FALSE;
				if((::GetTickCount()-g_OverLoadXLTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(OverLoadTime)) TimerOut=TRUE;
				if(TimerOut==TRUE)
				{
					ClearTimer(&g_OverLoadXLTimer[pProtectionCalculater->ProtectionID]);
					if(OverLoadCKOnOff==TRUE)
					{
						ActiveType=5;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;					//线路断路器动作跳闸
					}
					else
					{
						ActiveType=22;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;				//保护动作告警
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=1;
					}
				}
			}	
		}	
	}

	if(ActiveType!=0)		//计算保护动作信息
	{
		pProtectionCalculater->ProtectionActionType=ActiveType;
		pProtectionCalculater->ProtectionActionTime=ActiveTime;

		float ActionValueUa,ActionValueUb,ActionValueUc,ActionValueIa,ActionValueIb,ActionValueIc,ActionValueI0;
		float ActionValueUab,ActionValueUbc,ActionValueUca;
		float ActionValueUajd,ActionValueUbjd,ActionValueUcjd,ActionValueIajd,ActionValueIbjd,ActionValueIcjd;
		float ActionValueP,ActionValueQ,ActionValuecos;
			
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,ActionValueUa,ActionValueUajd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,ActionValueUb,ActionValueUbjd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],ActionPosition,ActionValueUc,ActionValueUcjd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],ActionPosition,ActionValueIa,ActionValueIajd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],ActionPosition,ActionValueIb,ActionValueIbjd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],ActionPosition,ActionValueIc,ActionValueIcjd);
		if(ZeroSequenceMeasure==FALSE)
		{
			ActionValueI0=(float)(sqrt(pow(ActionValueIa*cos(ActionValueIajd)+ActionValueIb*cos(ActionValueIbjd)+ActionValueIc*cos(ActionValueIcjd),2)+
				pow(ActionValueIa*sin(ActionValueIajd)+ActionValueIb*sin(ActionValueIbjd)+ActionValueIc*sin(ActionValueIcjd),2))/3.0);
		}
		else CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],ActionPosition,ActionValueI0);
		ActionValueUa/=PTFValue;
		ActionValueUb/=PTFValue;
		ActionValueUc/=PTFValue;
		ActionValueIa/=CTFValue;
		ActionValueIb/=CTFValue;
		ActionValueIc/=CTFValue;
		ActionValueI0/=CTZValue;
		ActionValueUab=(float)sqrt(pow(ActionValueUa*cos(ActionValueUajd)-ActionValueUb*cos(ActionValueUbjd),2)+
			pow(ActionValueUa*sin(ActionValueUajd)-ActionValueUb*sin(ActionValueUbjd),2));
		ActionValueUbc=(float)sqrt(pow(ActionValueUb*cos(ActionValueUbjd)-ActionValueUc*cos(ActionValueUcjd),2)+
			pow(ActionValueUb*sin(ActionValueUbjd)-ActionValueUc*sin(ActionValueUcjd),2));
		ActionValueUca=(float)sqrt(pow(ActionValueUc*cos(ActionValueUcjd)-ActionValueUa*cos(ActionValueUajd),2)+
			pow(ActionValueUc*sin(ActionValueUcjd)-ActionValueUa*sin(ActionValueUajd),2));
		ActionValueP=ActionValueUa*ActionValueIa*(float)cos(ActionValueUajd-ActionValueIajd)+
			ActionValueUb*ActionValueIb*(float)cos(ActionValueUbjd-ActionValueIbjd)+
			ActionValueUc*ActionValueIc*(float)cos(ActionValueUcjd-ActionValueIcjd);
		ActionValueQ=ActionValueUa*ActionValueIa*(float)sin(ActionValueUajd-ActionValueIajd)+
			ActionValueUb*ActionValueIb*(float)sin(ActionValueUbjd-ActionValueIbjd)+
			ActionValueUc*ActionValueIc*(float)sin(ActionValueUcjd-ActionValueIcjd);
		ActionValuecos=(float)cos(ActionValueUajd-ActionValueIajd);

		pProtectionCalculater->ProtectionActionValue.Value[0]=ActionValueUa;
		pProtectionCalculater->ProtectionActionValue.Value[1]=ActionValueUb;
		pProtectionCalculater->ProtectionActionValue.Value[2]=ActionValueUc;
		pProtectionCalculater->ProtectionActionValue.Value[3]=ActionValueIa;
		pProtectionCalculater->ProtectionActionValue.Value[4]=ActionValueIb;
		pProtectionCalculater->ProtectionActionValue.Value[5]=ActionValueIc;
		pProtectionCalculater->ProtectionActionValue.Value[6]=ActionValueI0;
		pProtectionCalculater->ProtectionActionValue.Value[7]=ActionValueUab;
		pProtectionCalculater->ProtectionActionValue.Value[8]=ActionValueUbc;
		pProtectionCalculater->ProtectionActionValue.Value[9]=ActionValueUca;
		pProtectionCalculater->ProtectionActionValue.Value[10]=ActionValueP;
		pProtectionCalculater->ProtectionActionValue.Value[11]=ActionValueQ;
		pProtectionCalculater->ProtectionActionValue.Value[12]=ActionValuecos;
		if(pProtectionCalculater->ProtectionActionBreakerTrip[0]==TRUE)
		{
			for(int sampno=ActionPosition;sampno<WAVE_SAMPLE_COUNT;sampno++)
			{
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno]=0;		//a相电压
				pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno]=0;		//b相电压
				pProtectionCalculater->ProtectionSample.ChannelValue[2][sampno]=0;		//c相电压
				pProtectionCalculater->ProtectionSample.ChannelValue[3][sampno]=0;		//a相电流
				pProtectionCalculater->ProtectionSample.ChannelValue[4][sampno]=0;		//b相电流
				pProtectionCalculater->ProtectionSample.ChannelValue[5][sampno]=0;		//c相电流
				pProtectionCalculater->ProtectionSample.ChannelValue[6][sampno]=0;		//零序电流
			}
		}

		return TRUE;
	}

	if(ActiveLossVoltage==FALSE) ClearTimer(&g_LossVoltageXLTimer[pProtectionCalculater->ProtectionID]);	//清除失压计时器
	if(ActiveOverLoad==FALSE) ClearTimer(&g_OverLoadXLTimer[pProtectionCalculater->ProtectionID]);			//清除过负荷计时器
	
	return TRUE;
}

BOOL WINAPI ProtectionCalculater2(TagProtectionCalculaterStruct *pProtectionCalculater)		//直流保护,采样通道顺序:Ud,Id
{
//读取保护压板
	BOOL CurrentQuickBreakOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];			//电流速断保护压板
	BOOL DDLOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];						//电流上升率及电流增量保护压板
	BOOL OverCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[2];				//过电流保护压板
	BOOL LossVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[3];				//低电压保护压板
	BOOL OverLoadOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[4];					//过负荷保护压板
	BOOL BilateralBreakOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[5];			//双边联跳保护压版
	BOOL RecloserOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[6];					//重合闸压板
	BOOL OverLoadCKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[7];				//0:过负荷告警,1:过负荷跳闸
	BOOL OverLoadSXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[8];				//0:过负荷反时限投入,1:过负荷定时限投入
//读取定值
	float CurrentQuickBreakValue=pProtectionCalculater->ProtectionParameter.Value[0];			//速断保护电流定值A
	float CurrentQuickBreakTime=pProtectionCalculater->ProtectionParameter.Value[1];			//速断保护时间定值ms
	float DDLEValue=pProtectionCalculater->ProtectionParameter.Value[2]*(float)0.625;			//电流上升率启动定值A/ms
	float DDLFValue=pProtectionCalculater->ProtectionParameter.Value[3]*(float)0.625;			//电流上升率返回定值A/ms
	float DDLDImaxValue=pProtectionCalculater->ProtectionParameter.Value[4];					//最大电流增量定值A
	float DDLDImaxTime=pProtectionCalculater->ProtectionParameter.Value[5];						//最大电流增量延时定值ms
	float DDLDIminValue=pProtectionCalculater->ProtectionParameter.Value[6];					//最小电流增量定值A
	float DDLDIminTime=pProtectionCalculater->ProtectionParameter.Value[7];						//最小电流增量延时定值ms
	float OverCurrentValue=pProtectionCalculater->ProtectionParameter.Value[8];					//过流保护电流定值A
	float OverCurrentTime=pProtectionCalculater->ProtectionParameter.Value[9]*1000;				//过流保护时间定值s
	float LossVoltageValue=pProtectionCalculater->ProtectionParameter.Value[10];				//低电压保护电压定值V
	float LossVoltageTime=pProtectionCalculater->ProtectionParameter.Value[11]*1000;			//低电压保护时间定值s
	float OverLoadValue=pProtectionCalculater->ProtectionParameter.Value[12];					//过负荷电流定值
	float OverLoadTime=pProtectionCalculater->ProtectionParameter.Value[13]*1000;				//过负荷时间定值
	float RecloserTime=pProtectionCalculater->ProtectionParameter.Value[14]*1000;				//重合闸时间定值s
	float RecloserBreakNum=pProtectionCalculater->ProtectionParameter.Value[15];				//重合闸防跳次数
	float TestTime=pProtectionCalculater->ProtectionParameter.Value[16]*1000;					//接触器动作时间定值s
	float TestNum=pProtectionCalculater->ProtectionParameter.Value[17];							//测试次数定值
	float TestDelay=pProtectionCalculater->ProtectionParameter.Value[18]*1000;					//测试时间间隔定值s
	float TestUflow=pProtectionCalculater->ProtectionParameter.Value[19];						//线路最低工作电压定值
	float TestUfres=pProtectionCalculater->ProtectionParameter.Value[20];						//线路残压定值
	float TestRmin=pProtectionCalculater->ProtectionParameter.Value[21];						//测试电阻最小值定值
	float TestUcalib=pProtectionCalculater->ProtectionParameter.Value[22];						//
	float TestRcalib=pProtectionCalculater->ProtectionParameter.Value[23];						//

	BOOL ActiveCurrentQuickBreak=FALSE;			//电流速断动作标志		1
	BOOL ActiveDDL=FALSE;						//DDL动作标志			2
	BOOL ActiveOverCurrent=FALSE;				//过流动作标志			3
	BOOL ActiveLossVoltage=FALSE;				//低电压动作标志		4
	BOOL ActiveOverLoad=FALSE;					//过负荷动作标志		5
	BOOL ActiveBilateralBreak=FALSE;			//双边联跳动作标志		6
	BOOL ActiveRecloser=FALSE;					//重合闸动作标志		7
	BOOL ActiveLineTest=FALSE;					//线路测试标志			
	
	int CurrentQuickBreakJS=0;
	int DDLJS=0;
	int OverCurrentJS=0;

	static float TempChannelValue[2][320];

	WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;

	if((CurrentQuickBreakOnOff==TRUE)&&(ActiveType==0))			//电流速断保护投入
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float didtValue,IValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);

			if(IValue>CurrentQuickBreakValue)
			{
				if(CurrentQuickBreakJS==int(CurrentQuickBreakTime*1.6))
				{	
					ActiveCurrentQuickBreak=TRUE;
					ActiveCurrentQuickBreakKX=TRUE;
					ActiveType=1;
					ActiveTime=(WORD)CurrentQuickBreakTime;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//馈线断路器动作跳闸
				}
				CurrentQuickBreakJS+=1;
			}
		}
	}

	if((DDLOnOff==TRUE)&&(ActiveType==0))			//电流上升率及电流增量保护投入
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float didtValue,IValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);

			if(didtValue>DDLEValue)
			{
				if(DDLJS==int(DDLDImaxTime*1.6))
				{	
					ActiveDDL=TRUE;
					ActiveDDLKX=TRUE;
					ActiveType=2;
					ActiveTime=(WORD)DDLDImaxTime;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//馈线断路器动作跳闸
				}
				DDLJS+=1;
			}
		}
		for(sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float didtValue,IValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);

			if(didtValue>DDLEValue)
			{
				int delay;
				float didtValue1,IValue1;
				delay=sampno+int(DDLDImaxTime*1.6);
				CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],delay,didtValue1,IValue1,TRUE);
				if((IValue1-IValue)>DDLDImaxValue)
				{
					ActiveDDL=TRUE;
					ActiveDDLKX=TRUE;
					ActiveType=2;
					ActiveTime=(WORD)DDLDImaxTime;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//馈线断路器动作跳闸
				}
			}
		}
		for(sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float didtValue,IValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);

			if(didtValue>DDLEValue)
			{
				int delay;
				float didtValue1,IValue1;
				delay=sampno+int(DDLDIminTime*1.6);
				CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],delay,didtValue1,IValue1,TRUE);
				if((IValue1-IValue)>DDLDIminValue)
				{
					ActiveDDL=TRUE;
					ActiveDDLKX=TRUE;
					ActiveType=2;
					ActiveTime=(WORD)DDLDIminTime;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//馈线断路器动作跳闸
				}
			}
		}
	}

	if((OverCurrentOnOff==TRUE)&&(ActiveType==0))			//过电流保护投入
	{
		float didtValue,IValue;
		int sampno=WAVE_SAMPLE_COUNT-1;						//取最后一个周波

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)			//断路器合闸
		{
			if(IValue>OverCurrentValue) ActiveOverCurrent=TRUE;
			else ActiveOverCurrent=FALSE;
		}
		else ActiveOverCurrent=FALSE;

		if(ActiveOverCurrent==FALSE)
		{
			ClearTimer(&g_OverCurrentDKXTimer[pProtectionCalculater->ProtectionID]);
		}
		else
		{
			if(g_OverCurrentDKXTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_OverCurrentDKXTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				if((::GetTickCount()-g_OverCurrentDKXTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(OverCurrentTime))
				{
					ClearTimer(&g_OverCurrentDKXTimer[pProtectionCalculater->ProtectionID]);
					ActiveType=3;
					ActiveTime=(WORD)::GetTickCount()%1000;
					ActionPosition=160+(WORD)::GetTickCount()%120;					
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//馈线断路器动作跳闸
				}
			}
		}
	}

	if((LossVoltageOnOff==TRUE)&&(ActiveType==0))			//低电压保护投入
	{
		float dudtValue,didtValue,UValue,IValue;
		int sampno=WAVE_SAMPLE_COUNT-1;						//取最后一个采样点

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,dudtValue,UValue,TRUE);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)			//断路器合闸
		{
			if((UValue<LossVoltageValue)&&(IValue<0.4)) ActiveLossVoltage=TRUE;
			else ActiveLossVoltage=FALSE;
		}
		else ActiveLossVoltage=FALSE;

		if(ActiveLossVoltage==FALSE)
		{
			ClearTimer(&g_LossVoltageDKXTimer[pProtectionCalculater->ProtectionID]);
		}
		else
		{
			if(g_LossVoltageDKXTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_LossVoltageDKXTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				if((::GetTickCount()-g_LossVoltageDKXTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(LossVoltageTime))
				{
					ClearTimer(&g_LossVoltageDKXTimer[pProtectionCalculater->ProtectionID]);
					ActiveType=4;
					ActiveTime=(WORD)::GetTickCount()%1000;
					ActionPosition=160+(WORD)::GetTickCount()%120;					
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//馈线断路器动作跳闸
				}
			}
		}
	}

	if((OverLoadOnOff==TRUE)&&(ActiveType==0))		//过负荷保护投入
	{
		float didtValue,IValue;
		int sampno=WAVE_SAMPLE_COUNT-1;				//取最后一个周波

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);

		if(IValue>OverLoadValue)
		{
			ActiveOverLoad=TRUE;
			if(OverLoadSXOnOff==TRUE) OverLoadTime=float(0.14*OverLoadTime/(pow(IValue/OverLoadValue,0.02)-1.0));
			else OverLoadTime=pProtectionCalculater->ProtectionParameter.Value[13]*1000;
		}
		else ActiveOverLoad=FALSE;
		if(ActiveOverLoad==FALSE)
		{
			ClearTimer(&g_OverLoadDKXTimer[pProtectionCalculater->ProtectionID]);
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=0;
		}
		else
		{
			if(g_OverLoadDKXTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_OverLoadDKXTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				BOOL TimerOut=FALSE;
				if((::GetTickCount()-g_OverLoadDKXTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(OverLoadTime)) TimerOut=TRUE;
				if(TimerOut==TRUE)
				{
					ClearTimer(&g_OverLoadDKXTimer[pProtectionCalculater->ProtectionID]);
					if(OverLoadCKOnOff==TRUE)
					{
						ActiveType=5;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;
					}
					else
					{
						ActiveType=22;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=1;
					}
				}
			}	
		}	
	}

	if(ActiveType!=0)		//计算保护动作信息
	{
		float didtValue,IValue;		
		int sampno=WAVE_SAMPLE_COUNT-1;
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);
		if(IValue>1.0) ForeverShort=TRUE;													//短路为永久性
		else ForeverShort=FALSE;															//短路为瞬时性

		for(int i=0;i<WAVE_SAMPLE_COUNT;i++)												//暂存采样数据
		{	
			TempChannelValue[0][i]=pProtectionCalculater->ProtectionSample.ChannelValue[0][i];
			TempChannelValue[1][i]=pProtectionCalculater->ProtectionSample.ChannelValue[1][i];
		}

		pProtectionCalculater->ProtectionActionType=ActiveType;
		pProtectionCalculater->ProtectionActionTime=ActiveTime;

		float ActionValueU,ActionValuedudt,ActionValueI,ActionValuedidt;	
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,ActionValuedudt,ActionValueU,TRUE);	//计算U
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,ActionValuedidt,ActionValueI,TRUE);	//计算I

		pProtectionCalculater->ProtectionActionValue.Value[0]=ActionValueU;
		pProtectionCalculater->ProtectionActionValue.Value[1]=ActionValueI;
		pProtectionCalculater->ProtectionActionValue.Value[2]=ActionValuedidt;

		if(pProtectionCalculater->ProtectionActionBreakerTrip[0]==TRUE)
		{
			for(int sampno1=0;sampno1<ActionPosition;sampno1++)
			{	
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno1]=TempChannelValue[0][sampno1];
				pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno1]=TempChannelValue[1][sampno1];
				pProtectionCalculater->ProtectionSample.ChannelValue[2][sampno1]=TempChannelValue[0][sampno1];
			}
			int isampno=0;
			for(int sampno2=ActionPosition;sampno2<WAVE_SAMPLE_COUNT;sampno2++)
			{	
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno2]=TempChannelValue[0][isampno];	//跳闸后母线电压恢复
				pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno2]=0;								//馈线电流
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno2]=0;								//馈线电压
				isampno=isampno+1;
				if(isampno>31) isampno=0;
			}
		}

		if((g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[16].Value>0)&&(RecloserOnOff==TRUE))//重合闸软,硬压板投入
		{	
			if((ActiveType>=1)&&(ActiveType<=3))
			{
				if(ForeverShort==FALSE)																		//线路检测无故障
				{
					if(g_RecloserDKXTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
					{
						StartupTimer(&g_RecloserDKXTimer[pProtectionCalculater->ProtectionID]);				//设置重合闸计时器
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=1;			//置重合闸启动标志
					}
					else g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=1;
					return TRUE;
				}
				else																						//线路检测有故障
				{
					ClearTimer(&g_RecloserDKXTimer[pProtectionCalculater->ProtectionID]);
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=0;
					return TRUE;
				}
			}
		}
		else return TRUE;
	}

	if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value>0)								//重合闸延时重合
	{
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value==0) ActiveRecloser=TRUE;	//馈线断路器已分闸
		else ActiveRecloser=FALSE;

		if(ActiveRecloser==FALSE)																			//重合条件不成立
		{
			ClearTimer(&g_RecloserDKXTimer[pProtectionCalculater->ProtectionID]);
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=0;						//清重合闸启动标志
		}
		else																								//重合条件满足
		{
			if((::GetTickCount()-g_RecloserDKXTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(RecloserTime))
			{
				ClearTimer(&g_RecloserDKXTimer[pProtectionCalculater->ProtectionID]);
				g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=0;					//清重合闸启动标志
				g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=1;					//置重合闸动作标志
				ActiveType=7;
				ActiveTime=(WORD)0;
				pProtectionCalculater->ProtectionActionType=ActiveType;
				pProtectionCalculater->ProtectionActionTime=ActiveTime;											
				g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value=1;						//馈线断路器合闸
				g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[1].Value=0;						//馈线断路器合闸
				pProtectionCalculater->ProtectionActionBreakerReclose=TRUE;									//佟(馈线断路器合闸)

				return TRUE;
			}
		}
	}

	if((ActiveCurrentQuickBreakKX||ActiveDDLKX||ActiveBilateralBreakKX)&&(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value==1))	// 速断和DDL和双边联跳跳闸并重合闸
	{
		g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=0;
		if(ForeverShort==TRUE)																				//永久性短路
		{
			if(ActiveCurrentQuickBreakKX==TRUE) ActiveType=1;
			else if(ActiveDDLKX==TRUE) ActiveType=2;
			else if(ActiveBilateralBreakKX==TRUE) ActiveType=6;
			pProtectionCalculater->ProtectionActionType=ActiveType;
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value=0;							//馈线断路器跳闸
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value=1;							//馈线断路器跳闸
			pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;										//佟(馈线断路器跳闸)
			ActiveCurrentQuickBreakKX=FALSE;
			ActiveDDLKX=FALSE;
			ActiveBilateralBreakKX=FALSE;
		}
		return TRUE;
	}

	if((g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[16].Value==0)||(RecloserOnOff==FALSE))	//重合闸软或硬压板未投入																			//重合闸硬压板退出或重合闸软压板退出
	{
		g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=0;							//清重合闸动作标志
	}

	if(ActiveLossVoltage==FALSE) ClearTimer(&g_LossVoltageDKXTimer[pProtectionCalculater->ProtectionID]);	//清除低电压计时器
	if(ActiveOverLoad==FALSE) ClearTimer(&g_OverLoadDKXTimer[pProtectionCalculater->ProtectionID]);			//清除过负荷计时器
	
	return TRUE;
}

BOOL WINAPI ProtectionCalculater3(TagProtectionCalculaterStruct *pProtectionCalculater)		//框架保护,采样通道顺序:Ul,Il
{
//读取保护压板
	BOOL FrameLeakCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];			//框架泄漏电流保护压板
	BOOL FrameLeakVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];			//框架泄漏电压保护压板
//读取定值
	float FrameLeakCurrentValue=pProtectionCalculater->ProtectionParameter.Value[0];			//框架泄漏电流定值
	float FrameLeakCurrentTime=pProtectionCalculater->ProtectionParameter.Value[1];				//泄漏电流时间定值ms
	float FrameLeakVoltageValue=pProtectionCalculater->ProtectionParameter.Value[2];			//框架泄漏电压定值
	float FrameLeakVoltageTime=pProtectionCalculater->ProtectionParameter.Value[3];				//泄漏电压时间定值ms

	BOOL ActiveFrameLeakCurrent=FALSE;			//框架泄漏电流保护动作标志		1
	BOOL ActiveFrameLeakVoltage=FALSE;			//框架泄漏电压保护动作标志		2

	int FrameLeakCurrentJS=0;

	WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;

	if((FrameLeakCurrentOnOff==TRUE)&&(ActiveType==0))			//框架泄漏电流保护投入
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float didtValue,IValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);

			if(IValue>FrameLeakCurrentValue)
			{
				if(FrameLeakCurrentJS==int(FrameLeakCurrentTime*1.6))
				{	
					ActiveFrameLeakCurrent=TRUE;
					ActiveType=1;
					ActiveTime=(WORD)FrameLeakCurrentTime;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//馈线断路器动作跳闸
				}
				FrameLeakCurrentJS+=1;
			}
		}
	}

	if((FrameLeakVoltageOnOff==TRUE)&&(ActiveType==0))		//框架泄漏电压保护投入
	{
		float dudtValue,UValue;
		int sampno=WAVE_SAMPLE_COUNT-1;						//取最后一个周波

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,dudtValue,UValue,TRUE);
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)			//断路器合闸
		{
			if(UValue>FrameLeakVoltageValue) ActiveFrameLeakVoltage=TRUE;
			else ActiveFrameLeakVoltage=FALSE;
		}
		else ActiveFrameLeakVoltage=FALSE;

		if(ActiveFrameLeakVoltage==FALSE)
		{
			ClearTimer(&g_LeakVoltageKJTimer[pProtectionCalculater->ProtectionID]);
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=0;
		}
		else
		{
			if(g_LeakVoltageKJTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_LeakVoltageKJTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				if((::GetTickCount()-g_LeakVoltageKJTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(FrameLeakVoltageTime))
				{
					ClearTimer(&g_LeakVoltageKJTimer[pProtectionCalculater->ProtectionID]);
					ActiveType=22;
					ActiveTime=(WORD)::GetTickCount()%1000;
					ActionPosition=160+(WORD)::GetTickCount()%120;					
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;				//保护动作告警
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=1;
				}
			}
		}
	}

	if(ActiveType!=0)		//计算保护动作信息
	{
		pProtectionCalculater->ProtectionActionType=ActiveType;
		pProtectionCalculater->ProtectionActionTime=ActiveTime;

		float ActionValueU,ActionValuedudt,ActionValueI,ActionValuedidt;	
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,ActionValuedudt,ActionValueU,TRUE);	//计算U
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,ActionValuedidt,ActionValueI,TRUE);	//计算I

		pProtectionCalculater->ProtectionActionValue.Value[0]=ActionValueU;
		pProtectionCalculater->ProtectionActionValue.Value[1]=ActionValueI;

		if(pProtectionCalculater->ProtectionActionBreakerTrip[0]==TRUE)
		{
			for(int sampno=ActionPosition;sampno<WAVE_SAMPLE_COUNT;sampno++)
			{
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno]=0;
			}
		}

		return TRUE;
	}

	if(ActiveFrameLeakVoltage==FALSE) ClearTimer(&g_LeakVoltageKJTimer[pProtectionCalculater->ProtectionID]);	//清除泄漏电压计时器

	return TRUE;
}
