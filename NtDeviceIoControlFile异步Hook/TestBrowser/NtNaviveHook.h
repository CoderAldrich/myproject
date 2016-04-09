#pragma once

#include <WinNT.h>
#include <winternl.h>
//#include <NTSTATUS.H>

typedef NTSTATUS (WINAPI *TypeNtDeviceIoControlFile)(
	HANDLE FileHandle,
	HANDLE Event,
	PVOID ApcRoutine,
	PVOID ApcContext,
	PVOID IoStatusBlock,
	ULONG IoControlCode,
	PVOID InputBuffer,
	ULONG InputBufferLength,
	PVOID OutputBuffer,
	ULONG OutputBufferLength 
	);

extern TypeNtDeviceIoControlFile pNtDeviceIoControlFile;

NTSTATUS WINAPI MyNtDeviceIoControlFile(
										HANDLE FileHandle,
										HANDLE Event,
										PVOID ApcRoutine,
										PVOID ApcContext,
										PVOID IoStatusBlock,
										ULONG IoControlCode,
										PVOID InputBuffer,
										ULONG InputBufferLength,
										PVOID OutputBuffer,
										ULONG OutputBufferLength 
										);

extern BOOL (WINAPI *pCloseHandle)(
								   __in HANDLE hObject 
								   );

BOOL WINAPI MyCloseHandle( HANDLE hObject );
