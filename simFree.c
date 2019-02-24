#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"
void simFree(sim *simPtr) {
	if(strcmp(simPtr->simConfigObj.NFTL_algo,"FAST")==0)
		FASTfree(simPtr);
}