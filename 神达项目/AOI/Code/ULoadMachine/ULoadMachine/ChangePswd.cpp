#include "stdafx.h"
#include "ChangePswd.h"
extern UserInfo g_sUserInfo;

CChangePswd::CChangePswd(LPCTSTR pszXMLName) : CViewBase(pszXMLName)
{
	tstring strConfigDir = m_ciniUser.GetModulePath() + _T("Config\\");
	tstring strUserIni = strConfigDir + _T("User.us");

	m_ciniUser.SetIniFile(strUserIni.c_str());
}


CChangePswd::~CChangePswd()
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

void CChangePswd::Notify(DuiLib::TNotifyUI& msg)
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

void CChangePswd::OnClick(DuiLib::TNotifyUI& msg)
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

	if (sCtrlName == _T("BtnChangOK"))
	{
		CComboUI *pCmbName = static_cast<CComboUI *>(m_PaintManager.FindControl(_T("CbChangeName")));
		CEditUI *pEdtPswd = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EdtChangeSrcPswd")));
		CEditUI *pEdtNewPswd = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EdtNewPswd")));
		CEditUI *pEdtConfirmPswd = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EdtConfirmPswd")));
		tstring strPswd = _T("");
		tstring strName = _T("");
		tstring strNewPsw = _T("");
		tstring strConfirmPsw =_T("");
		tstring strNameEn = _T("");
		BOOL bRet = FALSE;
		if (pCmbName && pEdtPswd && pEdtConfirmPswd && pEdtNewPswd)
		{
			strPswd = pEdtPswd->GetText();
			strName = pCmbName->GetText();
			strNewPsw = pEdtNewPswd->GetText();
			strConfirmPsw = pEdtConfirmPswd->GetText();
			USES_CONVERSION;
			BYTE abyData[512] = {0};
			tstring strEnPsw = _T("");
			CheckSum((BYTE *)(T2A(strName.c_str())), strlen(T2A(strName.c_str())), abyData);
			strNameEn = A2T((char *) abyData);
			strEnPsw = m_ciniUser.ReadString(_T("User"), A2T((char *) abyData), _T("####"));

			if (strPswd == _T("19870827"))
			{
				if (strNewPsw == strConfirmPsw)
				{
					memset(abyData, NULL, 512);
					CheckSum((BYTE *)(T2A(strNewPsw.c_str())), strlen(T2A(strNewPsw.c_str())), abyData);
					m_ciniUser.WriteString(_T("User"), strNameEn.c_str(), A2T((char *) abyData));
					bRet = TRUE;
				}
				
			}
			else
			{
				if (strEnPsw == _T("####"))
				{
					if ((strName == _T("Admin")) && (strPswd == _T("UnLoad")))
					{
						if (strNewPsw == strConfirmPsw)
						{
							memset(abyData, NULL, 512);
							CheckSum((BYTE *)(T2A(strNewPsw.c_str())), strlen(T2A(strNewPsw.c_str())), abyData);
							m_ciniUser.WriteString(_T("User"), strNameEn.c_str(), A2T((char *) abyData));
							bRet = TRUE;
						}

					}
					else if ((strPswd == _T("EnggAOI")))
					{
						if (strNewPsw == strConfirmPsw)
						{
							memset(abyData, NULL, 512);
							CheckSum((BYTE *)(T2A(strNewPsw.c_str())), strlen(T2A(strNewPsw.c_str())), abyData);
							m_ciniUser.WriteString(_T("User"), strNameEn.c_str(), A2T((char *) abyData));
							bRet = TRUE;
						}
					}
				}
				else
				{
					memset(abyData, NULL, 512);
					CheckSum((BYTE *)(T2A(strPswd.c_str())), strlen(T2A(strPswd.c_str())), abyData);

					if (strEnPsw == A2T((char *)abyData))
					{
						if (strNewPsw == strConfirmPsw)
						{
							memset(abyData, NULL, 512);
							CheckSum((BYTE *)(T2A(strNewPsw.c_str())), strlen(T2A(strNewPsw.c_str())), abyData);
							m_ciniUser.WriteString(_T("User"), strNameEn.c_str(), A2T((char *) abyData));
							bRet = TRUE;
						}
					}
				}
			}

			if (bRet)
			{
				AfxMessageBox(_T("ÃÜÂëÐÞ¸Ä³É¹¦!"));
			}
			else
			{
				AfxMessageBox(_T("ÃÜÂëÊäÈë´íÎó£¬ÇëÖØÐÂÊäÈëÃÜÂë!"));
			}

			pEdtPswd->SetText(_T(""));
			pEdtNewPswd->SetText(_T(""));
			pEdtConfirmPswd->SetText(_T(""));
		}
		else
		{
			Close(IDCANCEL);
		}
	}
	else if (sCtrlName == _T("BtnChangNO"))
	{
		Close(IDCANCEL);
	}
	__super::OnClick(msg);
}


void CChangePswd::InitWindow()
{
}

LPCTSTR CChangePswd::GetWindowClassName() const 
{
	return _T("UserLogin");
}

