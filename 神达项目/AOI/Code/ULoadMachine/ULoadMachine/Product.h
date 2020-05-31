#pragma once

class CProduct
{
public:
	void InitWindow(CPaintManagerUI *pPaintManagerUI, CIWorkFlow *pIWorkFlow);
	BOOL OnClick(DuiLib::TNotifyUI& msg);
	BOOL OnSelChanged(DuiLib::TNotifyUI& msg);
	

	void InitProductInfo();
	void InitTakeInfo();
	void InitCaptureInfo();
	void InitCapturePos();

	void StoreProductInfo(const int iMode = 0);
	void StorePickInfo();
	void StoreCaptureInfo();
	void DeleteProduct();

private:
	CPaintManagerUI *m_pPaintManagerUI;
	CIWorkFlow *m_pIWorkFlow;
	
	CIniFile m_classIni;
	tstring m_strConfigIni;
	tstring m_strProductDir;
	tstring m_strCurProduct;
};