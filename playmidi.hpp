#define MAX_CHANNELS 16

int GetTrackLength(TRACKDATA *trData,char *buf);

long getVarLength(char *ptr,long *varLen)
{
	long this_value=0;
	char this_byte = *ptr;
	ptr++;
	
	this_value=this_byte & 0x7F;

	if (varLen) *(varLen)=1;
	
	if (this_byte & 0x80)
	{

		this_byte = *ptr;
		ptr++;
		if (varLen) *varLen+=1;
		this_value=(this_value << 7)| (this_byte & 0x7F);
		
		if (this_byte & 0x80)
		{
			this_byte = *ptr;
			ptr++;
			if (varLen) *varLen+=1;
			this_value=(this_value << 7)| (this_byte & 0x7F);
			if (this_byte & 0x80)
			{
				this_byte = *ptr;
				ptr++;
				if (varLen) *varLen+=1;
				this_value=(this_value << 7)| (this_byte & 0x7F);
				if (this_byte & 0x80)
				{
					this_byte = *ptr;
					ptr++;
					this_value=(this_value << 7)| (this_byte & 0x7F);
				}
			}
		}
	}

	return this_value;
}



int AddTrackToQueue(int tracknum,int midi_time,int numtracks)
{
int	newPos;

	for (newPos=0;newPos<numtracks;newPos++)
	{
		if (EventsOrder[newPos] == 0)
			break;
	}

	if (newPos >=numtracks)
		return -1;		//No more tracks can be contained

//Sort out the track events so that the first event comes up top
	while (newPos > 0 && (midi_time < EventsOrder[newPos-1]->nextEventTime))
	{
		EventsOrder[newPos] = EventsOrder[newPos-1];
		newPos--;
	}

	EventsOrder[newPos] = &Tracks[tracknum];

	return 0;
}

int	AddEventToQueue(int tracknum,int midi_time,int numtracks)
{
TRACKDATA	*trackDataX;
TRACKDATA	*trackDataY;
int ccount;
	for (ccount = 0;ccount<numtracks;ccount++)
	{
		if (midi_time < EventsOrder[ccount]->nextEventTime)
		{
			memmove(&EventsOrder[ccount+1],&EventsOrder[ccount],(numtracks-ccount)*4);
			break;
		}
	}

	if (ccount >=numtracks)
		return -1;		//No more tracks can be contained

	return ccount;
}



void conv_bigendian32(long *value)
{

	long old_value=*value;
	char *p = (char *) &old_value;
	char *q = (char *) value;

	*q		= *(p+3);
	*(q+1)	= *(p+2);
	*(q+2)	= *(p+1);
	*(q+3)	= *p;
}

void conv_bigendian16(short int *value)
{

	long old_value=*value;
	char *p = (char *) &old_value;
	char *q = (char *) value;

	*q		= *(p+1);
	*(q+1)	= *p;
}

int ExecuteTrackEvent(TRACKDATA *trackData,char *buffer,bool bVirtual)
{
	unsigned char cmdBytes[8];
	long varLen=0;

	getVarLength(buffer + trackData->curPosition,&varLen);
	trackData->curPosition+=varLen;
//	cmdPtr+=(varLen);
	char	*cmdPtr = (buffer + trackData->curPosition);

	cmdBytes[0] = *cmdPtr;
	int maskCmd = cmdBytes[0] & 0xF0;

//	if (trackData == &Tracks[7])
//		varLen=0;

	if ((maskCmd & 0x80) == 0)			//If running status
	{
		maskCmd = trackData->lastCmd & 0xF0;
		cmdBytes[0] = trackData->lastCmd;
		cmdBytes[1] = *(cmdPtr);
		cmdBytes[2] = *(cmdPtr+1);
		cmdBytes[3]	= 0;
	}

	trackData->lastCmd = cmdBytes[0];

	if (maskCmd >= 0x80 && maskCmd <= 0xEF)
	{
		if (*cmdPtr & 0x80)
		{
			cmdBytes[1] = *(cmdPtr+1);
			cmdBytes[2] = *(cmdPtr+2);
			cmdBytes[3] = 0;

			if (maskCmd == 0xC0 || maskCmd == 0xD0)
				trackData->curPosition+=2;
			else
				trackData->curPosition+=3;
		}
		else
		{
			if (maskCmd == 0xC0 || maskCmd == 0xD0)
				trackData->curPosition+=1;
			else
				trackData->curPosition+=2;
		}

//		if (maskCmd == 0x80 || maskCmd == 0x90)
//			varLen = 2;

//	if ( (maskCmd != 0x90 && maskCmd != 0x80) || cmdBytes[0] == 0x97 || cmdBytes[0] == 0x87)
		{
			if (maskCmd == 0x90 ||  maskCmd == 0x80)
			{
//				char dbgstr[128];
	
//				int *cb = (int *) &cmdBytes[0];

//				sprintf( (char *) &dbgstr,"%06x %04x",*cb,trackData->curTime);
//				if ( !(*cb & 0xFF0000) || maskCmd == 0x80)
//					SendDlgItemMessage(seqHwnd,LB_DEBUG,LB_ADDSTRING,0,(LPARAM) &dbgstr);
//				else
//					SendDlgItemMessage(seqHwnd,LB_DEBUG2,LB_ADDSTRING,0,(LPARAM) &dbgstr);
//
//				maskCmd = maskCmd & 0xFF;
			}

			int *cb = (int *) &cmdBytes[0];
			if (!bVirtual || (maskCmd != 0x90 && maskCmd != 0x80)) //When virtual do not play any notes
			{
				if ((maskCmd == 0x90 || maskCmd == 0x80) && seqTranspose)
					if ( (cmdBytes[0] & 0xF) != 9)
						cmdBytes[1] += seqTranspose;

//				if ((cmdBytes[0] & 0xF) == 9)
					mmidiInMsg(*cb,0);
			}
		}


		int midi_time = getVarLength(buffer + trackData->curPosition,0);
		trackData->curTime += trackData->nextEventTime;		//The event that just occured
		trackData->nextEventTime = midi_time;
	}
	else
	{
		switch (cmdBytes[0])
		{
			case 0xFF:
				cmdBytes[1] = *(cmdPtr+1);		//Command byte
				cmdBytes[2] = *(cmdPtr+2);	//Length byte
	
				switch (cmdBytes[1])
				{
					case 0x00:
						break;
					case 0x01:
						break;
					case 0x02:
						break;
					case 0x03:
						break;
					case 0x04:
						break;
					case 0x05:
						break;
					case 0x06:
						break;
					case 0x07:
						break;
					case 0x08:
						break;
					case 0x2F:	//End of track
						break;
					case 0x51:
					{
						int *tempox= (int *) &cmdBytes[4];
						cmdBytes[4] = *(cmdPtr+5);
						cmdBytes[5] = *(cmdPtr+4);
						cmdBytes[6] = *(cmdPtr+3);

						microSecPerQuarter = *(tempox) & 0xFFFFFF;
						setTempo();
						break;
					}
					case 0x58:
					{
						int timesig=0;
						break;
					}
					case 0x59:
						break;
					case 0x7F:
						break;		
					default:
						break;
				}

				if (cmdBytes[1] != 0x2f)
				{
					int metaLen = getVarLength(buffer + trackData->curPosition+2,&varLen);
					trackData->curPosition += (metaLen+2+varLen);
					int midi_time = getVarLength(buffer+trackData->curPosition,0);
					trackData->curTime += trackData->nextEventTime;		//The event that just occured
					trackData->nextEventTime = midi_time;
				}
				else
					trackData->nextEventTime = -1;
		}
	}

	if (trackData->curPosition >= trackData->endPosition)
		trackData->nextEventTime = -1;

	return 0;
}

/*
void ExecuteEvents(int tymCode,int numtracks,char *buffer)
{

int timeCode = tymCode;

	if (timeCode == -1)
		timeCode = EventsOrder[0]->nextEventTime;


	for (int count=0;count < numtracks;count++)
	{

		unsigned int midi_time = EventsOrder[count]->nextEventTime;
		while (midi_time == timeCode && midi_time != -1)
		{			
			ExecuteTrackEvent(EventsOrder[count],buffer);
			midi_time = EventsOrder[count]->nextEventTime;

			if (midi_time != timeCode)
			{
				int ncount = count;
				TRACKDATA *tData = EventsOrder[count];

				if (count+1 < numtracks && midi_time >= EventsOrder[count+1]->nextEventTime)		//Downwards
				{
					while (ncount+1 < numtracks && midi_time >= EventsOrder[ncount+1]->nextEventTime)
					{
						EventsOrder[ncount] = EventsOrder[ncount+1];
						ncount++;
					}
					EventsOrder[ncount] = tData;
				}
				else		// Upwards
				{
					if (ncount > 0)		//if zero, dont change anything
					{
						while (ncount > 0 && midi_time < EventsOrder[ncount]->nextEventTime)
						{
							EventsOrder[ncount] = EventsOrder[ncount-1];		//Move upwards
							ncount--;
						}
					}
					EventsOrder[ncount] = tData;

				}
				if (ncount != count)	//If some pointers were changed, go back to avoid skipping unprocessed events
					count--;
			}
		}
	}
}
*/
int ResetPlayback()
{
	for (int count = 0;count < MAX_TRACKS;count++)
	{
		memset( &Tracks[count],0,sizeof(TRACKDATA));
		EventsOrder[count] =  0;
	}

	midiSpeed=1;
//	resetMidiNotes();
	return 0;
}

void playMidiFile(HWND hwnd)
{
	char *midiFileName=0;
	char *nameBuffer=0;
	char *buffer=0;

	seqHwnd = hwnd;
	int			mError	=	0;
	int			dwTemp1;
	short int	wTemp1;
	int			nextEventTime=0;
	char *ptr;
	
	MIDIFILEHDR		mThd;
	MIDITRACKHDR	mTrk;

	try
	{

		if (seqCurFileDir == 0 || seqCurMidiFile == 0 )
			throw MIDI_FILEERROR;

		int fpathLen=strlen( seqCurFileDir)+(strlen(seqCurMidiFile)*2)+4;

		midiFileName = (char *) malloc(fpathLen);
		nameBuffer=(char *) malloc(strlen(SEQ_WINTEXT)+strlen(seqCurMidiFile)+10);

		if (nameBuffer == 0)
			throw MIDI_MEMORYERROR;
		
		sprintf(nameBuffer,"%s - %s\0",SEQ_WINTEXT,seqCurMidiFile);
		

		sprintf(midiFileName,"%s\\%s\0",seqCurFileDir,seqCurMidiFile);

		int fhnd = open(midiFileName,O_RDONLY);
		if (fhnd == -1 || fhnd == 0)
			throw(MIDI_FILEERROR);

		int nFileSize = filelength(fhnd);
		
		if (nFileSize & 0xF8000000)
			throw(MIDI_FILEERROR);

		buffer= (char *) malloc(nFileSize);
		memset(buffer,0,nFileSize);
		
		ResetPlayback();
		
		if (buffer == 0)
			throw(MIDI_MEMORYERROR);

		if (fhnd==0)
			throw(MIDI_FILEERROR);
		
//		int dwBytesRead=fread(buffer,nFileSize,1,midiFile);
		int dwBytesRead=read(fhnd,buffer,nFileSize);
		dwBytesRead+=read(fhnd,buffer+(dwBytesRead),nFileSize);
		close(fhnd);

		memcpy( &mThd,buffer,sizeof(mThd));
		
		if (memcmp(&mThd.signature,"MThd",4) != 0)
			throw(MIDI_INVALIDHEADER);
	
		conv_bigendian32( (long *) &mThd.headerSize);
		conv_bigendian16( (short int *) &mThd.numTracks);
		conv_bigendian16( (short int *) &mThd.deltaTime);

		ticksPerQuarter = mThd.deltaTime;
		if (ticksPerQuarter & 0x8000)
		{
			microSecPerQuarter=60000000/120;		//Set a tempo of 120bpm
		}

		microSecPerQuarter=60000000/120;		//Set a tempo of 120bpm
		setTempo();

		if (mThd.headerSize != 6)
		{
			throw(MIDI_INVALIDHEADER);
		}

		ptr = buffer + 14;

		unsigned __int64 totalTime=0;

		for (int ccount=0;ccount<mThd.numTracks;ccount++)
		{
			if (memcmp(ptr,"MTrk",4) != 0)
				throw (MIDI_INVALIDFILE);

			int *iPtr=(int *) (ptr+4);
			Tracks[ccount].trackLength = (*iPtr);
			conv_bigendian32((long *) &Tracks[ccount].trackLength);
			Tracks[ccount].startPosition=(ptr - buffer)+8;
			Tracks[ccount].curPosition = (ptr - buffer)+8;
			Tracks[ccount].endPosition = (ptr - buffer)+8+Tracks[ccount].trackLength;

			Tracks[ccount].totalTime = GetTrackLength(&Tracks[ccount],buffer);
			if (Tracks[ccount].totalTime > totalTime && Tracks[ccount].totalTime != -1)
				totalTime = Tracks[ccount].totalTime;
			ptr=(ptr+Tracks[ccount].trackLength)+8;

		}

		int lastTick = GetTickCount();
		int thisTick;
		int timeDelayed;
//		int startTick=lastTick;
		int endedtracks=0;

		signed __int64 curTime=0;
		unsigned __int64 curTimeSecs=0;
		unsigned __int64 trSeekValue=0;
		float xmidiSpeed=midiSpeed;
		bool bPauseAck=false;
		bPause=false;

		totalTime= (signed __int64) totalTime * milliSecPerTick;

		SetWindowText(hwnd,(LPSTR) nameBuffer);
		SendDlgItemMessage(hwnd,LB_SEQPLAYLIST,LB_SETCURSEL,seqCurPlayListFile,0);
		while (endedtracks < mThd.numTracks && bPlayMidiFile)
		{
			if (bServiceCommand)
			{
				if (dwSeqCmd < SEQCMD_PLAYBACK)
					break;		//We do not service these ones
				else
				{
					switch (dwSeqCmd)
					{
						case SEQCMD_REW:
							if (midiSpeed>0.1)
								midiSpeed-=0.1;
							break;
						case SEQCMD_FF:
							if (midiSpeed<3)
								midiSpeed+=0.1;
							break;
						case SEQCMD_TRANSPOSEUP:
							seqTranspose+=dwSeqCmdData;
							if (seqTranspose > 48)
								seqTranspose = 48;
							break;
						case SEQCMD_TRANSPOSEDOWN:
							seqTranspose-=dwSeqCmdData;
							if (seqTranspose < -48)
								seqTranspose = -48;
							break;
						case SEQCMD_SEEK:
//							break;
							if (trSeekValue > dwSeqCmdData)
							{
								for (int ccount=0;ccount<mThd.numTracks;ccount++)
								{
									Tracks[ccount].curPosition = Tracks[ccount].startPosition;
									Tracks[ccount].nextEventTime = 0;
									Tracks[ccount].curTime=0;
									Tracks[ccount].lastCmd = 0;
								}
							}

							signed __int64 targetTime = ((signed __int64) (dwSeqCmdData * totalTime) / trSeekMaxValue);

							for (int cx = 0; cx < mThd.numTracks;cx++)
							{
								while (Tracks[cx].nextEventTime != -1 &&  ((Tracks[cx].curTime + Tracks[cx].nextEventTime)*milliSecPerTick) < targetTime)
								{
									ExecuteTrackEvent(&Tracks[cx],buffer,true);
								}
							}

							curTime=targetTime;
							lastTick = GetTickCount();
							break;
					}
					mmidiOutReset();
					bServiceCommand=false;

				}
			}

			Sleep(0);

			if (bPause && bPauseAck)
			{
				Sleep(0);
			}
			else if ((!bPause) || (bPause && !bPauseAck))
			{
				thisTick=GetTickCount();

				if (bPauseAck && !bPause)
				{
					lastTick=thisTick;
					bPauseAck = false;
				}

				timeDelayed=(thisTick-lastTick)*xmidiSpeed;
				curTime+=timeDelayed;
				lastTick=thisTick;


				if (bPause)
				{
					bPauseAck=true;
					mmidiOutReset();
				}
				else
				{
					if (curTimeSecs != (curTime/1000))
					{
						curTimeSecs = curTime / 1000;
						int secs = (curTime / 1000) % 60;	//Seconds
						int min = (curTime / 60000) % 60;
						int hrs = (curTime / 3600000) % 60;

						int xsecs = (totalTime / 1000) % 60;	//Seconds
						int xmin = (totalTime / 60000) % 60;
						int xhrs = (totalTime / 3600000) % 60;

						char wrbuf[128];

						char	csecs[4];
						char	cmins[4];
						char	chours[4];

						float midiPerc = (float) (midiSpeed);
						sprintf((char *) &wrbuf,"(x%.2f) %02d:%02d:%02d of %02d:%02d:%02d   %I64u,cur%I64u,ttl %I64u,trMx%u, Transpose:%i",midiPerc,hrs,min,secs,xhrs,xmin,xsecs,trSeekValue,curTime,totalTime,trSeekMaxValue,seqTranspose);
						SendDlgItemMessage(hwnd,ED_CURTIME,WM_SETTEXT,NULL,(LPARAM) &wrbuf);
					}

					__int64 trSV = curTime * trSeekMaxValue /totalTime;
					
					if (trSeekValue != trSV)
					{
						trSeekValue = trSV;
						SendDlgItemMessage(hwnd,TR_SEEK,TBM_SETPOS,true,trSeekValue);
					}

				}
			
				endedtracks=0;
				for (int cx = 0; cx < mThd.numTracks;cx++)
				{
					if (( (  (Tracks[cx].curTime + Tracks[cx].nextEventTime)*milliSecPerTick )) <= (curTime) && Tracks[cx].nextEventTime != -1)
					{
						ExecuteTrackEvent(&Tracks[cx],buffer,false);
					}
					else if (Tracks[cx].nextEventTime == -1)
						endedtracks++;
				}
				xmidiSpeed = midiSpeed;
			}
		}
	}

	catch(int nError)
	{
		if (buffer)
			free(buffer);
		
		if (nameBuffer)
			free(nameBuffer);

		if (midiFileName)
			free(midiFileName);

		return;
	}

	if (buffer)
		free(buffer);
		
	if (nameBuffer)
		free(nameBuffer);

	if (midiFileName)
		free(midiFileName);

	return;
}

DWORD playmidithread(LPVOID param)
{
	HWND hwnd = (HWND) param;
	bMidiSeqThreadRunning=true;
	DWORD ppC = GetPriorityClass(GetCurrentProcess());

	if (ppC==0);

//	bPlayMidiFile = false;
//	bMidiFilePlaying=false;

	while (bMidiSeqThreadRunning)
	{
		if (bServiceCommand)
		{
			switch (dwSeqCmd)
			{
				case SEQCMD_PLAY:
					dwSeqCmdData = seqCurPlayListFile;
				case SEQCMD_JUMPTO:
					seqCurPlayListFile = dwSeqCmdData;
					bPlayMidiFile=true;
					SendDlgItemMessage(hwnd,BN_PLAYMIDIFILE,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_PAUSE));
					break;
				case SEQCMD_BACK:
					if (seqCurPlayListFile < dwSeqCmdData)
						seqCurPlayListFile = numPlayListFiles-1;
					seqCurPlayListFile-=dwSeqCmdData;
					break;
				case SEQCMD_FORWARD:
					seqCurPlayListFile+=dwSeqCmdData;
					if (seqCurPlayListFile > MAX_PLAYLISTFILES)
						seqCurPlayListFile=0;
					break;
				case SEQCMD_STOP:
					bPlayMidiFile=false;
					SendDlgItemMessage(hwnd,BN_PLAYMIDIFILE,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_PLAY));
					break;
				case SEQCMD_QUIT:
					bMidiSeqThreadRunning=false;
					break;
			}
			bServiceCommand=false;
		}

		if (bPlayMidiFile)
		{
			if (seqCurPlayListFile > MAX_PLAYLISTFILES || seqCurPlayListFile >= numPlayListFiles)
				seqCurPlayListFile=0;

			seqCurMidiFile = plf[seqCurPlayListFile].fileName;
			seqCurFileDir = DirList[ plf[seqCurPlayListFile].dirNum ];

			if (seqCurMidiFile == 0 || seqCurFileDir == 0)
			{
				bPlayMidiFile = false;
			}
			else
			{
				SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_HIGHEST);
				bMidiFilePlaying=true;
				playMidiFile(hwnd);
				if (!bPlayMidiFile)
					SendDlgItemMessage(hwnd,BN_PLAYMIDIFILE,BM_SETIMAGE,IMAGE_ICON,(LPARAM)LoadIcon(hInstance,(LPSTR) IDI_PLAY));

			}

			if (bPlayMidiFile && !bServiceCommand)
			seqCurPlayListFile++;
			bMidiFilePlaying=false;
			mmidiOutReset();
			SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_IDLE);
		}
	}
	return 0;
}

int GetTrackLength(TRACKDATA *trData,char *buf)
{
	TRACKDATA	trackData;
	char *buffer = buf;

	memcpy((char *) &trackData,(char *) trData,sizeof(TRACKDATA));
	unsigned char cmdBytes[8];
	while (trackData.curPosition < trackData.endPosition && trackData.nextEventTime != -1)
	{
		long varLen=0;
		getVarLength(buffer + trackData.curPosition,&varLen);
		trackData.curPosition+=varLen;
		char	*cmdPtr = (buffer + trackData.curPosition);

		cmdBytes[0] = *cmdPtr;
		int maskCmd = cmdBytes[0] & 0xF0;

		if ((maskCmd & 0x80) == 0)
		{
			maskCmd = trackData.lastCmd &0xF0;
			cmdBytes[0] = trackData.lastCmd;
			cmdBytes[1] = *(cmdPtr);
			cmdBytes[2] = *(cmdPtr+1);
			cmdBytes[3]	= 0;
		}

		trackData.lastCmd = cmdBytes[0];

		if (maskCmd >= 0x80 && maskCmd <= 0xEF)
		{

			if (*cmdPtr & 0x80)
			{
				cmdBytes[1] = *(cmdPtr+1);
				cmdBytes[2] = *(cmdPtr+2);
				cmdBytes[3] = 0;

				if (maskCmd == 0xC0 || maskCmd == 0xD0)
					trackData.curPosition+=2;
				else
					trackData.curPosition+=3;

			}
			else
			{
				if (maskCmd == 0xC0 || maskCmd == 0xD0)
					trackData.curPosition+=1;
				else
					trackData.curPosition+=2;
			}

			int midi_time = getVarLength(buffer + trackData.curPosition,0);
			trackData.curTime += trackData.nextEventTime;		//The event that just occured
			trackData.nextEventTime = midi_time;
		}
		else
		{
			switch (cmdBytes[0])
			{
				case 0xFF:
					cmdBytes[1] = *(cmdPtr+1);		//Command byte
					cmdBytes[2] = *(cmdPtr+2);	//Length byte

					switch (cmdBytes[1])
					{
						case 0x51:
						{
							int *tempox= (int *) &cmdBytes[4];
							cmdBytes[4] = *(cmdPtr+5);
							cmdBytes[5] = *(cmdPtr+4);
							cmdBytes[6] = *(cmdPtr+3);
	
							microSecPerQuarter = *(tempox) & 0xFFFFFF;
							setTempo();
							break;
						}
						case 0x7F:
							{
								int tempocx= microSecPerQuarter+2;
								if (Tempo)
									tempocx = 0;
							}
							break;
						default:
							break;
					}

					if (cmdBytes[1] != 0x2f)
					{
						int metaLen = getVarLength(buffer + trackData.curPosition+2,&varLen);
						trackData.curPosition += (metaLen+2+varLen);
						int midi_time = getVarLength(buffer+trackData.curPosition,0);
						trackData.curTime += trackData.nextEventTime;		//The event that just occured
						trackData.nextEventTime = midi_time;
					}
					else
						trackData.nextEventTime = -1;
			}
		}
	}
	return trackData.curTime;
}
