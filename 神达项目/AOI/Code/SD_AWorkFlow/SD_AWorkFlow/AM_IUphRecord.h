#pragma once

#ifdef AM_UPHRECORD_EXPORTS
#define AM_UPH_API __declspec(dllexport)
#else
#define AM_UPH_API __declspec(dllimport)
#endif

class CIUphRecord
{
public: 
	CIUphRecord () {};
	virtual ~CIUphRecord() {};

	virtual DWORD GetHourPassCount() = 0;
	virtual DWORD GetHourFailCount() = 0;
	virtual DWORD GetHourOtherCount() = 0;
	virtual DWORD GetDayPassCount() = 0;
	virtual DWORD GetDayFailCount() = 0;
	virtual DWORD GetDayOtherCount() = 0;
	virtual double GetHourYeild() = 0;
	virtual double GetDayYeild() = 0;

	virtual void AddHourPassCount(const int iCount = 1) = 0;
	virtual void AddHourFailCount(const int iCount = 1) = 0;
	virtual void AddHourOtherCount(const int iCount = 1) = 0;
	virtual void SetProductName(LPCTSTR lpstrName) = 0;
	virtual void SetUpDataTime(const int iUpdataTime) = 0;
};

extern "C"
{
	AM_UPH_API void CreateUph(CIUphRecord **ppIUphRecord);
	AM_UPH_API void ReleaseUph(CIUphRecord *pIUphRecord);
};