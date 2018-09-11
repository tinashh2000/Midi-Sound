#define PROGRAM_NAME    "MT-VPiano"

#include "Include/ss.h"
#include "midi.hpp"
#include "set.hpp"
#include "keyWin.hpp"
#include "tools.hpp"
#include "toolsproc.hpp"
#include "ssProc.hpp"


int WINAPI WinMain(HINSTANCE hhInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd)
{
//	hRichEdit = LoadLibrary("RICHED32.DLL");
    InitCommonControls();

	hInstance = hhInstance;
	WNDCLASSEX	wc;
	memset(&wc,0,sizeof(wc));
	wc.cbSize=sizeof(WNDCLASSEX);
	wc.hbrBackground=(HBRUSH) COLOR_BTNFACE;
	wc.hInstance=hInstance;
	wc.lpszClassName="MTSOUNDSTUDIOCLASS";
	wc.hCursor=LoadCursor(hInstance,IDC_ARROW);
	wc.hIconSm=LoadIcon(hInstance,(LPSTR) IDI_MAIN);
	wc.lpfnWndProc=&ssProc;
	wc.lpszMenuName=(LPSTR) SSMENU;

	RegisterClassEx(&wc);

	hwndMain = CreateWindowEx(WS_EX_CLIENTEDGE,"MTSOUNDSTUDIOCLASS","MT-SoundStudio",WS_VISIBLE | WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,500,400,NULL,LoadMenu(hInstance,(LPSTR) SSMENU),hInstance,NULL);

	ShowWindow(hwndMain,SW_SHOW);
	while (hwndMain)
	{
		MSG msg;
		int retCode=GetMessage(&msg,0,0,0);
		if (retCode == -1 || retCode == 0)
			break;

//		TranslateMDISysAccel(hMDIClient,&msg);
//		TranslateMDISysAccel(hSSWnd,&msg);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

//	WSACleanup();
//	FreeLibrary(hRichEdit);
    ExitProcess(0);
	return 0;
	
}

int main()
{
	return ( WinMain(GetModuleHandle(0),0,GetCommandLine(),SW_SHOW) );
}
