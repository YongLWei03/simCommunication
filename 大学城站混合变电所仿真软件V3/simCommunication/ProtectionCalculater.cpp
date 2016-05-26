////�ǹ�ǣ����ѹ��ϱ�����̵籣���������(AC35kV/AC400V/DC750V)
#include "stdafx.h"
#include "ProtectionCalculater.h"
#define	PI 3.141592654
#define	number_of_samples 32		//ÿ�ܲ���������,��������Ϊ0.625ms

typedef struct TagTimerStruct		//��ʱ��
{
	BOOL  StartupState;				//����״̬
	DWORD StartupTickCount;			//��������
}TIMERSTRUCT;

static struct TagTimerStruct g_LossVoltageXLTimer[UNITCOUNT];		// ʧѹ��ʱ��(������ѹ��·����)
static struct TagTimerStruct g_OverLoadXLTimer[UNITCOUNT];			// �����ɼ�ʱ��(������ѹ��·����)
static struct TagTimerStruct g_LossVoltageDKXTimer[UNITCOUNT];		// �͵�ѹ��ʱ��(����ֱ�����߱���)
static struct TagTimerStruct g_OverLoadDKXTimer[UNITCOUNT];			// �����ɼ�ʱ��(����ֱ�����߱���)
static struct TagTimerStruct g_OverCurrentDKXTimer[UNITCOUNT];		// ��������ʱ��(����ֱ�����߱���)
static struct TagTimerStruct g_RecloserDKXTimer[UNITCOUNT];			// �غ�բ��ʱ��(����ֱ�����߱���)
static struct TagTimerStruct g_LeakVoltageKJTimer[UNITCOUNT];		// й©��ѹ��ʱ��(����ֱ����ܱ���)

static BOOL ActiveCurrentQuickBreakKX=FALSE;						// �����غ�բ����բ�ж�
static BOOL ActiveDDLKX=FALSE;
static BOOL ActiveOverCurrentKX=FALSE;
static BOOL ActiveBilateralBreakKX=FALSE;						
static BOOL ForeverShort=FALSE;										// ���߷�����·���ϵ�����,0:˲ʱ�Զ�·,1:�����Զ�·

BOOL StartupTimer(TagTimerStruct *pTimer)
{
	if(pTimer->StartupState==TRUE) return FALSE;
	pTimer->StartupState=TRUE;										//���ü�ʱ������״̬
	pTimer->StartupTickCount=::GetTickCount();						//���ü�ʱ������ʱ��
				
	return TRUE;
}

BOOL ClearTimer(TagTimerStruct *pTimer)
{
	if(pTimer->StartupState==FALSE) return FALSE;
	pTimer->StartupState=FALSE;										//�����ʱ��״̬
	pTimer->StartupTickCount=0;
				
	return TRUE;
}

BOOL CalculaterValue(float SampleValueList[WAVE_SAMPLE_COUNT],WORD BeginPosition,float &FundamentalWaveValue)	//�����BeginPosition��ʼ��ǰ32��������Ļ���ֵ
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

BOOL CalculaterValue(float SampleValueList[WAVE_SAMPLE_COUNT],WORD BeginPosition,int Harmonic,float &FundamentalWaveValue)	//�����BeginPosition��ʼ��ǰ32���������Harmonic��г��ֵ
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

BOOL CalculaterValue(float SampleValueList[WAVE_SAMPLE_COUNT],WORD BeginPosition,float &DDValue,float &DCValue,BOOL Result)	//�����BeginPosition��ʼ��ֱ����
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
		case 1:														//��ѹ��·����
		case 3:														//��ѹ��ѹ������
		case 4:														//��ѹ�������鱣��
		case 8:														//��ѹ��·����
			ProtectionCalculater1(pProtectionCalculater);
			break;
		case 5:														//ֱ�����߱���
		case 6:														//ֱ�����߱���
			ProtectionCalculater2(pProtectionCalculater);
			break;
		case 7:														//ֱ����ܱ���
			ProtectionCalculater3(pProtectionCalculater);
			break;
	}
	
	return TRUE;
}

BOOL WINAPI ProtectionCalculater1(TagProtectionCalculaterStruct *pProtectionCalculater)		//��������,����ͨ��˳��:Ua,Ub,Uc,Ia,Ib,Ic,I0
{
//��ȡ����ѹ��
	BOOL CurrentQuickBreakOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];				//�����ٶϱ���ѹ��
	BOOL OverCurrentSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];			//������1�α���ѹ��
	BOOL OverCurrentSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[2];			//������2�α���ѹ��
	BOOL SpillCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[3];					//��·�ݲ��ѹ��
	BOOL ZeroOverCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[4];				//�����������ѹ��
	BOOL LossVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[5];					//ʧѹ����ѹ��
	BOOL OverLoadOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[6];						//�����ɱ���ѹ��
	BOOL OverLoadCKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[7];					//0:�����ɸ澯,1:��������բ
	BOOL OverLoadSXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[8];					//0:�����ɶ�ʱ��,1:�����ɷ�ʱ��
	BOOL ZeroSequenceMeasure=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[9];				//0:���������,1:���򻥸���
	BOOL PTUnusualOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[30];					//PT�쳣���
//��ȡ��ֵ
	float PTFValue=pProtectionCalculater->ProtectionParameter.Value[0]*(float)0.001;				//����PT���
	float CTFValue=pProtectionCalculater->ProtectionParameter.Value[1];								//����CT���
	float CTZValue=pProtectionCalculater->ProtectionParameter.Value[2];								//����CT���
	float CurrentMutationValue=pProtectionCalculater->ProtectionParameter.Value[3];					//����ͻ����������ֵ
	float CurrentQuickBreakAValue=pProtectionCalculater->ProtectionParameter.Value[4];				//A���ٶϵ�����ֵ
	float CurrentQuickBreakBValue=pProtectionCalculater->ProtectionParameter.Value[5];				//B���ٶϵ�����ֵ
	float CurrentQuickBreakCValue=pProtectionCalculater->ProtectionParameter.Value[6];				//C���ٶϵ�����ֵ
	float CurrentQuickBreakTime=pProtectionCalculater->ProtectionParameter.Value[7]*1000;			//�����ٶ�ʱ�䶨ֵ
	float OverCurrentSection1AValue=pProtectionCalculater->ProtectionParameter.Value[8];			//A�����1�ε�����ֵ
	float OverCurrentSection1BValue=pProtectionCalculater->ProtectionParameter.Value[9];			//B�����1�ε�����ֵ
	float OverCurrentSection1CValue=pProtectionCalculater->ProtectionParameter.Value[10];			//C�����1�ε�����ֵ
	float OverCurrentSection1Time=pProtectionCalculater->ProtectionParameter.Value[11]*1000;		//����1��ʱ�䶨ֵ
	float OverCurrentSection2AValue=pProtectionCalculater->ProtectionParameter.Value[12];			//A�����2�ε�����ֵ
	float OverCurrentSection2BValue=pProtectionCalculater->ProtectionParameter.Value[13];			//B�����2�ε�����ֵ
	float OverCurrentSection2CValue=pProtectionCalculater->ProtectionParameter.Value[14];			//C�����2�ε�����ֵ
	float OverCurrentSection2Time=pProtectionCalculater->ProtectionParameter.Value[15]*1000;		//����2��ʱ�䶨ֵ
	float LossVoltageValue=pProtectionCalculater->ProtectionParameter.Value[16];					//ʧѹ������ֵ
	float LossVoltageTime=pProtectionCalculater->ProtectionParameter.Value[17]*1000;				//ʧѹʱ�䶨ֵ
	float OverLoadAValue=pProtectionCalculater->ProtectionParameter.Value[18];						//A������ɵ�����ֵ
	float OverLoadBValue=pProtectionCalculater->ProtectionParameter.Value[19];						//B������ɵ�����ֵ
	float OverLoadCValue=pProtectionCalculater->ProtectionParameter.Value[20];						//C������ɵ�����ֵ
	float OverLoadTime=pProtectionCalculater->ProtectionParameter.Value[21]*1000;					//������ʱ�䶨ֵ
	float ZeroOverCurrentValue=pProtectionCalculater->ProtectionParameter.Value[22];				//�������������ֵ
	float ZeroOverCurrentTime=pProtectionCalculater->ProtectionParameter.Value[23]*1000;			//�������ʱ�䶨ֵ
				
	BOOL ActiveCurrentQuickBreak=FALSE;		//�����ٶ϶�����־		1
	BOOL ActiveOverCurrentSection1=FALSE;	//������1�ζ�����־		2
	BOOL ActiveOverCurrentSection2=FALSE;	//������2�ζ�����־		3
	BOOL ActiveLossVoltage=FALSE;			//ʧѹ������־			4
	BOOL ActiveOverLoad=FALSE;				//�����ɶ�����־		5
	BOOL ActiveZeroOverCurrent=FALSE;		//�������������־		6
	BOOL ActiveSpillCurrent=FALSE;			//��·�ݲ����־		7
	BOOL ActivePT=FALSE;					//PT�쳣��־			18
				
	int CurrentQuickBreakJS=0;				//�����ٶϼ�����
	int OverCurrentSection1JS=0;			//������1�μ�����
	int OverCurrentSection2JS=0;			//������2�μ�����
	int ZeroOverCurrentJS=0;				//�������������
	int SpillCurrentJS=0;					//�����ݲ������

	WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;

	if((CurrentQuickBreakOnOff==TRUE)&&(ActiveType==0))		//�����ٶϱ���Ͷ��
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float IaValue,IbValue,IcValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);	//����Ia
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);	//����Ib
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);	//����Ic
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;							//��·��·��������բ
				}
				CurrentQuickBreakJS+=1;
			}
		}
	}

    if((OverCurrentSection1OnOff==TRUE)||(OverCurrentSection2OnOff==TRUE))		//����������Ͷ��
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float IaValue,IbValue,IcValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);	//����Ia
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);	//����Ib
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);	//����Ic
			IaValue/=CTFValue;
			IbValue/=CTFValue;
			IcValue/=CTFValue;

			if((OverCurrentSection1OnOff==TRUE)&&(ActiveType==0))										//������1�α���Ͷ��
			{
				if((IaValue>OverCurrentSection1AValue)||(IbValue>OverCurrentSection1BValue)||(IcValue>OverCurrentSection1CValue))
				{
					if(OverCurrentSection1JS==int(OverCurrentSection1Time*0.1))	
					{
						ActiveOverCurrentSection1=TRUE;
						ActiveType=2;
						ActiveTime=(WORD)OverCurrentSection1Time;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��·��·��������բ
					}
					OverCurrentSection1JS+=1;
				}
			}
			if((OverCurrentSection2OnOff==TRUE)&&(ActiveType==0))										//������2�α���Ͷ��
			{
				if((IaValue>OverCurrentSection2AValue)||(IbValue>OverCurrentSection2BValue)||(IcValue>OverCurrentSection2CValue))
				{
					if(OverCurrentSection2JS==int(OverCurrentSection2Time*0.1))
					{
						ActiveOverCurrentSection2=TRUE;
						ActiveType=3;
						ActiveTime=(WORD)OverCurrentSection2Time;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��·��·��������բ
					}
					OverCurrentSection2JS+=1;
				}
			}
		}
	}

	if((ZeroOverCurrentOnOff==TRUE)&&(ActiveType==0))		//�����������Ͷ��
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;							//��·��·��������բ
				}
				ZeroOverCurrentJS+=1;
			}
		}
	}

	if((SpillCurrentOnOff==TRUE)&&(ActiveType==0))			//��·�ݲ��Ͷ��
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

		}
	}

	if((LossVoltageOnOff==TRUE)&&(ActiveType==0))			//ʧѹ����Ͷ��
	{
		float UaValue,UbValue,UcValue,IaValue,IbValue,IcValue;		
		int sampno=WAVE_SAMPLE_COUNT;						//ȡ���һ���ܲ�

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UaValue);		//����Ua
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UbValue);		//����Ub
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UcValue);		//����Uc
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);		//����Ia
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);		//����Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);		//����Ic
		UaValue/=PTFValue;
		UbValue/=PTFValue;
		UcValue/=PTFValue;
		IaValue/=CTFValue;
		IbValue/=CTFValue;
		IcValue/=CTFValue;

		if(((UaValue<LossVoltageValue)&&(IaValue<0.4))||((UbValue<LossVoltageValue)&&(IbValue<0.4))||((UcValue<LossVoltageValue)&&(IcValue<0.4)))														//��ѹС��ʧѹ��ֵ
		{
			if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0) ActiveLossVoltage=TRUE;	//��·���ں�λ
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

	if((OverLoadOnOff==TRUE)&&(ActiveType==0))				//�����ɱ���Ͷ��
	{
		float IaValue,IbValue,IcValue;
		int sampno=WAVE_SAMPLE_COUNT;						//ȡ���һ���ܲ�

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);	//����Ia
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);	//����Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);	//����Ic
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
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;					//��·��·��������բ
					}
					else
					{
						ActiveType=22;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;				//���������澯
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=1;
					}
				}
			}	
		}	
	}

	if(ActiveType!=0)		//���㱣��������Ϣ
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
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno]=0;		//a���ѹ
				pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno]=0;		//b���ѹ
				pProtectionCalculater->ProtectionSample.ChannelValue[2][sampno]=0;		//c���ѹ
				pProtectionCalculater->ProtectionSample.ChannelValue[3][sampno]=0;		//a�����
				pProtectionCalculater->ProtectionSample.ChannelValue[4][sampno]=0;		//b�����
				pProtectionCalculater->ProtectionSample.ChannelValue[5][sampno]=0;		//c�����
				pProtectionCalculater->ProtectionSample.ChannelValue[6][sampno]=0;		//�������
			}
		}

		return TRUE;
	}

	if(ActiveLossVoltage==FALSE) ClearTimer(&g_LossVoltageXLTimer[pProtectionCalculater->ProtectionID]);	//���ʧѹ��ʱ��
	if(ActiveOverLoad==FALSE) ClearTimer(&g_OverLoadXLTimer[pProtectionCalculater->ProtectionID]);			//��������ɼ�ʱ��
	
	return TRUE;
}

BOOL WINAPI ProtectionCalculater2(TagProtectionCalculaterStruct *pProtectionCalculater)		//ֱ������,����ͨ��˳��:Ud,Id
{
//��ȡ����ѹ��
	BOOL CurrentQuickBreakOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];			//�����ٶϱ���ѹ��
	BOOL DDLOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];						//���������ʼ�������������ѹ��
	BOOL OverCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[2];				//����������ѹ��
	BOOL LossVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[3];				//�͵�ѹ����ѹ��
	BOOL OverLoadOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[4];					//�����ɱ���ѹ��
	BOOL BilateralBreakOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[5];			//˫����������ѹ��
	BOOL RecloserOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[6];					//�غ�բѹ��
	BOOL OverLoadCKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[7];				//0:�����ɸ澯,1:��������բ
	BOOL OverLoadSXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[8];				//0:�����ɷ�ʱ��Ͷ��,1:�����ɶ�ʱ��Ͷ��
//��ȡ��ֵ
	float CurrentQuickBreakValue=pProtectionCalculater->ProtectionParameter.Value[0];			//�ٶϱ���������ֵA
	float CurrentQuickBreakTime=pProtectionCalculater->ProtectionParameter.Value[1];			//�ٶϱ���ʱ�䶨ֵms
	float DDLEValue=pProtectionCalculater->ProtectionParameter.Value[2]*(float)0.625;			//����������������ֵA/ms
	float DDLFValue=pProtectionCalculater->ProtectionParameter.Value[3]*(float)0.625;			//���������ʷ��ض�ֵA/ms
	float DDLDImaxValue=pProtectionCalculater->ProtectionParameter.Value[4];					//������������ֵA
	float DDLDImaxTime=pProtectionCalculater->ProtectionParameter.Value[5];						//������������ʱ��ֵms
	float DDLDIminValue=pProtectionCalculater->ProtectionParameter.Value[6];					//��С����������ֵA
	float DDLDIminTime=pProtectionCalculater->ProtectionParameter.Value[7];						//��С����������ʱ��ֵms
	float OverCurrentValue=pProtectionCalculater->ProtectionParameter.Value[8];					//��������������ֵA
	float OverCurrentTime=pProtectionCalculater->ProtectionParameter.Value[9]*1000;				//��������ʱ�䶨ֵs
	float LossVoltageValue=pProtectionCalculater->ProtectionParameter.Value[10];				//�͵�ѹ������ѹ��ֵV
	float LossVoltageTime=pProtectionCalculater->ProtectionParameter.Value[11]*1000;			//�͵�ѹ����ʱ�䶨ֵs
	float OverLoadValue=pProtectionCalculater->ProtectionParameter.Value[12];					//�����ɵ�����ֵ
	float OverLoadTime=pProtectionCalculater->ProtectionParameter.Value[13]*1000;				//������ʱ�䶨ֵ
	float RecloserTime=pProtectionCalculater->ProtectionParameter.Value[14]*1000;				//�غ�բʱ�䶨ֵs
	float RecloserBreakNum=pProtectionCalculater->ProtectionParameter.Value[15];				//�غ�բ��������
	float TestTime=pProtectionCalculater->ProtectionParameter.Value[16]*1000;					//�Ӵ�������ʱ�䶨ֵs
	float TestNum=pProtectionCalculater->ProtectionParameter.Value[17];							//���Դ�����ֵ
	float TestDelay=pProtectionCalculater->ProtectionParameter.Value[18]*1000;					//����ʱ������ֵs
	float TestUflow=pProtectionCalculater->ProtectionParameter.Value[19];						//��·��͹�����ѹ��ֵ
	float TestUfres=pProtectionCalculater->ProtectionParameter.Value[20];						//��·��ѹ��ֵ
	float TestRmin=pProtectionCalculater->ProtectionParameter.Value[21];						//���Ե�����Сֵ��ֵ
	float TestUcalib=pProtectionCalculater->ProtectionParameter.Value[22];						//
	float TestRcalib=pProtectionCalculater->ProtectionParameter.Value[23];						//

	BOOL ActiveCurrentQuickBreak=FALSE;			//�����ٶ϶�����־		1
	BOOL ActiveDDL=FALSE;						//DDL������־			2
	BOOL ActiveOverCurrent=FALSE;				//����������־			3
	BOOL ActiveLossVoltage=FALSE;				//�͵�ѹ������־		4
	BOOL ActiveOverLoad=FALSE;					//�����ɶ�����־		5
	BOOL ActiveBilateralBreak=FALSE;			//˫������������־		6
	BOOL ActiveRecloser=FALSE;					//�غ�բ������־		7
	BOOL ActiveLineTest=FALSE;					//��·���Ա�־			
	
	int CurrentQuickBreakJS=0;
	int DDLJS=0;
	int OverCurrentJS=0;

	static float TempChannelValue[2][320];

	WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;

	if((CurrentQuickBreakOnOff==TRUE)&&(ActiveType==0))			//�����ٶϱ���Ͷ��
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//���߶�·��������բ
				}
				CurrentQuickBreakJS+=1;
			}
		}
	}

	if((DDLOnOff==TRUE)&&(ActiveType==0))			//���������ʼ�������������Ͷ��
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//���߶�·��������բ
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//���߶�·��������բ
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//���߶�·��������բ
				}
			}
		}
	}

	if((OverCurrentOnOff==TRUE)&&(ActiveType==0))			//����������Ͷ��
	{
		float didtValue,IValue;
		int sampno=WAVE_SAMPLE_COUNT-1;						//ȡ���һ���ܲ�

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)			//��·����բ
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//���߶�·��������բ
				}
			}
		}
	}

	if((LossVoltageOnOff==TRUE)&&(ActiveType==0))			//�͵�ѹ����Ͷ��
	{
		float dudtValue,didtValue,UValue,IValue;
		int sampno=WAVE_SAMPLE_COUNT-1;						//ȡ���һ��������

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,dudtValue,UValue,TRUE);
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)			//��·����բ
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//���߶�·��������բ
				}
			}
		}
	}

	if((OverLoadOnOff==TRUE)&&(ActiveType==0))		//�����ɱ���Ͷ��
	{
		float didtValue,IValue;
		int sampno=WAVE_SAMPLE_COUNT-1;				//ȡ���һ���ܲ�

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

	if(ActiveType!=0)		//���㱣��������Ϣ
	{
		float didtValue,IValue;		
		int sampno=WAVE_SAMPLE_COUNT-1;
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,didtValue,IValue,TRUE);
		if(IValue>1.0) ForeverShort=TRUE;													//��·Ϊ������
		else ForeverShort=FALSE;															//��·Ϊ˲ʱ��

		for(int i=0;i<WAVE_SAMPLE_COUNT;i++)												//�ݴ��������
		{	
			TempChannelValue[0][i]=pProtectionCalculater->ProtectionSample.ChannelValue[0][i];
			TempChannelValue[1][i]=pProtectionCalculater->ProtectionSample.ChannelValue[1][i];
		}

		pProtectionCalculater->ProtectionActionType=ActiveType;
		pProtectionCalculater->ProtectionActionTime=ActiveTime;

		float ActionValueU,ActionValuedudt,ActionValueI,ActionValuedidt;	
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,ActionValuedudt,ActionValueU,TRUE);	//����U
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,ActionValuedidt,ActionValueI,TRUE);	//����I

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
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno2]=TempChannelValue[0][isampno];	//��բ��ĸ�ߵ�ѹ�ָ�
				pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno2]=0;								//���ߵ���
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno2]=0;								//���ߵ�ѹ
				isampno=isampno+1;
				if(isampno>31) isampno=0;
			}
		}

		if((g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[16].Value>0)&&(RecloserOnOff==TRUE))//�غ�բ��,Ӳѹ��Ͷ��
		{	
			if((ActiveType>=1)&&(ActiveType<=3))
			{
				if(ForeverShort==FALSE)																		//��·����޹���
				{
					if(g_RecloserDKXTimer[pProtectionCalculater->ProtectionID].StartupState==FALSE)
					{
						StartupTimer(&g_RecloserDKXTimer[pProtectionCalculater->ProtectionID]);				//�����غ�բ��ʱ��
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=1;			//���غ�բ������־
					}
					else g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=1;
					return TRUE;
				}
				else																						//��·����й���
				{
					ClearTimer(&g_RecloserDKXTimer[pProtectionCalculater->ProtectionID]);
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=0;
					return TRUE;
				}
			}
		}
		else return TRUE;
	}

	if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value>0)								//�غ�բ��ʱ�غ�
	{
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value==0) ActiveRecloser=TRUE;	//���߶�·���ѷ�բ
		else ActiveRecloser=FALSE;

		if(ActiveRecloser==FALSE)																			//�غ�����������
		{
			ClearTimer(&g_RecloserDKXTimer[pProtectionCalculater->ProtectionID]);
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=0;						//���غ�բ������־
		}
		else																								//�غ���������
		{
			if((::GetTickCount()-g_RecloserDKXTimer[pProtectionCalculater->ProtectionID].StartupTickCount)>=(DWORD)(RecloserTime))
			{
				ClearTimer(&g_RecloserDKXTimer[pProtectionCalculater->ProtectionID]);
				g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=0;					//���غ�բ������־
				g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=1;					//���غ�բ������־
				ActiveType=7;
				ActiveTime=(WORD)0;
				pProtectionCalculater->ProtectionActionType=ActiveType;
				pProtectionCalculater->ProtectionActionTime=ActiveTime;											
				g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value=1;						//���߶�·����բ
				g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[1].Value=0;						//���߶�·����բ
				pProtectionCalculater->ProtectionActionBreakerReclose=TRUE;									//١(���߶�·����բ)

				return TRUE;
			}
		}
	}

	if((ActiveCurrentQuickBreakKX||ActiveDDLKX||ActiveBilateralBreakKX)&&(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value==1))	// �ٶϺ�DDL��˫��������բ���غ�բ
	{
		g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=0;
		if(ForeverShort==TRUE)																				//�����Զ�·
		{
			if(ActiveCurrentQuickBreakKX==TRUE) ActiveType=1;
			else if(ActiveDDLKX==TRUE) ActiveType=2;
			else if(ActiveBilateralBreakKX==TRUE) ActiveType=6;
			pProtectionCalculater->ProtectionActionType=ActiveType;
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value=0;							//���߶�·����բ
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value=1;							//���߶�·����բ
			pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;										//١(���߶�·����բ)
			ActiveCurrentQuickBreakKX=FALSE;
			ActiveDDLKX=FALSE;
			ActiveBilateralBreakKX=FALSE;
		}
		return TRUE;
	}

	if((g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[16].Value==0)||(RecloserOnOff==FALSE))	//�غ�բ���Ӳѹ��δͶ��																			//�غ�բӲѹ���˳����غ�բ��ѹ���˳�
	{
		g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=0;							//���غ�բ������־
	}

	if(ActiveLossVoltage==FALSE) ClearTimer(&g_LossVoltageDKXTimer[pProtectionCalculater->ProtectionID]);	//����͵�ѹ��ʱ��
	if(ActiveOverLoad==FALSE) ClearTimer(&g_OverLoadDKXTimer[pProtectionCalculater->ProtectionID]);			//��������ɼ�ʱ��
	
	return TRUE;
}

BOOL WINAPI ProtectionCalculater3(TagProtectionCalculaterStruct *pProtectionCalculater)		//��ܱ���,����ͨ��˳��:Ul,Il
{
//��ȡ����ѹ��
	BOOL FrameLeakCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];			//���й©��������ѹ��
	BOOL FrameLeakVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];			//���й©��ѹ����ѹ��
//��ȡ��ֵ
	float FrameLeakCurrentValue=pProtectionCalculater->ProtectionParameter.Value[0];			//���й©������ֵ
	float FrameLeakCurrentTime=pProtectionCalculater->ProtectionParameter.Value[1];				//й©����ʱ�䶨ֵms
	float FrameLeakVoltageValue=pProtectionCalculater->ProtectionParameter.Value[2];			//���й©��ѹ��ֵ
	float FrameLeakVoltageTime=pProtectionCalculater->ProtectionParameter.Value[3];				//й©��ѹʱ�䶨ֵms

	BOOL ActiveFrameLeakCurrent=FALSE;			//���й©��������������־		1
	BOOL ActiveFrameLeakVoltage=FALSE;			//���й©��ѹ����������־		2

	int FrameLeakCurrentJS=0;

	WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;

	if((FrameLeakCurrentOnOff==TRUE)&&(ActiveType==0))			//���й©��������Ͷ��
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//���߶�·��������բ
				}
				FrameLeakCurrentJS+=1;
			}
		}
	}

	if((FrameLeakVoltageOnOff==TRUE)&&(ActiveType==0))		//���й©��ѹ����Ͷ��
	{
		float dudtValue,UValue;
		int sampno=WAVE_SAMPLE_COUNT-1;						//ȡ���һ���ܲ�

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,dudtValue,UValue,TRUE);
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)			//��·����բ
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;				//���������澯
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=1;
				}
			}
		}
	}

	if(ActiveType!=0)		//���㱣��������Ϣ
	{
		pProtectionCalculater->ProtectionActionType=ActiveType;
		pProtectionCalculater->ProtectionActionTime=ActiveTime;

		float ActionValueU,ActionValuedudt,ActionValueI,ActionValuedidt;	
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,ActionValuedudt,ActionValueU,TRUE);	//����U
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,ActionValuedidt,ActionValueI,TRUE);	//����I

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

	if(ActiveFrameLeakVoltage==FALSE) ClearTimer(&g_LeakVoltageKJTimer[pProtectionCalculater->ProtectionID]);	//���й©��ѹ��ʱ��

	return TRUE;
}
