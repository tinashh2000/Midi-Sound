int			nMidiInDevs=0;
int			nMidiOutDevs=0;
char		sysInBuffer[256];
int			sysInX;

MIDIHDR		midiHDR;
HMIDIIN		mIn;
HMIDIIN		mTTSVPianoIn;
HMIDIOUT	mOut;

typedef struct __midiFileHDR {
	char	signature[4];
	DWORD	headerSize;
	WORD	fileFormat;
	WORD	numTracks;
	unsigned short int	deltaTime;
} MIDIFILEHDR;

typedef struct __midiTrackHDR {
	char	signature[4];
	unsigned	int	trackLength;
} MIDITRACKHDR;

typedef	struct	_TRACKDATA {
	unsigned int	lastCmd;			//Last command
	unsigned int	startPosition;		//Start position
	unsigned int	endPosition;		//End Position
	unsigned int	curPosition;		//Position in bytes
	unsigned int	trackLength;		//Length in bytes

	unsigned int	totalTime;			//Running time for the track
	unsigned int	curTime;			//Time before next event
	unsigned int	nextEventTime;		//Time at which this event will take place
	unsigned int	TrackNumber;		//Track on which the event will occur

} TRACKDATA;

/*
typedef	struct	_nextEvent {
} TRACKSTATUS;

/*
typedef	struct _track {
	TRACKSTATE	Tracks;			//The state of events
	TRACKSTATUS	NextEvent;			//Next Event
} TRACKDATA;
*/

TRACKDATA Tracks[MAX_TRACKS];

TRACKDATA *EventsOrder[MAX_TRACKS];

#define MIDI_FILEERROR		1
#define MIDI_INVALIDHEADER	2
#define MIDI_MEMORYERROR	3
#define MIDI_INVALIDFILE	4

#define SEQCMD_BACK			0x001		//Interrupts current track from playing
#define SEQCMD_FORWARD		0x002
#define	SEQCMD_PLAY			0x003
#define SEQCMD_STOP			0x004
#define SEQCMD_JUMPTO		0x005
#define SEQCMD_QUIT			0x006

#define SEQCMD_PLAYBACK		0x100		//
#define SEQCMD_TRANSPOSE	0x100		//Applies to currently playing track
#define	SEQCMD_SEEK			0x101
#define SEQCMD_REW			0x102
#define SEQCMD_FF			0x103
#define SEQCMD_TRANSPOSEUP		0x104
#define SEQCMD_TRANSPOSEDOWN	0x105
#define	SEQ_WINTEXT			"Midi Sequencer"

bool bServiceCommand=false;
bool bCommandLock=false;
bool bPlayMidiFile=false;
bool bMidiFilePlaying=false;

// bool bMidiThreadStarted=false;
bool bMidiSeqThreadRunning=false;
unsigned __int32	trSeekMaxValue	=	36000;
unsigned __int32	dwSeqCmd;
unsigned __int32	dwSeqCmdData;

float	Tempo;			//Beats per minute
//int	milliPerTick;		//Milli-seconds per tick

float	midiSpeed=1;
int		seqTranspose=0;
int		ticksPerQuarter;
int		microSecPerQuarter;
float	ticksPerSecond;
float	milliSecPerTick;
bool	bPause;
HWND	seqHwnd;
#include "seqTool.h"
void setTempo()
{
	Tempo=60000000/microSecPerQuarter;
	milliSecPerTick=((float)60000/(Tempo*ticksPerQuarter));
	return;
}

