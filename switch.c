#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"

void init(sim *simPtr)
{
	DWORD i;
	NFTL		*N = &(simPtr->NFTLobj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	/*N->st = (NFTLsectorElement_500Gup *)calloc(NC->LsizePage, sizeof(NFTLsectorElement_500Gup)); 
	assert(N->st != NULL);
	for (i = 0; i < NC->LsizePage; i++) {
		N->st[i].blockNo = i / NC->blockSizePage; assert(N->st[i].blockNo == i / NC->blockSizePage);
		N->st[i].pageNo = (i % NC->blockSizePage); assert(N->st[i].pageNo == (i % NC->blockSizePage));
	}*/
	N->st = (NFTLsectorElement_500Gup *)calloc(NC->LsizeSection, sizeof(NFTLsectorElement_500Gup));
	assert(N->st != NULL);
	for (i = 0; i < NC->LsizeSection; i++) {
		N->st[i].blockNo = i / NC->blockSizeSection; assert(N->st[i].blockNo == i / NC->blockSizeSection);
		N->st[i].sectionNo = (i % NC->blockSizeSection); assert(N->st[i].sectionNo == (i % NC->blockSizeSection));
	}
	printf("init ok\n");
}

void setsection(sim *simPtr, DWORD SectorTableNo, DWORD sectionNo)
{
	NFTL		*N = &(simPtr->NFTLobj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	N->st[SectorTableNo / NC->Section].sectionNo = sectionNo;

}

void setblock(sim *simPtr, DWORD SectorTableNo, DWORD blockNo)
{
	NFTL		*N = &(simPtr->NFTLobj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	N->st[SectorTableNo / NC->Section].blockNo = blockNo;
}

DWORD getsection(sim *simPtr, DWORD SectorTableNo)
{
	NFTL		*N = &(simPtr->NFTLobj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	return N->st[SectorTableNo / NC->Section].sectionNo;
}

DWORD getblock(sim *simPtr, DWORD SectorTableNo)
{
	NFTL		*N = &(simPtr->NFTLobj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	return N->st[SectorTableNo / NC->Section].blockNo;
}
