
LRESULT CALLBACK globalSettingsToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
    {
        case WM_INITDIALOG:
			SendDlgItemMessage(hwnd,TR_GBLTRANSPOSE,TBM_SETRANGE,0,(LPARAM) MAKELONG(0,255));
			SendDlgItemMessage(hwnd,TR_GBLVELOCITY,TBM_SETRANGE,0,(LPARAM) MAKELONG(0,255));
			SendDlgItemMessage(hwnd,TR_GBLMODULATION,TBM_SETRANGE,0,(LPARAM) MAKELONG(0,255));
			SendDlgItemMessage(hwnd,TR_GBLPITCHBEND,TBM_SETRANGE,0,(LPARAM) MAKELONG(0,255));

			setGlobalTranspose(hwnd,0);
			setGlobalVelocity(hwnd,100);
			setGlobalModulation(hwnd,0);
			setGlobalPitchBend(hwnd,0);

			return -1;
        case WM_CLOSE:
			EndDialog(hwnd,0);
            break;
        case WM_SIZE:
            break;
		case WM_SETFOCUS:
			ShowWindow(hwnd,SW_HIDE);
			SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
			ShowWindow(hwnd,SW_SHOW);
			return 0;
		case WM_KILLFOCUS:
			SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
			return 0;

		default:
			return 0;
	}
return 0;
}

LRESULT CALLBACK channelSettingsToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
    {
        case WM_INITDIALOG:
			SendDlgItemMessage(hwnd,BN_PAGE1,BM_SETSTATE,true,0);
			SendDlgItemMessage(hwnd,BN_PAGE2,BM_SETSTATE,true,0);

			SetMenu(hwnd,LoadMenu(hInstance,(LPSTR) SSMENU));
			return -1;
		case WM_SETFOCUS:
			ShowWindow(hwnd,SW_HIDE);
			SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
			ShowWindow(hwnd,SW_SHOW);
			return 0;
		case WM_KILLFOCUS:
			SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
			return 0;
        case WM_CLOSE:
			EndDialog(hwnd,0);
            break;
        case WM_SIZE:
            break;
		default:
			return 0;
	}
return 0;
}

LRESULT CALLBACK splitToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int count;
	switch (msg)
    {
        case WM_INITDIALOG:
			for (count=0;count<MAX_CHANNELS;count++)
			{
				char thisStr[16];
				if (count < 5)
				{
					sprintf((char *)&thisStr,"Split %d",count);
					SendDlgItemMessage(hwnd,CB_SPLITPAGE,CB_ADDSTRING,0,(LPARAM) &thisStr);
				}
				sprintf((char *)&thisStr,"Channel %d",count);
				SendDlgItemMessage(hwnd,CB_CHANNEL2,CB_ADDSTRING,0,(LPARAM) "All Channels");
				SendDlgItemMessage(hwnd,CB_CHANNEL,CB_ADDSTRING,0,(LPARAM) &thisStr);
				SendDlgItemMessage(hwnd,CB_CHANNEL2,CB_ADDSTRING,0,(LPARAM) &thisStr);
			}
			SendDlgItemMessage(hwnd,CB_SPLITPAGE,CB_SETCURSEL,0,(LPARAM) curSplitPage);
			return -1;

        case WM_CLOSE:
			EndDialog(hwnd,0);
            break;
        case WM_SIZE:
            break;
		default:
			return 0;
	}
return 0;
}

#include "seqTool.hpp"