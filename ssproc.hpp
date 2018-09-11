void	setCurChannelPage(int page)
{
}
void setCurChannel(int chan)
{
	//WM_GET
}

int	resetChannel(int nChannel)
{
	for (int count=0;count<128;count++)
		hKi[nChannel].keyDown[count]=0;

	RECT	rect;

	GetWindowRect(hKi[nChannel].hwnd,&rect);

	rect.bottom=rect.bottom-rect.top;
	rect.right=rect.right-rect.left;
	rect.top=0;
	rect.left=0;

	InvalidateRect(hKi[nChannel].hwnd,&rect,true);

	return 0;
}

int resetAllChannels()
{
	for (int count=0;count< MAX_CHANNELS;count++)
	{
		if (hKi[count].hwnd)
			resetChannel(count);
	}
	return 0;
}

void	scrollEvent(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	int newPos = SendMessage((HWND) lParam,TBM_GETPOS,0,0);
	
	if ((HWND) lParam == GetDlgItem(hwnd,TR_TRANSPOSE))
		setTranspose(hwnd,newPos-128);
	else if ((HWND) lParam == GetDlgItem(hwnd,TR_VELOCITY))
		setVelocity(hwnd,newPos-128);
	else if ( (HWND) lParam == GetDlgItem(hwnd,TR_MODULATION))
		setModulation(hwnd,newPos-128);
	else if ( (HWND) lParam == GetDlgItem(hwnd,TR_PITCHBEND))
		setPitchBend(hwnd,newPos-128);
	else if ( (HWND) lParam == GetDlgItem(hwnd,TR_SPLITSTART))
		setSplitKey(hwnd,newPos);

	else if ( (HWND) lParam == GetDlgItem(hwnd,TR_GBLTRANSPOSE))
		setGlobalTranspose(hwnd,newPos-128);
	else if ( (HWND) lParam == GetDlgItem(hwnd,TR_GBLVELOCITY))
		setGlobalVelocity(hwnd,newPos-128);
	else if ( (HWND) lParam == GetDlgItem(hwnd,TR_GBLMODULATION))
		setGlobalModulation(hwnd,newPos-128);
	else if ( (HWND) lParam == GetDlgItem(hwnd,TR_GBLPITCHBEND))
		setGlobalPitchBend(hwnd,newPos-128);
}

void	splitPageChange(HWND hwnd)
{
	curSplitPage=SendDlgItemMessage(hwnd,CB_SPLITPAGE,CB_GETCURSEL,0,0);
	setTranspose(hwnd,split[curSplitPage].Transpose);
	setVelocity(hwnd,split[curSplitPage].Velocity);
	setModulation(hwnd,split[curSplitPage].Modulation);
	setPitchBend(hwnd,split[curSplitPage].PitchBend);
	setSplitKey(hwnd,split[curSplitPage].Key);
	setChannel(hwnd,split[curSplitPage].Channel);
}

void mainResize(HWND hwnd)
{
RECT	rect;
RECT	crect;
	return;
	GetWindowRect(hwnd,&rect);
	for (int c=0;c<MAX_CHANNELS;c++)
	{
		if (hKi[c].hwnd )
		{
			GetWindowRect(hKi[c].hwnd,&crect);
			int xdif=rect.left-mainRect.left;
			int ydif=rect.top-mainRect.top;

			int winx = crect.right-crect.left;
			int winy = crect.bottom-crect.top;
			crect.left+=xdif;
			crect.top+=ydif;
			MoveWindow(hKi[c].hwnd,crect.left,crect.top,winx,winy,true);
		}
	}

	memcpy((void *) &mainRect,(void *) &rect,sizeof(rect));
}

LRESULT CALLBACK ssProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
    {
		case WM_CREATE:
        case WM_INITDIALOG:
		{
			WNDCLASSEX	wc;
			int	count;

//			TransposeQueue();
			
			memset((char *) &wc,0,sizeof(WNDCLASSEX));

//			kKeyInputWidth=15;
//			kKeyInputHeight=80;
			
//			hKeysBmp = LoadBitmap(hInstance,(LPSTR) IDB_KEYS15);
			RedBrush=CreatePatternBrush(LoadBitmap(hInstance,(LPSTR) IDB_RED));

			hKiBrush[0]=CreatePatternBrush(LoadBitmap(hInstance,(LPSTR) IDB_KEYS9));
			hKiBrush[1]=CreatePatternBrush(LoadBitmap(hInstance,(LPSTR) IDB_KEYS15));
			hKiBrush[2]=CreatePatternBrush(LoadBitmap(hInstance,(LPSTR) IDB_KEYS20));
			hKiBrush[3]=CreatePatternBrush(LoadBitmap(hInstance,(LPSTR) IDB_KEYS30));
			hKiBrush[4]=CreatePatternBrush(LoadBitmap(hInstance,(LPSTR) IDB_KEYS45));

			wc.cbSize=sizeof(WNDCLASSEX);
			wc.hbrBackground=CreateSolidBrush(0xFFFFFF);
			wc.hCursor=LoadCursor(hInstance,IDC_ARROW);
			wc.hIcon=LoadIcon(hInstance,IDI_APPLICATION);
			wc.hIconSm=LoadIcon(hInstance,IDI_APPLICATION);
			wc.hInstance=hInstance;
			wc.lpfnWndProc=(WNDPROC) bnKeyInputProc;
			wc.lpszClassName=CLS_KEYINPUT;
			wc.lpszMenuName=0;
			wc.style=CS_VREDRAW | CS_HREDRAW;

//			int exStyle=GetWindowLong(hwnd,GWL_EXSTYLE);
//			exStyle=exStyle | WS_EX_TOPMOST;
//			SetWindowLong(hwnd,GWL_EXSTYLE,exStyle);

			hKiMenu=GetSubMenu(LoadMenu(hInstance,(LPSTR) KIMENU),0);

			if (!RegisterClassEx(&wc))
			{
				MessageBox(hwnd,"Failed to initialize",PROGRAM_NAME,MB_OK | MB_ICONERROR);
				EndDialog(hwnd,0);
				return 0;
			}
			makeKiWindow(hwnd);

			{
				for (count=0;count < 10;count++)
				{
					split[count].Channel=count;
					split[count].Instrument=0;
					split[count].Key=60-(count*12);
					split[count].Modulation=0;
					split[count].PitchBend=0;
					split[count].Transpose=0;
					split[count].Velocity=96;
				}
			}
			
			{
				for (int count=0;count<MAX_CHANNELS;count++)
				{	
					bPlayChannel[count]=true;
				}
			}

			hSSMenu=LoadMenu(hInstance,(LPSTR) SSMENU);
			SetMenu(hwnd,hSSMenu);

			SendDlgItemMessage(hwnd,TR_TRANSPOSE,TBM_SETRANGE,0,(LPARAM) MAKELONG(0,255));
			SendDlgItemMessage(hwnd,TR_VELOCITY,TBM_SETRANGE,0,(LPARAM) MAKELONG(0,255));
			SendDlgItemMessage(hwnd,TR_MODULATION,TBM_SETRANGE,0,(LPARAM) MAKELONG(0,255));
			SendDlgItemMessage(hwnd,TR_PITCHBEND,TBM_SETRANGE,0,(LPARAM) MAKELONG(0,255));
			SendDlgItemMessage(hwnd,TR_SPLITSTART,TBM_SETRANGE,0,(LPARAM) MAKELONG(0,127));


			SendDlgItemMessage(hwnd,CB_CHANNEL,CB_SETCURSEL,0,(LPARAM) split[curSplitPage].Channel);
			SendDlgItemMessage(hwnd,CB_CHANNEL2,CB_SETCURSEL,0,0);
			splitPageChange(hwnd);
			lastMouse.NotePos=-1;

			count=0;
			while (*Instruments[count] != 0)
			{
				SendDlgItemMessage(hwnd,CB_INSTRUMENT,CB_ADDSTRING,0,(LPARAM) Instruments[count]);
				count++;
			}
			SendDlgItemMessage(hwnd,CB_INSTRUMENT,CB_SETCURSEL,0,0);
			
			hwndMain	=	hwnd;

//			CreateDialogParam(hInstance,(LPSTR) GLOBALSETTINGSDLG,hwnd,(DLGPROC)globalSettingsToolProc,0);
//			CreateDialogParam(hInstance,(LPSTR) CHANNELSETTINGSDLG,hwnd,(DLGPROC) channelSettingsToolProc,0);
			CreateDialogParam(hInstance,(LPSTR) MIDIDEVICEDLG,hwnd,(DLGPROC) midiToolProc,0);
			CreateDialogParam(hInstance,(LPSTR) SEQDLG,hwnd,(DLGPROC) seqToolProc,0);
//			CreateDialogParam(hInstance,(LPSTR) SPLITDLG,hwnd,(DLGPROC) splitToolProc,0);
		}
		return 0;
        case WM_CLOSE:
			DestroyWindow(hwnd);
            break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
        case WM_SIZE:
			mainResize(hwnd);
//			return 1;
            break;
		case WM_PAINT:
		{
		PAINTSTRUCT psPaint;
		RECT		rect,rect2;

			HDC	tHDC = BeginPaint(hwnd,&psPaint);
			GetClientRect(hwnd,&rect);
			memcpy((char *) &rect2,(char *) &rect,sizeof(RECT));
			rect.bottom=300;

			HBRUSH	hbr;
			POINT pt;
			hbr=RedBrush;
			FillRect(tHDC,&rect,hbr);

			rect.top=400;
			rect.bottom=400+150;
			FillRect(tHDC,&rect,hbr);
//			DeleteObject(hbr);
			EndPaint(hwnd,&psPaint);
		}
		return 0;
		break;
	case WM_MOVE:
			mainResize(hwnd);
			break;
        case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case 0:
					switch (LOWORD(wParam))
					{
						case ID_OPENFILE:
							break;
						case CH_PLAYCHANNEL:
							setPlayChannel(hwnd);
							break;
						case CH_SHOWCHANNELKEYS:
							setShowChannelKeys(hwnd);
							break;
						case ID_FILE_EXIT:
							DestroyWindow(hwnd);
							break;
						case ID_CHANNEL_NEWWINDOW:
							makeKiWindow(hwnd);
							break;
						case ID_OPTIONS_ALWAYSONTOP:
							bAlwaysOnTop = !bAlwaysOnTop;
							int	wStyle=GetWindowLong(hwnd,GWL_EXSTYLE);
							if (!bAlwaysOnTop)
							{
								wStyle= wStyle & (~(WS_EX_TOPMOST));
								CheckMenuItem(GetMenu(hwnd),ID_OPTIONS_ALWAYSONTOP,MF_BYCOMMAND | MF_UNCHECKED);
								SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
							}
							else
							{
								wStyle= wStyle | WS_EX_TOPMOST;
								CheckMenuItem(GetMenu(hwnd),ID_OPTIONS_ALWAYSONTOP,MF_BYCOMMAND | MF_CHECKED);
								SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
							}
							break;
					}
					return 0;
				case CBN_SELCHANGE:
					switch (LOWORD(wParam))
					{
						case CB_SPLITPAGE:
							splitPageChange(hwnd);
							break;
						case CB_CHANNEL:
							setChannel(hwnd,SendMessage((HWND) lParam,CB_GETCURSEL,0,0));
							break;
						case CB_CHANNEL2:
							setChannelPresets(hwnd);
							break;
						case CB_INSTRUMENT:
							setInstrument(hwnd);
							break;
					}
			}
            break;
		case WM_KEYDOWN:
			keyPress(hwnd,WM_KEYDOWN,wParam,lParam);
			break;
		case WM_KEYUP:
			keyPress(hwnd,WM_KEYUP,wParam,lParam);
			break;
		case WM_CHAR:
			break;
		case WM_HSCROLL:
			scrollEvent(hwnd,wParam,lParam);
			break;
		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
		return DefWindowProc(hwnd,msg,wParam,lParam);
}