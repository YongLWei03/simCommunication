
#define PM_ADJUSTCLOCK                WM_USER+3     //时钟同步
#define PM_APPEXIT                    WM_USER+17    //退出
#define PM_SYSTEMREBOOT               WM_USER+18    //重启
#define PM_POWEROFF                   WM_USER+19    //关机
#define PM_REMOTE_RUN                 WM_USER+20    //远程启动
#define PM_SET_PANE_TEXT              WM_USER+73    //显示状态信息

#define PM_SEND_TESTLINK             WM_USER+800   //发送链路测试
#define PM_SEND_STARTUP_SIM_SYSTEM   WM_USER+801   //发送启动仿真系统

#define PM_SEND_FEED_TRANSMISION_BEGIN  WM_USER+810   //发送启动馈线传动
#define PM_SEND_FREE_TRAIN_BEGIN        WM_USER+811   //发送启动自由行车
#define PM_SEND_SIM_TRAIN_BEGIN         WM_USER+812   //发送启动仿真行车
#define PM_SEND_FAULT_TRANSMISION_BEGIN WM_USER+813   //发送启动故障传动
#define PM_SEND_POWER_CLEAR             WM_USER+814   //发送清除负荷
#define PM_SEND_SELECTED_POWER_ID       WM_USER+818   //牵引所仿真选择
#define PM_SEND_INLINE_VOTLAGE          WM_USER+819   //发送进线电压设置
#define PM_SEND_PT_VOTLAGE              WM_USER+820   //发送压互补偿设置
#define PM_SEND_CURRENT                 WM_USER+821   //发送负荷电流设置
#define PM_SEND_OPERATE_EANBLE_ID       WM_USER+822   //发送操作允许计算机编号
#define PM_SEND_STUDENT_OPERATE_INFO    WM_USER+823   //发送学员操作信息
#define PM_SEND_PAGEQUESTION_INFO       WM_USER+824   //发送学员考试信息
#define PM_SEND_STARTUP_SCENE_FILE      WM_USER+825   //发送启动场景文件
#define PM_SEND_END_SCENE_FILE          WM_USER+826   //发送终止场景文件
#define PM_SEND_STARTUP_PAGEQUESTION_FILE      WM_USER+827   //发送启动试卷文件
#define PM_SEND_END_PAGEQUESTION_FILE          WM_USER+828   //发送终止试卷文件
#define PM_SEND_STARTUP_EXAMINATION     WM_USER+832   //发送启动考试
#define PM_SEND_END_EXAMINATION         WM_USER+833   //发送终止考试
#define PM_SEND_COMPENSATION            WM_USER+834   //发送电容补偿设置

#define PM_CALL_ALLDATA              WM_USER+850   //召唤全数据
#define PM_SEND_ALLDATA              WM_USER+851   //发送全数据
#define PM_SEND_SOEDATA              WM_USER+852   //发送SOE
#define PM_SEND_REMOTEONOFF          WM_USER+853   //发送遥控分合
#define PM_SEND_REMOTEREVERT         WM_USER+854   //发送遥控复归
#define PM_CALL_PARAMETERGROUPID     WM_USER+855   //召唤定值组号
#define PM_SEND_PARAMETERGROUPID     WM_USER+856   //发送定值组号
#define PM_CALL_PARAMETER            WM_USER+857   //召唤定值
#define PM_SEND_PARAMETER            WM_USER+858   //发送定值

#define PM_FILESAVE_RECEIVE_CONFIRM   WM_USER+603   //文件保存接收确认
#define PM_FILESAVE_RECEIVE_FAIL      WM_USER+604   //文件保存接收失败
#define PM_FILEREMOVE_RECEIVE_CONFIRM WM_USER+605   //文件删除接收确认
#define PM_FILEREMOVE_RECEIVE_FAIL    WM_USER+606   //文件删除接收失败

#define PM_CALL_DISK_FILEINFO         WM_USER+612    //召唤磁盘文件信息
#define PM_SEND_DISK_FILEINFO         WM_USER+613    //发送磁盘文件信息
#define PM_CALL_DISK_COPY_FILEINFO    WM_USER+614    //召唤磁盘复制文件信息
#define PM_SEND_DISK_COPY_FILEINFO    WM_USER+615    //发送磁盘复制文件信息
#define PM_SEND_DISK_FILE_REMOVE      WM_USER+616    //磁盘文件删除
#define PM_CALL_DISK_FILE_PACKET      WM_USER+661    //召唤磁盘文件数据包
#define PM_SEND_DISK_FILE_PACKET      WM_USER+662    //发送磁盘文件数据包
