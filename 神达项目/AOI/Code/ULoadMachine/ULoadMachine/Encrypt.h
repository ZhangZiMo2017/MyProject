#pragma once
#ifdef ENCRYPT_EXPORTS
#define  ENCRYPT_API __declspec(dllexport)
#else
#define  ENCRYPT_API __declspec(dllimport)
#endif

extern "C"
{
	ENCRYPT_API int CheckSum(const BYTE *pbySrcCode, const int iLength, BYTE *pbyEncryptCode);
	ENCRYPT_API BOOL EncryptF(LPCTSTR lpszSource, LPCTSTR lpszDestination, LPCTSTR lpszPassword);
	ENCRYPT_API BOOL DecryptF(LPCTSTR lpszSource, LPCTSTR lpszDestination, LPCTSTR lpszPassword);
	ENCRYPT_API BOOL EncryptD(const BYTE *pbySrcCode, const int iLength, BYTE *pbyEncryptCode, LPCTSTR lpszPassword);
	ENCRYPT_API BOOL DecryptD(const BYTE *pbyEncryptCode, const int iLength, BYTE *pbySrcCode, LPCTSTR lpszPassword);
};