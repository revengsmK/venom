#include <windows.h>
#include <tlhelp32.h>

/*Blumentals Program Protector v4.x *VeNoM* helper functions*/


DWORD GetDLLBaseAddress(char moduleName[],DWORD pid){
	
	HANDLE hSnapshot;
	MODULEENTRY32 modEntry;
	DWORD base_address = 0;

	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,pid);
	
	// Initialize dwSize member before calling Module32First
	
	modEntry.dwSize = sizeof(MODULEENTRY32);
	
	// Get first module in the snapshot
	
	if(!Module32First(hSnapshot,&modEntry)){
		MessageBox(NULL,"Failed to load first module!\nAborting...","Error",MB_OK | MB_ICONEXCLAMATION);
		CloseHandle(hSnapshot);
		return 0;
	}
	
	// Search for specified module in the process
	
	while(Module32Next(hSnapshot,&modEntry)){
		
		if(strcmp(moduleName,modEntry.szModule) == 0)
		{
			base_address = (DWORD)modEntry.modBaseAddr;
			break;
		}
	}
	
	// Clean up snapshot object
	
	CloseHandle(hSnapshot);
	
	return base_address;
}


/* Returns image base address of suspended process */

DWORD GetBaseAddress(HWND hwnd, HANDLE hProc ,HANDLE hThread){
	
	DWORD baseAddr = 0;
	CONTEXT ctx;
	
	ctx.ContextFlags = CONTEXT_INTEGER;
	
	if(!GetThreadContext(hThread,&ctx))
	{
		MessageBox(hwnd,"Failed to get thread context!","Error",MB_OK | MB_ICONERROR);
		return 0;
	}
	
	
	if(!ReadProcessMemory(hProc,(char*)(ctx.Ebx + 8),&baseAddr,sizeof(char*),NULL))
	{
		MessageBox(hwnd,"Failed to get base address!\nFailed to read process memory!","Error",MB_OK | MB_ICONERROR);
		return 0;
	}
	
	
	return baseAddr;
}

/* Searches for a specified unmasked byte pattern in a process */

DWORD seekbytes (HANDLE hProc, unsigned char pattern[] , DWORD base_addr , DWORD end_addr, SIZE_T numOfBytes){
	
	unsigned char buffer[4096];
	SIZE_T bytesRead = 0;
	DWORD block = base_addr;
	int i;
	
	while(block <= end_addr)
	{
		
		if(ReadProcessMemory(hProc,(void*)block,buffer,4096,&bytesRead) == 0){
			MessageBox(NULL,"Cannot read process memory!\nAborting...","Error",MB_OK | MB_ICONEXCLAMATION);
			break;
		}
		
		if(bytesRead == 0)
			return 0;
		
		for( i = 0 ; i < bytesRead; i++){
		
			if(memcmp(pattern,buffer+i,numOfBytes) == 0)
				return block + i;
		}
		
		block = block + bytesRead;
	}
	
	return 0;
}

IMAGE_NT_HEADERS *getPEHeader(HANDLE hProcess, DWORD baseAddr){
	
	IMAGE_DOS_HEADER DosHead;
	static IMAGE_NT_HEADERS hPE;
	BOOL rVal;
	BOOL rVal2;
	
	rVal = ReadProcessMemory(hProcess,(char*)baseAddr,&DosHead,sizeof(IMAGE_DOS_HEADER),NULL);
	rVal2 = ReadProcessMemory(hProcess,(char*)baseAddr + DosHead.e_lfanew,&hPE,sizeof(IMAGE_NT_HEADERS),NULL);
	
	if(rVal == 0 || rVal2 == 0)
		return NULL;
	
	
	return &hPE;
		
}
