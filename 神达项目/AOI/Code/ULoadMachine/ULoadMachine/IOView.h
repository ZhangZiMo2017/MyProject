#pragma once

class CIOView
{
public:
	void InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow);
	BOOL OnClick(DuiLib::TNotifyUI& msg);
	void UpdataStatus();
private:
	CPaintManagerUI *m_pPaintManagerUI;
	CIWorkFlow *m_pIWorkFlow;
};