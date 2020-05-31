#pragma once
#ifdef SD_AWORKFLOW_EXPORTS
#define SD_WORKFLOW_API __declspec(dllexport)
#else
#define SD_WORKFLOW_API __declspec(dllimport)
#endif

class CIWorkFlow
{
public:

	CIWorkFlow() {};
	virtual ~CIWorkFlow() {};

	virtual void LoadIOInfomaion() = 0;

	virtual BOOL OpenOutPort(LPCTSTR lpstrName) = 0;
	virtual BOOL CloseOutPort(LPCTSTR lpstrName) = 0;

	virtual int CheckOutPort(LPCTSTR lpstrName) = 0;
	virtual int ReadInPort(LPCTSTR lpstrName) = 0;

//	virtual int TakeFlow(StepInfo &sStepInfo) = 0;
	virtual void LoadPosInfo() = 0;

	virtual void LoadConfig() = 0;
	virtual void LoadProduct() = 0;
	virtual BOOL ChangeProduct(LPCTSTR lpstrName) = 0;

	virtual SIZE GetProductSize() = 0;
	virtual SIZE GetTraySize() = 0;
	virtual SIZE GetNgTraySize() = 0;

	virtual void GetAllIOStatus() = 0;

	virtual int GetIOStatus(LPCTSTR lpstrName, LPCTSTR lpstrType) = 0;
	//
	virtual int GetPosCount() = 0;
	virtual BOOL GetPosValue(const int iIndex, LPTSTR lpstrName, double &dXPos, double &dYPos, double &dZPos, double &dUPos) = 0;
	virtual int GetIOCount(LPCTSTR lpstrType) = 0;
	virtual BOOL GetIOName(const int iIndex, LPCTSTR lpstrType, LPTSTR lpstrName) = 0;

	//UPH
	virtual int GetHourPassCount() = 0;
	virtual int GetHourFailCount() = 0;
	virtual int GetHourOtherCount() = 0;
	virtual int GetDayPassCount() = 0;
	virtual int GetDayFailCount() = 0;
	virtual int GetDayOtherCount() = 0;
	virtual double GetDayYeild() = 0;
	virtual double GetHourYeild() = 0;

	//Robot
	virtual BOOL GotoDistance(const LPCTSTR lpstrAxis, double dDistance, const int iMoveMode = 0) = 0;   //iMoveMode : 0. 轴运动 1.Jog运动
	virtual BOOL IsRobotBusy(DWORD dwTimeOut = 20000) = 0;	
	virtual BOOL QryCurPos(double &dXPos, double &dYPos, double &dZPos, double &dUPos) = 0;
	virtual BOOL StorePos(LPCTSTR lpstrPosName) = 0;
	virtual BOOL StorePos(LPCTSTR lpstrPosName, const double dXPos, const double dYPos, const double dZPos, const double dUPos) = 0;
	virtual BOOL JumpPos(LPCTSTR lpstrPosName) = 0;
	virtual int ChangeTool(const int iToolNo) = 0;
	virtual BOOL IsRobotConnect() = 0;
	virtual BOOL ConnectRobot() = 0;
	virtual BOOL DisConnectRobot() = 0;
	virtual BOOL SetSpeed(const int iSpeed) = 0;

	virtual BOOL ConnectVision() = 0;
	virtual BOOL IsVisionConnect() = 0;
	virtual BOOL DisconnectVision() = 0;

	virtual BOOL CalibCamera() = 0;
	virtual BOOL CalibPCB(const int iModel) = 0;
	virtual BOOL CalibPickTool() = 0;

	//GT 轴控制
	virtual BOOL AxisMoveDis(const int iAxis, long lDistance) = 0;
	virtual BOOL StoreTrayPos(const int iAxis) = 0;
	virtual BOOL StoreOffSet(const int iAxis) = 0;
	virtual BOOL CalibTrayHavePos(const int iAxis) = 0;
	virtual long GetAxisPos(const int iAxis) = 0;

	virtual void RecordLog(LPCTSTR lpstrData, const int iType) = 0;

	//
	virtual BOOL Reset() = 0;
	virtual BOOL Start() = 0;
	virtual BOOL Suspend() = 0;
	virtual BOOL Resume() = 0;
	virtual BOOL Stop() = 0;
	virtual BOOL IsRunning() = 0;
	virtual BOOL IsSuspend() = 0;

	virtual void GetCurrentProduct(LPTSTR lpstrName) = 0;
	virtual void SetParent(HWND hWnd) = 0;

	virtual int GetBoardRst(const int iIndex) = 0;
	virtual BOOL GoHome(const int iAxis) = 0;
	virtual void TrayNR() = 0;

	virtual BOOL SetEOpenPos() = 0;
	virtual BOOL MoveToEOpenPos() = 0;
	virtual BOOL SetFOpenPos() = 0;
	virtual BOOL MoveToFOpenPos() = 0;
	virtual BOOL SetECheckPos() = 0;
	virtual BOOL MoveToECheckPos() = 0;
	virtual BOOL SetFCheckPos() = 0;
	virtual BOOL MoveToFCheckPos() = 0;
	virtual BOOL SetFFullPos() = 0;
	virtual BOOL SetEEmptyPos() = 0;
	virtual int GetClassType() = 0;
};

extern "C"
{
	SD_WORKFLOW_API void CreateWorkFlow(CIWorkFlow **ppWorkFlow);
	SD_WORKFLOW_API void ReleaseWorkFlow(CIWorkFlow *pWorkFlow);
};