int waitForCommandLock()
{
int wTimer = GetTickCount();
	while (bCommandLock)
	{
		if(!bMidiSeqThreadRunning || (GetTickCount()- wTimer) > 5000 )
			return 0;
	}
	return -1;
}

int seqToolScroll(HWND hwnd,WPARAM wParam,LPARAM lParam)
{
	int newPos = SendMessage((HWND) lParam,TBM_GETPOS,0,0);
	
	if ((HWND) lParam == GetDlgItem(hwnd,TR_SEEK))
	{
		if (bMidiSeqThreadRunning && waitForCommandLock() )
		{
			bCommandLock=true;
			dwSeqCmd = SEQCMD_SEEK;
			dwSeqCmdData = newPos;
			bServiceCommand = true;
			bCommandLock=false;
		}
	}
	return 0;
}

int seqAddPlayListDir(char *dir)
{
	if (numPlayListDirs >= MAX_PLAYLISTDIRS)
		return -1;

	int	nEmptyDir=-1;

	for (int count=0,fIndex=0;count < numPlayListDirs || fIndex==MAX_PLAYLISTDIRS;fIndex++)
	{
		if ( DirList[fIndex])
		{
			if (strcmpi(dir,DirList[fIndex])==0)
			{
				return fIndex;
			}
			count++;
		}
		else
		{
			if (nEmptyDir == -1)
			{
				nEmptyDir = fIndex;
			}
		}
	}

	if (nEmptyDir == -1)
	{
		for (int count=0;count < MAX_PLAYLISTDIRS;count++)
		{
			if (DirList[count] == 0)
			{
				nEmptyDir = count;
				break;
			}
		}
	}

	int dirLen = strlen(dir);

	DirList[nEmptyDir] = (char *) malloc(dirLen);
	if (DirList[nEmptyDir])
	{
		strcpy(DirList[nEmptyDir],dir);
		numPlayListDirs++;
		return nEmptyDir;
	}
	else
		return -1;
}

int seqAddPlayListFile(HWND hwnd,int dirNum,char *fileName)
{
	if (numPlayListFiles >= MAX_PLAYLISTFILES)
		return -1;

	int fileLen = strlen(fileName);

	plf[numPlayListFiles].dirNum = dirNum;
	plf[numPlayListFiles].fileName=(char *)malloc(fileLen+2);
	
	if (plf[numPlayListFiles].fileName)
	{
		strcpy(plf[numPlayListFiles].fileName,fileName);
		numPlayListFiles++;
		SendMessage(hwnd,LB_ADDSTRING,0,(LPARAM) fileName);
	}
	return -1;

	return 0;
}

LRESULT CALLBACK seqToolProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	switch (msg)
    {
        case WM_INITDIALOG:
		{
			for (int count=0;count<MAX_PLAYLISTFILES;count++)
			{
				plf[count].fileName=0;
				DirList[count] = 0;
			}
			SendDlgItemMessage(hwnd,BN_OPENMIDIFILE,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_OPEN));
			SendDlgItemMessage(hwnd,BN_SAVEMIDIFILE,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_SAVE));

			SendDlgItemMessage(hwnd,BN_PLAYMIDIFILE,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_PLAY));
			SendDlgItemMessage(hwnd,BN_STOPMIDIFILE,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_STOP));

			SendDlgItemMessage(hwnd,BN_FF,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_FF));
			SendDlgItemMessage(hwnd,BN_REW,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_REW));
			SendDlgItemMessage(hwnd,BN_BACK,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_BACK));
			SendDlgItemMessage(hwnd,BN_FORWARD,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_FORWARD));

			SendDlgItemMessage(hwnd,BN_TRANSPOSEUP,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_TRANSPOSEUP));
			SendDlgItemMessage(hwnd,BN_TRANSPOSEDOWN,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_TRANSPOSEDOWN));

			SendDlgItemMessage(hwnd,TR_SEEK,TBM_SETRANGEMIN,0,0);
			SendDlgItemMessage(hwnd,TR_SEEK,TBM_SETRANGEMAX,0,trSeekMaxValue);
			SendDlgItemMessage(hwnd,TR_SEEK,TBM_SETPOS,true,0);
			SetFocus(hwnd);
			return -1;
		}
		case WM_CLOSE:
			while (bMidiSeqThreadRunning)
			{
				bPlayMidiFile=false;
				bServiceCommand=true;
				dwSeqCmd=SEQCMD_QUIT;
			}
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
		case WM_HSCROLL:
			seqToolScroll(hwnd,wParam,lParam);
			return 0;
		case WM_COMMAND:
			switch ( HIWORD(wParam) )
			{
				case 0:
					switch (LOWORD(wParam))
					{
						case BN_OPENMIDIFILE:
						{
							OPENFILENAME lpfn;
							char *fBuffer = (char *) malloc(1048576*2);
							char *mBuffer=fBuffer;
							int	nSeqNumFiles = numPlayListFiles;
	//						if (seqFileBuffer==0)
	//							seqFileBuffer =  malloc(1048576*2);
							memset(&lpfn,0,sizeof(OPENFILENAME));
							fBuffer+=2;
							*fBuffer = 0;

							lpfn.hInstance= hInstance;
							lpfn.lStructSize=sizeof(OPENFILENAME);
							lpfn.lpstrFilter="Midi Files\0*.mid;*.midi;*.rmi\0Karaoke File\0*.kar\0All supported types\0*.mid;*.midi;*.rmi;*.kar;*.sty\0All files\0*.*\0\0";
							lpfn.nFilterIndex=3; 
							lpfn.nMaxFile=2*1048576;
							lpfn.lpstrDefExt="exe";
							lpfn.hwndOwner=hwnd;
							lpfn.lpstrFile=fBuffer;
							lpfn.lpstrTitle="Open Midi File";
							lpfn.Flags=OFN_EXPLORER | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST;
							if (GetOpenFileName(&lpfn) && *fBuffer != 0)
							{
								char *filePTR = fBuffer + (lpfn.nFileOffset);
								int dirPathLen = strlen(fBuffer);
								
								if (dirPathLen > lpfn.nFileOffset)
								{
									memmove(fBuffer-2,fBuffer,lpfn.nFileOffset);
									fBuffer-=2;
									*(fBuffer+lpfn.nFileOffset)=0;
									*(fBuffer+lpfn.nFileOffset-1)=0;
								}

								int dirNum = seqAddPlayListDir(fBuffer);
								char *nullChar;
								do
								{
									nullChar = strchr(filePTR,0);
									if (nullChar)
									{
										seqAddPlayListFile(GetDlgItem(hwnd,LB_SEQPLAYLIST),dirNum,filePTR);
										filePTR = (char *) nullChar+1;
									}
									else
										break;
								}
								while ( *filePTR != 0 );

								free(mBuffer);

								if (!bPlayMidiFile)
								{
									seqCurPlayListFile=nSeqNumFiles ;
									seqCurMidiFile=plf[0].fileName;
									seqCurFileDir=DirList[ plf[0].dirNum ];
								}
								else
									break;
							}
						}
						case BN_PLAYMIDIFILE:
						{
							if (seqCurFileDir == 0 || seqCurMidiFile == 0)
							{
								if (numPlayListFiles)
								{
									seqCurPlayListFile=0;
									seqCurMidiFile=plf[0].fileName;
									seqCurFileDir=DirList[ plf[0].dirNum ];
								}
								break;
							}

							if (!bMidiSeqThreadRunning)
							{
								if (!CreateThread(0,0,(LPTHREAD_START_ROUTINE) &playmidithread,(LPVOID) hwnd,0,&threadID))
								{
									MessageBox(hwnd,"Failed to initialize midi playing thread",PROGRAM_NAME,MB_OK | MB_ICONERROR);
									break;
								}
							}

							if (bPlayMidiFile)
								bPause = (!bPause);

							bPlayMidiFile=true;

							if (bPause)
								SendDlgItemMessage(hwnd,BN_PLAYMIDIFILE,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_PLAY));
							else
								SendDlgItemMessage(hwnd,BN_PLAYMIDIFILE,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_PAUSE));
							break;
						}
						case BN_STOPMIDIFILE:
							if (bMidiSeqThreadRunning && waitForCommandLock() )
							{
								bCommandLock=true;
								dwSeqCmd = SEQCMD_STOP;
								bServiceCommand = true;
								bCommandLock=false;
							}
							break;
						case BN_FF:
							if (bMidiSeqThreadRunning && waitForCommandLock() )
							{
								bCommandLock=true;
								dwSeqCmd = SEQCMD_FF;
								bServiceCommand = true;
								bCommandLock=false;
							}
							break;
						case BN_REW:
								if (bMidiSeqThreadRunning && waitForCommandLock() )
								{
									bCommandLock=true;	
									dwSeqCmd = SEQCMD_REW;
									bServiceCommand = true;
									bCommandLock=false;
								}
							break;
						case BN_FORWARD:
								if (bMidiSeqThreadRunning && waitForCommandLock() )
								{
									bCommandLock=true;
									dwSeqCmd = SEQCMD_FORWARD;
									dwSeqCmdData=1;
									bServiceCommand = true;
									bCommandLock=false;
								}
							break;
						case BN_BACK:
								if (bMidiSeqThreadRunning && waitForCommandLock() )
								{
									bCommandLock=true;
									dwSeqCmd = SEQCMD_BACK;
									dwSeqCmdData=1;
									bServiceCommand = true;
									bCommandLock=false;
								}
							break;
						case BN_TRANSPOSEUP:
								if (bMidiSeqThreadRunning && waitForCommandLock() )
								{
									bCommandLock=true;
									dwSeqCmd = SEQCMD_TRANSPOSEUP;
									dwSeqCmdData=1;
									bServiceCommand = true;
									bCommandLock=false;
								}
							break;
						case BN_TRANSPOSEDOWN:
								if (bMidiSeqThreadRunning && waitForCommandLock() )
								{
									bCommandLock=true;
									dwSeqCmd = SEQCMD_TRANSPOSEDOWN;
									dwSeqCmdData=1;
									bServiceCommand = true;
									bCommandLock=false;
								}
							break;
					}
					return 0;
				case LBN_DBLCLK:
					switch (LOWORD(wParam))
					{
						case LB_SEQPLAYLIST:
							int curItem =SendMessage((HWND) lParam,LB_GETCURSEL,0,0);
							if (curItem != -1)
							{

								if (bMidiSeqThreadRunning && waitForCommandLock() )
								{
									bCommandLock=true;
									dwSeqCmd = SEQCMD_JUMPTO;
									dwSeqCmdData = curItem;
									bServiceCommand = true;
									bCommandLock=false;
								}
							}
							break;
					}
					break;
		default:
			return 0;
			}
	}
return 0;
}
