#include "stdafx.h"


#include <windows.h>  
#include <psapi.h>  
#pragma comment(lib,"psapi.lib")


#define MakeREX(W,R,X,B) __asm __emit 0100##W##R##X##B##b  
#define rep __asm __emit 0xf3  
#pragma pack(1)  
typedef struct _STUBARGS  
{  
	unsigned __int64 addr;  
	unsigned int argc;  
	__int64 argv[1];  
}STUBARGS,*PSTUBARGS;  
#pragma pack()  
__declspec(naked) void __stdcall stubto64(void)  
{  
	__asm   
	{  
		push esi  
			push edi  
			push ebp  
			lea ebp,[esp+44]  


		mov eax,[ebp+STUBARGS.argc]  
		neg eax  
			lea esp,[esp+eax*8]  
		neg eax  
			and esp,not 15  


			lea esi,[ebp+STUBARGS.argv]  
		mov edi,esp  
			mov ecx,eax  
			cld  


			rep  
			MakeREX(1,0,0,0)  
			movsd  


			dec eax  
			jl callproc  
			MakeREX(1,0,0,0)  
			mov ecx,dword ptr[ebp+STUBARGS.argv+8*0]  
		dec eax  
			jl callproc  
			MakeREX(1,0,0,0)  
			mov edx,dword ptr[ebp+STUBARGS.argv+8*1]  
		dec eax  
			jl callproc  
			MakeREX(1,1,0,0)  
			mov eax,dword ptr[ebp+STUBARGS.argv+8*2]  
		dec eax  
			jl callproc  
			MakeREX(1,1,0,0)  
			mov ecx,dword ptr[ebp+STUBARGS.argv+8*3]  



callproc:  
		MakeREX(1,0,0,0)  
			call dword ptr[ebp+STUBARGS.addr]  

		MakeREX(1,0,0,0)  
			mov edx,eax  
			MakeREX(1,0,0,0)  
			shr edx,32  


			lea esp,[ebp-44]  
		pop ebp  
			pop edi  
			pop esi  
			retf  
	}  
}  


__declspec(naked) __int64 __cdecl lcall(void*off,unsigned short seg,...)//__int64 func,unsigned argc,...)  
{  
	__asm call Far ptr[esp+4]  
	__asm ret  
}  


#define Call64Proc(addr,argc,...) lcall(stubto64,0x33,(unsigned __int64)addr,argc,__VA_ARGS__)  
#define To64(x) ((__int64)(x))  
HMODULE GetNativeNtdll(void)  
{  
	MEMORY_BASIC_INFORMATION mbi;static HMODULE ntdll=0;char*lastone=0;  
	if(ntdll)return ntdll;  
	for(char*p=0;size_t(p)<0x80000000;p+=mbi.RegionSize)  
	{  
		if(!VirtualQueryEx((HANDLE)-1,p,&mbi,sizeof mbi))break;  
		if(mbi.AllocationBase==lastone||mbi.State!=MEM_COMMIT||mbi.Type!=MEM_IMAGE)continue;  
		wchar_t name[256];  
		const wchar_t ntdllname[]=L"\\windows\\system32\\ntdll.dll";  
		const unsigned int ntdllnamelen=sizeof ntdllname/sizeof(wchar_t)-1;  
		DWORD namelen;  
		if((namelen=GetMappedFileNameW((HANDLE)-1,mbi.AllocationBase,name,256))>ntdllnamelen)  
		{  

			if(wcsicmp(ntdllname,name+namelen-ntdllnamelen)==0)  
			{  
				PIMAGE_DOS_HEADER dosheader=PIMAGE_DOS_HEADER(mbi.AllocationBase);  
				PIMAGE_NT_HEADERS64 ntheaders=PIMAGE_NT_HEADERS64(size_t(dosheader)+dosheader->e_lfanew);  
				if(ntheaders->FileHeader.Machine==IMAGE_FILE_MACHINE_AMD64)return (HMODULE)mbi.AllocationBase;  
			}  
		}  
		lastone=(char*)mbi.AllocationBase;  
	}  
	return ntdll;  
}  


void* GetProcAddress64(const void*BaseAddress,const char*Name)  
{  
	UINT_PTR base=reinterpret_cast<UINT_PTR>(BaseAddress);  
	PIMAGE_DOS_HEADER DosHeader=PIMAGE_DOS_HEADER(BaseAddress);  
	PIMAGE_NT_HEADERS64 NtHeaders=PIMAGE_NT_HEADERS64(size_t(DosHeader)+DosHeader->e_lfanew);;  
	DWORD ExpRVA=NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;  
	DWORD ExpSize=NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;  
	if(ExpRVA&&ExpSize)  
	{  
		PIMAGE_EXPORT_DIRECTORY ExpDir=reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(base+ExpRVA);  
		PDWORD NameList=reinterpret_cast<PDWORD>(base+ExpDir->AddressOfNames);  
		PWORD OrdinalList=reinterpret_cast<PWORD>(base+ExpDir->AddressOfNameOrdinals);  
		PDWORD FunctionList=reinterpret_cast<PDWORD>(base+ExpDir->AddressOfFunctions);  
		for(DWORD i=0;i<ExpDir->NumberOfNames;++i)  
		{  
			const char*ExpName=reinterpret_cast<const char*>(base+NameList[i]);  
			if(strcmp(Name,ExpName)==0)  
			{  
				WORD Ordinal=OrdinalList[i];  
				return (char*)base+FunctionList[Ordinal];  
			}  
		}  
	}  
	return 0;  
}  






HANDLE CreateThread64(HANDLE ProcessHandle,void*ThreadProc,void*Parameter,int Suspended)  
{  
	__int64 hThread=0,cid[2];  
	static void*RtlCreateUserThread64=0;  
	if(!RtlCreateUserThread64)  
		RtlCreateUserThread64=GetProcAddress64(GetNativeNtdll(),"RtlCreateUserThread");  
	Call64Proc(RtlCreateUserThread64,10,To64(ProcessHandle),To64(0),To64(Suspended),To64(0),To64(0),To64(0),To64(ThreadProc),To64(Parameter),To64(&hThread),To64(cid));  
	return (HANDLE)hThread;  
}  


typedef struct _UNICODE_STRING64  
{  
	USHORT Length;  
	USHORT MaximumLength;  
	ULONGLONG  Buffer;  
} UNICODE_STRING64, *PUNICODE_STRING64;  
HMODULE InjectLibrary64(HANDLE ProcessHandle,wchar_t*Name,unsigned short NameLength)  
{  
	void*stub,*buf;unsigned stublen;  
	__asm  
	{  


		mov stub,offset stubstart  
			mov stublen,offset stubend  
			sub stublen,offset stubstart  
			jmp stubend  
stubstart:  
		MakeREX(1,1,0,0)  
			lea eax,[ecx+8]  
		MakeREX(1,1,0,0)  
			lea ecx,[esp-8]  
		MakeREX(1,0,0,0)  
			sub esp,48  
			xor ecx,ecx  
			xor edx,edx  

			MakeREX(1,0,0,1)  
			call [eax-8]  
		MakeREX(1,0,0,0)  
			add esp,48  
			MakeREX(1,0,0,0)  
			mov eax,[esp-8]  
		MakeREX(1,0,0,0)  
			mov edx,eax  
			MakeREX(1,0,0,0)  
			shr edx,32  
			ret  
stubend:  
	}  
	static struct   
	{  
		__int64 LdrLoadDll;  
		UNICODE_STRING64 us;  
	}pre={0};  
	buf=VirtualAllocEx(ProcessHandle,0,NameLength+sizeof pre+stublen,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);  
	if(buf)  
	{  
		if(!pre.LdrLoadDll)pre.LdrLoadDll=(__int64)GetProcAddress64(GetNativeNtdll(),"LdrLoadDll");  
		pre.us.Length=pre.us.MaximumLength=NameLength;  
		pre.us.Buffer=(ULONGLONG)buf+sizeof pre;  
		WriteProcessMemory(ProcessHandle,buf,&pre,sizeof pre,0);  
		WriteProcessMemory(ProcessHandle,(void*)pre.us.Buffer,Name,NameLength,0);  
		void*code=(char*)buf+sizeof pre+NameLength;  
		WriteProcessMemory(ProcessHandle,code,stub,stublen,0);  
		HANDLE hThread=CreateThread64(ProcessHandle,code,buf,0);  
		if(hThread)  
		{  
			if(WaitForSingleObject(hThread,-1))  
			{  
				DWORD ec;  
				GetExitCodeThread(hThread,&ec);  
				CloseHandle(hThread);  
				return (HMODULE)ec;  
			}  
			CloseHandle(hThread);  
		}  
		VirtualFreeEx(ProcessHandle,buf,0,MEM_RELEASE);  
	}  
	return 0;  
}  
