#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"




#ifdef SLBNSLB_Tmerge

//----------------------------------------------
//2SLB write to NSLB, both GC
//----------------------------------------------
void FASTmergeRWSLBpart5(sim *simPtr) {
	DWORD	k, RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo = 0, PageNo = 0;
	DWORD	j = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	int validIs2 = 0, validIs0 = 0, validIs1 = 0;


	SLB->mergecnt++;

	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWpage;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWpage + NC->trackSizePage - 1) % (SLB->partial_page + 1);

	int num = 0;
	assert(tsp_start <= tsp_end);

	for (i = tsp_start; i <= tsp_end; i++){	//merge valid page on the victim track of log buffer													//scan victim log block
		SectorTableNo = N->blocks[Victim].pages[i].sector / NC->pageSizeSector;	// check sectorTable is right

		if (!(tsp_start <= i && i <= tsp_end)){
			printf("%lu %lu %lu\n", tsp_start, i, tsp_end);
			system("pause");
		}

		if (N->blocks[Victim].pages[i].valid == 1 || N->blocks[Victim].pages[i].valid == 2) { // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].pages[i].sector) < NC->LsizePage);
			RWLBA = N->blocks[Victim].pages[i].sector / NC->blockSizeSector;		// logical address
			assert(RWLBA < NC->LsizeBlock);


			newBlock = dataBlock = N->L2Ptable[RWLBA]; //N->L2Ptable[RWLBA] = dataBlock; 

			validIs2 = 0; validIs0 = 0; validIs1 = 0;
			DWORD start = 0, end = 0;
			for (j = 0; j < NC->blockSizePage; j++) { //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
				j = NC->blockSizePage - 1;//speed up
				if (j == NC->blockSizePage - 1) { //最後1個page
					end = j;
					validPages = 0;		// temp use, for stat. of logblock use
					invalidPages = 0;
					int _ivp[8] = { 0 };
					for (j = start; j <= end; j++) { //sub-band的起點到終點
						if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;
							assert(getblock(simPtr, SectorTableNo) != Victim);
							SLB->pageRead++; SLB->pageWrite++;

							validIs1++;
						}
						else if (N->blocks[dataBlock].pages[j].valid == 2){
							NSLB->writedPageNum--;
							NSLB->pageRead++; NSLB->pageWrite++;

							validIs2++;
						}
						else{ //在log band or victim_backup 裡的dirty page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;

							//invalid在LB的該page 
							if (getblock(simPtr, dataBlock*NC->blockSizePage + j) >= NC->LsizeBlock && getblock(simPtr, dataBlock*NC->blockSizePage + j) < NC->PsizeBlock){
								N->blocks[getblock(simPtr, dataBlock*NC->blockSizePage + j)].pages[getpage(simPtr, dataBlock*NC->blockSizePage + j)].valid = 0;	//old變invalid
							}
							SLB->pageRead++; SLB->pageWrite++;

							validIs0++;
						}
						SectorTableNo = RWLBA*NC->blockSizePage + j;
						assert(SectorTableNo < NC->LsizePage);
						setblock(simPtr, SectorTableNo, newBlock);	//old 2 new
						setpage(simPtr, SectorTableNo, j);			//old 2 new
						assert(RWLBA*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
						N->blocks[newBlock].pages[j].sector = RWLBA*NC->blockSizeSector + j;	//new
						N->blocks[newBlock].pages[j].valid = 1;													//new變valid

						//both update
						/*NS->pageRead++; NS->pageWrite++;*/
					}


					break;
				}

			}
			N->blocks[Victim].pages[i].valid = 0;
			/*if (SectorTableNo == 553722000)
			{
			printf("SLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
#ifdef MergeAssociativity
			fprintf(fp_SLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
#endif
		}
		else {
			assert(SectorTableNo < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo) != Victim || getpage(simPtr, SectorTableNo) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].pages[i].valid == 0);
	}


	for (j = tsp_start; j <= tsp_end; j++){
		assert(N->blocks[Victim].pages[j].valid == 0);													//new變valid
	}

	//update SMR LB information
	SLB->firstRWpage = (SLB->firstRWpage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->writedPageNum -= (NC->trackSizePage);
}

void FASTmergeRWNSLBpart5(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	int validIs2 = 0, validIs0 = 0, validIs1 = 0;
	DWORD small = 0; //small time: means the band have the earliest time

	NSLB->mergecnt++;
	//printf("NSLB merge, go\n");
	do
	{
		small = 0;
		while (N->BPLRU[small] == 0) small++;

		if (small != NC->LsizeBlock - 1)
		{
			for (i = small + 1; i < NC->LsizeBlock; i++)
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] != 0) small = i;
			}
		}
		//printf("small = %lu\n", small);
		//for (i = 0; i < NC->LsizeBlock; i++) printf("%lu ", N->BPLRU[i]);
		//system("pause");

		for (i = 0; i < NC->LsizeBlock; i++)
		{
			if (N->BPLRU[i] != 0)
			{
				if (N->BPLRU[i] == N->BPLRU[small]) N->BPLRU[i] = 1;
				else N->BPLRU[i] -= N->BPLRU[small];
			}
		}
		N->cnt -= N->BPLRU[small];
		dataBlock = small;//BPLRU

		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		/*if (!(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1))
		{
		printf("dataBlock %lu\n", dataBlock);
		system("pause");
		}*/

		for (k = 0; k < NC->blockSizeSector; k++)
		{
			SectorTableNo = dataBlock*NC->blockSizeSector + k;

			//statistics
			if (N->blocks[dataBlock].pages[k].valid == 1){
				validIs1++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 2){
				validIs2++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 0){
				DWORD tmp_sector = SectorTableNo;
				assert(getblock(simPtr, tmp_sector) == NC->PsizeBlock - 1);
				assert(N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid == 1 || N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid == 2);
				assert(N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector == tmp_sector);
				N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid = 0;
				//N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector = 0x7fffffff;
				validIs0++;
			}

			N->blocks[dataBlock].pages[k].valid = 1;
			N->blocks[dataBlock].pages[k].sector = dataBlock*NC->blockSizeSector + k;

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu --> ", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/

			setblock(simPtr, SectorTableNo, dataBlock);
			setpage(simPtr, SectorTableNo, k);

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
		}
		N->BPLRU[small] = 0;
		//printf("NSLB merge, go 2, small = %lu\n",small);
	} while (validIs2 == 0);

	NSLB->pageRead += NC->blockSizePage;
	NSLB->pageWrite += NC->blockSizePage;

	NSLB->writedPageNum = NSLB->writedPageNum - (DWORD)validIs2;

#ifdef MergeAssoc_NSLBpart
	//FILE *fp = fopen("MA_NSLB.txt", "a");
	fprintf(fp_NSLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
	//fclose(fp);

#endif
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//3SLB write to NSLB, both GC
//----------------------------------------------
void FASTmergeRWSLBpart6(sim *simPtr) {
	DWORD	k, RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo = 0, PageNo = 0;
	DWORD	j = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	int validIs2 = 0, validIs0 = 0, validIs1 = 0;


	SLB->mergecnt++;

	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWpage;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWpage + NC->trackSizePage - 1) % (SLB->partial_page + 1);

	int num = 0;
	assert(tsp_start <= tsp_end);

	for (i = tsp_start; i <= tsp_end; i++){	//merge valid page on the victim track of log buffer													//scan victim log block
		SectorTableNo = N->blocks[Victim].pages[i].sector / NC->pageSizeSector;	// check sectorTable is right

		if (!(tsp_start <= i && i <= tsp_end)){
			printf("%lu %lu %lu\n", tsp_start, i, tsp_end);
			system("pause");
		}

		//decide the write times in SLB
		if (N->blocks[Victim].pages[i].valid == 1 || N->blocks[Victim].pages[i].valid == 2 || N->blocks[Victim].pages[i].valid == 3) { // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].pages[i].sector) < NC->LsizePage);
			RWLBA = N->blocks[Victim].pages[i].sector / NC->blockSizeSector;		// logical address
			assert(RWLBA < NC->LsizeBlock);


			newBlock = dataBlock = N->L2Ptable[RWLBA]; //N->L2Ptable[RWLBA] = dataBlock; 

			validIs2 = 0; validIs0 = 0; validIs1 = 0;
			DWORD start = 0, end = 0;
			for (j = 0; j < NC->blockSizePage; j++) { //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
				j = NC->blockSizePage - 1;//speed up
				if (j == NC->blockSizePage - 1) { //最後1個page
					end = j;
					validPages = 0;		// temp use, for stat. of logblock use
					invalidPages = 0;
					int _ivp[8] = { 0 };
					for (j = start; j <= end; j++) { //sub-band的起點到終點
						if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;
							assert(getblock(simPtr, SectorTableNo) != Victim);
							SLB->pageRead++; SLB->pageWrite++;

							validIs1++;
						}
						else if (N->blocks[dataBlock].pages[j].valid == 2){
							NSLB->writedPageNum--;
							NSLB->pageRead++; NSLB->pageWrite++;

							validIs2++;
						}
						else{ //在log band or victim_backup 裡的dirty page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;

							//invalid在LB的該page 
							if (getblock(simPtr, dataBlock*NC->blockSizePage + j) >= NC->LsizeBlock && getblock(simPtr, dataBlock*NC->blockSizePage + j) < NC->PsizeBlock){
								N->blocks[getblock(simPtr, dataBlock*NC->blockSizePage + j)].pages[getpage(simPtr, dataBlock*NC->blockSizePage + j)].valid = 0;	//old變invalid
							}
							SLB->pageRead++; SLB->pageWrite++;

							validIs0++;//validIs0 means the valid=1or2or3 page in SLB
						}
						SectorTableNo = RWLBA*NC->blockSizePage + j;
						assert(SectorTableNo < NC->LsizePage);
						setblock(simPtr, SectorTableNo, newBlock);	//old 2 new
						setpage(simPtr, SectorTableNo, j);			//old 2 new
						assert(RWLBA*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
						N->blocks[newBlock].pages[j].sector = RWLBA*NC->blockSizeSector + j;	//new
						N->blocks[newBlock].pages[j].valid = 1;													//new變valid

						//both update
						/*NS->pageRead++; NS->pageWrite++;*/
					}


					break;
				}

			}
			N->blocks[Victim].pages[i].valid = 0;
			/*if (SectorTableNo == 553722000)
			{
			printf("SLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
#ifdef MergeAssociativity
			fprintf(fp_SLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
#endif
		}
		else {
			assert(SectorTableNo < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo) != Victim || getpage(simPtr, SectorTableNo) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].pages[i].valid == 0);
	}


	for (j = tsp_start; j <= tsp_end; j++){
		assert(N->blocks[Victim].pages[j].valid == 0);													//new變valid
	}

	//update SMR LB information
	SLB->firstRWpage = (SLB->firstRWpage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->writedPageNum -= (NC->trackSizePage);
}

void FASTmergeRWNSLBpart6(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	int validIs2 = 0, validIs0 = 0, validIs1 = 0;
	DWORD small = 0; //small time: means the band have the earliest time

	NSLB->mergecnt++;
	//printf("NSLB merge, go\n");
	do
	{
		small = 0;
		while (N->BPLRU[small] == 0) small++;

		if (small != NC->LsizeBlock - 1)
		{
			for (i = small + 1; i < NC->LsizeBlock; i++)
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] != 0) small = i;
			}
		}
		//printf("small = %lu\n", small);
		//for (i = 0; i < NC->LsizeBlock; i++) printf("%lu ", N->BPLRU[i]);
		//system("pause");

		for (i = 0; i < NC->LsizeBlock; i++)
		{
			if (N->BPLRU[i] != 0)
			{
				if (N->BPLRU[i] == N->BPLRU[small]) N->BPLRU[i] = 1;
				else N->BPLRU[i] -= N->BPLRU[small];
			}
		}
		N->cnt -= N->BPLRU[small];
		dataBlock = small;//BPLRU

		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		/*if (!(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1))
		{
		printf("dataBlock %lu\n", dataBlock);
		system("pause");
		}*/

		for (k = 0; k < NC->blockSizeSector; k++)
		{
			SectorTableNo = dataBlock*NC->blockSizeSector + k;

			//statistics
			if (N->blocks[dataBlock].pages[k].valid == 1){
				validIs1++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 2){
				validIs2++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 0){
				DWORD tmp_sector = SectorTableNo;
				assert(getblock(simPtr, tmp_sector) == NC->PsizeBlock - 1);
				assert(N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid == 1 || N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid == 2 || N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid == 3);
				assert(N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector == tmp_sector);
				N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid = 0;
				//N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector = 0x7fffffff;
				validIs0++;//validIs0 means the valid=1or2or3 page in SLB
			}

			N->blocks[dataBlock].pages[k].valid = 1;
			N->blocks[dataBlock].pages[k].sector = dataBlock*NC->blockSizeSector + k;

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu --> ", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/

			setblock(simPtr, SectorTableNo, dataBlock);
			setpage(simPtr, SectorTableNo, k);

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
		}
		N->BPLRU[small] = 0;
		//printf("NSLB merge, go 2, small = %lu\n",small);
	} while (validIs2 == 0);

	NSLB->pageRead += NC->blockSizePage;
	NSLB->pageWrite += NC->blockSizePage;

	NSLB->writedPageNum = NSLB->writedPageNum - (DWORD)validIs2;

#ifdef MergeAssoc_NSLBpart
	//FILE *fp = fopen("MA_NSLB.txt", "a");
	fprintf(fp_NSLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
	//fclose(fp);

#endif
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//6SLB write to NSLB, both GC
//----------------------------------------------
void FASTmergeRWSLBpart9(sim *simPtr) {
	DWORD	k, RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo = 0, PageNo = 0;
	DWORD	j = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	int validIs2 = 0, validIs0 = 0, validIs1 = 0;


	SLB->mergecnt++;

	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWpage;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWpage + NC->trackSizePage - 1) % (SLB->partial_page + 1);

	int num = 0;
	assert(tsp_start <= tsp_end);

	for (i = tsp_start; i <= tsp_end; i++){	//merge valid page on the victim track of log buffer													//scan victim log block
		SectorTableNo = N->blocks[Victim].pages[i].sector / NC->pageSizeSector;	// check sectorTable is right

		if (!(tsp_start <= i && i <= tsp_end)){
			printf("%lu %lu %lu\n", tsp_start, i, tsp_end);
			system("pause");
		}

		//decide the write times in SLB
		if (1 <= N->blocks[Victim].pages[i].valid && N->blocks[Victim].pages[i].valid <= 6) { // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].pages[i].sector) < NC->LsizePage);
			RWLBA = N->blocks[Victim].pages[i].sector / NC->blockSizeSector;		// logical address
			assert(RWLBA < NC->LsizeBlock);


			newBlock = dataBlock = N->L2Ptable[RWLBA]; //N->L2Ptable[RWLBA] = dataBlock; 

			validIs2 = 0; validIs0 = 0; validIs1 = 0;
			DWORD start = 0, end = 0;
			for (j = 0; j < NC->blockSizePage; j++) { //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
				j = NC->blockSizePage - 1;//speed up
				if (j == NC->blockSizePage - 1) { //最後1個page
					end = j;
					validPages = 0;		// temp use, for stat. of logblock use
					invalidPages = 0;
					int _ivp[8] = { 0 };
					for (j = start; j <= end; j++) { //sub-band的起點到終點
						if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;
							assert(getblock(simPtr, SectorTableNo) != Victim);
							SLB->pageRead++; SLB->pageWrite++;

							validIs1++;
						}
						else if (N->blocks[dataBlock].pages[j].valid == 2){
							NSLB->writedPageNum--;
							NSLB->pageRead++; NSLB->pageWrite++;

							validIs2++;
						}
						else{ //在log band or victim_backup 裡的dirty page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;

							//invalid在LB的該page 
							if (getblock(simPtr, dataBlock*NC->blockSizePage + j) >= NC->LsizeBlock && getblock(simPtr, dataBlock*NC->blockSizePage + j) < NC->PsizeBlock){
								N->blocks[getblock(simPtr, dataBlock*NC->blockSizePage + j)].pages[getpage(simPtr, dataBlock*NC->blockSizePage + j)].valid = 0;	//old變invalid
							}
							SLB->pageRead++; SLB->pageWrite++;

							validIs0++;//validIs0 means the valid=1or2or3 page in SLB
						}
						SectorTableNo = RWLBA*NC->blockSizePage + j;
						assert(SectorTableNo < NC->LsizePage);
						setblock(simPtr, SectorTableNo, newBlock);	//old 2 new
						setpage(simPtr, SectorTableNo, j);			//old 2 new
						assert(RWLBA*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
						N->blocks[newBlock].pages[j].sector = RWLBA*NC->blockSizeSector + j;	//new
						N->blocks[newBlock].pages[j].valid = 1;													//new變valid

						//both update
						/*NS->pageRead++; NS->pageWrite++;*/
					}


					break;
				}

			}
			N->blocks[Victim].pages[i].valid = 0;
			/*if (SectorTableNo == 553722000)
			{
			printf("SLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
#ifdef MergeAssociativity
			fprintf(fp_SLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
#endif
		}
		else {
			assert(SectorTableNo < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo) != Victim || getpage(simPtr, SectorTableNo) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].pages[i].valid == 0);
	}


	for (j = tsp_start; j <= tsp_end; j++){
		assert(N->blocks[Victim].pages[j].valid == 0);													//new變valid
	}

	//update SMR LB information
	SLB->firstRWpage = (SLB->firstRWpage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->writedPageNum -= (NC->trackSizePage);
}

void FASTmergeRWNSLBpart9(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	int validIs2 = 0, validIs0 = 0, validIs1 = 0;
	DWORD small = 0; //small time: means the band have the earliest time

	NSLB->mergecnt++;
	//printf("NSLB merge, go\n");
	do
	{
		small = 0;
		while (N->BPLRU[small] == 0) small++;

		if (small != NC->LsizeBlock - 1)
		{
			for (i = small + 1; i < NC->LsizeBlock; i++)
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] != 0) small = i;
			}
		}
		//printf("small = %lu\n", small);
		//for (i = 0; i < NC->LsizeBlock; i++) printf("%lu ", N->BPLRU[i]);
		//system("pause");

		for (i = 0; i < NC->LsizeBlock; i++)
		{
			if (N->BPLRU[i] != 0)
			{
				if (N->BPLRU[i] == N->BPLRU[small]) N->BPLRU[i] = 1;
				else N->BPLRU[i] -= N->BPLRU[small];
			}
		}
		N->cnt -= N->BPLRU[small];
		dataBlock = small;//BPLRU

		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		/*if (!(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1))
		{
		printf("dataBlock %lu\n", dataBlock);
		system("pause");
		}*/

		for (k = 0; k < NC->blockSizeSector; k++)
		{
			SectorTableNo = dataBlock*NC->blockSizeSector + k;

			//statistics
			if (N->blocks[dataBlock].pages[k].valid == 1){
				validIs1++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 2){
				validIs2++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 0){
				DWORD tmp_sector = SectorTableNo;
				assert(getblock(simPtr, tmp_sector) == NC->PsizeBlock - 1);
				assert(1 <= N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid && N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid <= 6);
				assert(N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector == tmp_sector);
				N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid = 0;
				//N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector = 0x7fffffff;
				validIs0++;//validIs0 means the valid=1or2or3 page in SLB
			}

			N->blocks[dataBlock].pages[k].valid = 1;
			N->blocks[dataBlock].pages[k].sector = dataBlock*NC->blockSizeSector + k;

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu --> ", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/

			setblock(simPtr, SectorTableNo, dataBlock);
			setpage(simPtr, SectorTableNo, k);

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
		}
		N->BPLRU[small] = 0;
		//printf("NSLB merge, go 2, small = %lu\n",small);
	} while (validIs2 == 0);

	NSLB->pageRead += NC->blockSizePage;
	NSLB->pageWrite += NC->blockSizePage;

	NSLB->writedPageNum = NSLB->writedPageNum - (DWORD)validIs2;

#ifdef MergeAssoc_NSLBpart
	//FILE *fp = fopen("MA_NSLB.txt", "a");
	fprintf(fp_NSLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
	//fclose(fp);

#endif
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//8SLB write to NSLB, both GC
//----------------------------------------------
void FASTmergeRWSLBpart11(sim *simPtr) {
	DWORD	k, RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo = 0, PageNo = 0;
	DWORD	j = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	int validIs2 = 0, validIs0 = 0, validIs1 = 0;


	SLB->mergecnt++;

	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWpage;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWpage + NC->trackSizePage - 1) % (SLB->partial_page + 1);

	int num = 0;
	assert(tsp_start <= tsp_end);

	for (i = tsp_start; i <= tsp_end; i++){	//merge valid page on the victim track of log buffer													//scan victim log block
		SectorTableNo = N->blocks[Victim].pages[i].sector / NC->pageSizeSector;	// check sectorTable is right

		if (!(tsp_start <= i && i <= tsp_end)){
			printf("%lu %lu %lu\n", tsp_start, i, tsp_end);
			system("pause");
		}

		//decide the write times in SLB
		if (1 <= N->blocks[Victim].pages[i].valid && N->blocks[Victim].pages[i].valid <= 8) { // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].pages[i].sector) < NC->LsizePage);
			RWLBA = N->blocks[Victim].pages[i].sector / NC->blockSizeSector;		// logical address
			assert(RWLBA < NC->LsizeBlock);


			newBlock = dataBlock = N->L2Ptable[RWLBA]; //N->L2Ptable[RWLBA] = dataBlock; 

			validIs2 = 0; validIs0 = 0; validIs1 = 0;
			DWORD start = 0, end = 0;
			for (j = 0; j < NC->blockSizePage; j++) { //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
				j = NC->blockSizePage - 1;//speed up
				if (j == NC->blockSizePage - 1) { //最後1個page
					end = j;
					validPages = 0;		// temp use, for stat. of logblock use
					invalidPages = 0;
					int _ivp[8] = { 0 };
					for (j = start; j <= end; j++) { //sub-band的起點到終點
						if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;
							assert(getblock(simPtr, SectorTableNo) != Victim);
							SLB->pageRead++; SLB->pageWrite++;

							validIs1++;
						}
						else if (N->blocks[dataBlock].pages[j].valid == 2){
							NSLB->writedPageNum--;
							NSLB->pageRead++; NSLB->pageWrite++;

							validIs2++;
						}
						else{ //在log band or victim_backup 裡的dirty page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;

							//invalid在LB的該page 
							if (getblock(simPtr, dataBlock*NC->blockSizePage + j) >= NC->LsizeBlock && getblock(simPtr, dataBlock*NC->blockSizePage + j) < NC->PsizeBlock){
								N->blocks[getblock(simPtr, dataBlock*NC->blockSizePage + j)].pages[getpage(simPtr, dataBlock*NC->blockSizePage + j)].valid = 0;	//old變invalid
							}
							SLB->pageRead++; SLB->pageWrite++;

							validIs0++;//validIs0 means the valid=1or2or3 page in SLB
						}
						SectorTableNo = RWLBA*NC->blockSizePage + j;
						assert(SectorTableNo < NC->LsizePage);
						setblock(simPtr, SectorTableNo, newBlock);	//old 2 new
						setpage(simPtr, SectorTableNo, j);			//old 2 new
						assert(RWLBA*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
						N->blocks[newBlock].pages[j].sector = RWLBA*NC->blockSizeSector + j;	//new
						N->blocks[newBlock].pages[j].valid = 1;													//new變valid

						//both update
						/*NS->pageRead++; NS->pageWrite++;*/
					}


					break;
				}

			}
			N->blocks[Victim].pages[i].valid = 0;
			/*if (SectorTableNo == 553722000)
			{
			printf("SLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
#ifdef MergeAssociativity
			fprintf(fp_SLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
#endif
		}
		else {
			assert(SectorTableNo < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo) != Victim || getpage(simPtr, SectorTableNo) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].pages[i].valid == 0);
	}


	for (j = tsp_start; j <= tsp_end; j++){
		assert(N->blocks[Victim].pages[j].valid == 0);													//new變valid
	}

	//update SMR LB information
	SLB->firstRWpage = (SLB->firstRWpage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->writedPageNum -= (NC->trackSizePage);
}

void FASTmergeRWNSLBpart11(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	int validIs2 = 0, validIs0 = 0, validIs1 = 0;
	DWORD small = 0; //small time: means the band have the earliest time

	NSLB->mergecnt++;
	//printf("NSLB merge, go\n");
	do
	{
		small = 0;
		while (N->BPLRU[small] == 0) small++;

		if (small != NC->LsizeBlock - 1)
		{
			for (i = small + 1; i < NC->LsizeBlock; i++)
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] != 0) small = i;
			}
		}
		//printf("small = %lu\n", small);
		//for (i = 0; i < NC->LsizeBlock; i++) printf("%lu ", N->BPLRU[i]);
		//system("pause");

		for (i = 0; i < NC->LsizeBlock; i++)
		{
			if (N->BPLRU[i] != 0)
			{
				if (N->BPLRU[i] == N->BPLRU[small]) N->BPLRU[i] = 1;
				else N->BPLRU[i] -= N->BPLRU[small];
			}
		}
		N->cnt -= N->BPLRU[small];
		dataBlock = small;//BPLRU

		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		/*if (!(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1))
		{
		printf("dataBlock %lu\n", dataBlock);
		system("pause");
		}*/

		for (k = 0; k < NC->blockSizeSector; k++)
		{
			SectorTableNo = dataBlock*NC->blockSizeSector + k;

			//statistics
			if (N->blocks[dataBlock].pages[k].valid == 1){
				validIs1++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 2){
				validIs2++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 0){
				DWORD tmp_sector = SectorTableNo;
				assert(getblock(simPtr, tmp_sector) == NC->PsizeBlock - 1);
				assert(1 <= N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid && N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid <= 8);
				assert(N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector == tmp_sector);
				N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid = 0;
				//N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector = 0x7fffffff;
				validIs0++;//validIs0 means the valid=1or2or3 page in SLB
			}

			N->blocks[dataBlock].pages[k].valid = 1;
			N->blocks[dataBlock].pages[k].sector = dataBlock*NC->blockSizeSector + k;

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu --> ", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/

			setblock(simPtr, SectorTableNo, dataBlock);
			setpage(simPtr, SectorTableNo, k);

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
		}
		N->BPLRU[small] = 0;
		//printf("NSLB merge, go 2, small = %lu\n",small);
	} while (validIs2 == 0);

	NSLB->pageRead += NC->blockSizePage;
	NSLB->pageWrite += NC->blockSizePage;

	NSLB->writedPageNum = NSLB->writedPageNum - (DWORD)validIs2;

#ifdef MergeAssoc_NSLBpart
	//FILE *fp = fopen("MA_NSLB.txt", "a");
	fprintf(fp_NSLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
	//fclose(fp);

#endif
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//11SLB write to NSLB, both GC
//----------------------------------------------
void FASTmergeRWSLBpart13(sim *simPtr) {
	DWORD	k, RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo = 0, PageNo = 0;
	DWORD	j = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	int validIs2 = 0, validIs0 = 0, validIs1 = 0;


	SLB->mergecnt++;

	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWpage;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWpage + NC->trackSizePage - 1) % (SLB->partial_page + 1);

	int num = 0;
	assert(tsp_start <= tsp_end);

	for (i = tsp_start; i <= tsp_end; i++){	//merge valid page on the victim track of log buffer													//scan victim log block
		SectorTableNo = N->blocks[Victim].pages[i].sector / NC->pageSizeSector;	// check sectorTable is right

		if (!(tsp_start <= i && i <= tsp_end)){
			printf("%lu %lu %lu\n", tsp_start, i, tsp_end);
			system("pause");
		}

		//decide the write times in SLB
		if (1 <= N->blocks[Victim].pages[i].valid && N->blocks[Victim].pages[i].valid <= 10) { // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].pages[i].sector) < NC->LsizePage);
			RWLBA = N->blocks[Victim].pages[i].sector / NC->blockSizeSector;		// logical address
			assert(RWLBA < NC->LsizeBlock);


			newBlock = dataBlock = N->L2Ptable[RWLBA]; //N->L2Ptable[RWLBA] = dataBlock; 

			validIs2 = 0; validIs0 = 0; validIs1 = 0;
			DWORD start = 0, end = 0;
			for (j = 0; j < NC->blockSizePage; j++) { //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
				j = NC->blockSizePage - 1;//speed up
				if (j == NC->blockSizePage - 1) { //最後1個page
					end = j;
					validPages = 0;		// temp use, for stat. of logblock use
					invalidPages = 0;
					int _ivp[8] = { 0 };
					for (j = start; j <= end; j++) { //sub-band的起點到終點
						if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;
							assert(getblock(simPtr, SectorTableNo) != Victim);
							SLB->pageRead++; SLB->pageWrite++;

							validIs1++;
						}
						else if (N->blocks[dataBlock].pages[j].valid == 2){
							NSLB->writedPageNum--;
							NSLB->pageRead++; NSLB->pageWrite++;

							validIs2++;
						}
						else{ //在log band or victim_backup 裡的dirty page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;

							//invalid在LB的該page 
							if (getblock(simPtr, dataBlock*NC->blockSizePage + j) >= NC->LsizeBlock && getblock(simPtr, dataBlock*NC->blockSizePage + j) < NC->PsizeBlock){
								N->blocks[getblock(simPtr, dataBlock*NC->blockSizePage + j)].pages[getpage(simPtr, dataBlock*NC->blockSizePage + j)].valid = 0;	//old變invalid
							}
							SLB->pageRead++; SLB->pageWrite++;

							validIs0++;//validIs0 means the valid=1or2or3 page in SLB
						}
						SectorTableNo = RWLBA*NC->blockSizePage + j;
						assert(SectorTableNo < NC->LsizePage);
						setblock(simPtr, SectorTableNo, newBlock);	//old 2 new
						setpage(simPtr, SectorTableNo, j);			//old 2 new
						assert(RWLBA*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
						N->blocks[newBlock].pages[j].sector = RWLBA*NC->blockSizeSector + j;	//new
						N->blocks[newBlock].pages[j].valid = 1;													//new變valid

						//both update
						/*NS->pageRead++; NS->pageWrite++;*/
					}


					break;
				}

			}
			N->blocks[Victim].pages[i].valid = 0;
			/*if (SectorTableNo == 553722000)
			{
			printf("SLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
#ifdef MergeAssociativity
			fprintf(fp_SLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
#endif
		}
		else {
			assert(SectorTableNo < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo) != Victim || getpage(simPtr, SectorTableNo) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].pages[i].valid == 0);
	}


	for (j = tsp_start; j <= tsp_end; j++){
		assert(N->blocks[Victim].pages[j].valid == 0);													//new變valid
	}

	//update SMR LB information
	SLB->firstRWpage = (SLB->firstRWpage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->writedPageNum -= (NC->trackSizePage);
}

void FASTmergeRWNSLBpart13(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	int validIs2 = 0, validIs0 = 0, validIs1 = 0;
	DWORD small = 0; //small time: means the band have the earliest time

	NSLB->mergecnt++;
	//printf("NSLB merge, go\n");
	do
	{
		small = 0;
		while (N->BPLRU[small] == 0) small++;

		if (small != NC->LsizeBlock - 1)
		{
			for (i = small + 1; i < NC->LsizeBlock; i++)
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] != 0) small = i;
			}
		}
		//printf("small = %lu\n", small);
		//for (i = 0; i < NC->LsizeBlock; i++) printf("%lu ", N->BPLRU[i]);
		//system("pause");

		for (i = 0; i < NC->LsizeBlock; i++)
		{
			if (N->BPLRU[i] != 0)
			{
				if (N->BPLRU[i] == N->BPLRU[small]) N->BPLRU[i] = 1;
				else N->BPLRU[i] -= N->BPLRU[small];
			}
		}
		N->cnt -= N->BPLRU[small];
		dataBlock = small;//BPLRU

		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		/*if (!(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1))
		{
		printf("dataBlock %lu\n", dataBlock);
		system("pause");
		}*/

		for (k = 0; k < NC->blockSizeSector; k++)
		{
			SectorTableNo = dataBlock*NC->blockSizeSector + k;

			//statistics
			if (N->blocks[dataBlock].pages[k].valid == 1){
				validIs1++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 2){
				validIs2++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 0){
				DWORD tmp_sector = SectorTableNo;
				assert(getblock(simPtr, tmp_sector) == NC->PsizeBlock - 1);
				assert(1 <= N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid && N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid <= 10);
				assert(N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector == tmp_sector);
				N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid = 0;
				//N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector = 0x7fffffff;
				validIs0++;//validIs0 means the valid=1or2or3 page in SLB
			}

			N->blocks[dataBlock].pages[k].valid = 1;
			N->blocks[dataBlock].pages[k].sector = dataBlock*NC->blockSizeSector + k;

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu --> ", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/

			setblock(simPtr, SectorTableNo, dataBlock);
			setpage(simPtr, SectorTableNo, k);

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
		}
		N->BPLRU[small] = 0;
		//printf("NSLB merge, go 2, small = %lu\n",small);
	} while (validIs2 == 0);

	NSLB->pageRead += NC->blockSizePage;
	NSLB->pageWrite += NC->blockSizePage;

	NSLB->writedPageNum = NSLB->writedPageNum - (DWORD)validIs2;

#ifdef MergeAssoc_NSLBpart
	//FILE *fp = fopen("MA_NSLB.txt", "a");
	fprintf(fp_NSLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
	//fclose(fp);

#endif
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//12SLB write to NSLB, both GC
//----------------------------------------------
void FASTmergeRWSLBpart15(sim *simPtr) {
	DWORD	k, RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo = 0, PageNo = 0;
	DWORD	j = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	int validIs2 = 0, validIs0 = 0, validIs1 = 0;


	SLB->mergecnt++;

	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWpage;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWpage + NC->trackSizePage - 1) % (SLB->partial_page + 1);

	int num = 0;
	assert(tsp_start <= tsp_end);

	for (i = tsp_start; i <= tsp_end; i++){	//merge valid page on the victim track of log buffer													//scan victim log block
		SectorTableNo = N->blocks[Victim].pages[i].sector / NC->pageSizeSector;	// check sectorTable is right

		if (!(tsp_start <= i && i <= tsp_end)){
			printf("%lu %lu %lu\n", tsp_start, i, tsp_end);
			system("pause");
		}

		//decide the write times in SLB
		if (1 <= N->blocks[Victim].pages[i].valid && N->blocks[Victim].pages[i].valid <= 12) { // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].pages[i].sector) < NC->LsizePage);
			RWLBA = N->blocks[Victim].pages[i].sector / NC->blockSizeSector;		// logical address
			assert(RWLBA < NC->LsizeBlock);


			newBlock = dataBlock = N->L2Ptable[RWLBA]; //N->L2Ptable[RWLBA] = dataBlock; 

			validIs2 = 0; validIs0 = 0; validIs1 = 0;
			DWORD start = 0, end = 0;
			for (j = 0; j < NC->blockSizePage; j++) { //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
				j = NC->blockSizePage - 1;//speed up
				if (j == NC->blockSizePage - 1) { //最後1個page
					end = j;
					validPages = 0;		// temp use, for stat. of logblock use
					invalidPages = 0;
					int _ivp[8] = { 0 };
					for (j = start; j <= end; j++) { //sub-band的起點到終點
						if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;
							assert(getblock(simPtr, SectorTableNo) != Victim);
							SLB->pageRead++; SLB->pageWrite++;

							validIs1++;
						}
						else if (N->blocks[dataBlock].pages[j].valid == 2){
							NSLB->writedPageNum--;
							NSLB->pageRead++; NSLB->pageWrite++;

							validIs2++;
						}
						else{ //在log band or victim_backup 裡的dirty page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;

							//invalid在LB的該page 
							if (getblock(simPtr, dataBlock*NC->blockSizePage + j) >= NC->LsizeBlock && getblock(simPtr, dataBlock*NC->blockSizePage + j) < NC->PsizeBlock){
								N->blocks[getblock(simPtr, dataBlock*NC->blockSizePage + j)].pages[getpage(simPtr, dataBlock*NC->blockSizePage + j)].valid = 0;	//old變invalid
							}
							SLB->pageRead++; SLB->pageWrite++;

							validIs0++;//validIs0 means the valid=1or2or3 page in SLB
						}
						SectorTableNo = RWLBA*NC->blockSizePage + j;
						assert(SectorTableNo < NC->LsizePage);
						setblock(simPtr, SectorTableNo, newBlock);	//old 2 new
						setpage(simPtr, SectorTableNo, j);			//old 2 new
						assert(RWLBA*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
						N->blocks[newBlock].pages[j].sector = RWLBA*NC->blockSizeSector + j;	//new
						N->blocks[newBlock].pages[j].valid = 1;													//new變valid

						//both update
						/*NS->pageRead++; NS->pageWrite++;*/
					}


					break;
				}

			}
			N->blocks[Victim].pages[i].valid = 0;
			/*if (SectorTableNo == 553722000)
			{
			printf("SLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
#ifdef MergeAssociativity
			fprintf(fp_SLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
#endif
		}
		else {
			assert(SectorTableNo < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo) != Victim || getpage(simPtr, SectorTableNo) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].pages[i].valid == 0);
	}


	for (j = tsp_start; j <= tsp_end; j++){
		assert(N->blocks[Victim].pages[j].valid == 0);													//new變valid
	}

	//update SMR LB information
	SLB->firstRWpage = (SLB->firstRWpage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->writedPageNum -= (NC->trackSizePage);
}

void FASTmergeRWNSLBpart15(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	int validIs2 = 0, validIs0 = 0, validIs1 = 0;
	DWORD small = 0; //small time: means the band have the earliest time

	NSLB->mergecnt++;
	//printf("NSLB merge, go\n");
	do
	{
		small = 0;
		while (N->BPLRU[small] == 0) small++;

		if (small != NC->LsizeBlock - 1)
		{
			for (i = small + 1; i < NC->LsizeBlock; i++)
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] != 0) small = i;
			}
		}
		//printf("small = %lu\n", small);
		//for (i = 0; i < NC->LsizeBlock; i++) printf("%lu ", N->BPLRU[i]);
		//system("pause");

		for (i = 0; i < NC->LsizeBlock; i++)
		{
			if (N->BPLRU[i] != 0)
			{
				if (N->BPLRU[i] == N->BPLRU[small]) N->BPLRU[i] = 1;
				else N->BPLRU[i] -= N->BPLRU[small];
			}
		}
		N->cnt -= N->BPLRU[small];
		dataBlock = small;//BPLRU

		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		/*if (!(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1))
		{
		printf("dataBlock %lu\n", dataBlock);
		system("pause");
		}*/

		for (k = 0; k < NC->blockSizeSector; k++)
		{
			SectorTableNo = dataBlock*NC->blockSizeSector + k;

			//statistics
			if (N->blocks[dataBlock].pages[k].valid == 1){
				validIs1++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 2){
				validIs2++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 0){
				DWORD tmp_sector = SectorTableNo;
				assert(getblock(simPtr, tmp_sector) == NC->PsizeBlock - 1);
				assert(1 <= N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid && N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid <= 12);
				assert(N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector == tmp_sector);
				N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid = 0;
				//N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector = 0x7fffffff;
				validIs0++;//validIs0 means the valid=1or2or3 page in SLB
			}

			N->blocks[dataBlock].pages[k].valid = 1;
			N->blocks[dataBlock].pages[k].sector = dataBlock*NC->blockSizeSector + k;

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu --> ", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/

			setblock(simPtr, SectorTableNo, dataBlock);
			setpage(simPtr, SectorTableNo, k);

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
		}
		N->BPLRU[small] = 0;
		//printf("NSLB merge, go 2, small = %lu\n",small);
	} while (validIs2 == 0);

	NSLB->pageRead += NC->blockSizePage;
	NSLB->pageWrite += NC->blockSizePage;

	NSLB->writedPageNum = NSLB->writedPageNum - (DWORD)validIs2;

#ifdef MergeAssoc_NSLBpart
	//FILE *fp = fopen("MA_NSLB.txt", "a");
	fprintf(fp_NSLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
	//fclose(fp);

#endif
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//16SLB write to NSLB, both GC
//----------------------------------------------
void FASTmergeRWSLBpart19(sim *simPtr) {
	DWORD	k, RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo = 0, PageNo = 0;
	DWORD	j = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	int validIs2 = 0, validIs0 = 0, validIs1 = 0;


	SLB->mergecnt++;

	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWpage;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWpage + NC->trackSizePage - 1) % (SLB->partial_page + 1);

	int num = 0;
	assert(tsp_start <= tsp_end);

	for (i = tsp_start; i <= tsp_end; i++){	//merge valid page on the victim track of log buffer													//scan victim log block
		SectorTableNo = N->blocks[Victim].pages[i].sector / NC->pageSizeSector;	// check sectorTable is right

		if (!(tsp_start <= i && i <= tsp_end)){
			printf("%lu %lu %lu\n", tsp_start, i, tsp_end);
			system("pause");
		}

		//decide the write times in SLB
		if (1 <= N->blocks[Victim].pages[i].valid && N->blocks[Victim].pages[i].valid <= 16) { // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].pages[i].sector) < NC->LsizePage);
			RWLBA = N->blocks[Victim].pages[i].sector / NC->blockSizeSector;		// logical address
			assert(RWLBA < NC->LsizeBlock);


			newBlock = dataBlock = N->L2Ptable[RWLBA]; //N->L2Ptable[RWLBA] = dataBlock; 

			validIs2 = 0; validIs0 = 0; validIs1 = 0;
			DWORD start = 0, end = 0;
			for (j = 0; j < NC->blockSizePage; j++) { //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
				j = NC->blockSizePage - 1;//speed up
				if (j == NC->blockSizePage - 1) { //最後1個page
					end = j;
					validPages = 0;		// temp use, for stat. of logblock use
					invalidPages = 0;
					int _ivp[8] = { 0 };
					for (j = start; j <= end; j++) { //sub-band的起點到終點
						if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;
							assert(getblock(simPtr, SectorTableNo) != Victim);
							SLB->pageRead++; SLB->pageWrite++;

							validIs1++;
						}
						else if (N->blocks[dataBlock].pages[j].valid == 2){
							NSLB->writedPageNum--;
							NSLB->pageRead++; NSLB->pageWrite++;

							validIs2++;
						}
						else{ //在log band or victim_backup 裡的dirty page
							SectorTableNo = N->blocks[dataBlock].pages[j].sector / NC->pageSizeSector;

							//invalid在LB的該page 
							if (getblock(simPtr, dataBlock*NC->blockSizePage + j) >= NC->LsizeBlock && getblock(simPtr, dataBlock*NC->blockSizePage + j) < NC->PsizeBlock){
								N->blocks[getblock(simPtr, dataBlock*NC->blockSizePage + j)].pages[getpage(simPtr, dataBlock*NC->blockSizePage + j)].valid = 0;	//old變invalid
							}
							SLB->pageRead++; SLB->pageWrite++;

							validIs0++;//validIs0 means the valid=1or2or3 page in SLB
						}
						SectorTableNo = RWLBA*NC->blockSizePage + j;
						assert(SectorTableNo < NC->LsizePage);
						setblock(simPtr, SectorTableNo, newBlock);	//old 2 new
						setpage(simPtr, SectorTableNo, j);			//old 2 new
						assert(RWLBA*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
						N->blocks[newBlock].pages[j].sector = RWLBA*NC->blockSizeSector + j;	//new
						N->blocks[newBlock].pages[j].valid = 1;													//new變valid

						//both update
						/*NS->pageRead++; NS->pageWrite++;*/
					}


					break;
				}

			}
			N->blocks[Victim].pages[i].valid = 0;
			/*if (SectorTableNo == 553722000)
			{
			printf("SLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
#ifdef MergeAssociativity
			fprintf(fp_SLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
#endif
		}
		else {
			assert(SectorTableNo < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo) != Victim || getpage(simPtr, SectorTableNo) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].pages[i].valid == 0);
	}


	for (j = tsp_start; j <= tsp_end; j++){
		assert(N->blocks[Victim].pages[j].valid == 0);													//new變valid
	}

	//update SMR LB information
	SLB->firstRWpage = (SLB->firstRWpage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
	SLB->writedPageNum -= (NC->trackSizePage);
}

void FASTmergeRWNSLBpart19(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	int validIs2 = 0, validIs0 = 0, validIs1 = 0;
	DWORD small = 0; //small time: means the band have the earliest time

	NSLB->mergecnt++;
	//printf("NSLB merge, go\n");
	do
	{
		small = 0;
		while (N->BPLRU[small] == 0) small++;

		if (small != NC->LsizeBlock - 1)
		{
			for (i = small + 1; i < NC->LsizeBlock; i++)
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] != 0) small = i;
			}
		}
		//printf("small = %lu\n", small);
		//for (i = 0; i < NC->LsizeBlock; i++) printf("%lu ", N->BPLRU[i]);
		//system("pause");

		for (i = 0; i < NC->LsizeBlock; i++)
		{
			if (N->BPLRU[i] != 0)
			{
				if (N->BPLRU[i] == N->BPLRU[small]) N->BPLRU[i] = 1;
				else N->BPLRU[i] -= N->BPLRU[small];
			}
		}
		N->cnt -= N->BPLRU[small];
		dataBlock = small;//BPLRU

		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		/*if (!(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1))
		{
		printf("dataBlock %lu\n", dataBlock);
		system("pause");
		}*/

		for (k = 0; k < NC->blockSizeSector; k++)
		{
			SectorTableNo = dataBlock*NC->blockSizeSector + k;

			//statistics
			if (N->blocks[dataBlock].pages[k].valid == 1){
				validIs1++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 2){
				validIs2++;
			}

			if (N->blocks[dataBlock].pages[k].valid == 0){
				DWORD tmp_sector = SectorTableNo;
				assert(getblock(simPtr, tmp_sector) == NC->PsizeBlock - 1);
				assert(1 <= N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid && N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid <= 16);
				assert(N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector == tmp_sector);
				N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].valid = 0;
				//N->blocks[getblock(simPtr, tmp_sector)].pages[getpage(simPtr, tmp_sector)].sector = 0x7fffffff;
				validIs0++;//validIs0 means the valid=1or2or3 page in SLB
			}

			N->blocks[dataBlock].pages[k].valid = 1;
			N->blocks[dataBlock].pages[k].sector = dataBlock*NC->blockSizeSector + k;

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu --> ", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/

			setblock(simPtr, SectorTableNo, dataBlock);
			setpage(simPtr, SectorTableNo, k);

			/*if (SectorTableNo == 553722000)
			{
			printf("NSLB, merge to data band (%lu %lu)valid:%lu sector:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);

			}*/
		}
		N->BPLRU[small] = 0;
		//printf("NSLB merge, go 2, small = %lu\n",small);
	} while (validIs2 == 0);

	NSLB->pageRead += NC->blockSizePage;
	NSLB->pageWrite += NC->blockSizePage;

	NSLB->writedPageNum = NSLB->writedPageNum - (DWORD)validIs2;

#ifdef MergeAssoc_NSLBpart
	//FILE *fp = fopen("MA_NSLB.txt", "a");
	fprintf(fp_NSLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
	//fclose(fp);

#endif
}

//----------------------------------------------
//end
//----------------------------------------------
#endif


