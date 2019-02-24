#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"

// Convert (align) a write to sectors to a write to pages

// if the request is not aligned (starting address or ending address)
// note: we have to prevent the following condition occurs:
// [--00][0000][00--]
// [----][----][--00][0000][00--]
//
// [0000][0000][0000]
// [----][----][0000][0000][0000]
// 
// this causes very strange behaviors, since each sequential &bulk write invalidates *one* AU.
// The correct way to simulate this is:
//
// [--00][0000][00--]
// [----][----][--00][0000][00--]
//
// [0000][0000][----]  
// [----][----][0000][0000][----]
// 

void FASTconvertReq(sim *simPtr,DWORD *offsetSector,DWORD *lenSector)
{
	NFTLconfig		*NC = &(simPtr->NFTLobj.configObj);

	*offsetSector = *offsetSector/(NC->pageSizeSector);

	if((*lenSector) % NC->pageSizeSector != 0)
		*lenSector = 1 + *lenSector/NC->pageSizeSector;
	else
		*lenSector = *lenSector/NC->pageSizeSector;

	*offsetSector	*= (NC->pageSizeSector);
	*lenSector		*= (NC->pageSizeSector);
}