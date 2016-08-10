// wininet 上传文件

#include "stdafx.h"
#include <Windows.h>
#include <WinInet.h>
#include <atlstr.h>
#pragma comment(lib,"wininet.lib")

BYTE *BuildPostData(LPCWSTR pszFilePath,LPCWSTR pszBoudary,DWORD *pdwDataSize)
{
	CStringA straBoundary;
	CStringA strFormHeader;
	CStringA straFileName;

	straBoundary = pszBoudary;
	straFileName = pszFilePath;
	straFileName = straFileName.Right( straFileName.GetLength() - straFileName.ReverseFind('\\') - 1 );
	strFormHeader.Format(
		"%s\r\n"
		"Content-Disposition: form-data; name=\"file\"; filename=\"%s\"\r\n"
		"Content-Type: application/octet-stream\r\n\r\n"
		,straBoundary,straFileName
		);
	CStringA strFormTail;
	strFormTail.Format(
		"\r\n%s\r\n"
		"Content-Disposition: form-data; name=\"submit\"\r\n"
		"\r\n"
		"Submit\r\n"
		"%s--\r\n"
		,straBoundary,straBoundary
		);

	DWORD dwTotalDataLen = 0;

	dwTotalDataLen += strFormHeader.GetLength();
	dwTotalDataLen += strFormTail.GetLength();


	HANDLE hFile = CreateFile(pszFilePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	LARGE_INTEGER liFileSize;
	if ( INVALID_HANDLE_VALUE != hFile )
	{

		GetFileSizeEx(hFile,&liFileSize);

		dwTotalDataLen +=liFileSize.LowPart;
	}

	DWORD dwErrorCode = GetLastError();

	BYTE *pPostData = new BYTE[dwTotalDataLen];
	DWORD dwCurWriteLen = 0;
	memcpy_s(pPostData+dwCurWriteLen,dwTotalDataLen-dwCurWriteLen,strFormHeader.GetBuffer(),strFormHeader.GetLength());
	dwCurWriteLen+=strFormHeader.GetLength();

	if ( INVALID_HANDLE_VALUE != hFile )
	{
		DWORD dwReadLen = 0;
		ReadFile(hFile,pPostData+dwCurWriteLen,liFileSize.LowPart,&dwReadLen,NULL);

		dwCurWriteLen+=dwReadLen;

		CloseHandle(hFile);
	}

	memcpy_s(pPostData+dwCurWriteLen,dwTotalDataLen-dwCurWriteLen,strFormTail.GetBuffer(),strFormTail.GetLength());

	if (pdwDataSize)
	{
		*pdwDataSize = dwTotalDataLen;
	}

	return pPostData;
}

BOOL UploadFile( LPCWSTR pszUploadUrl,LPCWSTR pszFilePath )
{
	BOOL bUploadRes = FALSE;

	LPCWSTR pszBoundary = L"---------------------------7e029231808ce";

	CString strContentType;
	strContentType.Format(L"Content-Type: multipart/form-data; boundary=%s",pszBoundary);

	// for clarity, error-checking has been removed
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	PBYTE     pFileData = NULL;

	do 
	{
		DWORD dwUrlLen = 0;
		if ( NULL == pszUploadUrl || (dwUrlLen = wcslen(pszUploadUrl)) == 0 )
		{
			break;
		}

		if ( NULL == pszFilePath || wcslen(pszFilePath) == 0 || FALSE == PathFileExistsW(pszFilePath) )
		{
			break;
		}


		CString   strHostName;
		CString   strUrlPath;
		INTERNET_PORT nPort = 80;

		URL_COMPONENTSW UrlComp;
		ZeroMemory(&UrlComp,sizeof(UrlComp));
		UrlComp.dwStructSize = sizeof(UrlComp);
		UrlComp.lpszHostName = strHostName.GetBuffer( MAX_PATH );
		UrlComp.dwHostNameLength = MAX_PATH;
		UrlComp.lpszUrlPath = strUrlPath.GetBuffer(2000);
		UrlComp.dwUrlPathLength = 2000;

		BOOL bCrackRes = InternetCrackUrlW( pszUploadUrl , dwUrlLen , 0 , &UrlComp );

		nPort = UrlComp.nPort;
		strHostName.ReleaseBuffer();
		strUrlPath.ReleaseBuffer();

		if ( FALSE == bCrackRes )
		{
			break;
		}

		hSession = InternetOpen(NULL,INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if ( NULL == hSession )
		{
			break;
		}

		hConnect = InternetConnect(hSession, strHostName ,
			nPort,
			NULL,
			NULL,
			INTERNET_SERVICE_HTTP,
			0,
			1);

		if ( NULL == hConnect )
		{
			break;
		}

		hRequest = HttpOpenRequestW(hConnect, 
			L"POST",
			strUrlPath, 
			NULL, 
			NULL, 
			NULL, 
			0, 
			1);

		if( NULL == hRequest )
		{
			break;
		}



		DWORD dwDataSize = 0;
		pFileData = BuildPostData(pszFilePath,L"--"+CString(pszBoundary),&dwDataSize);

		bUploadRes = HttpSendRequest(hRequest, 
			strContentType, 
			strContentType.GetLength(), 
			pFileData, 
			dwDataSize);

	} while (FALSE);

	if (pFileData)
	{
		delete pFileData;
	}

	if (hRequest)
	{
		InternetCloseHandle(hRequest);
	}

	if (hConnect)
	{
		InternetCloseHandle(hConnect);
	}

	if (hSession)
	{
		InternetCloseHandle(hSession);
	}


	return bUploadRes;

}
