#include "stdafx.h"

#include "detours.h"
#pragma comment(lib,"detours.lib")
#include <WinInet.h>

#include "RecordBaseT.h"

BOOL g_bEnableWriteDisk = TRUE;
#define MAX_MEM_FILE_SIZE 1024*1024*5;

//临界区互斥锁
class CCSLock
{
private:
	CRITICAL_SECTION m_cs;
public:
	CCSLock()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CCSLock()
	{
		DeleteCriticalSection(&m_cs);
	}
	VOID Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	VOID UnLock()
	{
		LeaveCriticalSection(&m_cs);
	}

};

CCSLock g_DebugLock;

int nMaxMemUse = 0;
int nCurMemUse = 0;

class CMemFileObject;

class CUserFileObject
{
protected:
	HANDLE  m_hFile;
	ULONGLONG m_ulFilePointer;
	CMemFileObject *m_pMemFileObject;
	CRWLock m_rwLock;
	BOOL    m_bValid;
	BOOL    m_bCanDel;
public:
	CUserFileObject( HANDLE  hFile,CMemFileObject *pMemFileObject , BOOL bCanDel );
	~CUserFileObject();

	VOID SetDisableObject();
	BOOL GetCanDel();
	CMemFileObject *GetMemFileObjPtr();
	BOOL WriteFile( LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten );
	BOOL ReadFile(  LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead );
	DWORD SetFilePointer( LONG lDistanceToMove, PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod );
	DWORD GetFileSize(  LPDWORD lpFileSizeHigh 	);
	BOOL GetFileSizeEx(PLARGE_INTEGER lpFileSize );
};

class CMemFileObject
{
#define VALUE_CHECK1 0x028EFB6F
#define VALUE_CHECK2 0xCF18FDBC
protected:
	DWORD   dwSafeCheck;
	
	BYTE    *m_pMemBuf;
	LONGLONG m_ulMemLen;
	ULONGLONG m_ulFileLen;
	LONG     m_lReaderCount;
	LONG     m_lWriterCount;

	LONG     m_lRefCount;
	CString m_strFileName;

	CRWLock m_rwLock;
	
	CRecordBaseT<CUserFileObject *,CUserFileObject *> m_RefRecord; 

	DWORD   dwSafeCheck1;

public:
	CMemFileObject( LPCWSTR pszFileName )
	{
		dwSafeCheck = VALUE_CHECK1;
		dwSafeCheck1 = VALUE_CHECK2;
		m_pMemBuf = NULL;
		m_ulMemLen = 0;
		m_ulFileLen = 0;
		m_lReaderCount = 0;
		m_lWriterCount = 0;
		m_lRefCount = 0;

		m_strFileName = pszFileName;
	}

	~CMemFileObject()
	{
		dwSafeCheck = 0;
		dwSafeCheck1 = 0;

		if(m_pMemBuf)
		{
			free(m_pMemBuf);

			g_DebugLock.Lock();
			nCurMemUse-=m_ulMemLen;
			g_DebugLock.UnLock();

		}
	}
	
	BOOL CheckSafe()
	{
		if( dwSafeCheck == VALUE_CHECK1 && dwSafeCheck1 == VALUE_CHECK2)
		{
			return TRUE;
		}
		return FALSE;
	}

	VOID AddRef( CUserFileObject *pUserFileObj )
	{
		if(CheckSafe())
		{
			m_RefRecord.AddRecord(pUserFileObj,pUserFileObj);
		}
	}

	VOID Release( CUserFileObject *pUserFileObj )
	{
		if(CheckSafe())
		{
			CUserFileObject *pTempUserFileObj = NULL;
			m_RefRecord.DelRecord(pUserFileObj,&pTempUserFileObj);
		}
	}

	LONG GetRefCount()
	{
		if(FALSE==CheckSafe()) 
		{
			return 0;
		}
		return m_RefRecord.GetRecordCount();
	}

	BOOL CanDel()
	{
		if(FALSE==CheckSafe()) return FALSE;
		if( GetRefCount() == 0 )
		{
			return TRUE;
		}

		BOOL bAllCanDel = TRUE;

		m_RefRecord.WLock();

 		CRecordBaseT<CUserFileObject *,CUserFileObject *>::RECORD_DATA_LIST List;
 		m_RefRecord.GetAllNodeData(&List,TRUE);
 		for (CRecordBaseT<CUserFileObject *,CUserFileObject *>::RECORD_DATA_LIST::iterator it = List.begin();it!=List.end();it++)
 		{
			if((*it)->GetCanDel())
			{
				(*it)->SetDisableObject();
#ifdef DEBUG
				OutputDebugStringW(L"One UserFileObject Can Delete File\n");
#endif
			}
			else
			{
#ifdef DEBUG
				OutputDebugStringW(L"One UserFileObject Can Not Delete File\n");
#endif
				bAllCanDel = FALSE;
			}
 		}

		m_RefRecord.ULock();

#ifdef DEBUG
		if(bAllCanDel)
		{
			OutputDebugStringW(L"Can Delete File\n");
		}
		else
		{
			OutputDebugStringW(L"Can Not Delete File\n");
		}
#endif

		return bAllCanDel;

	}

	BOOL WriteFileData( LPCVOID lpBuffer, CONST DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten,CONST ULONGLONG ulFilePointer  )
	{
		if(FALSE==CheckSafe()) return FALSE;
		m_rwLock.wlock();

		ULONGLONG ulPreMemLen = m_ulMemLen;

		ULONGLONG dwNeedExLen = 0;
		if ( m_ulMemLen < ulFilePointer+nNumberOfBytesToWrite )
		{
			dwNeedExLen = ulFilePointer+nNumberOfBytesToWrite - m_ulMemLen;
		}

		if ( dwNeedExLen > 0 )
		{
			if ( NULL == m_pMemBuf )
			{
				ULONGLONG ulAllocLen = ( dwNeedExLen/4096+1)*4096;
				m_pMemBuf = (BYTE *)malloc(ulAllocLen);
				m_ulMemLen = ulAllocLen;
			}
			else
			{
				ULONGLONG ulAllocLen = ( dwNeedExLen/4096+1)*4096+m_ulMemLen;
				m_pMemBuf = (BYTE *)realloc(m_pMemBuf,ulAllocLen);
				m_ulMemLen = ulAllocLen;
			}	
		}


		g_DebugLock.Lock();

		nCurMemUse+=(m_ulMemLen-ulPreMemLen);
		if (nCurMemUse > nMaxMemUse)
		{
			nMaxMemUse = nCurMemUse;
		}

#ifdef DEBUG
		CString strMsgOut;
		strMsgOut.Format(L"当前内存占用 %u 最大内存占用 %u\r\n",nCurMemUse,nMaxMemUse);
		OutputDebugStringW(strMsgOut);
		
		strMsgOut.Format(L"%d",GetCurrentProcessId());
		CString strTemp;
		strTemp.Format(L"%ul",nMaxMemUse);
		WritePrivateProfileStringW(L"MaxMemUse",strMsgOut,strTemp,L"C:\\MatrixDebug.log");
#endif
		g_DebugLock.UnLock();

		memcpy_s(m_pMemBuf+ulFilePointer,nNumberOfBytesToWrite,lpBuffer,nNumberOfBytesToWrite);

		m_ulFileLen = ulFilePointer+nNumberOfBytesToWrite;

		if(lpNumberOfBytesWritten)
		{
			*lpNumberOfBytesWritten = nNumberOfBytesToWrite;
		}

		m_rwLock.unlock();


		return TRUE;
	}

	BOOL ReadFileData( LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,CONST ULONGLONG ulFilePointer )
	{
		if(FALSE==CheckSafe()) return FALSE;

		m_rwLock.rlock();

		ULONGLONG ulMaxReadLen = min(nNumberOfBytesToRead,(m_ulFileLen - ulFilePointer));
		if ( ulMaxReadLen > 0 )
		{
			memcpy_s(lpBuffer,ulMaxReadLen,m_pMemBuf+ulFilePointer,ulMaxReadLen);
		}

		if(lpNumberOfBytesRead)
		{
			*lpNumberOfBytesRead = ulMaxReadLen;
		}

		m_rwLock.unlock();

		return TRUE;
	}

	ULONGLONG GetFileLength()
	{
		if(FALSE==CheckSafe()) return FALSE;
		ULONGLONG ulFileLen = 0;
		m_rwLock.rlock();
		ulFileLen = m_ulFileLen;
		m_rwLock.unlock();
		return ulFileLen;
	}

	LPCWSTR GetFileName()
	{
		if(FALSE==CheckSafe()) return FALSE;
		return m_strFileName;
	}

};


CUserFileObject::CUserFileObject( HANDLE  hFile,CMemFileObject *pMemFileObject , BOOL bCanDel )
{
	if(pMemFileObject && pMemFileObject->CheckSafe())
	{
		pMemFileObject->AddRef(this);
		this->m_pMemFileObject = pMemFileObject;
	}
	else
	{
		m_pMemFileObject = NULL;
	}

	this->m_hFile = hFile;
	m_ulFilePointer = 0;
	m_bValid = 1;
	m_bCanDel = bCanDel;
}
CUserFileObject::~CUserFileObject()
{
	SetDisableObject();
	if (m_pMemFileObject->CheckSafe())
	{
		m_pMemFileObject->Release(this);
	}
	else
	{
		ASSERT(FALSE);
	}
}

VOID CUserFileObject::SetDisableObject()
{
	m_bValid = 0;
}

BOOL CUserFileObject::GetCanDel()
{
	return m_bCanDel;
}

CMemFileObject *CUserFileObject::GetMemFileObjPtr()
{
	CMemFileObject *pMemFileObject = NULL;
	if( 1 == m_bValid )
	{
		pMemFileObject = m_pMemFileObject;
	}
	return pMemFileObject;
}

BOOL CUserFileObject::WriteFile( LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite,LPDWORD lpNumberOfBytesWritten )
{
	BOOL bRes = FALSE;

	if (lpNumberOfBytesWritten)
	{
		*lpNumberOfBytesWritten = 0;
	}

	if( 1 == m_bValid )
	{
		DWORD dwWriteLen = 0;

		m_rwLock.wlock();

		bRes = m_pMemFileObject->WriteFileData(lpBuffer, nNumberOfBytesToWrite,&dwWriteLen,m_ulFilePointer);

		if (lpNumberOfBytesWritten)
		{
			*lpNumberOfBytesWritten = dwWriteLen;
		}

		m_ulFilePointer+=dwWriteLen;

		m_rwLock.unlock();
	}

	return bRes;
}


BOOL CUserFileObject::ReadFile(  LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead )
{
	BOOL bRes = FALSE;

	if (lpNumberOfBytesRead)
	{
		*lpNumberOfBytesRead = 0;
	}

	if( 1 == m_bValid )
	{
		DWORD dwReadLen = 0;

		m_rwLock.wlock();

		bRes =  m_pMemFileObject->ReadFileData(lpBuffer, nNumberOfBytesToRead, &dwReadLen,m_ulFilePointer);

		if (lpNumberOfBytesRead)
		{
			*lpNumberOfBytesRead = dwReadLen;
		}
		
		m_ulFilePointer+=dwReadLen;

		m_rwLock.unlock();
	}
	return bRes;
}


DWORD CUserFileObject::SetFilePointer( LONG lDistanceToMove, PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod )
{
	ULONGLONG ulNewFilePointer = 0;

	if( 1 == m_bValid )
	{
		m_rwLock.wlock();
		if( FILE_BEGIN == dwMoveMethod )
		{
			m_ulFilePointer = lDistanceToMove;
		}

		if( FILE_CURRENT == dwMoveMethod )
		{
			m_ulFilePointer += lDistanceToMove;
		}

		if( FILE_END == dwMoveMethod )
		{
			m_ulFilePointer = m_pMemFileObject->GetFileLength();
			m_ulFilePointer += lDistanceToMove;
		}

		ulNewFilePointer = m_ulFilePointer;
		m_rwLock.unlock();
	}

	return ulNewFilePointer;
}


DWORD CUserFileObject::GetFileSize(  LPDWORD lpFileSizeHigh 	)
{
	if( 1 == m_bValid )
	{
		if (lpFileSizeHigh)
		{
			*lpFileSizeHigh = 0;
		}
		return m_pMemFileObject->GetFileLength();
	}

	return 0;
}


BOOL CUserFileObject::GetFileSizeEx(PLARGE_INTEGER lpFileSize )
{
	if( 1 == m_bValid )
	{
		if(lpFileSize)
		{
			lpFileSize->QuadPart = m_pMemFileObject->GetFileLength();
		}
		
		return TRUE;
	}

	return FALSE;
}


typedef CRecordBaseT<HANDLE,CUserFileObject *>  CUserFileRecord;
typedef CRecordBaseT<CString,CMemFileObject *>  CMemFileRecord;

CUserFileRecord FRecord;
CMemFileRecord FMemRecord;

HANDLE g_hHandleBase = (HANDLE)0xF0000000;


HANDLE GetFalseHandle()
{
	static CCSLock falseHandleLock;

	HANDLE hTempHandle = NULL;
	
	falseHandleLock.Lock();
	g_hHandleBase = (HANDLE)((DWORD)(g_hHandleBase)+1);
	hTempHandle = g_hHandleBase;
	falseHandleLock.UnLock();

	return hTempHandle;
}

BOOL (WINAPI *pCloseHandle)( HANDLE hObject ) = CloseHandle;


HANDLE (WINAPI *pCreateFileW)( LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile ) = CreateFileW;

HANDLE WINAPI MyCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{


	CString strFileName;
	strFileName = lpFileName;
	strFileName.MakeLower();

	if (g_bEnableWriteDisk)
	{
		goto Pass;
	}

	CMemFileObject *pMemFileObj = NULL;
	FMemRecord.GetRecordData(strFileName,&pMemFileObj);

	BOOL bCanCreate = TRUE;
	g_DebugLock.Lock();
	bCanCreate = nCurMemUse < MAX_MEM_FILE_SIZE; 
	g_DebugLock.UnLock();

	if ( strFileName.Find(L"msimgsiz.dat") >= 0 || strFileName.Find(L"cryptneturlcache") >=0 || strFileName.Find(L"\\temp\\") >=0 )
	{
		return INVALID_HANDLE_VALUE;
	}

	if ( NULL == pMemFileObj && bCanCreate )
	{
		BOOL bPreExist = PathFileExists(lpFileName);

		if( 
			FALSE == bPreExist    //之前文件不存在
			&& !( lpFileName[0] == L'\\' && lpFileName[1] == L'\\')   //不是驱动对象
			&& (dwCreationDisposition&CREATE_ALWAYS || dwCreationDisposition&CREATE_NEW)  //想要创建新的文件
			&& GENERIC_WRITE&dwDesiredAccess     //需要写文件
			)
		{
			pMemFileObj = new CMemFileObject(lpFileName);

			FMemRecord.AddRecord(strFileName,pMemFileObj);

#if defined(DEBUG) || defined(_DEBUG)
			CString strMsgOut;
			strMsgOut.Format(L"创建内存文件：%s\n",strFileName);
			OutputDebugStringW(strMsgOut);
#endif

		}
	}

	if (pMemFileObj)
	{
		HANDLE hFalseHandle = GetFalseHandle();

		CUserFileObject *pUserFileObj = new CUserFileObject(hFalseHandle,pMemFileObj,dwShareMode&FILE_SHARE_DELETE);
		FRecord.AddRecord(hFalseHandle,pUserFileObj);

#if defined(DEBUG) || defined(_DEBUG)
		CString strMsgOut;
		strMsgOut.Format(L"创建伪文件  Share %d Flag %d Handle 0x%x Path：%s\n",dwShareMode,dwFlagsAndAttributes,hFalseHandle,strFileName);
		OutputDebugStringW(strMsgOut);
#endif

		return hFalseHandle;
	}


Pass:
	return pCreateFileW(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile
		);
}

BOOL (WINAPI *pWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped ) = WriteFile;


BOOL WINAPI MyWriteFile( HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped )
{

	CUserFileObject *pUserFileObj = NULL;
	BOOL bRes = FRecord.GetRecordData(hFile,&pUserFileObj);
	
	if ( pUserFileObj )
	{
#if defined(DEBUG) || defined(_DEBUG)

		CString strFileName;

		CMemFileObject *pMemFileObj = pUserFileObj->GetMemFileObjPtr();
		if (pMemFileObj)
		{
			strFileName = pMemFileObj->GetFileName();
		}

		CString strMsgOut;
		strMsgOut.Format(L"写入伪文件 Handle 0x%x Length %d Path：%s\n",hFile,nNumberOfBytesToWrite,strFileName);
		OutputDebugStringW(strMsgOut);
#endif

		return pUserFileObj->WriteFile(lpBuffer,nNumberOfBytesToWrite,lpNumberOfBytesWritten);;
	}


	return pWriteFile(
			hFile,
			lpBuffer,
			nNumberOfBytesToWrite,
			lpNumberOfBytesWritten,
			lpOverlapped
			);
}

BOOL (WINAPI *pReadFile)(HANDLE hFile,LPVOID lpBuffer, DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped) = ReadFile;
BOOL WINAPI MyReadFile(HANDLE hFile,LPVOID lpBuffer, DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped)
{

	CUserFileObject *pUserFileObj = NULL;
	BOOL bRes = FRecord.GetRecordData(hFile,&pUserFileObj);

	if (pUserFileObj)
	{
		BOOL bRes = FALSE;
		bRes = pUserFileObj->ReadFile( lpBuffer,nNumberOfBytesToRead,lpNumberOfBytesRead);

#if defined(DEBUG) || defined(_DEBUG)

		CString strFileName;

		CMemFileObject *pMemFileObj = pUserFileObj->GetMemFileObjPtr();
		if (pMemFileObj)
		{
			strFileName = pMemFileObj->GetFileName();
		}

		CString strMsgOut;
		strMsgOut.Format(L"读取伪文件 Handle 0x%x Length %d Path：%s\n",hFile,*lpNumberOfBytesRead,strFileName);
		OutputDebugStringW(strMsgOut);
#endif

		return bRes;
	}

	BOOL TReturn = pReadFile(
		hFile,
		lpBuffer,
		nNumberOfBytesToRead,
		lpNumberOfBytesRead,
		lpOverlapped
		);
	return TReturn;
};

DWORD (WINAPI *pSetFilePointer)( HANDLE hFile,LONG lDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod ) = SetFilePointer;
DWORD WINAPI MySetFilePointer( HANDLE hFile,LONG lDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod )
{

	CUserFileObject *pUserFileObj = NULL;
	BOOL bRes = FRecord.GetRecordData(hFile,&pUserFileObj);
	if( bRes )
	{
		return pUserFileObj->SetFilePointer(lDistanceToMove,lpDistanceToMoveHigh,dwMoveMethod);
	}

	DWORD TReturn = pSetFilePointer(
		hFile,
		lDistanceToMove,
		lpDistanceToMoveHigh,
		dwMoveMethod
		);
	return TReturn;
};

DWORD (WINAPI *pGetFileSize)(HANDLE hFile,LPDWORD lpFileSizeHigh) = GetFileSize;
DWORD WINAPI MyGetFileSize(HANDLE hFile,LPDWORD lpFileSizeHigh)
{

	CUserFileObject *pUserFileObj = NULL;
	BOOL bRes = FRecord.GetRecordData(hFile,&pUserFileObj);
	if (pUserFileObj)
	{
		return pUserFileObj->GetFileSize(lpFileSizeHigh);
	}

	DWORD TReturn = pGetFileSize(
		hFile,
		lpFileSizeHigh
		);


	return TReturn;
};

BOOL (WINAPI *pGetFileSizeEx)(HANDLE hFile,PLARGE_INTEGER lpFileSize) = GetFileSizeEx;
BOOL WINAPI MyGetFileSizeEx(HANDLE hFile,PLARGE_INTEGER lpFileSize)
{

	CUserFileObject *pUserFileObj = NULL;
	BOOL bRes = FRecord.GetRecordData(hFile,&pUserFileObj);
	if (pUserFileObj)
	{
		return pUserFileObj->GetFileSizeEx(lpFileSize);
	}


	BOOL TReturn = pGetFileSizeEx(
		hFile,
		lpFileSize
		);

	return TReturn;
};

BOOL (WINAPI *pGetFileInformationByHandle)(HANDLE hFile,LPBY_HANDLE_FILE_INFORMATION lpFileInformation ) = GetFileInformationByHandle;
BOOL WINAPI MyGetFileInformationByHandle(HANDLE hFile,LPBY_HANDLE_FILE_INFORMATION lpFileInformation )
{

	BOOL TReturn = pGetFileInformationByHandle(
		hFile,
		lpFileInformation
		);
	return TReturn;
};

BOOL WINAPI MyCloseHandle( HANDLE hObject )
{
 	if( FRecord.GetRecordCount() > 0 )
 	{
		CUserFileObject *pUserFileObj = NULL;
		BOOL bDelRes = FRecord.DelRecord(hObject,&pUserFileObj);
		if ( bDelRes )
		{

#if defined(DEBUG) || defined(_DEBUG)
			CString strFileName;

			CMemFileObject *pMemFileObj = pUserFileObj->GetMemFileObjPtr();
			if (pMemFileObj)
			{
				strFileName = pMemFileObj->GetFileName();
			}

			CString strMsgOut;
			strMsgOut.Format(L"关闭伪文件 Handle 0x%x Path：%s\n",hObject,strFileName);
			OutputDebugStringW(strMsgOut);
#endif

			if (pUserFileObj)
			{
				delete pUserFileObj;
			}

			return TRUE;
		}
	}

	return pCloseHandle( hObject );
	
};

BOOL (WINAPI *pDeleteFileW)( LPCWSTR lpFileName ) = DeleteFileW;
BOOL WINAPI MyDeleteFileW( LPCWSTR lpFileName )
{
	CString strFileName;
	strFileName = lpFileName;
	strFileName.MakeLower();
	CMemFileObject *pMemFileObject = NULL;
	FMemRecord.GetRecordData(strFileName,&pMemFileObject);

	if (pMemFileObject)
	{
		if ( pMemFileObject->CanDel() )
		{
#if defined(DEBUG) || defined(_DEBUG)
			CString strMsgOut;
			strMsgOut.Format(L"删除内存文件 Path：%s\n",pMemFileObject->GetFileName());
			OutputDebugStringW(strMsgOut);
#endif
			FMemRecord.DelRecord(strFileName,NULL);
			delete pMemFileObject;
		}
		else
		{
#if defined(DEBUG) || defined(_DEBUG)
			CString strMsgOut;
			strMsgOut.Format(L"****异常 删除内存文件 Path：%s\n",pMemFileObject->GetFileName());
			OutputDebugStringW(strMsgOut);
#endif
			return FALSE;
		}
		
	}

	BOOL TReturn = pDeleteFileW(
		lpFileName
		);
	return TReturn;
};

 BOOL (WINAPI *pFlushViewOfFile)(
	 __in LPCVOID lpBaseAddress,
	 __in SIZE_T dwNumberOfBytesToFlush 
	 ) = FlushViewOfFile;
 BOOL WINAPI MyFlushViewOfFile(
	 __in LPCVOID lpBaseAddress,
	 __in SIZE_T dwNumberOfBytesToFlush 
	 )
 {
	 return TRUE;
	 BOOL TReturn = pFlushViewOfFile(
		 lpBaseAddress,
		 dwNumberOfBytesToFlush
		 );
	 return TReturn;
 };

BOOL WINAPI InitStopWriteDisk()
{
	static BOOL bInit = FALSE;
	if ( FALSE == bInit )
	{
		bInit = TRUE;
		g_bEnableWriteDisk = TRUE;

 		DetourTransactionBegin();
 		DetourUpdateThread(GetCurrentThread());
 		DetourAttach(&(PVOID&)pCreateFileW, (PBYTE)MyCreateFileW);
 		DetourAttach(&(PVOID&)pWriteFile, (PBYTE)MyWriteFile);
 		DetourAttach(&(PVOID&)pReadFile, (PBYTE)MyReadFile);
 		DetourAttach(&(PVOID&)pSetFilePointer, (PBYTE)MySetFilePointer);
 		DetourAttach(&(PVOID&)pCloseHandle, (PBYTE)MyCloseHandle);
 		DetourAttach(&(PVOID&)pDeleteFileW, (PBYTE)MyDeleteFileW);
 		DetourAttach(&(PVOID&)pGetFileSize, (PBYTE)MyGetFileSize);
 		DetourAttach(&(PVOID&)pGetFileSizeEx, (PBYTE)MyGetFileSizeEx);
 		DetourAttach(&(PVOID&)pGetFileInformationByHandle, (PBYTE)MyGetFileInformationByHandle);
 		DetourAttach(&(PVOID&)pFlushViewOfFile, (PBYTE)MyFlushViewOfFile);
 		DetourTransactionCommit();

	}

	return FALSE;
}
BOOL WINAPI SetEnableWriteDisk(BOOL bEnableWriteDisk)
{
	g_bEnableWriteDisk = bEnableWriteDisk;

	return FALSE;
}