#ifndef INJECT_H
#define INJECT_H
#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <io.h>
#include <tchar.h>

class Inject
{
public:
    Inject();

    BOOL CheckDllInProcess(DWORD dwPID, LPCTSTR szDllPath);

    BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath);

    BOOL EjectDll(DWORD dwPID, LPCTSTR szDllPath);

    BOOL FindProcessPid(LPCTSTR ProcessName, DWORD& dwPid);

};

#endif // INJECT_H
