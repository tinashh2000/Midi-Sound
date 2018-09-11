int	Get3StateCheckValue(int Value,int forTrue,int forFalse)
{
	if (Value==forTrue)
		return BST_CHECKED;
	else if (Value==forFalse)
		return BST_UNCHECKED;

	return BST_INDETERMINATE;
}

int	setInstrument(HWND hwnd)
{
	int	curInstrument=SendDlgItemMessage(hwnd,CB_INSTRUMENT,CB_GETCURSEL,0,0);
	int	curChannel=SendDlgItemMessage(hwnd,CB_CHANNEL2,CB_GETCURSEL,0,0);
	mmidiInstrument(mOut,curChannel,curInstrument);
	return -1;
}

void	setChannelPresets(HWND hwnd)
{
	/*
int curChannel=SendDlgItemMessage(hwnd,CB_CHANNEL2,CB_GETCURSEL,0,0);

	if (curChannel==0)
	{
		int	ShowKeys=0;
		int	PlayChannel=0;
		for (int count=0;count<16;count++)
		{
			ShowKeys=+ (bShowChannelKeys[count]?1:0);
			PlayChannel=+ (bShowChannelKeys[count]?1:0);
		}

		ShowKeys=Get3StateCheckValue(ShowKeys,16,0);
		PlayChannel=Get3StateCheckValue(PlayChannel,16,0);
	}
	else
	{
		SendDlgItemMessage(hwnd,CH_SHOWCHANNELKEYS,BM_SETCHECK,bShowChannelKeys[curChannel-1] ? BST_CHECKED:BST_UNCHECKED,0);
		SendDlgItemMessage(hwnd,CH_PLAYCHANNEL,BM_SETCHECK,bPlayChannel[curChannel-1] ? BST_CHECKED:BST_UNCHECKED,0);
	}
	return;

	*/
}

void	setShowChannelKeys(HWND hwnd)
{
/*
	int curChannel=SendDlgItemMessage(hwnd,CB_CHANNEL2,CB_GETCURSEL,0,0);
	bool checkState=(SendDlgItemMessage(hwnd,CH_SHOWCHANNELKEYS,BM_GETCHECK,0,0)==BST_CHECKED) ?true:false;
	if (curChannel==0)
	{
		for (int count=0;count<16;count++)
		{
			bShowChannelKeys[count]=checkState;
		}
	}
	else
		bShowChannelKeys[curChannel-1]=checkState;
*/
}

void	setPlayChannel(HWND hwnd)
{
/*
	int curChannel=SendDlgItemMessage(hwnd,CB_CHANNEL2,CB_GETCURSEL,0,0);
	bool checkState=(SendDlgItemMessage(hwnd,CH_PLAYCHANNEL,BM_GETCHECK,0,0)==BST_CHECKED) ?true:false;
	if (curChannel==0)
	{
		for (int count=0;count<16;count++)
		{
			bPlayChannel[count]=checkState;
		}
	}
	else
		bPlayChannel[curChannel-1]=checkState;
*/
}

void	setGlobalTranspose(HWND hwnd,int newValue)
{
char	numBuf[12];

	if (newValue > 255)
		newValue = 255;

	global.Transpose = newValue;
	itoa(global.Transpose,(char *) &numBuf,10);
	SendDlgItemMessage(hwnd,ED_GBLTRANSPOSE,WM_SETTEXT,0,(LPARAM) &numBuf);

	int newPos = SendDlgItemMessage(hwnd,TR_GBLTRANSPOSE,TBM_GETPOS,0,0);

	if (newPos-128 != newValue)
	{
		SendDlgItemMessage(hwnd,TR_GBLTRANSPOSE,TBM_SETPOS,true,newValue+128);
	}
}

void	setGlobalVelocity(HWND hwnd,int newValue)
{
char	numBuf[12];

	if (newValue > 255)
		newValue = 255;

	global.Velocity = newValue;
	itoa(global.Velocity,(char *) &numBuf,10);
	SendDlgItemMessage(hwnd,ED_GBLVELOCITY,WM_SETTEXT,0,(LPARAM) &numBuf);

	int newPos = SendDlgItemMessage(hwnd,TR_GBLVELOCITY,TBM_GETPOS,0,0);
	if (newPos-128 != newValue)
	{
		SendDlgItemMessage(hwnd,TR_GBLVELOCITY,TBM_SETPOS,true,newValue+128);
	}
}

void	setGlobalModulation(HWND hwnd,int newValue)
{
char	numBuf[12];

	if (newValue > 255)
		newValue = 255;

	global.Modulation = newValue;
	itoa(global.Modulation,(char *) &numBuf,10);
	SendDlgItemMessage(hwnd,ED_GBLMODULATION,WM_SETTEXT,0,(LPARAM) &numBuf);

	int newPos = SendDlgItemMessage(hwnd,TR_GBLMODULATION,TBM_GETPOS,0,0);

	if (newPos-128 != newValue)
	{
		SendDlgItemMessage(hwnd,TR_GBLMODULATION,TBM_SETPOS,true,newValue+128);
	}
}

void	setGlobalPitchBend(HWND hwnd,int newValue)
{
char	numBuf[12];

	if (newValue > 255)
		newValue = 255;

	global.PitchBend = newValue;
	itoa(global.PitchBend,(char *) &numBuf,10);
	SendDlgItemMessage(hwnd,ED_GBLPITCHBEND,WM_SETTEXT,0,(LPARAM) &numBuf);

	int newPos = SendDlgItemMessage(hwnd,TR_GBLPITCHBEND,TBM_GETPOS,0,0);
	if (newPos-128 != newValue)
	{
		SendDlgItemMessage(hwnd,TR_GBLPITCHBEND,TBM_SETPOS,true,newValue+128);
	}
}

void	setChannel(HWND hwnd,int newValue)
{
	if (newValue >= MAX_CHANNELS)
		newValue=MAX_CHANNELS-1;

	split[curSplitPage].Channel=newValue;
	SendDlgItemMessage(hwnd,CB_CHANNEL,CB_SETCURSEL,newValue,0);
	return;
}
void	setTranspose(HWND hwnd,int newValue)
{
char	numBuf[12];

	if (newValue > 255)
		newValue = 255;

	split[curSplitPage].Transpose = newValue;
	itoa(split[curSplitPage].Transpose,(char *) &numBuf,10);
	SendDlgItemMessage(hwnd,ED_TRANSPOSE,WM_SETTEXT,0,(LPARAM) &numBuf);

	int newPos = SendDlgItemMessage(hwnd,TR_TRANSPOSE,TBM_GETPOS,0,0);

	if (newPos-128 != newValue)
	{
		SendDlgItemMessage(hwnd,TR_TRANSPOSE,TBM_SETPOS,true,newValue+128);
	}
}

void	setVelocity(HWND hwnd,int newValue)
{
char	numBuf[12];

	if (newValue > 255)
		newValue = 255;

	split[curSplitPage].Velocity = newValue;
	itoa(split[curSplitPage].Velocity,(char *) &numBuf,10);
	SendDlgItemMessage(hwnd,ED_VELOCITY,WM_SETTEXT,0,(LPARAM) &numBuf);

	int newPos = SendDlgItemMessage(hwnd,TR_VELOCITY,TBM_GETPOS,0,0);
	if (newPos-128 != newValue)
	{
		SendDlgItemMessage(hwnd,TR_VELOCITY,TBM_SETPOS,true,newValue+128);
	}
}
void	setModulation(HWND hwnd,int newValue)
{
char	numBuf[12];

	if (newValue > 255)
		newValue = 255;

	split[curSplitPage].Modulation = newValue;
	itoa(split[curSplitPage].Modulation,(char *) &numBuf,10);
	SendDlgItemMessage(hwnd,ED_MODULATION,WM_SETTEXT,0,(LPARAM) &numBuf);

	int newPos = SendDlgItemMessage(hwnd,TR_MODULATION,TBM_GETPOS,0,0);

	if (newPos-128 != newValue)
	{
		SendDlgItemMessage(hwnd,TR_MODULATION,TBM_SETPOS,true,newValue+128);
	}
}
void	setPitchBend(HWND hwnd,int newValue)
{
char	numBuf[12];

	if (newValue > 255)
		newValue = 255;

	split[curSplitPage].PitchBend = newValue;
	itoa(split[curSplitPage].PitchBend,(char *) &numBuf,10);
	SendDlgItemMessage(hwnd,ED_PITCHBEND,WM_SETTEXT,0,(LPARAM) &numBuf);

	int newPos = SendDlgItemMessage(hwnd,TR_PITCHBEND,TBM_GETPOS,0,0);
	if (newPos-128 != newValue)
	{
		SendDlgItemMessage(hwnd,TR_PITCHBEND,TBM_SETPOS,true,newValue+128);
	}
}

void	setSplitKey(HWND hwnd,int newValue)
{
char	numBuf[12];

	split[curSplitPage].Key = newValue;
	int TheOctave=newValue/12;
	int TheKey=newValue % 12;
	sprintf( (char*) &numBuf,"%s%d",&keyStr[TheKey],TheOctave);
	SendDlgItemMessage(hwnd,ED_SPLITKEY,WM_SETTEXT,0,(LPARAM) &numBuf);

	int newPos = SendDlgItemMessage(hwnd,TR_SPLITSTART,TBM_GETPOS,0,0);

	if (newPos != newValue)
	{
		SendDlgItemMessage(hwnd,TR_SPLITSTART,TBM_SETPOS,true,newValue);
	}
}
