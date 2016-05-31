// Shell.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Shell.h"

 #pragma comment(linker, "/merge:.data=.text") 
 #pragma comment(linker, "/merge:.rdata=.text")


#pragma comment(linker, "/section:.text,RWE")
//#pragma comment(linker,"/entry:main")

//函数和变量的声明
DWORD MyGetProcAddress(HMODULE hModule);		//自定义GetProcAddress
HMODULE	GetKernel32Addr();		//获取Kernel32加载基址



DWORD WINAPI TdRun( PVOID pParam )
{
	fnGetProcAddress	g_pfnGetProcAddress		= NULL;
	fnLoadLibraryA		g_pfnLoadLibraryA		= NULL;
	fnMessageBox		g_pfnMessageBoxA		= NULL;

	char mystring[] = {'L','o','a','d','L','i','b','r','a','r','y','A',0};

	HMODULE hKernel32		= GetKernel32Addr();

	g_pfnGetProcAddress		= (fnGetProcAddress)MyGetProcAddress(hKernel32);

	if ( NULL == g_pfnGetProcAddress )
	{
		return -1;
	}

	g_pfnLoadLibraryA		= (fnLoadLibraryA)g_pfnGetProcAddress(hKernel32, mystring);


	char mystring1[] = {'U','s','e','r','3','2','.','d','l','l',0};


	HMODULE hUser32 = g_pfnLoadLibraryA(mystring1);

	char mystring2[] = {'M','e','s','s','a','g','e','B','o','x','A',0};

	g_pfnMessageBoxA = (fnMessageBox)g_pfnGetProcAddress(hUser32,mystring2);
	g_pfnMessageBoxA(NULL,mystring2,mystring2,0);

	return 0;
}


//************************************************************
// 函数名称:	GetKernel32Addr
// 函数说明:	获取Kernel32加载基址
// 作	者:	cyxvc
// 时	间:	2015/12/28
// 返 回	值:	HMODULE
//************************************************************
//定位Kernel32
__declspec(naked) HMODULE GetKernel32Addr()
{
	__asm
	{
		XOR     ECX, ECX; ECX = 0
		MOV     ESI, FS:[ECX + 0x30]; ESI = &(PEB)([FS:0x30])
		MOV     ESI, [ESI + 0x0C]; ESI = PEB->Ldr
		MOV     ESI, [ESI + 0x1C]; ESI = PEB->Ldr.InInitOrder
	next_module :
		MOV     EAX, [ESI + 0x08]; EBP = InInitOrder[X].base_address
		MOV     EDI, [ESI + 0x20]; EBP = InInitOrder[X].module_name(unicode)
		MOV     ESI, [ESI]; ESI = InInitOrder[X].flink(next module)
		CMP[EDI + 12 * 2], CL; modulename[12] == 0 ?
		JNE     next_module; No: try next module.
		ret
	}
}


//************************************************************
// 函数名称:	MyGetProcAddress
// 函数说明:	自定义GetProcAddress
// 作	者:	cyxvc
// 时	间:	2015/12/28
// 返 回	值:	DWORD
//************************************************************
DWORD MyGetProcAddress(HMODULE hModule)
{
	//1.获取DOS头
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)(PBYTE)hModule;
	//2.获取NT头
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)((PBYTE)hModule + pDosHeader->e_lfanew);
	//3.获取导出表的结构体指针
	PIMAGE_DATA_DIRECTORY pExportDir =
		&(pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]);

	PIMAGE_EXPORT_DIRECTORY pExport = 
		(PIMAGE_EXPORT_DIRECTORY)((PBYTE)hModule + pExportDir->VirtualAddress);

	//EAT
	PDWORD pEAT = (PDWORD)((DWORD)hModule + pExport->AddressOfFunctions);
	//ENT
	PDWORD pENT = (PDWORD)((DWORD)hModule + pExport->AddressOfNames);
	//EIT
	PWORD pEIT = (PWORD)((DWORD)hModule + pExport->AddressOfNameOrdinals);

	//4.遍历导出表，获取GetProcAddress()函数地址
	DWORD dwNumofFun = pExport->NumberOfFunctions;
	DWORD dwNumofName = pExport->NumberOfNames;
	for (DWORD i = 0; i < dwNumofFun; i++)
	{
		//如果为无效函数，跳过
		if (pEAT[i] == NULL)
			continue;
		//判断是以函数名导出还是以序号导出
		DWORD j = 0;
		for (; j < dwNumofName; j++)
		{
			if (i == pEIT[j])
			{
				break;
			}
		}
		if (j != dwNumofName)
		{
			//如果是函数名方式导出的
			//函数名
			char* ExpFunName = (CHAR*)((PBYTE)hModule + pENT[j]);
			//进行对比,如果正确返回地址
 			//if (!strcmp(ExpFunName, "GetProcAddress"))
			if(    ExpFunName[0] == 'G'
				&& ExpFunName[1] == 'e'
				&& ExpFunName[2] == 't'
				&& ExpFunName[3] == 'P'
				&& ExpFunName[4] == 'r'
				&& ExpFunName[5] == 'o'
				&& ExpFunName[6] == 'c'
				&& ExpFunName[7] == 'A'
				&& ExpFunName[8] == 'd'
				&& ExpFunName[9] == 'd'
				&& ExpFunName[10] == 'r'
				&& ExpFunName[11] == 'e'
				&& ExpFunName[12] == 's'
				&& ExpFunName[13] == 's'
				)
 			{
 				return pEAT[i] + pNtHeader->OptionalHeader.ImageBase;
 			}
		}
		else
		{
			//序号
		}
	}
	return 0;
}