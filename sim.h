


// simulation configurtation table (and its entries)

typedef struct
{
	BYTE	section[20];	// configuration section name 
	BYTE	string[100];	// slot name
	BYTE	value[100];		// string, not converted yet
	BYTE	unit[100];		// currently: K, M, or G
}configSlot;

typedef struct 
{
	configSlot	configTable[100];
	BYTE		simTimeStamp[100];
	BYTE		configFile[100];// the experiment configuration file
	FILE		*configFp;	
	BYTE		logFile[100];	// the log file
	FILE		*logFp;		
	BYTE		traceFile[100];	// the trace file
	FILE		*traceFp;
	DWORD		traceReplay;	// # of replaying the input trace
	BYTE		NFTL_algo[100]; // using NFTL algo name
	BYTE		Buf_algo[100];  // using Buffer algo name
	DWORD		replayReq;	
}simConfig;

//
// ============= simulation statistics
//
typedef struct{
	//DWORD	totalReq;
	//DWORD	writeReq;
	//DWORD	readReq;
	I64	totalReq;
	I64	writeReq;
	I64		reqSectorWrite;
	//I64     reqSectorWeight;
	DWORD reqSectorRead_max;
	I64 readReq;
	I64 reqSectorRead;
	BYTE	resultTable[100][100][50];	// a 100*100 table, each slot is a 50-char string

}simStat;

typedef struct{
	//buffer		bufferObj;
	NFTL		NFTLobj;
	simStat		simStatObj;
	simConfig	simConfigObj;
}sim;







BYTE simGetConfig(simConfig *C);
double simGetDouble(simConfig *C,BYTE *section,BYTE *string);
DWORD simGetDWORD(simConfig *C,BYTE *section,BYTE *string);
double simGetDouble(simConfig *C,BYTE *section,BYTE *string);
I64 simGetI64(simConfig *C,BYTE *section,BYTE *string);
BYTE *simGetString(simConfig *C,BYTE *section,BYTE *string);

