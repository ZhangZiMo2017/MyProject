#pragma once
#include "SD_IWorkFlow.h"


namespace SD_WORKFLOW
{
	UINT __stdcall InTrackThread(LPVOID lpParam);
	UINT __stdcall FailTrayThread(LPVOID lpParam);
	UINT __stdcall PassTrayThread(LPVOID lpParam);
	UINT __stdcall PickPutThread(LPVOID lpParam);
	UINT __stdcall AlarmThread(LPVOID lpParam);

	UINT __stdcall WorkFlowThread(LPVOID lpParam);
	UINT __stdcall SignalThread(LPVOID);

	class CWorkFlow : public CIWorkFlow
	{
	public:

		CWorkFlow();
		virtual ~CWorkFlow();

		void LoadIOInfomaion();

		BOOL OpenOutPort(LPCTSTR lpstrName);
		BOOL CloseOutPort(LPCTSTR lpstrName);

		int CheckOutPort(LPCTSTR lpstrName);
		int ReadInPort(LPCTSTR lpstrName);

		int TakeFlow(StepInfo &sStepInfo);
		void LoadPosInfo();

		void LoadConfig();
		void LoadProduct();
		BOOL ChangeProduct(LPCTSTR lpstrName);

		SIZE GetProductSize();
		SIZE GetTraySize();
		SIZE GetNgTraySize();

		//复位流程
		void F_ResetTray(StepInfo &sStepInfo);
		void F_ResetRobot(StepInfo &sStepInfo);
		void F_ResetFail(StepInfo &sStepInfo);

		//标定流程
		//标定相机与机械手
		void F_CalibCamera(StepInfo &sStepInfo);
		void F_CreatePickTool(StepInfo &sStepInfo);
		void F_CreateTool(StepInfo &sStepInfo);
		void F_CalibPutPos(LPCTSTR lpstrNameFormat);
		void F_CalibPutPos(const int iModel);
		void F_CalibPutPos(LPCTSTR lpstrNameFormat, const int iPosIndex, StepInfo &sStepInfo);
		void F_CalibPutPos(const int iModel, const int iPosIndex, StepInfo &sStepInfo);
		void F_CalibPos(LPCTSTR lpstrNameFormat, const int iPosIndex, StepInfo &sStepInfo);

		//运行流程
		void F_PickAndPut(StepInfo &sStepInfo);
		void F_FailTray(StepInfo &sStepInfo);
		void F_PassTray(StepInfo &sStepInfo);
		void F_EmptyTray(StepInfo &sStepInfo);
		void F_FullTray(StepInfo &sStepInfo);
		void F_InTrack(StepInfo &sStepInfo);

		void InTrack();
		void FailTray();
		void PassTray();
		void PickAndPut();
		void Alarm();
		void WorkFlow();
		void Signal();

		//
		BOOL Reset();
		BOOL Start();
		BOOL Suspend();
		BOOL Resume();
		BOOL Stop();
		BOOL IsRunning();
		BOOL IsSuspend();

		//Log
		void RecordLog(LPCTSTR lpstrData, const int iType);

		void GetAllIOStatus();

		int GetIOStatus(LPCTSTR lpstrName, LPCTSTR lpstrType);

		BOOL SetThreeLight(const int iLight);


		//
		int GetPosCount();
		BOOL GetPosValue(const int iIndex, LPTSTR lpstrName, double &dXPos, double &dYPos, double &dZPos, double &dUPos);

		int GetIOCount(LPCTSTR lpstrType);
		BOOL GetIOName(const int iIndex, LPCTSTR lpstrType, LPTSTR lpstrName);

		//UPH
		int GetHourPassCount();
		int GetHourFailCount();
		int GetHourOtherCount();
		int GetDayPassCount();
		int GetDayFailCount();
		int GetDayOtherCount();
		double GetDayYeild();
		double GetHourYeild();
		void AddPsCnt();
		void AddFiCnt();
		void AddOtherCnt();

		//robot
		BOOL GotoDistance(const LPCTSTR lpstrAxis, double dDistance, const int iMoveMode = 0);   //iMoveMode : 0. 轴运动 1.Jog运动
		BOOL IsRobotBusy(DWORD dwTimeOut = 20000);	
		BOOL JumpPosMod(RobotPosInfo sRobotPosInfo, const double dZHight = 600.0);
		BOOL JumpPos(const int iPosIndex, const double dZHight = 600);
		BOOL QryCurPos(double &dXPos, double &dYPos, double &dZPos, double &dUPos);
		BOOL StorePos(LPCTSTR lpstrPosName);
		BOOL StorePos(LPCTSTR lpstrPosName, const double dXPos, const double dYPos, const double dZPos, const double dUPos);
		BOOL JumpPos(LPCTSTR lpstrPosName);
		BOOL GotoPos(LPCTSTR lpstrPosName);
		int ChangeTool(const int iToolNo);
		BOOL IsRobotConnect();
		BOOL ConnectRobot();
		BOOL DisConnectRobot();
		BOOL CalibCamera();
		BOOL CalibPCB(const int iModel);
		BOOL SetSpeed(const int iSpeed);
		BOOL CalibPickTool();


		BOOL ConnectVision();
		BOOL IsVisionConnect();
		BOOL DisconnectVision();

		//GT Axis
		BOOL AxisMoveDis(const int iAxis, long lDistance);
		BOOL AxisMovePos(const int iAxis, long lPos);
		BOOL StoreTrayPos(const int iAxis);
		BOOL StoreOffSet(const int iAxis);
		BOOL CalibTrayHavePos(const int iAxis);
		long GetAxisPos(const int iAxis);

		LRESULT SendMsg(UINT Msg, WPARAM wParam, LPARAM lParam);
		LRESULT PostMsg(UINT Msg, WPARAM wParam, LPARAM lParam);
		LRESULT AlarmMsg(WPARAM wParam);
		void	ViewMsg(WPARAM wParam);
		void AlarmSetStep(StepInfo &sStepInfo, const int iAlaramNo, const int iYesStep, const int iCancelStep = -1);

		void GetCurrentProduct(LPTSTR lpstrName);
		void SetParent(HWND hWnd);

		int GetBoardRst(const int iIndex);

		BOOL GoHome(const int iAxis);
		void TrayNR();

		//
		BOOL SetEOpenPos();
		BOOL MoveToEOpenPos();
		BOOL SetFOpenPos();
		BOOL MoveToFOpenPos();
		BOOL SetECheckPos();
		BOOL MoveToECheckPos();
		BOOL SetFCheckPos();
		BOOL MoveToFCheckPos();
		BOOL SetFFullPos();
		BOOL SetEEmptyPos();
		int GetClassType();

	protected:
		int HexToInt(TCHAR tcHex);
		void InitClassInfo();
		void TransResult(const BYTE *pByData, const int iCaptureIndex);

	private:

		tstring m_strModuleDir;
		tstring m_strConfigDir;
		tstring m_strConfigIni;
		tstring m_strIOIni;
		tstring m_strProductName;
		tstring m_strProductDir;
		CIniFile m_classIniConfig;
		CIniFile m_classIniProduct;

		map<tstring, map<tstring, PlcInfo>> m_mapPlcInfo; 
		map<tstring, RobotPosInfo> m_mapRobotPosInfo; 
		map<tstring, RobotPosInfo> m_mapProductPosInfo; 
		map<tstring, vector<tstring>> m_mvIOName;
		vector<tstring> m_vecPosIndex;
		map<int, TKPosInfo> m_mapTKPosInfo;
		map<int, CaptrueInfo> m_mapCaptureInfo;
		vector<StepInfo *> m_vecStepInfo;

		CGoogolTech m_classGT;

		int m_iProductRow;
		int m_iProductCol;
		int m_iTrayRow;
		int m_iTrayCol;
		int m_iNGTrayRow;
		int m_iNGTrayCol;

		int m_iCaptrueCount;
		int m_iPickCircle;

		int m_iCalibPosIndex;
		int m_iPickPosIndex;	//取次数索引
		int m_iSuckerIndex;		//吸头位置索引
		int m_iCaptrueIndex;

		int m_iPutPassPos;
		int m_iPutFailPos;

		int m_iAddDis;

		int m_iCylCount;

		int m_aiTestResult[50];
		int m_aiResultTemp[50];
		BOOL m_bAllowPutPass;
		BOOL m_bAllowPutFail;
		BOOL m_bAllPick;
	

		long m_alOutValue[5];
		long m_alInValue[5];

		HWND m_hParaent;

		CIClient *m_pIVision;
		CIRobot *m_pIRobot;
		CIUphRecord *m_pIUphRecord;
		CAM_IFile *m_pIFile;

		//流程的Step
		StepInfo m_sInTrackFlow;
		StepInfo m_sPickAndPutFlow;
		StepInfo m_sEmptyTrayFlow;
		StepInfo m_sFullTrayFlow;
		StepInfo m_sFailTrayFlow;
		StepInfo m_sPassTrayFlow;

		//线程的标志
		BOOL m_bThread;
		BOOL m_bStart;
		BOOL m_bReset;
		BOOL m_bSuspend;
		BOOL m_bEngSuspend;
		int m_iSuspendCount;

		HANDLE m_ahThread[8];

		//Robot Param
		tstring m_strRobotIp;
		int m_iInitPort;
		int m_iCtrlPort;
		int m_iMovePort;
		int m_iVisionPort;

		//
		long m_lFTrayPos;
		long m_lFNoTrayOffset;
		long m_lFTrayOffset;
		long m_lFSensorPos;

		long m_lETrayOffset;
		long m_lETrayPos;
		long m_lESensorPos;
		

		//Calib
		double m_dAngle;

		//
		BOOL m_bAllowMoveToFull;
		BOOL m_bAllowMoveToEmpty;

		//
		CLock m_csVector;
		CLock m_csSuspend;
		CLock m_csStart;
		CLock m_csClass;

		double m_dYeildTarget;		//良率的预报警
		double m_dYeildSpec;		//良率报警
		double m_dSafeHigh;
		double m_dPutOKHigh;
		double m_dPutNgHigh;


		//
		BOOL m_bNULLRun;
	

		BOOL m_bSkipVision;		//跳过视觉
		BOOL m_bTrayNRun;

		double m_dPassPosHigh;
		double m_dFailPosHigh;

		int m_iClassType;
		int m_iClassPsCnt;
		int m_iClassNgCnt;
		int m_iClassNcCnt;
	};
}