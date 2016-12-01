// Ping.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>  
#include <winsock.h>  
#include <stdio.h>  
#include <string.h>  


typedef struct tagIPINFO  
{  
	u_char Ttl;             // Time To Live  
	u_char Tos;             // Type Of Service  
	u_char IPFlags;         // IP flags  
	u_char OptSize;         // Size of options data  
	u_char FAR *Options;    // Options data buffer  
}IPINFO, *PIPINFO;  


typedef struct tagICMPECHO  
{  
	u_long Source;          // Source address  
	u_long Status;          // IP status  
	u_long RTTime;          // Round trip time in milliseconds  
	u_short DataSize;       // Reply data size  
	u_short Reserved;       // Unknown  
	void FAR *pData;        // Reply data buffer  
	IPINFO  ipInfo;         // Reply options  
}ICMPECHO, *PICMPECHO;  




// ICMP.DLL Export Function Pointers  
HANDLE (WINAPI *pIcmpCreateFile)(VOID);  
BOOL (WINAPI *pIcmpCloseHandle)(HANDLE);  
DWORD (WINAPI *pIcmpSendEcho)(HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD);


//  
//  
BOOL Ping(const char *lpdest,int nPintTimes,int nTimeOut,DWORD *pdwAverageTime )
{  
	WSADATA wsaData;            // WSADATA  
	ICMPECHO icmpEcho;          // ICMP Echo reply buffer  
	HMODULE hndlIcmp;            // LoadLibrary() handle to ICMP.DLL  
	HANDLE hndlFile;            // Handle for IcmpCreateFile()  
	//LPHOSTENT pHost;            // Pointer to host entry structure  
	struct in_addr iaDest;      // Internet address structure  
	DWORD *dwAddress;           // IP Address  
	IPINFO ipInfo;              // IP Options structure  
	int nRet;                   // General use return code  
	DWORD dwRet;                // DWORD return code  
	int x;  

	*pdwAverageTime = 0;

	// Dynamically load the ICMP.DLL  
	hndlIcmp = LoadLibrary(L"ICMP.DLL");  
	if (hndlIcmp == NULL)  
	{  
		fprintf(stderr,"\nCould not load ICMP.DLL\n");  
		return FALSE;  
	}  
	// Retrieve ICMP function pointers  
	pIcmpCreateFile = (HANDLE (WINAPI *)())GetProcAddress(hndlIcmp,"IcmpCreateFile");  
	pIcmpCloseHandle = (BOOL (WINAPI *)(HANDLE))GetProcAddress(hndlIcmp,"IcmpCloseHandle");  
	pIcmpSendEcho = (DWORD (WINAPI *)(HANDLE,DWORD,LPVOID,WORD,PIPINFO,LPVOID,DWORD,DWORD))GetProcAddress(hndlIcmp,"IcmpSendEcho");  
	// Check all the function pointers  
	if (pIcmpCreateFile == NULL     ||   
		pIcmpCloseHandle == NULL    ||  
		pIcmpSendEcho == NULL)  
	{  
		fprintf(stderr,"\nError getting ICMP proc address\n");  
		FreeLibrary(hndlIcmp);  
		return FALSE;  
	}  


	// Init WinSock  
	nRet = WSAStartup(0x0101, &wsaData );  
	if (nRet)  
	{  
		fprintf(stderr,"\nWSAStartup() error: %d\n", nRet);   
		WSACleanup();  
		FreeLibrary(hndlIcmp);  
		return FALSE;  
	}  
	// Check WinSock version  
	if (0x0101 != wsaData.wVersion)  
	{  
		fprintf(stderr,"\nWinSock version 1.1 not supported\n");  
		WSACleanup();  
		FreeLibrary(hndlIcmp);  
		return FALSE;  
	}  

	iaDest.s_addr = inet_addr(lpdest);  


	// Copy the IP address  
	dwAddress = (DWORD *)(&iaDest);  

	DWORD dwTotalTime = 0;
	// Get an ICMP echo request handle          
	hndlFile = pIcmpCreateFile();  
	for (x = 0; x < nPintTimes; x++)  
	{  
		// Set some reasonable default values  
		ipInfo.Ttl = 255;  
		ipInfo.Tos = 0;  
		ipInfo.IPFlags = 0;  
		ipInfo.OptSize = 0;  
		ipInfo.Options = NULL;  
		//icmpEcho.ipInfo.Ttl = 256;  
		// Reqest an ICMP echo  
		dwRet = pIcmpSendEcho(  
			hndlFile,       // Handle from IcmpCreateFile()  
			*dwAddress,     // Destination IP address  
			NULL,           // Pointer to buffer to send  
			0,              // Size of buffer in bytes  
			&ipInfo,        // Request options  
			&icmpEcho,      // Reply buffer  
			sizeof(struct tagICMPECHO),  
			5000);          // Time to wait in milliseconds  
		// Print the results  
		iaDest.s_addr = icmpEcho.Source;  
		if (icmpEcho.Status)
		{
			return FALSE;
		}
		
		dwTotalTime += icmpEcho.RTTime;
	}  

	*pdwAverageTime = dwTotalTime/nPintTimes;

	pIcmpCloseHandle(hndlFile);  
	FreeLibrary(hndlIcmp);  
	WSACleanup();

	return TRUE;
} 