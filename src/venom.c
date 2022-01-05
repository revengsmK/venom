#include <windows.h>

#include "venom.h"
#include "vnmfunc.h"
#include "resource.h"

// Globals

char szFileName[MAX_PATH] = "";
int *memblock = NULL;


void int2uchar(unsigned char *buf){
    
	int address = (int)memblock;
	int i;
    
    for(i = 3 ; i >= 0; i--){
        buf[i] = address & 0xFF;
        address>>= 8;
    }
}


BOOL openFileDialog(HWND hDlg, char szTitle[]){
	
	OPENFILENAME ofn;

	// zero ofn members
	ZeroMemory(&ofn,sizeof(ofn));
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "Executable Files\0*.exe\0\0";
	ofn.lpstrTitle = szTitle;
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	
	ofn.lpstrDefExt = "exe";
	
	if(GetOpenFileName(&ofn) != 0)
		return TRUE;
	
	
	return FALSE;
	
}

void patchCPHook(HWND hDlg){
	
	unsigned char pattern[] = { 0x8D, 0x4D ,0xE4 ,0xC7 ,0x45 ,0xE0 ,0x00 ,0x00 ,0x00 ,0x00};  //  lea ecx, dword ptr ss:[ebp-1c]  mov dword ptr ss:[ebp-20],0
	
	unsigned char cave_address [] = { 0xE9, 0xB8 ,0x8E ,0x00 ,0x00 ,0x90 ,0x90 ,0x90 ,0x90 ,0x90 };
	
    unsigned char cave_bytes[] = {0x8B,0xF0,0x8D,0x3D,0x90,0x15,0x02,0x00,0x66,0xA5,0x66,0x83,0x3E,0x00,0x0F,
													0x85,0xF4,0xFF,0xFF,0xFF,0x8D,0x4D,0xE4,0xC7,0x45,0xE0,0x00,0x00,0x00,0x00,0xE9,0x2A,0x71,0xFF,0xFF};	
													
	unsigned char pattern2[] = { 0x83,0xC4,0x08,0x85,0xC0,0x74,0x45 }; // add esp,8	test eax,eax	je [address]
	unsigned char patch_bytes2[] = { 0x83,0xC4,0x08,0x85,0xC0,0xEB,0x45 };
	
	unsigned char mem_bytes[4];
	
	unsigned int PID = 0;
	
	HANDLE hProc;
	IMAGE_NT_HEADERS *hPE;
	DWORD base_address;
	DWORD end_address; 
	DWORD address; 
	
	// allocate and zero out new memory block to store password
	memblock = (int*)malloc(50);
	
	if(memblock == NULL){
		MessageBox(hDlg,"Failed to allocate memory block!","Error",MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	ZeroMemory((int*)memblock,50);
	
	// convert allocated memory block pointer to array of unsigned chars
    int2uchar(mem_bytes);
	
   
	// these 4 statements form dynamic memory address returned by malloc()
	cave_bytes[4] = mem_bytes[3];
	cave_bytes[5] = mem_bytes[2];
	cave_bytes[6] = mem_bytes[1];
	cave_bytes[7] = mem_bytes[0];
    
	
	PID = GetCurrentProcessId();
		
	hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE,FALSE,PID);

		if(!hProc){
			MessageBox(hDlg,"Failed to open process!","Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		
		base_address = GetDLLBaseAddress("CPHook32.dll",PID);
	
		if(base_address == 0){
			MessageBox(hDlg,"CPHook32.dll module NOT loaded!","Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		}


	hPE = getPEHeader(hProc,base_address);

	end_address = base_address + hPE->OptionalHeader.SizeOfCode;
	
	address = seekbytes(hProc,pattern,base_address,end_address,10);
	

	if(WriteProcessMemory(hProc,(char*)address,cave_address,10,NULL) == 0){
		  MessageBox(hDlg,"Failed to write to process memory!","Error",MB_OK | MB_ICONEXCLAMATION);
		  return;
	}
		
		// set memory address to free space and write cave_bytes
		
		address = address + 0x8EBD;
		
		if(WriteProcessMemory(hProc,(char*)address,cave_bytes,35,NULL) == 0){
			MessageBox(hDlg,"Failed to write to process memory!","Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		
		
		address = seekbytes(hProc,pattern2,base_address,end_address,7);
		
		if(WriteProcessMemory(hProc,(char*)address,patch_bytes2,7,NULL) == 0){
			MessageBox(hDlg,"Failed to write to process memory!","Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		}
	
}

void getPassword(HWND hDlg){
	
	if(openFileDialog(hDlg,"Select protected program to get password :)") != FALSE){
		
		ShellExecute(hDlg,"open",szFileName,"",NULL,SW_SHOW);

		SetDlgItemTextW(hDlg,IDC_TXTPASS,(LPCWSTR)memblock);
		ZeroMemory((int*)memblock,50);
	}
}


void runProtectedProgram(HWND hDlg){
	
	if(openFileDialog(hDlg,"Select protected program to run :)") != FALSE)
		
		if((int)ShellExecute(hDlg,"open",szFileName,"",NULL,SW_SHOW) <= 32)
			MessageBox(hDlg,"Failed to run protected program :(","Error",MB_OK | MB_ICONEXCLAMATION);
	
}

void unprotectPrograms(HWND hDlg){
	
	
	unsigned char pattern1[] = { 0x66, 0x72 ,0x6D ,0x55 ,0x6E ,0x70 ,0x72 ,0x6F ,0x74 ,0x65 ,0x63 ,0x74 ,0x00 };
	unsigned char pattern2[] = { 0x6A,0xFF,0x68,0x98,0x3A,0x00,0x00,0x68,0x90,0x01,0x00,0x00,0x56 }; // push -1 push 0x3a98 push 0x190 push esi
	unsigned char admin_pattern[] = {0x83,0x38,0x00};
	
	unsigned char admin_patch[] = {0x90,0xE9};
	unsigned char mutex_patch[] = {0x00,0x00};
	
	unsigned char patch1 [] = {0x74,0x00};
	unsigned char patch2 [] = {0xEB,0x43};
	
	DWORD oep;
	DWORD addr;
	
	DWORD baseAddr = 0;
	DWORD endAddr = 0;
	
	STARTUPINFO startInfo;
	PROCESS_INFORMATION procInfo;
	IMAGE_NT_HEADERS *hPE;


	if(	openFileDialog(hDlg,"Select Program Protector EXE file (protect.exe) :)") != FALSE){
	
	GetStartupInfo(&startInfo);

	if(!CreateProcess(szFileName,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startInfo,&procInfo))
	{
		MessageBox(hDlg,"Cannot create Program Protector process!","Error",MB_OK | MB_ICONERROR);
		return;
	}

	
	 baseAddr = GetBaseAddress(hDlg,procInfo.hProcess,procInfo.hThread);
	
	 if(baseAddr == 0)
	 {
	 	MessageBox(hDlg,"Failed to get image base address!","Error",MB_OK | MB_ICONERROR);
		cleanup(procInfo.hThread, procInfo.hProcess);
	 	return;
	 }
	
	
	 hPE = getPEHeader(procInfo.hProcess,baseAddr);
	 
	 oep = baseAddr + hPE->OptionalHeader.AddressOfEntryPoint;

	if(oep != 0){
		
		// bypass CreateMutex() check for existing instance in memory
		
		if(WriteProcessMemory(procInfo.hProcess,(char*)oep+0x44,mutex_patch,2,NULL) == 0)
		{
			cleanup(procInfo.hThread, procInfo.hProcess);
			MessageBox(hDlg,"Failed to write to process!","Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		
		addr = seekbytes(procInfo.hProcess,admin_pattern,oep,oep+0x1000,3);

		// bypass administrative password
		
		if(WriteProcessMemory(procInfo.hProcess,(char*)addr+3,admin_patch,2,NULL) == 0)
		{
			cleanup(procInfo.hThread, procInfo.hProcess);
			MessageBox(hDlg,"Failed to write to process!","Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		
		
		endAddr = baseAddr + hPE->OptionalHeader.SizeOfCode;

		addr = seekbytes(procInfo.hProcess,pattern2,baseAddr,endAddr,13);
		addr +=0x23;
		
		if(WriteProcessMemory(procInfo.hProcess,(char*)addr,patch2,2,NULL) == 0){
			cleanup(procInfo.hThread, procInfo.hProcess);
			MessageBox(hDlg,"Failed to write to process!","Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		}
		
		addr = seekbytes(procInfo.hProcess,pattern1,addr,endAddr,13);
		addr += 0x42;

		if(WriteProcessMemory(procInfo.hProcess,(char*)addr,patch1,2,NULL) == 0)
		{
			cleanup(procInfo.hThread, procInfo.hProcess);
			MessageBox(hDlg,"Failed to write to process!","Error",MB_OK | MB_ICONEXCLAMATION);
			return;
		}
			
		MessageBox(hDlg,"Process patching done!\nUnprotect any program from the list without entering password.","inf0",MB_OK | MB_ICONINFORMATION);
		
		ResumeThread(procInfo.hThread);
		cleanup(procInfo.hThread, procInfo.hProcess);
	
	}
	else
	{
		cleanup(procInfo.hThread, procInfo.hProcess);
		MessageBox(hDlg,"Failed to get process OEP!","Error",MB_OK | MB_ICONEXCLAMATION);
	}
	
 }

}


void cleanup(HANDLE hThread, HANDLE hProcess){
	
	CloseHandle(hThread);
	CloseHandle(hProcess);
	
}

