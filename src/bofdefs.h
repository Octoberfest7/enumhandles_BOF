#pragma once
#include <windows.h>

WINBASEAPI HANDLE WINAPI KERNEL32$CreateFileA (LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
WINBASEAPI WINBOOL WINAPI KERNEL32$CloseHandle (HANDLE hObject);
WINBASEAPI DWORD WINAPI KERNEL32$GetLastError();
WINBASEAPI HANDLE WINAPI KERNEL32$OpenProcess(DWORD, BOOL, DWORD);
WINBASEAPI BOOL WINAPI KERNEL32$QueryFullProcessImageNameA(HANDLE, DWORD, LPSTR, PDWORD);

#define CreateFileA                 KERNEL32$CreateFileA
#define CloseHandle                 KERNEL32$CloseHandle
#define GetLastError                KERNEL32$GetLastError
#define OpenProcess                 KERNEL32$OpenProcess
#define QueryFullProcessImageNameA  KERNEL32$QueryFullProcessImageNameA

NTSYSAPI NTSTATUS NTAPI NTDLL$NtQueryInformationFile(HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS);

#define NtQueryInformationFile      NTDLL$NtQueryInformationFile

WINBASEAPI void* __cdecl MSVCRT$calloc(size_t number, size_t size);
WINBASEAPI void __cdecl MSVCRT$free(void* memblock);
WINBASEAPI void* __cdecl MSVCRT$memset(void* dest, int c, size_t count);

#define calloc                      MSVCRT$calloc
#define free                        MSVCRT$free
#define memset                      MSVCRT$memset
