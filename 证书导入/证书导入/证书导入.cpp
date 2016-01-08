// 证书导入.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <Wincrypt.h>
#include <atlstr.h>
#pragma comment(lib,"Crypt32.lib")


#include "stdafx.h"
//#include "CertImportx.h"

#define MY_ENCODING_TYPE (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

int ImportCACert();
int ImportCACert(LPCTSTR szFileName);
int ImportCACert(BYTE* pBinByte , unsigned long binBytes);

// Global function for free handles...
void FreeHandles(HCERTSTORE hFileStore, PCCERT_CONTEXT pctx,     HCERTSTORE pfxStore, HCERTSTORE myStore )
{

	if (myStore)
		CertCloseStore(myStore, 0);

	if (pfxStore)
		CertCloseStore(pfxStore, CERT_CLOSE_STORE_FORCE_FLAG);

	if(pctx)
		CertFreeCertificateContext(pctx);

	if (hFileStore)
		CertCloseStore(hFileStore, 0);
}

// This function imports a CA certificate...
int ImportCACert(LPCTSTR szFileName)
{
	HANDLE hfile = INVALID_HANDLE_VALUE;

	BYTE pByte[4096] = {0} , pBinByte[8192]={0};
	unsigned long bytesRead = 0;
	unsigned long binBytes = 4096;

	// Open it...
	hfile = CreateFile(szFileName, FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (INVALID_HANDLE_VALUE == hfile)
		return -1;

	ReadFile( hfile , pByte, 4096, &bytesRead ,NULL );
	CloseHandle(hfile);

	//BOOL bRes = CryptStringToBinaryA( (LPCSTR)pByte , bytesRead ,CRYPT_STRING_BASE64HEADER , pBinByte , &binBytes ,NULL,NULL);
	//return ImportCACert(pBinByte , binBytes );

	return ImportCACert(pByte , bytesRead );
}

int ImportCACert(BYTE* pBinByte , unsigned long binBytes)
{
	HCERTSTORE pfxStore = 0;
	HCERTSTORE myStore = 0;
	HCERTSTORE hFileStore = 0;
	PCCERT_CONTEXT pctx = NULL;
	DWORD err = 0;

	pctx = CertCreateCertificateContext(MY_ENCODING_TYPE, (BYTE*)pBinByte , binBytes );
	if(pctx == NULL)
	{
		DWORD err = GetLastError();
		FreeHandles(hFileStore,pctx, pfxStore, myStore);   
		return err;
	}

	// we open the store for the CA
	hFileStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, L"Root" );
	if (!hFileStore)
	{
		DWORD err = GetLastError();
		FreeHandles(hFileStore,pctx, pfxStore, myStore);   
		return err;
	}

	if( !CertAddCertificateContextToStore(hFileStore, pctx, CERT_STORE_ADD_NEW, 0) )
	{
		err = GetLastError();
		if( CRYPT_E_EXISTS == err )
		{
			//            if( AfxMessageBox("An equivalent previous personal certificate already exists. Overwrite ? (Yes/No)", MB_YESNO) == IDYES)
			{
				if( !CertAddCertificateContextToStore(hFileStore, pctx , CERT_STORE_ADD_REPLACE_EXISTING, 0))
				{
					err = GetLastError();
					FreeHandles(hFileStore,pctx, pfxStore, myStore);
					                    
					return err;
				}
			}
		}
		else
		{
			FreeHandles(hFileStore, pctx , pfxStore , myStore);
			
			return err;
		}
	}
	FreeHandles(hFileStore,pctx, pfxStore, myStore);
	return 0;
}

BOOL ImportCACrl( LPCTSTR szFileName )
{
	HANDLE hCrlFile = INVALID_HANDLE_VALUE;
	HCERTSTORE hFileStore = NULL;
	PCCRL_CONTEXT pCrlContext = NULL;
	PCCRL_CONTEXT pOldCrlContext = NULL;
	BYTE *pFileData = NULL;

	BOOL bImportRes = FALSE;
	do 
	{
		hCrlFile = CreateFile(szFileName, FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (INVALID_HANDLE_VALUE == hCrlFile)
			break;
		
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow = GetFileSize(hCrlFile,&dwSizeHigh);
		pFileData = new BYTE[dwSizeLow];

		
		DWORD dwTotalReadLen = 0;
		while( dwTotalReadLen < dwSizeLow )
		{
			DWORD dwReadLen = 0;
			BOOL bRes = ReadFile( hCrlFile , pFileData + dwTotalReadLen , dwSizeLow - dwTotalReadLen , &dwReadLen ,NULL );
			if ( FALSE == bRes )
			{
				break;
			}
			dwTotalReadLen+=dwReadLen;
		}


		pCrlContext = CertCreateCRLContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING , pFileData , dwTotalReadLen );

		if ( NULL == pCrlContext )
		{
			break;
		}

		hFileStore = CertOpenStore(CERT_STORE_PROV_SYSTEM, 0, 0, CERT_STORE_OPEN_EXISTING_FLAG | CERT_SYSTEM_STORE_LOCAL_MACHINE, L"Root" );
		if ( NULL == hFileStore )
		{
			break;
		}

		bImportRes = CertAddCRLContextToStore(hFileStore,pCrlContext,CERT_STORE_ADD_ALWAYS,&pOldCrlContext);
	} while (FALSE);


	if ( INVALID_HANDLE_VALUE != hCrlFile)
	{
		CloseHandle(hCrlFile);
	}

	if (hFileStore)
	{
		CertCloseStore(hFileStore,0);
	}
	
	if (pCrlContext)
	{
		CertFreeCRLContext(pCrlContext);
	}

	if (pOldCrlContext)
	{
		CertFreeCRLContext(pOldCrlContext);
	}
	
	if (pFileData)
	{
		delete pFileData;
	}

	return bImportRes;
}


int _tmain(int argc, _TCHAR* argv[])
{

	int nValue = 0;

	for (int i=1;i<=14;i++)
	{
		CString strFile;
		strFile.Format(L"C:\\cert (%d).cer",i);
		ImportCACert(strFile);
	}
	

	


	return 0;
}

