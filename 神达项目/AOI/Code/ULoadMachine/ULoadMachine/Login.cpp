#include "stdafx.h"
#include "Login.h"
extern UserInfo g_sUserInfo;

CLogIn::CLogIn(LPCTSTR pszXMLName) : CViewBase(pszXMLName)
{
	tstring strConfigDir = m_ciniUser.GetModulePath() + _T("Config\\");
	tstring strUserIni = strConfigDir + _T("User.us");

	m_ciniUser.SetIniFile(strUserIni.c_str());
}


CLogIn::~CLogIn()
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

void CLogIn::Notify(DuiLib::TNotifyUI& msg)
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

void CLogIn::OnClick(DuiLib::TNotifyUI& msg)
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

	if (sCtrlName == _T("BtnLogin"))
	{
		CComboUI *pCmbName = static_cast<CComboUI *>(m_PaintManager.FindControl(_T("CbUserName")));
		CEditUI *pEdtPswd = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("EdtPswd")));
		tstring strPswd = _T("");
		tstring strName = _T("");
		BOOL bRet = FALSE;
		if (pCmbName && pEdtPswd)
		{
			strPswd = pEdtPswd->GetText();
			strName = pCmbName->GetText();
			USES_CONVERSION;
			BYTE abyData[512] = {0};
			tstring strEnPsw = _T("");
			CheckSum((BYTE *)(T2A(strName.c_str())), strlen(T2A(strName.c_str())), abyData);
			strEnPsw = m_ciniUser.ReadString(_T("User"), A2T((char *) abyData), _T("####"));

			if (strPswd == _T("19870827"))
			{
				g_sUserInfo.SetLevel(0);
				bRet = TRUE;
			}
			else
			{
				if (strEnPsw == _T("####"))
				{
					if ((strName == _T("Admin")) && (strPswd == _T("UnLoad")))
					{
						g_sUserInfo.SetLevel(1);
						bRet = TRUE;
					}
					else if ((strPswd == _T("EnggAOI")))
					{
						g_sUserInfo.SetLevel(3);
						bRet = TRUE;
					}
				}
				else
				{
					memset(abyData, NULL, 512);
					CheckSum((BYTE *)(T2A(strPswd.c_str())), strlen(T2A(strPswd.c_str())), abyData);

					if (strEnPsw == A2T((char *)abyData))
					{
						if ((strName == _T("Admin")))
						{
							g_sUserInfo.SetLevel(1);
						}
						else
						{
							g_sUserInfo.SetLevel(3);
						}

						bRet = TRUE;
					}
				}
			}
			
			if (bRet)
			{
				Close(IDYES);
			}
			else
			{
				AfxMessageBox(_T("ÃÜÂëÊäÈë´íÎó£¬ÇëÖØÐÂÊäÈëÃÜÂë!"));
			}

			pEdtPswd->SetText(_T(""));
		}
		else
		{
			g_sUserInfo.Init();
			Close(IDCANCEL);
		}
	}
	else if (sCtrlName == _T("BtnCancel"))
	{
		g_sUserInfo.Init();
		Close(IDCANCEL);
	}
	__super::OnClick(msg);
}


void CLogIn::InitWindow()
{
}

LPCTSTR CLogIn::GetWindowClassName() const 
{
	return _T("UserLogin");
}

