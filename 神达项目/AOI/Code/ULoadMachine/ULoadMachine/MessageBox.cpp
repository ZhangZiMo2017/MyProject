#include "stdafx.h"
#include "MessageBox.h"


CMessageBox::CMessageBox(LPCTSTR pszXMLName, LPCTSTR pszTitle, LPCTSTR pszMsg) : CViewBase(pszXMLName)
{
	m_strTitle = pszTitle;
	m_strMsg = pszMsg;
	m_pBtnCancel = NULL;
	m_pBtnNO = NULL;
	m_pBtnOK = NULL;
}


CMessageBox::~CMessageBox()
{
}

// DUI_BEGIN_MESSAGE_MAP(CMessageBox, DuiLib::CNotifyPump)
// DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
// DUI_END_MESSAGE_MAP()

// DuiLib::CDuiString CMessageBox::GetSkinFile()
// {
// 	return m_strXMLName;
// }
// 
// DuiLib::CDuiString CMessageBox::GetSkinFolder()
// {
// 	return _T("");
// }

void CMessageBox::Notify(DuiLib::TNotifyUI& msg)
{
	if (msg.sType == _T("click"))
	{
		OnClick(msg);
	}
	else
	{
		__super::Notify(msg);
	}
}

void CMessageBox::OnClick(DuiLib::TNotifyUI& msg)
{
	CDuiString sCtrlName = msg.pSender->GetName();
	if (sCtrlName == _T("minbtn"))
	{
		return;
	}
	else if (sCtrlName == _T("maxbtn"))
	{
		return;
	}
	else if (sCtrlName == _T("restorebtn"))
	{
		
		return;
	}

	if (msg.pSender == m_pBtnCancel)
	{
		Close(IDCANCEL);
	}
	else if (msg.pSender == m_pBtnOK)
	{
		Close(IDYES);
	}
	else if (msg.pSender == m_pBtnNO)
	{
		Close(IDNO);
	}
	else
	{
		Close(IDNO);
	}
	__super::OnClick(msg);
}
	

void CMessageBox::InitWindow()
{
	m_pBtnCancel = static_cast<DuiLib::CButtonUI *>(m_PaintManager.FindControl(_T("Btn_Box_Cancel")));
	m_pBtnOK = static_cast<DuiLib::CButtonUI *>(m_PaintManager.FindControl(_T("Btn_Box_OK")));
	m_pBtnNO = static_cast<DuiLib::CButtonUI *>(m_PaintManager.FindControl(_T("Btn_Box_NO")));
	m_pLabMsg = static_cast<DuiLib::CTextUI *>(m_PaintManager.FindControl(_T("Lab_Box_Msg")));
	m_pLabTitle = static_cast<DuiLib::CLabelUI *>(m_PaintManager.FindControl(_T("Lab_Box_Title")));

	if (m_pLabMsg)
	{
		m_pLabMsg->SetMultiLine(true);
		m_pLabMsg->SetText(m_strMsg);
	}

	if (m_pLabTitle)
	{
		m_pLabTitle->SetText(m_strTitle);
	}
}

LPCTSTR CMessageBox::GetWindowClassName() const 
{
	return _T("MessageBox");
}

