#ifndef VNMFUNC_H
#define VNMFUNC_H

DWORD GetDLLBaseAddress(char moduleName[],DWORD pid);
DWORD GetBaseAddress(HWND hwnd, HANDLE hProc ,HANDLE hThread);
DWORD seekbytes (HANDLE hProc,unsigned char pattern[] , DWORD base_addr , DWORD end_addr, SIZE_T numOfBytes);
IMAGE_NT_HEADERS *getPEHeader(HANDLE hProcess, DWORD baseAddr);

#endif 