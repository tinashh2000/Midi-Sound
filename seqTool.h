#define MAX_PLAYLISTDIRS	65536
#define MAX_PLAYLISTFILES	65536

typedef struct _PLAYLISTFILE {
	int		dirNum;
	char	*fileName;
} PLAYLISTFILE;

PLAYLISTFILE	plf[MAX_PLAYLISTFILES];
char			*DirList[MAX_PLAYLISTDIRS];

int numPlayListFiles=0;
int numPlayListDirs=0;
char	seqFileBuffer[1048576];
int		seqPlayList[65536*2];

int		seqCurPlayListFile=0;
char	*seqCurMidiFile=0;
char	*seqCurFileDir=0;
