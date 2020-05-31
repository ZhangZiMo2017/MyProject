#pragma once
#include "ViewBase.h"
class CLogIn :
	public CViewBase
{

public:
	CLogIn(LPCTSTR pszXMLName);
	~CLogIn();

public:
	virtual void OnClick(DuiLib::TNotifyUI& msg);
	virtual void InitWindow();
	LPCTSTR GetWindowClassName() const;

protected:
	DuiLib::CDuiString m_strTitle;
	DuiLib::CDuiString m_strMsg;
	DuiLib::CButtonUI *m_pBtnOK;
	DuiLib::CButtonUI *m_pBtnNO;
	DuiLib::CButtonUI *m_pBtnCancel;
	DuiLib::CLabelUI *m_pLabTitle;
	DuiLib::CTextUI *m_pLabMsg;
	CIniFile m_ciniUser;

	void Notify(DuiLib::TNotifyUI& msg);
};

