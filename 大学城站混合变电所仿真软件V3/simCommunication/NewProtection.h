#define UNITCOUNT 32
#define VARGROUPCOUNT 3
#define VARCOUNT 64
#define PARAMETERGROUPCOUNT 4
#define PARAMETERCOUNT 128

#define PROTECTION_SWITCH_COUNT         32  //保护压板数量
#define PROTECTION_PARAMETER_COUNT      96  //保护定值数量
#define PROTECTION_ACTION_VALUE_COUNT   32  //保护动作值数量
#define WAVE_CHANNEL_COUNT              12  //保护采样通道数量
#define WAVE_SAMPLE_COUNT               320 //保护采样数量

struct TagUnitStruct;
struct TagVarGroupStruct;
struct TagVarStruct;
struct TagBypassFeedStruct;
struct TagSceneOperateStruct;
extern TagUnitStruct g_Unit[];
extern TagBypassFeedStruct g_UnitBypassFeed[];

extern CString g_SceneOperateFileName;
extern TagSceneOperateStruct g_SceneOperate;
extern int g_SceneOperateID;

extern CString defUnitType[];

extern float g_SimTrainData[UNITCOUNT][VARCOUNT][100];

extern void WINAPI CreatUnitList();
extern void WINAPI LoadUnitList();
extern void WINAPI SaveUnitList();
extern void WINAPI LoadParameterList();
extern void WINAPI SaveParameterList();

extern float WINAPI GetProtectionChannelMaxValue(float SampleWaveValue[]);
extern float WINAPI GetProtectionTrainMaxValue(float TrainDataValue[]);

extern BOOL WINAPI GetBypassFeedState(WORD UnitID);
extern BOOL WINAPI IsBypassFeed(WORD UnitID);

extern void WINAPI ProductActionValue(WORD UnitID,float ActionValue[VARCOUNT]);
extern void WINAPI ProductSOEReport(WORD UnitID,WORD SOEID,CTime SOETime,WORD SOEMSEL,WORD SOEValue,float ActionValue[VARCOUNT]);

extern CString WINAPI LoadSOEDefName(WORD UnitType,WORD SOEID);
extern BOOL    WINAPI LoadSOEDefActionValue(WORD UnitType,WORD SOEID);
extern CString WINAPI LoadActionValueDefName(WORD UnitType,WORD ActionValueID);

extern BOOL WINAPI ClearSceneOperate(TagSceneOperateStruct *pSceneOperate);
extern BOOL WINAPI StartupSceneOperate(TagSceneOperateStruct *pSceneOperate);
extern BOOL WINAPI LoadSceneOperateFile(TagSceneOperateStruct *pSceneOperate,CString FileName);
extern BOOL WINAPI SaveSceneOperateFile(TagSceneOperateStruct *pSceneOperate,CString FileName);

extern void WINAPI CreateShareMemory();
extern void WINAPI CloseShareMemory();
extern void WINAPI ReadUnitFromShareMemory();
extern void WINAPI WriteUnitToShareMemory();
extern void WINAPI WriteSOEReportToShareMemory(TagSOEReportStruct *pSOEReport);
