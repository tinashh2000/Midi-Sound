#include "midi.h"
void CALLBACK MidiInputProc(HMIDIIN mIn,UINT msg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2);

void mmidiInstrument(HMIDIOUT hmOut,int Channel,int Instrument)
{
	midiOutShortMsg(hmOut,(Instrument << 8) | (0xC0 | (Channel & 0x0F)));
}

void mmidiOutKeyOn(int Channel,int Note,int Velocity,int Transpose)
{
	if (bTTSVPianoOut)
		midiInMessage(mTTSVPianoIn,0x4001,((Velocity << 16) + ((Note+Transpose) << 8) + 0x90) & 0xFFFFFF,0);
	else
		midiOutShortMsg(mOut, (Velocity << 16) | ((Note+Transpose) <<8) | (0x90 | (Channel & 0xFF)));
}

void mmidiOutKeyOff(int Channel,int Note,int Velocity,int Transpose)
{
	if (bTTSVPianoOut)
		midiInMessage(mTTSVPianoIn,0x4001,(((Note+Transpose) << 8) + 0x90 | (Channel & 0xFF) ) & 0xFFFFFF,0);
	else
		midiOutShortMsg(mOut, ((Note+Transpose) <<8) | (0x80 | (Channel & 0xFF)));
}

bool mmidiInReset()
{
	if (mInOpen)
		return (midiInReset(mIn)==0);
}

bool	mmidiInStart(HWND hwnd,int mDevNum)
{
	if (mInOpen)
	{
		midiInUnprepareHeader(mIn,&midiHDR,sizeof(midiHDR));
		midiInClose(mIn);
		mInOpen=false;
		bTTSVPianoIn=false;
	}


	int curDev=mDevNum;

	if (curDev == -1)
		int curDev=SendDlgItemMessage(hwnd,CB_MIDIINDEVICE,CB_GETCURSEL,0,0);

	if (!midiInOpen(&mIn,curDev,(DWORD) MidiInputProc,curDev,CALLBACK_FUNCTION))
	{
		bool	mError=false;
		mInOpen=true;
		midiHDR.lpData=(LPSTR) &sysInBuffer;
		midiHDR.dwBufferLength=sizeof(sysInBuffer);
		midiHDR.dwFlags=0;

		int retCode=midiInPrepareHeader(mIn,&midiHDR,sizeof(midiHDR));
		if (!retCode)
		{
			retCode=midiInAddBuffer(mIn,&midiHDR,sizeof(midiHDR));
			if (!retCode)
			{
				retCode=midiInStart(mIn);
				if (retCode!= MMSYSERR_NOERROR)
					mError=true;
			}
			else
				mError=true;
		}
		else
			mError=true;

		if (mError==true)
		{
			char errtxt[128];

			midiInGetErrorText(retCode,(char *) errtxt,128);
			midiInUnprepareHeader(mIn,&midiHDR,sizeof(midiHDR));
			midiInClose(mIn);
			mInOpen=false;
			bTTSVPianoIn=false;
		}
		else
		{
			SendDlgItemMessage(hwnd,BN_MIDIINSTART,WM_SETTEXT,0,(LPARAM) "Stop");

 			if (curDev==dwTTSVPianoInDevice)
				bTTSVPianoIn=true;
		}
	}


	if (mInOpen && bTTSVPianoIn)
	{
//		midiInMessage(mIn,0x4001,0x4000E0,0);	//Reset VPiano PitchBend
//		midiInMessage(mIn,0x4001,0x0001B0,0);	//Reset Modulation
//		midiInMessage(mIn,0x4001,0x644890,0);
	}
	return mInOpen;
}

bool mmidiInStop(HWND hwnd,HMIDIIN midiIn)
{
	if (midiIn==0 || (midiIn == mIn) )
	{
		if (mInOpen==true && !midiInClose(mIn))
		{
			mIn=0;
			mInOpen=false;
			SendDlgItemMessage(hwnd,BN_MIDIINSTART,WM_SETTEXT,0,(LPARAM) "Start");
			return true;
		}
	}
	else
		return (midiInClose(midiIn));

}


bool	mmidiOutStart(HWND hwnd,int mDevNum)
{
	if (mOutOpen)
	{
		if (!midiOutClose(mOut))
		{
			mOut=0;
			mOutOpen=false;
			bTTSVPianoOut=false;
		}
		else
			return false;
	}

	int curDev=mDevNum;

	if (curDev == -1)
		curDev=SendDlgItemMessage(hwnd,CB_MIDIOUTDEVICE,CB_GETCURSEL,0,0);

	{

		if (curDev==dwTTSVPianoOutDevice)
		{
			if (!midiInOpen(&mTTSVPianoIn,dwTTSVPianoInDevice,0,0,0))
			{
				bTTSVPianoOut=true;
				midiInMessage(mTTSVPianoIn,0x4001,0x4000E0,0);	//Reset VPiano PitchBend
				midiInMessage(mTTSVPianoIn,0x4001,0x0001B0,0);	//Reset Modulation
//				midiInMessage(mTTSVPianoIn,0x4001,0x644890,0);
				mOutOpen=true;
			}
		}
		else
		{
			if (!midiOutOpen(&mOut,curDev,0,0,0))
			{
				SendDlgItemMessage(hwnd,BN_MIDIOUTSTART,WM_SETTEXT,0,(LPARAM) "Stop");
				mOutOpen=true;
//				mOut=mOut;
			}
		}
	}

	return mOutOpen;
}

bool mmidiOutReset()
{
	if (mOutOpen)
		return (midiOutReset(mOut)==0);
}

bool mmidiOutStop(HWND hwnd,HMIDIOUT midiOut)
{
	if(midiOut==0 || (midiOut == mOut))
	{
		if (mOutOpen==true && !midiOutClose(mOut))
		{
			mOut=0;
			mOutOpen=false;
			SendDlgItemMessage(hwnd,BN_MIDIOUTSTART,WM_SETTEXT,0,(LPARAM) "Start");
			return true;
		}
	}
	else
		return (midiOutClose(midiOut));
}

int mmidiInChangeProgram(int msg)
{
	midiOutShortMsg(mOut,msg);
	return -1;
}

void	midiInDeviceChange(HWND hwnd)
{
	int	curDev=SendDlgItemMessage(hwnd,CB_MIDIINDEVICE,CB_GETCURSEL,0,0);
	mmidiInStart(hwnd,curDev);
}

void	midiOutDeviceChange(HWND hwnd)
{
	int	curDev=SendDlgItemMessage(hwnd,CB_MIDIOUTDEVICE,CB_GETCURSEL,0,0);
	mmidiOutStart(hwnd,curDev);
}

int mmidiInPlayNote(int msg)
{
	int notemsg = (msg & 0xF0);
	int channel = (msg & 0x0F);
	if (notemsg == 0x90 || notemsg == 0x80)
	{
		int	xNote		=	( (msg & 0xFF00) >> 8) & 0xFF;
		int xxNote		=	(xNote) % 12;
		int	xOctave		=	(xNote/12);
		bool	blackKey = ((xxNote<=4 && (xxNote & 1)) || ((xxNote >4) && ((xxNote+1) & 1)));
		
		if (xxNote <= 4)
			xxNote=xxNote/2;
		else
			xxNote=(xxNote+1)/2;

		xNote=(xOctave*7)+xxNote;

//		for (int count=0;count<MAX_CHANNELS;count++)
//		{
//			if (hKi[count].hwnd && hKi[count].bPlayMidiInput)
//			{
		if (bPlayChannel[channel])
		{
			if (notemsg == 0x90 && (msg & 0xFF0000))
				mmidiOutKeyOn(msg & 0x0F,(msg >> 8) & 0xFF,(msg >> 16) & 0xFF,0);
			else if (notemsg == 0x80 || (notemsg == 0x90 && !(msg&0xFF0000))) 
				mmidiOutKeyOff(msg & 0x0F,(msg >> 8) & 0xFF,(msg >> 16) & 0xFF,0);
		}

//			}
//		}

		if ((notemsg == 0x90 || notemsg == 0x80))
		{
			for (int count=0;count<MAX_CHANNELS;count++)
			{
				if (hKi[count].hwnd && hKi[count].bShowMidiInput[channel])
				{
					playKey(hKi[count].hwnd,(notemsg == 0x80 || (notemsg == 0x90 && !(msg&0xFF0000))),xNote,blackKey);
					break;
				}
			}
		}
	}
	return 1;
}

int	mmidiInMsg(int msg,int timestamp)
{
int mmsg=msg & 0xFFFFFF;
int thmsg=mmsg & 0xFF;
char	mystr[512];
	sprintf((char *) &mystr,"0000000%X",msg);
	int slen=strlen((char *) &mystr);
	if (slen > 8)
		strcpy((char *) &mystr,(char *) &mystr+(slen-8));

//	int curptr=SendDlgItemMessage(hwndMain,LST_REPORT,LB_ADDSTRING,0,(LPARAM) &mystr);
//	SendDlgItemMessage(hwndMain,LST_REPORT,LB_SETCURSEL,curptr,0);

	if (thmsg >= 0x80 && thmsg <=0x9F)
		return mmidiInPlayNote(mmsg);
	else if (thmsg >= 0xA0 && thmsg <=0xAF)
	{
		midiOutShortMsg(mOut,mmsg);
		return -1;
	}
	else if (thmsg >= 0xB0 && thmsg <=0xBF)
	{
		midiOutShortMsg(mOut,mmsg);
		return -1;
	}
	else if (thmsg >= 0xC0 && thmsg <=0xCF)
	{
		midiOutShortMsg(mOut,mmsg);
		return -1;
	}
	else if (thmsg >= 0xD0 && thmsg <=0xDF)
	{
		midiOutShortMsg(mOut,mmsg);
		return -1;
	}
	else if (thmsg >= 0xE0 && thmsg <=0xEF)
	{
		midiOutShortMsg(mOut,mmsg);
		return -1;
	}
	
	return 1;
}

void CALLBACK MidiInputProc(HMIDIIN mIn,UINT msg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
char myBuf[512];
MMRESULT	retCode;
LPMIDIHDR		lpMIDIHeader;
unsigned char *	ptr;
TCHAR			buffer[80];
unsigned char 	bytes;

	switch (msg)
	{
		case MIM_OPEN:
			break;
		case MIM_CLOSE:
//			midiInStop(mIn);
			break;
		case MIM_DATA:
			mmidiInMsg(dwParam1,dwParam2);
			break;
		case MIM_LONGDATA:
			break;

			/* If this application is ready to close down, then don't midiInAddBuffer() again */
				lpMIDIHeader = (LPMIDIHDR)dwParam1;

				ptr = (unsigned char *)(lpMIDIHeader->lpData);

				midiInAddBuffer(mIn, lpMIDIHeader, sizeof(MIDIHDR));

			break;
		default:
			MessageBox(0,"Data","Data",0);
	}
	return;
}
#include "playmidi.hpp"
#include "midiProc.hpp"
