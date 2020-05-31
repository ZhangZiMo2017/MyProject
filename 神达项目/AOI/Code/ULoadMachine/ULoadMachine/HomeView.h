#pragma once

class CHomeView
{
public:
	void InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow);
	BOOL OnClick(DuiLib::TNotifyUI& msg);
	void UpdataStatus();

	void ClearAllRstBoard();
	void ClearRstBoard(const int iIndex);
	void SetRstBoard(const int iIndex, const int iStatus);
	void UpdataRst();

	void ClearAllOKBoard();
	void SetOKBoard(const int iIndex);

	void ClearAllNGBoard();
	void SetNGBoard(const int iIndex);

	void UpdataUph();

private:
	CPaintManagerUI *m_pPaintManagerUI;
	CIWorkFlow *m_pIWorkFlow;
	CIniFile m_classConfig;
};