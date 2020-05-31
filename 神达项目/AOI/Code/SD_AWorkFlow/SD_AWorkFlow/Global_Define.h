#pragma once

#define SD_ROBOT_CAMERA_CAL_POS						_T("机械手相机%d号标定位置")
#define SD_ROBOT_CAMERA_CAL_VAL						_T("机械手相机%d号图像坐标")
#define SD_ROBOT_CAMERA_PTRAY_POS					_T("PASSTray%d号标定位置")
#define SD_ROBOT_CAMERA_PTOOL_POS					_T("PTOOLTray%d号标定位置")
#define SD_ROBOT_CAMERA_NTRAY_POS					_T("NGTray%d号标定位置")
#define SD_ROBOT_CAMERA_NTOOL_POS					_T("NTOOLTray%d号标定位置")
#define SD_ROBOT_PASSTRAY_POS						_T("PASSTray%d号位置")
#define SD_ROBOT_NGTRAY_POS							_T("NGTray%d号位置")
#define SD_ROBOT_TRANSOK_POS						_T("放OK板过渡位置")
#define SD_ROBOT_TRANSNG_POS						_T("放NG板过渡位置")
#define SD_ROBOT_XBOARD_POS							_T("%d号X板丢弃位")
#define SD_ROBOT_PICK_POS							_T("第%d次%d号取板位置")
#define SD_ROBOT_WAIT_POS							_T("等待位置")
#define SD_ROBOT_SUCKER_TOOL						_T("%d号吸头标定位置")


//IO
//OutPort
#define SD_GT_O_SUCKERVAC_OPEN						_T("吸盘%d开真空")
#define SD_GT_O_SUCKERVAC_CLOSE						_T("吸盘%d关真空")
#define SD_GT_O_SUCKERCYL_DOWN						_T("吸盘%d下降")

//InPort
#define SD_GT_I_SUCKERCYL_DOWN						_T("吸盘%d下降到位")
#define SD_GT_I_SUCKERCYL_UP						_T("吸盘%d上升到位")
#define SD_GT_I_SUCKERVAC							_T("吸盘%d有料")


//Step
#define STEP_PICKANDPUT								100
#define STEP_EMPTTRAY								101
#define STEP_FULLTRAY								102
#define STEP_INTRACK								103
#define STEP_FAILTRAY								104
#define STEP_PASSRAY								105

#define L_ALLCLOSE									0
#define L_ALLOPEN									1
#define L_RED										2
#define L_YEL										3
#define L_GRN										4

#define ROBOT _T("Robot")
#define RPOSITION _T("Position")

#define SD_RESET_SUC 99999
#define SD_RESET_FAIL 99998

#define CYL_TIMEOUT  20000
#define ROBOT_TIMEOUT 10000

#define HOME_OFFSET -20000
#define SEARCH_HOME -400000

#define ROBOT_SAFEHIGH 0.0


#define WM_ULMSG_BEGIN (WM_USER + 0x100)

#define WM_ULMSG_IOSTATUS (WM_ULMSG_BEGIN + 1)
#define WM_ULMSG_LOCALTIME (WM_ULMSG_BEGIN + 2)
#define WM_ULMSG_UPDATAUPH (WM_ULMSG_BEGIN + 3)
#define WM_ULMSG_PUTOKP0S (WM_ULMSG_BEGIN + 4)
#define WM_ULMSG_PUTNGP0S (WM_ULMSG_BEGIN + 5)
#define WM_ULMSG_UPDATARST (WM_ULMSG_BEGIN + 6)
#define WM_ULMSG_RESETPCB (WM_ULMSG_BEGIN + 7)
#define WM_ULMSG_PICKRSTPOS (WM_ULMSG_BEGIN + 8)
#define WM_ULMSG_WARNING (WM_ULMSG_BEGIN + 9)
#define WM_ULMSG_STOP (WM_ULMSG_BEGIN + 10)
#define WM_ULMSG_START (WM_ULMSG_BEGIN + 11)
#define WM_ULMSG_SUSPEND (WM_ULMSG_BEGIN + 12)
#define WM_ULMSG_RESET (WM_ULMSG_BEGIN + 13)
#define WM_ULMSG_INFORMATION (WM_ULMSG_BEGIN + 14)
#define WM_ULMSG_INFORMATION_V (WM_ULMSG_BEGIN + 15)
#define WM_ULMSG_CHANGEPORDUCT (WM_ULMSG_BEGIN + 16)


typedef struct PLCINFO
{
	int iStation;
	int iPortNo;
	int iCardNo;
	int iHexNo;
	PLCINFO ()
	{
		iHexNo = 0;
		iStation = 1;
		iPortNo = 0;
		iCardNo = 0;
	}
}PlcInfo, *PPlcInfo;

typedef struct STEPINFO
{
private:
	int iLastStep;
	int iSubStep;
	int iLastMainStep;
	int iMainStep;
	DWORD dwSTTime;
	BOOL bNoSuspend;
public:
	STEPINFO()
	{
		Init();
	}

	void Init()
	{
		iLastStep = 0;
		iSubStep = 0;
		iMainStep = 0;
		iLastMainStep = 0;
		dwSTTime = 0;
		bNoSuspend = FALSE;
	}

	void SetSubStep(const int iSubStep)
	{
		iLastStep = iSubStep;
		this->iSubStep = iSubStep;
	}

	void SetMainSubStep(const int iMainStep, const int iSubStep)
	{
		iLastMainStep = this->iMainStep;
		this->iMainStep = iMainStep;
		SetSubStep(iSubStep);
	}

	void UpdataStTime()
	{
		dwSTTime = ::GetTickCount();
	}

	BOOL IsTimeOut(const DWORD dwTimeOut = 1000)
	{
		BOOL bRet = FALSE;

		if (::GetTickCount() - dwSTTime > dwTimeOut)
		{
			bRet = TRUE;
		}

		return bRet;
	}

	int GetMainStep()
	{
		return iMainStep;
	}

	int GetSubStep()
	{
		return iSubStep;
	}

	void SetNoSuspend(const BOOL bNoSuspend)
	{
		this->bNoSuspend = bNoSuspend;
	}

	BOOL IsNoSupsend()
	{
		return bNoSuspend;
	}

}StepInfo, *PStepInfo;

class CLock
{
public: 
	CLock()
	{
		::InitializeCriticalSection(&m_csLock);
	}

	~CLock()
	{
		::DeleteCriticalSection(&m_csLock);
	}

	void Lock()
	{
		::EnterCriticalSection(&m_csLock);
	}

	void Unlock()

	{
		::LeaveCriticalSection(&m_csLock);	
	}

private:
	CRITICAL_SECTION m_csLock;
};

class CNullLock
{
public: 
	CNullLock()
	{

	}

	~CNullLock()
	{

	}

	void Lock()
	{

	}

	void Unlock()

	{

	}

};


// void DoEvent()
// {
// 	MSG msg;
// 	// Process existing messages in the application's message queue.
// 	// When the queue is empty, do clean up and return.
// 	// 	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
// 	// 	{
// 	// 		if ( !AfxGetThread()->PumpMessage() )
// 	// 		{
// 	// 			return;
// 	// 		}
// 	// 	}
// 
// 	if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
// 	{
// 		DispatchMessage(&msg);
// 		TranslateMessage(&msg);
// 	}
// 
// }
// 
// void WaitDoevent(const DWORD &delaytime)
// {
// 	long Starttime,EndTime;
// 	Starttime=GetTickCount();
// 	EndTime=GetTickCount();
// 	for(;labs(EndTime-Starttime)<=labs(delaytime);)
// 	{
// 		EndTime=GetTickCount();
// 		DoEvent();
// 	}
// }

typedef struct TOOLSET
{
	double dXPos;
	double dYPos;
	double dUPos;

	TOOLSET ()
	{
		dXPos = 0.0;
		dYPos = 0.0;
		dUPos = 0.0;
	}

	void Init()
	{
		dXPos = 0.0;
		dYPos = 0.0;
		dUPos = 0.0;
	}

}ToolSet, *PToolSet;


typedef struct TKPOSINFO
{
	int iTakeMode;		// 0: 吸头全部同时取 1：单个吸头一个一个取
	ToolSet asToolSet[4];
	int aiSuckerToPos[4];
	int iTkCount;

	TKPOSINFO()
	{
		iTakeMode = 0;
		iTkCount = 0;

		for (int i = 0; i < 4; ++i)
		{
			asToolSet[i].Init();
			aiSuckerToPos[i] = -1;
		}
	}
}TKPosInfo, *PTKPosInfo;

typedef struct CAPTRUEINFO
{
	int iProductCount;
	int iRow;
	int iCol;
	int aiProductNo[30];

	CAPTRUEINFO()
	{
		iProductCount = 1;
		iRow = 1;
		iCol = 1;
		for (int i = 0; i < 30; ++i)
		{
			aiProductNo[i] = -1;
		}
	}
}CaptrueInfo, *PCaptrueInfo;





//
/*
1=机械手移动失败,重新移动(Yes) 停止(No)
2=机械手等待移动超时 继续等待(Yes) 停止(No) 重新移动(Cancel)
3=吸头真空控制失败 重新控制(Yes) 停止(No) 忽略(Cancel)
4=吸头气缸打开缸控制失败 重新控制(Yes) 停止(No) 忽略(Cancel)
5=吸头气缸到位检测失败 重新检测(Yes) 停止(No) 忽略(Cancel)
6=吸头气缸原位检测失败 重新检测(Yes) 停止(No) 忽略(Cancel)
7=吸头气缸关闭缸控制失败 重新控制(Yes) 停止(No) 忽略(Cancel)
8=吸头真空检测失败 重新检测(Yes) 停止(No) 忽略(Cancel)
9=设置Tool失败 重新设置(Yes) 停止(No)
10=等待放允许放Pass把超时  继续等待(Yes) 停止(No)
11=切换Tool失败 重新切换(Yes) 停止(No)
12=等待放允许放Fail把超时  继续等待(Yes) 停止(No)
13=Z1orZ2回零失败  
14=启动时:是否延续上次结束时继续生产 是(Yes) 否(NO)
15=当前良率已经低于规格值,请检查确认!
16=安全门被打开!
17=NG门禁被打开!
18=请把料框推到位!
19=等待测试结果超时 继续等待(Yes) 默认NoCode继续(No)
20=等待[中转位左移到位]超时 请检查传感器 继续等待(Yes) 
21=等待[中转位右移到位]超时 请检查传感器 继续等待(Yes) 
22=等待[收料位侧夹松开到位]超时 请检查传感器 继续等待(Yes) 
23=等待[收料位侧夹夹紧到位]超时 请检查传感器 继续等待(Yes) 
24=等待[已到拍照位1]或[已到拍照位4]超时 继续运行(Yes) 继续等待(Cancel) 
25=等待[已到拍照位2]或[已到拍照位4]超时 继续运行(Yes)  继续等待(Cancel) 
26=等待[已到拍照位2]或[已到拍照位3]超时 继续运行(Yes)  继续等待(Cancel) 
27=等待[已到拍照位1]或[已到拍照位3]超时 继续运行(Yes)  继续等待(Cancel)
28=满料盘被带过来，请料盘后确定清除报警!
29=整片产品中超过不良个数请检查！
30=Z1或者Z2驱动器报警。程序已经停止！
31=物料没有放平请检查后，点击确定，然后点击界面恢复运行！
32=空料盘掉落请检查空料盘是否放好！
33=有产品没有找到条码,请确认!
34=急停按钮被按下，程序已经停止! 请检查!
*/

#define ALARM_ROBOTMOVE				1
#define ALARM_ROBOTWAIT				2
#define ALARM_SUCKERVACCTRL			3
#define ALARM_SUCKERCYLOPEN			4
#define ALARM_SUCKERCYLCLOSE		7
#define ALARM_SUCKERCYLDOWN			5
#define ALARM_SUCKERCYLUP			6
#define ALARM_SUCKERVACCHECK		8
#define ALARM_TOOLSET				9
#define ALARM_WAITPUTPASS			10
#define ALARM_CHANGETOOL			11
#define ALARM_WAITPUTFAIL			12
#define ALARM_GOHOMEFAIL			13
#define ALARM_UPHYIELDLOW			15
#define ALARM_SAFEDOOR				16
#define ALARM_NGDOOR				17
#define ALARM_PASSTRAY				18
#define ALARM_WAITRSTTIMEOUT		19

/*
1=启动时:重新开始或者延续上次结束时继续生产 重新开始(Yes) 延续上次(No)
2=确定要断开机械手的连接 断开(Yes) 取消(No)
3=当前良率已经低于目标值,请检查!
4=复位中
5=复位完成
6=复位失败
7=暂停中
8=运行中
10=机器人未连接
11=是否要中断程序？ 中断(Yes) 取消(No)
12=是否要复位程序？ 复位(Yes) 取消(No)
20=工料框满了或者收料框空了 请更换
21=视觉未连接 请点击连接
22=请先复位程序
23=设置机械手Power或者移动失败
24=机械手等待移动超时
25=NG料盘已经放满，请更换
*/
#define INFO_CONTINUEINFO			1
#define INFO_UPHLOWTARGET			3
#define INFO_RESET					4
#define INFO_RESET_COM				5
#define INFO_RESET_FAIL				6
#define INFO_SUSPEND				7
#define INFO_RUNNING				8
#define INFO_ROBOTDISCONNECT		10
#define INFO_WANTSTOP				11
#define INFO_WANTRESET				12

#define INFO_BOXFULLOREMPTY			20
#define INFO_VISIONDISCONNECT		21
#define INFO_NORESETCOM				22
#define INFO_ROBOTSETFAIL			23
#define INFO_ROBOTWAIT				24
#define INFO_FAILTRAYFULL			25