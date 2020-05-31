#pragma once

class CParamView
{
public:
	void InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow);
	BOOL OnClick(DuiLib::TNotifyUI& msg);

protected:
	void InitRobot();
	void InitYeild();
	void InitAxis();
	void StoreAxis();

	void StoreRobot();
	void StoreYeild();
private:
	CPaintManagerUI *m_pPaintManagerUI;
	CIWorkFlow *m_pIWorkFlow;
	CIniFile m_classConfig;

	//Robot

	CEditUI *m_pEdtRobotIp;
	CEditUI *m_pEdtInitPort;
	CEditUI *m_pEdtMovePort;
	CEditUI *m_pEdtCtrlPort;
	CEditUI *m_pEdtVisionPort;
	CEditUI *m_pEdtSafeHigh;
	CEditUI *m_pEdtOKHigh;
	CEditUI *m_pEdtNGHigh;
	CEditUI *m_pEdtYeildTarget;
	CEditUI *m_pEdtYeildSpec;
	CEditUI *m_pEdtFailCount;

	//Öá
	CEditUI *m_pEdtZ1HomeSpeed;
	CEditUI *m_pEdtZ1JogSpeed;
	CEditUI *m_pEdtZ1MoveSpeed;
	CEditUI *m_pEdtZ1HomeAcc;
	CEditUI *m_pEdtZ1HomeDec;
	CEditUI *m_pEdtZ1JogAcc;
	CEditUI *m_pEdtZ1JogDec;
	CEditUI *m_pEdtZ1MoveAcc;
	CEditUI *m_pEdtZ1MoveDec;

	CEditUI *m_pEdtZ2HomeSpeed;
	CEditUI *m_pEdtZ2JogSpeed;
	CEditUI *m_pEdtZ2MoveSpeed;
	CEditUI *m_pEdtZ2HomeAcc;
	CEditUI *m_pEdtZ2HomeDec;
	CEditUI *m_pEdtZ2JogAcc;
	CEditUI *m_pEdtZ2JogDec;
	CEditUI *m_pEdtZ2MoveAcc;
	CEditUI *m_pEdtZ2MoveDec;
};