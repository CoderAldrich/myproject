// �ڴ�ִ��EXE.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "�ڴ�ִ��EXE.h"

typedef int (*TypePEStartup)();

#include <atlstr.h>

// �Ƿ�������ض����б�   
BOOL HasRelocationTable(PIMAGE_NT_HEADERS peH)   
{   
	return (peH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress)   
		&& (peH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);   
}   




#pragma pack(push, 1)   
typedef struct{   
	unsigned long VirtualAddress;   
	unsigned long SizeOfBlock;   
} *PImageBaseRelocation;   
#pragma pack(pop)   

// �ض���PE�õ��ĵ�ַ   
void DoRelocation(PIMAGE_NT_HEADERS peH, void *OldBase, void *NewBase)   
{   
	unsigned long Delta = (unsigned long)NewBase - peH->OptionalHeader.ImageBase;   
	PImageBaseRelocation p = (PImageBaseRelocation)((unsigned long)OldBase    
		+ peH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);   
	while(p->VirtualAddress + p->SizeOfBlock)   
	{   
		unsigned short *pw = (unsigned short *)((int)p + sizeof(*p));   
		for(unsigned int i=1; i <= (p->SizeOfBlock - sizeof(*p)) / 2; ++i)   
		{   
			if((*pw) & 0xF000 == 0x3000){   
				unsigned long *t = (unsigned long *)((unsigned long)(OldBase) + p->VirtualAddress + ((*pw) & 0x0FFF));   
				*t += Delta;   
			}   
			++pw;   
		}   
		p = (PImageBaseRelocation)pw;   
	}   
}

VOID ImportReloc( BYTE *pPeData,PIMAGE_NT_HEADERS32 pNtHeader,int nImportType )
{
	PIMAGE_IMPORT_DESCRIPTOR pImportTable = (PIMAGE_IMPORT_DESCRIPTOR)(pPeData + pNtHeader->OptionalHeader.DataDirectory[nImportType].VirtualAddress);   
	DWORD                    dwImportTableSize= pNtHeader->OptionalHeader.DataDirectory[nImportType].Size;


	while ( pImportTable->Characteristics)
	{
		BYTE *pDllName = pPeData+pImportTable->Name;
		HMODULE hImportDll = LoadLibraryA((char *)pDllName);
		if (hImportDll)
		{
			//PIMAGE_THUNK_DATA32 pThunkData = (PIMAGE_THUNK_DATA32)(pPeData + pImportTable->OriginalFirstThunk/*FirstThunk*/);
			PIMAGE_THUNK_DATA32 pThunkData = (PIMAGE_THUNK_DATA32)(pPeData + pImportTable->FirstThunk);

			while (pThunkData->u1.AddressOfData && pThunkData->u1.AddressOfData < 0x80000000 )
			{
				PIMAGE_IMPORT_BY_NAME pImportByName = (PIMAGE_IMPORT_BY_NAME)(pPeData+pThunkData->u1.AddressOfData);

				BYTE *pFunAddr = (BYTE *)GetProcAddress(hImportDll,(char *)(pImportByName->Name));

				BYTE *pIATAddr = (BYTE *)(pThunkData);

				DWORD dwOldProtectFlag = 0;
				BOOL bRes = VirtualProtect(pIATAddr,4,PAGE_EXECUTE_READWRITE,&dwOldProtectFlag);

				*((DWORD *)pIATAddr) = (DWORD)pFunAddr;

				pThunkData++;
			}
		}


		pImportTable++;
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HMODULE hExeModule = NULL;

	//hExeModule = LoadLibraryW(L"F:\\ΰ��\\�ڴ�ִ��EXE\\Debug\\TestPE.exe");
	//hExeModule = LoadLibraryW(L"C:\\Users\\GAOZAN\\Documents\\Tencent Files\\2592705588\\FileRecv\\NewTool.exe");
	//hExeModule = LoadLibraryW(L"G:\\���Թ���\\����.exe");
	//hExeModule = LoadLibraryW(L"F:\\�����Ŀ\\WN58Soft\\58��ά�����\\��ά��Ŀ\\��Ϸ�˵�ҵ��\\Release\\IEWebPage.exe");
	//hExeModule = LoadLibraryW(L"G:\\��������\\DLL Export Viewer v1.50\\DLL Export Viewer v1.50\\dllexp.exe");
	hExeModule = LoadLibraryW(L"TestPE.exe");

	BYTE *pPeData = (BYTE *)hExeModule;

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pPeData;

	if ( pDosHeader->e_magic == 0x5A4D )
	{
		PIMAGE_NT_HEADERS32 pNtHeader = (PIMAGE_NT_HEADERS32)(pPeData+pDosHeader->e_lfanew);
		if ( pNtHeader->Signature == 0x00004550 )
		{
			ImportReloc( pPeData,pNtHeader,IMAGE_DIRECTORY_ENTRY_IMPORT );
			ImportReloc( pPeData,pNtHeader,IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT );
			ImportReloc( pPeData,pNtHeader,IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT );


			BYTE *pEntryPoint = pPeData + pNtHeader->OptionalHeader.AddressOfEntryPoint;

			TypePEStartup pStartup = (TypePEStartup)pEntryPoint;
			pStartup();
		}

	}

	return (int)0;
}

