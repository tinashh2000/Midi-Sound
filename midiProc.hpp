void InitMidiInDevices(HWND hwnd)
{
	int nMidiDevs = midiInGetNumDevs();
	int nCurDev=0;
	nMidiInDevs=0;			
	MIDIINCAPS mInCaps;

	dwDefMidiInDevice=0;
	dwTTSVPianoOutDevice=-1;

	for (nCurDev=0;nCurDev < nMidiDevs;nCurDev++)
	{
		if (midiInGetDevCaps(nCurDev,&mInCaps,sizeof(mInCaps))==0)
		{
			SendDlgItemMessage(hwnd,CB_MIDIINDEVICE,CB_ADDSTRING,0,(LPARAM) &mInCaps.szPname);
			if (strcmp((char *) &mInCaps.szPname,"TTS Virtual Piano In") == 0)
				dwTTSVPianoInDevice	=	nCurDev;

			if (strcmp( (char *) &mInCaps.szPname,(char *) &lpDefMidiInDevice) == 0)
				dwDefMidiInDevice=nCurDev;

			nMidiInDevs++;
		}
	}

	if (nMidiInDevs)
	{
		SendDlgItemMessage(hwnd,CB_MIDIINDEVICE,CB_SETCURSEL,dwDefMidiInDevice,0);
	}
}

void InitMidiOutDevices(HWND hwnd)
{
	int nMidiDevs = midiOutGetNumDevs();
	int nCurDev=0;
	nMidiOutDevs=0;			
	MIDIOUTCAPS mOutCaps;
	MIDIINCAPS	mInCaps;

	dwDefMidiOutDevice=0;
	dwTTSVPianoOutDevice=-1;
	for (nCurDev=0;nCurDev < nMidiDevs;nCurDev++)
	{
		if (midiOutGetDevCaps(nCurDev,&mOutCaps,sizeof(mOutCaps))==0)
		{
			SendDlgItemMessage(hwnd,CB_MIDIOUTDEVICE,CB_ADDSTRING,0,(LPARAM) &mOutCaps.szPname);
//			if (strcmp((char *) &mOutCaps.szPname,"TTS Virtual Piano In") == 0)
//				dwTTSVPianoOutDevice	=	nCurDev;

			if (strcmp( (char *) &mOutCaps.szPname,(char *) &lpDefMidiOutDevice) == 0)
				dwDefMidiOutDevice=nCurDev;

			nMidiOutDevs++;
		}
	}

	int nMidiInDevs=midiInGetNumDevs();
	for (nCurDev=0;nCurDev < nMidiInDevs;nCurDev++)
	{
		if (midiInGetDevCaps(nCurDev,&mInCaps,sizeof(mInCaps))==0)
		{
			if (strcmp((char *) &mInCaps.szPname,"TTS Virtual Piano In") == 0)
			{
				SendDlgItemMessage(hwnd,CB_MIDIOUTDEVICE,CB_ADDSTRING,0,(LPARAM) &mInCaps.szPname);
				dwTTSVPianoOutDevice	=	nMidiOutDevs;

				if (dwTTSVPianoInDevice==-1)
					dwTTSVPianoInDevice=nCurDev;

				nMidiOutDevs++;
				break;
			}

		}
	}

	if (nMidiOutDevs)
	{
		SendDlgItemMessage(hwnd,CB_MIDIOUTDEVICE,CB_SETCURSEL,dwDefMidiOutDevice,0);
	}
}

LRESULT CALLBACK midiToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
    {
        case WM_INITDIALOG:
			InitMidiInDevices(hwnd);
			InitMidiOutDevices(hwnd);
			return -1;
        case WM_CLOSE:
			EndDialog(hwnd,0);
			return 1;
            break;
        case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case 0:
					switch (LOWORD(wParam))
					{
						case BN_MIDIINSTART:
							if (mInOpen)
								mmidiInStop(hwnd,0);
							else
								mmidiInStart(hwnd,0);
							break;
						case BN_MIDIOUTSTART:
							if (mOutOpen)
								mmidiOutStop(hwnd,0);
							else
								mmidiOutStart(hwnd,0);
							break;
						case BN_RESETNOTES:
							resetAllChannels();
							mmidiOutReset();
							break;
						case BN_MIDIINRESET:
							mmidiInReset();
							break;
						case BN_MIDIOUTRESET:
							mmidiOutReset();
							break;

					}
					break;
				case CBN_SELCHANGE:
					switch (LOWORD(wParam))
					{
						case CB_MIDIINDEVICE:
							midiInDeviceChange(hwnd);
							break;
						case CB_MIDIOUTDEVICE:
							midiOutDeviceChange(hwnd);
							break;
					}
					break;
				default:
					break;
			}
            break;
		default:
			return 0;
	}
return 0;
}




LRESULT CALLBACK xProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
    {
        case WM_INITDIALOG:
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