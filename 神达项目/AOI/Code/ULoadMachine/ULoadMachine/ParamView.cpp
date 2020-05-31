#include "stdafx.h"
#include "ParamView.h"

#define ROBOT _T("Robot")
extern UserInfo g_sUserInfo;

void CParamView::InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow)
{
	m_pPaintManagerUI = pPaintManagerUI;
	m_pIWorkFlow = pIWorkFlow;
	tstring strConfig = m_classConfig.GetModulePath() + _T("Config\\Config.ini");
	m_classConfig.SetIniFile(strConfig.c_str());

	m_pEdtRobotIp = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtRobotIp")));
	m_pEdtInitPort = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtInitPort")));
	m_pEdtMovePort = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtMovePort")));
	m_pEdtCtrlPort = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtCtrlPort")));
	m_pEdtVisionPort = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtVisionPort")));
	m_pEdtYeildSpec = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtYeildSpec")));
	m_pEdtYeildTarget = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtYeildTarget")));
	m_pEdtSafeHigh = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtSafeHigh")));
	m_pEdtOKHigh = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtOKHigh")));
	m_pEdtNGHigh = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtNGHigh")));
	m_pEdtFailCount = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtFailCount")));

	m_pEdtZ1HomeSpeed = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1HomeSpeed")));
	m_pEdtZ1JogSpeed = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1JogSpeed")));
	m_pEdtZ1MoveSpeed = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1MoveSpeed")));
	m_pEdtZ1HomeAcc = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1HomeAcc")));
	m_pEdtZ1HomeDec = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1HomeDec")));
	m_pEdtZ1JogAcc = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1JogAcc")));
	m_pEdtZ1JogDec = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1JogDec")));
	m_pEdtZ1MoveAcc = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1MoveAcc")));
	m_pEdtZ1MoveDec = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1MoveDec")));

	m_pEdtZ2HomeSpeed = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2HomeSpeed")));
	m_pEdtZ2JogSpeed = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2JogSpeed")));
	m_pEdtZ2MoveSpeed = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2MoveSpeed")));
	m_pEdtZ2HomeAcc = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2HomeAcc")));
	m_pEdtZ2HomeDec = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2HomeDec")));
	m_pEdtZ2JogAcc = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2JogAcc")));
	m_pEdtZ2JogDec = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2JogDec")));
	m_pEdtZ2MoveAcc = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2MoveAcc")));
	m_pEdtZ2MoveDec = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2MoveDec")));

	InitRobot();
	InitYeild();
	InitAxis();
}

BOOL CParamView::OnClick(DuiLib::TNotifyUI& msg)
{
	BOOL bRet = FALSE;

	if (msg.pSender->GetName() == _T("BtnStoreBaseParam"))
	{
		if (g_sUserInfo.GetLevel() < 3)
		{
			StoreRobot();
			StoreYeild();
		}
		else
		{
			AfxMessageBox(_T("当前用户权限不够，请登录更高权限保存!"));
		}

		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnStoreAxisParam"))
	{
		if (g_sUserInfo.GetLevel() < 3)
		{
			StoreAxis();
		}
		else
		{
			AfxMessageBox(_T("当前用户权限不够，请登录更高权限保存!"));
		}
		bRet = TRUE;
	}

	return bRet;
}

void CParamView::InitRobot()
{
	if (m_pEdtRobotIp)
	{
		m_pEdtRobotIp->SetText(m_classConfig.ReadString(ROBOT, _T("Ip"), _T("127.0.0.1")).c_str());
	}

	if (m_pEdtInitPort)
	{
		m_pEdtInitPort->SetText(m_classConfig.ReadString(ROBOT, _T("InitPort"), _T("5000")).c_str());
	}

	if (m_pEdtMovePort)
	{
		m_pEdtMovePort->SetText(m_classConfig.ReadString(ROBOT, _T("MovePort"), _T("2000")).c_str());
	}

	if (m_pEdtCtrlPort)
	{
		m_pEdtCtrlPort->SetText(m_classConfig.ReadString(ROBOT, _T("CtrlPort"), _T("2001")).c_str());
	}

	if (m_pEdtVisionPort)
	{
		m_pEdtVisionPort->SetText(m_classConfig.ReadString(ROBOT, _T("VisionPort"), _T("2010")).c_str());
	}


	if (m_pEdtSafeHigh)
	{
		m_pEdtSafeHigh->SetText(m_classConfig.ReadString(ROBOT, _T("SafeHigh"), _T("0")).c_str());
	}

	if (m_pEdtOKHigh)
	{
		m_pEdtOKHigh->SetText(m_classConfig.ReadString(ROBOT, _T("PutOKHigh"), _T("-23")).c_str());
	}

	if (m_pEdtNGHigh)
	{
		m_pEdtNGHigh->SetText(m_classConfig.ReadString(ROBOT, _T("PutNgHigh"), _T("-10")).c_str());
	}

}

void CParamView::InitYeild()
{
	if (m_pEdtYeildSpec)
	{
		m_pEdtYeildSpec->SetText(m_classConfig.ReadString(_T("Yeild"), _T("Spec"), _T("85.0")).c_str());
	}

	if (m_pEdtYeildTarget)
	{
		m_pEdtYeildTarget->SetText(m_classConfig.ReadString(_T("Yeild"), _T("Target"), _T("90.0")).c_str());
	}

	if (m_pEdtFailCount)
	{
		m_pEdtFailCount->SetText(m_classConfig.ReadString(_T("Yeild"), _T("FailCount"), _T("3")).c_str());
	}
}

void CParamView::InitAxis()
{
	double dHomeSpeed = 10.0;
	double dHomeAcc = 0.03;
	double dHomeDec = 0.25;
	TCHAR atcSection[128] = {0};
	swprintf(atcSection, _T("Z%d"), 1);
	if (m_pEdtZ1HomeSpeed)
	{
		m_pEdtZ1HomeSpeed->SetText(m_classConfig.ReadString(atcSection, _T("HomeSpeed"), _T("10.0")).c_str());
	}

	if (m_pEdtZ1HomeAcc)
	{
		m_pEdtZ1HomeAcc->SetText(m_classConfig.ReadString(atcSection, _T("HomeAcc"), _T("0.03")).c_str());
	}

	if (m_pEdtZ1HomeDec)
	{
		m_pEdtZ1HomeDec->SetText(m_classConfig.ReadString(atcSection, _T("HomeDec"), _T("0.25")).c_str());
	}

	if (m_pEdtZ1JogSpeed)
	{
		m_pEdtZ1JogSpeed->SetText(m_classConfig.ReadString(atcSection, _T("JogSpeed"), _T("20.0")).c_str());
	}

	if (m_pEdtZ1JogAcc)
	{
		m_pEdtZ1JogAcc->SetText(m_classConfig.ReadString(atcSection, _T("JogAcc"), _T("0.0625")).c_str());
	}

	if (m_pEdtZ1JogDec)
	{
		m_pEdtZ1JogDec->SetText(m_classConfig.ReadString(atcSection, _T("JogDec"), _T("0.0625")).c_str());
	}

	if (m_pEdtZ1MoveSpeed)
	{
		m_pEdtZ1MoveSpeed->SetText(m_classConfig.ReadString(atcSection, _T("MoveSpeed"), _T("20.0")).c_str());
	}

	if (m_pEdtZ1MoveAcc)
	{
		m_pEdtZ1MoveAcc->SetText(m_classConfig.ReadString(atcSection, _T("MoveAcc"), _T("0.1")).c_str());
	}

	if (m_pEdtZ1MoveDec)
	{
		m_pEdtZ1MoveDec->SetText(m_classConfig.ReadString(atcSection, _T("MoveDec"), _T("0.1")).c_str());
	}


	swprintf(atcSection, _T("Z%d"), 2);
	if (m_pEdtZ2HomeSpeed)
	{
		m_pEdtZ2HomeSpeed->SetText(m_classConfig.ReadString(atcSection, _T("HomeSpeed"), _T("10.0")).c_str());
	}

	if (m_pEdtZ2HomeAcc)
	{
		m_pEdtZ2HomeAcc->SetText(m_classConfig.ReadString(atcSection, _T("HomeAcc"), _T("0.03")).c_str());
	}

	if (m_pEdtZ2HomeDec)
	{
		m_pEdtZ2HomeDec->SetText(m_classConfig.ReadString(atcSection, _T("HomeDec"), _T("0.25")).c_str());
	}

	if (m_pEdtZ2JogSpeed)
	{
		m_pEdtZ2JogSpeed->SetText(m_classConfig.ReadString(atcSection, _T("JogSpeed"), _T("20.0")).c_str());
	}

	if (m_pEdtZ2JogAcc)
	{
		m_pEdtZ2JogAcc->SetText(m_classConfig.ReadString(atcSection, _T("JogAcc"), _T("0.0625")).c_str());
	}

	if (m_pEdtZ2JogDec)
	{
		m_pEdtZ2JogDec->SetText(m_classConfig.ReadString(atcSection, _T("JogDec"), _T("0.0625")).c_str());
	}

	if (m_pEdtZ2MoveSpeed)
	{
		m_pEdtZ2MoveSpeed->SetText(m_classConfig.ReadString(atcSection, _T("MoveSpeed"), _T("20.0")).c_str());
	}

	if (m_pEdtZ2MoveAcc)
	{
		m_pEdtZ2MoveAcc->SetText(m_classConfig.ReadString(atcSection, _T("MoveAcc"), _T("0.1")).c_str());
	}

	if (m_pEdtZ2MoveDec)
	{
		m_pEdtZ2MoveDec->SetText(m_classConfig.ReadString(atcSection, _T("MoveDec"), _T("0.1")).c_str());
	}
}

void CParamView::StoreAxis()
{
	double dHomeSpeed = 10.0;
	double dHomeAcc = 0.03;
	double dHomeDec = 0.25;
	TCHAR atcSection[128] = {0};
	swprintf(atcSection, _T("Z%d"), 1);
	if (m_pEdtZ1HomeSpeed)
	{		
		m_classConfig.WriteString(atcSection, _T("HomeSpeed"), m_pEdtZ1HomeSpeed->GetText());
	}

	if (m_pEdtZ1HomeAcc)
	{
		m_classConfig.WriteString(atcSection, _T("HomeAcc"),m_pEdtZ1HomeAcc->GetText());
	}

	if (m_pEdtZ1HomeDec)
	{
		m_classConfig.WriteString(atcSection, _T("HomeDec"), m_pEdtZ1HomeDec->GetText());
	}

	if (m_pEdtZ1JogSpeed)
	{
		m_classConfig.WriteString(atcSection, _T("JogSpeed"), m_pEdtZ1JogSpeed->GetText());
	}

	if (m_pEdtZ1JogAcc)
	{
		m_classConfig.WriteString(atcSection, _T("JogAcc"), m_pEdtZ1JogAcc->GetText());
	}

	if (m_pEdtZ1JogDec)
	{
		m_classConfig.WriteString(atcSection, _T("JogDec"), m_pEdtZ1JogDec->GetText());
	}

	if (m_pEdtZ1MoveSpeed)
	{
		m_classConfig.WriteString(atcSection, _T("MoveSpeed"),m_pEdtZ1MoveSpeed->GetText());
	}

	if (m_pEdtZ1MoveAcc)
	{
		m_classConfig.WriteString(atcSection, _T("MoveAcc"), m_pEdtZ1MoveAcc->GetText());
	}

	if (m_pEdtZ1MoveDec)
	{
		m_classConfig.WriteString(atcSection, _T("MoveDec"), m_pEdtZ1MoveDec->GetText());
	}


	swprintf(atcSection, _T("Z%d"), 2);
	if (m_pEdtZ2HomeSpeed)
	{
		m_classConfig.WriteString(atcSection, _T("HomeSpeed"),m_pEdtZ2HomeSpeed->GetText());
	}

	if (m_pEdtZ2HomeAcc)
	{
		m_classConfig.WriteString(atcSection, _T("HomeAcc"), m_pEdtZ2HomeAcc->GetText());
	}

	if (m_pEdtZ2HomeDec)
	{
		m_classConfig.WriteString(atcSection, _T("HomeDec"), m_pEdtZ2HomeDec->GetText());
	}

	if (m_pEdtZ2JogSpeed)
	{
		m_classConfig.WriteString(atcSection, _T("JogSpeed"), m_pEdtZ2JogSpeed->GetText());
	}

	if (m_pEdtZ2JogAcc)
	{
		m_classConfig.WriteString(atcSection, _T("JogAcc"), m_pEdtZ2JogAcc->GetText());
	}

	if (m_pEdtZ2JogDec)
	{
		m_classConfig.WriteString(atcSection, _T("JogDec"), m_pEdtZ2JogDec->GetText());
	}

	if (m_pEdtZ2MoveSpeed)
	{
		m_classConfig.WriteString(atcSection, _T("MoveSpeed"), m_pEdtZ2MoveSpeed->GetText());
	}

	if (m_pEdtZ2MoveAcc)
	{
		m_classConfig.WriteString(atcSection, _T("MoveAcc"), m_pEdtZ2MoveAcc->GetText());
	}

	if (m_pEdtZ2MoveDec)
	{
		m_classConfig.WriteString(atcSection, _T("MoveDec"), m_pEdtZ2MoveDec->GetText());
	}
}

void CParamView::StoreRobot()
{
	if (m_pEdtRobotIp)
	{
		m_classConfig.WriteString(ROBOT, _T("Ip"), m_pEdtRobotIp->GetText());
	}

	if (m_pEdtInitPort)
	{
		m_classConfig.WriteString(ROBOT, _T("InitPort"), m_pEdtInitPort->GetText());
	}

	if (m_pEdtMovePort)
	{
		m_classConfig.WriteString(ROBOT, _T("MovePort"), m_pEdtMovePort->GetText());
	}

	if (m_pEdtCtrlPort)
	{
		m_classConfig.WriteString(ROBOT, _T("CtrlPort"), m_pEdtCtrlPort->GetText());
	}

	if (m_pEdtVisionPort)
	{
		m_classConfig.WriteString(ROBOT, _T("VisionPort"), m_pEdtVisionPort->GetText());
	}

	if (m_pEdtSafeHigh)
	{
		m_classConfig.WriteString(ROBOT, _T("SafeHigh"), m_pEdtSafeHigh->GetText());
	}

	if (m_pEdtOKHigh)
	{
		m_classConfig.WriteString(ROBOT, _T("PutOKHigh"), m_pEdtOKHigh->GetText());
	}

	if (m_pEdtNGHigh)
	{
		m_classConfig.WriteString(ROBOT, _T("PutNgHigh"), m_pEdtNGHigh->GetText());
	}
}

void CParamView::StoreYeild()
{
	if (m_pEdtYeildSpec)
	{
		m_classConfig.WriteString(_T("Yeild"), _T("Spec"), m_pEdtYeildSpec->GetText());
	}

	if (m_pEdtYeildTarget)
	{
		m_classConfig.WriteString(_T("Yeild"), _T("Target"), m_pEdtYeildTarget->GetText());
	}

	if (m_pEdtFailCount)
	{
		m_classConfig.WriteString(_T("Yeild"), _T("FailCount"), m_pEdtFailCount->GetText());
	}
}