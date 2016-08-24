#pragma once

HANDLE LibCreateRemoteThread
(
 __in HANDLE hProcess, 
 __in LPTHREAD_START_ROUTINE lpStartAddress,
 __in_opt LPVOID lpParameter, 
 __in DWORD dwCreationFlags,
 __out_opt LPDWORD lpThreadId
 );