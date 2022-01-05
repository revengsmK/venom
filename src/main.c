#include <windows.h>

#include "venom.h"
#include "resource.h"
#include "ufmod.h"

#define TIMER_ID 100
#define TIMER_ID2 200


// Prototypes

INT_PTR CALLBACK mainDlgProc(HWND hwndDlg,UINT msg , WPARAM wParam , LPARAM lParam);
INT_PTR CALLBACK donateDlgProc(HWND hwndDlgDonate, UINT message, WPARAM wParam,LPARAM lParam);

// Global variables

HBRUSH gray_brush;
RECT rect;
LONG originalPos;
BOOL test = 0;
HDC hdc;
SIZE size;
HICON hIcon;
int scroll_x = 390;
int scroll_length = 0;

char *scroll_text = "Blumentals Program Protector v4.x  *Toolkit*"
	   "       Coded by:  Aleksandar 'systematiK'       GFX by: Aleksandar 'systematiK'"
       "       Music by: clawz       Coded in C \\ Win32 API" 
	   "       Tested on program versions : 4.8 , 4.11 , 4.12 , 4.13"
       "       This toolkit uses uFMOD to play this tune :)"
	   "       Visit: https://github.com/revengsmK for more projects ;)";
	   
	   
INT_PTR CALLBACK mainDlgProc(HWND hwndDlg,UINT msg , WPARAM wParam , LPARAM lParam){
	
	switch(msg){
		
		case WM_INITDIALOG:{
			
			hIcon = LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,
										GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON),0);
			
			if(hIcon != NULL)
					SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			
			patchCPHook(hwndDlg);
			
			hdc = GetDC(hwndDlg);
			
			GetWindowRect(hwndDlg,&rect);
			
			SetBkColor(hdc,RGB(38,37,37));
			SetTextColor(hdc,RGB(18, 172, 90));
			
			originalPos = rect.bottom - rect.top;
			gray_brush = CreateSolidBrush(RGB(38, 37, 37));
			
			scroll_length = lstrlen(scroll_text);
			
			GetTextExtentPoint32A(hdc,scroll_text,scroll_length,&size);
			
		}
		break;
				
		case WM_CTLCOLORDLG:
		
		return (LONG)gray_brush;
		
		
		case WM_CTLCOLORSTATIC:
		{
			HDC hdc = (HDC)wParam;
			SetBkMode(hdc,TRANSPARENT);
			
			SetTextColor(hdc,RGB(255,255,255));
			return (LONG)gray_brush;
		}
		
		case WM_CTLCOLORBTN:
		
		return (LONG)gray_brush;

		
		case WM_COMMAND:
			switch(LOWORD(wParam)){
				
				case IDC_GETPASS:
					getPassword(hwndDlg);
				break;
				
				case IDC_RUNPROGRAM:
					runProtectedProgram(hwndDlg);
				break;
				
				case IDC_UNPROTECT:
					unprotectPrograms(hwndDlg);
				break;
				
				case IDC_COPY:{	
					SendDlgItemMessage(hwndDlg,IDC_TXTPASS,EM_SETSEL,0,-1);
					SendDlgItemMessage(hwndDlg,IDC_TXTPASS,WM_COPY,0,0);
				}
				break;
				
				case IDC_ABT:
					SetTimer(hwndDlg,TIMER_ID,19,NULL);
				break;
				
				case IDC_DONATE:
					DialogBoxParam(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_DIALOG2),hwndDlg,donateDlgProc,0);
				break;
				
			}
			
			break;
			
			//  *very* simple window extending using timer
			case WM_TIMER:
			
				switch(wParam){
					
					case TIMER_ID:
					{
						
					GetWindowRect(hwndDlg,&rect);
				
					if((rect.bottom - rect.top) > 249){
						test = 1;
						SetTimer(hwndDlg,TIMER_ID2,15,NULL);
						
						KillTimer(hwndDlg,TIMER_ID);
						uFMOD_PlaySong((void*)IDR_RCDATA1,0,XM_RESOURCE);
					}
					
					else if((rect.bottom - rect.top) < originalPos){
						test = 0;
						KillTimer(hwndDlg,TIMER_ID);
						KillTimer(hwndDlg,TIMER_ID2);
						uFMOD_StopSong();
						scroll_x = 390;
						}
						
					if(test == 0)
						MoveWindow(hwndDlg,rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top + 1,TRUE);
					else
						MoveWindow(hwndDlg,rect.left,rect.top,rect.right - rect.left,rect.bottom - rect.top - 1,TRUE);
						
		
					return 0;
					}
					
					// *very* simple scroller here
					case TIMER_ID2:{
						
						// move scroll to the left
						scroll_x--;
						
						// print out text
						TextOut(hdc,scroll_x,195,scroll_text,scroll_length);
						
						// reset scroll to initial position
						if( scroll_x < -size.cx-50)
								scroll_x  = 390;
						
					return 0;
					}
					
				}
			break;
		
		case WM_CLOSE:{
			
			free(memblock);
			DeleteObject(gray_brush);
			ReleaseDC(hwndDlg,hdc);
			DestroyIcon(hIcon);
			EndDialog(hwndDlg,0);
			
		}
		break;
		
		
		default:
		 return FALSE;
	}
	return TRUE;
	
}

INT_PTR CALLBACK donateDlgProc(HWND hwndDlgDonate, UINT message, WPARAM wParam,LPARAM lParam){
	
	switch(message){
		
		case WM_INITDIALOG:
				SetDlgItemText(hwndDlgDonate,IDC_TXTBTCADDR,"bc1qjwedefp3xruv7qak72l89hzx5mx0c0ftq8qem7");
		break;
			
		case WM_COMMAND:{
		
			switch(LOWORD(wParam)){
				
				case IDC_X:
					EndDialog(hwndDlgDonate,0);
				break;
				
				case IDC_COPYADDR:
					SendDlgItemMessage(hwndDlgDonate,IDC_TXTBTCADDR,EM_SETSEL,0,-1);
					SendDlgItemMessage(hwndDlgDonate,IDC_TXTBTCADDR,WM_COPY,0,0);
					
				break;
			}
			
		}
		break;
		
		case WM_CTLCOLORBTN:
		
		return (LONG)gray_brush;
		
		case WM_CTLCOLORDLG:
			return (LONG)gray_brush;
		
		
		case WM_CTLCOLORSTATIC:{
			HDC hdc = (HDC)wParam;
			SetBkMode(hdc,TRANSPARENT);

			SetTextColor(hdc,RGB(210,210,210));
			return (LONG)gray_brush;
		}
		
		
		default:
			return FALSE;
	}
	
	return TRUE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nCmdShow){
	
	return DialogBoxParamW(hInstance,MAKEINTRESOURCEW(IDD_DIALOG1),NULL,mainDlgProc,0);
	
}