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
		strMsg.Format(_T("ȷ��Ҫ����Z1��Tray����ƫ�Ʊ��� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			m_pIWorkFlow->StoreOffSet(1);
		}
		
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnCalibZ1Pos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��ҪУ׼Z1��Tray������λ�� У׼ (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			m_pIWorkFlow->CalibTrayHavePos(1);
		}
		
		bRet = TRUE;
	}
	if (msg.pSender->GetName() == _T("BtnStoreZ2Offset"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ����Z2��Tray����ƫ�Ʊ��� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			m_pIWorkFlow->StoreOffSet(2);
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnStoreZ2Pos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ����Z1��Tray�����߶� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			m_pIWorkFlow->StoreTrayPos(1);
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnCalibZ2Pos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��ҪУ׼Z2��Tray������λ�� У׼ (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			m_pIWorkFlow->CalibTrayHavePos(2);
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnHomeZ1"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��ҪZ1���㣿 ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->GoHome(1))
			{
				FMessageBox(NULL, _T("Z1����ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("Z1����ʧ��!"), _T("��ʾ"));
			}
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnHomeZ2"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��ҪZ2���㣿 ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->GoHome(2))
			{
				FMessageBox(NULL, _T("Z2����ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("Z2����ʧ��!"), _T("��ʾ"));
			}
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnTrayNR"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ����Tray�� ��ȷ�Ϻ����е�λ���Ѿ�ѧϰ��ϣ���Ҫ����ʱ�볤����̨��[ֹͣ]����ť�� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			m_pIWorkFlow->TrayNR();
			FMessageBox(NULL, _T("Tray ����ֹͣ!"), _T("��ʾ"));
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnSetECheckPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ����Z2���λ�ã� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->SetECheckPos())
			{
				FMessageBox(NULL, _T("����Z2���λ�� �ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("����Z2���λ�� ʧ��!"), _T("��ʾ"));
			}
			
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnMoveToECheckPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ�ƶ���Z2���λ�ã� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->MoveToECheckPos())
			{
				FMessageBox(NULL, _T("�ƶ�λ�� �ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("�ƶ�λ�� ʧ��!"), _T("��ʾ"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnSetFCheckPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ����Z1���λ�ã� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->SetFCheckPos())
			{
				FMessageBox(NULL, _T("����Z2���λ�� �ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("����Z2���λ�� ʧ��!"), _T("��ʾ"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnMoveToFCheckPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ�ƶ���Z1���λ�ã� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->MoveToFCheckPos())
			{
				FMessageBox(NULL, _T("�ƶ�λ�� �ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("�ƶ�λ�� ʧ��!"), _T("��ʾ"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnSetEOpenPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ����Z2��λ�ã� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->SetEOpenPos())
			{
				FMessageBox(NULL, _T("����Z2��λ�� �ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("����Z2��λ�� ʧ��!"), _T("��ʾ"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnMoveToEOpenPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ�ƶ���Z2��λ�ã� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->MoveToEOpenPos())
			{
				FMessageBox(NULL, _T("�ƶ�λ�� �ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("�ƶ�λ�� ʧ��!"), _T("��ʾ"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnSetFOpenPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ����Z1��λ�ã� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->SetFOpenPos())
			{
				FMessageBox(NULL, _T("����Z1��λ�� �ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("����Z1��λ�� ʧ��!"), _T("��ʾ"));
			}

		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnMoveToFOpenPos"))
	{
		CDuiString strMsg = _T("");
		strMsg.Format(_T("ȷ��Ҫ�ƶ���Z1��λ�ã� ���� (Yes) ȡ��(No)"));
		if (FMessageBox(NULL, strMsg, _T("��ʾ")) == IDYES)
		{
			if (m_pIWorkFlow->MoveToFOpenPos())
			{
				FMessageBox(NULL, _T("�ƶ�λ�� �ɹ�!"), _T("��ʾ"));
			}
			else
			{
				FMessageBox(NULL, _T("�ƶ�λ�� ʧ��!"), _T("��ʾ"));
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