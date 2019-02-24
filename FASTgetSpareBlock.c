#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"


//DWORD FASTgetSpareBlock(sim *simPtr) { // get a spare block
//	DWORD	ret;
//	NFTL			*N	= &(simPtr->NFTLobj);
//	NFTLstat		*NS = &(simPtr->NFTLobj.statObj);
//	ret = N->firstSpareBlock;
//	N->firstSpareBlock = N->repTable[N->firstSpareBlock];	// point to the next spare block
//	if (N->firstSpareBlock == MLC_LOGBLOCK_NULL) N->lastSpareBlock = N->firstSpareBlock;
//	assert(ret != MLC_LOGBLOCK_NULL);
//	N->repTable[ret] = MLC_LOGBLOCK_NULL;
//	return ret;
//}