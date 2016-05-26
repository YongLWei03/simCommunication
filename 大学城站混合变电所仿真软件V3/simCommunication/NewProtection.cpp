#include "stdafx.h"
#include "NewProtection.h"
#include "DlgMessageBox.h"

CMutex g_ShareComputerMutex;
HANDLE g_hShareComputer = NULL;
TagComputerShareStruct *g_pShareComputer = NULL;

TagUnitStruct g_Unit[UNITCOUNT];
TagBypassFeedStruct g_UnitBypassFeed[UNITCOUNT];

CString g_SceneOperateFileName;
TagSceneOperateStruct g_SceneOperate;
int g_SceneOperateID = -1;

CString defUnitType[]={"����","��ѹ��·����","��ѹ����Ͷ","����ѹ������","�������鱣��","ֱ�����߱���","ֱ�����߱���","ֱ����ܱ���","��ѹ��·����","��ѹ����Ͷ",""};
//                        0           1             2             3             4               5             6            7                 8               9 
float g_SimTrainData[UNITCOUNT][VARCOUNT][100];

void WINAPI LoadUnitList()
{
  CWaitCursor WaitCursor;

  CString filename = g_ProjectFilePath + "simProtection.cfg";

  CWaitCursor cursor;

  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    g_Unit[unitno].UnitID = unitno;
    g_Unit[unitno].UnitType = 0;
    g_Unit[unitno].UnitName = "";
    g_Unit[unitno].UnitState = 0;
    g_Unit[unitno].UnitCT = 0;
    g_Unit[unitno].UnitPT = 0;
    g_Unit[unitno].CurrentParamaterGroupID = 0;
    g_Unit[unitno].CurrentValue = 0;
    g_Unit[unitno].VotlageValue = 0;
    g_Unit[unitno].AngleValue = 0;
    g_Unit[unitno].TransmissionState = FALSE;
    g_Unit[unitno].TrainState = FALSE;
            
    for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
    {
      g_Unit[unitno].VarGroup[vargroupno].UnitID = unitno;
      g_Unit[unitno].VarGroup[vargroupno].VarGroupID = vargroupno;
      for(int varno=0;varno<VARCOUNT;varno++)
      {
        g_Unit[unitno].VarGroup[vargroupno].Var[varno].UnitID = unitno;
        g_Unit[unitno].VarGroup[vargroupno].Var[varno].VarGroupID = vargroupno;
        g_Unit[unitno].VarGroup[vargroupno].Var[varno].VarID = varno;
        g_Unit[unitno].VarGroup[vargroupno].Var[varno].VarName = "";
        g_Unit[unitno].VarGroup[vargroupno].Var[varno].TextFormat = "";
        g_Unit[unitno].VarGroup[vargroupno].Var[varno].Value = 0;
        g_Unit[unitno].VarGroup[vargroupno].Var[varno].LastValue = g_Unit[unitno].VarGroup[vargroupno].Var[varno].Value;
      }
    }
    {
      for(int vargroupno=0;vargroupno<PARAMETERGROUPCOUNT;vargroupno++)
      {
        for(int varno=0;varno<PARAMETERCOUNT;varno++)
        {
          g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].Value = 0;
          g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].TextFormat = "";
        }
      }
    }
  }

	CFile file;
  CFileException fe;
	if(!file.Open(filename, CFile::modeRead, &fe))
  {
    ::AfxMessageBox(filename + " �ļ���ȡʧ��");
    return ;
  }
  else
  {
    CArchive ar(&file,CArchive::load);
    TRY    
    {
      CString title = "";
      ar>>title;

      WORD UnitCount;
      WORD VarGroupCount;
      WORD ParameterGroupCount;
      WORD VarCount;
      WORD ParameterCount;
      ar>>UnitCount;
      ar>>VarGroupCount;
      ar>>ParameterGroupCount;
      ar>>VarCount;
      ar>>ParameterCount;

      for(int unitno=0;unitno<UnitCount;unitno++)
      {
        g_Unit[unitno].UnitID = unitno;
        ar>>g_Unit[unitno].UnitType;
        ar>>g_Unit[unitno].UnitName;
        ar>>g_Unit[unitno].UnitState;
        ar>>g_Unit[unitno].UnitCT;
        ar>>g_Unit[unitno].UnitPT;
        ar>>g_Unit[unitno].CurrentParamaterGroupID;
        ar>>g_Unit[unitno].CurrentValue;
        ar>>g_Unit[unitno].VotlageValue;
        ar>>g_Unit[unitno].AngleValue;
        g_Unit[unitno].TransmissionState = FALSE;
        g_Unit[unitno].TrainState = FALSE;

        {
          for(int vargroupno=0;vargroupno<VarGroupCount;vargroupno++)
          {
            g_Unit[unitno].VarGroup[vargroupno].UnitID = unitno;
            g_Unit[unitno].VarGroup[vargroupno].VarGroupID = vargroupno;
            for(int varno=0;varno<VarCount;varno++)
            {
              g_Unit[unitno].VarLock[varno] = FALSE;
              g_Unit[unitno].VarAbnormal[varno] = FALSE;
              g_Unit[unitno].VarGroup[vargroupno].Var[varno].VarGroupID = vargroupno;
              g_Unit[unitno].VarGroup[vargroupno].Var[varno].VarID = varno;
              ar>>g_Unit[unitno].VarGroup[vargroupno].Var[varno].VarName;
              ar>>g_Unit[unitno].VarGroup[vargroupno].Var[varno].TextFormat;
              ar>>g_Unit[unitno].VarGroup[vargroupno].Var[varno].Value;
              g_Unit[unitno].VarGroup[vargroupno].Var[varno].LastValue = g_Unit[unitno].VarGroup[vargroupno].Var[varno].Value;
            }
          }
        }
        {
          CString inifilename = g_ProjectFilePath + "\\ini\\" + defUnitType[g_Unit[unitno].UnitType] + ".ini";
          for(int vargroupno=0;vargroupno<ParameterGroupCount;vargroupno++)
          {
            for(int varno=0;varno<ParameterCount;varno++)
            {
              g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].UnitID = unitno;
              g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].VarID = varno;
              ar>>g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].Value;

              char buf[1024];
              CString key;

              key.Format("%02d",varno);
              ::GetPrivateProfileString("��ֵ����",key,"",buf,1024,inifilename);
              g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].VarName = buf;

              if(varno<32)//����ѹ��
                g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].TextFormat = "��;Ͷ";
              else
                g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].TextFormat = "0.00";
            }
          }
        }

        {
          switch(g_Unit[unitno].UnitType)
          {
            case 2://��ѹ����Ͷ
            case 7://ֱ����ܱ���
            case 9://��ѹ����Ͷ
              break;
            case 1://��ѹ��·����
            case 3://��ѹ��ѹ������
            case 4://��ѹ�������鱣��
            case 5://ֱ�����߱���
            case 6://ֱ�����߱���
            case 8://��ѹ��·����
              if(g_Unit[unitno].VarGroup[1].Var[0].Value==g_Unit[unitno].VarGroup[1].Var[1].Value)//��·��
                g_Unit[unitno].VarGroup[1].Var[1].Value = !g_Unit[unitno].VarGroup[1].Var[0].Value;
              g_Unit[unitno].VarGroup[1].Var[3].Value = 1;//����
              g_Unit[unitno].VarGroup[1].Var[3].VarName = g_Unit[unitno].UnitName+"����״̬";
              g_Unit[unitno].VarGroup[1].Var[3].TextFormat = "δ����;����";
              g_Unit[unitno].VarGroup[1].Var[4].Value = 0;//���ƻ�·���߸澯
              g_Unit[unitno].VarGroup[1].Var[4].VarName = g_Unit[unitno].UnitName+"���ƻ�·���߸澯";
              g_Unit[unitno].VarGroup[1].Var[4].TextFormat = "δ����;����";
              g_Unit[unitno].VarGroup[1].Var[23].Value = 0;//��·���ܶ�
              g_Unit[unitno].VarGroup[1].Var[23].VarName = g_Unit[unitno].UnitName+"��·���ܶ�";
              g_Unit[unitno].VarGroup[1].Var[23].TextFormat = "��;��";
              break;
          }
        }
        g_Unit[unitno].VarGroup[1].Var[24].VarName = g_Unit[unitno].UnitName+"������ֵ������ٻ�";
        g_Unit[unitno].VarGroup[1].Var[25].VarName = g_Unit[unitno].UnitName+"������ֵ������л�";
        g_Unit[unitno].VarGroup[1].Var[26].VarName = g_Unit[unitno].UnitName+"������ֵ���0�趨";
        g_Unit[unitno].VarGroup[1].Var[27].VarName = g_Unit[unitno].UnitName+"������ֵ���1�趨";
        g_Unit[unitno].VarGroup[1].Var[28].VarName = g_Unit[unitno].UnitName+"������ֵ���2�趨";
        g_Unit[unitno].VarGroup[1].Var[29].VarName = g_Unit[unitno].UnitName+"������ֵ���3�趨";

        g_Unit[unitno].VarGroup[1].Var[24].TextFormat = "��;��";
        g_Unit[unitno].VarGroup[1].Var[25].TextFormat = "��;��";
        g_Unit[unitno].VarGroup[1].Var[26].TextFormat = "��;��";
        g_Unit[unitno].VarGroup[1].Var[27].TextFormat = "��;��";
        g_Unit[unitno].VarGroup[1].Var[28].TextFormat = "��;��";
        g_Unit[unitno].VarGroup[1].Var[29].TextFormat = "��;��";
        g_Unit[unitno].VarGroup[1].Var[24].Value = 0;
        g_Unit[unitno].VarGroup[1].Var[25].Value = 0;
        g_Unit[unitno].VarGroup[1].Var[26].Value = 0;
        g_Unit[unitno].VarGroup[1].Var[27].Value = 0;
        g_Unit[unitno].VarGroup[1].Var[28].Value = 0;
        g_Unit[unitno].VarGroup[1].Var[29].Value = 0;
      
        for(int varno=32;varno<VarCount;varno++)
        {
          g_Unit[unitno].VarGroup[1].Var[varno].VarName = "";
          g_Unit[unitno].VarGroup[1].Var[varno].TextFormat = "";
          g_Unit[unitno].VarGroup[1].Var[varno].Value = 0;
        }

        g_Unit[unitno].VarGroup[1].Var[63].VarName = "����ͨѶ�쳣";
        g_Unit[unitno].VarGroup[1].Var[63].TextFormat = "��;��";
        g_Unit[unitno].VarGroup[1].Var[63].Value = 0;

        if (g_Unit[unitno].UnitType==0)//����
        {
          for(int varno=0;varno<VarCount;varno++)
          {
            g_Unit[unitno].VarGroup[1].Var[varno].VarName = "";
            g_Unit[unitno].VarGroup[1].Var[varno].TextFormat = "";
            g_Unit[unitno].VarGroup[1].Var[varno].Value = 0;
          }
        }
      }

      ar.Flush();
      ar.Close();
      file.Close();
    }
    CATCH_ALL(e)
    {
	    TRY
        ar.Abort();
	    END_TRY
      CString str=filename+" ��ȡ�ļ�ʧ��!";
      AfxMessageBox(str,MB_SYSTEMMODAL);
      return;
	  }
	  END_CATCH_ALL
  }
  g_Unit[19].VarGroup[1].Var[61].Value = 0;//���϶�·��������
  g_Unit[19].VarGroup[1].Var[62].Value = 0;//�������ݲ鿴����
}

void WINAPI SaveUnitList()
{
  CString filename = g_ProjectFilePath + "simProtection.cfg";

  int state = ::GetFileAttributes(filename);
  if(state>=0&&state&FILE_ATTRIBUTE_READONLY)
  {
    CString str=filename+" Ϊֻ���ļ�,�Ƿ��������?";
    if(AfxMessageBox(str,MB_YESNO)==IDNO)
      return ;
    ::SetFileAttributes(filename,FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_NORMAL);
  }

	CFile file;
  CFileException fe;
	if(!file.Open(filename, CFile::modeCreate | CFile::modeWrite, &fe))
	{
    CString str=filename+" �ļ��洢����!";
    AfxMessageBox(str,MB_SYSTEMMODAL);
    return ;
	}  
  char buffer[1024];
  CArchive ar(&file,CArchive::store,1024,buffer);
  TRY
  {
    CString title = "VirtualProtection 1.0";
    ar<<title;

    ar<<(WORD)UNITCOUNT;
    ar<<(WORD)VARGROUPCOUNT;
    ar<<(WORD)PARAMETERGROUPCOUNT;
    ar<<(WORD)VARCOUNT;
    ar<<(WORD)PARAMETERCOUNT;

    for(int unitno=0;unitno<UNITCOUNT;unitno++)
    {
      ar<<g_Unit[unitno].UnitType;
      ar<<g_Unit[unitno].UnitName;
      ar<<g_Unit[unitno].UnitState;
      ar<<g_Unit[unitno].UnitCT;
      ar<<g_Unit[unitno].UnitPT;
      ar<<g_Unit[unitno].CurrentParamaterGroupID;
      ar<<g_Unit[unitno].CurrentValue;
      ar<<g_Unit[unitno].VotlageValue;
      ar<<g_Unit[unitno].AngleValue;
    
      {
        for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
        {
          for(int varno=0;varno<VARCOUNT;varno++)
          {
            ar<<g_Unit[unitno].VarGroup[vargroupno].Var[varno].VarName;
            ar<<g_Unit[unitno].VarGroup[vargroupno].Var[varno].TextFormat;
            ar<<g_Unit[unitno].VarGroup[vargroupno].Var[varno].Value;
          }
        }
      }
      {
        for(int vargroupno=0;vargroupno<PARAMETERGROUPCOUNT;vargroupno++)
        {
          for(int varno=0;varno<PARAMETERCOUNT;varno++)
            ar<<g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].Value;
        }
      }
    }

    ar.Flush();
    ar.Close();
    file.Close();
  }
  CATCH_ALL(e)
  {
	  TRY
      ar.Abort();
	  END_TRY

    CString str=filename+" �ļ��洢����!";
    AfxMessageBox(str,MB_SYSTEMMODAL);
    return ;
	}
	END_CATCH_ALL
}

void WINAPI LoadParameterList()
{
  CWaitCursor WaitCursor;

  CString filename = g_ProjectFilePath + "simProtection.par";

  CWaitCursor cursor;

	CFile file;
  CFileException fe;
	if(!file.Open(filename, CFile::modeRead, &fe))
  {
    ::AfxMessageBox(filename + " �ļ���ȡʧ��");
    return ;
  }
  else
  {
    CArchive ar(&file,CArchive::load);
    TRY    
    {
      CString title = "";
      ar>>title;

      WORD UnitCount;
      WORD ParameterGroupCount;
      WORD ParameterCount;
      ar>>UnitCount;
      ar>>ParameterGroupCount;
      ar>>ParameterCount;

      for(int unitno=0;unitno<UnitCount;unitno++)
      {
        for(int vargroupno=0;vargroupno<ParameterGroupCount;vargroupno++)
        {
          for(int varno=0;varno<ParameterCount;varno++)
            ar>>g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].Value;
        }
      }

      ar.Flush();
      ar.Close();
      file.Close();
    }
    CATCH_ALL(e)
    {
	    TRY
        ar.Abort();
	    END_TRY
      CString str=filename+" ��ȡ�ļ�ʧ��!";
      AfxMessageBox(str,MB_SYSTEMMODAL);
      return;
	  }
	  END_CATCH_ALL
  }
}

void WINAPI SaveParameterList()
{
  CString filename = g_ProjectFilePath + "simProtection.par";

  int state = ::GetFileAttributes(filename);
  if(state>=0&&state&FILE_ATTRIBUTE_READONLY)
  {
    CString str=filename+" Ϊֻ���ļ�,�Ƿ��������?";
    if(AfxMessageBox(str,MB_YESNO)==IDNO)
      return ;
    ::SetFileAttributes(filename,FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_NORMAL);
  }

	CFile file;
  CFileException fe;
	if(!file.Open(filename, CFile::modeCreate | CFile::modeWrite, &fe))
	{
    CString str=filename+" �ļ��洢����!";
    AfxMessageBox(str,MB_SYSTEMMODAL);
    return ;
	}  
  char buffer[1024];
  CArchive ar(&file,CArchive::store,1024,buffer);
  TRY
  {
    CString title = "VirtualProtection 1.0";
    ar<<title;

    ar<<(WORD)UNITCOUNT;
    ar<<(WORD)PARAMETERGROUPCOUNT;
    ar<<(WORD)PARAMETERCOUNT;

    for(int unitno=0;unitno<UNITCOUNT;unitno++)
    {
      for(int vargroupno=0;vargroupno<PARAMETERGROUPCOUNT;vargroupno++)
      {
        for(int varno=0;varno<PARAMETERCOUNT;varno++)
          ar<<g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].Value;
      }
    }

    ar.Flush();
    ar.Close();
    file.Close();
  }
  CATCH_ALL(e)
  {
	  TRY
      ar.Abort();
	  END_TRY

    CString str=filename+" �ļ��洢����!";
    AfxMessageBox(str,MB_SYSTEMMODAL);
    return ;
	}
	END_CATCH_ALL
}

void WINAPI ProductActionValue(WORD UnitID,float ActionValue[VARCOUNT])
{
  switch(g_Unit[UnitID].UnitType)
  {
    case 1://��ѹ�������
      break;
    case 2://��ѹ���ߺ󱸱���
    {
      float HiPTValue = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+0].Value;
      float LoPTValue = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+1].Value;
      float HiCTValuePhaseA = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+3].Value;//A��CT
      float HiCTValuePhaseB = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+4].Value;//B��CT
      float HiCTValuePhaseC = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+5].Value;//C��CT

      ActionValue[0] = g_Unit[UnitID].VarGroup[0].Var[3].Value / HiPTValue * 1000;//Ua
      ActionValue[1] = g_Unit[UnitID].VarGroup[0].Var[4].Value / HiPTValue * 1000;//Ub
      ActionValue[2] = g_Unit[UnitID].VarGroup[0].Var[5].Value / HiPTValue * 1000;//Uc
      ActionValue[3] = g_Unit[UnitID].VarGroup[0].Var[7].Value / HiCTValuePhaseA;//Ia
      ActionValue[4] = g_Unit[UnitID].VarGroup[0].Var[8].Value / HiCTValuePhaseB;//Ib
      ActionValue[5] = g_Unit[UnitID].VarGroup[0].Var[9].Value / HiCTValuePhaseC;//Ic
      ActionValue[6] = g_Unit[UnitID].VarGroup[0].Var[6].Value ;//Uo
      ActionValue[7] = g_Unit[UnitID].VarGroup[0].Var[10].Value;//Io
      ActionValue[8] = g_Unit[UnitID].VarGroup[0].Var[2].Value / HiPTValue * 1000;//U��
      ActionValue[9] = g_Unit[UnitID].VarGroup[0].Var[1].Value / HiPTValue * 1000;//U��
      ActionValue[10] = g_Unit[UnitID].VarGroup[0].Var[0].Value / HiPTValue * 1000;//Uab 
      ActionValue[11] = g_Unit[UnitID].VarGroup[0].Var[1].Value / HiPTValue * 1000;//Ubc 
      ActionValue[12] = g_Unit[UnitID].VarGroup[0].Var[2].Value / HiPTValue * 1000;//Uca 
      break;
    }
    case 3://��ѹ���ͺ󱸱���
    {
      float PTValue = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+0].Value;
      float CTValuePhaseA = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+1].Value;//����CT
      float CTValuePhaseB = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+2].Value;//����CT

      ActionValue[0] = g_Unit[UnitID].VarGroup[0].Var[0].Value / PTValue * 1000;//Ua
      ActionValue[1] = g_Unit[UnitID].VarGroup[0].Var[1].Value / PTValue * 1000;//Ub
      ActionValue[2] = g_Unit[UnitID].VarGroup[0].Var[2].Value / CTValuePhaseA;//Ia
      ActionValue[3] = g_Unit[UnitID].VarGroup[0].Var[3].Value / CTValuePhaseB;//Ib
      break;
    }
    case 4://��·����
    {
      float PTValue = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+0].Value;
      float CTValue = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+1].Value;
      ActionValue[0] = g_Unit[UnitID].VarGroup[0].Var[0].Value / PTValue * 1000;//U
      ActionValue[1] = g_Unit[UnitID].VarGroup[0].Var[1].Value / CTValue;//I
      break;
    }
    case 5://����������
    {
      float PTValue = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+0].Value;
      float CTValue = g_Unit[UnitID].ParameterGroup[g_Unit[UnitID].CurrentParamaterGroupID].Var[32+1].Value;
      ActionValue[0] = g_Unit[UnitID].VarGroup[0].Var[0].Value / PTValue * 1000;//U
      ActionValue[1] = g_Unit[UnitID].VarGroup[0].Var[1].Value / CTValue;//I
      break;
    }
  }
}

void WINAPI ProductSOEReport(WORD UnitID,WORD SOEID,CTime SOETime,WORD SOEMSEL,WORD SOEValue,float ActionValue[VARCOUNT])
{
  if(g_Unit[UnitID].UnitType==0)
    return;
  
  CSingleLock sLock(&g_SendSOEReportListMutex);
  sLock.Lock();

  TagSOEReportStruct * pLastSOEReport = NULL;
  if(g_Unit[UnitID].SOEReportList.GetCount()>0)
  {
    pLastSOEReport = (TagSOEReportStruct*)g_Unit[UnitID].SOEReportList.GetTail();  
    if((pLastSOEReport->SOEID==SOEID)&&(pLastSOEReport->SOEValue==SOEValue))
    {
      if(((SOETime - pLastSOEReport->SOETime).GetTotalSeconds())<10)
        return;
    }
  }
  
  TagSOEReportStruct * pSOEReportStruct = new TagSOEReportStruct;
  pSOEReportStruct->UnitID = UnitID;
  pSOEReportStruct->SOEID = SOEID;
  pSOEReportStruct->SOETime = SOETime;
  pSOEReportStruct->SOEMSEL = SOEMSEL;
  pSOEReportStruct->SOEValue = SOEValue;
  pSOEReportStruct->SendState = FALSE;
  for(int no=0;no<VARCOUNT;no++)
    pSOEReportStruct->ActionValue[no] = ActionValue[no];

  for(int ch=0;ch<12;ch++)
  {
    for(int samp=0;samp<320;samp++)
      pSOEReportStruct->RecordWaveValue[ch][samp] = g_Unit[UnitID].SampleWaveValue[ch][samp];
  }

  g_Unit[UnitID].SOEReportList.AddTail(pSOEReportStruct);
  if(g_Unit[UnitID].SOEReportList.GetCount()>10)
    delete (TagSOEReportStruct *)g_Unit[UnitID].SOEReportList.RemoveHead();

  ::WriteSOEReportToShareMemory(pSOEReportStruct);

  g_SendSOEReportList.AddTail(pSOEReportStruct);
}

CString WINAPI LoadSOEDefName(WORD UnitType,WORD SOEID)
{
  CString filename = g_ProjectFilePath + "\\ini\\" + defUnitType[UnitType] + ".ini";

  char buf[1024];
  CString key;

  key.Format("%02d",SOEID);
  ::GetPrivateProfileString("��������",key,"",buf,1024,filename);
  
  CNewString ns(buf,";","\r\n");
  CString SOEDefName = ns.GetWord(0,0);

  return SOEDefName;
}

BOOL WINAPI LoadSOEDefActionValue(WORD UnitType,WORD SOEID)
{
  CString filename = g_ProjectFilePath + "\\ini\\" + defUnitType[UnitType] + ".ini";

  char buf[1024];
  CString key;

  key.Format("%02d",SOEID);
  ::GetPrivateProfileString("��������",key,"",buf,1024,filename);
  
  CNewString ns(buf,";","\r\n");

  return ns.GetIntegerValue(0,1);
}

CString WINAPI LoadActionValueDefName(WORD UnitType,WORD ActionValueID)
{
  CString filename = g_ProjectFilePath + "\\ini\\" + defUnitType[UnitType] + ".ini";

  char buf[1024];
  CString key;

  key.Format("%02d",ActionValueID);
  ::GetPrivateProfileString("����ֵ",key,"",buf,1024,filename);
  
  CString ActionValueDefName = buf;

  return ActionValueDefName;
}

float WINAPI GetProtectionChannelMaxValue(float SampleWaveValue[])
{
  float MaxValue = 0;
  for(int samp=0;samp<WAVE_SAMPLE_COUNT;samp++)
  {
    float Value = (float)fabs(SampleWaveValue[samp]);
    if(MaxValue<Value)
      MaxValue = Value;
  }

  return MaxValue;
}

float WINAPI GetProtectionTrainMaxValue(float TrainDataValue[])
{
  float MaxValue = 0;
  for(int datano=0;datano<100;datano++)
  {
    float Value = (float)fabs(TrainDataValue[datano]);
    if(MaxValue<Value)
      MaxValue = Value;
  }

  return MaxValue;
}

BOOL WINAPI GetBypassFeedState(WORD UnitID)
{
  for(int no=0;no<3;no++)
  {
    float Value = g_Unit[g_UnitBypassFeed[UnitID].Var[no].UnitID].VarGroup[1].Var[g_UnitBypassFeed[UnitID].Var[no].VarID].Value;
    if(Value==0)
      return FALSE;
  }

  return TRUE;
}

BOOL WINAPI IsBypassFeed(WORD UnitID)
{
  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    if(g_Unit[unitno].UnitType==4)
    {
      if(g_UnitBypassFeed[unitno].Var[0].UnitID==UnitID)
        return TRUE;
    }
  }

  return FALSE;
}

BOOL WINAPI StartupSceneOperate(TagSceneOperateStruct *pSceneOperate)
{
  ::ClearPower();

  CSingleLock sLock(&g_PowerMutex);
  sLock.Lock();

  g_InlineVotlage.Ua1 = pSceneOperate->InlineVoltage.Ua1;
  g_InlineVotlage.Ub1 = pSceneOperate->InlineVoltage.Ub1;
  g_InlineVotlage.Uc1 = pSceneOperate->InlineVoltage.Uc1;
  g_InlineVotlage.Ua2 = pSceneOperate->InlineVoltage.Ua2;
  g_InlineVotlage.Ub2 = pSceneOperate->InlineVoltage.Ub2;
  g_InlineVotlage.Uc2 = pSceneOperate->InlineVoltage.Uc2;
  g_InlineVotlage.T1HiVoltageChangeRate = pSceneOperate->InlineVoltage.T1HiVoltageChangeRate;
  g_InlineVotlage.T1LoVoltageChangeRate = pSceneOperate->InlineVoltage.T1LoVoltageChangeRate;
  g_InlineVotlage.T2HiVoltageChangeRate = pSceneOperate->InlineVoltage.T2HiVoltageChangeRate;
  g_InlineVotlage.T2LoVoltageChangeRate = pSceneOperate->InlineVoltage.T2LoVoltageChangeRate;
  g_InlineVotlage.StartupDelaySecond = 0;
  g_InlineVotlage.BeginTime = 0;

  g_PTVoltage.T1HiVoltageValueK = pSceneOperate->PTVoltage.T1HiVoltageValueK;
  g_PTVoltage.T1HiCurrentMaxValue = pSceneOperate->PTVoltage.T1HiCurrentMaxValue;
  g_PTVoltage.T2HiVoltageValueK = pSceneOperate->PTVoltage.T2HiVoltageValueK;
  g_PTVoltage.T2HiCurrentMaxValue = pSceneOperate->PTVoltage.T2HiCurrentMaxValue;
  g_PTVoltage.T1LoVoltageValueK = pSceneOperate->PTVoltage.T1LoVoltageValueK;
  g_PTVoltage.T1LoCurrentMaxValue = pSceneOperate->PTVoltage.T1LoCurrentMaxValue;
  g_PTVoltage.T2LoVoltageValueK = pSceneOperate->PTVoltage.T2LoVoltageValueK;
  g_PTVoltage.T2LoCurrentMaxValue = pSceneOperate->PTVoltage.T2LoCurrentMaxValue;
  g_PTVoltage.T1Gear = pSceneOperate->PTVoltage.T1Gear;
  g_PTVoltage.T2Gear = pSceneOperate->PTVoltage.T2Gear;
  g_PTVoltage.T1Temp1 = pSceneOperate->PTVoltage.T1Temp1;
  g_PTVoltage.T1Temp2 = pSceneOperate->PTVoltage.T1Temp2;
  g_PTVoltage.T2Temp1 = pSceneOperate->PTVoltage.T2Temp1;
  g_PTVoltage.T2Temp2 = pSceneOperate->PTVoltage.T2Temp2;

  g_PTVoltage.StartupDelaySecond = 0;
  g_PTVoltage.BeginTime = 0;

  g_Current.NoloadCurrentChangeMaxValue = pSceneOperate->Current.NoloadCurrentChangeMaxValue;
  g_Current.StartupDelaySecond = 0;
  g_Current.BeginTime = 0;

  memcpy(&g_Compensation[0],&pSceneOperate->Compensation[0],sizeof(TagCompensationStruct));
  memcpy(&g_Compensation[1],&pSceneOperate->Compensation[1],sizeof(TagCompensationStruct));

  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      g_Unit[unitno].VarLock[varno] = pSceneOperate->VarLock[unitno][varno];
      g_Unit[unitno].VarAbnormal[varno] = pSceneOperate->VarAbnormal[unitno][varno];
      g_Unit[unitno].CurrentParamaterGroupID = pSceneOperate->CurrentParamaterGroupID[unitno];

      if(pSceneOperate->EnableVar[unitno][varno]==TRUE)
      {
        g_Unit[unitno].VarGroup[1].Var[varno].Value = pSceneOperate->VarValue[unitno][varno];
        g_Unit[unitno].VarGroup[1].Var[varno].LastValue = g_Unit[unitno].VarGroup[1].Var[varno].Value;
      }
    }

    for(int vargroupno=0;vargroupno<PARAMETERGROUPCOUNT;vargroupno++)
    {
      g_Unit[unitno].CurrentParamaterGroupID = pSceneOperate->CurrentParamaterGroupID[unitno];
      for(int varno=0;varno<PARAMETERCOUNT;varno++)
      {
        if(pSceneOperate->EnableParameterVar[unitno][vargroupno][varno]==TRUE)
          g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].Value = pSceneOperate->ParameterVarValue[unitno][vargroupno][varno];
      }
    }
  }

  if(pSceneOperate->EnableFreeTrain==TRUE)
  {
    TagFreeTrainStruct *pFreeTrain = new TagFreeTrainStruct;
    pFreeTrain->RealTimeCurrent = 0;
    pFreeTrain->RealTimeAngle = 0;
    pFreeTrain->AverCurrent = pSceneOperate->FreeTrain.AverCurrent;
    pFreeTrain->AverAngle = pSceneOperate->FreeTrain.AverAngle;
    pFreeTrain->CurrentChangeRate = pSceneOperate->FreeTrain.CurrentChangeRate;
    strcpy(pFreeTrain->InCurrentChangeRateList,pSceneOperate->FreeTrain.InCurrentChangeRateList);
    strcpy(pFreeTrain->OutCurrentChangeRateList,pSceneOperate->FreeTrain.OutCurrentChangeRateList);
    pFreeTrain->InContinuedSeconds = pSceneOperate->FreeTrain.InContinuedSeconds;
    pFreeTrain->OutContinuedSeconds = pSceneOperate->FreeTrain.OutContinuedSeconds;
    pFreeTrain->ChangeContinuedSeconds = pSceneOperate->FreeTrain.ChangeContinuedSeconds;
    pFreeTrain->InFeederUnitID = pSceneOperate->FreeTrain.InFeederUnitID;
    pFreeTrain->OutFeederUnitID = pSceneOperate->FreeTrain.OutFeederUnitID;
    pFreeTrain->StartupDelaySecond = pSceneOperate->FreeTrain.StartupDelaySecond;

    pFreeTrain->InBeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,pFreeTrain->StartupDelaySecond);;
    pFreeTrain->InFinishTime = pFreeTrain->InBeginTime+CTimeSpan(0,0,0,pFreeTrain->InContinuedSeconds);
    pFreeTrain->ChangeBeginTime = pFreeTrain->InFinishTime;
    pFreeTrain->ChangeFinishTime = pFreeTrain->ChangeBeginTime+CTimeSpan(0,0,0,pFreeTrain->ChangeContinuedSeconds);
    pFreeTrain->OutBeginTime = pFreeTrain->ChangeFinishTime;
    pFreeTrain->OutFinishTime = pFreeTrain->OutBeginTime+CTimeSpan(0,0,0,pFreeTrain->OutContinuedSeconds);

    ::AddFreeTrain(pFreeTrain);

    //g_pDlgMessageBox->DoShow("׼�����������г�",3);
  }
  if(pSceneOperate->EnableFreeTransmission==TRUE)
  {
    struct TagFreeTransmissionStruct *pFreeTransmission = new TagFreeTransmissionStruct;

    pFreeTransmission->FeederUnitID = pSceneOperate->FreeTransmission.FeederUnitID;
    pFreeTransmission->AverCurrent = pSceneOperate->FreeTransmission.AverCurrent;
    pFreeTransmission->AverAngle = pSceneOperate->FreeTransmission.AverAngle;
    pFreeTransmission->CurrentChangeRate = pSceneOperate->FreeTransmission.CurrentChangeRate;
    strcpy(pFreeTransmission->CurrentChangeRateList,pSceneOperate->FreeTransmission.CurrentChangeRateList);
    pFreeTransmission->ContinuedSeconds = pSceneOperate->FreeTransmission.ContinuedSeconds;
    pFreeTransmission->StartupDelaySecond = pSceneOperate->FreeTransmission.StartupDelaySecond;

    pFreeTransmission->BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,pFreeTransmission->StartupDelaySecond);
    pFreeTransmission->FinishTime = pFreeTransmission->BeginTime + CTimeSpan(0,0,0,pFreeTransmission->ContinuedSeconds);
    ::AddFreeTransmission(pFreeTransmission);

    //g_pDlgMessageBox->DoShow("׼���������ߴ���",3);
  }  
  if(pSceneOperate->EnableSimTrain==TRUE)
  {
    strcpy(g_SimTrain.SimTrainDataFileName,pSceneOperate->SimTrain.SimTrainDataFileName);
    g_SimTrain.StartupDelaySecond = pSceneOperate->SimTrain.StartupDelaySecond;
    g_SimTrain.BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,g_SimTrain.StartupDelaySecond);;
    g_SimTrain.SimTrainDataCount = 0;
    g_SimTrain.SimTrainDataCurrentPos = 0;
    g_SimTrain.SimTrainState = 1;//׼�����ļ�

    //g_pDlgMessageBox->DoShow("׼�����������г�",3);
  }
  if(pSceneOperate->EnableFaultTransmission==TRUE)
  {
    strcpy(g_FaultTransmission.FaultTransmissionDataFileName,pSceneOperate->FaultTransmission.FaultTransmissionDataFileName);
    g_FaultTransmission.StartupDelaySecond = pSceneOperate->FaultTransmission.StartupDelaySecond;
    g_FaultTransmission.BeginTime = CTime::GetCurrentTime() + CTimeSpan(0,0,0,g_FaultTransmission.StartupDelaySecond);
    g_FaultTransmission.FaultTransmissionState = 1;//׼�����ļ�

    //g_pDlgMessageBox->DoShow("׼���������ϴ���",3);
  }
  return TRUE;
}

BOOL WINAPI ClearSceneOperate(TagSceneOperateStruct *pSceneOperate)
{
  CString filename = g_SystemFilePath + "simPower.ini";
  char buf[1024];

  CWaitCursor WaitCursor;

  pSceneOperate->SceneName = "";
  pSceneOperate->SceneDescribe = "";
  
  pSceneOperate->EnableFreeTrain = FALSE;        //�����г�������Ч
  pSceneOperate->EnableSimTrain = FALSE;         //�����г�������Ч
  pSceneOperate->EnableFaultTransmission = FALSE;//���洫��������Ч
  pSceneOperate->EnableFreeTransmission = FALSE; //���ɴ���������Ч

  ::GetPrivateProfileString("Ĭ��ֵ","����������ѹֵ","66.4",buf,1024,filename);
  pSceneOperate->InlineVoltage.Ua1 = (float)atof(buf) * 35 / 110;
  pSceneOperate->InlineVoltage.Ub1 = (float)atof(buf) * 35 / 110;
  pSceneOperate->InlineVoltage.Uc1 = (float)atof(buf) * 35 / 110;
  pSceneOperate->InlineVoltage.Ua2 = (float)atof(buf) * 35 / 110;
  pSceneOperate->InlineVoltage.Ub2 = (float)atof(buf) * 35 / 110;
  pSceneOperate->InlineVoltage.Uc2 = (float)atof(buf) * 35 / 110;

  ::GetPrivateProfileString("Ĭ��ֵ","���ݼ�����ʱʱ��","10",buf,1024,filename);
	pSceneOperate->PTVoltage.StartupDelaySecond = atoi(buf);

  char VoltageChangeRate[1024];
  char CurrentMaxValue[1024];
  char VoltageValueK[1024];
  ::GetPrivateProfileString("Ĭ��ֵ","��ѹ����ֵ","0.01",VoltageChangeRate,1024,filename);
  ::GetPrivateProfileString("Ĭ��ֵ","��ѹ�����������ֵ","0",CurrentMaxValue,1024,filename);
  ::GetPrivateProfileString("Ĭ��ֵ","��ѹ����ϵ��","1",VoltageValueK,1024,filename);

	pSceneOperate->InlineVoltage.T1HiVoltageChangeRate = (float)atof(VoltageChangeRate);
	pSceneOperate->InlineVoltage.T2HiVoltageChangeRate = (float)atof(VoltageChangeRate);
	pSceneOperate->InlineVoltage.T1LoVoltageChangeRate = (float)atof(VoltageChangeRate);
	pSceneOperate->InlineVoltage.T2LoVoltageChangeRate = (float)atof(VoltageChangeRate);

	pSceneOperate->PTVoltage.T1HiCurrentMaxValue = (float)atof(CurrentMaxValue);
	pSceneOperate->PTVoltage.T1HiVoltageValueK = (float)atof(VoltageValueK);
	pSceneOperate->PTVoltage.T2HiCurrentMaxValue = (float)atof(CurrentMaxValue);
	pSceneOperate->PTVoltage.T2HiVoltageValueK = (float)atof(VoltageValueK);
	pSceneOperate->PTVoltage.T1LoCurrentMaxValue = (float)atof(CurrentMaxValue);
	pSceneOperate->PTVoltage.T1LoVoltageValueK = (float)atof(VoltageValueK);
	pSceneOperate->PTVoltage.T2LoCurrentMaxValue = (float)atof(CurrentMaxValue);
	pSceneOperate->PTVoltage.T2LoVoltageValueK = (float)atof(VoltageValueK);

  char Gear[1024];
  char Temp1[1024];
  char Temp2[1024];
  ::GetPrivateProfileString("Ĭ��ֵ","���䵵λ","7",Gear,1024,filename);
  ::GetPrivateProfileString("Ĭ��ֵ","��������1","35",Temp1,1024,filename);
  ::GetPrivateProfileString("Ĭ��ֵ","��������2","35",Temp2,1024,filename);

	pSceneOperate->PTVoltage.T1Gear = atoi(Gear);
	pSceneOperate->PTVoltage.T2Gear = atoi(Gear);
	pSceneOperate->PTVoltage.T1Temp1 = atoi(Temp1);
	pSceneOperate->PTVoltage.T1Temp2 = atoi(Temp2);
	pSceneOperate->PTVoltage.T2Temp1 = atoi(Temp1);
	pSceneOperate->PTVoltage.T2Temp2 = atoi(Temp2);

  memcpy(&pSceneOperate->Compensation[0],&g_Compensation[0],sizeof(TagCompensationStruct));
  memcpy(&pSceneOperate->Compensation[1],&g_Compensation[1],sizeof(TagCompensationStruct));

  ::GetPrivateProfileString("Ĭ��ֵ","���ص�������ֵ","1",buf,1024,filename);
  pSceneOperate->Current.NoloadCurrentChangeMaxValue = (float)atof(buf);
  pSceneOperate->Current.StartupDelaySecond = atoi(buf);

  pSceneOperate->FreeTransmission.FeederUnitID = 0;      //���ɴ������߱��
  pSceneOperate->FreeTransmission.AverCurrent = 300;     //���ɴ�������ֵ
  pSceneOperate->FreeTransmission.AverAngle = 30;        //���ɴ����Ƕ�ֵ
	pSceneOperate->FreeTransmission.CurrentChangeRate = (float)0.01;  //���ɴ�����������
	pSceneOperate->FreeTransmission.StartupDelaySecond = 10;          //���ɴ�������ʱ��
  ::GetPrivateProfileString("Ĭ��ֵ","���ݼ�����ʱʱ��","10",buf,1024,filename);
	pSceneOperate->FreeTransmission.ContinuedSeconds = atoi(buf);     //���ɴ�������ʱ��
  pSceneOperate->FreeTransmission.CurrentChangeRateList[0] = 0x00;  //���ɴ��������仯��

  pSceneOperate->FreeTrain.InContinuedSeconds = 30;       //�����г�ʻ��ʱ��
  pSceneOperate->FreeTrain.OutContinuedSeconds = 30;      //�����г�ʻ��ʱ��
  pSceneOperate->FreeTrain.ChangeContinuedSeconds = 5;    //�����г�����ʱ��
  pSceneOperate->FreeTrain.AverCurrent = 300;             //�����г�ƽ������ֵ
  pSceneOperate->FreeTrain.AverAngle = 30;                //�����г�ƽ���Ƕ�ֵ
  pSceneOperate->FreeTrain.InFeederUnitID = 0;            //�����г�ʻ�����߱��
  pSceneOperate->FreeTrain.OutFeederUnitID = 0;           //�����г�ʻ�����߱��
	pSceneOperate->FreeTrain.CurrentChangeRate = (float)0.01;   //�����г���������
  ::GetPrivateProfileString("Ĭ��ֵ","���ݼ�����ʱʱ��","10",buf,1024,filename);
	pSceneOperate->FreeTrain.StartupDelaySecond = atoi(buf);    //�����г�����ʱ��
  pSceneOperate->FreeTrain.InCurrentChangeRateList[0] = 0x00; //�����г�ʻ������仯�ʱ�
  pSceneOperate->FreeTrain.OutCurrentChangeRateList[0] = 0x00;//�����г�ʻ�������仯�ʱ�

  pSceneOperate->SimTrain.SimTrainDataFileName[0] = 0x00;                         //�����г������ļ�����
  ::GetPrivateProfileString("Ĭ��ֵ","���ݼ�����ʱʱ��","10",buf,1024,filename);
	pSceneOperate->SimTrain.StartupDelaySecond = atoi(buf);         

  pSceneOperate->FaultTransmission.FaultTransmissionDataFileName[0] = 0x00;       //���ϴ��������ļ�����
  ::GetPrivateProfileString("Ĭ��ֵ","���ݼ�����ʱʱ��","10",buf,1024,filename);
	pSceneOperate->FaultTransmission.StartupDelaySecond = atoi(buf);

  for(int unitno=0;unitno<UNITCOUNT;unitno++)
  {
    pSceneOperate->CurrentParamaterGroupID[unitno] = 0;
    for(int varno=0;varno<VARCOUNT;varno++)
    {
      pSceneOperate->VarLock[unitno][varno] = FALSE;
      pSceneOperate->VarAbnormal[unitno][varno] = FALSE;
      pSceneOperate->EnableVar[unitno][varno] = FALSE;
      pSceneOperate->VarValue[unitno][varno] = 0;

      if(g_Unit[unitno].VarGroup[1].Var[varno].VarName=="")
        continue;

      if(g_Unit[unitno].VarGroup[1].Var[varno].VarName.Find("����")>=0)
        continue;

      pSceneOperate->VarValue[unitno][varno] = g_Unit[unitno].VarGroup[1].Var[varno].Value;
    }

    for(int vargroupno=0;vargroupno<PARAMETERGROUPCOUNT;vargroupno++)
    {
      for(int varno=0;varno<PARAMETERCOUNT;varno++)
      {
        pSceneOperate->EnableParameterVar[unitno][vargroupno][varno] = FALSE;
        pSceneOperate->ParameterVarValue[unitno][vargroupno][varno] = g_Unit[unitno].ParameterGroup[vargroupno].Var[varno].Value;
      }
    }
  }

  while(pSceneOperate->ScenePictureList.GetCount())
  {
    TagScenePictureStruct *pScenePicture = (TagScenePictureStruct *)pSceneOperate->ScenePictureList.RemoveHead();
    delete pScenePicture;
  }

  return TRUE;
}

BOOL WINAPI LoadSceneOperateFile(TagSceneOperateStruct *pSceneOperate,CString FileName)
{
  CWaitCursor WaitCursor;

	CFile file;
  CFileException fe;
	if(!file.Open(FileName, CFile::modeRead, &fe))
  {
    AfxMessageBox(FileName+" ��ȡ�ļ�ʧ��!",MB_SYSTEMMODAL);
    return FALSE;
  }

  ::ClearSceneOperate(pSceneOperate);

  CArchive ar(&file,CArchive::load);
  CString title = "";
  ar>>title;

  if(title=="SceneOperateFile 2.0")
  {
    ar>>pSceneOperate->SceneName;
    ar>>pSceneOperate->SceneDescribe;

    ar>>pSceneOperate->EnableFreeTrain;        //�����г�������Ч
    ar>>pSceneOperate->EnableSimTrain;         //�����г�������Ч
    ar>>pSceneOperate->EnableFaultTransmission;//���洫��������Ч
    ar>>pSceneOperate->EnableFreeTransmission; //���ɴ���������Ч

    ar>>pSceneOperate->InlineVoltage.Ua1;
    ar>>pSceneOperate->InlineVoltage.Ub1;
    ar>>pSceneOperate->InlineVoltage.Uc1;
    ar>>pSceneOperate->InlineVoltage.Ua2;
    ar>>pSceneOperate->InlineVoltage.Ub2;
    ar>>pSceneOperate->InlineVoltage.Uc2;
	  ar>>pSceneOperate->PTVoltage.StartupDelaySecond;

	  ar>>pSceneOperate->InlineVoltage.T1HiVoltageChangeRate;
	  ar>>pSceneOperate->InlineVoltage.T2HiVoltageChangeRate;
	  ar>>pSceneOperate->InlineVoltage.T1LoVoltageChangeRate;
	  ar>>pSceneOperate->InlineVoltage.T2LoVoltageChangeRate;
	  ar>>pSceneOperate->InlineVoltage.Reserved1;
	  ar>>pSceneOperate->InlineVoltage.Reserved2;

	  ar>>pSceneOperate->PTVoltage.T1HiCurrentMaxValue;
	  ar>>pSceneOperate->PTVoltage.T1HiVoltageValueK;
	  ar>>pSceneOperate->PTVoltage.T2HiCurrentMaxValue;
	  ar>>pSceneOperate->PTVoltage.T2HiVoltageValueK;
	  ar>>pSceneOperate->PTVoltage.T1LoCurrentMaxValue;
	  ar>>pSceneOperate->PTVoltage.T1LoVoltageValueK;
	  ar>>pSceneOperate->PTVoltage.T1Temp1;
	  ar>>pSceneOperate->PTVoltage.T1Temp2;
	  ar>>pSceneOperate->PTVoltage.T2Temp1;
	  ar>>pSceneOperate->PTVoltage.T2Temp2;
	  ar>>pSceneOperate->PTVoltage.T2LoCurrentMaxValue;
	  ar>>pSceneOperate->PTVoltage.T2LoVoltageValueK;
	  ar>>pSceneOperate->PTVoltage.T1Gear;
	  ar>>pSceneOperate->PTVoltage.T2Gear;
    ar>>pSceneOperate->PTVoltage.Reserved;
    
    if((pSceneOperate->PTVoltage.T1Gear<=0)||(pSceneOperate->PTVoltage.T1Gear>=21))
      pSceneOperate->PTVoltage.T1Gear = 7;
    if((pSceneOperate->PTVoltage.T2Gear<=0)||(pSceneOperate->PTVoltage.T2Gear>=21))
      pSceneOperate->PTVoltage.T2Gear = 7;
    if((pSceneOperate->PTVoltage.T1Temp1<=10)||(pSceneOperate->PTVoltage.T1Temp1>=100))
      pSceneOperate->PTVoltage.T1Temp1 = 35;
    if((pSceneOperate->PTVoltage.T1Temp2<=10)||(pSceneOperate->PTVoltage.T1Temp2>=100))
      pSceneOperate->PTVoltage.T1Temp2 = 35;
    if((pSceneOperate->PTVoltage.T2Temp1<=10)||(pSceneOperate->PTVoltage.T2Temp1>=100))
      pSceneOperate->PTVoltage.T2Temp1 = 35;
    if((pSceneOperate->PTVoltage.T2Temp2<=10)||(pSceneOperate->PTVoltage.T2Temp2>=100))
      pSceneOperate->PTVoltage.T2Temp2 = 35;

    ar>>pSceneOperate->Current.NoloadCurrentChangeMaxValue;
    ar>>pSceneOperate->Current.Reserved1;
    ar>>pSceneOperate->Current.Reserved2;
    ar>>pSceneOperate->Current.Reserved3;
    ar>>pSceneOperate->Current.Reserved4;
    ar>>pSceneOperate->Current.StartupDelaySecond;

    {
      ar>>pSceneOperate->Compensation[0].CompensationType;
	    ar>>pSceneOperate->Compensation[0].CompensationUnitID;
      for(int branchno=0;branchno<4;branchno++)
      {
        ar>>pSceneOperate->Compensation[0].CompensationBranchValueZ[branchno];
	      ar>>pSceneOperate->Compensation[0].CompensationBranchValueK[branchno];
        CString CompensationBranchVarI;
        CString CompensationBranchVarQ;
        CString CompensationBranchPowerPathVarList;
        ar>>CompensationBranchVarI;
        ar>>CompensationBranchVarQ;
        ar>>CompensationBranchPowerPathVarList;
        strcpy(pSceneOperate->Compensation[0].CompensationBranchVarI[branchno],CompensationBranchVarI.GetBuffer(0));
        strcpy(pSceneOperate->Compensation[0].CompensationBranchVarI[branchno],CompensationBranchVarI.GetBuffer(0));
        strcpy(pSceneOperate->Compensation[0].CompensationBranchPowerPathVarList[branchno],CompensationBranchPowerPathVarList.GetBuffer(0));
      }
    }
    {
      ar>>pSceneOperate->Compensation[1].CompensationType;
	    ar>>pSceneOperate->Compensation[1].CompensationUnitID;
      for(int branchno=0;branchno<4;branchno++)
      {
        ar>>pSceneOperate->Compensation[1].CompensationBranchValueZ[branchno];
	      ar>>pSceneOperate->Compensation[1].CompensationBranchValueK[branchno];
        CString CompensationBranchVarI;
        CString CompensationBranchVarQ;
        CString CompensationBranchPowerPathVarList;
        ar>>CompensationBranchVarI;
        ar>>CompensationBranchVarQ;
        ar>>CompensationBranchPowerPathVarList;
        strcpy(pSceneOperate->Compensation[1].CompensationBranchVarI[branchno],CompensationBranchVarI.GetBuffer(0));
        strcpy(pSceneOperate->Compensation[1].CompensationBranchVarI[branchno],CompensationBranchVarI.GetBuffer(0));
        strcpy(pSceneOperate->Compensation[1].CompensationBranchPowerPathVarList[branchno],CompensationBranchPowerPathVarList.GetBuffer(0));
      }
    }

    ar>>pSceneOperate->FreeTransmission.FeederUnitID;           //���ɴ������߱��
    ar>>pSceneOperate->FreeTransmission.AverCurrent;            //���ɴ�������ֵ
    ar>>pSceneOperate->FreeTransmission.AverAngle;              //���ɴ����Ƕ�ֵ
	  ar>>pSceneOperate->FreeTransmission.CurrentChangeRate;      //���ɴ�����������
	  ar>>pSceneOperate->FreeTransmission.StartupDelaySecond;     //���ɴ�������ʱ��
	  ar>>pSceneOperate->FreeTransmission.ContinuedSeconds;       //���ɴ�������ʱ��
    CString CurrentChangeRateList;
    ar>>CurrentChangeRateList;                                  //���ɴ��������仯��
    strcpy(pSceneOperate->FreeTransmission.CurrentChangeRateList,CurrentChangeRateList.GetBuffer(0));

    ar>>pSceneOperate->FreeTrain.InContinuedSeconds;            //�����г�ʻ��ʱ��
    ar>>pSceneOperate->FreeTrain.OutContinuedSeconds;           //�����г�ʻ��ʱ��
    ar>>pSceneOperate->FreeTrain.ChangeContinuedSeconds;        //�����г�����ʱ��
    ar>>pSceneOperate->FreeTrain.AverCurrent;                   //�����г�ƽ������ֵ
    ar>>pSceneOperate->FreeTrain.AverAngle;                     //�����г�ƽ���Ƕ�ֵ
    ar>>pSceneOperate->FreeTrain.InFeederUnitID;                //�����г�ʻ�����߱��
    ar>>pSceneOperate->FreeTrain.OutFeederUnitID;               //�����г�ʻ�����߱��
	  ar>>pSceneOperate->FreeTrain.CurrentChangeRate;             //�����г���������
	  ar>>pSceneOperate->FreeTrain.StartupDelaySecond;            //�����г�����ʱ��
    CString InCurrentChangeRateList;
    CString OutCurrentChangeRateList;
    ar>>InCurrentChangeRateList;                                //�����г�ʻ������仯�ʱ�
    ar>>OutCurrentChangeRateList;                               //�����г�ʻ�������仯�ʱ�
    strcpy(pSceneOperate->FreeTrain.InCurrentChangeRateList,InCurrentChangeRateList.GetBuffer(0));
    strcpy(pSceneOperate->FreeTrain.OutCurrentChangeRateList,OutCurrentChangeRateList.GetBuffer(0));

    CString SimTrainDataFileName;
    ar>>SimTrainDataFileName;                                   //�����г������ļ�����
    strcpy(pSceneOperate->SimTrain.SimTrainDataFileName,SimTrainDataFileName.GetBuffer(0));
	  ar>>pSceneOperate->SimTrain.StartupDelaySecond;         

    CString FaultTransmissionDataFileName;
    ar>>FaultTransmissionDataFileName;                                   //���ϴ��������ļ�����
    strcpy(pSceneOperate->FaultTransmission.FaultTransmissionDataFileName,FaultTransmissionDataFileName.GetBuffer(0));
	  ar>>pSceneOperate->FaultTransmission.StartupDelaySecond;

    WORD UnitCount;
    WORD ParameterGroupCount;
    WORD VarCount;
    WORD ParameterCount;
    ar>>UnitCount;
    ar>>ParameterGroupCount;
    ar>>VarCount;
    ar>>ParameterCount;

    for(int unitno=0;unitno<UnitCount;unitno++)
    {
      ar>>pSceneOperate->CurrentParamaterGroupID[unitno];
      for(int varno=0;varno<VarCount;varno++)
      {
        ar>>pSceneOperate->EnableVar[unitno][varno];
        ar>>pSceneOperate->VarValue[unitno][varno];
        ar>>pSceneOperate->VarLock[unitno][varno];
        ar>>pSceneOperate->VarAbnormal[unitno][varno];
        g_Unit[unitno].VarLock[varno] = pSceneOperate->VarLock[unitno][varno];;
        g_Unit[unitno].VarAbnormal[varno] = pSceneOperate->VarAbnormal[unitno][varno];;
      }

      for(int vargroupno=0;vargroupno<ParameterGroupCount;vargroupno++)
      {
        for(int varno=0;varno<ParameterCount;varno++)
        {
          ar>>pSceneOperate->EnableParameterVar[unitno][vargroupno][varno];
          ar>>pSceneOperate->ParameterVarValue[unitno][vargroupno][varno];
        }
      }
    }
  }
  if(title=="TestQuestionOperateFile 2.0")
  {
    ar>>pSceneOperate->SceneName;
    ar>>pSceneOperate->SceneDescribe;

    ar>>pSceneOperate->EnableFreeTrain;        //�����г�������Ч
    ar>>pSceneOperate->EnableSimTrain;         //�����г�������Ч
    ar>>pSceneOperate->EnableFaultTransmission;//���洫��������Ч
    ar>>pSceneOperate->EnableFreeTransmission; //���ɴ���������Ч

    ar>>pSceneOperate->InlineVoltage.Ua1;
    ar>>pSceneOperate->InlineVoltage.Ub1;
    ar>>pSceneOperate->InlineVoltage.Uc1;
    ar>>pSceneOperate->InlineVoltage.Ua2;
    ar>>pSceneOperate->InlineVoltage.Ub2;
    ar>>pSceneOperate->InlineVoltage.Uc2;
	  ar>>pSceneOperate->PTVoltage.StartupDelaySecond;

	  ar>>pSceneOperate->InlineVoltage.T1HiVoltageChangeRate;
	  ar>>pSceneOperate->InlineVoltage.T2HiVoltageChangeRate;
	  ar>>pSceneOperate->InlineVoltage.T1LoVoltageChangeRate;
	  ar>>pSceneOperate->InlineVoltage.T2LoVoltageChangeRate;
	  ar>>pSceneOperate->InlineVoltage.Reserved1;
	  ar>>pSceneOperate->InlineVoltage.Reserved2;

	  ar>>pSceneOperate->PTVoltage.T1HiCurrentMaxValue;
	  ar>>pSceneOperate->PTVoltage.T1HiVoltageValueK;
	  ar>>pSceneOperate->PTVoltage.T2HiCurrentMaxValue;
	  ar>>pSceneOperate->PTVoltage.T2HiVoltageValueK;
	  ar>>pSceneOperate->PTVoltage.T1LoCurrentMaxValue;
	  ar>>pSceneOperate->PTVoltage.T1LoVoltageValueK;
	  ar>>pSceneOperate->PTVoltage.T1Temp1;
	  ar>>pSceneOperate->PTVoltage.T1Temp2;
	  ar>>pSceneOperate->PTVoltage.T2Temp1;
	  ar>>pSceneOperate->PTVoltage.T2Temp2;
	  ar>>pSceneOperate->PTVoltage.T2LoCurrentMaxValue;
	  ar>>pSceneOperate->PTVoltage.T2LoVoltageValueK;
	  ar>>pSceneOperate->PTVoltage.T1Gear;
	  ar>>pSceneOperate->PTVoltage.T2Gear;
    ar>>pSceneOperate->PTVoltage.Reserved;
    
    if((pSceneOperate->PTVoltage.T1Gear<=0)||(pSceneOperate->PTVoltage.T1Gear>=21))
      pSceneOperate->PTVoltage.T1Gear = 7;
    if((pSceneOperate->PTVoltage.T2Gear<=0)||(pSceneOperate->PTVoltage.T2Gear>=21))
      pSceneOperate->PTVoltage.T2Gear = 7;
    if((pSceneOperate->PTVoltage.T1Temp1<=10)||(pSceneOperate->PTVoltage.T1Temp1>=100))
      pSceneOperate->PTVoltage.T1Temp1 = 35;
    if((pSceneOperate->PTVoltage.T1Temp2<=10)||(pSceneOperate->PTVoltage.T1Temp2>=100))
      pSceneOperate->PTVoltage.T1Temp2 = 35;
    if((pSceneOperate->PTVoltage.T2Temp1<=10)||(pSceneOperate->PTVoltage.T2Temp1>=100))
      pSceneOperate->PTVoltage.T2Temp1 = 35;
    if((pSceneOperate->PTVoltage.T2Temp2<=10)||(pSceneOperate->PTVoltage.T2Temp2>=100))
      pSceneOperate->PTVoltage.T2Temp2 = 35;

    ar>>pSceneOperate->Current.NoloadCurrentChangeMaxValue;
    ar>>pSceneOperate->Current.Reserved1;
    ar>>pSceneOperate->Current.Reserved2;
    ar>>pSceneOperate->Current.Reserved3;
    ar>>pSceneOperate->Current.Reserved3;
    ar>>pSceneOperate->Current.StartupDelaySecond;

    {
      ar>>pSceneOperate->Compensation[0].CompensationType;
	    ar>>pSceneOperate->Compensation[0].CompensationUnitID;
      for(int branchno=0;branchno<4;branchno++)
      {
        ar>>pSceneOperate->Compensation[0].CompensationBranchValueZ[branchno];
	      ar>>pSceneOperate->Compensation[0].CompensationBranchValueK[branchno];
        CString CompensationBranchVarI;
        CString CompensationBranchVarQ;
        CString CompensationBranchPowerPathVarList;
        ar>>CompensationBranchVarI;
        ar>>CompensationBranchVarQ;
        ar>>CompensationBranchPowerPathVarList;
        strcpy(pSceneOperate->Compensation[0].CompensationBranchVarI[branchno],CompensationBranchVarI.GetBuffer(0));
        strcpy(pSceneOperate->Compensation[0].CompensationBranchVarI[branchno],CompensationBranchVarI.GetBuffer(0));
        strcpy(pSceneOperate->Compensation[0].CompensationBranchPowerPathVarList[branchno],CompensationBranchPowerPathVarList.GetBuffer(0));
      }
    }
    {
      ar>>pSceneOperate->Compensation[1].CompensationType;
	    ar>>pSceneOperate->Compensation[1].CompensationUnitID;
      for(int branchno=0;branchno<4;branchno++)
      {
        ar>>pSceneOperate->Compensation[1].CompensationBranchValueZ[branchno];
	      ar>>pSceneOperate->Compensation[1].CompensationBranchValueK[branchno];
        CString CompensationBranchVarI;
        CString CompensationBranchVarQ;
        CString CompensationBranchPowerPathVarList;
        ar>>CompensationBranchVarI;
        ar>>CompensationBranchVarQ;
        ar>>CompensationBranchPowerPathVarList;
        strcpy(pSceneOperate->Compensation[1].CompensationBranchVarI[branchno],CompensationBranchVarI.GetBuffer(0));
        strcpy(pSceneOperate->Compensation[1].CompensationBranchVarI[branchno],CompensationBranchVarI.GetBuffer(0));
        strcpy(pSceneOperate->Compensation[1].CompensationBranchPowerPathVarList[branchno],CompensationBranchPowerPathVarList.GetBuffer(0));
      }
    }

    ar>>pSceneOperate->FreeTransmission.FeederUnitID;           //���ɴ������߱��
    ar>>pSceneOperate->FreeTransmission.AverCurrent;            //���ɴ�������ֵ
    ar>>pSceneOperate->FreeTransmission.AverAngle;              //���ɴ����Ƕ�ֵ
	  ar>>pSceneOperate->FreeTransmission.CurrentChangeRate;      //���ɴ�����������
	  ar>>pSceneOperate->FreeTransmission.StartupDelaySecond;     //���ɴ�������ʱ��
	  ar>>pSceneOperate->FreeTransmission.ContinuedSeconds;       //���ɴ�������ʱ��
    CString CurrentChangeRateList;
    ar>>CurrentChangeRateList;                                  //���ɴ��������仯��
    strcpy(pSceneOperate->FreeTransmission.CurrentChangeRateList,CurrentChangeRateList.GetBuffer(0));

    ar>>pSceneOperate->FreeTrain.InContinuedSeconds;            //�����г�ʻ��ʱ��
    ar>>pSceneOperate->FreeTrain.OutContinuedSeconds;           //�����г�ʻ��ʱ��
    ar>>pSceneOperate->FreeTrain.ChangeContinuedSeconds;        //�����г�����ʱ��
    ar>>pSceneOperate->FreeTrain.AverCurrent;                   //�����г�ƽ������ֵ
    ar>>pSceneOperate->FreeTrain.AverAngle;                     //�����г�ƽ���Ƕ�ֵ
    ar>>pSceneOperate->FreeTrain.InFeederUnitID;                //�����г�ʻ�����߱��
    ar>>pSceneOperate->FreeTrain.OutFeederUnitID;               //�����г�ʻ�����߱��
	  ar>>pSceneOperate->FreeTrain.CurrentChangeRate;             //�����г���������
	  ar>>pSceneOperate->FreeTrain.StartupDelaySecond;            //�����г�����ʱ��
    CString InCurrentChangeRateList;
    CString OutCurrentChangeRateList;
    ar>>InCurrentChangeRateList;                                //�����г�ʻ������仯�ʱ�
    ar>>OutCurrentChangeRateList;                               //�����г�ʻ�������仯�ʱ�
    strcpy(pSceneOperate->FreeTrain.InCurrentChangeRateList,InCurrentChangeRateList.GetBuffer(0));
    strcpy(pSceneOperate->FreeTrain.OutCurrentChangeRateList,OutCurrentChangeRateList.GetBuffer(0));

    CString SimTrainDataFileName;
    ar>>SimTrainDataFileName;                                   //�����г������ļ�����
    strcpy(pSceneOperate->SimTrain.SimTrainDataFileName,SimTrainDataFileName.GetBuffer(0));
	  ar>>pSceneOperate->SimTrain.StartupDelaySecond;         

    CString FaultTransmissionDataFileName;
    ar>>FaultTransmissionDataFileName;                                   //���ϴ��������ļ�����
    strcpy(pSceneOperate->FaultTransmission.FaultTransmissionDataFileName,FaultTransmissionDataFileName.GetBuffer(0));
	  ar>>pSceneOperate->FaultTransmission.StartupDelaySecond;

    WORD UnitCount;
    WORD ParameterGroupCount;
    WORD VarCount;
    WORD ParameterCount;
    ar>>UnitCount;
    ar>>ParameterGroupCount;
    ar>>VarCount;
    ar>>ParameterCount;

    for(int unitno=0;unitno<UnitCount;unitno++)
    {
      ar>>pSceneOperate->CurrentParamaterGroupID[unitno];
      for(int varno=0;varno<VarCount;varno++)
      {
        ar>>pSceneOperate->EnableVar[unitno][varno];
        ar>>pSceneOperate->VarValue[unitno][varno];
        ar>>pSceneOperate->VarLock[unitno][varno];
        ar>>pSceneOperate->VarAbnormal[unitno][varno];
        g_Unit[unitno].VarLock[varno] = pSceneOperate->VarLock[unitno][varno];;
        g_Unit[unitno].VarAbnormal[varno] = pSceneOperate->VarAbnormal[unitno][varno];;
        pSceneOperate->EnableVar[unitno][varno] = TRUE;
      }

      for(int vargroupno=0;vargroupno<ParameterGroupCount;vargroupno++)
      {
        for(int varno=0;varno<ParameterCount;varno++)
        {
          ar>>pSceneOperate->EnableParameterVar[unitno][vargroupno][varno];
          ar>>pSceneOperate->ParameterVarValue[unitno][vargroupno][varno];
        }
      }
    }

    WORD ScenePictureCount;
    ar>>ScenePictureCount;
    for(int pictureno=0;pictureno<ScenePictureCount;pictureno++)
    {
      TagScenePictureStruct *pScenePicture = new TagScenePictureStruct;
      pSceneOperate->ScenePictureList.AddTail(pScenePicture);
      ar>>pScenePicture->ScenePrompt;            //��ʾ
      ar>>pScenePicture->SceneNote;              //˵��
      ar>>pScenePicture->VarOperate;             //ң�Ų���
      ar>>pScenePicture->VarWait;                //ң�ŵȴ�
      ar>>pScenePicture->VarConfirm;             //ң��ȷ��
      ar>>pScenePicture->SceneMultiSelectOperate;//��ѡ����
      ar>>pScenePicture->SceneMessageShowOperate;//��ʾ����
      ar>>pScenePicture->SceneDelayOperate;      //��ʱ����
      ar>>pScenePicture->SceneParameterOperate;  //��ֵ����
      ar>>pScenePicture->SceneInlineVotlage;     //��ѹ����
      ar>>pScenePicture->ScenePTVotlage;         //ѹ������
      ar>>pScenePicture->SceneCurrent;           //��������
      ar>>pScenePicture->SceneCompensation;      //��������
      ar>>pScenePicture->SceneTransmission;      //��������
      ar>>pScenePicture->SceneTrain;             //�г�����
      ar>>pScenePicture->SceneFileLoad;          //��������

      ar>>pScenePicture->ScenePictureName[0];
      ar>>pScenePicture->ScenePictureName[1];
      ar>>pScenePicture->SceneVideoName;
      ar>>pScenePicture->SceneType;
      ar>>pScenePicture->SceneUnitID;
      ar>>pScenePicture->SceneVarID;
      ar>>pScenePicture->SceneTarge;
      ar>>pScenePicture->SceneOperateError;
      ar>>pScenePicture->SceneCorrectScore;
      ar>>pScenePicture->SceneErrorScore;
      ar>>pScenePicture->SceneOperateUsedSeconds;
    }
  }

  pSceneOperate->TotalScore = 0;
  pSceneOperate->CorrectScore = 0;

  ar.Flush();
  ar.Close();
  file.Close();
  
  return TRUE;
}

BOOL WINAPI SaveSceneOperateFile(TagSceneOperateStruct *pSceneOperate,CString FileName)
{
  return TRUE;
}

void WINAPI CreateShareMemory()
{
	g_hShareComputer=CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,sizeof(g_Unit),"simDataExchange"); 
  g_pShareComputer=(TagComputerShareStruct*)MapViewOfFile(g_hShareComputer,FILE_MAP_ALL_ACCESS,0,0,0); 
}

void WINAPI CloseShareMemory()
{
  UnmapViewOfFile(g_pShareComputer);
  CloseHandle(g_hShareComputer);
  g_hShareComputer = NULL;
  g_pShareComputer = NULL;
}

void WINAPI ReadUnitFromShareMemory()
{
  return;

  CSingleLock sLock(&g_ShareComputerMutex);
  sLock.Lock();

	if(g_hShareComputer&&g_pShareComputer)
  {
	  if(g_pShareComputer->simProtectionNewSOEReport)
    {
      g_pShareComputer->simProtectionNewSOEReport = FALSE;
    
      WORD UnitID = g_pShareComputer->SOEReport.UnitID;
      if(g_Unit[UnitID].UnitType>0)
      {
        TagSOEReportStruct * pSOEReportStruct = new TagSOEReportStruct;
        memcpy(pSOEReportStruct,&g_pShareComputer->SOEReport,sizeof(TagSOEReportStruct));

        g_Unit[UnitID].SOEReportList.AddTail(pSOEReportStruct);
        if(g_Unit[UnitID].SOEReportList.GetCount()>10)
          delete (TagSOEReportStruct *)g_Unit[UnitID].SOEReportList.RemoveHead();

        CSingleLock sLock(&g_SendSOEReportListMutex);
        sLock.Lock();
        g_SendSOEReportList.AddTail(pSOEReportStruct);
      }
    }

    if(g_pShareComputer->simProtectionNewData==TRUE)
    {
      g_pShareComputer->simProtectionNewData = FALSE;

      for(int unitno=0;unitno<UNITCOUNT;unitno++)
      {
        TagUnitStruct *pUnit = &g_Unit[unitno];
        TagUnitShareStruct *pShareUnit = &g_pShareComputer->CommunicationUnitList[pUnit->UnitID];

        pUnit->UnitState = pShareUnit->UnitState;
        pUnit->CurrentParamaterGroupID = pShareUnit->CurrentParamaterGroupID;
        pUnit->CurrentValue = pShareUnit->CurrentValue;
        pUnit->VotlageValue = pShareUnit->VotlageValue;
        pUnit->AngleValue = pShareUnit->AngleValue;
        pUnit->TransmissionState = pShareUnit->TransmissionState;

        {
          for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
          {
            for(int varno=0;varno<VARCOUNT;varno++)
              pUnit->VarGroup[vargroupno].Var[varno].Value = pShareUnit->VarGroup[vargroupno].Var[varno].Value;
          }
        }
        {
          for(int vargroupno=0;vargroupno<PARAMETERGROUPCOUNT;vargroupno++)
          {
            for(int varno=0;varno<PARAMETERCOUNT;varno++)
              pUnit->ParameterGroup[vargroupno].Var[varno].Value = pShareUnit->ParameterGroup[vargroupno].Var[varno].Value;
          }
        }
      }
    }
  }
}

void WINAPI WriteUnitToShareMemory()
{
  return;

  CSingleLock sLock(&g_ShareComputerMutex);
  sLock.Lock();

	if(g_hShareComputer&&g_pShareComputer)
  {
    g_pShareComputer->simCommunicationNewData = TRUE;

    for(int unitno=0;unitno<UNITCOUNT;unitno++)
    {
      TagUnitStruct *pUnit = &g_Unit[unitno];
      TagUnitShareStruct *pShareUnit = &g_pShareComputer->ProtectionUnitList[pUnit->UnitID];

      pShareUnit->UnitState  = pUnit->UnitState;
      pShareUnit->CurrentParamaterGroupID  = pUnit->CurrentParamaterGroupID;
      pShareUnit->CurrentValue  = pUnit->CurrentValue;
      pShareUnit->VotlageValue  = pUnit->VotlageValue;
      pShareUnit->AngleValue  = pUnit->AngleValue;
      pShareUnit->TransmissionState  = pUnit->TransmissionState;

      {
        for(int vargroupno=0;vargroupno<VARGROUPCOUNT;vargroupno++)
        {
          for(int varno=0;varno<VARCOUNT;varno++)
            pShareUnit->VarGroup[vargroupno].Var[varno].Value  = pUnit->VarGroup[vargroupno].Var[varno].Value;
        }
      }
      {
        for(int vargroupno=0;vargroupno<PARAMETERGROUPCOUNT;vargroupno++)
        {
          for(int varno=0;varno<PARAMETERCOUNT;varno++)
            pShareUnit->ParameterGroup[vargroupno].Var[varno].Value  = pUnit->ParameterGroup[vargroupno].Var[varno].Value;
        }
      }
    }
  }
}

void WINAPI WriteSOEReportToShareMemory(TagSOEReportStruct *pSOEReport)
{
  CSingleLock sLock(&g_ShareComputerMutex);
  sLock.Lock();

	if(g_hShareComputer&&g_pShareComputer)
  {
    g_pShareComputer->simCommunicationNewSOEReport = TRUE;
    
    memcpy(&g_pShareComputer->SOEReport,pSOEReport,sizeof(TagSOEReportStruct));
  }
}
