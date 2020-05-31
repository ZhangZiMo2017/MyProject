#pragma once

class CRobotView
{
public:
	void InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow);
	BOOL OnClick(DuiLib::TNotifyUI& msg);
	void ViewCurPos();
	void MoveDistance(LPCTSTR lpstrAxis, BOOL bDir);
	void AxisMoveDiatance(BOOL bDir);
	BOOL OnSelChanged(DuiLib::TNotifyUI& msg);
private:
	CPaintManagerUI *m_pPaintManagerUI;
	CIWorkFlow *m_pIWorkFlow;
};