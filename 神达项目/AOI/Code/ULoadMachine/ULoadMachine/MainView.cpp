#include "stdafx.h"
#include "MainView.h"

UserInfo g_sUserInfo;

CMainView::CMainView()
{
	m_strConfigDir = m_ciniConfig.GetModulePath() + _T("Config\\");
	m_strAlarmIni = m_strConfigDir + _T("Alarm.ini");

	m_strConfigIni = m_ciniConfig.GetModulePath() + _T("Config\\Config.ini");
	m_ciniConfig.SetIniFile(m_strConfigIni.c_str());
	m_strProductIni = m_ciniConfig.GetModulePath() + _T("Product\\Product.ini");
	m_ciniProduct.SetIniFile(m_strProductIni.c_str());
	m_ciniMessage.SetIniFile(m_strAlarmIni.c_str());
	m_pIWorkFlow = NULL;
	CreateWorkFlow(&m_pIWorkFlow);
}


CMainView::~CMainView()
{
	ReleaseWorkFlow(m_pIWorkFlow);
}

//DUI_BEGIN_MESSAGE_MAP(CMainView, DuiLib::CNotifyPump)
DUI_BASE_BEGIN_MESSAGE_MAP(CMainView)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnOpChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnSelChanged)
DUI_END_MESSAGE_MAP()

LPCTSTR CMainView::GetWindowClassName() const
{
	return _T("ULoad");
}

DuiLib::CControlUI* CMainView::CreateControl(LPCTSTR pstrClass)
{
	if (pstrClass)
	{
		DuiLib::CDialogBuilder builder;
		DuiLib::CDuiString strXML = pstrClass;
		strXML += _T(".xml");
		DuiLib::CContainerUI* pSubUi = static_cast<DuiLib::CContainerUI*>(builder.Create(strXML.GetData(), (UINT)0, this, &m_PaintManager, NULL));
		if (pSubUi) 
		{
			return pSubUi;
		}
		else 
		{
			return NULL;
		}
	}
	return NULL;
}

void CMainView::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
}

DuiLib::CDuiString CMainView::GetSkinFile()
{
	return _T("MainView.xml");
}

DuiLib::CDuiString CMainView::GetSkinFolder()
{
	return  _T("");
}

void CMainView::InitWindow()
{
	m_classRobotView.InitWindow(&m_PaintManager, m_pIWorkFlow);
	m_classIOView.InitWindow(&m_PaintManager, m_pIWorkFlow);
	m_classProduct.InitWindow(&m_PaintManager, m_pIWorkFlow);
	m_classHomeView.InitWindow(&m_PaintManager, m_pIWorkFlow);
	m_classTrayView.InitWindow(&m_PaintManager, m_pIWorkFlow);
	m_classParamView.InitWindow(&m_PaintManager, m_pIWorkFlow);
	m_pIWorkFlow->SetParent(m_hWnd);
	m_pBtnStop = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnStop")));
	m_pBtnStart = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnStart")));
	m_pBtnReset = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnReset")));
	m_pVerAlarmMessage = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("VerAlarmMessage")));
	m_pVerInfoMessage = static_cast<CVerticalLayoutUI *>(m_PaintManager.FindControl(_T("VerInfoMessage")));
	m_pLblTitle = static_cast<CTextUI *>(m_PaintManager.FindControl(_T("TxtTitle")));
	m_pBtnStart->SetEnabled(false);
	m_dwSTPsw = 0;
	CButtonUI *pbtnTemp = NULL;
	g_sUserInfo.Init();
	pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnParam")));
	if (pbtnTemp)
	{
		pbtnTemp->SetVisible(false);
	}
	pbtnTemp = NULL;
	pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnRobot")));
	if (pbtnTemp)
	{
		pbtnTemp->SetVisible(false);
	}
	m_bSTTime = FALSE;
	UpdataTitle();
}

void CMainView::Notify(DuiLib::TNotifyUI& msg)
{

	return __super::Notify(msg);
} 

void CMainView::OnClick(DuiLib::TNotifyUI& msg)
{
	msg.pSender->SetMouseEnabled(false);
	CHECKRESULT(m_classRobotView.OnClick(msg), msg);
	CHECKRESULT(m_classProduct.OnClick(msg), msg);
	CHECKRESULT(m_classIOView.OnClick(msg), msg);
	CHECKRESULT(m_classParamView.OnClick(msg), msg);
	CHECKRESULT(m_classTrayView.OnClick(msg), msg);
	if (msg.pSender->GetName() == _T("BtnRobotStatus"))
	{
		if (m_pIWorkFlow)
		{
			if (m_pIWorkFlow->IsRobotConnect() && !m_pIWorkFlow->IsRunning())
			{
				if (::SendMessage(m_hWnd, WM_ULMSG_INFORMATION, 2, NULL) == IDYES)
				{
					m_pIWorkFlow->DisConnectRobot();
				}			
			}
			else
			{
				m_pIWorkFlow->ConnectRobot();
			}
		}
	}
	if (msg.pSender->GetName() == _T("BtnVisionStatus"))
	{
		if (m_pIWorkFlow)
		{
			if (m_pIWorkFlow->IsVisionConnect())
			{

					m_pIWorkFlow->DisconnectVision();
						
			}
			else
			{
				m_pIWorkFlow->ConnectVision();
			}
		}
	}
	else if (msg.pSender->GetName() == _T("BtnReset"))
	{
		OnReset();
	}
	else if (msg.pSender->GetName() == _T("BtnStart"))
	{
		OnStart();
	}
	else if (msg.pSender->GetName() == _T("BtnStop"))
	{
		OnStop();
	}
	else if (msg.pSender->GetName() == _T("BtnUserLogin"))
	{
		if (g_sUserInfo.GetLevel() < 5)
		{
			if (AfxMessageBox(_T("确定要注销用户? 注销(Yes) 取消(No)"), MB_YESNO) == IDYES)
			{
				g_sUserInfo.Init();
				CTabLayoutUI *pTabUi = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl(_T("MainViewBar")));

				if (pTabUi)
				{
					pTabUi->SelectItem(0);
				}
			}
		}
		else
		{
			Login(m_hWnd);
		}
		
		if (g_sUserInfo.GetLevel() < 5)
		{
			CButtonUI *pbtnTemp = NULL;
			pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnParam")));
			if (pbtnTemp)
			{
				pbtnTemp->SetVisible(true);
			}
			pbtnTemp = NULL;
			pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnRobot")));
			if (pbtnTemp)
			{
				pbtnTemp->SetVisible(true);
			}	
		}
		else
		{
			CButtonUI *pbtnTemp = NULL;
			pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnParam")));
			if (pbtnTemp)
			{
				pbtnTemp->SetVisible(false);
			}
			pbtnTemp = NULL;
			pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnRobot")));
			if (pbtnTemp)
			{
				pbtnTemp->SetVisible(false);
			}
		}
	}
	else if (msg.pSender->GetName() == _T("BtnChangePswd"))
	{
		ChangePsw(m_hWnd);
	}
	msg.pSender->SetMouseEnabled(true);
	__super::OnClick(msg);
}

void CMainView::OnSelChanged(DuiLib::TNotifyUI& msg)
{
	//主要用于List Combo 的SelChanged  这地方好像不能出现阻塞模式的
	msg.pSender->SetMouseEnabled(false);
	CHECKRESULT(m_classProduct.OnSelChanged(msg), msg);
	CHECKRESULT(m_classRobotView.OnSelChanged(msg), msg);
	msg.pSender->SetMouseEnabled(true);
	__super::OnClick(msg);
}

void CMainView::OnOpChanged(DuiLib::TNotifyUI& msg)
{
	//主要用于Option操作的选择切换
	CDuiString strTreeName = _T("");
	CDuiString strIndex = _T("");
	strTreeName = (static_cast<COptionUI *>(msg.pSender))->GetGroup();
	strIndex = (static_cast<COptionUI *>(msg.pSender))->GetUserData();

	CTabLayoutUI *pTabUi = static_cast<CTabLayoutUI *>(m_PaintManager.FindControl(strTreeName));

	if (pTabUi)
	{
		if ( strTreeName == _T("MainViewBar"))
		{
			if ((m_ciniConfig.ReadInt(_T("Login"), _T("LoginMode"), 0) == 1))
			{
				if (_ttoi(strIndex) == 0)
				{
					m_dwSTTime= ::GetTickCount();
					m_bSTTime = TRUE;
				}
				else
				{
					m_bSTTime = FALSE;
				}

			}
			else
			{
				if (_ttoi(strIndex) == 0)
				{
					CButtonUI *pbtnTemp = NULL;
					g_sUserInfo.Init();
					pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnParam")));
					if (pbtnTemp)
					{
						pbtnTemp->SetVisible(false);
					}
					pbtnTemp = NULL;
					pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnRobot")));
					if (pbtnTemp)
					{
						pbtnTemp->SetVisible(false);
					}
				}
				else
				{
					m_bSTTime = FALSE;
				}
			}
		}
	
		
		pTabUi->SelectItem(_ttoi(strIndex));

	}

	__super::OnClick(msg); 
}

void CMainView::SetParent(HWND hParentWnd)
{
	m_hParentWnd = hParentWnd;
}

LRESULT CMainView::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ULMSG_IOSTATUS:
		{
			m_classIOView.UpdataStatus();
		}
		break;

	case WM_ULMSG_LOCALTIME:
		{
			UpdataLocalTime();
			if (m_ciniConfig.ReadInt(_T("Login"), _T("LoginMode"), 0) == 1)
			{
				if (m_bSTTime)
				{
					int iTimeOut = m_ciniConfig.ReadInt(_T("Login"), _T("TimeOut"), 3);
					if ((::GetTickCount() - m_dwSTTime) > (iTimeOut * 60 * 1000))
					{
						CButtonUI *pbtnTemp = NULL;
						g_sUserInfo.Init();
						pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnParam")));
						if (pbtnTemp)
						{
							pbtnTemp->SetVisible(false);
						}
						pbtnTemp = NULL;
						pbtnTemp = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnRobot")));
						if (pbtnTemp)
						{
							pbtnTemp->SetVisible(false);
							m_bSTTime = FALSE;
						}
					}
				}
			}
		}
		break;

	case WM_ULMSG_UPDATAUPH:
		{
			m_classHomeView.UpdataUph();
		}
		break;

	case WM_ULMSG_RESETPCB:
		{
			switch (wParam)
			{
			case 0:
				{
					m_classHomeView.ClearAllRstBoard();
				}
				break;

			case 1:
				{
					m_classHomeView.ClearAllOKBoard();
				}
				break;

			case 2:
				{
					m_classHomeView.ClearAllNGBoard();
				}
				break;
			}
		}
		break;

	case WM_ULMSG_PUTOKP0S:
		{
			m_classHomeView.SetOKBoard(wParam);
		}
		break;

	case WM_ULMSG_PUTNGP0S:
		{
			m_classHomeView.SetNGBoard(wParam);
		}
		break;

	case WM_ULMSG_PICKRSTPOS:
		{
			m_classHomeView.ClearRstBoard(wParam);
		}
		break;

	case WM_ULMSG_UPDATARST:
		{
			m_classHomeView.UpdataRst();
		}
		break;


	case WM_ULMSG_WARNING:
		{
			CDuiString strKey = _T("");
			CDuiString strMessage = _T("");
			CDuiString strTime = _T("");
			strKey.Format(_T("%d"), wParam);
			SYSTEMTIME sysTime;
			LRESULT lRst = 0;
			::GetLocalTime(&sysTime);
			strTime.Format(_T("%02d:%02d:%02d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			strMessage = m_ciniMessage.ReadString(_T("Alarm"), strKey.GetData(), _T("未知报错代码")).c_str();
			strKey.Format(_T("错误代码: %d."), wParam);
			AddMessage(m_pVerAlarmMessage, strTime.GetData(), strMessage.GetData(), uMsg);
			lRst = FMessageBox(m_hWnd, strMessage.GetData(), strKey.GetData());
			if (wParam == 15)
			{
				int iTime = m_ciniConfig.ReadInt(_T("PassWordTime"), _T("Time"), 15);

				if (iTime < 5 || iTime > 30)
				{
					iTime = 15;
				}

				if (::GetTickCount() - m_dwSTPsw > (iTime * 1000))
				{
relogin:
					Login(m_hWnd);

					if (g_sUserInfo.GetLevel() < 5)
					{
						g_sUserInfo.Init();
						m_dwSTPsw = ::GetTickCount();
					}
					else
					{
						goto relogin;
					}
				}
			}
			



			CreateLog(strTime.GetData(), wParam, strMessage.GetData(), lRst, 0);
			return lRst;
		}
		break;

	case WM_ULMSG_INFORMATION:
		{
			CDuiString strKey = _T("");
			CDuiString strMessage = _T("");
			strKey.Format(_T("%d"), wParam);
			CDuiString strTime = _T("");
			LRESULT lRst = 0;
			SYSTEMTIME sysTime;
			::GetLocalTime(&sysTime);
			strTime.Format(_T("%02d:%02d:%02d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			strMessage = m_ciniMessage.ReadString(_T("Message"), strKey.GetData(), _T("未知消息代码")).c_str();
			strKey.Format(_T("提示: %d."), wParam);
			AddMessage(m_pVerInfoMessage, strTime.GetData(), strMessage.GetData(), uMsg);
			lRst = FMessageBox(m_hWnd, strMessage.GetData(), strKey.GetData());
			CreateLog(strTime.GetData(), wParam, strMessage.GetData(), lRst, 1);
			return lRst;
		}
		break;

	case WM_ULMSG_INFORMATION_V:
		{
			CDuiString strKey = _T("");
			CDuiString strMessage = _T("");
			strKey.Format(_T("%d"), wParam);
			CDuiString strTime = _T("");
			SYSTEMTIME sysTime;
			::GetLocalTime(&sysTime);
			strTime.Format(_T("%02d:%02d:%02d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			strMessage = m_ciniMessage.ReadString(_T("Message"), strKey.GetData(), _T("未知消息代码")).c_str();
			AddMessage(m_pVerInfoMessage, strTime.GetData(), strMessage.GetData(), WM_ULMSG_INFORMATION);	
			CreateLog(strTime.GetData(), wParam, strMessage.GetData(), -1, 1);
			return 0;
		}
		break;

	case WM_ULMSG_CHANGEPORDUCT:
		{
			m_classRobotView.InitWindow(&m_PaintManager, m_pIWorkFlow);
			TCHAR atcName[128] = {0};
			CDuiString strMessage = _T("");
			CDuiString strTime = _T("");
			SYSTEMTIME sysTime;
			m_pIWorkFlow->GetCurrentProduct(atcName);
			::GetLocalTime(&sysTime);
			strTime.Format(_T("%02d:%02d:%02d"), sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
			strMessage.Format(_T("产品切换: %s -> %s"), m_strCurProduct.c_str(), atcName);
			AddMessage(m_pVerInfoMessage, strTime.GetData(), strMessage.GetData(), WM_ULMSG_INFORMATION);	
			CreateLog(strTime.GetData(), 0, strMessage.GetData(), -1, 1);
			UpdataTitle();
			m_classHomeView.UpdataStatus();
			m_classHomeView.UpdataUph();
			return 0;
		}
		break;

	case WM_ULMSG_START:
		{
			OnStart();
		}
		break;

	case WM_ULMSG_STOP:
		{
			OnStop();
		}
		break;

	case WM_ULMSG_RESET:
		{
			OnReset();
		}
		break;	

	case WM_ULMSG_SUSPEND:
		{
			if (m_pBtnStart)
			{
				m_pBtnStart->SetEnabled(false);

				if (m_pIWorkFlow->IsRunning())
				{
					if (m_pIWorkFlow->IsSuspend())
					{
						m_pBtnStart->SetForeImage(_T("file='Start.png' dest='0,0,64,64'"));
						m_pBtnStart->SetText(_T("恢复"));
					}
					else
					{
						m_pBtnStart->SetForeImage(_T("file='Suspend.png' dest='0,0,64,64'"));
						m_pBtnStart->SetText(_T("暂停"));
					}	
				}
				else
				{
					m_pBtnStart->SetForeImage(_T("file='Start.png' dest='0,0,64,64'"));
					m_pBtnStart->SetText(_T("运行"));
				}
				m_pBtnStart->SetEnabled(true);
			}
		}
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void CMainView::CreateLog(LPCTSTR lpstrTime, int iCode, LPCTSTR lpstrData, const int iSelect, const int iType)
{
	CDuiString strData = _T("");
	switch (iSelect)
	{
	case IDNO:
		{
			strData.Format(_T("%s,%d,%s,IDNO"), lpstrTime, iCode, lpstrData);
		}
		break;

	case IDYES:
		{
			strData.Format(_T("%s,%d,%s,IDYES"), lpstrTime, iCode, lpstrData);
		}
		break;

	case IDCANCEL:
		{
			strData.Format(_T("%s,%d,%s,IDCANCEL"), lpstrTime, iCode, lpstrData);
		}
		break;

	case -1:
		{
			strData.Format(_T("%s,%d,%s,,"), lpstrTime, iCode, lpstrData);
		}
		break;
	}

	m_pIWorkFlow->RecordLog(strData.GetData(), iType);
}

LRESULT CMainView::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if ((uMsg == WM_KEYDOWN) && (wParam == VK_ESCAPE))
	{
		return FALSE;
	}

	return __super::MessageHandler(uMsg, wParam, lParam, bHandled);
}

void CMainView::UpdataLocalTime()
{
	CDuiString strTime = _T("");
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	CLabelUI* pLocalTime = static_cast<CLabelUI *>(m_PaintManager.FindControl(_T("LblLocalTime")));

	if (pLocalTime)
	{
		pLocalTime->SetText(strTime);
	}

	CButtonUI *pBtnUI = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnCurHour")));

	if (pBtnUI)
	{
		strTime.Format(_T("%02d -- %02d"), sysTime.wHour, sysTime.wHour + 1);
		pBtnUI->SetText(strTime);
	}

	CButtonUI* pBtnRobotST = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnRobotStatus")));

	if (pBtnRobotST && m_pIWorkFlow && m_pIWorkFlow->IsRobotConnect())
	{
		pBtnRobotST->SetNormalImage(_T("file='GreenLed.png' dest='0,10,20,30'"));
		pBtnRobotST->SetText(_T("Robot已连接"));
	}
	else
	{
		if (pBtnRobotST)
		{
			pBtnRobotST->SetNormalImage(_T("file='RedLed.png' dest='0,10,20,30'"));
			pBtnRobotST->SetText(_T("Robot已断开"));
		}
	}

	pBtnRobotST = static_cast<CButtonUI *>(m_PaintManager.FindControl(_T("BtnVisionStatus")));

	if (pBtnRobotST && m_pIWorkFlow && m_pIWorkFlow->IsVisionConnect())
	{
		pBtnRobotST->SetNormalImage(_T("file='GreenLed.png' dest='0,10,20,30'"));
		pBtnRobotST->SetText(_T("Vision已连接"));
	}
	else
	{
		if (pBtnRobotST)
		{
			pBtnRobotST->SetNormalImage(_T("file='RedLed.png' dest='0,10,20,30'"));
			pBtnRobotST->SetText(_T("Vision已断开"));
		}
	}

	::PostMessage(m_hWnd, WM_ULMSG_UPDATAUPH, NULL, NULL);
}

void CMainView::ShowLog(const LPCTSTR lpstrDir)
{
	if (!ShellExecute(NULL, _T("open"), lpstrDir, NULL, NULL, SW_SHOW))
	{
		switch (GetLastError())
		{
		case ERROR_FILE_NOT_FOUND:
			{
				FMessageBox(m_hWnd, _T("文件名错误"), _T("错误"));
			}
			break;

		case ERROR_PATH_NOT_FOUND:
			{
				FMessageBox(m_hWnd, _T("路径名错误"), _T("错误"));
			}
			break;

		case ERROR_BAD_FORMAT:
			{
				FMessageBox(m_hWnd, _T("EXE 文件无效"), _T("错误"));
			}
			break;

		case SE_ERR_SHARE:
			{
				FMessageBox(m_hWnd, _T("发生共享错误"), _T("错误"));
			}
			break;

		case SE_ERR_ASSOCINCOMPLETE:
			{
				FMessageBox(m_hWnd, _T("文件名不完全或无效"), _T("错误"));
			}
			break;

		case SE_ERR_DDETIMEOUT:
			{
				FMessageBox(m_hWnd, _T("超时"), _T("错误"));
			}
			break;

		case SE_ERR_DDEFAIL:
			{
				FMessageBox(m_hWnd, _T("DDE 事务失败"), _T("错误"));
			}
			break;

		case SE_ERR_DDEBUSY:
			{
				FMessageBox(m_hWnd, _T("正在处理其他 DDE 事务而不能完成该 DDE 事务"), _T("错误"));
			}
			break;

		case SE_ERR_NOASSOC:
			{
				FMessageBox(m_hWnd, _T("没有相关联的应用程序"), _T("错误"));
			}
			break;
		}
	}
	
}

void CMainView::UpdataTitle()
{
	if (m_pLblTitle && m_pIWorkFlow)
	{
		TCHAR atcName[128] = {0};
		m_pIWorkFlow->GetCurrentProduct(atcName);
		CDuiString strTitle = _T("");
		m_strCurProduct = atcName;
		strTitle.Format(_T("尺寸检测 : %s"), atcName);
		m_pLblTitle->SetText(strTitle);
	}
}

void CMainView::OnReset()
{
	m_pBtnReset->SetEnabled(false);
	if (m_pIWorkFlow->Reset())
	{
		if (m_pBtnStart && m_pBtnStop)
		{
			m_pBtnStart->SetEnabled(true);
			m_pBtnStop->SetEnabled(true);
		}
	}
	else
	{
		if (m_pBtnStart && m_pBtnStop)
		{
			m_pBtnStart->SetEnabled(false);
			m_pBtnStop->SetEnabled(false);
		}
	}
	m_pBtnReset->SetEnabled(true);
}

void CMainView::OnStart()
{
	m_pBtnStart->SetEnabled(false);
	if (m_pIWorkFlow->IsRunning())
	{
		if (m_pIWorkFlow->IsSuspend())
		{
			m_pIWorkFlow->Resume();
		}
		else
		{
			m_pIWorkFlow->Suspend();
		}
	}
	else 
	{
		if (m_pIWorkFlow->Start())
		{
			if (m_pBtnReset)
			{
				m_pBtnReset->SetEnabled(false);
			}
		}
	}

	if (m_pIWorkFlow->IsRunning())
	{
		if (m_pIWorkFlow->IsSuspend())
		{
			m_pBtnStart->SetForeImage(_T("file='Start.png' dest='0,0,64,64'"));
			m_pBtnStart->SetText(_T("恢复"));
		}
		else
		{
			m_pBtnStart->SetForeImage(_T("file='Suspend.png' dest='0,0,64,64'"));
			m_pBtnStart->SetText(_T("暂停"));
		}	
	}
	else
	{
		m_pBtnStart->SetForeImage(_T("file='Start.png' dest='0,0,64,64'"));
		m_pBtnStart->SetText(_T("运行"));
	}
	m_pBtnStart->SetEnabled(true);
}

void CMainView::OnStop()
{
	m_pBtnStop->SetEnabled(false);
	if (m_pIWorkFlow->Stop() && m_pBtnReset && m_pBtnStop && m_pBtnStart)
	{
		m_pBtnStart->SetEnabled(false);
		m_pBtnStop->SetEnabled(false);
		m_pBtnReset->SetEnabled(true);
		m_pBtnStart->SetForeImage(_T("file='Start.png' dest='0,0,64,64'"));
		m_pBtnStart->SetText(_T("运行"));
	}
}