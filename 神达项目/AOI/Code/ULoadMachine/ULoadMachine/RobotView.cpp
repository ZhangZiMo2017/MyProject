#include "stdafx.h"
#include "RobotView.h"

void CRobotView::InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow)
{
	m_pPaintManagerUI = pPaintManagerUI;
	m_pIWorkFlow = pIWorkFlow;
	if (m_pPaintManagerUI)
	{
		CListUI *pListUI = NULL;
		pListUI = static_cast<CListUI *>(m_pPaintManagerUI->FindControl(_T("LstRobotPos")));

		if (pListUI)
		{
			RemoveListAllItem(pListUI);
			if (m_pIWorkFlow)
			{
				for (int i = 0; i < m_pIWorkFlow->GetPosCount(); ++i)
				{
					CContainerUI *pSubUi = AddListItem(pListUI, _T("Pos"));
					TCHAR ptcName[128] = {0};
					double dXPos = 0.0;
					double dYPos = 0.0;
					double dZPos = 0.0;
					double dUPos = 0.0;
					m_pIWorkFlow->GetPosValue(i, ptcName, dXPos, dYPos, dZPos, dUPos);
					pSubUi->GetItemAt(0)->SetText(ptcName);
					swprintf(ptcName, _T("%.3f"), dXPos);
					pSubUi->GetItemAt(1)->SetText(ptcName);
					swprintf(ptcName, _T("%.3f"), dYPos);
					pSubUi->GetItemAt(2)->SetText(ptcName);
					swprintf(ptcName, _T("%.3f"), dZPos);
					pSubUi->GetItemAt(3)->SetText(ptcName);
					swprintf(ptcName, _T("%.3f"), dUPos);
					pSubUi->GetItemAt(4)->SetText(ptcName);
				}
			}
		}

		CComboUI *pCmbDis = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbDistance")));

		if (pCmbDis)
		{
			RemoveComboAllItem(pCmbDis);
			AddComboItem(pCmbDis, _T("0.01"));
			AddComboItem(pCmbDis, _T("0.05"));
			AddComboItem(pCmbDis, _T("0.1"));
			AddComboItem(pCmbDis, _T("0.5"));
			AddComboItem(pCmbDis, _T("1.0"));
			AddComboItem(pCmbDis, _T("5.0"));
			AddComboItem(pCmbDis, _T("10.0"));
			AddComboItem(pCmbDis, _T("20.0"));
			pCmbDis->SelectItem(0);
		}

		pCmbDis = NULL;
		pCmbDis = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbTool")));
		if (pCmbDis)
		{
			RemoveComboAllItem(pCmbDis);
			AddComboItem(pCmbDis, _T("0"));
			AddComboItem(pCmbDis, _T("1"));
			
			pCmbDis->SelectItem(0);
		}
	}
}

BOOL CRobotView::OnClick(DuiLib::TNotifyUI& msg)
{
	BOOL bRet = FALSE;
	CContainerUI *pContainerUI = NULL;

	if (msg.pSender->GetName() == _T("BtnPosMove"))
	{
		pContainerUI = static_cast<CContainerUI *>(msg.pSender->GetParent());
		CDuiString strName = pContainerUI->GetItemAt(0)->GetText();
		if (m_pIWorkFlow)
		{
			m_pIWorkFlow->JumpPos(strName.GetData());	
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnPosSet"))
	{
		pContainerUI = static_cast<CContainerUI *>(msg.pSender->GetParent());
		CDuiString strName = pContainerUI->GetItemAt(0)->GetText();
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要设定位置[%s] 设定 (Yes) 取消(No)"), strName);
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow)
			{
				m_pIWorkFlow->StorePos(strName.GetData());
				double dXPos = 0.0;
				double dYPos = 0.0;
				double dZPos = 0.0;
				double dUPos = 0.0;
				TCHAR ptcName[128] = {0};
				m_pIWorkFlow->QryCurPos(dXPos, dYPos, dZPos, dUPos);
				swprintf(ptcName, _T("%.3f"), dXPos);
				pContainerUI->GetItemAt(1)->SetText(ptcName);
				swprintf(ptcName, _T("%.3f"), dYPos);
				pContainerUI->GetItemAt(2)->SetText(ptcName);
				swprintf(ptcName, _T("%.3f"), dZPos);
				pContainerUI->GetItemAt(3)->SetText(ptcName);
				swprintf(ptcName, _T("%.3f"), dUPos);
				pContainerUI->GetItemAt(4)->SetText(ptcName);
			}
		}
		
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnPosStore"))
	{
		pContainerUI = static_cast<CContainerUI *>(msg.pSender->GetParent());
		CDuiString strName = pContainerUI->GetItemAt(0)->GetText();
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要保存位置[%s] 保存 (Yes) 取消(No)"), strName);
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow)
			{

				double dXPos = 0.0;
				double dYPos = 0.0;
				double dZPos = 0.0;
				double dUPos = 0.0;

				dXPos = _ttof(pContainerUI->GetItemAt(1)->GetText());
				dYPos = _ttof(pContainerUI->GetItemAt(2)->GetText());
				dZPos = _ttof(pContainerUI->GetItemAt(3)->GetText());
				dUPos = _ttof(pContainerUI->GetItemAt(4)->GetText());
				m_pIWorkFlow->StorePos(strName.GetData(), dXPos, dYPos, dZPos, dUPos);
			}
		}

		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnXAdd"))
	{
		MoveDistance(_T("XAxis"), TRUE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnYAdd"))
	{
		MoveDistance(_T("YAxis"), TRUE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnZAdd"))
	{
		MoveDistance(_T("ZAxis"), TRUE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnUAdd"))
	{
		MoveDistance(_T("UAxis"), TRUE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnXDec"))
	{
		MoveDistance(_T("XAxis"), FALSE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnYDec"))
	{
		MoveDistance(_T("YAxis"), FALSE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnZDec"))
	{
		MoveDistance(_T("ZAxis"), FALSE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnUDec"))
	{
		MoveDistance(_T("UAxis"), FALSE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnSet"))
	{
		if (m_pIWorkFlow)
		{
			CDuiString strSpeed = _T("10");
			CEditUI *pEditUI = NULL;
			pEditUI = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtSpeed")));

			if (pEditUI)
			{
				strSpeed = pEditUI->GetText();
			}

			m_pIWorkFlow->SetSpeed(_ttoi(strSpeed));
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnCalibNGPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要校准NG位置? 校准 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow)
			{
				m_pIWorkFlow->CalibPCB(1);
			}
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnCalibOKPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要校准OK位置? 校准 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow)
			{
				m_pIWorkFlow->CalibPCB(0);
			}
		}
		bRet = TRUE;
	}//
	else if (msg.pSender->GetName() == _T("BtnCalibCamera"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要校准底部相机? 校准 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow)
			{
				m_pIWorkFlow->CalibCamera();
			}
		}
		bRet = TRUE;
	}//
	else if (msg.pSender->GetName() == _T("BtnCalibPickPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要校准取放位置? 校准 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow)
			{
				m_pIWorkFlow->CalibPickTool();
			}
		}
		bRet = TRUE;
	}//

	return bRet;
}

void CRobotView::ViewCurPos()
{
	if (m_pIWorkFlow)
	{
		double dXPos = 0.0;
		double dYPos = 0.0;
		double dZPos = 0.0;
		double dUPos = 0.0;
		CLabelUI *pLblXPos = NULL;
		CLabelUI *pLblYPos = NULL;
		CLabelUI *pLblZPos = NULL;
		CLabelUI *pLblUPos = NULL;

		pLblXPos = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(_T("LblXPos")));
		pLblYPos = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(_T("LblYPos")));
		pLblZPos = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(_T("LblZPos")));
		pLblUPos = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(_T("LblUPos")));

		m_pIWorkFlow->QryCurPos(dXPos, dYPos, dZPos, dUPos);
		CDuiString strValue = _T("");

		if (pLblXPos)
		{
			strValue.Format(_T("%.3f"), dXPos);
			pLblXPos->SetText(strValue);
		}

		if (pLblYPos)
		{
			strValue.Format(_T("%.3f"), dYPos);
			pLblYPos->SetText(strValue);
		}

		if (pLblZPos)
		{
			strValue.Format(_T("%.3f"), dZPos);
			pLblZPos->SetText(strValue);
		}

		if (pLblUPos)
		{
			strValue.Format(_T("%.3f"), dUPos);
			pLblUPos->SetText(strValue);
		}
	}
}

void CRobotView::MoveDistance(LPCTSTR lpstrAxis, BOOL bDir)
{
	CComboUI *pCmbDis = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbDistance")));
	if (pCmbDis && m_pIWorkFlow)
	{
		tstring strDistance = _T("");
		strDistance = pCmbDis->GetText();
		if (bDir)
		{
			m_pIWorkFlow->GotoDistance(lpstrAxis, _ttof(strDistance.c_str()));
		}
		else
		{
			m_pIWorkFlow->GotoDistance(lpstrAxis, -_ttof(strDistance.c_str()));
		}
		
		m_pIWorkFlow->IsRobotBusy();
		ViewCurPos();
	}
}

void CRobotView::AxisMoveDiatance(BOOL bDir)
{
	CComboUI *pCmbDis = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbDistance")));
	//CmbAxis
	CComboUI *pCmbAxis = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbAxis")));
	if (pCmbDis && m_pIWorkFlow && pCmbAxis)
	{
		tstring strDistance = _T("");
		strDistance = pCmbDis->GetText();
		if (bDir)
		{
			m_pIWorkFlow->AxisMoveDis(_ttoi(pCmbAxis->GetText().GetData()), _ttof(strDistance.c_str()) * 1000);
		}
		else
		{
			m_pIWorkFlow->AxisMoveDis(_ttoi(pCmbAxis->GetText().GetData()), -_ttof(strDistance.c_str()) * 1000);
		}
		
	}
}

BOOL CRobotView::OnSelChanged(DuiLib::TNotifyUI& msg)
{
	BOOL bRet = FALSE;
	if (msg.pSender->GetName() == _T("CmbTool"))
	{
		if (m_pIWorkFlow)
		{
			m_pIWorkFlow->ChangeTool(_ttoi(msg.pSender->GetText()));
			ViewCurPos();
		}

		bRet= TRUE;
	}
	return bRet;
}