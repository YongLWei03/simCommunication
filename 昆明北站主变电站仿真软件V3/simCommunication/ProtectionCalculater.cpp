//�ǹ�����������̵籣���������(AC110kV/AC35kV)
#include "stdafx.h"
#include "ProtectionCalculater.h"
#define	PI 3.141592654
#define	number_of_samples 32		//ÿ�ܲ���������,ÿ0.625ms��һ����

typedef struct TagTimerStruct		//��ʱ��
{
	BOOL  StartupState;				//����״̬
	DWORD StartupTickCount;			//��������
}TIMERSTRUCT;

static struct TagTimerStruct g_ZeroOverCurrentTimer[UNITCOUNT];		// ���������ʱ��(���ڱ�ѹ�������)
static struct TagTimerStruct g_OverLoadSection1Timer[UNITCOUNT];	// ������1�μ�ʱ��(���ڱ�ѹ���󱸱���)
static struct TagTimerStruct g_OverLoadSection2Timer[UNITCOUNT];	// ������2�μ�ʱ��(���ڱ�ѹ���󱸱���)
static struct TagTimerStruct g_LossVoltageTimer[UNITCOUNT];			// ʧѹ��ʱ��(���ڱ�ѹ���󱸱���)
static struct TagTimerStruct g_LossVoltageJXTimer[UNITCOUNT];		// ʧѹ��ʱ��(������ѹ���߱���)
static struct TagTimerStruct g_OverLoadJXTimer[UNITCOUNT];			// �����ɼ�ʱ��(������ѹ���߱���)
static struct TagTimerStruct g_ZeroOverCurrentJXTimer[UNITCOUNT];	// ���������ʱ��(������ѹ���߱���)
static struct TagTimerStruct g_LossVoltageXLTimer[UNITCOUNT];		// ʧѹ��ʱ��(������ѹ���߱���)
static struct TagTimerStruct g_OverLoadXLTimer[UNITCOUNT];			// �����ɼ�ʱ��(������ѹ���߱���)

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

BOOL WINAPI ProtectionCalculater(TagProtectionCalculaterStruct *pProtectionCalculater)
{
	switch(pProtectionCalculater->ProtectionType)
	{
		case 1:														//��ѹ�������
			ProtectionCalculater1(pProtectionCalculater);
			break;
		case 2:														//��ѹ���󱸱���
			ProtectionCalculater2(pProtectionCalculater);
			break;
		case 3:														//��ѹ���߱���
			ProtectionCalculater3(pProtectionCalculater);
			break;
		case 4:														//��ѹ���߱���
		case 6:														//���ñ䱣��
			ProtectionCalculater4(pProtectionCalculater);
			break;
		case 5:														//����������
			//ProtectionCalculater5(pProtectionCalculater);
			break;
	}
	
	return TRUE;
}

BOOL WINAPI ProtectionCalculater1(TagProtectionCalculaterStruct *pProtectionCalculater)		//��ѹ�������,����ͨ��˳��:IA,IB,IC,Ia,Ib,Ic,I0
{
//��ȡ����ѹ��
	BOOL DifferentialQuickBreakOnOff=BOOL(pProtectionCalculater->ProtectionSwitch.Value[0]);	//��ٶ�ѹ��
	BOOL DifferentialRatioOnOff=BOOL(pProtectionCalculater->ProtectionSwitch.Value[1]);			//�����ѹ��
	BOOL ZeroOverCurrentOnOff=BOOL(pProtectionCalculater->ProtectionSwitch.Value[2]);			//�����������ѹ��
//��ȡ��ֵ
	float CTHRatio=pProtectionCalculater->ProtectionParameter.Value[0];							//��ѹ��CT���
	float CTLRatio=pProtectionCalculater->ProtectionParameter.Value[1];							//��ѹ��CT���
	float CTZRatio=pProtectionCalculater->ProtectionParameter.Value[2];							//����CT���
	float ITBL=pProtectionCalculater->ProtectionParameter.Value[3];								//ͻ����������ֵ
	float ISDA=pProtectionCalculater->ProtectionParameter.Value[4];								//A�������ٶ϶�ֵ
	float ISDB=pProtectionCalculater->ProtectionParameter.Value[5];								//B�������ٶ϶�ֵ
	float ISDC=pProtectionCalculater->ProtectionParameter.Value[6];								//C�������ٶ϶�ֵ
	float IDZA=pProtectionCalculater->ProtectionParameter.Value[7];								//A��������ֵ
	float IDZB=pProtectionCalculater->ProtectionParameter.Value[8];								//B��������ֵ
	float IDZC=pProtectionCalculater->ProtectionParameter.Value[9];								//C��������ֵ
	float IZDA=pProtectionCalculater->ProtectionParameter.Value[10];							//A���ƶ�������ֵ
	float IZDB=pProtectionCalculater->ProtectionParameter.Value[11];							//B���ƶ�������ֵ
	float IZDC=pProtectionCalculater->ProtectionParameter.Value[12];							//C���ƶ�������ֵ
	float KZD=pProtectionCalculater->ProtectionParameter.Value[13];								//�����ƶ�ϵ��
	float IGJA=pProtectionCalculater->ProtectionParameter.Value[14];							//A������澯��ֵ
	float IGJB=pProtectionCalculater->ProtectionParameter.Value[15];							//B������澯��ֵ
	float IGJC=pProtectionCalculater->ProtectionParameter.Value[16];							//C������澯��ֵ
	float KYL=pProtectionCalculater->ProtectionParameter.Value[17];								//����г���ƶ�ϵ��
	float KPH=pProtectionCalculater->ProtectionParameter.Value[18];								//ƽ��ϵ��
	float ILX=pProtectionCalculater->ProtectionParameter.Value[19];								//�������������ֵ
	float TLX=pProtectionCalculater->ProtectionParameter.Value[20]*1000;						//�������ʱ�䶨ֵ

	BOOL ActiveDifferentialQuickBreakA=FALSE;													//A������ٶ϶�����־
	BOOL ActiveDifferentialQuickBreakB=FALSE;													//B������ٶ϶�����־
	BOOL ActiveDifferentialQuickBreakC=FALSE;													//C������ٶ϶�����־
	BOOL ActiveDifferentialRatioA=FALSE;														//A����ʲ������־
	BOOL ActiveDifferentialRatioB=FALSE;														//B����ʲ������־
	BOOL ActiveDifferentialRatioC=FALSE;														//C����ʲ������־
	BOOL ActiveDifferentialWarningA=FALSE;														//A������澯������־
	BOOL ActiveDifferentialWarningB=FALSE;														//B������澯������־
	BOOL ActiveDifferentialWarningC=FALSE;														//C������澯������־
	BOOL ActiveZeroOverCurrent=FALSE;															//�����������������־

	int ZeroOverCurrentJS=0;																	//�������������

	float sampletbl[3]={0.0,0.0,0.0};															//����ͻ������������
	int dltb[3]={0,0,0};																		//����ͻ���������

	WORD ActiveType=0;
	WORD ActionPosition=0;

	if((DifferentialQuickBreakOnOff==TRUE)||(DifferentialRatioOnOff==TRUE))						//����ΪYNd11���߱�ѹ��
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;

			float IAValue,IBValue,ICValue,IaValue,IbValue,IcValue,IAValuejd,IBValuejd,ICValuejd,IaValuejd,IbValuejd,IcValuejd;
			float I2AValue,I2BValue,I2CValue,I2aValue,I2bValue,I2cValue;
			float IAaValue,IAaValuejd,IBbValue,IBbValuejd,ICcValue,ICcValuejd;					//��ѹ�����ת������ѹ��ĸ������
			float DIAValue,BIAValue,DIBValue,BIBValue,DICValue,BICValue;						//�����D,�ƶ�����B
							
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,IAValue,IAValuejd);	//����IA
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,IBValue,IBValuejd);	//����IB
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,ICValue,ICValuejd);	//����IC
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue,IaValuejd);	//����Ia
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue,IbValuejd);	//����Ib
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue,IcValuejd);	//����Ic
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,2,I2AValue);			//����I2A
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,2,I2BValue);			//����I2B
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,2,I2CValue);			//����I2C
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,2,I2aValue);			//����I2a
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,2,I2bValue);			//����I2b
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,2,I2cValue);			//����I2c
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

			for(int j=0;j<3;j++)					//���A��,B��,C�����ͻ����
			{
				if(dltb[j]<4)
				{
					sampletbl[j]=float((fabs(pProtectionCalculater->ProtectionSample.ChannelValue[j][sampno])-fabs(pProtectionCalculater->ProtectionSample.ChannelValue[j][sampno-number_of_samples]))); //����˲ʱֵ��ֵ 
					if((sampletbl[j]/CTHRatio)>ITBL) ++dltb[j];
					else dltb[j]=0;
				}
			}
							
			if(dltb[0]>=4)																		//A�����ͻ�������������
			{
				IAaValue=IaValue/KPH;															//��a������任����ѹ��
				IAaValuejd=float(IaValuejd-PI); 												//��a�������λȡ��
				DIAValue=float(sqrt(pow((IAValue*cos(IAValuejd)+IAaValue*cos(IAaValuejd)),2)+pow((IAValue*sin(IAValuejd)+IAaValue*sin(IAaValuejd)),2)));	//����A������,��������λ�෴,���Ϊ�  
				BIAValue=float(sqrt(pow((IAValue*cos(IAValuejd)-IAaValue*cos(IAaValuejd)),2)+pow((IAValue*sin(IAValuejd)-IAaValue*sin(IAaValuejd)),2))/2);	//����A���ƶ�����,���������,��ֵ�ٳ�2
				if((DifferentialQuickBreakOnOff==TRUE)&&(ActiveType==0))						//��ٶ�Ͷ��
				{
					if(DIAValue>ISDA)
					{
						ActiveDifferentialQuickBreakA=TRUE;
						ActiveType=1;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=ActionPosition;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//��ѹ���·��������բ
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;				//��ѹ���·��������բ
					}
				}
				if((DifferentialRatioOnOff==TRUE)&&(ActiveType==0))								//�����Ͷ��
				{
					if(((I2AValue/IAValue)<KYL)&&((I2BValue/IBValue)<KYL)&&((I2CValue/ICValue)<KYL))
					{
						if((BIAValue<=IZDA)&&(DIAValue>IDZA)) ActiveDifferentialRatioA=TRUE;	//��Izd<Izds��Icd>Icds �����
						else if((BIAValue>IZDA)&&(DIAValue>(KZD*(BIAValue-IZDA)+IDZA))) ActiveDifferentialRatioA=TRUE;	//��Izd>=Izds��Icd>(k*(Izd-Izds)+Icds) �����
						else ActiveDifferentialRatioA=FALSE;
						if(ActiveDifferentialRatioA==TRUE) 
						{
							ActiveType=2;
							ActionPosition=sampno;
							pProtectionCalculater->ProtectionActionType=ActiveType;
							pProtectionCalculater->ProtectionActionTime=ActionPosition;
							pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;			//��ѹ���·��������բ
							pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;			//��ѹ���·��������բ
						}
					}
				}
				if((ActiveType==0)&&(sampno>120))
				{
					if(DIAValue>IGJA)															//�����澯
					{
						ActiveDifferentialWarningA=TRUE;
						ActiveType=22;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=100;						//?
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;			//���������澯
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=TRUE;
					}
				}
			}

			if(dltb[1]>=4)																		//B�����ͻ�������������
			{
				IBbValue=IbValue/KPH;															//��b������任����ѹ��
				IBbValuejd=float(IbValuejd-PI); 												//��b�������λȡ��
				DIBValue=float(sqrt(pow((IBValue*cos(IBValuejd)+IBbValue*cos(IBbValuejd)),2)+pow((IBValue*sin(IBValuejd)+IBbValue*sin(IBbValuejd)),2)));	//����B������,��������λ�෴,���Ϊ�  
				BIBValue=float(sqrt(pow((IBValue*cos(IBValuejd)-IBbValue*cos(IBbValuejd)),2)+pow((IBValue*sin(IBValuejd)-IBbValue*sin(IBbValuejd)),2))/2);	//����B���ƶ�����,���������,��ֵ�ٳ�2
				if((DifferentialQuickBreakOnOff==TRUE)&&(ActiveType==0))						//��ٶ�Ͷ��
				{
					if(DIBValue>ISDB)
					{
						ActiveDifferentialQuickBreakB=TRUE;
						ActiveType=1;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=ActionPosition;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//��ѹ���·��������բ
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;				//��ѹ���·��������բ
					}
				}
				if((DifferentialRatioOnOff==TRUE)&&(ActiveType==0))								//�����Ͷ��
				{
					if(((I2AValue/IAValue)<KYL)&&((I2BValue/IBValue)<KYL)&&((I2CValue/ICValue)<KYL))
					{
						if((BIBValue<=IZDB)&&(DIBValue>IDZB)) ActiveDifferentialRatioB=TRUE;	//��Izd<Izds��Icd>Icds �����
						else if((BIBValue>IZDB)&&(DIBValue>(KZD*(BIBValue-IZDB)+IDZB))) ActiveDifferentialRatioB=TRUE;	//��Izd>=Izd2s��Icd>k2*(Izd-Izd2s)+k1*(Izd2s-Izd1s)+Icds �����
						else ActiveDifferentialRatioB=FALSE;
						if(ActiveDifferentialRatioB==TRUE) 
						{
							ActiveType=2;
							ActionPosition=sampno;
							pProtectionCalculater->ProtectionActionType=ActiveType;
							pProtectionCalculater->ProtectionActionTime=ActionPosition;
							pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;			//��ѹ���·��������բ
							pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;			//��ѹ���·��������բ
						}
					}
				}
				if((ActiveType==0)&&(sampno>120))
				{
					if(DIBValue>IGJB)															//�����澯
					{
						ActiveDifferentialWarningB=TRUE;
						ActiveType=23;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=100;						//?
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;			//���������澯
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[23].Value=TRUE;
					}
				}
			}

			if(dltb[2]>=4)																		//C�����ͻ�������������
			{
				ICcValue=IcValue/KPH;
				ICcValuejd=float(IcValuejd-PI);
				DICValue=float(sqrt(pow((ICValue*cos(ICValuejd)+ICcValue*cos(ICcValuejd)),2)+pow((ICValue*sin(ICValuejd)+ICcValue*sin(ICcValuejd)),2)));
				BICValue=float(sqrt(pow((ICValue*cos(ICValuejd)-ICcValue*cos(ICcValuejd)),2)+pow((ICValue*sin(ICValuejd)-ICcValue*sin(ICcValuejd)),2))/2);	
				if((DifferentialQuickBreakOnOff==TRUE)&&(ActiveType==0))						//��ٶ�Ͷ��
				{
					if(DICValue>ISDC)
					{
						ActiveDifferentialQuickBreakC=TRUE;
						ActiveType=1;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=ActionPosition;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;				//��ѹ���·��������բ
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;				//��ѹ���·��������բ
					}
				}	
				if((DifferentialRatioOnOff==TRUE)&&(ActiveType==0))								//�����Ͷ��
				{
					if(((I2AValue/IAValue)<KYL)&&((I2BValue/IBValue)<KYL)&&((I2CValue/ICValue)<KYL))
					{
						if((BICValue<=IZDC)&&(DICValue>IDZC)) ActiveDifferentialRatioC=TRUE;	//��Izd<Izds��Icd>Icds �����
						else if((BICValue>IZDC)&&(DICValue>(KZD*(BICValue-IZDC)+IDZC))) ActiveDifferentialRatioC=TRUE;	//��Izd>=Izd2s��Icd>k2*(Izd-Izd2s)+k1*(Izd2s-Izd1s)+Icds �����
						else ActiveDifferentialRatioC=FALSE;
						if(ActiveDifferentialRatioC==TRUE) 
						{
							ActiveType=2;
							ActionPosition=sampno;
							pProtectionCalculater->ProtectionActionType=ActiveType;
							pProtectionCalculater->ProtectionActionTime=ActionPosition;
							pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;			//��ѹ���·��������բ
							pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;			//��ѹ���·��������բ
						}
					}
				}
				if((ActiveType==0)&&(sampno>120))
				{
					if(DICValue>IGJC)															//�����澯
					{
						ActiveDifferentialWarningC=TRUE;
						ActiveType=24;
						ActionPosition=sampno;
						pProtectionCalculater->ProtectionActionType=ActiveType;
						pProtectionCalculater->ProtectionActionTime=100;						//?
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;			//���������澯
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[24].Value=TRUE;
					}
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;					//��ѹ���·��������բ
					pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;					//��ѹ���·��������բ
				}
				ZeroOverCurrentJS+=1;
			}
		}
	}

/*	if((ZeroOverCurrentOnOff==TRUE)&&(ActiveType==0))		//���򱣻���ѹ��Ͷ��
	{
		float I0Value;
		int sampno=WAVE_SAMPLE_COUNT;					//ȡ���һ���ܲ�
				
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,I0Value);//����I0
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;					//��ѹ���·��������բ
					pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;					//��ѹ���·��������բ
				}
			}
		}
	}*/

	if(ActiveType!=0)			//���㱣������ʱ�����в���
	{
		float ActionValueIA,ActionValueIB,ActionValueIC,ActionValueIa,ActionValueIb,ActionValueIc,ActionValueI0;
		float ActionValueIAjd,ActionValueIBjd,ActionValueICjd,ActionValueIajd,ActionValueIbjd,ActionValueIcjd;
		float ActionValueI2A,ActionValueI2B,ActionValueI2C;
		float ActionValueIAa,ActionValueIAajd,ActionValueDIA,ActionValueBIA;
		float ActionValueIBb,ActionValueIBbjd,ActionValueDIB,ActionValueBIB;
		float ActionValueICc,ActionValueICcjd,ActionValueDIC,ActionValueBIC;

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,ActionValueIA,ActionValueIAjd);	//����IA
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,ActionValueIB,ActionValueIBjd);	//����IB
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],ActionPosition,ActionValueIC,ActionValueICjd);	//����IC
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],ActionPosition,ActionValueIa,ActionValueIajd);	//����Ia
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],ActionPosition,ActionValueIb,ActionValueIbjd);	//����Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],ActionPosition,ActionValueIc,ActionValueIcjd);	//����Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],ActionPosition,2,ActionValueI2A);				//����IA2
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],ActionPosition,2,ActionValueI2B);				//����IB2
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],ActionPosition,2,ActionValueI2C);				//����IC2
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],ActionPosition,ActionValueI0);					//����I0
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
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)			//����˹ң��
		{
			if((g_Unit[pProtectionCalculater->ProtectionID+1].VarGroup[1].Var[0].Value!=0)||(g_Unit[pProtectionCalculater->ProtectionID+2].VarGroup[1].Var[0].Value!=0))
			{
				ActiveType=3;
				pProtectionCalculater->ProtectionActionType=ActiveType;
				pProtectionCalculater->ProtectionActionTime=0;
				pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;					//��ѹ���·��������բ
				pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;					//��ѹ���·��������բ
			}
		}

		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[1].Value>0)			//����˹ң��
		{
			ActiveType=18;
			pProtectionCalculater->ProtectionActionType=ActiveType;
			pProtectionCalculater->ProtectionActionTime=100;								//?
			pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//���������澯
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=TRUE;
		}

		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[2].Value>0)			//�¶�ң��
		{
			ActiveType=19;
			pProtectionCalculater->ProtectionActionType=ActiveType;
			pProtectionCalculater->ProtectionActionTime=100;								//?
			pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//���������澯
			g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=TRUE;
		}

		return TRUE;
	}

	if(ActiveZeroOverCurrent==FALSE) ClearTimer(&g_ZeroOverCurrentTimer[pProtectionCalculater->ProtectionID]);

	return TRUE;	
}

BOOL WINAPI ProtectionCalculater2(TagProtectionCalculaterStruct *pProtectionCalculater)			//��ѹ���󱸱���,����ͨ��˳��:UA,UB,UC,Ua,Ub,Uc,IA,IB,IC
{
//��ȡ����ѹ��
	BOOL OverCurrentSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];		//����1��ѹ��
	BOOL OverCurrentSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];		//����2��ѹ��
	BOOL LossVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[2];				//ʧѹѹ��
	BOOL OverLoadOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[3];					//������ѹ��
	BOOL OverLoadSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[4];			//������1��Ͷ��
	BOOL OverLoadSection1CKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[5];		//0:������1�θ澯,1:������1����բ
	BOOL OverLoadSection1SXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[6];		//0:������1�ζ�ʱ��,1:������1�α�׼��ʱ��
	BOOL OverLoadSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[7];			//������2��Ͷ��
	BOOL OverLoadSection2CKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[8];		//0:������2�θ澯,1:������2����բ 
	BOOL OverLoadSection2SXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[9];		//0:������2�ζ�ʱ��,1:������2�α�׼��ʱ��
	BOOL LowVoltageSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[10];		//��ѹ��������1��ѹ��
	BOOL LowVoltageSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[11];		//��ѹ��������2��ѹ��
	BOOL PTUnusualSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[12];		//0:PT�쳣�˳�����1��,1:PT�쳣�˳�1�ε�ѹ����
	BOOL PTUnusualSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[13];		//0:PT�쳣�˳�����2��,1:PT�쳣�˳�2�ε�ѹ����
	BOOL PTUnusualOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[30];				//PT�쳣���
//��ȡ��ֵ��ֵ
	float PTHValue=pProtectionCalculater->ProtectionParameter.Value[0]*(float)0.001;			//��ѹ��PT���
	float PTLValue=pProtectionCalculater->ProtectionParameter.Value[1]*(float)0.001;			//��ѹ��PT���
	float CTHValue=pProtectionCalculater->ProtectionParameter.Value[2];							//��ѹ��CT���		
	float OverCurrentSection1AValue=pProtectionCalculater->ProtectionParameter.Value[3];		//����1��A�ඨֵ
	float OverCurrentSection1BValue=pProtectionCalculater->ProtectionParameter.Value[4];		//����1��B�ඨֵ
	float OverCurrentSection1CValue=pProtectionCalculater->ProtectionParameter.Value[5];		//����1��C�ඨֵ
	float OverCurrentSection1Time=pProtectionCalculater->ProtectionParameter.Value[6]*1000;		//����1��ʱ�䶨ֵ
	float OverCurrentSection2AValue=pProtectionCalculater->ProtectionParameter.Value[7];		//����2��A�ඨֵ
	float OverCurrentSection2BValue=pProtectionCalculater->ProtectionParameter.Value[8];		//����2��B�ඨֵ
	float OverCurrentSection2CValue=pProtectionCalculater->ProtectionParameter.Value[9];		//����2��C�ඨֵ
	float OverCurrentSection2Time=pProtectionCalculater->ProtectionParameter.Value[10]*1000;	//����2��ʱ�䶨ֵ
	float LowVoltageSectionValue=pProtectionCalculater->ProtectionParameter.Value[11];			//��ѹ����������ѹ��ֵ	
	float LossVoltageValue=pProtectionCalculater->ProtectionParameter.Value[12];				//ʧѹ������ֵ
	float LossVoltageTime=pProtectionCalculater->ProtectionParameter.Value[13]*1000;			//ʧѹʱ�䶨ֵ
	float OverLoadSection1AValue=pProtectionCalculater->ProtectionParameter.Value[14];			//A�������1�α�����ֵ
	float OverLoadSection1BValue=pProtectionCalculater->ProtectionParameter.Value[15];			//B�������1�α�����ֵ
	float OverLoadSection1CValue=pProtectionCalculater->ProtectionParameter.Value[16];			//C�������1�α�����ֵ
	float OverLoadSection1Time=pProtectionCalculater->ProtectionParameter.Value[17]*1000;		//������1��ʱ�䶨ֵ
	float OverLoadSection2AValue=pProtectionCalculater->ProtectionParameter.Value[18];			//A�������2�α�����ֵ
	float OverLoadSection2BValue=pProtectionCalculater->ProtectionParameter.Value[19];			//B�������2�α�����ֵ
	float OverLoadSection2CValue=pProtectionCalculater->ProtectionParameter.Value[20];			//C�������2�α�����ֵ
	float OverLoadSection2Time=pProtectionCalculater->ProtectionParameter.Value[21]*1000;		//������2��ʱ�䶨ֵ

	BOOL ActiveOverCurrentSection1=FALSE;		//����1�ζ�����־		1
	BOOL ActiveOverCurrentSection2=FALSE;		//����2�ζ�����־		2
	BOOL ActiveLossVoltage=FALSE;				//ʧѹ������־			3
	BOOL ActiveOverLoadSection1=FALSE;			//������1�ζ�����־		4
	BOOL ActiveOverLoadSection2=FALSE;			//������2�ζ�����־		5
	BOOL ActivePTH=FALSE;						//��ѹ��PT�쳣			18
	BOOL ActivePTL=FALSE;						//��ѹ��PT�쳣			19
				
	int OverCurrentSection1JS=0;
	int OverCurrentSection2JS=0;

	WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;
				
	if(PTUnusualOnOff==TRUE)					//PT�쳣Ͷ��
	{
		for(int sampno=32;sampno<60;sampno+=1)
		{
			float UAValue,UBValue,UCValue,UaValue,UbValue,UcValue;
			float UAValuejd,UBValuejd,UCValuejd,UaValuejd,UbValuejd,UcValuejd;
			float UABValue,UBCValue,UCAValue,UabValue,UbcValue,UcaValue;
							
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UAValue,UAValuejd);		//����UA
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UBValue,UBValuejd);		//����UB
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UCValue,UCValuejd);		//����UC
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,UaValue,UaValuejd);		//����Ua
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,UbValue,UbValuejd);		//����Ub
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,UcValue,UcValuejd);		//����Uc
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

			if((g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)&&(g_Unit[pProtectionCalculater->ProtectionID+1].VarGroup[1].Var[0].Value>0))	//�ߵ�ѹ��·����բ
			{
				if((min(min(UABValue,UBCValue),UCAValue)<70)&&(UabValue>70)&&(UbcValue>70)&&(UcaValue>70))
				{
                	ActivePTH=TRUE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=TRUE;		//��ID�豸��ң��18�ŵ㷢"��ѹ��PT�쳣"
				}
				else
				{	
					ActivePTH=FALSE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=FALSE;	//��ID�豸��ң��18�ŵ㷢"��ѹ��PT����"
				}

				if((min(min(UabValue,UbcValue),UcaValue)<70)&&(UABValue>70)&&(UBCValue>70)&&(UCAValue>70))
				{
					ActivePTL=TRUE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=TRUE;		//��ID�豸��ң��19�ŵ㷢"��ѹ��PT�쳣"
				}
				else
				{
					ActivePTL=FALSE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[19].Value=FALSE;	//��ID�豸��ң��19�ŵ㷢"��ѹ��PT����"
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

	if((OverCurrentSection1OnOff==TRUE)||(OverCurrentSection2OnOff==TRUE))		//����1�λ����2��ѹ��Ͷ��
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;
			
			float UaValue,UbValue,UcValue,IAValue,IBValue,ICValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,UaValue);	//����Ua
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,UbValue);	//����Ub
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,UcValue);	//����Uc
			UaValue/=PTLValue;
			UbValue/=PTLValue;
			UcValue/=PTLValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,IAValue);	//����IA
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[7],sampno,IBValue);	//����IB
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[8],sampno,ICValue);	//����IC
			IAValue/=CTHValue;
			IBValue/=CTHValue;
			ICValue/=CTHValue;

			if((OverCurrentSection1OnOff==TRUE)&&(ActiveType==0))										//����1��ѹ��Ͷ��											
			{
				if(LowVoltageSection1OnOff==TRUE)														//��ѹ��������1��Ͷ��
				{
					if((ActivePTH||ActivePTL)==TRUE)													//PT�쳣
					{
						if(PTUnusualSection1OnOff==TRUE)												//�˳�����1�ε�ѹ����
						{
							if((IAValue>OverCurrentSection1AValue)||(IBValue>OverCurrentSection1BValue)||(ICValue>OverCurrentSection1CValue)) ActiveOverCurrentSection1=TRUE;
							else ActiveOverCurrentSection1=FALSE;
						}
						else ActiveOverCurrentSection1=FALSE;											//�˳�����1��
					}
					else																				//PT����
					{
						if((UaValue<LowVoltageSectionValue)||(UbValue<LowVoltageSectionValue)||(UcValue<LowVoltageSectionValue))
						{
							if((IAValue>OverCurrentSection1AValue)||(IBValue>OverCurrentSection1BValue)||(ICValue>OverCurrentSection1CValue)) ActiveOverCurrentSection1=TRUE;
							else ActiveOverCurrentSection1=FALSE;
						}
						else ActiveOverCurrentSection1=FALSE;
					}
				}
				else																					//��ѹ��������1��δͶ
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
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��ѹ���·��������բ
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//��ѹ���·��������բ
					}
					OverCurrentSection1JS+=1;
				}
			}

			if((OverCurrentSection2OnOff==TRUE)&&(ActiveType==0))										//����2��ѹ��Ͷ��
			{
				if(LowVoltageSection2OnOff==TRUE)														//��ѹ��������2��Ͷ��
				{
					if((ActivePTH||ActivePTL)==TRUE)													//PT�쳣
					{
						if(PTUnusualSection2OnOff==TRUE)												//�˳�����2�ε�ѹ����
						{
							if((IAValue>OverCurrentSection2AValue)||(IBValue>OverCurrentSection2BValue)||(ICValue>OverCurrentSection2CValue)) ActiveOverCurrentSection2=TRUE;
							else ActiveOverCurrentSection2=FALSE;
						}
						else ActiveOverCurrentSection2=FALSE;											//�˳�����2��
					}
					else																				//PT����
					{
						if((UaValue<LowVoltageSectionValue)||(UbValue<LowVoltageSectionValue)||(UcValue<LowVoltageSectionValue))
						{
							if((IAValue>OverCurrentSection2AValue)||(IBValue>OverCurrentSection2BValue)||(ICValue>OverCurrentSection2CValue)) ActiveOverCurrentSection2=TRUE;
							else ActiveOverCurrentSection2=FALSE;
						}
						else ActiveOverCurrentSection2=FALSE;
					}
				}
				else																					//��ѹ��������2��δͶ
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
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��ѹ���·��������բ
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//��ѹ���·��������բ
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
				int isampno=0;																//��բ���ѹ�������ѹ�ָ�
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

	if((LossVoltageOnOff==TRUE)&&(ActiveType==0))		//ʧѹ����Ͷ��
	{
		float UAValue,UBValue,UCValue,UaValue,UbValue,UcValue;
		int sampno=WAVE_SAMPLE_COUNT;					//ȡ���һ���ܲ�

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UAValue);	//����UA
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UBValue);	//����UB
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UCValue);	//����UC
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,UaValue);	//����Ua
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,UbValue);	//����Ub
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,UcValue);	//����Uc
		UAValue/=PTHValue;
		UBValue/=PTHValue;
		UCValue/=PTHValue;
		UaValue/=PTLValue;
		UbValue/=PTLValue;
		UcValue/=PTLValue;
		
		if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)					//��ѹ��·����բ(ID�豸��ң��0�ŵ�)
		{
			if((max(max(UAValue,UBValue),UCValue)<LossVoltageValue)&&(UaValue<LossVoltageValue)&&(UbValue<LossVoltageValue)&&(UcValue<LossVoltageValue)) ActiveLossVoltage=TRUE;	//��ѹ������ߵ�ѹ��ѹС��ʧѹ��ֵ	
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��ѹ���·��������բ
					pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//��ѹ���·��������բ
				}
			}
		}
	}

	if((OverLoadOnOff==TRUE)&&(OverLoadSection1OnOff==TRUE)&&(ActiveType==0))	//������1��Ͷ��
	{
		float IAValue,IBValue,ICValue;
		int sampno=WAVE_SAMPLE_COUNT;											//ȡ���һ���ܲ�

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,IAValue);		//����IA
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[7],sampno,IBValue);		//����IB
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[8],sampno,ICValue);		//����IC
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
			if(OverLoadSection1SXOnOff==TRUE)															//1:������1�α�׼��ʱ��,0:������1�ζ�ʱ��
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
					if(OverLoadSection1CKOnOff==TRUE)													//0:������1�θ澯,1:������1����բ 
					{
						ActiveType=4;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��ѹ���·��������բ
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//��ѹ���·��������բ
					}
					else
					{
						ActiveType=22;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//���������澯
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=TRUE;
					}
				}
			}
		}
	}

	if((OverLoadOnOff==TRUE)&&(OverLoadSection2OnOff==TRUE)&&(ActiveType==0))	//������2��ѹ��Ͷ��
	{
		float IAValue,IBValue,ICValue;
		int sampno=WAVE_SAMPLE_COUNT;											//ȡ���һ���ܲ�

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,IAValue);	//����IA
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[7],sampno,IBValue);	//����IB
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[8],sampno,ICValue);	//����IC
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
			if(OverLoadSection2SXOnOff==TRUE)															//1:������2�α�׼��ʱ��,0:������2�ζ�ʱ��
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
					if(OverLoadSection2CKOnOff==TRUE)													//0:������2�θ澯,1:������2����բ 
					{	
						ActiveType=5;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��ѹ���·��������բ
						pProtectionCalculater->ProtectionActionBreakerTrip[1]=TRUE;						//��ѹ���·��������բ
					}
					else
					{
						ActiveType=23;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//���������澯
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
			int isampno=0;																//��բ���ѹ�������ѹ�ָ�
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

	if(ActiveLossVoltage==FALSE) ClearTimer(&g_LossVoltageTimer[pProtectionCalculater->ProtectionID]);			//���ʧѹ��ʱ��
	if(ActiveOverLoadSection1==FALSE) ClearTimer(&g_OverLoadSection1Timer[pProtectionCalculater->ProtectionID]);//���������1�μ�ʱ��
	if(ActiveOverLoadSection2==FALSE) ClearTimer(&g_OverLoadSection2Timer[pProtectionCalculater->ProtectionID]);//���������2�μ�ʱ��

	return TRUE;
}

BOOL WINAPI ProtectionCalculater3(TagProtectionCalculaterStruct *pProtectionCalculater)		//��ѹ���߱���,����ͨ��˳��:Ua,Ub,Uc,Ia,Ib,Ic,I0
{
//��ȡ����ѹ��
	BOOL OverCurrentSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[0];		//����1�α���ѹ��
	BOOL OverCurrentSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[1];		//����2�α���ѹ��
	BOOL ZeroOverCurrentOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[2];			//�����������ѹ��
	BOOL LossVoltageOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[3];				//ʧѹ����ѹ��
	BOOL OverLoadOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[4];					//�����ɱ���ѹ��
	BOOL OverLoadCKOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[5];				//0:�����ɸ澯,1:��������բ
	BOOL OverLoadSXOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[6];				//0:�����ɶ�ʱ��,1:�����ɱ�׼��ʱ��
	BOOL LowVoltageSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[7];		//��ѹ��������1��ѹ��
	BOOL LowVoltageSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[8];		//��ѹ��������2��ѹ��
	BOOL PTUnusualSection1OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[9];			//0:PT�쳣�˳�����1��,1:PT�쳣�˳�1�ε�ѹ����
	BOOL PTUnusualSection2OnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[10];		//0:PT�쳣�˳�����2��,1:PT�쳣�˳�2�ε�ѹ����
	BOOL PTUnusualOnOff=(BOOL)pProtectionCalculater->ProtectionSwitch.Value[30];				//PT�쳣���
//��ȡ��ֵ
	float PTLValue=pProtectionCalculater->ProtectionParameter.Value[0]*(float)0.001;			//��ѹ��PT���
	float CTLValue=pProtectionCalculater->ProtectionParameter.Value[1];							//��ѹ��CT���
	float CTZValue=pProtectionCalculater->ProtectionParameter.Value[2];							//����CT���
	float OverCurrentSection1aValue=pProtectionCalculater->ProtectionParameter.Value[3];		//����1��a�ඨֵ
	float OverCurrentSection1bValue=pProtectionCalculater->ProtectionParameter.Value[4];		//����1��b�ඨֵ
	float OverCurrentSection1cValue=pProtectionCalculater->ProtectionParameter.Value[5];		//����1��c�ඨֵ
	float OverCurrentSection1Time=pProtectionCalculater->ProtectionParameter.Value[6]*1000;		//����1��ʱ�䶨ֵ
	float OverCurrentSection2aValue=pProtectionCalculater->ProtectionParameter.Value[7];		//����2��a�ඨֵ
	float OverCurrentSection2bValue=pProtectionCalculater->ProtectionParameter.Value[8];		//����2��b�ඨֵ
	float OverCurrentSection2cValue=pProtectionCalculater->ProtectionParameter.Value[9];		//����2��c�ඨֵ
	float OverCurrentSection2Time=pProtectionCalculater->ProtectionParameter.Value[10]*1000;	//����2��ʱ�䶨ֵ
	float LowVoltageSectionValue=pProtectionCalculater->ProtectionParameter.Value[11];			//��ѹ����������ѹ��ֵ	
	float LossVoltageValue=pProtectionCalculater->ProtectionParameter.Value[12];				//ʧѹ������ֵ
	float LossVoltageTime=pProtectionCalculater->ProtectionParameter.Value[13]*1000;			//ʧѹʱ�䶨ֵ
	float OverLoadaValue=pProtectionCalculater->ProtectionParameter.Value[14];					//a������ɱ�����ֵ
	float OverLoadbValue=pProtectionCalculater->ProtectionParameter.Value[15];					//b������ɱ�����ֵ
	float OverLoadcValue=pProtectionCalculater->ProtectionParameter.Value[16];					//c������ɱ�����ֵ
	float OverLoadTime=pProtectionCalculater->ProtectionParameter.Value[17]*1000;				//������ʱ�䶨ֵ
	float ZeroOverCurrentValue=pProtectionCalculater->ProtectionParameter.Value[18];			//�������������ֵ
	float ZeroOverCurrentTime=pProtectionCalculater->ProtectionParameter.Value[19]*1000;		//�������ʱ�䶨ֵ

	BOOL ActiveOverCurrentSection1=FALSE;		//������1�ζ�����־		1
	BOOL ActiveOverCurrentSection2=FALSE;		//������2�ζ�����־		2
	BOOL ActiveZeroOverCurrent=FALSE;			//�������������־		3
	BOOL ActiveLossVoltage=FALSE;				//ʧѹ������־			4
	BOOL ActiveOverLoad=FALSE;					//�����ɶ�����־		5
	BOOL ActivePT=FALSE;						//PT�쳣
	BOOL ZeroSequenceMeasure=FALSE;
				
	int OverCurrentSection1JS=0;
	int OverCurrentSection2JS=0;
	int ZeroOverCurrentJS=0;
	
    WORD ActiveType=0;
	WORD ActiveTime=0;
	WORD ActionPosition=0;
				
	if(PTUnusualOnOff==TRUE)					//PT�쳣Ͷ��
	{
		for(int sampno=32;sampno<60;sampno+=1)
		{
			float UaValue,UbValue,UcValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UaValue);	//����Ua
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UbValue);	//����Ub
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UcValue);	//����Uc
			UaValue/=PTLValue;
			UbValue/=PTLValue;
			UcValue/=PTLValue;

			if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0)				//��ѹ���·����բ(ID�豸��ң��0�ŵ�)
			{
				if(min(min(UaValue,UbValue),UcValue)<70)
				{
					ActivePT=TRUE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=TRUE;		//��ID�豸��ң��18�ŵ㷢"��ѹ��PT�쳣"
				}
				else
				{
					ActivePT=FALSE;
					g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=FALSE;	//��ID�豸��ң��18�ŵ㷢"��ѹ��PT����"
				}
			}
		}
	}
	else
	{
		ActivePT=FALSE;
		g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[18].Value=FALSE;
	}

	if((OverCurrentSection1OnOff==TRUE)||(OverCurrentSection2OnOff==TRUE))		//����1�λ����2��ѹ��Ͷ��
	{
		for(int sampno=0;sampno<WAVE_SAMPLE_COUNT;sampno+=1)
		{
			if(sampno<32) continue;
			if(ActiveType!=0) break;
			
			float UaValue,UbValue,UcValue,IaValue,IbValue,IcValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UaValue);	//����Ua
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UbValue);	//����Ub
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UcValue);	//����Uc
			UaValue/=PTLValue;
			UbValue/=PTLValue;
			UcValue/=PTLValue;
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);	//����Ia
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);	//����Ib
			CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);	//����Ic
			IaValue/=CTLValue;
			IbValue/=CTLValue;
			IcValue/=CTLValue;

			if((OverCurrentSection1OnOff==TRUE)&&(ActiveType==0))										//����1��ѹ��Ͷ��											
			{
				if(LowVoltageSection1OnOff==TRUE)														//��ѹ��������1��Ͷ��
				{
					if(ActivePT==TRUE)																	//PT�쳣
					{
						if(PTUnusualSection1OnOff==TRUE)												//�˳�����1�ε�ѹ����
						{
							if((IaValue>OverCurrentSection1aValue)||(IbValue>OverCurrentSection1bValue)||(IcValue>OverCurrentSection1cValue)) ActiveOverCurrentSection1=TRUE;
							else ActiveOverCurrentSection1=FALSE;
						}
						else ActiveOverCurrentSection1=FALSE;											//�˳�����1��
					}
					else																				//PT����
					{
						if((UaValue<LowVoltageSectionValue)||(UbValue<LowVoltageSectionValue)||(UcValue<LowVoltageSectionValue))
						{
							if((IaValue>OverCurrentSection1aValue)||(IbValue>OverCurrentSection1bValue)||(IcValue>OverCurrentSection1cValue)) ActiveOverCurrentSection1=TRUE;
							else ActiveOverCurrentSection1=FALSE;
						}
						else ActiveOverCurrentSection1=FALSE;
					}
				}
				else																					//��ѹ��������1��δͶ
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
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��ѹ���·��������բ
					}
					OverCurrentSection1JS+=1;
				}
			}

			if((OverCurrentSection2OnOff==TRUE)&&(ActiveType==0))										//����2��ѹ��Ͷ��
			{
				if(LowVoltageSection2OnOff==TRUE)														//��ѹ��������2��Ͷ��
				{
					if(ActivePT==TRUE)																	//PT�쳣
					{
						if(PTUnusualSection2OnOff==TRUE)												//�˳�����2�ε�ѹ����
						{
							if((IaValue>OverCurrentSection2aValue)||(IbValue>OverCurrentSection2bValue)||(IcValue>OverCurrentSection2cValue)) ActiveOverCurrentSection2=TRUE;
							else ActiveOverCurrentSection2=FALSE;
						}
						else ActiveOverCurrentSection2=FALSE;											//�˳�����2��
					}
					else																				//PT����
					{
						if((UaValue<LowVoltageSectionValue)||(UbValue<LowVoltageSectionValue)||(UcValue<LowVoltageSectionValue))
						{
							if((IaValue>OverCurrentSection2aValue)||(IbValue>OverCurrentSection2bValue)||(IcValue>OverCurrentSection2cValue)) ActiveOverCurrentSection2=TRUE;
							else ActiveOverCurrentSection2=FALSE;
						}
						else ActiveOverCurrentSection2=FALSE;
					}
				}
				else																					//��ѹ��������2��δͶ
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
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��ѹ���·��������բ
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
					ActiveType=3;
					ActiveTime=(WORD)ZeroOverCurrentTime;
					ActionPosition=sampno;
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;							//��ѹ���·��������բ
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
					pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno]=0;		//a���ѹ
					pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno]=0;		//b���ѹ
					pProtectionCalculater->ProtectionSample.ChannelValue[2][sampno]=0;		//c���ѹ
					pProtectionCalculater->ProtectionSample.ChannelValue[3][sampno]=0;		//a�����
					pProtectionCalculater->ProtectionSample.ChannelValue[4][sampno]=0;		//b�����
					pProtectionCalculater->ProtectionSample.ChannelValue[5][sampno]=0;		//c�����
			}
		}
			
		return TRUE;
	}

	if((LossVoltageOnOff==TRUE)&&(ActiveType==0))		//ʧѹѹ��Ͷ��
	{	
		float UaValue,UbValue,UcValue; 
		int sampno=WAVE_SAMPLE_COUNT;					//ȡ���һ���ܲ�
		   
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[0],sampno,UaValue);			//����Ua
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[1],sampno,UbValue);			//����Ub
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[2],sampno,UcValue);			//����Uc
		UaValue/=PTLValue;
		UbValue/=PTLValue;
		UcValue/=PTLValue;

		if((UaValue<LossVoltageValue)&&(UbValue<LossVoltageValue)&&(UcValue<LossVoltageValue))																		//a���ѹС��ʧѹ��ֵ
		{
			if(g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[0].Value>0) ActiveLossVoltage=TRUE;	//��ѹ���·����բ
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��ѹ���·��������բ
				}
			}
		}
	}

	if((OverLoadOnOff==TRUE)&&(ActiveType==0))		//������ѹ��Ͷ��
	{
		float IaValue,IbValue,IcValue;
		int sampno=WAVE_SAMPLE_COUNT;											//ȡ���һ���ܲ�

		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[3],sampno,IaValue);		//����Ia
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[4],sampno,IbValue);		//����Ib
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[5],sampno,IcValue);		//����Ic
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
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//��ѹ���·��������բ
			
					}
					else
					{
						ActiveType=22;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;					//���������澯
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=TRUE;
					}
				}
			}
		}
	}

/*	if((ZeroOverCurrentOnOff==TRUE)&&(ActiveType==0))		//���򱣻���ѹ��Ͷ��
	{
		float I0Value;
		int sampno=WAVE_SAMPLE_COUNT;						//ȡ���һ���ܲ�
				
		CalculaterValue(pProtectionCalculater->ProtectionSample.ChannelValue[6],sampno,I0Value);	//����I0
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
					pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;						//���߶�·��������բ
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
				pProtectionCalculater->ProtectionSample.ChannelValue[0][sampno]=0;		//a���ѹ
				pProtectionCalculater->ProtectionSample.ChannelValue[1][sampno]=0;		//b���ѹ
				pProtectionCalculater->ProtectionSample.ChannelValue[2][sampno]=0;		//c���ѹ
				pProtectionCalculater->ProtectionSample.ChannelValue[3][sampno]=0;		//a�����
				pProtectionCalculater->ProtectionSample.ChannelValue[4][sampno]=0;		//b�����
				pProtectionCalculater->ProtectionSample.ChannelValue[5][sampno]=0;		//c�����
			}
		}

		return TRUE;
	}
										
	if(ActiveLossVoltage==FALSE) ClearTimer(&g_LossVoltageJXTimer[pProtectionCalculater->ProtectionID]);
	if(ActiveOverLoad==FALSE) ClearTimer(&g_OverLoadJXTimer[pProtectionCalculater->ProtectionID]);
	if(ActiveZeroOverCurrent==FALSE) ClearTimer(&g_ZeroOverCurrentJXTimer[pProtectionCalculater->ProtectionID]);

	return TRUE;
}

BOOL WINAPI ProtectionCalculater4(TagProtectionCalculaterStruct *pProtectionCalculater)		//��ѹ���߱���,����ͨ��˳��:Ua,Ub,Uc,Ia,Ib,Ic,I0
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
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=TRUE;					//��·��·��������բ
					}
					else
					{
						ActiveType=22;
						ActiveTime=(WORD)::GetTickCount()%1000;
						ActionPosition=160+(WORD)::GetTickCount()%120;
						pProtectionCalculater->ProtectionActionBreakerTrip[0]=FALSE;				//���������澯
						g_Unit[pProtectionCalculater->ProtectionID].VarGroup[1].Var[22].Value=TRUE;
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

//BOOL WINAPI ProtectionCalculater6(TagProtectionCalculaterStruct *pProtectionCalculater)		//����ͨ��˳��:���ݵ�ѹU,���ݵ���I,����һ��SI1,��ѹһ��SU1,��������SI2,��ѹ����SU2
//{
//	return TRUE;
//}


