//城轨主降变电所继电保护仿真程序(AC110kV/AC35kV)
#include "stdafx.h"
#include "ProtectionCalculater.h"
#define	PI 3.141592654
#define	number_of_samples 32		//每周波采样点数,每0.625ms采一个点

typedef struct TagTimerStruct		//计时器
{
	BOOL  StartupState;				//启动状态
	DWORD StartupTickCount;			//启动计数
}TIMERSTRUCT;

static struct TagTimerStruct g_ZeroOverCurrentTimer[UNITCOUNT];		// 零序过流计时器(用于变压器差动保护)
static struct TagTimerStruct g_OverLoadSection1Timer[UNITCOUNT];	// 过负荷1段计时器(用于变压器后备保护)
static struct TagTimerStruct g_OverLoadSection2Timer[UNITCOUNT];	// 过负荷2段计时器(用于变压器后备保护)
static struct TagTimerStruct g_LossVoltageTimer[UNITCOUNT];			// 失压计时器(用于变压器后备保护)
static struct TagTimerStruct g_LossVoltageJXTimer[UNITCOUNT];		// 失压计时器(用于中压进线保护)
static struct TagTimerStruct g_OverLoadJXTimer[UNITCOUNT];			// 过负荷计时器(用于中压进线保护)
static struct TagTimerStruct g_ZeroOverCurrentJXTimer[UNITCOUNT];	// 零序过流计时器(用于中压进线保护)
static struct TagTimerStruct g_LossVoltageXLTimer[UNITCOUNT];		// 失压计时器(用于中压馈线保护)
static struct TagTimerStruct g_OverLoadXLTimer[UNITCOUNT];			// 过负荷计时器(用于中压馈线保护)

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

BOOL WINAPI ProtectionCalculater(TagProtectionCalculaterStruct *pProtectionCalculater)
{
	switch(pProtectionCalculater->ProtectionType)
	{
		case 1:														//变压器差动保护
			ProtectionCalculater1(pProtectionCalculater);
			break;
		case 2:														//变压器后备保护
			ProtectionCalculater2(pProtectionCalculater);
			break;
		case 3:														//中压进线保护
			ProtectionCalculater3(pProtectionCalculater);
			break;
		case 4:														//中压馈线保护
		case 6:														//所用变保护
			ProtectionCalculater4(pProtectionCalculater);
			break;
		case 5:														//电容器保护
			//ProtectionCalculater5(pProtectionCalculater);
			break;
	}
	
	return TRUE;
}

BOOL WINAPI ProtectionCalculater1(TagProtectionCalculaterStruct *pProtectionCalculater)		//变压器差动保护,采样通道顺序:IA,IB,IC,Ia,Ib,Ic,I0
{
//读取保护压板
	BOOL DifferentialQuickBreakOnOff=BOOL(pProtectionCalculater->ProtectionSwitch.Value[0]);	//差动速断压板
	BOOL DifferentialRatioOnOff=BOOL(pProtectionCalculater->ProtectionSwitch.Value[1]);			//比例差动压板
	BOOL ZeroOverCurrentOnOff=BOOL(pProtectionCalculater->ProtectionSwitch.Value[2]);			//零序过流保护压板
//读取定值
	float CTHRatio=pProtectionCalculater->ProtectionParameter.Value[0];							//高压侧CT变比
	float CTLRatio=pProtectionCalculater->ProtectionParameter.Value[1];							//低压侧CT变比
	float CTZRatio=pProtectionCalculater->ProtectionParameter.Value[2];							//零序CT变比
	float ITBL=pProtectionCalculater->ProtectionParameter.Value[3];								//突变量启动定值
	float ISDA=pProtectionCalculater->ProtectionParameter.Value[4];								//A相差电流速断定值
	float ISDB=pProtectionCalculater->ProtectionParameter.Value[5];								//B相差电流速断定值
	float ISDC=pProtectionCalculater->ProtectionParameter.Value[6];								//C相差电流速断定值
	float IDZA=pProtectionCalculater->ProtectionParameter.Value[7];								//A相差动电流定值
	float IDZB=pProtectionCalculater->ProtectionParameter.Value[8];								//B相差动电流定值
	float IDZC=pProtectionCalculater->ProtectionParameter.Value[9];								//C相差动电流定值
	float IZDA=pProtectionCalculater->ProtectionParameter.Value[10];							//A相差动制动电流定值
	float IZDB=pProtectionCalculater->ProtectionParameter.Value[11];							//B相差动制动电流定值
	float IZDC=pProtectionCalculater->ProtectionParameter.Value[12];							//C相差动制动电流定值
	float KZD=pProtectionCalculater->ProtectionParameter.Value[13];								//比例制动系数
	float IGJA=pProtectionCalculater->ProtectionParameter.Value[14];							//A相差流告警定值
	float IGJB=pProtectionCalculater->ProtectionParameter.Value[15];							//B相差流告警定值
	float IGJC=pProtectionCalculater->ProtectionParameter.Value[16];							//C相差流告警定值
	float KYL=pProtectionCalculater->ProtectionParameter.Value[17];								//二次谐波制动系数
	float KPH=pProtectionCalculater->ProtectionParameter.Value[18];								//平衡系数
	float ILX=pProtectionCalculater->ProtectionParameter.Value[19];								//零序过流电流定值
	float TLX=pProtectionCalculater->ProtectionParameter.Value[20]*1000;						//零序过流时间定值

	BOOL ActiveDifferentialQuickBreakA=FALSE;													//A相差流速断动作标志
	BOOL ActiveDifferentialQuickBreakB=FALSE;													//B相差流速断动作标志
	BOOL ActiveDifferentialQuickBreakC=FALSE;													//C相差流速断动作标志
	BOOL ActiveDifferentialRatioA=FALSE;														//A相比率差动动作标志
	BOOL ActiveDifferentialRatioB=FALSE;														//B相比率差动动作标志
	BOOL ActiveDifferentialRatioC=FALSE;														//C相比率差动动作标志
	BOOL ActiveDifferentialWarningA=FALSE;														//A相差流告警动作标志
	BOOL ActiveDifferentialWarningB=FALSE;														//B相差流告警动作标志
	BOOL ActiveDifferentialWarningC=FALSE;														//C相差流告警动作标志
	BOOL ActiveZeroOverCurrent=FALSE;															//零序过流保护动作标志

	int ZeroOverCurrentJS=0;																	//零序过流计数器

	float sampletbl[3]={0.0,0.0,0.0};															//定义突变量计算数组
	int dltb[3]={0,0,0};																		//电流突变计数数组

	WORD ActiveType=0;
	WORD ActionPosition=0;

	if((DifferentialQuickBreakOnOff==TRUE)||(DifferentialRatioOnOff==TRUE))						//主变为YNd11接线变压器
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float IAValue,IBValue,ICValue,IaValue,IbValue,IcValue,IAValuejd,IBValuejd,ICValuejd,IaValuejd,IbValuejd,IcValuejd;
			float I2AValue,I2BValue,I2CValue,I2aValue,I2bValue,I2cValue;
			float IAaValue,IAaValuejd,IBbValue,IBbValuejd,ICcValue,ICcValuejd;					//低压侧电流转换到高压侧的各相电流
			float DIAValue,BIAValue,DIBValue,BIBValue,DICValue,BICValue;						//差动电流D,制动电流B
							
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,IAValue,IAValuejd);	//计算IA
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,IBValue,IBValuejd);	//计算IB
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,ICValue,ICValuejd);	//计算IC
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue,IaValuejd);	//计算Ia
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue,IbValuejd);	//计算Ib
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue,IcValuejd);	//计算Ic
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,2,I2AValue);			//计算I2A
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,2,I2BValue);			//计算I2B
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,2,I2CValue);			//计算I2C
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,2,I2aValue);			//计算I2a
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,2,I2bValue);			//计算I2b
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,2,I2cValue);			//计算I2c
			IAValue=IAValue/CTHRatio;
			IBValue=IBValue/CTHRatio;
			ICValue=ICValue/CTHRatio;
			I2AValue=I2AValue/CTHRatio;
			I2BValue=I2BValue/CTHRatio;
			I2CValue=I2CValue/CTHRatio;
			IaValue=IaValue/CTLRatio;
			IbValue=IbValue/CTLRatio;
			IcValue=IcValue/CTLRatio;
			I2aValue=I2aValue/CTLRatio;
			I2bValue=I2bValue/CTLRatio;
			I2cValue=I2cValue/CTLRatio;

			for(int j=0;j<3;j++)					//检测A相,B相,C相电流突变量
			{
				if(dltb[j]<4)
				{
					sampletbl[j]=float((fabs(pProtectionCalculater->ProtectionSample.ChannelValue[j][sampno])-fabs(pProtectionCalculater->ProtectionSample.ChannelValue[j][sampno-number_of_samples]))); //计算瞬时值差值 
					if((sampletbl[j]/CTHRatio)>ITBL) ++dltb[j];
					else dltb[j]=0;
				}
			}
							
			if(dltb[0]>=4)																		//A相电流突变量启动差动保护
			{
				IAaValue=IaValue/KPH;															//将a相电流变换到高压侧
				IAaValuejd=float(IaValuejd-PI); 												//将a相电流相位取反
				DIAValue=float(sqrt(pow((IAValue*cos(IAValuejd)+IAaValue*cos(IAaValuejd)),2)+pow((IAValue*sin(IAValuejd)+IAaValue*sin(IAaValuejd)),2)));	//计算A相差动电流,两电流相位相反,相加为差动  
				BIAValue=float(sqrt(pow((IAValue*cos(IAValuejd)-IAaValue*cos(IAaValuejd)),2)+pow((IAValue*sin(IAValuejd)-IAaValue*sin(IAaValuejd)),2))/2);	//计算A相制动电流,两电流相减,幅值再除2
				if((DifferentialQuickBreakOnOff==TRUE)&&(ActiveType==0))						//差动速断投入
				{
					if(DIAValue>ISDA)
					{
						ActiveDifferentialQuickBreakA=TRUE;
						ActiveType=1;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=ActionPosition;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//高压侧断路器动作跳闸
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;				//低压侧断路器动作跳闸
					}
				}
				if((DifferentialRatioOnOff==TRUE)&&(ActiveType==0))								//比例差动投入
				{
					if(((I2AValue/IAValue)<KYL)&&((I2BValue/IBValue)<KYL)&&((I2CValue/ICValue)<KYL))
					{
						if((BIAValue<=IZDA)&&(DIAValue>IDZA)) ActiveDifferentialRatioA=TRUE;	//当Izd<Izds并Icd>Icds 差动动作
						else if((BIAValue>IZDA)&&(DIAValue>(KZD*(BIAValue-IZDA)+IDZA))) ActiveDifferentialRatioA=TRUE;	//当Izd>=Izds并Icd>(k*(Izd-Izds)+Icds) 差动动作
						else ActiveDifferentialRatioA=FALSE;
						if(ActiveDifferentialRatioA==TRUE) 
						{
							ActiveType=2;
							ActionPosition=sampno;
							pProtectionCalculater->ProtectionActionType=ActiveType;
							pProtectionCalculater->ProtectionActionTime=ActionPosition;
							pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;			//高压侧断路器动作跳闸
							pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;			//低压侧断路器动作跳闸
						}
					}
				}
				if((ActiveType==0)&&(sampno>120))
				{
					if(DIAValue>IGJA)															//差流告警
					{
						ActiveDifferentialWarningA=TRUE;
						ActiveType=22;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=100;						//?
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;			//保护动作告警
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=TRUE;
					}
				}
			}

			if(dltb[1]>=4)																		//B相电流突变量启动差动保护
			{
				IBbValue=IbValue/KPH;															//将b相电流变换到高压侧
				IBbValuejd=float(IbValuejd-PI); 												//将b相电流相位取反
				DIBValue=float(sqrt(pow((IBValue*cos(IBValuejd)+IBbValue*cos(IBbValuejd)),2)+pow((IBValue*sin(IBValuejd)+IBbValue*sin(IBbValuejd)),2)));	//计算B相差动电流,两电流相位相反,相加为差动  
				BIBValue=float(sqrt(pow((IBValue*cos(IBValuejd)-IBbValue*cos(IBbValuejd)),2)+pow((IBValue*sin(IBValuejd)-IBbValue*sin(IBbValuejd)),2))/2);	//计算B相制动电流,两电流相减,幅值再除2
				if((DifferentialQuickBreakOnOff==TRUE)&&(ActiveType==0))						//差动速断投入
				{
					if(DIBValue>ISDB)
					{
						ActiveDifferentialQuickBreakB=TRUE;
						ActiveType=1;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=ActionPosition;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//高压侧断路器动作跳闸
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;				//低压侧断路器动作跳闸
					}
				}
				if((DifferentialRatioOnOff==TRUE)&&(ActiveType==0))								//比例差动投入
				{
					if(((I2AValue/IAValue)<KYL)&&((I2BValue/IBValue)<KYL)&&((I2CValue/ICValue)<KYL))
					{
						if((BIBValue<=IZDB)&&(DIBValue>IDZB)) ActiveDifferentialRatioB=TRUE;	//当Izd<Izds并Icd>Icds 差动动作
						else if((BIBValue>IZDB)&&(DIBValue>(KZD*(BIBValue-IZDB)+IDZB))) ActiveDifferentialRatioB=TRUE;	//当Izd>=Izd2s并Icd>k2*(Izd-Izd2s)+k1*(Izd2s-Izd1s)+Icds 差动动作
						else ActiveDifferentialRatioB=FALSE;
						if(ActiveDifferentialRatioB==TRUE) 
						{
							ActiveType=2;
							ActionPosition=sampno;
							pProtectionCalculater->ProtectionActionType=ActiveType;
							pProtectionCalculater->ProtectionActionTime=ActionPosition;
							pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;			//高压侧断路器动作跳闸
							pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;			//低压侧断路器动作跳闸
						}
					}
				}
				if((ActiveType==0)&&(sampno>120))
				{
					if(DIBValue>IGJB)															//差流告警
					{
						ActiveDifferentialWarningB=TRUE;
						ActiveType=23;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=100;						//?
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;			//保护动作告警
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[23].Value=TRUE;
					}
				}
			}

			if(dltb[2]>=4)																		//C相电流突变量启动差动保护
			{
				ICcValue=IcValue/KPH;
				ICcValuejd=float(IcValuejd-PI);
				DICValue=float(sqrt(pow((ICValue*cos(ICValuejd)+ICcValue*cos(ICcValuejd)),2)+pow((ICValue*sin(ICValuejd)+ICcValue*sin(ICcValuejd)),2)));
				BICValue=float(sqrt(pow((ICValue*cos(ICValuejd)-ICcValue*cos(ICcValuejd)),2)+pow((ICValue*sin(ICValuejd)-ICcValue*sin(ICcValuejd)),2))/2);	
				if((DifferentialQuickBreakOnOff==TRUE)&&(ActiveType==0))						//差动速断投入
				{
					if(DICValue>ISDC)
					{
						ActiveDifferentialQuickBreakC=TRUE;
						ActiveType=1;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=ActionPosition;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//高压侧断路器动作跳闸
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;				//低压侧断路器动作跳闸
					}
				}	
				if((DifferentialRatioOnOff==TRUE)&&(ActiveType==0))								//比例差动投入
				{
					if(((I2AValue/IAValue)<KYL)&&((I2BValue/IBValue)<KYL)&&((I2CValue/ICValue)<KYL))
					{
						if((BICValue<=IZDC)&&(DICValue>IDZC)) ActiveDifferentialRatioC=TRUE;	//当Izd<Izds并Icd>Icds 差动动作
						else if((BICValue>IZDC)&&(DICValue>(KZD*(BICValue-IZDC)+IDZC))) ActiveDifferentialRatioC=TRUE;	//当Izd>=Izd2s并Icd>k2*(Izd-Izd2s)+k1*(Izd2s-Izd1s)+Icds 差动动作
						else ActiveDifferentialRatioC=FALSE;
						if(ActiveDifferentialRatioC==TRUE) 
						{
							ActiveType=2;
							ActionPosition=sampno;
							pProtectionCalculater->ProtectionActionType=ActiveType;
							pProtectionCalculater->ProtectionActionTime=ActionPosition;
							pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;			//高压侧断路器动作跳闸
							pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;			//低压侧断路器动作跳闸
						}
					}
				}
				if((ActiveType==0)&&(sampno>120))
				{
					if(DICValue>IGJC)															//差流告警
					{
						ActiveDifferentialWarningC=TRUE;
						ActiveType=24;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=100;						//?
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;			//保护动作告警
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[24].Value=TRUE;
					}
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

			float I0Value;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,I0Value);
			I0Value/=CTZRatio;

			if(I0Value>ILX)
			{
				if(ZeroOverCurrentJS==int(TLX*0.1))
				{	
					ActiveZeroOverCurrent=TRUE;
					ActiveType=4;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionType=ActiveType;
					pProtectionCalculater->ProtectionActionTime=(WORD)TLX;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;					//高压侧断路器动作跳闸
					pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;					//低压侧断路器动作跳闸
				}
				ZeroOverCurrentJS+=1;
			}
		}
	}

/*	if((ZeroOverCurrentOnOff==TRUE)&&(ActiveType==0))		//零序保护软压板投入
	{
		float I0Value;
		int sampno=WAVE_SAMPLE_COUNT;					//取最后一个周波
				
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,I0Value);//计算I0
		I0Value/=CTZRatio;	
		if(I0Value>ILX) ActiveZeroOverCurrent=TRUE;
		else ActiveZeroOverCurrent=FALSE;
		if(ActiveZeroOverCurrent==FALSE)
		{
			ClearTimer(&g_ZeroOverCurrentTimer[pProtectionCalculater->ProtectionID]);
		}
		else
		{
			if(g_ZeroOverCurrentTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_ZeroOverCurrentTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				if((::GetTickCount()-g_ZeroOverCurrentTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(TLX))
				{
					ClearTimer(&g_ZeroOverCurrentTimer[pProtectionCalculater->ProtectionID]);
					ActiveType=4;
					ActionPosition=160+(WORD)::GetTickCount()%120;
					pProtectionCalculater->ProtectionActionType=ActiveType;
					pProtectionCalculater->ProtectionActionTime=(WORD)::GetTickCount()%1000;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;					//高压侧断路器动作跳闸
					pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;					//低压侧断路器动作跳闸
				}
			}
		}
	}*/

	if(ActiveType!=0)			//计算保护动作时的所有参数
	{
		float ActionValueIA,ActionValueIB,ActionValueIC,ActionValueIa,ActionValueIb,ActionValueIc,ActionValueI0;
		float ActionValueIAjd,ActionValueIBjd,ActionValueICjd,ActionValueIajd,ActionValueIbjd,ActionValueIcjd;
		float ActionValueI2A,ActionValueI2B,ActionValueI2C;
		float ActionValueIAa,ActionValueIAajd,ActionValueDIA,ActionValueBIA;
		float ActionValueIBb,ActionValueIBbjd,ActionValueDIB,ActionValueBIB;
		float ActionValueICc,ActionValueICcjd,ActionValueDIC,ActionValueBIC;

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,ActionValueIA,ActionValueIAjd);	//计算IA
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,ActionValueIB,ActionValueIBjd);	//计算IB
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],ActionPosition,ActionValueIC,ActionValueICjd);	//计算IC
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],ActionPosition,ActionValueIa,ActionValueIajd);	//计算Ia
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],ActionPosition,ActionValueIb,ActionValueIbjd);	//计算Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],ActionPosition,ActionValueIc,ActionValueIcjd);	//计算Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,2,ActionValueI2A);				//计算IA2
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,2,ActionValueI2B);				//计算IB2
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],ActionPosition,2,ActionValueI2C);				//计算IC2
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],ActionPosition,ActionValueI0);					//计算I0
		ActionValueIA/=CTHRatio;
		ActionValueIB/=CTHRatio;
		ActionValueIC/=CTHRatio;
		ActionValueIa/=CTLRatio;
		ActionValueIb/=CTLRatio;
		ActionValueIc/=CTLRatio;
		ActionValueI2A/=CTHRatio;
		ActionValueI2B/=CTHRatio;
		ActionValueI2C/=CTHRatio;
		ActionValueI0/=CTZRatio;
		ActionValueIAa=ActionValueIa/KPH;
		ActionValueIAajd=float(ActionValueIajd-PI);
		ActionValueDIA=float(sqrt(pow((ActionValueIA*cos(ActionValueIAjd)+ActionValueIAa*cos(ActionValueIAajd)),2)+pow((ActionValueIA*sin(ActionValueIAjd)+ActionValueIAa*sin(ActionValueIAajd)),2)));
		ActionValueBIA=float(sqrt(pow((ActionValueIA*cos(ActionValueIAjd)-ActionValueIAa*cos(ActionValueIAajd)),2)+pow((ActionValueIA*sin(ActionValueIAjd)-ActionValueIAa*sin(ActionValueIAajd)),2))/2);
		ActionValueIBb=ActionValueIb/KPH;
		ActionValueIBbjd=float(ActionValueIbjd-PI);
		ActionValueDIB=float(sqrt(pow((ActionValueIB*cos(ActionValueIBjd)+ActionValueIBb*cos(ActionValueIBbjd)),2)+pow((ActionValueIB*sin(ActionValueIBjd)+ActionValueIBb*sin(ActionValueIBbjd)),2)));
		ActionValueBIB=float(sqrt(pow((ActionValueIB*cos(ActionValueIBjd)-ActionValueIBb*cos(ActionValueIBbjd)),2)+pow((ActionValueIB*sin(ActionValueIBjd)-ActionValueIBb*sin(ActionValueIBbjd)),2))/2);
		ActionValueICc=ActionValueIc/KPH;
		ActionValueICcjd=float(ActionValueIcjd-PI);
		ActionValueDIC=float(sqrt(pow((ActionValueIC*cos(ActionValueICjd)+ActionValueICc*cos(ActionValueICcjd)),2)+pow((ActionValueIC*sin(ActionValueICjd)+ActionValueICc*sin(ActionValueICcjd)),2)));
		ActionValueBIC=float(sqrt(pow((ActionValueIC*cos(ActionValueICjd)-ActionValueICc*cos(ActionValueICcjd)),2)+pow((ActionValueIC*sin(ActionValueICjd)-ActionValueICc*sin(ActionValueICcjd)),2))/2);	
						
		pProtectionCalculater->ProtectionActionValue.Value[0]=ActionValueIA;
		pProtectionCalculater->ProtectionActionValue.Value[1]=ActionValueIB;
		pProtectionCalculater->ProtectionActionValue.Value[2]=ActionValueIC;
		pProtectionCalculater->ProtectionActionValue.Value[3]=ActionValueIa;
		pProtectionCalculater->ProtectionActionValue.Value[4]=ActionValueIb;
		pProtectionCalculater->ProtectionActionValue.Value[5]=ActionValueIc;
		pProtectionCalculater->ProtectionActionValue.Value[6]=ActionValueDIA;
		pProtectionCalculater->ProtectionActionValue.Value[7]=ActionValueBIA;
		pProtectionCalculater->ProtectionActionValue.Value[8]=ActionValueDIB;
		pProtectionCalculater->ProtectionActionValue.Value[9]=ActionValueBIB;
		pProtectionCalculater->ProtectionActionValue.Value[10]=ActionValueDIC;
		pProtectionCalculater->ProtectionActionValue.Value[11]=ActionValueBIC;
		pProtectionCalculater->ProtectionActionValue.Value[12]=max(max(ActionValueI2A/ActionValueIA,ActionValueI2B/ActionValueIB),ActionValueI2C/ActionValueIC);
		pProtectionCalculater->ProtectionActionValue.Value[13]=ActionValueI0;

		if((pProtectionCalculater->ProtectionActionBreakerTrip[0]==TRUE)&&(pProtectionCalculater->ProtectionActionBreakerTrip[1]==TRUE))
        {
			for(int sampno=ActionPosition;sampno<WAVE_SAMPLE_COUNT;sampno++)
			{
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[2][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[3][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[4][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[5][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[6][sampno]=0;
			}
        }

		return TRUE;
	}
	else
	{
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)			//重瓦斯遥信
		{
			if((g_Unit[pProtectionCalculater->ProtectionID+1].VarGroup[1].Var[0].Value!=0)||(g_Unit[pProtectionCalculater->ProtectionID+2].VarGroup[1].Var[0].Value!=0))
			{
				ActiveType=3;
				pProtectionCalculater->ProtectionActionType=ActiveType;
				pProtectionCalculater->ProtectionActionTime=0;
				pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;					//高压侧断路器动作跳闸
				pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;					//低压侧断路器动作跳闸
			}
		}

		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[1].Value>0)			//轻瓦斯遥信
		{
			ActiveType=18;
			pProtectionCalculater->ProtectionActionType=ActiveType;
			pProtectionCalculater->ProtectionActionTime=100;								//?
			pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//保护动作告警
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=TRUE;
		}

		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[2].Value>0)			//温度遥信
		{
			ActiveType=19;
			pProtectionCalculater->ProtectionActionType=ActiveType;
			pProtectionCalculater->ProtectionActionTime=100;								//?
			pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//保护动作告警
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=TRUE;
		}

		return TRUE;
	}

	if(ActiveZeroOverCurrent==FALSE) ClearTimer(&g_ZeroOverCurrentTimer[pProtectionCalculater->ProtectionID]);

	return TRUE;	
}

BOOL WINAPI ProtectionCalculater2(TagProtectionCalculaterStruct *pProtectionCalculater)			//变压器后备保护,采样通道顺序:UA,UB,UC,Ua,Ub,Uc,IA,IB,IC
{
//读取保护压板
	BOOL OverCurrentSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];		//过流1段压板
	BOOL OverCurrentSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];		//过流2段压板
	BOOL LossVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[2];				//失压压板
	BOOL OverLoadOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[3];					//过负荷压板
	BOOL OverLoadSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[4];			//过负荷1段投入
	BOOL OverLoadSection1CKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[5];		//0:过负荷1段告警,1:过负荷1段跳闸
	BOOL OverLoadSection1SXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[6];		//0:过负荷1段定时限,1:过负荷1段标准反时限
	BOOL OverLoadSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[7];			//过负荷2段投入
	BOOL OverLoadSection2CKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[8];		//0:过负荷2段告警,1:过负荷2段跳闸 
	BOOL OverLoadSection2SXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[9];		//0:过负荷2段定时限,1:过负荷2段标准反时限
	BOOL LowVoltageSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[10];		//低压闭锁过流1段压板
	BOOL LowVoltageSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[11];		//低压闭锁过流2段压板
	BOOL PTUnusualSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[12];		//0:PT异常退出过流1段,1:PT异常退出1段低压闭锁
	BOOL PTUnusualSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[13];		//0:PT异常退出过流2段,1:PT异常退出2段低压闭锁
	BOOL PTUnusualOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[30];				//PT异常检测
//读取定值数值
	float PTHValue=pProtectionCalculater->ProtectionParameter.Value[0]*(float)0.001;			//高压侧PT变比
	float PTLValue=pProtectionCalculater->ProtectionParameter.Value[1]*(float)0.001;			//低压侧PT变比
	float CTHValue=pProtectionCalculater->ProtectionParameter.Value[2];							//高压侧CT变比		
	float OverCurrentSection1AValue=pProtectionCalculater->ProtectionParameter.Value[3];		//过流1段A相定值
	float OverCurrentSection1BValue=pProtectionCalculater->ProtectionParameter.Value[4];		//过流1段B相定值
	float OverCurrentSection1CValue=pProtectionCalculater->ProtectionParameter.Value[5];		//过流1段C相定值
	float OverCurrentSection1Time=pProtectionCalculater->ProtectionParameter.Value[6]*1000;		//过流1段时间定值
	float OverCurrentSection2AValue=pProtectionCalculater->ProtectionParameter.Value[7];		//过流2段A相定值
	float OverCurrentSection2BValue=pProtectionCalculater->ProtectionParameter.Value[8];		//过流2段B相定值
	float OverCurrentSection2CValue=pProtectionCalculater->ProtectionParameter.Value[9];		//过流2段C相定值
	float OverCurrentSection2Time=pProtectionCalculater->ProtectionParameter.Value[10]*1000;	//过流2段时间定值
	float LowVoltageSectionValue=pProtectionCalculater->ProtectionParameter.Value[11];			//低压闭锁过流电压定值	
	float LossVoltageValue=pProtectionCalculater->ProtectionParameter.Value[12];				//失压保护定值
	float LossVoltageTime=pProtectionCalculater->ProtectionParameter.Value[13]*1000;			//失压时间定值
	float OverLoadSection1AValue=pProtectionCalculater->ProtectionParameter.Value[14];			//A相过负荷1段保护定值
	float OverLoadSection1BValue=pProtectionCalculater->ProtectionParameter.Value[15];			//B相过负荷1段保护定值
	float OverLoadSection1CValue=pProtectionCalculater->ProtectionParameter.Value[16];			//C相过负荷1段保护定值
	float OverLoadSection1Time=pProtectionCalculater->ProtectionParameter.Value[17]*1000;		//过负荷1段时间定值
	float OverLoadSection2AValue=pProtectionCalculater->ProtectionParameter.Value[18];			//A相过负荷2段保护定值
	float OverLoadSection2BValue=pProtectionCalculater->ProtectionParameter.Value[19];			//B相过负荷2段保护定值
	float OverLoadSection2CValue=pProtectionCalculater->ProtectionParameter.Value[20];			//C相过负荷2段保护定值
	float OverLoadSection2Time=pProtectionCalculater->ProtectionParameter.Value[21]*1000;		//过负荷2段时间定值

	BOOL ActiveOverCurrentSection1=FALSE;		//过流1段动作标志		1
	BOOL ActiveOverCurrentSection2=FALSE;		//过流2段动作标志		2
	BOOL ActiveLossVoltage=FALSE;				//失压动作标志			3
	BOOL ActiveOverLoadSection1=FALSE;			//过负荷1段动作标志		4
	BOOL ActiveOverLoadSection2=FALSE;			//过负荷2段动作标志		5
	BOOL ActivePTH=FALSE;						//高压侧PT异常			18
	BOOL ActivePTL=FALSE;						//低压侧PT异常			19
				
	int OverCurrentSection1JS=0;
	int OverCurrentSection2JS=0;

	WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;
				
	if(PTUnusualOnOff==TRUE)					//PT异常投入
	{
		for(int sampno=32;sampno<60;sampno+=1)
		{
			float UAValue,UBValue,UCValue,UaValue,UbValue,UcValue;
			float UAValuejd,UBValuejd,UCValuejd,UaValuejd,UbValuejd,UcValuejd;
			float UABValue,UBCValue,UCAValue,UabValue,UbcValue,UcaValue;
							
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UAValue,UAValuejd);		//计算UA
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UBValue,UBValuejd);		//计算UB
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UCValue,UCValuejd);		//计算UC
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,UaValue,UaValuejd);		//计算Ua
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,UbValue,UbValuejd);		//计算Ub
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,UcValue,UcValuejd);		//计算Uc
			UAValue/=PTHValue;
			UBValue/=PTHValue;
			UCValue/=PTHValue;
			UABValue=(float)sqrt(pow(UAValue*cos(UAValuejd)-UBValue*cos(UBValuejd),2)+pow(UAValue*sin(UAValuejd)-UBValue*sin(UBValuejd),2));
			UBCValue=(float)sqrt(pow(UBValue*cos(UBValuejd)-UCValue*cos(UCValuejd),2)+pow(UBValue*sin(UBValuejd)-UCValue*sin(UCValuejd),2));
			UCAValue=(float)sqrt(pow(UCValue*cos(UCValuejd)-UAValue*cos(UAValuejd),2)+pow(UCValue*sin(UCValuejd)-UAValue*sin(UAValuejd),2));
			UaValue/=PTLValue;
			UbValue/=PTLValue;
			UcValue/=PTLValue;
			UabValue=(float)sqrt(pow(UaValue*cos(UaValuejd)-UbValue*cos(UbValuejd),2)+pow(UaValue*sin(UaValuejd)-UbValue*sin(UbValuejd),2));
			UbcValue=(float)sqrt(pow(UbValue*cos(UbValuejd)-UcValue*cos(UcValuejd),2)+pow(UbValue*sin(UbValuejd)-UcValue*sin(UcValuejd),2));
			UcaValue=(float)sqrt(pow(UcValue*cos(UcValuejd)-UaValue*cos(UaValuejd),2)+pow(UcValue*sin(UcValuejd)-UaValue*sin(UaValuejd),2));

			if((g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)&&(g_Unit[pProtectionCalculater->ProtectionID+1].VarGroup[1].Var[0].Value>0))	//高低压断路器合闸
			{
				if((min(min(UABValue,UBCValue),UCAValue)<70)&&(UabValue>70)&&(UbcValue>70)&&(UcaValue>70))
				{
                	ActivePTH=TRUE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=TRUE;		//向ID设备的遥信18号点发"高压侧PT异常"
				}
				else
				{	
					ActivePTH=FALSE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=FALSE;	//向ID设备的遥信18号点发"高压侧PT正常"
				}

				if((min(min(UabValue,UbcValue),UcaValue)<70)&&(UABValue>70)&&(UBCValue>70)&&(UCAValue>70))
				{
					ActivePTL=TRUE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=TRUE;		//向ID设备的遥信19号点发"低压侧PT异常"
				}
				else
				{
					ActivePTL=FALSE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=FALSE;	//向ID设备的遥信19号点发"低压侧PT正常"
				}
			}
		}	
	}
	else
	{
		ActivePTH=FALSE;
		g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=FALSE;
		ActivePTL=FALSE;
		g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=FALSE;
	}

	if((OverCurrentSection1OnOff==TRUE)||(OverCurrentSection2OnOff==TRUE))		//过流1段或过流2段压板投入
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;
			
			float UaValue,UbValue,UcValue,IAValue,IBValue,ICValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,UaValue);	//计算Ua
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,UbValue);	//计算Ub
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,UcValue);	//计算Uc
			UaValue/=PTLValue;
			UbValue/=PTLValue;
			UcValue/=PTLValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,IAValue);	//计算IA
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[7],sampno,IBValue);	//计算IB
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[8],sampno,ICValue);	//计算IC
			IAValue/=CTHValue;
			IBValue/=CTHValue;
			ICValue/=CTHValue;

			if((OverCurrentSection1OnOff==TRUE)&&(ActiveType==0))										//过流1段压板投入											
			{
				if(LowVoltageSection1OnOff==TRUE)														//低压闭锁过流1段投入
				{
					if((ActivePTH||ActivePTL)==TRUE)													//PT异常
					{
						if(PTUnusualSection1OnOff==TRUE)												//退出过流1段低压闭锁
						{
							if((IAValue>OverCurrentSection1AValue)||(IBValue>OverCurrentSection1BValue)||(ICValue>OverCurrentSection1CValue)) ActiveOverCurrentSection1=TRUE;
							else ActiveOverCurrentSection1=FALSE;
						}
						else ActiveOverCurrentSection1=FALSE;											//退出过流1段
					}
					else																				//PT正常
					{
						if((UaValue<LowVoltageSectionValue)||(UbValue<LowVoltageSectionValue)||(UcValue<LowVoltageSectionValue))
						{
							if((IAValue>OverCurrentSection1AValue)||(IBValue>OverCurrentSection1BValue)||(ICValue>OverCurrentSection1CValue)) ActiveOverCurrentSection1=TRUE;
							else ActiveOverCurrentSection1=FALSE;
						}
						else ActiveOverCurrentSection1=FALSE;
					}
				}
				else																					//低压闭锁过流1段未投
				{
					if((IAValue>OverCurrentSection1AValue)||(IBValue>OverCurrentSection1BValue)||(ICValue>OverCurrentSection1CValue)) ActiveOverCurrentSection1=TRUE;
					else ActiveOverCurrentSection1=FALSE;
				}		
				if(ActiveOverCurrentSection1==TRUE)
				{
					if(OverCurrentSection1JS==int(OverCurrentSection1Time*0.1))
					{
						ActiveType=1;
						ActiveTime=(WORD)OverCurrentSection1Time;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//高压侧断路器动作跳闸
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//低压侧断路器动作跳闸
					}
					OverCurrentSection1JS+=1;
				}
			}

			if((OverCurrentSection2OnOff==TRUE)&&(ActiveType==0))										//过流2段压板投入
			{
				if(LowVoltageSection2OnOff==TRUE)														//低压闭锁过流2段投入
				{
					if((ActivePTH||ActivePTL)==TRUE)													//PT异常
					{
						if(PTUnusualSection2OnOff==TRUE)												//退出过流2段低压闭锁
						{
							if((IAValue>OverCurrentSection2AValue)||(IBValue>OverCurrentSection2BValue)||(ICValue>OverCurrentSection2CValue)) ActiveOverCurrentSection2=TRUE;
							else ActiveOverCurrentSection2=FALSE;
						}
						else ActiveOverCurrentSection2=FALSE;											//退出过流2段
					}
					else																				//PT正常
					{
						if((UaValue<LowVoltageSectionValue)||(UbValue<LowVoltageSectionValue)||(UcValue<LowVoltageSectionValue))
						{
							if((IAValue>OverCurrentSection2AValue)||(IBValue>OverCurrentSection2BValue)||(ICValue>OverCurrentSection2CValue)) ActiveOverCurrentSection2=TRUE;
							else ActiveOverCurrentSection2=FALSE;
						}
						else ActiveOverCurrentSection2=FALSE;
					}
				}
				else																					//低压闭锁过流2段未投
				{
					if((IAValue>OverCurrentSection2AValue)||(IBValue>OverCurrentSection2BValue)||(ICValue>OverCurrentSection2CValue)) ActiveOverCurrentSection2=TRUE;
					else ActiveOverCurrentSection2=FALSE;
				}		
				if(ActiveOverCurrentSection2==TRUE)
				{
					if(OverCurrentSection2JS==int(OverCurrentSection2Time*0.1))
					{
						ActiveType=2;
						ActiveTime=(WORD)OverCurrentSection2Time;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//高压侧断路器动作跳闸
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//低压侧断路器动作跳闸
					}
					OverCurrentSection2JS+=1;
				}
			}
		}

		if(ActiveType!=0)
		{
			pProtectionCalculater->ProtectionActionType=ActiveType;
			pProtectionCalculater->ProtectionActionTime=ActiveTime;

			float ActionValueUA,ActionValueUB,ActionValueUC,ActionValueIA,ActionValueIB,ActionValueIC;
			float ActionValueUa,ActionValueUb,ActionValueUc;
			float ActionValueUAB,ActionValueUBC,ActionValueUCA;
			float ActionValueUAjd,ActionValueUBjd,ActionValueUCjd,ActionValueIAjd,ActionValueIBjd,ActionValueICjd;
			
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,ActionValueUA,ActionValueUAjd);
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,ActionValueUB,ActionValueUBjd);
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],ActionPosition,ActionValueUC,ActionValueUCjd);
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],ActionPosition,ActionValueUa);
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],ActionPosition,ActionValueUb);
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],ActionPosition,ActionValueUc);
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],ActionPosition,ActionValueIA,ActionValueIAjd);
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[7],ActionPosition,ActionValueIB,ActionValueIBjd);
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[8],ActionPosition,ActionValueIC,ActionValueICjd);
			ActionValueUA/=PTHValue;
			ActionValueUB/=PTHValue;
			ActionValueUC/=PTHValue;
			ActionValueIA/=CTHValue;
			ActionValueIB/=CTHValue;
			ActionValueIC/=CTHValue;
			ActionValueUa/=PTLValue;
			ActionValueUb/=PTLValue;
			ActionValueUc/=PTLValue;
			ActionValueUAB=(float)sqrt(pow(ActionValueUA*cos(ActionValueUAjd)-ActionValueUB*cos(ActionValueUBjd),2)+
				pow(ActionValueUA*sin(ActionValueUAjd)-ActionValueUB*sin(ActionValueUBjd),2));
			ActionValueUBC=(float)sqrt(pow(ActionValueUB*cos(ActionValueUBjd)-ActionValueUC*cos(ActionValueUCjd),2)+
				pow(ActionValueUB*sin(ActionValueUBjd)-ActionValueUC*sin(ActionValueUCjd),2));
			ActionValueUCA=(float)sqrt(pow(ActionValueUC*cos(ActionValueUCjd)-ActionValueUA*cos(ActionValueUAjd),2)+
				pow(ActionValueUC*sin(ActionValueUCjd)-ActionValueUA*sin(ActionValueUAjd),2));

			pProtectionCalculater->ProtectionActionValue.Value[0]=ActionValueUA;
			pProtectionCalculater->ProtectionActionValue.Value[1]=ActionValueUB;
			pProtectionCalculater->ProtectionActionValue.Value[2]=ActionValueUC;
			pProtectionCalculater->ProtectionActionValue.Value[3]=ActionValueIA;
			pProtectionCalculater->ProtectionActionValue.Value[4]=ActionValueIB;
			pProtectionCalculater->ProtectionActionValue.Value[5]=ActionValueIC;
			pProtectionCalculater->ProtectionActionValue.Value[6]=ActionValueUa;
			pProtectionCalculater->ProtectionActionValue.Value[7]=ActionValueUb;
			pProtectionCalculater->ProtectionActionValue.Value[8]=ActionValueUc;
			pProtectionCalculater->ProtectionActionValue.Value[9]=ActionValueUAB;
			pProtectionCalculater->ProtectionActionValue.Value[10]=ActionValueUBC;
			pProtectionCalculater->ProtectionActionValue.Value[11]=ActionValueUCA;
			if(pProtectionCalculater->ProtectionActionBreakerTrip[0]==TRUE)
			{
				int isampno=0;																//跳闸后高压侧三相电压恢复
				for(int sampno=ActionPosition;sampno<WAVE_SAMPLE_COUNT;sampno++)
				{	
					pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno]=pProtectionCalculater->ProtectionSample.ChannelValue[0][isampno];
					pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno]=pProtectionCalculater->ProtectionSample.ChannelValue[1][isampno];
					pProtectionCalculater->ProtectionSample.ChannelValue[2][sampno]=pProtectionCalculater->ProtectionSample.ChannelValue[2][isampno];
					isampno=isampno+1;
					if(isampno>31) isampno=0;
					pProtectionCalculater->ProtectionSample.ChannelValue[3][sampno]=0;
					pProtectionCalculater->ProtectionSample.ChannelValue[4][sampno]=0;
					pProtectionCalculater->ProtectionSample.ChannelValue[5][sampno]=0;
					pProtectionCalculater->ProtectionSample.ChannelValue[6][sampno]=0;
					pProtectionCalculater->ProtectionSample.ChannelValue[7][sampno]=0;
					pProtectionCalculater->ProtectionSample.ChannelValue[8][sampno]=0;
				}
			}
			
			return TRUE;
		}
	}

	if((LossVoltageOnOff==TRUE)&&(ActiveType==0))		//失压保护投入
	{
		float UAValue,UBValue,UCValue,UaValue,UbValue,UcValue;
		int sampno=WAVE_SAMPLE_COUNT;					//取最后一个周波

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UAValue);	//计算UA
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UBValue);	//计算UB
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UCValue);	//计算UC
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,UaValue);	//计算Ua
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,UbValue);	//计算Ub
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,UcValue);	//计算Uc
		UAValue/=PTHValue;
		UBValue/=PTHValue;
		UCValue/=PTHValue;
		UaValue/=PTLValue;
		UbValue/=PTLValue;
		UcValue/=PTLValue;
		
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)					//高压断路器合闸(ID设备的遥信0号点)
		{
			if((max(max(UAValue,UBValue),UCValue)<LossVoltageValue)&&(UaValue<LossVoltageValue)&&(UbValue<LossVoltageValue)&&(UcValue<LossVoltageValue)) ActiveLossVoltage=TRUE;	//高压侧最大线电压电压小于失压定值	
			else ActiveLossVoltage=FALSE;
		}
		else ActiveLossVoltage=FALSE;
		if(ActiveLossVoltage==FALSE)
		{
			ClearTimer(&g_LossVoltageTimer[pProtectionCalculater->ProtectionID]);
		}
		else
		{
			if(g_LossVoltageTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_LossVoltageTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				BOOL TimerOut=FALSE;
				if((::GetTickCount()-g_LossVoltageTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(LossVoltageTime)) TimerOut=TRUE;
				if(TimerOut==TRUE)
				{
					ClearTimer(&g_LossVoltageTimer[pProtectionCalculater->ProtectionID]);
					ActiveType=3;
					ActiveTime=(WORD)::GetTickCount()%1000;
					ActionPosition=160+(WORD)::GetTickCount()%120;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//高压侧断路器动作跳闸
					pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//低压侧断路器动作跳闸
				}
			}
		}
	}

	if((OverLoadOnOff==TRUE)&&(OverLoadSection1OnOff==TRUE)&&(ActiveType==0))	//过负荷1段投入
	{
		float IAValue,IBValue,ICValue;
		int sampno=WAVE_SAMPLE_COUNT;											//取最后一个周波

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,IAValue);		//计算IA
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[7],sampno,IBValue);		//计算IB
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[8],sampno,ICValue);		//计算IC
		IAValue/=CTHValue;
		IBValue/=CTHValue;
		ICValue/=CTHValue;

		float TimePhaseA=100000;
		float TimePhaseB=100000;
		float TimePhaseC=100000;
		BOOL ActivePhaseA=FALSE;
		BOOL ActivePhaseB=FALSE;
		BOOL ActivePhaseC=FALSE;
		
		if(IAValue>OverLoadSection1AValue)
		{
			ActivePhaseA=TRUE;
			if(OverLoadSection1SXOnOff==TRUE)															//1:过负荷1段标准反时限,0:过负荷1段定时限
			{
				TimePhaseA=(float)0.14*OverLoadSection1Time*(IAValue/OverLoadSection1AValue-(float)1.0);
			}
		}
		if(IBValue>OverLoadSection1BValue)
		{
			ActivePhaseB=TRUE;
			if(OverLoadSection1SXOnOff==TRUE)
			{
				TimePhaseB=(float)0.14*OverLoadSection1Time*(IBValue/OverLoadSection1BValue-(float)1.0);
			}
		}
		if(ICValue>OverLoadSection1CValue)
		{
			ActivePhaseC=TRUE;
			if(OverLoadSection1SXOnOff==TRUE)
			{
				TimePhaseC=(float)0.14*OverLoadSection1Time*(ICValue/OverLoadSection1CValue-(float)1.0);
			}
		}
		if(ActivePhaseA||ActivePhaseB||ActivePhaseC)
		{
			ActiveOverLoadSection1=TRUE;
			if(OverLoadSection1SXOnOff==TRUE) OverLoadSection1Time=min(min(TimePhaseA,TimePhaseB),TimePhaseC);
			else OverLoadSection1Time=pProtectionCalculater->ProtectionParameter.Value[17]*1000;
		}
		else ActiveOverLoadSection1=FALSE;
		if(ActiveOverLoadSection1==FALSE)
		{
			ClearTimer(&g_OverLoadSection1Timer[pProtectionCalculater->ProtectionID]);
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=FALSE;
		}
		else
		{
			if(g_OverLoadSection1Timer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_OverLoadSection1Timer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				BOOL TimerOut=FALSE;
				if((::GetTickCount()-g_OverLoadSection1Timer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(OverLoadSection1Time)) TimerOut=TRUE;
				if(TimerOut==TRUE)
				{
					ClearTimer(&g_OverLoadSection1Timer[pProtectionCalculater->ProtectionID]);
					if(OverLoadSection1CKOnOff==TRUE)													//0:过负荷1段告警,1:过负荷1段跳闸 
					{
						ActiveType=4;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//高压侧断路器动作跳闸
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//低压侧断路器动作跳闸
					}
					else
					{
						ActiveType=22;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//保护动作告警
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=TRUE;
					}
				}
			}
		}
	}

	if((OverLoadOnOff==TRUE)&&(OverLoadSection2OnOff==TRUE)&&(ActiveType==0))	//过负荷2段压板投入
	{
		float IAValue,IBValue,ICValue;
		int sampno=WAVE_SAMPLE_COUNT;											//取最后一个周波

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,IAValue);	//计算IA
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[7],sampno,IBValue);	//计算IB
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[8],sampno,ICValue);	//计算IC
		IAValue/=CTHValue;
		IBValue/=CTHValue;
		ICValue/=CTHValue;

		float TimePhaseA=100000;
		float TimePhaseB=100000;
		float TimePhaseC=100000;
		BOOL ActivePhaseA=FALSE;
		BOOL ActivePhaseB=FALSE;
		BOOL ActivePhaseC=FALSE;

		if(IAValue>OverLoadSection2AValue)
		{
			ActivePhaseA=TRUE;
			if(OverLoadSection2SXOnOff==TRUE)															//1:过负荷2段标准反时限,0:过负荷2段定时限
			{
				TimePhaseA=(float)0.14*OverLoadSection2Time*(IAValue/OverLoadSection2AValue-(float)1.0);
			}
		}
		if(IBValue>OverLoadSection2BValue)
		{
			ActivePhaseB=TRUE;
			if(OverLoadSection2SXOnOff==TRUE)
			{
				TimePhaseB=(float)0.14*OverLoadSection2Time*(IBValue/OverLoadSection2BValue-(float)1.0);
			}
		}
		if(ICValue>OverLoadSection2CValue)
		{
			ActivePhaseC=TRUE;
			if(OverLoadSection2SXOnOff==TRUE)
			{
				TimePhaseC=(float)0.14*OverLoadSection2Time*(ICValue/OverLoadSection2CValue-(float)1.0);
			}
		}
		if(ActivePhaseA||ActivePhaseB||ActivePhaseC)
		{
			ActiveOverLoadSection2=TRUE;
			if(OverLoadSection2SXOnOff==TRUE) OverLoadSection2Time=min(min(TimePhaseA,TimePhaseB),TimePhaseC);
			else OverLoadSection2Time=pProtectionCalculater->ProtectionParameter.Value[21]*1000;
		}
		else ActiveOverLoadSection2=FALSE;
		if(ActiveOverLoadSection2==FALSE)
		{
			ClearTimer(&g_OverLoadSection2Timer[pProtectionCalculater->ProtectionID]);
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[23].Value=FALSE;
		}
		else
		{
			if(g_OverLoadSection2Timer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_OverLoadSection2Timer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				BOOL TimerOut=FALSE;
				if((::GetTickCount()-g_OverLoadSection2Timer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(OverLoadSection2Time)) TimerOut=TRUE;
				if(TimerOut==TRUE)
				{
					ClearTimer(&g_OverLoadSection2Timer[pProtectionCalculater->ProtectionID]);
					if(OverLoadSection2CKOnOff==TRUE)													//0:过负荷2段告警,1:过负荷2段跳闸 
					{	
						ActiveType=5;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//高压侧断路器动作跳闸
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//低压侧断路器动作跳闸
					}
					else
					{
						ActiveType=23;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//保护动作告警
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[23].Value=TRUE;
					}
				}
			}
		}
	}

	if(ActiveType!=0)
	{
		pProtectionCalculater->ProtectionActionType=ActiveType;
		pProtectionCalculater->ProtectionActionTime=ActiveTime;

		float ActionValueUA,ActionValueUB,ActionValueUC,ActionValueIA,ActionValueIB,ActionValueIC;
		float ActionValueUa,ActionValueUb,ActionValueUc;
		float ActionValueUAB,ActionValueUBC,ActionValueUCA;
		float ActionValueUAjd,ActionValueUBjd,ActionValueUCjd,ActionValueIAjd,ActionValueIBjd,ActionValueICjd;
			
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,ActionValueUA,ActionValueUAjd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,ActionValueUB,ActionValueUBjd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],ActionPosition,ActionValueUC,ActionValueUCjd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],ActionPosition,ActionValueUa);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],ActionPosition,ActionValueUb);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],ActionPosition,ActionValueUc);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],ActionPosition,ActionValueIA,ActionValueIAjd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[7],ActionPosition,ActionValueIB,ActionValueIBjd);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[8],ActionPosition,ActionValueIC,ActionValueICjd);
		ActionValueUA/=PTHValue;
		ActionValueUB/=PTHValue;
		ActionValueUC/=PTHValue;
		ActionValueIA/=CTHValue;
		ActionValueIB/=CTHValue;
		ActionValueIC/=CTHValue;
		ActionValueUa/=PTLValue;
		ActionValueUb/=PTLValue;
		ActionValueUc/=PTLValue;
		ActionValueUAB=(float)sqrt(pow(ActionValueUA*cos(ActionValueUAjd)-ActionValueUB*cos(ActionValueUBjd),2)+
			pow(ActionValueUA*sin(ActionValueUAjd)-ActionValueUB*sin(ActionValueUBjd),2));
		ActionValueUBC=(float)sqrt(pow(ActionValueUB*cos(ActionValueUBjd)-ActionValueUC*cos(ActionValueUCjd),2)+
			pow(ActionValueUB*sin(ActionValueUBjd)-ActionValueUC*sin(ActionValueUCjd),2));
		ActionValueUCA=(float)sqrt(pow(ActionValueUC*cos(ActionValueUCjd)-ActionValueUA*cos(ActionValueUAjd),2)+
			pow(ActionValueUC*sin(ActionValueUCjd)-ActionValueUA*sin(ActionValueUAjd),2));

		pProtectionCalculater->ProtectionActionValue.Value[0]=ActionValueUA;
		pProtectionCalculater->ProtectionActionValue.Value[1]=ActionValueUB;
		pProtectionCalculater->ProtectionActionValue.Value[2]=ActionValueUC;
		pProtectionCalculater->ProtectionActionValue.Value[3]=ActionValueIA;
		pProtectionCalculater->ProtectionActionValue.Value[4]=ActionValueIB;
		pProtectionCalculater->ProtectionActionValue.Value[5]=ActionValueIC;
		pProtectionCalculater->ProtectionActionValue.Value[6]=ActionValueUa;
		pProtectionCalculater->ProtectionActionValue.Value[7]=ActionValueUb;
		pProtectionCalculater->ProtectionActionValue.Value[8]=ActionValueUc;
		pProtectionCalculater->ProtectionActionValue.Value[9]=ActionValueUAB;
		pProtectionCalculater->ProtectionActionValue.Value[10]=ActionValueUBC;
		pProtectionCalculater->ProtectionActionValue.Value[11]=ActionValueUCA;
		if(pProtectionCalculater->ProtectionActionBreakerTrip[0]==TRUE)
		{
			int isampno=0;																//跳闸后高压侧三相电压恢复
			for(int sampno=ActionPosition;sampno<WAVE_SAMPLE_COUNT;sampno++)
			{	
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno]=pProtectionCalculater->ProtectionSample.ChannelValue[0][isampno];
				pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno]=pProtectionCalculater->ProtectionSample.ChannelValue[1][isampno];
				pProtectionCalculater->ProtectionSample.ChannelValue[2][sampno]=pProtectionCalculater->ProtectionSample.ChannelValue[2][isampno];
				isampno=isampno+1;
				if(isampno>31) isampno=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[3][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[4][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[5][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[6][sampno]=0;
				pProtectionCalculater->ProtectionSample.ChannelValue[7][sampno]=0;
			}
		}

		return TRUE;
	}

	if(ActiveLossVoltage==FALSE) ClearTimer(&g_LossVoltageTimer[pProtectionCalculater->ProtectionID]);			//清除失压计时器
	if(ActiveOverLoadSection1==FALSE) ClearTimer(&g_OverLoadSection1Timer[pProtectionCalculater->ProtectionID]);//清除过负荷1段计时器
	if(ActiveOverLoadSection2==FALSE) ClearTimer(&g_OverLoadSection2Timer[pProtectionCalculater->ProtectionID]);//清除过负荷2段计时器

	return TRUE;
}

BOOL WINAPI ProtectionCalculater3(TagProtectionCalculaterStruct *pProtectionCalculater)		//中压进线保护,采样通道顺序:Ua,Ub,Uc,Ia,Ib,Ic,I0
{
//读取保护压板
	BOOL OverCurrentSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];		//过流1段保护压板
	BOOL OverCurrentSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];		//过流2段保护压板
	BOOL ZeroOverCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[2];			//零序过流保护压板
	BOOL LossVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[3];				//失压保护压板
	BOOL OverLoadOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[4];					//过负荷保护压板
	BOOL OverLoadCKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[5];				//0:过负荷告警,1:过负荷跳闸
	BOOL OverLoadSXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[6];				//0:过负荷定时限,1:过负荷标准反时限
	BOOL LowVoltageSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[7];		//低压闭锁过流1段压板
	BOOL LowVoltageSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[8];		//低压闭锁过流2段压板
	BOOL PTUnusualSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[9];			//0:PT异常退出过流1段,1:PT异常退出1段低压闭锁
	BOOL PTUnusualSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[10];		//0:PT异常退出过流2段,1:PT异常退出2段低压闭锁
	BOOL PTUnusualOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[30];				//PT异常检测
//读取定值
	float PTLValue=pProtectionCalculater->ProtectionParameter.Value[0]*(float)0.001;			//低压侧PT变比
	float CTLValue=pProtectionCalculater->ProtectionParameter.Value[1];							//低压侧CT变比
	float CTZValue=pProtectionCalculater->ProtectionParameter.Value[2];							//零序CT变比
	float OverCurrentSection1aValue=pProtectionCalculater->ProtectionParameter.Value[3];		//过流1段a相定值
	float OverCurrentSection1bValue=pProtectionCalculater->ProtectionParameter.Value[4];		//过流1段b相定值
	float OverCurrentSection1cValue=pProtectionCalculater->ProtectionParameter.Value[5];		//过流1段c相定值
	float OverCurrentSection1Time=pProtectionCalculater->ProtectionParameter.Value[6]*1000;		//过流1段时间定值
	float OverCurrentSection2aValue=pProtectionCalculater->ProtectionParameter.Value[7];		//过流2段a相定值
	float OverCurrentSection2bValue=pProtectionCalculater->ProtectionParameter.Value[8];		//过流2段b相定值
	float OverCurrentSection2cValue=pProtectionCalculater->ProtectionParameter.Value[9];		//过流2段c相定值
	float OverCurrentSection2Time=pProtectionCalculater->ProtectionParameter.Value[10]*1000;	//过流2段时间定值
	float LowVoltageSectionValue=pProtectionCalculater->ProtectionParameter.Value[11];			//低压闭锁过流电压定值	
	float LossVoltageValue=pProtectionCalculater->ProtectionParameter.Value[12];				//失压保护定值
	float LossVoltageTime=pProtectionCalculater->ProtectionParameter.Value[13]*1000;			//失压时间定值
	float OverLoadaValue=pProtectionCalculater->ProtectionParameter.Value[14];					//a相过负荷保护定值
	float OverLoadbValue=pProtectionCalculater->ProtectionParameter.Value[15];					//b相过负荷保护定值
	float OverLoadcValue=pProtectionCalculater->ProtectionParameter.Value[16];					//c相过负荷保护定值
	float OverLoadTime=pProtectionCalculater->ProtectionParameter.Value[17]*1000;				//过负荷时间定值
	float ZeroOverCurrentValue=pProtectionCalculater->ProtectionParameter.Value[18];			//零序过流电流定值
	float ZeroOverCurrentTime=pProtectionCalculater->ProtectionParameter.Value[19]*1000;		//零序过流时间定值

	BOOL ActiveOverCurrentSection1=FALSE;		//过电流1段动作标志		1
	BOOL ActiveOverCurrentSection2=FALSE;		//过电流2段动作标志		2
	BOOL ActiveZeroOverCurrent=FALSE;			//零序过流动作标志		3
	BOOL ActiveLossVoltage=FALSE;				//失压动作标志			4
	BOOL ActiveOverLoad=FALSE;					//过负荷动作标志		5
	BOOL ActivePT=FALSE;						//PT异常
	BOOL ZeroSequenceMeasure=FALSE;
				
	int OverCurrentSection1JS=0;
	int OverCurrentSection2JS=0;
	int ZeroOverCurrentJS=0;
	
    WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;
				
	if(PTUnusualOnOff==TRUE)					//PT异常投入
	{
		for(int sampno=32;sampno<60;sampno+=1)
		{
			float UaValue,UbValue,UcValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UaValue);	//计算Ua
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UbValue);	//计算Ub
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UcValue);	//计算Uc
			UaValue/=PTLValue;
			UbValue/=PTLValue;
			UcValue/=PTLValue;

			if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)				//低压侧断路器合闸(ID设备的遥信0号点)
			{
				if(min(min(UaValue,UbValue),UcValue)<70)
				{
					ActivePT=TRUE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=TRUE;		//向ID设备的遥信18号点发"低压侧PT异常"
				}
				else
				{
					ActivePT=FALSE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=FALSE;	//向ID设备的遥信18号点发"低压侧PT正常"
				}
			}
		}
	}
	else
	{
		ActivePT=FALSE;
		g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=FALSE;
	}

	if((OverCurrentSection1OnOff==TRUE)||(OverCurrentSection2OnOff==TRUE))		//过流1段或过流2段压板投入
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;
			
			float UaValue,UbValue,UcValue,IaValue,IbValue,IcValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UaValue);	//计算Ua
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UbValue);	//计算Ub
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UcValue);	//计算Uc
			UaValue/=PTLValue;
			UbValue/=PTLValue;
			UcValue/=PTLValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);	//计算Ia
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);	//计算Ib
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);	//计算Ic
			IaValue/=CTLValue;
			IbValue/=CTLValue;
			IcValue/=CTLValue;

			if((OverCurrentSection1OnOff==TRUE)&&(ActiveType==0))										//过流1段压板投入											
			{
				if(LowVoltageSection1OnOff==TRUE)														//低压闭锁过流1段投入
				{
					if(ActivePT==TRUE)																	//PT异常
					{
						if(PTUnusualSection1OnOff==TRUE)												//退出过流1段低压闭锁
						{
							if((IaValue>OverCurrentSection1aValue)||(IbValue>OverCurrentSection1bValue)||(IcValue>OverCurrentSection1cValue)) ActiveOverCurrentSection1=TRUE;
							else ActiveOverCurrentSection1=FALSE;
						}
						else ActiveOverCurrentSection1=FALSE;											//退出过流1段
					}
					else																				//PT正常
					{
						if((UaValue<LowVoltageSectionValue)||(UbValue<LowVoltageSectionValue)||(UcValue<LowVoltageSectionValue))
						{
							if((IaValue>OverCurrentSection1aValue)||(IbValue>OverCurrentSection1bValue)||(IcValue>OverCurrentSection1cValue)) ActiveOverCurrentSection1=TRUE;
							else ActiveOverCurrentSection1=FALSE;
						}
						else ActiveOverCurrentSection1=FALSE;
					}
				}
				else																					//低压闭锁过流1段未投
				{
					if((IaValue>OverCurrentSection1aValue)||(IbValue>OverCurrentSection1bValue)||(IcValue>OverCurrentSection1cValue)) ActiveOverCurrentSection1=TRUE;
					else ActiveOverCurrentSection1=FALSE;
				}		
				if(ActiveOverCurrentSection1==TRUE)
				{
					if(OverCurrentSection1JS==int(OverCurrentSection1Time*0.1))
					{
						ActiveType=1;
						ActiveTime=(WORD)OverCurrentSection1Time;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//低压侧断路器动作跳闸
					}
					OverCurrentSection1JS+=1;
				}
			}

			if((OverCurrentSection2OnOff==TRUE)&&(ActiveType==0))										//过流2段压板投入
			{
				if(LowVoltageSection2OnOff==TRUE)														//低压闭锁过流2段投入
				{
					if(ActivePT==TRUE)																	//PT异常
					{
						if(PTUnusualSection2OnOff==TRUE)												//退出过流2段低压闭锁
						{
							if((IaValue>OverCurrentSection2aValue)||(IbValue>OverCurrentSection2bValue)||(IcValue>OverCurrentSection2cValue)) ActiveOverCurrentSection2=TRUE;
							else ActiveOverCurrentSection2=FALSE;
						}
						else ActiveOverCurrentSection2=FALSE;											//退出过流2段
					}
					else																				//PT正常
					{
						if((UaValue<LowVoltageSectionValue)||(UbValue<LowVoltageSectionValue)||(UcValue<LowVoltageSectionValue))
						{
							if((IaValue>OverCurrentSection2aValue)||(IbValue>OverCurrentSection2bValue)||(IcValue>OverCurrentSection2cValue)) ActiveOverCurrentSection2=TRUE;
							else ActiveOverCurrentSection2=FALSE;
						}
						else ActiveOverCurrentSection2=FALSE;
					}
				}
				else																					//低压闭锁过流2段未投
				{
					if((IaValue>OverCurrentSection2aValue)||(IbValue>OverCurrentSection2bValue)||(IcValue>OverCurrentSection2cValue)) ActiveOverCurrentSection2=TRUE;
					else ActiveOverCurrentSection2=FALSE;
				}		
				if(ActiveOverCurrentSection2==TRUE)
				{
					if(OverCurrentSection2JS==int(OverCurrentSection2Time*0.1))
					{
						ActiveType=2;
						ActiveTime=(WORD)OverCurrentSection2Time;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//低压侧断路器动作跳闸
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
					ActiveType=3;
					ActiveTime=(WORD)ZeroOverCurrentTime;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;							//低压侧断路器动作跳闸
				}
				ZeroOverCurrentJS+=1;
			}
		}
	}

	if(ActiveType!=0)
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
		ActionValueUa/=PTLValue;
		ActionValueUb/=PTLValue;
		ActionValueUc/=PTLValue;
		ActionValueIa/=CTLValue;
		ActionValueIb/=CTLValue;
		ActionValueIc/=CTLValue;
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
			}
		}
			
		return TRUE;
	}

	if((LossVoltageOnOff==TRUE)&&(ActiveType==0))		//失压压板投入
	{	
		float UaValue,UbValue,UcValue; 
		int sampno=WAVE_SAMPLE_COUNT;					//取最后一个周波
		   
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UaValue);			//计算Ua
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UbValue);			//计算Ub
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UcValue);			//计算Uc
		UaValue/=PTLValue;
		UbValue/=PTLValue;
		UcValue/=PTLValue;

		if((UaValue<LossVoltageValue)&&(UbValue<LossVoltageValue)&&(UcValue<LossVoltageValue))																		//a相电压小于失压定值
		{
			if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0) ActiveLossVoltage=TRUE;	//低压侧断路器合闸
			else ActiveLossVoltage=FALSE;
		}
		else ActiveLossVoltage=FALSE;

		if(ActiveLossVoltage==FALSE)
		{
			ClearTimer(&g_LossVoltageJXTimer[pProtectionCalculater->ProtectionID]);
		}
		else
		{
			if(g_LossVoltageJXTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_LossVoltageJXTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				BOOL TimerOut=FALSE;
				if((::GetTickCount()-g_LossVoltageJXTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(LossVoltageTime)) TimerOut=TRUE;
				if(TimerOut==TRUE)
				{
					ClearTimer(&g_LossVoltageJXTimer[pProtectionCalculater->ProtectionID]);
					ActiveType=4;
					ActiveTime=(WORD)::GetTickCount()%1000;
					ActionPosition=160+(WORD)::GetTickCount()%120;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//低压侧断路器动作跳闸
				}
			}
		}
	}

	if((OverLoadOnOff==TRUE)&&(ActiveType==0))		//过负荷压板投入
	{
		float IaValue,IbValue,IcValue;
		int sampno=WAVE_SAMPLE_COUNT;											//取最后一个周波

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);		//计算Ia
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);		//计算Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);		//计算Ic
		IaValue/=CTLValue;
		IbValue/=CTLValue;
		IcValue/=CTLValue;

		float TimePhasea=100000;
		float TimePhaseb=100000;
		float TimePhasec=100000;
		BOOL ActivePhasea=FALSE;
		BOOL ActivePhaseb=FALSE;
		BOOL ActivePhasec=FALSE;
		
		if(IaValue>OverLoadaValue)
		{
			ActivePhasea=TRUE;
			if(OverLoadSXOnOff==TRUE)
			{
				TimePhasea=(float)0.14*OverLoadTime*(IaValue/OverLoadaValue-(float)1.0);
			}
		}
		if(IbValue>OverLoadbValue)
		{
			ActivePhaseb=TRUE;
			if(OverLoadSXOnOff==TRUE)
			{
				TimePhaseb=(float)0.14*OverLoadTime*(IbValue/OverLoadbValue-(float)1.0);
			}
		}
		if(IcValue>OverLoadcValue)
		{
			ActivePhasec=TRUE;
			if(OverLoadSXOnOff==TRUE)
			{
				TimePhasec=(float)0.14*OverLoadTime*(IcValue/OverLoadcValue-(float)1.0);
			}
		}
		if(ActivePhasea||ActivePhaseb||ActivePhasec)
		{
			ActiveOverLoad=TRUE;
			if(OverLoadSXOnOff==TRUE) OverLoadTime=min(min(TimePhasea,TimePhaseb),TimePhasec);
			else OverLoadTime=pProtectionCalculater->ProtectionParameter.Value[16]*1000;
		}
		else ActiveOverLoad=FALSE;

		if(ActiveOverLoad==FALSE)
		{
			ClearTimer(&g_OverLoadJXTimer[pProtectionCalculater->ProtectionID]);
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=FALSE;
		}
		else
		{
			if(g_OverLoadJXTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_OverLoadJXTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				BOOL TimerOut=FALSE;
				if((::GetTickCount()-g_OverLoadJXTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(OverLoadTime)) TimerOut=TRUE;
				if(TimerOut==TRUE)
				{
					ClearTimer(&g_OverLoadJXTimer[pProtectionCalculater->ProtectionID]);
					if(OverLoadCKOnOff==TRUE)
					{
						ActiveType=5;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//低压侧断路器动作跳闸
			
					}
					else
					{
						ActiveType=22;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//保护动作告警
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=TRUE;
					}
				}
			}
		}
	}

/*	if((ZeroOverCurrentOnOff==TRUE)&&(ActiveType==0))		//零序保护软压板投入
	{
		float I0Value;
		int sampno=WAVE_SAMPLE_COUNT;						//取最后一个周波
				
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,I0Value);	//计算I0
		I0Value/=CTZValue;	
		if(I0Value>ZeroOverCurrentValue) ActiveZeroOverCurrent=TRUE;
		else ActiveZeroOverCurrent=FALSE;

		if(ActiveZeroOverCurrent==FALSE)
		{
			ClearTimer(&g_ZeroOverCurrentJXTimer[pProtectionCalculater->ProtectionID]);
		}
		else
		{
			if(g_ZeroOverCurrentJXTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
			{
				StartupTimer(&g_ZeroOverCurrentJXTimer[pProtectionCalculater->ProtectionID]);
			}
			else
			{
				if((::GetTickCount()-g_ZeroOverCurrentJXTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(ZeroOverCurrentTime))
				{
					ClearTimer(&g_ZeroOverCurrentJXTimer[pProtectionCalculater->ProtectionID]);
					ActiveType=3;
					ActiveTime=(WORD)::GetTickCount()%1000;
					ActionPosition=160+(WORD)::GetTickCount()%120;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//馈线断路器动作跳闸
				}
			}
		}
	}*/

	if(ActiveType!=0)
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
		ActionValueUa/=PTLValue;
		ActionValueUb/=PTLValue;
		ActionValueUc/=PTLValue;
		ActionValueIa/=CTLValue;
		ActionValueIb/=CTLValue;
		ActionValueIc/=CTLValue;
		ActionValueI0=0.0;
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
			}
		}

		return TRUE;
	}
										
	if(ActiveLossVoltage==FALSE) ClearTimer(&g_LossVoltageJXTimer[pProtectionCalculater->ProtectionID]);
	if(ActiveOverLoad==FALSE) ClearTimer(&g_OverLoadJXTimer[pProtectionCalculater->ProtectionID]);
	if(ActiveZeroOverCurrent==FALSE) ClearTimer(&g_ZeroOverCurrentJXTimer[pProtectionCalculater->ProtectionID]);

	return TRUE;
}

BOOL WINAPI ProtectionCalculater4(TagProtectionCalculaterStruct *pProtectionCalculater)		//中压馈线保护,采样通道顺序:Ua,Ub,Uc,Ia,Ib,Ic,I0
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
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=FALSE;
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
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=TRUE;
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

//BOOL WINAPI ProtectionCalculater6(TagProtectionCalculaterStruct *pProtectionCalculater)		//采样通道顺序:电容电压U,电容电流I,差流一组SI1,差压一组SU1,差流二组SI2,差压二组SU2
//{
//	return TRUE;
//}


