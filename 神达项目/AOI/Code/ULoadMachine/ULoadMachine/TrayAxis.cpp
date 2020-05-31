#include "stdafx.h"
#include "TrayAxis.h"


void CTrayAxis::InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow)
{
	m_pPaintManagerUI = pPaintManagerUI;
	m_pIWorkFlow = pIWorkFlow;

	CComboUI *pCmbUI = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbTrayAxisDis")));

	if (pCmbUI)
	{
		AddComboItem(pCmbUI, _T("10"));
		AddComboItem(pCmbUI, _T("50"));
		AddComboItem(pCmbUI, _T("100"));
		AddComboItem(pCmbUI, _T("500"));
		AddComboItem(pCmbUI, _T("1000"));
		AddComboItem(pCmbUI, _T("5000"));
		AddComboItem(pCmbUI, _T("10000"));
		AddComboItem(pCmbUI, _T("20000"));
		pCmbUI->SelectItem(0);
	}
}

BOOL CTrayAxis::OnClick(DuiLib::TNotifyUI& msg)
{
	BOOL bRet = FALSE;
	if (msg.pSender->GetName() == _T("BtnZ1Add"))
	{
		MoveDistance(1, TRUE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnZ1Dec"))
	{
		MoveDistance(1, FALSE);
		bRet = TRUE;
	}
	if (msg.pSender->GetName() == _T("BtnZ2Add"))
	{
		MoveDistance(2, TRUE);
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnZ2Dec"))
	{
		MoveDistance(2, FALSE);
		bRet = TRUE;
	}
	if (msg.pSender->GetName() == _T("BtnStoreZ1Offset"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要保存Z1有Tray上升偏移保存 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			m_pIWorkFlow->StoreOffSet(1);
		}
		
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnCalibZ1Pos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要校准Z1有Tray传感器位置 校准 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			m_pIWorkFlow->CalibTrayHavePos(1);
		}
		
		bRet = TRUE;
	}
	if (msg.pSender->GetName() == _T("BtnStoreZ2Offset"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要保存Z2有Tray上升偏移保存 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			m_pIWorkFlow->StoreOffSet(2);
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnStoreZ2Pos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要保存Z1无Tray上升高度 保存 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			m_pIWorkFlow->StoreTrayPos(1);
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnCalibZ2Pos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要校准Z2有Tray传感器位置 校准 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			m_pIWorkFlow->CalibTrayHavePos(2);
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnHomeZ1"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要Z1回零？ 回零 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->GoHome(1))
			{
				FMessageBox(NULL, _T("Z1回零成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("Z1回零失败!"), _T("提示"));
			}
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnHomeZ2"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要Z2回零？ 回零 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->GoHome(2))
			{
				FMessageBox(NULL, _T("Z2回零成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("Z2回零失败!"), _T("提示"));
			}
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnTrayNR"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要空跑Tray盘 请确认好所有点位都已经学习完毕，需要结束时请长按机台上[停止]物理按钮？ 空跑 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			m_pIWorkFlow->TrayNR();
			FMessageBox(NULL, _T("Tray 空跑停止!"), _T("提示"));
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnSetECheckPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要设置Z2检测位置？ 设置 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->SetECheckPos())
			{
				FMessageBox(NULL, _T("设置Z2检测位置 成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("设置Z2检测位置 失败!"), _T("提示"));
			}
			
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnMoveToECheckPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要移动到Z2检测位置？ 设置 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->MoveToECheckPos())
			{
				FMessageBox(NULL, _T("移动位置 成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("移动位置 失败!"), _T("提示"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnSetFCheckPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要设置Z1检测位置？ 设置 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->SetFCheckPos())
			{
				FMessageBox(NULL, _T("设置Z2检测位置 成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("设置Z2检测位置 失败!"), _T("提示"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnMoveToFCheckPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要移动到Z1检测位置？ 设置 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->MoveToFCheckPos())
			{
				FMessageBox(NULL, _T("移动位置 成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("移动位置 失败!"), _T("提示"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnSetEOpenPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要设置Z2打开位置？ 设置 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->SetEOpenPos())
			{
				FMessageBox(NULL, _T("设置Z2打开位置 成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("设置Z2打开位置 失败!"), _T("提示"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnMoveToEOpenPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要移动到Z2打开位置？ 设置 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->MoveToEOpenPos())
			{
				FMessageBox(NULL, _T("移动位置 成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("移动位置 失败!"), _T("提示"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnSetFOpenPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要设置Z1打开位置？ 设置 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->SetFOpenPos())
			{
				FMessageBox(NULL, _T("设置Z1打开位置 成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("设置Z1打开位置 失败!"), _T("提示"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnMoveToFOpenPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("确定要移动到Z1打开位置？ 设置 (Yes) 取消(No)"));
		if (FMessageBox(NULL, strMsg, _T("提示")) == IDYES)
		{
			if (m_pIWorkFlow->MoveToFOpenPos())
			{
				FMessageBox(NULL, _T("移动位置 成功!"), _T("提示"));
			}
			else
			{
				FMessageBox(NULL, _T("移动位置 失败!"), _T("提示"));
			}

		}
		bRet = TRUE;
	}

	return bRet;
}

void CTrayAxis::ViewCurPos()
{
	if (m_pIWorkFlow)
	{
		long lPos = 0;
		CDuiString strValue = _T("");
		CEditUI *pEditUI = NULL;
		lPos = m_pIWorkFlow->GetAxisPos(1);
		strValue.Format(_T("%d"), lPos);

		pEditUI = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ1Pos")));

		if (pEditUI)
		{
			pEditUI->SetText(strValue);
		}

		pEditUI = NULL;
		lPos = m_pIWorkFlow->GetAxisPos(2);
		pEditUI = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtZ2Pos")));
		strValue.Format(_T("%d"), lPos);
		if (pEditUI)
		{
			pEditUI->SetText(strValue);
		}

		pEditUI = NULL;
	}
}

void CTrayAxis::MoveDistance(const int iAxis, BOOL bDir)
{
	CComboUI *pCmbUI = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbTrayAxisDis")));

	int iLength = 0;

	if (pCmbUI)
	{
		iLength = _ttoi(pCmbUI->GetText());

		if (!bDir)
		{
			iLength = -iLength;
		}

		m_pIWorkFlow->AxisMoveDis(iAxis, iLength);

		ViewCurPos();
	}
}