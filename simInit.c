#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"


void simInit(sim *simPtr)
{
	simStat			*SS = &(simPtr->simStatObj);
	simConfig		*SC	= &(simPtr->simConfigObj);

	memset(SS->resultTable,0,100*100*50*sizeof(BYTE));
	memset(SC->configTable,0,100*sizeof(configSlot));


	SS->totalReq		= 0;
	SS->readReq			= 0;
	SS->writeReq		= 0;
	SS->reqSectorWrite	= 0;
	SS->reqSectorRead	= 0;
	//SS->reqSectorWeight = 0;

	SC->logFp		= NULL;
}