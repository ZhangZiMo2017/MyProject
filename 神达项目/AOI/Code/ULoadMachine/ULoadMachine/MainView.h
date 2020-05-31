#pragma once

#include "RobotView.h"
#include "IOView.h"
#include "Product.h"
#include "HomeView.h"
#include "TrayAxis.h"
#include "ParamView.h"


class CMainView : public DuiLib::WindowImplBase
{
public:
	CMainView();
	~CMainView();

public:
	DUI_DECLARE_MESSAGE_MAP()

public:
	LPCTSTR GetWindowClassName() const;
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual DuiLib::CDuiString GetSkinFile();
	virtual DuiLib::CDuiString GetSkinFolder();
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnClick(DuiLib::TNotifyUI& msg);
	virtual void OnSelChanged(DuiLib::TNotifyUI& msg);
	virtual void OnOpChanged(DuiLib::TNotifyUI& msg);
	virtual void ShowLog(const LPCTSTR lpstrDir);


	
	void SetParent(HWND hParentWnd);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void CreateLog(LPCTSTR lpstrTime, int iCode, LPCTSTR lpstrData, const int iSelect, const int iType);
	void UpdataLocalTime();
	void UpdataTitle();

	void OnReset();
	void OnStart();
	void OnStop();
protected:

	void Notify(DuiLib::TNotifyUI& msg);

private:
	HWND	m_hParentWnd;

	CCriticalSection	m_csMsgDlg;

	tstring m_strAlarmIni;
	tstring m_strConfigDir;
	tstring	m_strConfigIni;
	tstring m_strProductIni;
	tstring m_strCurProduct;
	
	CIniFile m_ciniProduct;
	CIniFile m_ciniConfig;
	CIniFile m_ciniMessage;

	CRobotView m_classRobotView;
	CIOView	   m_classIOView;
	CIWorkFlow *m_pIWorkFlow;
	CProduct m_classProduct;
	CHomeView m_classHomeView;
	CTrayAxis m_classTrayView;
	CParamView m_classParamView;
	DWORD m_dwSTTime;
	BOOL m_bSTTime;

	DWORD m_dwSTPsw;

	//
	CButtonUI *m_pBtnReset;
	CButtonUI *m_pBtnStart;
	CButtonUI *m_pBtnStop;
	CVerticalLayoutUI *m_pVerAlarmMessage;
	CVerticalLayoutUI *m_pVerInfoMessage;
	CTextUI *m_pLblTitle;

};

