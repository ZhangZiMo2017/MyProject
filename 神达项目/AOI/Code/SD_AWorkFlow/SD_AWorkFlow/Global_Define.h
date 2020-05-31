#pragma once

#define SD_ROBOT_CAMERA_CAL_POS						_T("��е�����%d�ű궨λ��")
#define SD_ROBOT_CAMERA_CAL_VAL						_T("��е�����%d��ͼ������")
#define SD_ROBOT_CAMERA_PTRAY_POS					_T("PASSTray%d�ű궨λ��")
#define SD_ROBOT_CAMERA_PTOOL_POS					_T("PTOOLTray%d�ű궨λ��")
#define SD_ROBOT_CAMERA_NTRAY_POS					_T("NGTray%d�ű궨λ��")
#define SD_ROBOT_CAMERA_NTOOL_POS					_T("NTOOLTray%d�ű궨λ��")
#define SD_ROBOT_PASSTRAY_POS						_T("PASSTray%d��λ��")
#define SD_ROBOT_NGTRAY_POS							_T("NGTray%d��λ��")
#define SD_ROBOT_TRANSOK_POS						_T("��OK�����λ��")
#define SD_ROBOT_TRANSNG_POS						_T("��NG�����λ��")
#define SD_ROBOT_XBOARD_POS							_T("%d��X�嶪��λ")
#define SD_ROBOT_PICK_POS							_T("��%d��%d��ȡ��λ��")
#define SD_ROBOT_WAIT_POS							_T("�ȴ�λ��")
#define SD_ROBOT_SUCKER_TOOL						_T("%d����ͷ�궨λ��")


//IO
//OutPort
#define SD_GT_O_SUCKERVAC_OPEN						_T("����%d�����")
#define SD_GT_O_SUCKERVAC_CLOSE						_T("����%d�����")
#define SD_GT_O_SUCKERCYL_DOWN						_T("����%d�½�")

//InPort
#define SD_GT_I_SUCKERCYL_DOWN						_T("����%d�½���λ")
#define SD_GT_I_SUCKERCYL_UP						_T("����%d������λ")
#define SD_GT_I_SUCKERVAC							_T("����%d����")


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
	int iTakeMode;		// 0: ��ͷȫ��ͬʱȡ 1��������ͷһ��һ��ȡ
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
1=��е���ƶ�ʧ��,�����ƶ�(Yes) ֹͣ(No)
2=��е�ֵȴ��ƶ���ʱ �����ȴ�(Yes) ֹͣ(No) �����ƶ�(Cancel)
3=��ͷ��տ���ʧ�� ���¿���(Yes) ֹͣ(No) ����(Cancel)
4=��ͷ���״򿪸׿���ʧ�� ���¿���(Yes) ֹͣ(No) ����(Cancel)
5=��ͷ���׵�λ���ʧ�� ���¼��(Yes) ֹͣ(No) ����(Cancel)
6=��ͷ����ԭλ���ʧ�� ���¼��(Yes) ֹͣ(No) ����(Cancel)
7=��ͷ���׹رո׿���ʧ�� ���¿���(Yes) ֹͣ(No) ����(Cancel)
8=��ͷ��ռ��ʧ�� ���¼��(Yes) ֹͣ(No) ����(Cancel)
9=����Toolʧ�� ��������(Yes) ֹͣ(No)
10=�ȴ��������Pass�ѳ�ʱ  �����ȴ�(Yes) ֹͣ(No)
11=�л�Toolʧ�� �����л�(Yes) ֹͣ(No)
12=�ȴ��������Fail�ѳ�ʱ  �����ȴ�(Yes) ֹͣ(No)
13=Z1orZ2����ʧ��  
14=����ʱ:�Ƿ������ϴν���ʱ�������� ��(Yes) ��(NO)
15=��ǰ�����Ѿ����ڹ��ֵ,����ȷ��!
16=��ȫ�ű���!
17=NG�Ž�����!
18=����Ͽ��Ƶ�λ!
19=�ȴ����Խ����ʱ �����ȴ�(Yes) Ĭ��NoCode����(No)
20=�ȴ�[��תλ���Ƶ�λ]��ʱ ���鴫���� �����ȴ�(Yes) 
21=�ȴ�[��תλ���Ƶ�λ]��ʱ ���鴫���� �����ȴ�(Yes) 
22=�ȴ�[����λ����ɿ���λ]��ʱ ���鴫���� �����ȴ�(Yes) 
23=�ȴ�[����λ��мн���λ]��ʱ ���鴫���� �����ȴ�(Yes) 
24=�ȴ�[�ѵ�����λ1]��[�ѵ�����λ4]��ʱ ��������(Yes) �����ȴ�(Cancel) 
25=�ȴ�[�ѵ�����λ2]��[�ѵ�����λ4]��ʱ ��������(Yes)  �����ȴ�(Cancel) 
26=�ȴ�[�ѵ�����λ2]��[�ѵ�����λ3]��ʱ ��������(Yes)  �����ȴ�(Cancel) 
27=�ȴ�[�ѵ�����λ1]��[�ѵ�����λ3]��ʱ ��������(Yes)  �����ȴ�(Cancel)
28=�����̱��������������̺�ȷ���������!
29=��Ƭ��Ʒ�г��������������飡
30=Z1����Z2�����������������Ѿ�ֹͣ��
31=����û�з�ƽ����󣬵��ȷ����Ȼ��������ָ����У�
32=�����̵�������������Ƿ�źã�
33=�в�Ʒû���ҵ�����,��ȷ��!
34=��ͣ��ť�����£������Ѿ�ֹͣ! ����!
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
1=����ʱ:���¿�ʼ���������ϴν���ʱ�������� ���¿�ʼ(Yes) �����ϴ�(No)
2=ȷ��Ҫ�Ͽ���е�ֵ����� �Ͽ�(Yes) ȡ��(No)
3=��ǰ�����Ѿ�����Ŀ��ֵ,����!
4=��λ��
5=��λ���
6=��λʧ��
7=��ͣ��
8=������
10=������δ����
11=�Ƿ�Ҫ�жϳ��� �ж�(Yes) ȡ��(No)
12=�Ƿ�Ҫ��λ���� ��λ(Yes) ȡ��(No)
20=���Ͽ����˻������Ͽ���� �����
21=�Ӿ�δ���� ��������
22=���ȸ�λ����
23=���û�е��Power�����ƶ�ʧ��
24=��е�ֵȴ��ƶ���ʱ
25=NG�����Ѿ������������
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