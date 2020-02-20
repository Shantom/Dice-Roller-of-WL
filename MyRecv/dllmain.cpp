// dllmain.cpp : 定义 DLL 应用程序的入口点。
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <shlobj.h>

#define SIZE 6
BYTE oldBytes[SIZE] = { 0 }; //系统recv的前六个字节的备份
BYTE JMP[SIZE] = { 0 };
DWORD oldProtect, myProtect = PAGE_EXECUTE_READWRITE;
typedef int (WINAPI* pRecv)(UINT, PSTR, int, int);
pRecv pOrigAddress = NULL; //系统recv的地址 
const char head[] = { 0x10,0x6b,0xa5,0xad,0xba,0x7e,0xac };
BOOL recorded = 0;
CHAR socketFile[MAX_PATH], resFile[MAX_PATH];

void BeginRedirect(LPVOID);
int WINAPI MyRecv(UINT s, PSTR buf, int len, int flags);
BOOL FindProcessPid(LPCTSTR ProcessName, DWORD& dwPid);
void getFilePaths();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        getFilePaths();
        hModule = GetModuleHandle(L"WS2_32.dll");
        pOrigAddress = (pRecv)GetProcAddress(hModule, "recv");
        if (pOrigAddress != NULL)
            BeginRedirect(MyRecv);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

int WINAPI MyRecv(UINT s, PSTR buf, int len, int flags)
{
    VirtualProtect((LPVOID)pOrigAddress, SIZE, myProtect, &oldProtect);//修改recv前六个字节权限
    memcpy(pOrigAddress, oldBytes, SIZE);//因为下一行我需要调用recv，所以暂时恢复recv的备份
    int retValue = recv(s, buf, len, flags);
    memcpy(pOrigAddress, JMP, SIZE);//再覆盖回来
    VirtualProtect((LPVOID)pOrigAddress, SIZE, oldProtect, &myProtect);

    DWORD targetID;
    FindProcessPid(_T("Roll.exe"), targetID);
    WSAPROTOCOL_INFO ProtocolInfo;
    int err = WSADuplicateSocket(s, targetID, &ProtocolInfo);

    if (!recorded)
    {
        FILE* sock = fopen(socketFile, "wb");
        fwrite(&ProtocolInfo, sizeof(WSAPROTOCOL_INFO), 1, sock);
        fclose(sock);
        //recorded = 1;
    }

    if (retValue == 12 && !strncmp(head, buf, 6))
    {
        FILE* result = fopen(resFile, "w");
        if (result)
        {
            for (int i = 0; i < 5; i++)
            {
                unsigned char num = 0xad ^ buf[7 + i];
                fprintf(result, "%d ", (int)num);
            }
            fputc(10, result);
            fclose(result);
        }
    }
    return retValue;
}

void BeginRedirect(LPVOID newFunction)
{
    BYTE tempJMP[SIZE] = { 0xE9, 0x90, 0x90, 0x90, 0x90, 0xC3 };
    memcpy(JMP, tempJMP, SIZE);
    DWORD JMPSize = ((DWORD)newFunction - (DWORD)pOrigAddress - 5); // 需要跳过的字节数
    VirtualProtect((LPVOID)pOrigAddress, SIZE, PAGE_EXECUTE_READWRITE, &oldProtect); //修改recv前六个字节权限
    memcpy(oldBytes, pOrigAddress, SIZE);//备份recv前六个字节
    memcpy(&JMP[1], &JMPSize, 4);//将真正需要跳过的字节数填到jmp指令的后半段
    memcpy(pOrigAddress, JMP, SIZE);//将jmp指令复写到recv的最开始
    VirtualProtect((LPVOID)pOrigAddress, SIZE, oldProtect, &myProtect);
}

BOOL FindProcessPid(LPCTSTR ProcessName, DWORD& dwPid)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return(FALSE);
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return(FALSE);
    }

    BOOL    bRet = FALSE;
    do
    {
        const wchar_t* thisProcess = (pe32.szExeFile);
        if (!_tcscmp(ProcessName, thisProcess))
        {
            dwPid = pe32.th32ProcessID;
            bRet = TRUE;
            break;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return bRet;
}

void getFilePaths()
{
    HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, socketFile);
    strcpy(resFile, socketFile);
    char stail[] = "\\wlRoll\\socket";
    char rtail[] = "\\wlRoll\\res";
    strcat(socketFile, stail);
    strcat(resFile, rtail);
}
