#include "stdafx.h"
#include "Product.h"

#define DEFAULT_STRING _T("####")
#define DEFAULT_VALUE _T("0.0,0.0,0.0,0.0,")

void CProduct::InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow)
{
	m_pPaintManagerUI = pPaintManagerUI;
	m_pIWorkFlow = pIWorkFlow;
	m_strConfigIni = m_classIni.GetModulePath() + _T("Config\\Config.ini");
	m_strProductDir = m_classIni.GetModulePath() + _T("Product\\");
	m_classIni.SetIniFile(m_strConfigIni.c_str());
	
	if (m_pPaintManagerUI)
	{
		CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));

		if (pComboUi)
		{
			RemoveComboAllItem(pComboUi);
			if (m_classIni.GetKeyCount(_T("ProductList")) > 0)
			{
				for (int i = 0; i < m_classIni.GetKeyCount(_T("ProductList")); ++i)
				{
					AddComboItem(pComboUi, m_classIni.GetKeyName(_T("ProductList"), i).c_str());

					if (m_classIni.ReadInt(_T("ProductList"), m_classIni.GetKeyName(_T("ProductList"), i).c_str(), 0) == 1)
					{
						m_strCurProduct = m_classIni.GetKeyName(_T("ProductList"), i);
					}
				}
			}
			else
			{
				AddComboItem(pComboUi, _T("SD"));
				m_classIni.WriteString(_T("ProductList"), _T("SD"), _T("1"));
			}
		}

	}
}

BOOL CProduct::OnClick(DuiLib::TNotifyUI& msg)
{
	BOOL bRet = FALSE;
	if (msg.pSender->GetName() == _T("BtnStoreProduct"))
	{
		if (FMessageBox(NULL, _T("确定要保存产品? 保存(Yes) 取消(No)"), _T("提示"), MB_YESNO) == IDYES)
		{
			StoreProductInfo();
		}
		
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnChgProduct"))
	{
		if (m_pIWorkFlow)
		{
			tstring strProductName = _T("");
			if (FMessageBox(NULL, _T("确定要切换产品? 切换(Yes) 取消(No)"), _T("提示"), MB_YESNO) == IDYES)
			{
				if (m_pPaintManagerUI)
				{
					CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));
					strProductName = pComboUi->GetText();
					if (pComboUi && m_pIWorkFlow->ChangeProduct(strProductName.c_str()))
					{
						m_strCurProduct = strProductName;
					}
				}

			}
		}

		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnDelProduct"))
	{
		if (FMessageBox(NULL, _T("确定要删除产品? 删除(Yes) 取消(No)"), _T("提示"), MB_YESNO) == IDYES)
		{
			DeleteProduct();
			InitWindow(m_pPaintManagerUI, m_pIWorkFlow);
		}
		
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnNewProduct"))
	{
		if (FMessageBox(NULL, _T("确定要新建产品? 新建(Yes) 取消(No)"), _T("提示"), MB_YESNO) == IDYES)
		{
			StoreProductInfo(1);
			InitWindow(m_pPaintManagerUI, m_pIWorkFlow);
		}
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnStorePick"))
	{
//		if (FMessageBox(NULL, _T("确定要删除产品? 保存(Yes) 取消(No)"), _T("提示"), MB_YESNO) == IDYES)
		{
			StorePickInfo();
		}
		
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("BtnStoreCapture"))
	{
		StoreCaptureInfo();
		bRet = TRUE;
	}
	return bRet;
}

BOOL CProduct::OnSelChanged(DuiLib::TNotifyUI& msg)
{
	BOOL bRet = FALSE;
	if (msg.pSender->GetName() == _T("CmbCaptureList"))
	{
		InitCaptureInfo();
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("CmbPickList"))
	{
		InitTakeInfo();
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("CmbProductList"))
	{
		InitProductInfo();
		InitTakeInfo();
		InitCaptureInfo();
		bRet = TRUE;
	}
	else if (msg.pSender->GetName() == _T("CmbProductIndex"))
	{
		InitCapturePos();
	}
	return bRet;
}

void CProduct::InitProductInfo()
{
	CIniFile classIni;
	tstring strProductName = _T("");
	tstring  strProductRow = _T("");
	tstring  strProductCol = _T("");
	tstring  strTrayRow  = _T("");
	tstring  strTrayCol  = _T("");
	tstring  strCaptrueCount  = _T("");
	tstring  strPickCircle  = _T("");

	if (m_pPaintManagerUI)
	{
		CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));

		if (pComboUi)
		{
			
			strProductName = pComboUi->GetText();
			classIni.SetIniFile((m_strProductDir + strProductName + _T("\\Product.inf")).c_str());

			strProductRow = classIni.ReadString(_T("Product"), _T("Row"), _T("3"));
			strProductCol = classIni.ReadString(_T("Product"), _T("Col"), _T("4"));
			strTrayRow = classIni.ReadString(_T("Tray"), _T("Row"), _T("4"));
			strTrayCol = classIni.ReadString(_T("Tray"), _T("Col"), _T("10"));
			strCaptrueCount = classIni.ReadString(_T("Product"), _T("CaptrueCount"), _T("1"));
			strPickCircle = classIni.ReadString(_T("Product"), _T("PickCircle"), _T("3"));
			CEditUI *pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtCaptureCount")));
			if (pEditUi)
			{
				pEditUi->SetText(strCaptrueCount.c_str());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtProductRow")));

			if (pEditUi)
			{
				pEditUi->SetText(strProductRow.c_str());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtProductCol")));

			if (pEditUi)
			{
				pEditUi->SetText(strProductCol.c_str());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtTrayRow")));

			if (pEditUi)
			{
				pEditUi->SetText(strTrayRow.c_str());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtTrayCol")));

			if (pEditUi)
			{
				pEditUi->SetText(strTrayCol.c_str());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtPickCircle")));

			if (pEditUi)
			{
				pEditUi->SetText(strPickCircle.c_str());
			}

			pEditUi = NULL;

			pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbPickList")));

			if (pComboUi)
			{
				RemoveComboAllItem(pComboUi);
				for (int i = 0; i < _ttoi(strPickCircle.c_str()); ++i)
				{
					CDuiString strTemp = _T("");
					strTemp.Format(_T("%d"), i + 1);
					AddComboItem(pComboUi, strTemp);
				}

				pComboUi->SelectItem(0);
			}


			pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbCaptureList")));

			if (pComboUi)
			{
				RemoveComboAllItem(pComboUi);
				for (int i = 0; i < _ttoi(strCaptrueCount.c_str()); ++i)
				{
					CDuiString strTemp = _T("");
					strTemp.Format(_T("%d"), i + 1);
					AddComboItem(pComboUi, strTemp);
				}

				pComboUi->SelectItem(0);
			}
		}

	}
	
}

void CProduct::StoreProductInfo(const int iMode /* = 0 */)
{
	CIniFile classIni;
	tstring strProductName = _T("");
	tstring  strProductRow = _T("");
	tstring  strProductCol = _T("");
	tstring  strTrayRow  = _T("");
	tstring  strTrayCol  = _T("");
	tstring  strCaptrueCount  = _T("");
	tstring  strPickCircle  = _T("");
	if (m_pPaintManagerUI)
	{
		CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));
	
		if (pComboUi)
		{
			CEditUI *pEditUi = NULL;
			if (iMode == 1)
			{
				pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtNewProductName")));
				if (pEditUi)
				{
					strProductName = pEditUi->GetText();
				}
				
				if (strProductName == _T(""))
				{
					strProductName = pComboUi->GetText();
				}
				else
				{
					if (strProductName != m_strCurProduct)
					{
						m_classIni.WriteString(_T("ProductList"), strProductName.c_str(), _T("0"));
					}
				}
			}
			else
			{
				strProductName = pComboUi->GetText();
			}
			
			classIni.SetIniFile((m_strProductDir + strProductName + _T("\\Product.inf")).c_str());

			strProductRow = classIni.ReadString(_T("Product"), _T("Row"), _T("3"));
			strProductCol = classIni.ReadString(_T("Product"), _T("Col"), _T("4"));
			strTrayRow = classIni.ReadString(_T("Tray"), _T("Row"), _T("4"));
			strTrayCol = classIni.ReadString(_T("Tray"), _T("Col"), _T("10"));
			strCaptrueCount = classIni.ReadString(_T("Product"), _T("CaptrueCount"), _T("1"));
			strPickCircle = classIni.ReadString(_T("Product"), _T("PickCircle"), _T("3"));

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtCaptureCount")));
			if (pEditUi)
			{
				strCaptrueCount = pEditUi->GetText();
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtProductRow")));

			if (pEditUi)
			{
				strProductRow = pEditUi->GetText();
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtProductCol")));

			if (pEditUi)
			{
				strProductCol = pEditUi->GetText();
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtTrayRow")));

			if (pEditUi)
			{
				strTrayRow = pEditUi->GetText();
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtTrayCol")));

			if (pEditUi)
			{
				strTrayCol = pEditUi->GetText();
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtPickCircle")));

			if (pEditUi)
			{
				strPickCircle = pEditUi->GetText();
			}

			pEditUi = NULL;

			classIni.WriteString(_T("Product"), _T("Row"), strProductRow.c_str());
			classIni.WriteString(_T("Product"), _T("Col"), strProductCol.c_str());
			classIni.WriteString(_T("Tray"), _T("Row"), strTrayRow.c_str());
			classIni.WriteString(_T("Tray"), _T("Col"), strTrayCol.c_str());
			classIni.WriteString(_T("Product"), _T("CaptrueCount"), strCaptrueCount.c_str());
			classIni.WriteString(_T("Product"), _T("PickCircle"), strPickCircle.c_str());

			pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbPickList")));


			if (pComboUi)
			{
				RemoveComboAllItem(pComboUi);
				for (int i = 0; i < _ttoi(strPickCircle.c_str()); ++i)
				{
					CDuiString strTemp = _T("");
					strTemp.Format(_T("%d"), i + 1);
					AddComboItem(pComboUi, strTemp);
				}

				pComboUi->SelectItem(0);
			}

			pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbCaptureList")));

			if (pComboUi)
			{
				RemoveComboAllItem(pComboUi);
				for (int i = 0; i < _ttoi(strCaptrueCount.c_str()); ++i)
				{
					CDuiString strTemp = _T("");
					strTemp.Format(_T("%d"), i + 1);
					AddComboItem(pComboUi, strTemp);
				}

				pComboUi->SelectItem(0);
			}
			
			if (iMode == 1)
			{
				classIni.SetIniFile((m_strProductDir + strProductName + _T("\\Position.dat")).c_str());
				if (classIni.ReadString(_T("Position"), _T("Board标定位置"), DEFAULT_STRING) == DEFAULT_STRING)
				{
					classIni.WriteString(_T("Position"), _T("Board标定位置"), DEFAULT_VALUE);
				}

				if (classIni.ReadString(_T("Position"), _T("放OK板过渡位置"), DEFAULT_STRING) == DEFAULT_STRING)
				{
					classIni.WriteString(_T("Position"), _T("放OK板过渡位置"), DEFAULT_VALUE);
				}

				if (classIni.ReadString(_T("Position"), _T("放NG板过渡位置"), DEFAULT_STRING) == DEFAULT_STRING)
				{
					classIni.WriteString(_T("Position"), _T("放NG板过渡位置"), DEFAULT_VALUE);
				}

				if (classIni.ReadString(_T("Position"), _T("等待位置"), DEFAULT_STRING) == DEFAULT_STRING)
				{
					classIni.WriteString(_T("Position"), _T("等待位置"), DEFAULT_VALUE);
				}

				CDuiString strKey = _T("");
				for (int i = 0; i < 3; ++i)
				{
					strKey.Format(_T("PASSTray%d号标定位置"), i + 1);

					if (classIni.ReadString(_T("Position"), strKey, DEFAULT_STRING) == DEFAULT_STRING)
					{
						classIni.WriteString(_T("Position"), strKey, DEFAULT_VALUE);
					}
				}

				for (int i = 0; i < 3; ++i)
				{
					strKey.Format(_T("NGTray%d号标定位置"), i + 1);

					if (classIni.ReadString(_T("Position"), strKey, DEFAULT_STRING) == DEFAULT_STRING)
					{
						classIni.WriteString(_T("Position"), strKey, DEFAULT_VALUE);
					}
				}

				for (int i = 0; i < 3; ++i)
				{
					strKey.Format(_T("PTOOLTray%d号标定位置"), i + 1);

					if (classIni.ReadString(_T("Position"), strKey, DEFAULT_STRING) == DEFAULT_STRING)
					{
						classIni.WriteString(_T("Position"), strKey, DEFAULT_VALUE);
					}
				}

				for (int i = 0; i < 3; ++i)
				{
					strKey.Format(_T("NTOOLTray%d号标定位置"), i + 1);

					if (classIni.ReadString(_T("Position"), strKey, DEFAULT_STRING) == DEFAULT_STRING)
					{
						classIni.WriteString(_T("Position"), strKey, DEFAULT_VALUE);
					}
				}

				for (int i = 0; i < 4; ++i)
				{
					strKey.Format(_T("%d号X板丢弃位"), i + 1);

					if (classIni.ReadString(_T("Position"), strKey, DEFAULT_STRING) == DEFAULT_STRING)
					{
						classIni.WriteString(_T("Position"), strKey, DEFAULT_VALUE);
					}
				}

				for (int i = 0; i < 4; ++i)
				{
					strKey.Format(_T("%d号吸头标定位置"), i + 1);

					if (classIni.ReadString(_T("Position"), strKey, DEFAULT_STRING) == DEFAULT_STRING)
					{
						classIni.WriteString(_T("Position"), strKey, DEFAULT_VALUE);
					}
				}

				for (int j = 0; j < _ttoi(strPickCircle.c_str()); ++j)
				{
					for (int i = 0; i < 4; ++i)
					{
						strKey.Format(_T("第%d次%d号取板位置"), j + 1, i + 1);

						if (classIni.ReadString(_T("Position"), strKey, DEFAULT_STRING) == DEFAULT_STRING)
						{
							classIni.WriteString(_T("Position"), strKey, DEFAULT_VALUE);
						}
					}
				}

				for (int i = 0; i < 100; ++i)
				{
					if (i < _ttoi(strTrayRow.c_str()) * _ttoi(strTrayCol.c_str()))
					{
						strKey.Format(_T("PASSTray%d号位置"), i + 1);

						if (classIni.ReadString(_T("Position"), strKey, DEFAULT_STRING) == DEFAULT_STRING)
						{
							classIni.WriteString(_T("Position"), strKey, DEFAULT_VALUE);
						}
					}
					else
					{
						strKey.Format(_T("PASSTray%d号位置"), i + 1);
						classIni.WriteString(_T("Position"), strKey, NULL);				
						strKey.Format(_T("NGTray%d号位置"), i + 1);		
						classIni.WriteString(_T("Position"), strKey, NULL);					
					}
				}

				for (int i = 0; i < 100; ++i)
				{
					if (i < _ttoi(strTrayRow.c_str()) * _ttoi(strTrayCol.c_str()))
					{
						strKey.Format(_T("NGTray%d号位置"), i + 1);

						if (classIni.ReadString(_T("Position"), strKey, DEFAULT_STRING) == DEFAULT_STRING)
						{
							classIni.WriteString(_T("Position"), strKey, DEFAULT_VALUE);
						}
					}
					else
					{
						strKey.Format(_T("PASSTray%d号位置"), i + 1);
						classIni.WriteString(_T("Position"), strKey, NULL);				
						strKey.Format(_T("NGTray%d号位置"), i + 1);		
						classIni.WriteString(_T("Position"), strKey, NULL);					
					}
				}
			}
			
		}

	}
}

void CProduct::InitTakeInfo()
{
	CIniFile classIni;
	tstring strProductName = _T("");
	int iSelect = 0;
	

	if (m_pPaintManagerUI)
	{
		CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));

		if (pComboUi)
		{
			strProductName = pComboUi->GetText();
		}
		else
		{
			strProductName = _T("SD");

		}

		pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbPickList")));

		if (pComboUi)
		{
			CDuiString strSection = _T("");
			iSelect = pComboUi->GetCurSel();
			classIni.SetIniFile((m_strProductDir + strProductName + _T("\\Sucker.inf")).c_str());
			strSection.Format(_T("Sucker_%d"), iSelect);


			int iPickMode = classIni.ReadInt(strSection, _T("PickMode"), 0);
			int iPickCount = classIni.ReadInt(strSection, _T("PickCount"), 1);
			int iSuckerToPos0 = classIni.ReadInt(strSection, _T("Sucker_0_Pos"), 0 + iSelect * 4);
			int iSuckerToPos1 = classIni.ReadInt(strSection, _T("Sucker_1_Pos"), 1 + iSelect * 4);
			int iSuckerToPos2 = classIni.ReadInt(strSection, _T("Sucker_2_Pos"), 2 + iSelect * 4);
			int iSuckerToPos3 = classIni.ReadInt(strSection, _T("Sucker_3_Pos"), 3 + iSelect * 4);
			
			CEditUI *pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtPickMode")));
			if (pEditUi)
			{
				strSection.Format(_T("%d"), iPickMode);
				pEditUi->SetText(strSection);
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtPickCount")));

			if (pEditUi)
			{
				strSection.Format(_T("%d"), iPickCount);
				pEditUi->SetText(strSection);
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtOneSuckerNum")));

			if (pEditUi)
			{
				strSection.Format(_T("%d"), iSuckerToPos0);
				pEditUi->SetText(strSection);
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtTwoSuckerNum")));

			if (pEditUi)
			{
				strSection.Format(_T("%d"), iSuckerToPos1);
				pEditUi->SetText(strSection);
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtThreeSuckerNum")));

			if (pEditUi)
			{
				strSection.Format(_T("%d"), iSuckerToPos2);
				pEditUi->SetText(strSection);
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtFourSuckerNum")));

			if (pEditUi)
			{
				strSection.Format(_T("%d"), iSuckerToPos3);
				pEditUi->SetText(strSection);
			}

			pEditUi = NULL;
		}

	}
}

void CProduct::StorePickInfo()
{
	CIniFile classIni;
	tstring strProductName = _T("");
	int iSelect = 0;


	if (m_pPaintManagerUI)
	{
		CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));

		if (pComboUi)
		{
			strProductName = pComboUi->GetText();
		}
		else
		{
			strProductName = _T("SD");

		}

		pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbPickList")));

		if (pComboUi)
		{
			
			CDuiString strSection = _T("");
			iSelect = pComboUi->GetCurSel();
			classIni.SetIniFile((m_strProductDir + strProductName + _T("\\Sucker.inf")).c_str());
			strSection.Format(_T("Sucker_%d"), iSelect);


			int iPickMode = classIni.ReadInt(strSection, _T("PickMode"), 0);
			int iPickCount = classIni.ReadInt(strSection, _T("PickCount"), 1);
			int iSuckerToPos0 = classIni.ReadInt(strSection, _T("Sucker_0_Pos"), 0 + iSelect * 4);
			int iSuckerToPos1 = classIni.ReadInt(strSection, _T("Sucker_1_Pos"), 1 + iSelect * 4);
			int iSuckerToPos2 = classIni.ReadInt(strSection, _T("Sucker_2_Pos"), 2 + iSelect * 4);
			int iSuckerToPos3 = classIni.ReadInt(strSection, _T("Sucker_3_Pos"), 3 + iSelect * 4);

			CEditUI *pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtPickMode")));
			if (pEditUi)
			{
				iPickMode = _ttoi(pEditUi->GetText());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtPickCount")));

			if (pEditUi)
			{
				iPickCount = _ttoi(pEditUi->GetText());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtOneSuckerNum")));

			if (pEditUi)
			{
				iSuckerToPos0 = _ttoi(pEditUi->GetText());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtTwoSuckerNum")));

			if (pEditUi)
			{
				iSuckerToPos1 = _ttoi(pEditUi->GetText());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtThreeSuckerNum")));

			if (pEditUi)
			{
				iSuckerToPos2 = _ttoi(pEditUi->GetText());
			}

			pEditUi = NULL;

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtFourSuckerNum")));

			if (pEditUi)
			{
				iSuckerToPos3 = _ttoi(pEditUi->GetText());
			}

			pEditUi = NULL;

			classIni.WriteInt(strSection, _T("PickMode"), iPickMode);
			classIni.WriteInt(strSection, _T("PickCount"), iPickCount);
			classIni.WriteInt(strSection, _T("Sucker_0_Pos"), iSuckerToPos0);
			classIni.WriteInt(strSection, _T("Sucker_1_Pos"), iSuckerToPos1);
			classIni.WriteInt(strSection, _T("Sucker_2_Pos"), iSuckerToPos2);
			classIni.WriteInt(strSection, _T("Sucker_3_Pos"), iSuckerToPos3);
		}

	}
}

void CProduct::InitCaptureInfo()			// 初始化拍照次数的信息
{
	CIniFile classIni;
	tstring strProductName = _T("");
	int iSelect = 0;


	if (m_pPaintManagerUI)
	{
		CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));

		if (pComboUi)
		{
			strProductName = pComboUi->GetText();
		}
		else
		{
			strProductName = _T("SD");
		}

		pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbCaptureList")));

		if (pComboUi)
		{
			CDuiString strSection = _T("");
			CDuiString strValue = _T("");
			iSelect = pComboUi->GetCurSel();
			classIni.SetIniFile((m_strProductDir + strProductName + _T("\\Capture.inf")).c_str());
			strSection.Format(_T("Capture_%d"), iSelect);


			int iProductCount = classIni.ReadInt(strSection, _T("ProductCount"), 1);
		

			CEditUI *pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtProductCount")));
			if (pEditUi)
			{
				strValue.Format(_T("%d"), iProductCount);
				pEditUi->SetText(strValue);
			}

			pEditUi = NULL;

			int iRow = classIni.ReadInt(strSection, _T("Row"), 1);
			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtCaptureRow")));
			if (pEditUi)
			{
				strValue.Format(_T("%d"), iRow);
				pEditUi->SetText(strValue);
			}

			pEditUi = NULL;

			int iCol = classIni.ReadInt(strSection, _T("Col"), 1);
			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtCaptureCol")));
			if (pEditUi)
			{
				strValue.Format(_T("%d"), iCol);
				pEditUi->SetText(strValue);
			}

			pEditUi = NULL;

			pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductIndex")));

			if (pComboUi)
			{
				RemoveComboAllItem(pComboUi);
				for (int i = 0; i < iProductCount; ++i)
				{
					CDuiString strTemp = _T("");
					strTemp.Format(_T("%d"), i + 1);
					AddComboItem(pComboUi, strTemp);
				}

				pComboUi->SelectItem(0);
			}
		}

		InitCapturePos();
	}
}   

void CProduct::InitCapturePos()
{
	CIniFile classIni;
	tstring strProductName = _T("");
	CDuiString strSection = _T("");
	CDuiString strKey = _T("");
	int iSelect = 0;


	if (m_pPaintManagerUI)
	{
		CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));

		if (pComboUi)
		{
			strProductName = pComboUi->GetText();
		}
		else
		{
			strProductName = _T("SD");
		}

		pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbCaptureList")));
		
		if (pComboUi)
		{
			iSelect = pComboUi->GetCurSel();
			strSection.Format(_T("Capture_%d"), iSelect);

			pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductIndex")));

			if (pComboUi)
			{
				classIni.SetIniFile((m_strProductDir + strProductName + _T("\\Capture.inf")).c_str());
				strKey.Format(_T("Product_%d"), pComboUi->GetCurSel());
				int iProductCount = classIni.ReadInt(strSection, strKey, 1);
				CEditUI *pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtProductNum")));

				if (pEditUi)
				{
					strSection.Format(_T("%d"), iProductCount);
					pEditUi->SetText(strSection);
				}

				pEditUi = NULL;
			}
		}
	}
}

void CProduct::StoreCaptureInfo()
{
	CIniFile classIni;
	tstring strProductName = _T("");
	int iSelect = 0;
	CDuiString strSection = _T("");
	CDuiString strKey = _T("");

	if (m_pPaintManagerUI)
	{
		CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));

		if (pComboUi)
		{
			strProductName = pComboUi->GetText();
		}
		else
		{
			strProductName = _T("SD");
		}

		pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbCaptureList")));

		if (pComboUi)
		{
			CDuiString strSection = _T("");
			iSelect = pComboUi->GetCurSel();
			classIni.SetIniFile((m_strProductDir + strProductName + _T("\\Capture.inf")).c_str());
			strSection.Format(_T("Capture_%d"), iSelect);


			int iProductCount = classIni.ReadInt(strSection, _T("ProductCount"), 1);


			CEditUI *pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtProductCount")));
			if (pEditUi)
			{
				iProductCount = _ttoi(pEditUi->GetText());
			}

			pEditUi = NULL;
			classIni.WriteInt(strSection, _T("ProductCount"), iProductCount);

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtCaptureRow")));
			if (pEditUi)
			{
				iProductCount = _ttoi(pEditUi->GetText());
			}

			pEditUi = NULL;
			classIni.WriteInt(strSection, _T("Row"), iProductCount);

			pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtCaptureCol")));
			if (pEditUi)
			{
				iProductCount = _ttoi(pEditUi->GetText());
			}

			pEditUi = NULL;
			classIni.WriteInt(strSection, _T("Col"), iProductCount);
		

			pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductIndex")));

			if (pComboUi)
			{
				classIni.SetIniFile((m_strProductDir + strProductName + _T("\\Capture.inf")).c_str());
				
				strKey.Format(_T("Product_%d"), pComboUi->GetCurSel());
				CEditUI *pEditUi = static_cast<CEditUI *>(m_pPaintManagerUI->FindControl(_T("EdtProductNum")));

				if (pEditUi)
				{
					classIni.WriteString(strSection, strKey, pEditUi->GetText());
				}

				pEditUi = NULL;
			}
		}



		InitCaptureInfo();
	}
}

void CProduct::DeleteProduct()
{
	tstring strProductName = _T("");

	if (m_pPaintManagerUI)
	{
		CComboUI *pComboUi = static_cast<CComboUI *>(m_pPaintManagerUI->FindControl(_T("CmbProductList")));

		if (pComboUi)
		{
			strProductName = pComboUi->GetText();

			if (strProductName != m_strCurProduct)
			{
				m_classIni.WriteString(_T("ProductList"), strProductName.c_str(), NULL);
			}
		}
	}
}

