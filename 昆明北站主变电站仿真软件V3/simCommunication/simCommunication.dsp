# Microsoft Developer Studio Project File - Name="simCommunication" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=simCommunication - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "simCommunication.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "simCommunication.mak" CFG="simCommunication - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "simCommunication - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "simCommunication - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "simCommunication - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /out:"C:\simPower2000\昆明北站主变电站仿真系统\simCommunication.exe"

!ELSEIF  "$(CFG)" == "simCommunication - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"C:\simPower2000\昆明北站主变电站仿真系统\simCommunication.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "simCommunication - Win32 Release"
# Name "simCommunication - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CommThread.cpp
# End Source File
# Begin Source File

SOURCE=.\DataExchangeSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCurrentDistribution.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgEditVar.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFaultTransmission.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFreeTransmission.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFreeTransmissionList.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgInlineVotlage.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgListSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOperateParameter.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgOperateUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\DLgParameterInput.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgParameterOnOff.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPowerMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPowerThumbnail.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgPTVotlage.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgReplace.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSceneFileSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgUnitEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\global.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\NewComputer.cpp
# End Source File
# Begin Source File

SOURCE=.\NewProtection.cpp
# End Source File
# Begin Source File

SOURCE=.\NewSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\NewString.cpp
# End Source File
# Begin Source File

SOURCE=.\PowerThread.cpp
# End Source File
# Begin Source File

SOURCE=.\ProtectionCalculater.cpp
# End Source File
# Begin Source File

SOURCE=.\ProtocolTH104.cpp
# End Source File
# Begin Source File

SOURCE=.\simCommunication.cpp
# End Source File
# Begin Source File

SOURCE=.\simCommunication.rc
# End Source File
# Begin Source File

SOURCE=.\simCommunicationSOEListView.cpp
# End Source File
# Begin Source File

SOURCE=.\simCommunicationUnitListView.cpp
# End Source File
# Begin Source File

SOURCE=.\simCommunicationVarListView.cpp
# End Source File
# Begin Source File

SOURCE=.\Sortclass.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Systemtray.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CommThread.h
# End Source File
# Begin Source File

SOURCE=.\DataExchangeSocket.h
# End Source File
# Begin Source File

SOURCE=.\DlgCurrent.h
# End Source File
# Begin Source File

SOURCE=.\DlgCurrentDistribution.h
# End Source File
# Begin Source File

SOURCE=.\DlgEditVar.h
# End Source File
# Begin Source File

SOURCE=.\DlgFaultTransmission.h
# End Source File
# Begin Source File

SOURCE=.\DlgFreeTrain.h
# End Source File
# Begin Source File

SOURCE=.\DlgFreeTrainList.h
# End Source File
# Begin Source File

SOURCE=.\DlgFreeTransmission.h
# End Source File
# Begin Source File

SOURCE=.\DlgFreeTransmissionList.h
# End Source File
# Begin Source File

SOURCE=.\DlgInlineVotlage.h
# End Source File
# Begin Source File

SOURCE=.\DlgListSelect.h
# End Source File
# Begin Source File

SOURCE=.\DlgMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\DlgOperateParameter.h
# End Source File
# Begin Source File

SOURCE=.\DlgOperateUnit.h
# End Source File
# Begin Source File

SOURCE=.\DLgParameterInput.h
# End Source File
# Begin Source File

SOURCE=.\DlgParameterOnOff.h
# End Source File
# Begin Source File

SOURCE=.\DlgPowerMonitor.h
# End Source File
# Begin Source File

SOURCE=.\DlgPowerThumbnail.h
# End Source File
# Begin Source File

SOURCE=.\DlgPTVotlage.h
# End Source File
# Begin Source File

SOURCE=.\DlgReplace.h
# End Source File
# Begin Source File

SOURCE=.\DlgSceneFileSelect.h
# End Source File
# Begin Source File

SOURCE=.\DlgScenePicture.h
# End Source File
# Begin Source File

SOURCE=.\DlgSimTrain.h
# End Source File
# Begin Source File

SOURCE=.\DlgUnitEdit.h
# End Source File
# Begin Source File

SOURCE=.\global.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Message.h
# End Source File
# Begin Source File

SOURCE=.\NewComputer.h
# End Source File
# Begin Source File

SOURCE=.\NewProtection.h
# End Source File
# Begin Source File

SOURCE=.\NewSocket.h
# End Source File
# Begin Source File

SOURCE=.\NewString.h
# End Source File
# Begin Source File

SOURCE=.\PowerThread.h
# End Source File
# Begin Source File

SOURCE=.\ProtectionCalculater.h
# End Source File
# Begin Source File

SOURCE=.\ProtocolTH104.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\simCommunication.h
# End Source File
# Begin Source File

SOURCE=.\simCommunicationSOEListView.h
# End Source File
# Begin Source File

SOURCE=.\simCommunicationUnitListView.h
# End Source File
# Begin Source File

SOURCE=.\simCommunicationVarListView.h
# End Source File
# Begin Source File

SOURCE=.\Sortclass.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Systemtray.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\fault_da.ico
# End Source File
# Begin Source File

SOURCE=.\res\file.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00002.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00003.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico00004.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_powe.ico
# End Source File
# Begin Source File

SOURCE=.\res\nodrag.cur
# End Source File
# Begin Source File

SOURCE=.\res\PUSH.CUR
# End Source File
# Begin Source File

SOURCE=.\res\simCommunication.ico
# End Source File
# Begin Source File

SOURCE=.\res\simCommunication.rc2
# End Source File
# Begin Source File

SOURCE=.\res\simCommunicationDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\train_da.ico
# End Source File
# Begin Source File

SOURCE=.\res\unit.ico
# End Source File
# Begin Source File

SOURCE=.\res\unitdisa.ico
# End Source File
# Begin Source File

SOURCE=.\res\unitfaul.ico
# End Source File
# Begin Source File

SOURCE=.\res\var.ico
# End Source File
# Begin Source File

SOURCE=.\res\vargroup.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
