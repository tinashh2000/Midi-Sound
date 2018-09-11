LRESULT CALLBACK keyWinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

int	getWindowOwnerIndex(HWND hwnd)
{
	for (int count=0;count<MAX_CHANNELS;count++)
	{
		if (hKi[count].hwnd == hwnd)
			return count;
	}
	return -1;
}

DWORD keyInputWndThread(LPVOID param)
{
MSG msg;
CHANNEL	*hKi = (CHANNEL *) param;
HWND	hwnd=hKi->hwnd;
	while (GetMessage(&msg,hwnd,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

HWND	makeKiWindow(HWND hwnd)
{
int count;
RECT	rc;
	for (count=0;count<MAX_CHANNELS;count++)
	{
		if (hKi[count].hwnd==0)
			break;
	}

	if (count <MAX_CHANNELS)
	{
		if (hKi[count].hwnd==0)
		{
			hKi[count].bShowKeyboard=true;
			for (int count2=0;count2>=MAX_CHANNELS;count2++)
				hKi[count].bShowMidiInput[count2]=0;

			hKi[count].bShowMidiInput[count]=true;

			hKi[count].kKeyHeight=80;
			hKi[count].kKeyWidth=15;

			GetClientRect(hwnd,&rc);
			int mX=rc.right-rc.left;
			int mY=rc.bottom-rc.top;

//			hKi[count].hwnd=CreateWindowEx(WS_EX_CONTROLPARENT ,CLS_KEYINPUT,"Window",WS_THICKFRAME | WS_VISIBLE | WS_POPUP | WS_CHILD,10,mY-hKi[count].kKeyHeight-10,mX-10,hKi[count].kKeyHeight+10,hwnd,0,hInstance,0);
			hKi[count].hwnd = CreateDialogParam(hInstance,(LPSTR) KEYWINDLG,hwnd,(DLGPROC) keyWinProc,0);
			
			ShowWindow(hKi[count].hwnd,SW_SHOW);

			if (!CreateThread(0,0,(LPTHREAD_START_ROUTINE) &keyInputWndThread,(LPVOID) &hKi[count],0,&threadID))
			{
				MessageBox(hwnd,"Failed to initialize",PROGRAM_NAME,MB_OK | MB_ICONERROR);
				EndDialog(hwnd,0);
			}

			if (hKi[count].hwnd)
			{
				char winStr[16];
				sprintf((char *) winStr,"View %d",count);
				SetWindowText(hKi[count].hwnd,(char *) &winStr);
			}
			return hKi[count].hwnd;
		}
	}
}

int	findKey(int keyCode,char *keyMap)
{
int Split=0,SplitRemove=0;

char *kMap = keyMap;

	for (int count=0;((count < 512) || *kMap == 0);count++,kMap++)
	{
		if (*kMap == (char) keyCode)
		{
			return (Split << 16) + (count-SplitRemove);
		}

		if (*kMap == VP_SPLIT)
		{
			Split++;
			SplitRemove=count+1;
		}
	}

	return -1;
}

int	keyPress(HWND hwnd,int msg,WPARAM wParam,LPARAM lParam)
{
	int bShiftState=GetKeyState(VK_SHIFT);
	int bCtrlState=GetKeyState(VK_CONTROL);
	int bAltState=GetKeyState(VK_MENU);

	switch (wParam)
	{
		case VK_F5:
			if (msg == WM_KEYDOWN)
			{
				if (bCtrlState & 0x80)
					setTranspose(hwndMain,split[curSplitPage].Transpose-11);
				else
					setGlobalTranspose(hwndMain,global.Transpose-1);
			}
			break;
		case VK_F6:
			if (msg == WM_KEYDOWN)
			{
				if (bCtrlState & 0x80)
					setTranspose(hwndMain,split[curSplitPage].Transpose+1);
				else
					setGlobalTranspose(hwndMain,global.Transpose+1);
			}
			break;
		case VK_F7:
			if (msg == WM_KEYDOWN)
			{
				if (bCtrlState & 0x80)
					setVelocity(hwndMain,split[curSplitPage].Velocity-1);
				else
					setGlobalVelocity(hwndMain,global.Velocity-1);
			}
			break;
		case VK_F8:
			if (msg == WM_KEYDOWN)
			{
				if (bCtrlState & 0x80)
					setVelocity(hwndMain,split[curSplitPage].Velocity+1);
				else
					setGlobalVelocity(hwndMain,global.Velocity+1);
			}
			break;
		case VK_F11:
			if (msg == WM_KEYDOWN)
			{
				if (bCtrlState & 0x80)
					setPitchBend(hwndMain,split[curSplitPage].PitchBend-1);
				else
					setGlobalPitchBend(hwndMain,global.PitchBend-1);
			}
			break;
		case VK_F12:
			if (msg == WM_KEYDOWN)
			{
				if (bCtrlState & 0x80)
					setPitchBend(hwndMain,split[curSplitPage].PitchBend+1);
				else
					setGlobalPitchBend(hwndMain,global.PitchBend+1);
			}
			break;
	}

	int keyPos=findKey(wParam,(char *)&KeyMap);
	if (keyPos != -1)
	{		
		int	notePos		=	(keyPos) & 0xFF;
		int	noteSplit	=	(keyPos >> 16) & 0xFFFF;
		int	noteChannel	=	split[noteSplit].Channel;
		int	noteKey		=	split[noteSplit].Key;

		int	xNote		=	(noteKey+notePos);
		int xxNote		=	(noteKey+notePos) % 12;
		int	xOctave		=	(xNote/12);
		bool	blackKey = ((xxNote<=4 && (xxNote & 1)) || ((xxNote >4) && ((xxNote+1) & 1)));
		
		if (xxNote <= 4)
			xxNote=xxNote/2;
		else
			xxNote=(xxNote+1)/2;

		xNote=(xOctave*7)+xxNote;

		if (msg == WM_KEYDOWN && (!(lParam & 0x40000000)))	
		{
			playKey(hwnd,false,xNote,blackKey);
			mmidiOutKeyOn(noteChannel,notePos,split[noteSplit].Velocity,global.Transpose+split[noteSplit].Transpose+noteKey);
		}
		else if (msg==WM_KEYUP)
		{
			playKey(hwnd,true,xNote,blackKey);
			mmidiOutKeyOff(noteChannel,notePos,split[noteSplit].Velocity,global.Transpose+split[noteSplit].Transpose+noteKey);
		}
	}
	return 0;
}

void keyInputClick(HWND hwnd,int msg,WPARAM wParam,LPARAM lParam)
{

	int	thisChannel = getWindowOwnerIndex(hwnd);
	if (thisChannel == -1)
		return;

int	mX	=	LOWORD(lParam);
int mY  =	HIWORD(lParam);

int	whichKey			= mX/hKi[thisChannel].kKeyWidth;		//Key number
int whichOctave			= whichKey/7;			//The Octave
int whichKeyInOctave	= whichKey % 7;		//The Key in octave
int blackWhite			= mX % hKi[thisChannel].kKeyWidth;
bool	blackKey		= false;

	if (msg==WM_LBUTTONDOWN)
		blackKey		=	false;

	if (mY < (hKi[thisChannel].kKeyHeight/2))
	{
		if (whichKeyInOctave == 1 || whichKeyInOctave == 4 || whichKeyInOctave == 5)
		{
			if (blackWhite > ((hKi[thisChannel].kKeyWidth*2)/3))
				blackKey=true;
			else if (blackWhite < (hKi[thisChannel].kKeyWidth/3))
			{
				whichKeyInOctave--;
				whichKey--;
				blackKey=true;
			}
		}
		else if (whichKeyInOctave == 0 || whichKeyInOctave == 3)	//C & F
		{
			if (blackWhite > ((hKi[thisChannel].kKeyWidth*2)/3))
				blackKey=true;
		}
		else if (whichKeyInOctave == 2 || whichKeyInOctave == 6) // E & B
		{
			if (blackWhite < (hKi[thisChannel].kKeyWidth/3))
			{
				whichKeyInOctave--;
				whichKey--;
				blackKey=true;
			}
		}
		else
		{
			MessageBox(hwnd,"Code error",PROGRAM_NAME,MB_OK);
			return;
		}
	}

//	if (msg != WM_MOUSEMOVE && msg != WM_LBUTTONDOWN)
//		msg=msg+10-10;

	whichKeyInOctave*=2;
	if (whichKeyInOctave>4) whichKeyInOctave--;
	int	notePos=whichOctave*12+whichKeyInOctave+blackKey;
//		(whichKey<=4?whichKey*2:(whichKey*2)-1)+blackKey;

	if (lastMouse.NotePos != -1 && lastMouse.NotePos != notePos)
	{
		playKey(hwnd,true,lastMouse.whichKey,lastMouse.blackKey);
		mmidiOutKeyOff(lastMouse.SplitChannel,lastMouse.NotePos,0,lastMouse.SplitTranspose);
		lastMouse.NotePos=-1;
	}

	if (lastMouse.NotePos == notePos && lastMouse.SplitChannel == split[curSplitPage].Channel &&	lastMouse.SplitTranspose == split[curSplitPage].Transpose)
	{
		if (msg == WM_MOUSEMOVE) return;
	}

	{
		lastMouse.NotePos=-1;
		lastMouse.whichKey=whichKey;
		lastMouse.blackKey=blackKey;
		lastMouse.SplitChannel=split[curSplitPage].Channel;
		lastMouse.SplitKey=split[curSplitPage].Key;
		lastMouse.SplitTranspose=split[curSplitPage].Transpose;

		switch (msg)
		{
			case WM_LBUTTONDOWN:
				playKey(hwnd,false,whichKey,blackKey);
				mmidiOutKeyOn(split[curSplitPage].Channel,notePos,split[curSplitPage].Velocity,split[curSplitPage].Transpose);
				lastMouse.NotePos=notePos;
				break;

			case WM_LBUTTONUP:
				playKey(hwnd,true,whichKey,blackKey);
				mmidiOutKeyOff(split[curSplitPage].Channel,notePos,split[curSplitPage].Velocity,split[curSplitPage].Transpose);
				break;

			case WM_RBUTTONDOWN:
				if (true)
				{
					POINT	lp;
					lp.x=mX;
					lp.y=mY;
					ClientToScreen(hwnd,&lp);

					HMENU hShowMenu = GetSubMenu(hKiMenu,1);
					HMENU hPlayMenu = GetSubMenu(hKiMenu,2);

					for (int count=0;count<MAX_CHANNELS;count++)
					{
						if (hKi[thisChannel].bShowMidiInput[count])
						{
							CheckMenuItem(hShowMenu,count,MF_BYPOSITION | MF_CHECKED);
						}
						else
						{
							CheckMenuItem(hShowMenu,count,MF_BYPOSITION | MF_UNCHECKED);
						}

						if (bPlayChannel[count])
						{
							CheckMenuItem(hPlayMenu,count,MF_BYPOSITION | MF_CHECKED);
						}
						else
						{
							CheckMenuItem(hPlayMenu,count,MF_BYPOSITION | MF_UNCHECKED);
						}
					}

					TrackPopupMenu(hKiMenu,0,lp.x,lp.y,0,hwnd,0);
				}
				else
				{
					playKey(hwnd,true,whichKey,blackKey);
					mmidiOutKeyOff(split[curSplitPage].Channel,notePos,split[curSplitPage].Velocity,split[curSplitPage].Transpose);
					lastMouse.NotePos=notePos;
				}
				break;
			case WM_RBUTTONUP:
				if (true)
				{
				}
				else
				{
					playKey(hwnd,true,whichKey,blackKey);
					mmidiOutKeyOff(split[curSplitPage].Channel,notePos,split[curSplitPage].Velocity,split[curSplitPage].Transpose);
				}
				break;

			case WM_MOUSEMOVE:
				if (wParam & MK_LBUTTON)
				{
					playKey(hwnd,false,whichKey,blackKey);
					mmidiOutKeyOn(split[curSplitPage].Channel,notePos,split[curSplitPage].Velocity,split[curSplitPage].Transpose);
					lastMouse.NotePos=notePos;
				}
				else if (wParam & MK_RBUTTON)
				{
					playKey(hwnd,true,whichKey,blackKey);
					mmidiOutKeyOff(split[curSplitPage].Channel,notePos,split[curSplitPage].Velocity,split[curSplitPage].Transpose);
				}
				break;
		}
	}
	return;
}

LRESULT	CALLBACK	bnKeyInputProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{	
	int	thisChannel = getWindowOwnerIndex(hwnd);

	switch (msg)
	{
		case WM_CREATE:
			return 0;	
		case WM_KEYDOWN:
			if (!(lParam & 0x40000000))
			{
				keyPress(hwnd,WM_KEYDOWN,wParam,lParam);
			}
			return 0;
			break;
		case WM_KEYUP:
			keyPress(hwnd,WM_KEYUP,wParam,lParam);
			return 0;
			break;

		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_LBUTTONDOWN,wParam,lParam);
			return 0;
			break;
		case WM_LBUTTONUP:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_LBUTTONUP,wParam,lParam);
			return 0;
			break;			
		case WM_RBUTTONDOWN:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_RBUTTONDOWN,wParam,lParam);
			return 0;
			break;
		case WM_RBUTTONUP:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_RBUTTONUP,wParam,lParam);
			return 0;
			break;			

		case WM_MOUSEMOVE:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_MOUSEMOVE,wParam,lParam);
			return 0;
			break;			
		case WM_CLOSE:
		{
			int wfl=GetWindowLong(hwnd,GWL_STYLE);
			if (wfl & WS_CHILD)
				return 0;

			DestroyWindow(hwnd);
			break;
		}
		case MVP_PLAYNOTE:
			playKey(hwnd,wParam,lParam & 0xFF,(lParam >> 8) & 0xFF);
			break;
		case WM_PAINT:
		{
		PAINTSTRUCT psPaint;
		RECT		rect,rect2;

//			return DefWindowProc(hwnd,msg,wParam,lParam); //CallWindowProc(oldBnKeyInputProc,hwnd,msg,wParam,lParam);
			if (thisChannel == -1)
				return 0;

			HDC	tHDC = BeginPaint(hwnd,&psPaint);
			GetClientRect(hwnd,&rect);
			memcpy((char *) &rect2,(char *) &rect,sizeof(RECT));

			HBRUSH	hbr;
			switch (hKi[thisChannel].kKeyWidth)
			{
				case 9:
					hbr=hKiBrush[0];
					break;
				case 15:
					hbr=hKiBrush[1];
					break;
				case 20:
					hbr=hKiBrush[2];
					break;
				case 30:
					hbr=hKiBrush[3];
					break;
				case 45:
					hbr=hKiBrush[4];
					break;
				default:
					hbr=hKiBrush[1];
					break;
			}

			rect.left=hKi[thisChannel].kKeyWidth*7;
			rect.bottom=hKi[thisChannel].kKeyHeight;

			POINT pt;

//			MoveToEx(tHDC,rect.left,0,&pt);
			FillRect(tHDC,&rect,hbr);

			for (int count=0;count<127;count++)
			{
				if (hKi[thisChannel].keyDown[count])
				{
					int blackKey=hKi[thisChannel].keyDown[count]>>8;
					int whichKey=hKi[thisChannel].keyDown[count] & 0xFF;	

					if (blackKey)
					{
						rect.left=(whichKey*hKi[thisChannel].kKeyWidth)+ (hKi[thisChannel].kKeyWidth/3)*2;
						rect.top=0;
						rect.bottom=rect.top+hKi[thisChannel].kKeyHeight/2;
						rect.right=rect.left+((hKi[thisChannel].kKeyWidth/3)*2);
						FillRect(tHDC,&rect,RedBrush);
					}
					else
					{
						rect.left=(whichKey*hKi[thisChannel].kKeyWidth);
						rect.top=blackKey?0:hKi[thisChannel].kKeyHeight/2;
						rect.bottom=rect.top+hKi[thisChannel].kKeyHeight/2;
						rect.right=rect.left+hKi[thisChannel].kKeyWidth;
						FillRect(tHDC,&rect,RedBrush);
					}
				}
			}
//			DeleteObject(hbr);
			EndPaint(hwnd,&psPaint);
		}
		return 0;
		break;
	
		case WM_COMMAND:
			switch (HIWORD(wParam))
			{
				case 0:
					switch (LOWORD(wParam))
					{
						case KI_ATTACH:
						{
							ShowWindow(hwnd,SW_HIDE);
							int fStyle=GetWindowLong(hwnd,GWL_STYLE);
							if (fStyle & WS_CAPTION)
							{
								fStyle=fStyle & ~(WS_CAPTION);

							}
							else
							{
								fStyle=fStyle | WS_CAPTION;
							}
							SetWindowLong(hwnd,GWL_STYLE,fStyle);
							{
							RECT	rc;
								GetClientRect(hwnd,&rc);
								int	mY= rc.bottom-rc.top;
								if (mY != hKi[thisChannel].kKeyHeight)
								{
									//if (mY > hKi[thisChannel].kKeyHeight)
									rc.right=rc.top+hKi[thisChannel].kKeyHeight;

								}
							}

							ShowWindow(hwnd,SW_SHOW);					
							break;
						}

						case KI_KEYBOARD_SMALLEST:
							if (hKi[thisChannel].kKeyWidth != 9)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=48;
								hKi[thisChannel].kKeyWidth=9;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						case KI_KEYBOARD_SMALL:
							if (hKi[thisChannel].kKeyWidth != 15)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=80;
								hKi[thisChannel].kKeyWidth=15;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						case KI_KEYBOARD_MEDIUM:
							if (hKi[thisChannel].kKeyWidth != 20)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=80;
								hKi[thisChannel].kKeyWidth=20;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						case KI_KEYBOARD_BIG:
							if (hKi[thisChannel].kKeyWidth != 30)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=120;
								hKi[thisChannel].kKeyWidth=30;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						case KI_KEYBOARD_BIGGEST:
							if (hKi[thisChannel].kKeyWidth != 45)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=144;
								hKi[thisChannel].kKeyWidth=45;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						default:
						{
							int wmCmd=LOWORD(wParam);
							if (wmCmd >= KI_SHOWCHANNEL0 && wmCmd < KI_SHOWCHANNEL0+MAX_CHANNELS)
							{
								int chan=LOWORD(wParam) - KI_SHOWCHANNEL0;
//								KiShowChannel(thisChannel,chan,!hKi[thisChannel].bShowMidiInput[chan]);
								hKi[thisChannel].bShowMidiInput[chan]=!hKi[thisChannel].bShowMidiInput[chan];
							}
							else if (wmCmd >= ID_PLAYCHANNEL0 && wmCmd < ID_PLAYCHANNEL0 + MAX_CHANNELS)
							{
								int chan=LOWORD(wParam) - KI_SHOWCHANNEL0;
//								KiPlayChannel(thisChannel,chan,!hKi[thisChannel].bShowMidiInput[chan]);
								bPlayChannel[chan]=!bPlayChannel[chan];
							}
							
						}

					}
			}
			break;
		default:
			return DefWindowProc(hwnd,msg,wParam,lParam); //CallWindowProc(oldBnKeyInputProc,hwnd,msg,wParam,lParam);
	}
}

void	playKey(HWND hwnd,bool Release,int whichKey,int blackKey)
{
RECT	rect;
LPPAINTSTRUCT	lpPaint;
int	ptr=0;

	int	thisChannel = getWindowOwnerIndex(hwnd);
	if (thisChannel == -1)
		return;

	if (Release)
		for (int count=0;count<128;count++)
		{
			if (hKi[thisChannel].keyDown[count] ==((blackKey<<8) | whichKey))
				hKi[thisChannel].keyDown[count]=0;
		}
	else
	{
		while (hKi[thisChannel].keyDown[ptr] != 0 && (hKi[thisChannel].keyDown[ptr] != ((blackKey<<8) | whichKey)) && ptr <= 127)
		{
			ptr++;
		}

		if (ptr==128)
		{
			ptr--;
			int erBlackKey=(hKi[thisChannel].keyDown[0] >> 8) & 0xFF;
			int erWhichKey=hKi[thisChannel].keyDown[0] & 0xFF;
			hKi[thisChannel].keyDown[0]=0;
			playKey(hwnd,true,erWhichKey,erBlackKey);
			memmove(&hKi[thisChannel].keyDown[0],&hKi[thisChannel].keyDown[1],sizeof(int)*127);
//			keyDown[127]=0;
		}
//		else if (keyDown[ptr] == ((blackKey<<8) | whichKey))
//			return;

		hKi[thisChannel].keyDown[ptr]=((blackKey<<8) | whichKey);
	}

	int keyX=whichKey*hKi[thisChannel].kKeyWidth;

			while (bnKeyInputPainting)
			{
//				bnKeyInputPainting=true;
				int i=10;
				if (i=1)
					i=i+1;
			}
//			bnKeyInputPainting=true;

	if (hKi[thisChannel].hwnd != hwnd)
	{
		GetClientRect(hKi[thisChannel].hwnd,&rect);
	}

	GetClientRect(hwnd,&rect);
	rect.left=keyX;
	rect.right=keyX+hKi[thisChannel].kKeyWidth+(blackKey?hKi[thisChannel].kKeyWidth/3:0);
	rect.top=0;
	rect.bottom=rect.top+hKi[thisChannel].kKeyHeight;
	InvalidateRect(hKi[thisChannel].hwnd,&rect,false);
	return;
}



LRESULT CALLBACK keyWinProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	int	thisChannel = getWindowOwnerIndex(hwnd);

	switch (msg)
    {
        case WM_INITDIALOG:
			return -1;
        case WM_CLOSE:
			EndDialog(hwnd,0);
            break;
        case WM_SIZE:
            break;
		case WM_KEYDOWN:
			if (!(lParam & 0x40000000))
			{
				keyPress(hwnd,WM_KEYDOWN,wParam,lParam);
			}
			return 0;
			break;
		case WM_KEYUP:
			keyPress(hwnd,WM_KEYUP,wParam,lParam);
			return 0;
			break;

		case WM_LBUTTONDOWN:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_LBUTTONDOWN,wParam,lParam);
			return 0;
			break;
		case WM_LBUTTONUP:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_LBUTTONUP,wParam,lParam);
			return 0;
			break;			
		case WM_RBUTTONDOWN:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_RBUTTONDOWN,wParam,lParam);
			return 0;
			break;
		case WM_RBUTTONUP:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_RBUTTONUP,wParam,lParam);
			return 0;
			break;			

		case WM_MOUSEMOVE:
			SetFocus(hwnd);
			keyInputClick(hwnd,WM_MOUSEMOVE,wParam,lParam);
			return 0;
			break;			
		case WM_PAINT:
		{
		PAINTSTRUCT psPaint;
		RECT		rect,rect2;

//			return DefWindowProc(hwnd,msg,wParam,lParam); //CallWindowProc(oldBnKeyInputProc,hwnd,msg,wParam,lParam);
			if (thisChannel == -1)
				return 0;

			HDC	tHDC = BeginPaint(hwnd,&psPaint);
			GetClientRect(hwnd,&rect);
			memcpy((char *) &rect2,(char *) &rect,sizeof(RECT));

			HBRUSH	hbr;
			switch (hKi[thisChannel].kKeyWidth)
			{
				case 9:
					hbr=hKiBrush[0];
					break;
				case 15:
					hbr=hKiBrush[1];
					break;
				case 20:
					hbr=hKiBrush[2];
					break;
				case 30:
					hbr=hKiBrush[3];
					break;
				case 45:
					hbr=hKiBrush[4];
					break;
				default:
					hbr=hKiBrush[1];
					break;
			}

			rect.left=hKi[thisChannel].kKeyWidth*7;
			rect.bottom=hKi[thisChannel].kKeyHeight;

			POINT pt;

//			MoveToEx(tHDC,rect.left,0,&pt);
			FillRect(tHDC,&rect,hbr);

			for (int count=0;count<127;count++)
			{
				if (hKi[thisChannel].keyDown[count])
				{
					int blackKey=hKi[thisChannel].keyDown[count]>>8;
					int whichKey=hKi[thisChannel].keyDown[count] & 0xFF;	

					if (blackKey)
					{
						rect.left=(whichKey*hKi[thisChannel].kKeyWidth)+ (hKi[thisChannel].kKeyWidth/3)*2;
						rect.top=0;
						rect.bottom=rect.top+hKi[thisChannel].kKeyHeight/2;
						rect.right=rect.left+((hKi[thisChannel].kKeyWidth/3)*2);
						FillRect(tHDC,&rect,RedBrush);
					}
					else
					{
						rect.left=(whichKey*hKi[thisChannel].kKeyWidth);
						rect.top=blackKey?0:hKi[thisChannel].kKeyHeight/2;
						rect.bottom=rect.top+hKi[thisChannel].kKeyHeight/2;
						rect.right=rect.left+hKi[thisChannel].kKeyWidth;
						FillRect(tHDC,&rect,RedBrush);
					}
				}
			}
//			DeleteObject(hbr);
			EndPaint(hwnd,&psPaint);
		}
		return 0;
		break;
		case WM_COMMAND:
			switch (HIWORD(wParam))
			{
				case 0:
					switch (LOWORD(wParam))
					{
						case KI_ATTACH:
						{
							ShowWindow(hwnd,SW_HIDE);
							int fStyle=GetWindowLong(hwnd,GWL_STYLE);
							if (fStyle & WS_CAPTION)
							{
								fStyle=fStyle & ~(WS_CAPTION);

							}
							else
							{
								fStyle=fStyle | WS_CAPTION;
							}
							SetWindowLong(hwnd,GWL_STYLE,fStyle);
							{
							RECT	rc;
								GetClientRect(hwnd,&rc);
								int	mY= rc.bottom-rc.top;
								if (mY != hKi[thisChannel].kKeyHeight)
								{
									//if (mY > hKi[thisChannel].kKeyHeight)
									rc.right=rc.top+hKi[thisChannel].kKeyHeight;

								}
							}

							ShowWindow(hwnd,SW_SHOW);					
							break;
						}

						case KI_KEYBOARD_SMALLEST:
							if (hKi[thisChannel].kKeyWidth != 9)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=48;
								hKi[thisChannel].kKeyWidth=9;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						case KI_KEYBOARD_SMALL:
							if (hKi[thisChannel].kKeyWidth != 15)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=80;
								hKi[thisChannel].kKeyWidth=15;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						case KI_KEYBOARD_MEDIUM:
							if (hKi[thisChannel].kKeyWidth != 20)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=80;
								hKi[thisChannel].kKeyWidth=20;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						case KI_KEYBOARD_BIG:
							if (hKi[thisChannel].kKeyWidth != 30)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=120;
								hKi[thisChannel].kKeyWidth=30;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						case KI_KEYBOARD_BIGGEST:
							if (hKi[thisChannel].kKeyWidth != 45)
							{
								ShowWindow(hKi[thisChannel].hwnd,SW_HIDE);
								hKi[thisChannel].kKeyHeight=144;
								hKi[thisChannel].kKeyWidth=45;
								ShowWindow(hKi[thisChannel].hwnd,SW_SHOW);
							}
							break;
						case KI_RESETALLKEYS:
							{
							for (int cx=0;cx<128;cx++)
								hKi[thisChannel].keyDown[cx]=0;

							InvalidateRect(hKi[thisChannel].hwnd,NULL,true);

							break;
							}
						default:
						{
							int wmCmd=LOWORD(wParam);
							if (wmCmd >= KI_SHOWCHANNEL0 && wmCmd < KI_SHOWCHANNEL0+MAX_CHANNELS+2)
							{
								int chan=LOWORD(wParam) - KI_SHOWCHANNEL0;
								if (chan == (MAX_CHANNELS-2))
								{
									for (int cx=0;cx< MAX_CHANNELS;cx++){hKi[cx].bShowKeyboard = true;}
								}
								else if (chan == (MAX_CHANNELS-1))
								{
									for (int cx=0;cx< MAX_CHANNELS;cx++){hKi[cx].bShowKeyboard = false;}
								}
								else
								{
									hKi[thisChannel].bShowMidiInput[chan]=!hKi[thisChannel].bShowMidiInput[chan];
								}
							}
							else if (wmCmd >= ID_PLAYCHANNEL0 && wmCmd < ID_PLAYCHANNEL0 + MAX_CHANNELS)
							{
								int chan=LOWORD(wParam) - KI_SHOWCHANNEL0;

								if (chan == (MAX_CHANNELS-2))
								{
									for (int cx=0;cx< MAX_CHANNELS;cx++){bPlayChannel[cx] = true;}
								}
								else if (chan == (MAX_CHANNELS-1))
								{
									for (int cx=0;cx< MAX_CHANNELS;cx++){bPlayChannel[cx] = false;}
								}
								else
								{
									bPlayChannel[chan]=!bPlayChannel[chan];
								}
							}							
						}
					}
			}
		default:
			return 0;
	}
return 0;
}
