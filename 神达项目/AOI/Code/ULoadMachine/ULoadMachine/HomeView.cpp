#include "stdafx.h"
#include "HomeView.h"

void CHomeView::InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow)
{
	m_pIWorkFlow = pIWorkFlow;
	m_pPaintManagerUI = pPaintManagerUI;
	UpdataStatus();
	UpdataUph();
	m_classConfig.SetIniFile((m_classConfig.GetModulePath() + _T("Config\\Config.ini")).c_str());
}

BOOL CHomeView::OnClick(DuiLib::TNotifyUI& msg)
{
	BOOL bRet = FALSE;

	return bRet;
}

void CHomeView::UpdataStatus()
{
	if (m_pPaintManagerUI && m_pIWorkFlow)
	{
		int iIndex = 0;
		int iRow = 0;
		int iCol = 0;
		SIZE  sProductSize;
		CDuiString strName = _T("");
		sProductSize = m_pIWorkFlow->GetProductSize();

		for (iRow = 0; iRow < ROW; iRow++)
		{
			for (iCol = 0; iCol < COL; ++iCol)
			{
				strName.Format(_T("BtnRst%d_%d"), iRow, iCol);
				CLabelUI *pButtnUi = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(strName));

				if (pButtnUi)
				{
					if (iRow < sProductSize.cx && iCol < sProductSize.cy)
					{
						pButtnUi->SetBkColor(0xFFE0E0E0);
						strName.Format(_T("%d"), iRow * sProductSize.cy + iCol + 1);
						pButtnUi->SetText(strName);
						pButtnUi->SetMouseEnabled(false);
						pButtnUi->SetVisible(true);
					}
					else
					{

						if (iRow >= sProductSize.cx)
						{
							pButtnUi->GetParent()->SetVisible(false);
						}
						pButtnUi->SetVisible(false);
					}
					
					
				}
			}
		}

		sProductSize = m_pIWorkFlow->GetTraySize();
		CHorizontalLayoutUI *pHorOKUI = static_cast<CHorizontalLayoutUI *>(m_pPaintManagerUI->FindControl(_T("Hor_OKBoard")));
		if (pHorOKUI)
		{
			pHorOKUI->RemoveAll();
		}

		if (pHorOKUI)
		{
			pHorOKUI->RemoveAll();
		}
		for (iCol = 0; iCol < sProductSize.cy && pHorOKUI; ++iCol)
		{
			DuiLib::CDialogBuilder builder;
			DuiLib::CDuiString strXML = _T("");
			strXML += _T("Board.xml");
			DuiLib::CContainerUI* pSubUi = static_cast<DuiLib::CContainerUI*>(builder.Create(strXML.GetData(), (UINT)0, NULL, NULL, NULL));

			for (iRow = 0; iRow < ROW; iRow++)
			{
				strName.Format(_T("BtnBoard_%d"), iRow);
				CButtonUI *pButtnUi = static_cast<CButtonUI *>(pSubUi->FindSubControl(strName));

				if (pButtnUi)
				{
					if (iRow < sProductSize.cx)
					{
						strName.Format(_T("BtnOK_%d"), iRow + sProductSize.cx * iCol);
						//pButtnUi->SetBkColor(0xFFE0E0E0);
						pButtnUi->SetBkColor(0xFFFF000);
						pButtnUi->SetName(strName);
						strName.Format(_T("%d"), iRow + sProductSize.cx * iCol + 1);
						pButtnUi->SetText(strName);
						pButtnUi->SetMouseEnabled(false);
						pButtnUi->SetVisible(true);
					}
					else
					{
						pButtnUi->SetVisible(false);
					}
				}
			}

			pHorOKUI->Add(pSubUi);
		}

		ClearAllOKBoard();


		pHorOKUI = NULL;
		pHorOKUI = static_cast<CHorizontalLayoutUI *>(m_pPaintManagerUI->FindControl(_T("Hor_NGBoard")));
		if (pHorOKUI)
		{
			pHorOKUI->RemoveAll();
		}

		sProductSize = m_pIWorkFlow->GetNgTraySize();

		for (iCol = 0; iCol < sProductSize.cy && pHorOKUI; ++iCol)
		{
			DuiLib::CDialogBuilder builder;
			DuiLib::CDuiString strXML = _T("");
			strXML += _T("Board.xml");
			DuiLib::CContainerUI* pSubUi = static_cast<DuiLib::CContainerUI*>(builder.Create(strXML.GetData(), (UINT)0, NULL, NULL, NULL));

			for (iRow = 0; iRow < ROW; iRow++)
			{
				strName.Format(_T("BtnBoard_%d"), iRow);
				CLabelUI *pButtnUi = static_cast<CLabelUI *>(pSubUi->FindSubControl(strName));

				if (pButtnUi)
				{
					if (iRow < sProductSize.cx)
					{
						strName.Format(_T("BtnNG_%d"), iRow + sProductSize.cx * iCol);
						pButtnUi->SetBkColor(0xFFE0E0E0);
						pButtnUi->SetName(strName);
						strName.Format(_T("%d"), iRow + sProductSize.cx * iCol + 1);
						pButtnUi->SetText(strName);
						pButtnUi->SetMouseEnabled(false);
						pButtnUi->SetVisible(true);
					}
					else
					{
						pButtnUi->SetVisible(false);
					}
				}
			}

			pHorOKUI->Add(pSubUi);
		}
	}
}

void CHomeView::ClearAllRstBoard()
{
	if (m_pPaintManagerUI && m_pIWorkFlow)
	{
		int iIndex = 0;
		int iRow = 0;
		int iCol = 0;
		SIZE  sProductSize;
		CDuiString strName = _T("");
		sProductSize = m_pIWorkFlow->GetProductSize();

		for (iRow = 0; iRow < ROW; iRow++)
		{
			for (iCol = 0; iCol < COL; ++iCol)
			{
				strName.Format(_T("BtnRst%d_%d"), iRow, iCol);
				CButtonUI *pButtnUi = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(strName));

				if (pButtnUi)
				{
					if (iRow < sProductSize.cx && iCol < sProductSize.cy)
					{
						pButtnUi->SetBkColor(0xFFE0E0E0);
						pButtnUi->SetMouseEnabled(false);
						pButtnUi->SetVisible(true);
					}
					else
					{
						pButtnUi->SetVisible(false);
					}
				}
			}
		}
	}
}

void CHomeView::SetRstBoard(const int iIndex, const int iStatus)
{
	if (m_pPaintManagerUI && m_pIWorkFlow)
	{
		int iRow = 0;
		int iCol = 0;
		SIZE  sProductSize;
		CDuiString strName = _T("");
		sProductSize = m_pIWorkFlow->GetProductSize();
		iRow = iIndex / sProductSize.cy;
		iCol = iIndex % sProductSize.cy;
	
		strName.Format(_T("BtnRst%d_%d"), iRow, iCol);
		CLabelUI *pButtnUi = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(strName));

		if (pButtnUi)
		{
			switch (iStatus)
			{
			case 0:
				{
					pButtnUi->SetBkColor(0xFFFF0000);
				}
				break;
				
			case 1:
				{
					pButtnUi->SetBkColor(0xFF00FF00);
				}
				break;
			case 3:
			case 2:
				{
					pButtnUi->SetBkColor(0xFFB200FF);
				}
				break;

			default:
				{
					pButtnUi->SetBkColor(0xFFE0E0E0);
				}
			}
			
			pButtnUi->SetMouseEnabled(false);
			pButtnUi->SetVisible(true);
		}
	
	}
}

void CHomeView::UpdataRst()
{
	if (m_pPaintManagerUI && m_pIWorkFlow)
	{
		int iRow = 0;
		int iCol = 0;
		SIZE  sProductSize;
		CDuiString strName = _T("");
		sProductSize = m_pIWorkFlow->GetProductSize();
		
		for (int i = 0; i < sProductSize.cx * sProductSize.cy; ++i)
		{
			SetRstBoard(i, m_pIWorkFlow->GetBoardRst(i));
		}

	}
}

void CHomeView::UpdataUph()
{
	if (m_pIWorkFlow)
	{
		CDuiString strData = _T("");
		CButtonUI *pBtnUI = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(_T("BtnHPsPcs")));

		if (pBtnUI)
		{
			strData.Format(_T("%d"), m_pIWorkFlow->GetHourPassCount());
			pBtnUI->SetText(strData);
		}

		pBtnUI = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(_T("BtnClassType")));

		if (pBtnUI)
		{
			if (m_pIWorkFlow->GetClassType() == 0)
			{
				strData = _T("°×°à");
			}
			else
			{
				strData = _T("Íí°à");
			}
			
			pBtnUI->SetText(strData);
		}

		pBtnUI = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(_T("BtnHFiPcs")));

		if (pBtnUI)
		{
			strData.Format(_T("%d"), m_pIWorkFlow->GetHourFailCount());
			pBtnUI->SetText(strData);
		}

		pBtnUI = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(_T("BtnHOtPcs")));

		if (pBtnUI)
		{
			strData.Format(_T("%d"), m_pIWorkFlow->GetHourOtherCount());
			pBtnUI->SetText(strData);
		}

		pBtnUI = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(_T("BtnDOtPcs")));

		if (pBtnUI)
		{
			strData.Format(_T("%d"), m_pIWorkFlow->GetDayOtherCount());
			pBtnUI->SetText(strData);
		}

		pBtnUI = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(_T("BtnHYield")));

		if (pBtnUI)
		{

			double dYeild = m_pIWorkFlow->GetHourYeild();
			strData.Format(_T("%.1f"), dYeild);
			if (m_classConfig.ReadDouble(_T("Yeild"), _T("Spec"), 85.0) > dYeild)
			{
				pBtnUI->SetTextColor(RGB(0, 0, 255));
			}
			else if (m_classConfig.ReadDouble(_T("Yeild"), _T("Target"), 85.0) > dYeild)
			{
				pBtnUI->SetTextColor(RGB(0, 128, 128));
			}
			else
			{
				pBtnUI->SetTextColor(RGB(0, 255, 0));
			}
			pBtnUI->SetText(strData);
		}

		pBtnUI = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(_T("BtnDPsPcs")));

		if (pBtnUI)
		{
			strData.Format(_T("%d"), m_pIWorkFlow->GetDayPassCount());
			pBtnUI->SetTextColor(RGB(0, 255, 0));
			pBtnUI->SetText(strData);
		}

		pBtnUI = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(_T("BtnDFiPcs")));

		if (pBtnUI)
		{
			strData.Format(_T("%d"), m_pIWorkFlow->GetDayFailCount());
			pBtnUI->SetTextColor(RGB(0, 0, 255));
			pBtnUI->SetText(strData);
		}

		pBtnUI = static_cast<CButtonUI *>(m_pPaintManagerUI->FindControl(_T("BtnDYield")));

		if (pBtnUI)
		{
			double dYeild = m_pIWorkFlow->GetDayYeild();
			strData.Format(_T("%.1f"), dYeild);
			if (m_classConfig.ReadDouble(_T("Yeild"), _T("Spec"), 85.0) > dYeild)
			{
				pBtnUI->SetTextColor(RGB(0, 0, 255));
			}
			else if (m_classConfig.ReadDouble(_T("Yeild"), _T("Target"), 85.0) > dYeild)
			{
				pBtnUI->SetTextColor(RGB(0, 128, 128));
			}
			else
			{
				pBtnUI->SetTextColor(RGB(0, 255, 0));
			}
			
			pBtnUI->SetText(strData);
		}
	}
}

void CHomeView::ClearAllOKBoard()
{
	if (m_pPaintManagerUI && m_pIWorkFlow)
	{
		int iIndex = 0;
		int iRow = 0;
		int iCol = 0;
		SIZE  sProductSize;
		CDuiString strName = _T("");
		sProductSize = m_pIWorkFlow->GetTraySize();

		for (iRow = 0; iRow < sProductSize.cx * sProductSize.cy; iRow++)
		{

			strName.Format(_T("BtnOK_%d"), iRow);
			CLabelUI *pButtnUi = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(strName));

			if (pButtnUi)
			{
//				if (iRow < sProductSize.cx && iCol < sProductSize.cy)
				{
					pButtnUi->SetBkColor(0xFFE0E0E0);
					pButtnUi->SetMouseEnabled(false);
					pButtnUi->SetVisible(true);
				}

			}
		}
	}
}

void CHomeView::ClearAllNGBoard()
{
	if (m_pPaintManagerUI && m_pIWorkFlow)
	{
		int iIndex = 0;
		int iRow = 0;
		int iCol = 0;
		SIZE  sProductSize;
		CDuiString strName = _T("");
		sProductSize = m_pIWorkFlow->GetNgTraySize();

		for (iRow = 0; iRow < sProductSize.cx * sProductSize.cy; iRow++)
		{
			
				strName.Format(_T("BtnNG_%d"), iRow);
				CLabelUI *pButtnUi = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(strName));

				if (pButtnUi)
				{
//					if (iRow < sProductSize.cx && iCol < sProductSize.cy)
					{
						pButtnUi->SetBkColor(0xFFE0E0E0);
						pButtnUi->SetMouseEnabled(false);
						pButtnUi->SetVisible(true);
					}

				}
		}
	}
}

void CHomeView::SetOKBoard(const int iIndex)
{
	if (m_pPaintManagerUI && m_pIWorkFlow)
	{
		int iRow = 0;
		int iCol = 0;
		SIZE  sProductSize;
		CDuiString strName = _T("");
		sProductSize = m_pIWorkFlow->GetTraySize();

		strName.Format(_T("BtnOK_%d"), iIndex);
		CLabelUI *pButtnUi = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(strName));

		if (pButtnUi)
		{

			pButtnUi->SetBkColor(0xFF00FF00);
			pButtnUi->SetMouseEnabled(false);
			pButtnUi->SetVisible(true);
		}

		UpdataUph();
	}
}

void CHomeView::SetNGBoard(const int iIndex)
{
	if (m_pPaintManagerUI && m_pIWorkFlow)
	{
		int iRow = 0;
		int iCol = 0;
		SIZE  sProductSize;
		CDuiString strName = _T("");
		sProductSize = m_pIWorkFlow->GetTraySize();

		strName.Format(_T("BtnNG_%d"), iIndex);
		CLabelUI *pButtnUi = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(strName));

		if (pButtnUi)
		{

			pButtnUi->SetBkColor(0xFFFF0000);
			pButtnUi->SetMouseEnabled(false);
			pButtnUi->SetVisible(true);
		}

		UpdataUph();
	}
}

void CHomeView::ClearRstBoard(const int iIndex)
{
	if (m_pPaintManagerUI && m_pIWorkFlow)
	{
		int iRow = 0;
		int iCol = 0;
		SIZE  sProductSize;
		CDuiString strName = _T("");
		sProductSize = m_pIWorkFlow->GetProductSize();
		iRow = iIndex / sProductSize.cy;
		iCol = iIndex % sProductSize.cy;

		strName.Format(_T("BtnRst%d_%d"), iRow, iCol);
		CLabelUI *pButtnUi = static_cast<CLabelUI *>(m_pPaintManagerUI->FindControl(strName));

		if (pButtnUi)
		{
		
			pButtnUi->SetBkColor(0xFFE0E0E0);
			
			pButtnUi->SetMouseEnabled(false);
			pButtnUi->SetVisible(true);
		}

	}
}