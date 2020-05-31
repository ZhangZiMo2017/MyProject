#pragma once

class CTrayAxis
{
public:
	void InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow);
	BOOL OnClick(DuiLib::TNotifyUI& msg);
	void ViewCurPos();
	void MoveDistance(const int iAxis, BOOL bDir);
private:
	CPaintManagerUI *m_pPaintManagerUI;
	CIWorkFlow *m_pIWorkFlow;
};