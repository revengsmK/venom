#ifndef VENOM_H
#define VENOM_H

void int2uchar(unsigned char *buf);
BOOL openFileDialog(HWND hDlg, char szTitle[]);
void patchCPHook(HWND hDlg);
void getPassword(HWND hDlg);
void runProtectedProgram(HWND hDlg);
void unprotectPrograms(HWND hDlg);
void cleanup(HANDLE hThread, HANDLE hProcess);

extern int* memblock;

#endif /*VENOM_H*/