#pragma once

#ifdef AM_FILE_EXPORTS   
#define AM_FILE_API  _declspec(dllexport)
#else
#define AM_FILE_API  _declspec(dllimport)
#endif

class CAM_IFile
{
public:
	CAM_IFile(){};
	virtual ~CAM_IFile(){};

	//bWriteMode: TRUE-Write mode   FALSE-Read mode
	virtual BOOL Open(LPCTSTR lpstrFilePath, BOOL bWriteMode = TRUE) = 0;
	virtual BOOL Write(const void* lpBuf, const int iCount) = 0;
	virtual BOOL Read(void* lpBuf, const int iCount) = 0;
	virtual BOOL MergeFile(CAM_IFile* pFile) = 0;
	virtual void Close() = 0;
	virtual int GetLength() = 0;
	virtual int GetLineCount() = 0;
	virtual LPCTSTR GetErrorMsg() = 0;
	virtual BOOL CreateDir(LPCTSTR lpstrFilePath) = 0;
};

extern "C"
{
	void AM_FILE_API AM_CreateFile(CAM_IFile** ppFile);
	void AM_FILE_API AM_ReleaseFile(CAM_IFile* pFile);
};

