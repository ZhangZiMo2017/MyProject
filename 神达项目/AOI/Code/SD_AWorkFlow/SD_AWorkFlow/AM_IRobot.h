#pragma once
#ifdef AM_ROBOT_EXPORTS
#define AM_ROBOT_API __declspec(dllexport)
#else
#define AM_ROBOT_API __declspec(dllimport)
#endif


typedef struct ROBOTPOSINFO
{
	float	dXPos;
	float	dYPos;
	float	dZPos;
	float  dUPos;
	float  dVPos;
	float	dWPos;
	int		nHand;			//1:Right Hand 2: Left Hand
	int		nElbow;
	int		nFlip;
	int		nJ4Flag;
	int		nJ6Flag;
	int		nRunMod;
	ROBOTPOSINFO()
	{
		dXPos = 0.0;
		dYPos = 0.0;
		dZPos = 0.0;
		dUPos = 0.0;
		dVPos = 0.0;
		dWPos = 0.0;
		nHand = 2;
		nElbow = 1;
		nFlip = 1;
		nJ4Flag = 0;
		nJ6Flag = 0;
		nRunMod = 0;
	}
}RobotPosInfo;

class CIRobot
{
public:
	CIRobot() {};
	virtual ~CIRobot() {};

	virtual void				SetRobotIp(const LPCTSTR lpStrRobotIp) = 0;
	virtual void				SetInitPort(const int iInitPort = 5000) = 0;
	virtual void				SetMovePort(const int iMovePort = 4000) = 0;
	virtual void				SetCtrlPort(const int iCtrlPort = 4001) = 0;
	virtual void				SetVisionPort(const int iVisionPort = 4010) = 0;
	virtual void				SetEndChar(const TCHAR *pEndChar, const int iEndCharCnt = 1) = 0;

	//运动
	virtual BOOL				GotoDistance(const LPCTSTR lpstrAxis, double dDistance, const int iMoveMode = 0) = 0;   //iMoveMode : 0. 轴运动 1.Jog运动
	virtual BOOL				IsRobotBusy(DWORD dwTimeOut = 20000) = 0;	
	virtual BOOL				JumpPosMod(RobotPosInfo sRobotPosInfo, const double dZHight = 600.0) = 0;
	virtual BOOL				GotoPosMod(RobotPosInfo sRobotPosInfo, const double dZHight = 100.0) = 0;
	virtual BOOL                GotoPos(const int iPosIndex, const double dZHight  = 600) = 0;
	virtual BOOL				PassPosMod(RobotPosInfo *pRobotPosInfo, const int iPassCnt, const double dZHight = 100.0) = 0;		//Pass Mode Use: pPasspPos中最后一个Pos 是目标点, iPassCnt Pos的个数包含目标点		
	virtual BOOL				MulitPosMod(RobotPosInfo *pRobotPosInfo, const int iPassCnt, const double dZHight = 100.0) = 0;		//Pass Mode Use: pPasspPos中最后一个Pos 是目标点, iPassCnt Pos的个数包含目标点		
	virtual BOOL                SavePort(int iIndex, RobotPosInfo pRobotPosInfo) = 0;



	//控制
	virtual int					SetPower(const BOOL bPower) = 0;				//Power: FALSE-Low TRUE-High
	virtual int					SetEnable(const BOOL bEnable) = 0;				//bEnable: FALSE-MotorOff TRUE-MotorOn
	virtual int					SetSpeed(const int iSpeed, const int iAccel) = 0;					//Set Speed Min:0 Max:100
	virtual int					OpenOutPort(const int iOutPort) = 0;
	virtual int					CloseOutPort(const int iOutPort) = 0;
	virtual int					QryOutPort(const int iOutPort) = 0;
	virtual int					QryInPort(const int iInPort) = 0;
	virtual int					QryCurPos(RobotPosInfo &sCurPos) = 0;
	virtual int					QryToolCurPos(RobotPosInfo &sCurPos) = 0;
	virtual int					QryEnable() = 0;
	virtual int					QryPower() = 0;
	virtual void				CloseConnect() = 0;

	virtual int					GetPosInfo(const int iPosIndex, RobotPosInfo &sPosInfo) = 0;//获取单个点
	virtual int					StorePos(const int iPosIndex) = 0;   //在机械手中保存当前位置到点P（iPosIndex）
	virtual int                 SetTool(const int iIndex, const double dXPos, const double dYPos, const double dZPos = 0.0, const double dUPos = 0.0) = 0;
	virtual int					GetTool(const int iIndex, double &dXPos, double &dYPos, double &dZPos, double &dUPos) = 0; 

	virtual int					SetPosition(const int iPosition, RobotPosInfo sRobotPosInfo) = 0;
	virtual int					CreateTool(const int iCameraNo, const int iToolNo, const double dXVal, const double dYVal) = 0;
	virtual int					ChangeTool(const int iToolNo) = 0;
	virtual int					CameraCal(const int iCameraNo, const int iType, LPTSTR lpstrVisionRst) = 0;
	virtual int					TransPosition(RobotPosInfo &sTransPos, const double dXVal, const double dYVal, const double dUVal, const int iCameraNo) = 0;

	//Robot Program Init
	virtual BOOL				InitRobot(LPTSTR lpstrError) = 0;
	virtual int					SuspendRobot() = 0;
	virtual int					ResumeRobot() = 0;
	virtual int					StopRobot() = 0;
	virtual BOOL				IsRobotConnnect() = 0;

	//Pallet 
	virtual int					CreatePallet(const int iPalletNo, const int iRow, const int iCol) = 0;
	virtual int					GetPalletPos(const int iPalletNo, const int iPositionNo, RobotPosInfo &sPosInfo) = 0;//获取单个点
};

extern "C"
{
	void AM_ROBOT_API CreateRobot(CIRobot **ppIRobot, const int iType);
	void AM_ROBOT_API ReleaseRobot(CIRobot *pIRobot);
};
