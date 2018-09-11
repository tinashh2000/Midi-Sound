#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>
#include <prsht.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <richedit.h>
#include <malloc.h>
#include <mmsystem.h>

#include "../Res/resource.h"
#include "midiInstr.h"

#define	CLS_KEYINPUT	"MT_SOUNDSTUDIO_CLASS"

#define	MVP_PLAYNOTE	WM_USER+1000

#define	MAX_CHANNELS	16
#define MAX_KEYS		128
#define	MAX_TRACKS		512

typedef struct _Settings {
	bool	bShowDeletedEntries;
	bool	bNoSecurityPassword;
} Settings;

HINSTANCE	hInstance;
HMENU		hSSMenu;
HMENU		hKiMenu;

HWND		hwndMain;
int			nSaveMode;
int			nDeletedEntries=0;
Settings	settings;

RECT		mainRect;

HBRUSH RedBrush;
HBITMAP	hKeysBmp;

bool	mInOpen=false;
bool	mOutOpen=false;

WNDPROC		oldBnKeyInputProc;

DWORD	threadID;

#define VP_SPLIT	0x10

int keyStr[] = {'\0C','\0#C','\0D','\0#D','\0E','\0F','\0#F','\0G','\0#G','\0A','\0#A','\0B'};

typedef	struct _split {
	signed		int	Key;
	unsigned	int	Channel;
	signed		int	Velocity;
	unsigned	int	Instrument;
	signed		int	Transpose;
	signed		int	Modulation;
	signed		int	PitchBend;
}	SPLIT;

typedef struct _lastMouse {
	int	SplitKey;
	int	SplitChannel;
//	int	SplitVelocity;
	int SplitTranspose;
//	int SplitModulation;
//	int SplitPitchBend;
	int	NotePos;
	int	whichKey;
	bool	blackKey;
} LASTMOUSE;

typedef struct _channel {
	HWND	hwnd;
	bool	bShowMidiInput[MAX_CHANNELS];
	bool	bPlayMidiInput[MAX_CHANNELS];
	bool	bShowKeyboard;
	int		keyDown[MAX_KEYS];
	int		kKeyHeight;
	int		kKeyWidth;

	int		OutputDriver;
	char	*Buffer;
//	int	
}CHANNEL;

#define EVENT_RESET		0x00

#define EVENT_NOTEON	0x01
#define EVENT_NOTEOFF	0x02

#define EVENT_PITCHBEND	0x03

typedef struct _ssevent{
	int		msTime;
	int		dwChannel;
	int		dwEventData[2];
	short int		dwChannelType;
	short int		dwEventId;
}SSEVENT;

bool	bPlayChannel[MAX_CHANNELS];
bool	bAlwaysOnTop=true;
int		OutputChannel;

LASTMOUSE lastMouse;
SPLIT	split[10];
SPLIT	global;

CHANNEL	hKi[MAX_CHANNELS];


char	lpDefMidiInDevice[32]	= {0,0,0,0};
char	lpDefMidiOutDevice[32]	= {0,0,0,0};


bool	bTTSVPianoIn=false;
bool	bTTSVPianoOut=false;

int		dwTTSVPianoInDevice		= -1;
INT		dwDefMidiInDevice		= 0;

int		dwTTSVPianoOutDevice	= -1;
INT		dwDefMidiOutDevice		= 0;

int		curSplitPage		=	0;
HBRUSH hKiBrush[5];
//int		kKeyInputWidth;
//int		kKeyInputHeight;

int myVal=0;


char	KeyMap[512] = {'Q','2','W','3','E','R','5','T','6','Y','7','U','I','9','O','0','P',0xDB,0xBB,0xDD,0xDC,VK_BACK,VP_SPLIT,'Z','S','X','D','C','V','G','B','H','N','J','M',0xBC,'L',0xBE,0xBA,0xBF,0xDE};


bool bnKeyInputPainting=0;
void playKey(HWND hwnd,bool Release,int whichKey,int blackKey);
int	keyPress(HWND hwnd,int msg,WPARAM wParam,LPARAM lParam);
LRESULT	CALLBACK	bnKeyInputProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
int resetAllChannels();