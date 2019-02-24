#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"





#ifdef SLBNSLB_Tmerge
//----------------------------------------------
//2SLB write to NSLB, both GC
//----------------------------------------------
void FASTwriteSectorSLBpart5(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, BlockNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD i = 0;
	DWORD SectorTableNo = 0, PageNo = 0;


	/*if (SectorTableNo == 553722000)
	{
	printf("in SLB write: (%lu %I64u) valid:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid);
	}*/


	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWpage == SLB->guardTrack1stPage)//當前空間已寫滿
		{
			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage)//發現guard track右邊是fRW，只能merge
			{
				//printf("SLB->writedPageNum = %lu, SLB->SLBpage_num = %lu, NC->trackSizePage = %lu %lu\n", SLB->writedPageNum, SLB->SLBpage_num, NC->trackSizePage, SLB->SLBpage_num - NC->trackSizePage);
				assert(SLB->writedPageNum == SLB->SLBpage_num);

				//for single SLB
				//FASTmergeRWSLBpart(simPtr);

				//for SLBNSLB
				FASTmergeRWSLBpart5(simPtr);
			}
			else//guard track右邊不是fRW，右移gt一格
			{
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
			}

		}

		assert(SLB->lastRWpage >= 0);
		assert(SLB->lastRWpage <= SLB->partial_page);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);

		//invalid the same page in log buffer or in data band
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)//in LB
		{
			//assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1);
			//N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
			//N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
			//N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 2;

			assert(0 <= getpage(simPtr, SectorTableNo) && getpage(simPtr, SectorTableNo) <= SLB->partial_page);
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid != 0);
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 2;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 2)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 2;
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)//in Data Band, first time write
		{
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1);
			N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old

			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;
		}
		else
		{
			printf("write error\n");
			system("pause");
		}

		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setpage(simPtr, SectorTableNo, SLB->lastRWpage);

		SLB->lastRWpage = (SLB->lastRWpage + 1) % (SLB->partial_page + 1);
		SLB->pageWrite++;
		SLB->writedPageNum++;

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorNSLBpart5(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//if (SectorTableNo == 553722000)//SLB寫過也改了setblock setpage，為何在NSLB裡還是找到1056 73872
	//{
	//	printf("init NSLB, getblock(simPtr, SectorTableNo):%lu ,getpage(simPtr, SectorTableNo):%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo));
	//}
	//if (SectorTableNo == 203944120)
	//{
	//	printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//	//N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;
	//	//printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//}

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//merge the BPLRU band
		N->cnt++;
		N->BPLRU[LBA] = N->cnt;

		if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1 && N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 2)
		{

		}
		else if (NSLB->writedPageNum >= NSLB->NSLBpage_num)//當前空間已寫滿
		{
			FASTmergeRWNSLBpart5(simPtr);
			assert(NSLB->writedPageNum < NSLB->NSLBpage_num);
		}
		

		DWORD page_no = getpage(simPtr, SectorTableNo);
		DWORD old_sector = 0;
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)
		{
			if (0 <= page_no && page_no <= SLB->partial_page)//move from SLB to NSLB
			{
				old_sector = N->blocks[NC->PsizeBlock - 1].pages[page_no].sector;
				assert(old_sector == SectorTableNo);
				assert(0 <= old_sector && old_sector <= NC->LsizeSector);
				//free SLB page
				assert(N->blocks[NC->PsizeBlock - 1].pages[page_no].valid == 2);

				N->blocks[NC->PsizeBlock - 1].pages[page_no].valid = 0;
				//N->blocks[NC->PsizeBlock - 1].pages[page_no].sector = 0;
				//set data band's page to original mapping
				setblock(simPtr, old_sector, old_sector / NC->blockSizeSector);
				setpage(simPtr, old_sector, old_sector % NC->blockSizeSector);
				//NSLB mapping is valid == 2
				assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 0);
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector = SectorTableNo;
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;

				NSLB->pageRead++;
				NSLB->pageWrite++;
				NSLB->writedPageNum++;
			}
			else if (SLB->partial_page + 1 <= page_no && page_no <= NSLB->partial_page)
			{
				//impossible
				printf("NSLB impossible mapping by st[]\n");
				system("pause");
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)
		{
			//new
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)//SLB page be merged by NSLB, thus have valid-is-1 page here
			{
				FASTwriteSectorSLBpart5(simPtr, offsetSector, NC->pageSizeSector);
				return;
			}
			//---------------------

			//NSLB page is going to rewrite
			assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2);//bug!!!
			NSLB->pageRead++;
			NSLB->pageWrite++;
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorSLBNSLBpart5(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j = 0, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	NFTL *N = &(simPtr->NFTLobj);

	while (1)
	{
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//FASTwriteSectorSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector);

		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);    //track merge, 不可以#define FreeList_replace_lastRWpage  !!! 如果做會有爆量的page write
		
		//BPLRU merge, 一定要#define FreeList_replace_lastRWpage  !!!
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));
		//FASTwriteSectorNSLBpart3(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector)); 

		/*if (SectorTableNo == 553722000)
		{
		printf("data band (%lu %lu) valid:%lu sector:%lu;  ", SectorTableNo / NC->blockSizeSector, SectorTableNo % NC->blockSizeSector, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector);
		printf("LB(%lu %I64u) valid:%lu sector:%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);
		}*/

		if (0 <= getblock(simPtr, offsetSector) && getblock(simPtr, offsetSector) <= NC->LsizeBlock - 1)//in Data Band
		{
			if (N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid == 2)
			{
				assert(NSLB->NSLBpage_num != 0);
				FASTwriteSectorNSLBpart5(simPtr, offsetSector, NC->pageSizeSector);//in NSLB write
			}
			else if (N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid == 1)
			{
				FASTwriteSectorSLBpart5(simPtr, offsetSector, NC->pageSizeSector);
			}
			else
			{
				//get block & page will tell you the valid page is where
				printf("%d %lu\n", N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid, N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].sector);
				printf("%d %lu\n", N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].valid, N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].sector);
				printf("S NS write error A\n");
				system("pause");
			}
		}
		else if (getblock(simPtr, offsetSector) == NC->PsizeBlock - 1)//from SLB to NSLB
		{
			if (0 <= getpage(simPtr, offsetSector) && getpage(simPtr, offsetSector) <= SLB->partial_page)
			{
				if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 1)
				{
					FASTwriteSectorSLBpart5(simPtr, offsetSector, NC->pageSizeSector);//make the SLB page valid = 2, second time write
				}
				else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 2)
				{
					assert(NSLB->NSLBpage_num != 0);
					FASTwriteSectorNSLBpart5(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));//change to NSLB write
				}
			}
			else
			{
				printf("S NS write error B\n");
				system("pause");
			}
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}
//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//3SLB write to NSLB, both GC
//----------------------------------------------
void FASTwriteSectorSLBpart6(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, BlockNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD i = 0;
	DWORD SectorTableNo = 0, PageNo = 0;


	/*if (SectorTableNo == 553722000)
	{
	printf("in SLB write: (%lu %I64u) valid:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid);
	}*/


	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWpage == SLB->guardTrack1stPage)//當前空間已寫滿
		{
			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage)//發現guard track右邊是fRW，只能merge
			{
				//printf("SLB->writedPageNum = %lu, SLB->SLBpage_num = %lu, NC->trackSizePage = %lu %lu\n", SLB->writedPageNum, SLB->SLBpage_num, NC->trackSizePage, SLB->SLBpage_num - NC->trackSizePage);
				assert(SLB->writedPageNum == SLB->SLBpage_num);

				//for single SLB
				//FASTmergeRWSLBpart(simPtr);

				//for SLBNSLB
				FASTmergeRWSLBpart6(simPtr);
			}
			else//guard track右邊不是fRW，右移gt一格
			{
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
			}

		}

		assert(SLB->lastRWpage >= 0);
		assert(SLB->lastRWpage <= SLB->partial_page);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);

		//invalid the same page in log buffer or in data band
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)//in LB
		{
			assert(0 <= getpage(simPtr, SectorTableNo) && getpage(simPtr, SectorTableNo) <= SLB->partial_page);
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid != 0);
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 2;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 2)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 3;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 3)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 3;
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)//in Data Band, first time write
		{
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1);//check original data band page is valid 1
			N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old

			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;
		}
		else
		{
			printf("write error\n");
			system("pause");
		}

		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setpage(simPtr, SectorTableNo, SLB->lastRWpage);

		SLB->lastRWpage = (SLB->lastRWpage + 1) % (SLB->partial_page + 1);
		SLB->pageWrite++;
		SLB->writedPageNum++;

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorNSLBpart6(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//if (SectorTableNo == 553722000)//SLB寫過也改了setblock setpage，為何在NSLB裡還是找到1056 73872
	//{
	//	printf("init NSLB, getblock(simPtr, SectorTableNo):%lu ,getpage(simPtr, SectorTableNo):%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo));
	//}
	//if (SectorTableNo == 203944120)
	//{
	//	printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//	//N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;
	//	//printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//}

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//merge the BPLRU band
		N->cnt++;
		N->BPLRU[LBA] = N->cnt;
		//驗整 getblock(simPtr, SectorTableNo) == SectorTableNo / NC->blockSizeSector ???
		if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock-1 && N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2)
		{

		}
		else if (NSLB->writedPageNum >= NSLB->NSLBpage_num)//當前空間已寫滿
		{
			FASTmergeRWNSLBpart6(simPtr);
			assert(NSLB->writedPageNum < NSLB->NSLBpage_num);
		}

		DWORD page_no = getpage(simPtr, SectorTableNo);
		DWORD old_sector = 0;
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)
		{
			if (0 <= page_no && page_no <= SLB->partial_page)//move from SLB to NSLB
			{
				old_sector = N->blocks[NC->PsizeBlock - 1].pages[page_no].sector;
				assert(old_sector == SectorTableNo);
				assert(0 <= old_sector && old_sector <= NC->LsizeSector);
				//free SLB page
				assert(N->blocks[NC->PsizeBlock - 1].pages[page_no].valid == 3);//decide the write times in SLB

				N->blocks[NC->PsizeBlock - 1].pages[page_no].valid = 0;
				//N->blocks[NC->PsizeBlock - 1].pages[page_no].sector = 0;
				//set data band's page to original mapping
				setblock(simPtr, old_sector, old_sector / NC->blockSizeSector);
				setpage(simPtr, old_sector, old_sector % NC->blockSizeSector);
				//NSLB mapping is valid == 2
				assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 0);
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector = SectorTableNo;
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;

				NSLB->pageRead++;
				NSLB->pageWrite++;
				NSLB->writedPageNum++;
			}
			else if (SLB->partial_page + 1 <= page_no && page_no <= NSLB->partial_page)
			{
				//impossible
				printf("NSLB impossible mapping by st[]\n");
				system("pause");
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)
		{
			//new
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)//SLB page be merged by NSLB, thus have valid-is-1 page here
			{
				FASTwriteSectorSLBpart6(simPtr, offsetSector, NC->pageSizeSector);
				return;
			}
			//---------------------

			//NSLB page is going to rewrite
			assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2);//bug!!!
			NSLB->pageRead++;
			NSLB->pageWrite++;
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorSLBNSLBpart6(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j = 0, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	NFTL *N = &(simPtr->NFTLobj);

	while (1)
	{
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//FASTwriteSectorSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector);

		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);    //track merge, 不可以#define FreeList_replace_lastRWpage  !!! 如果做會有爆量的page write

		//BPLRU merge, 一定要#define FreeList_replace_lastRWpage  !!!
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));
		//FASTwriteSectorNSLBpart3(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector)); 

		/*if (SectorTableNo == 553722000)
		{
		printf("data band (%lu %lu) valid:%lu sector:%lu;  ", SectorTableNo / NC->blockSizeSector, SectorTableNo % NC->blockSizeSector, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector);
		printf("LB(%lu %I64u) valid:%lu sector:%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);
		}*/

		if (0 <= getblock(simPtr, offsetSector) && getblock(simPtr, offsetSector) <= NC->LsizeBlock - 1)//in Data Band
		{
			if (N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid == 2)
			{
				assert(NSLB->NSLBpage_num != 0);
				FASTwriteSectorNSLBpart6(simPtr, offsetSector, NC->pageSizeSector);//in NSLB write
			}
			else if (N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid == 1)
			{
				FASTwriteSectorSLBpart6(simPtr, offsetSector, NC->pageSizeSector);
			}
			else
			{
				//get block & page will tell you the valid page is where
				printf("%d %lu\n", N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid, N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].sector);
				printf("%d %lu\n", N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].valid, N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].sector);
				printf("S NS write error A\n");
				system("pause");
			}
		}
		else if (getblock(simPtr, offsetSector) == NC->PsizeBlock - 1)//from SLB to NSLB
		{
			if (0 <= getpage(simPtr, offsetSector) && getpage(simPtr, offsetSector) <= SLB->partial_page)
			{
				if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 1 || N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 2)
				{
					FASTwriteSectorSLBpart6(simPtr, offsetSector, NC->pageSizeSector);//make the SLB page valid = 2, second time write
				}
				else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 3)
				{
					assert(NSLB->NSLBpage_num != 0);
					FASTwriteSectorNSLBpart6(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));//change to NSLB write
				}
			}
			else
			{
				printf("S NS write error B\n");
				system("pause");
			}
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//6SLB write to NSLB, both GC
//----------------------------------------------
void FASTwriteSectorSLBpart9(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, BlockNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD i = 0;
	DWORD SectorTableNo = 0, PageNo = 0;


	/*if (SectorTableNo == 553722000)
	{
	printf("in SLB write: (%lu %I64u) valid:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid);
	}*/


	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWpage == SLB->guardTrack1stPage)//當前空間已寫滿
		{
			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage)//發現guard track右邊是fRW，只能merge
			{
				//printf("SLB->writedPageNum = %lu, SLB->SLBpage_num = %lu, NC->trackSizePage = %lu %lu\n", SLB->writedPageNum, SLB->SLBpage_num, NC->trackSizePage, SLB->SLBpage_num - NC->trackSizePage);
				assert(SLB->writedPageNum == SLB->SLBpage_num);

				//for single SLB
				//FASTmergeRWSLBpart(simPtr);

				//for SLBNSLB
				FASTmergeRWSLBpart9(simPtr);
			}
			else//guard track右邊不是fRW，右移gt一格
			{
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
			}

		}

		assert(SLB->lastRWpage >= 0);
		assert(SLB->lastRWpage <= SLB->partial_page);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);

		//invalid the same page in log buffer or in data band
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)//in LB
		{
			assert(0 <= getpage(simPtr, SectorTableNo) && getpage(simPtr, SectorTableNo) <= SLB->partial_page);
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid != 0);
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 2;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 2)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 3;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 3)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 4;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 4)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 5;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 5)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 6;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 6)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 6;
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)//in Data Band, first time write
		{
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1);//check original data band page is valid 1
			N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old

			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;
		}
		else
		{
			printf("write error\n");
			system("pause");
		}

		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setpage(simPtr, SectorTableNo, SLB->lastRWpage);

		SLB->lastRWpage = (SLB->lastRWpage + 1) % (SLB->partial_page + 1);
		SLB->pageWrite++;
		SLB->writedPageNum++;

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorNSLBpart9(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//if (SectorTableNo == 553722000)//SLB寫過也改了setblock setpage，為何在NSLB裡還是找到1056 73872
	//{
	//	printf("init NSLB, getblock(simPtr, SectorTableNo):%lu ,getpage(simPtr, SectorTableNo):%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo));
	//}
	//if (SectorTableNo == 203944120)
	//{
	//	printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//	//N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;
	//	//printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//}

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//merge the BPLRU band
		N->cnt++;
		N->BPLRU[LBA] = N->cnt;

		if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1 && N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2)
		{

		}
		else if (NSLB->writedPageNum >= NSLB->NSLBpage_num)//當前空間已寫滿
		{
			FASTmergeRWNSLBpart9(simPtr);
			assert(NSLB->writedPageNum < NSLB->NSLBpage_num);
		}

		DWORD page_no = getpage(simPtr, SectorTableNo);
		DWORD old_sector = 0;
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)
		{
			if (0 <= page_no && page_no <= SLB->partial_page)//move from SLB to NSLB
			{
				old_sector = N->blocks[NC->PsizeBlock - 1].pages[page_no].sector;
				assert(old_sector == SectorTableNo);
				assert(0 <= old_sector && old_sector <= NC->LsizeSector);
				//free SLB page
				assert(N->blocks[NC->PsizeBlock - 1].pages[page_no].valid == 6);//decide the write times in SLB

				N->blocks[NC->PsizeBlock - 1].pages[page_no].valid = 0;
				//N->blocks[NC->PsizeBlock - 1].pages[page_no].sector = 0;
				//set data band's page to original mapping
				setblock(simPtr, old_sector, old_sector / NC->blockSizeSector);
				setpage(simPtr, old_sector, old_sector % NC->blockSizeSector);
				//NSLB mapping is valid == 2
				assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 0);
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector = SectorTableNo;
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;

				NSLB->pageRead++;
				NSLB->pageWrite++;
				NSLB->writedPageNum++;
			}
			else if (SLB->partial_page + 1 <= page_no && page_no <= NSLB->partial_page)
			{
				//impossible
				printf("NSLB impossible mapping by st[]\n");
				system("pause");
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)
		{
			//new
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)//SLB page be merged by NSLB, thus have valid-is-1 page here
			{
				FASTwriteSectorSLBpart9(simPtr, offsetSector, NC->pageSizeSector);
				return;
			}
			//---------------------

			//NSLB page is going to rewrite
			assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2);//bug!!!
			NSLB->pageRead++;
			NSLB->pageWrite++;
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorSLBNSLBpart9(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j = 0, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	NFTL *N = &(simPtr->NFTLobj);

	while (1)
	{
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//FASTwriteSectorSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector);

		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);    //track merge, 不可以#define FreeList_replace_lastRWpage  !!! 如果做會有爆量的page write

		//BPLRU merge, 一定要#define FreeList_replace_lastRWpage  !!!
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));
		//FASTwriteSectorNSLBpart3(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector)); 

		/*if (SectorTableNo == 553722000)
		{
		printf("data band (%lu %lu) valid:%lu sector:%lu;  ", SectorTableNo / NC->blockSizeSector, SectorTableNo % NC->blockSizeSector, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector);
		printf("LB(%lu %I64u) valid:%lu sector:%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);
		}*/

		if (0 <= getblock(simPtr, offsetSector) && getblock(simPtr, offsetSector) <= NC->LsizeBlock - 1)//in Data Band
		{
			if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 2)
			{
				assert(NSLB->NSLBpage_num != 0);
				FASTwriteSectorNSLBpart9(simPtr, offsetSector, NC->pageSizeSector);//in NSLB write
			}
			else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 1)//First time write to SLB
			{
				FASTwriteSectorSLBpart9(simPtr, offsetSector, NC->pageSizeSector);
			}
			else
			{
				//get block & page will tell you the valid page is where
				printf("%d %lu\n", N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid, N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].sector);
				printf("%d %lu\n", N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].valid, N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].sector);
				printf("S NS write error A\n");
				system("pause");
			}
		}
		else if (getblock(simPtr, offsetSector) == NC->PsizeBlock - 1)//from SLB to NSLB
		{
			if (0 <= getpage(simPtr, offsetSector) && getpage(simPtr, offsetSector) <= SLB->partial_page)
			{
				if (1 <= N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid && N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid <= 5)
				{
					FASTwriteSectorSLBpart9(simPtr, offsetSector, NC->pageSizeSector);//We write the same page 6 time in SLB
				}
				else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 6)
				{
					assert(NSLB->NSLBpage_num != 0);
					FASTwriteSectorNSLBpart9(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));//change to NSLB write
				}
			}
			else
			{
				printf("S NS write error B\n");
				system("pause");
			}
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//8SLB write to NSLB, both GC
//----------------------------------------------
void FASTwriteSectorSLBpart11(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, BlockNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD i = 0;
	DWORD SectorTableNo = 0, PageNo = 0;


	/*if (SectorTableNo == 553722000)
	{
	printf("in SLB write: (%lu %I64u) valid:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid);
	}*/


	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWpage == SLB->guardTrack1stPage)//當前空間已寫滿
		{
			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage)//發現guard track右邊是fRW，只能merge
			{
				//printf("SLB->writedPageNum = %lu, SLB->SLBpage_num = %lu, NC->trackSizePage = %lu %lu\n", SLB->writedPageNum, SLB->SLBpage_num, NC->trackSizePage, SLB->SLBpage_num - NC->trackSizePage);
				assert(SLB->writedPageNum == SLB->SLBpage_num);

				//for single SLB
				//FASTmergeRWSLBpart(simPtr);

				//for SLBNSLB
				FASTmergeRWSLBpart11(simPtr);
			}
			else//guard track右邊不是fRW，右移gt一格
			{
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
			}

		}

		assert(SLB->lastRWpage >= 0);
		assert(SLB->lastRWpage <= SLB->partial_page);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);

		//invalid the same page in log buffer or in data band
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)//in LB
		{
			assert(0 <= getpage(simPtr, SectorTableNo) && getpage(simPtr, SectorTableNo) <= SLB->partial_page);
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid != 0);
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 2;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 2)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 3;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 3)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 4;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 4)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 5;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 5)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 6;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 6)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 7;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 7)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 8;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 8)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 8;
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)//in Data Band, first time write
		{
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1);//check original data band page is valid 1
			N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old

			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;
		}
		else
		{
			printf("write error\n");
			system("pause");
		}

		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setpage(simPtr, SectorTableNo, SLB->lastRWpage);

		SLB->lastRWpage = (SLB->lastRWpage + 1) % (SLB->partial_page + 1);
		SLB->pageWrite++;
		SLB->writedPageNum++;

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorNSLBpart11(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//if (SectorTableNo == 553722000)//SLB寫過也改了setblock setpage，為何在NSLB裡還是找到1056 73872
	//{
	//	printf("init NSLB, getblock(simPtr, SectorTableNo):%lu ,getpage(simPtr, SectorTableNo):%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo));
	//}
	//if (SectorTableNo == 203944120)
	//{
	//	printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//	//N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;
	//	//printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//}

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//merge the BPLRU band
		N->cnt++;
		N->BPLRU[LBA] = N->cnt;

		if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1 && N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2)
		{

		}
		else if (NSLB->writedPageNum >= NSLB->NSLBpage_num)//當前空間已寫滿
		{
			FASTmergeRWNSLBpart11(simPtr);
			assert(NSLB->writedPageNum < NSLB->NSLBpage_num);
		}

		DWORD page_no = getpage(simPtr, SectorTableNo);
		DWORD old_sector = 0;
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)
		{
			if (0 <= page_no && page_no <= SLB->partial_page)//move from SLB to NSLB
			{
				old_sector = N->blocks[NC->PsizeBlock - 1].pages[page_no].sector;
				assert(old_sector == SectorTableNo);
				assert(0 <= old_sector && old_sector <= NC->LsizeSector);
				//free SLB page
				assert(N->blocks[NC->PsizeBlock - 1].pages[page_no].valid == 8);//decide the write times in SLB

				N->blocks[NC->PsizeBlock - 1].pages[page_no].valid = 0;
				//N->blocks[NC->PsizeBlock - 1].pages[page_no].sector = 0;
				//set data band's page to original mapping
				setblock(simPtr, old_sector, old_sector / NC->blockSizeSector);
				setpage(simPtr, old_sector, old_sector % NC->blockSizeSector);
				//NSLB mapping is valid == 2
				assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 0);
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector = SectorTableNo;
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;

				NSLB->pageRead++;
				NSLB->pageWrite++;
				NSLB->writedPageNum++;
			}
			else if (SLB->partial_page + 1 <= page_no && page_no <= NSLB->partial_page)
			{
				//impossible
				printf("NSLB impossible mapping by st[]\n");
				system("pause");
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)
		{
			//new
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)//SLB page be merged by NSLB, thus have valid-is-1 page here
			{
				FASTwriteSectorSLBpart11(simPtr, offsetSector, NC->pageSizeSector);
				return;
			}
			//---------------------

			//NSLB page is going to rewrite
			assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2);//bug!!!
			NSLB->pageRead++;
			NSLB->pageWrite++;
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorSLBNSLBpart11(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j = 0, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	NFTL *N = &(simPtr->NFTLobj);

	while (1)
	{
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//FASTwriteSectorSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector);

		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);    //track merge, 不可以#define FreeList_replace_lastRWpage  !!! 如果做會有爆量的page write

		//BPLRU merge, 一定要#define FreeList_replace_lastRWpage  !!!
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));
		//FASTwriteSectorNSLBpart3(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector)); 

		/*if (SectorTableNo == 553722000)
		{
		printf("data band (%lu %lu) valid:%lu sector:%lu;  ", SectorTableNo / NC->blockSizeSector, SectorTableNo % NC->blockSizeSector, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector);
		printf("LB(%lu %I64u) valid:%lu sector:%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);
		}*/

		if (0 <= getblock(simPtr, offsetSector) && getblock(simPtr, offsetSector) <= NC->LsizeBlock - 1)//in Data Band
		{
			if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 2)
			{
				assert(NSLB->NSLBpage_num != 0);
				FASTwriteSectorNSLBpart11(simPtr, offsetSector, NC->pageSizeSector);//in NSLB write
			}
			else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 1)//First time write to SLB
			{
				FASTwriteSectorSLBpart11(simPtr, offsetSector, NC->pageSizeSector);
			}
			else
			{
				//get block & page will tell you the valid page is where
				printf("%d %lu\n", N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid, N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].sector);
				printf("%d %lu\n", N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].valid, N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].sector);
				printf("S NS write error A\n");
				system("pause");
			}
		}
		else if (getblock(simPtr, offsetSector) == NC->PsizeBlock - 1)//from SLB to NSLB
		{
			if (0 <= getpage(simPtr, offsetSector) && getpage(simPtr, offsetSector) <= SLB->partial_page)
			{
				if (1 <= N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid && N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid <= 7)
				{
					FASTwriteSectorSLBpart11(simPtr, offsetSector, NC->pageSizeSector);//We write the same page 8 time in SLB
				}
				else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 8)
				{
					assert(NSLB->NSLBpage_num != 0);
					FASTwriteSectorNSLBpart11(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));//change to NSLB write
				}
			}
			else
			{
				printf("S NS write error B\n");
				system("pause");
			}
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//10SLB write to NSLB, both GC
//----------------------------------------------
void FASTwriteSectorSLBpart13(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, BlockNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD i = 0;
	DWORD SectorTableNo = 0, PageNo = 0;


	/*if (SectorTableNo == 553722000)
	{
	printf("in SLB write: (%lu %I64u) valid:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid);
	}*/


	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWpage == SLB->guardTrack1stPage)//當前空間已寫滿
		{
			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage)//發現guard track右邊是fRW，只能merge
			{
				//printf("SLB->writedPageNum = %lu, SLB->SLBpage_num = %lu, NC->trackSizePage = %lu %lu\n", SLB->writedPageNum, SLB->SLBpage_num, NC->trackSizePage, SLB->SLBpage_num - NC->trackSizePage);
				assert(SLB->writedPageNum == SLB->SLBpage_num);

				//for single SLB
				//FASTmergeRWSLBpart(simPtr);

				//for SLBNSLB
				FASTmergeRWSLBpart13(simPtr);
			}
			else//guard track右邊不是fRW，右移gt一格
			{
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
			}

		}

		assert(SLB->lastRWpage >= 0);
		assert(SLB->lastRWpage <= SLB->partial_page);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);

		//invalid the same page in log buffer or in data band
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)//in LB
		{
			assert(0 <= getpage(simPtr, SectorTableNo) && getpage(simPtr, SectorTableNo) <= SLB->partial_page);
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid != 0);
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 2;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 2)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 3;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 3)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 4;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 4)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 5;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 5)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 6;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 6)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 7;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 7)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 8;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 8)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 9;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 9)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 10;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 10)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 10;
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)//in Data Band, first time write
		{
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1);//check original data band page is valid 1
			N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old

			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;
		}
		else
		{
			printf("write error\n");
			system("pause");
		}

		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setpage(simPtr, SectorTableNo, SLB->lastRWpage);

		SLB->lastRWpage = (SLB->lastRWpage + 1) % (SLB->partial_page + 1);
		SLB->pageWrite++;
		SLB->writedPageNum++;

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorNSLBpart13(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//if (SectorTableNo == 553722000)//SLB寫過也改了setblock setpage，為何在NSLB裡還是找到1056 73872
	//{
	//	printf("init NSLB, getblock(simPtr, SectorTableNo):%lu ,getpage(simPtr, SectorTableNo):%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo));
	//}
	//if (SectorTableNo == 203944120)
	//{
	//	printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//	//N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;
	//	//printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//}

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//merge the BPLRU band
		N->cnt++;
		N->BPLRU[LBA] = N->cnt;

		if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1 && N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2)
		{

		}
		else if (NSLB->writedPageNum >= NSLB->NSLBpage_num)//當前空間已寫滿
		{
			FASTmergeRWNSLBpart13(simPtr);
			assert(NSLB->writedPageNum < NSLB->NSLBpage_num);
		}

		DWORD page_no = getpage(simPtr, SectorTableNo);
		DWORD old_sector = 0;
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)
		{
			if (0 <= page_no && page_no <= SLB->partial_page)//move from SLB to NSLB
			{
				old_sector = N->blocks[NC->PsizeBlock - 1].pages[page_no].sector;
				assert(old_sector == SectorTableNo);
				assert(0 <= old_sector && old_sector <= NC->LsizeSector);
				//free SLB page
				assert(N->blocks[NC->PsizeBlock - 1].pages[page_no].valid == 10);//decide the write times in SLB

				N->blocks[NC->PsizeBlock - 1].pages[page_no].valid = 0;
				//N->blocks[NC->PsizeBlock - 1].pages[page_no].sector = 0;
				//set data band's page to original mapping
				setblock(simPtr, old_sector, old_sector / NC->blockSizeSector);
				setpage(simPtr, old_sector, old_sector % NC->blockSizeSector);
				//NSLB mapping is valid == 2
				assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 0);
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector = SectorTableNo;
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;

				NSLB->pageRead++;
				NSLB->pageWrite++;
				NSLB->writedPageNum++;
			}
			else if (SLB->partial_page + 1 <= page_no && page_no <= NSLB->partial_page)
			{
				//impossible
				printf("NSLB impossible mapping by st[]\n");
				system("pause");
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)
		{
			//new
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)//SLB page be merged by NSLB, thus have valid-is-1 page here
			{
				FASTwriteSectorSLBpart13(simPtr, offsetSector, NC->pageSizeSector);
				return;
			}
			//---------------------

			//NSLB page is going to rewrite
			assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2);//bug!!!
			NSLB->pageRead++;
			NSLB->pageWrite++;
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorSLBNSLBpart13(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j = 0, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	NFTL *N = &(simPtr->NFTLobj);

	while (1)
	{
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//FASTwriteSectorSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector);

		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);    //track merge, 不可以#define FreeList_replace_lastRWpage  !!! 如果做會有爆量的page write

		//BPLRU merge, 一定要#define FreeList_replace_lastRWpage  !!!
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));
		//FASTwriteSectorNSLBpart3(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector)); 

		/*if (SectorTableNo == 553722000)
		{
		printf("data band (%lu %lu) valid:%lu sector:%lu;  ", SectorTableNo / NC->blockSizeSector, SectorTableNo % NC->blockSizeSector, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector);
		printf("LB(%lu %I64u) valid:%lu sector:%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);
		}*/

		if (0 <= getblock(simPtr, offsetSector) && getblock(simPtr, offsetSector) <= NC->LsizeBlock - 1)//in Data Band
		{
			if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 2)
			{
				assert(NSLB->NSLBpage_num != 0);
				FASTwriteSectorNSLBpart13(simPtr, offsetSector, NC->pageSizeSector);//in NSLB write
			}
			else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 1)//First time write to SLB
			{
				FASTwriteSectorSLBpart13(simPtr, offsetSector, NC->pageSizeSector);
			}
			else
			{
				//get block & page will tell you the valid page is where
				printf("%d %lu\n", N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid, N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].sector);
				printf("%d %lu\n", N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].valid, N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].sector);
				printf("S NS write error A\n");
				system("pause");
			}
		}
		else if (getblock(simPtr, offsetSector) == NC->PsizeBlock - 1)//from SLB to NSLB
		{
			if (0 <= getpage(simPtr, offsetSector) && getpage(simPtr, offsetSector) <= SLB->partial_page)
			{
				if (1 <= N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid && N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid <= 9)
				{
					FASTwriteSectorSLBpart13(simPtr, offsetSector, NC->pageSizeSector);//We write the same page 10 time in SLB
				}
				else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 10)
				{
					assert(NSLB->NSLBpage_num != 0);
					FASTwriteSectorNSLBpart13(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));//change to NSLB write
				}
			}
			else
			{
				printf("S NS write error B\n");
				system("pause");
			}
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//12SLB write to NSLB, both GC
//----------------------------------------------
void FASTwriteSectorSLBpart15(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, BlockNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD i = 0;
	DWORD SectorTableNo = 0, PageNo = 0;


	/*if (SectorTableNo == 553722000)
	{
	printf("in SLB write: (%lu %I64u) valid:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid);
	}*/


	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWpage == SLB->guardTrack1stPage)//當前空間已寫滿
		{
			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage)//發現guard track右邊是fRW，只能merge
			{
				//printf("SLB->writedPageNum = %lu, SLB->SLBpage_num = %lu, NC->trackSizePage = %lu %lu\n", SLB->writedPageNum, SLB->SLBpage_num, NC->trackSizePage, SLB->SLBpage_num - NC->trackSizePage);
				assert(SLB->writedPageNum == SLB->SLBpage_num);

				//for single SLB
				//FASTmergeRWSLBpart(simPtr);

				//for SLBNSLB
				FASTmergeRWSLBpart15(simPtr);
			}
			else//guard track右邊不是fRW，右移gt一格
			{
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
			}

		}

		assert(SLB->lastRWpage >= 0);
		assert(SLB->lastRWpage <= SLB->partial_page);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);

		//invalid the same page in log buffer or in data band
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)//in LB
		{
			assert(0 <= getpage(simPtr, SectorTableNo) && getpage(simPtr, SectorTableNo) <= SLB->partial_page);
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid != 0);
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 2;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 2)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 3;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 3)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 4;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 4)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 5;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 5)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 6;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 6)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 7;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 7)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 8;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 8)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 9;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 9)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 10;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 10)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 11;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 11)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 12;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 12)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 12;
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)//in Data Band, first time write
		{
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1);//check original data band page is valid 1
			N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old

			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;
		}
		else
		{
			printf("write error\n");
			system("pause");
		}

		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setpage(simPtr, SectorTableNo, SLB->lastRWpage);

		SLB->lastRWpage = (SLB->lastRWpage + 1) % (SLB->partial_page + 1);
		SLB->pageWrite++;
		SLB->writedPageNum++;

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorNSLBpart15(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//if (SectorTableNo == 553722000)//SLB寫過也改了setblock setpage，為何在NSLB裡還是找到1056 73872
	//{
	//	printf("init NSLB, getblock(simPtr, SectorTableNo):%lu ,getpage(simPtr, SectorTableNo):%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo));
	//}
	//if (SectorTableNo == 203944120)
	//{
	//	printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//	//N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;
	//	//printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//}

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//merge the BPLRU band
		N->cnt++;
		N->BPLRU[LBA] = N->cnt;

		if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1 && N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2)
		{

		}
		else if (NSLB->writedPageNum >= NSLB->NSLBpage_num)//當前空間已寫滿
		{
			FASTmergeRWNSLBpart15(simPtr);
			assert(NSLB->writedPageNum < NSLB->NSLBpage_num);
		}

		DWORD page_no = getpage(simPtr, SectorTableNo);
		DWORD old_sector = 0;
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)
		{
			if (0 <= page_no && page_no <= SLB->partial_page)//move from SLB to NSLB
			{
				old_sector = N->blocks[NC->PsizeBlock - 1].pages[page_no].sector;
				assert(old_sector == SectorTableNo);
				assert(0 <= old_sector && old_sector <= NC->LsizeSector);
				//free SLB page
				assert(N->blocks[NC->PsizeBlock - 1].pages[page_no].valid == 12);//decide the write times in SLB

				N->blocks[NC->PsizeBlock - 1].pages[page_no].valid = 0;
				//N->blocks[NC->PsizeBlock - 1].pages[page_no].sector = 0;
				//set data band's page to original mapping
				setblock(simPtr, old_sector, old_sector / NC->blockSizeSector);
				setpage(simPtr, old_sector, old_sector % NC->blockSizeSector);
				//NSLB mapping is valid == 2
				assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 0);
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector = SectorTableNo;
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;

				NSLB->pageRead++;
				NSLB->pageWrite++;
				NSLB->writedPageNum++;
			}
			else if (SLB->partial_page + 1 <= page_no && page_no <= NSLB->partial_page)
			{
				//impossible
				printf("NSLB impossible mapping by st[]\n");
				system("pause");
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)
		{
			//new
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)//SLB page be merged by NSLB, thus have valid-is-1 page here
			{
				FASTwriteSectorSLBpart15(simPtr, offsetSector, NC->pageSizeSector);
				return;
			}
			//---------------------

			//NSLB page is going to rewrite
			assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2);//bug!!!
			NSLB->pageRead++;
			NSLB->pageWrite++;
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorSLBNSLBpart15(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j = 0, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	NFTL *N = &(simPtr->NFTLobj);

	while (1)
	{
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//FASTwriteSectorSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector);

		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);    //track merge, 不可以#define FreeList_replace_lastRWpage  !!! 如果做會有爆量的page write

		//BPLRU merge, 一定要#define FreeList_replace_lastRWpage  !!!
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));
		//FASTwriteSectorNSLBpart3(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector)); 

		/*if (SectorTableNo == 553722000)
		{
		printf("data band (%lu %lu) valid:%lu sector:%lu;  ", SectorTableNo / NC->blockSizeSector, SectorTableNo % NC->blockSizeSector, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector);
		printf("LB(%lu %I64u) valid:%lu sector:%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);
		}*/

		if (0 <= getblock(simPtr, offsetSector) && getblock(simPtr, offsetSector) <= NC->LsizeBlock - 1)//in Data Band
		{
			if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 2)
			{
				assert(NSLB->NSLBpage_num != 0);
				FASTwriteSectorNSLBpart15(simPtr, offsetSector, NC->pageSizeSector);//in NSLB write
			}
			else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 1)//First time write to SLB
			{
				FASTwriteSectorSLBpart15(simPtr, offsetSector, NC->pageSizeSector);
			}
			else
			{
				//get block & page will tell you the valid page is where
				printf("%d %lu\n", N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid, N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].sector);
				printf("%d %lu\n", N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].valid, N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].sector);
				printf("S NS write error A\n");
				system("pause");
			}
		}
		else if (getblock(simPtr, offsetSector) == NC->PsizeBlock - 1)//from SLB to NSLB
		{
			if (0 <= getpage(simPtr, offsetSector) && getpage(simPtr, offsetSector) <= SLB->partial_page)
			{
				if (1 <= N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid && N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid <= 11)
				{
					FASTwriteSectorSLBpart15(simPtr, offsetSector, NC->pageSizeSector);//We write the same page 12 time in SLB
				}
				else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 12)
				{
					assert(NSLB->NSLBpage_num != 0);
					FASTwriteSectorNSLBpart15(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));//change to NSLB write
				}
			}
			else
			{
				printf("S NS write error B\n");
				system("pause");
			}
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

//----------------------------------------------
//end
//----------------------------------------------



//----------------------------------------------
//16SLB write to NSLB, both GC
//----------------------------------------------
void FASTwriteSectorSLBpart19(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, BlockNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD i = 0;
	DWORD SectorTableNo = 0, PageNo = 0;


	/*if (SectorTableNo == 553722000)
	{
	printf("in SLB write: (%lu %I64u) valid:%lu\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid);
	}*/


	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWpage == SLB->guardTrack1stPage)//當前空間已寫滿
		{
			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage)//發現guard track右邊是fRW，只能merge
			{
				//printf("SLB->writedPageNum = %lu, SLB->SLBpage_num = %lu, NC->trackSizePage = %lu %lu\n", SLB->writedPageNum, SLB->SLBpage_num, NC->trackSizePage, SLB->SLBpage_num - NC->trackSizePage);
				assert(SLB->writedPageNum == SLB->SLBpage_num);

				//for single SLB
				//FASTmergeRWSLBpart(simPtr);

				//for SLBNSLB
				FASTmergeRWSLBpart19(simPtr);
			}
			else//guard track右邊不是fRW，右移gt一格
			{
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++)
				{
					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
				}
			}

		}

		assert(SLB->lastRWpage >= 0);
		assert(SLB->lastRWpage <= SLB->partial_page);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);

		//invalid the same page in log buffer or in data band
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)//in LB
		{
			assert(0 <= getpage(simPtr, SectorTableNo) && getpage(simPtr, SectorTableNo) <= SLB->partial_page);
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid != 0);
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 2;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 2)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 3;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 3)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 4;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 4)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 5;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 5)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 6;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 6)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 7;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 7)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 8;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 8)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 9;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 9)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 10;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 10)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 11;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 11)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 12;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 12)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 13;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 13)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 14;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 14)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 15;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 15)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 16;
			}
			else if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 16)
			{
				N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
				N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 16;
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)//in Data Band, first time write
		{
			assert(N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1);//check original data band page is valid 1
			N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid = 0; //invalid old

			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
			N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;
		}
		else
		{
			printf("write error\n");
			system("pause");
		}

		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setpage(simPtr, SectorTableNo, SLB->lastRWpage);

		SLB->lastRWpage = (SLB->lastRWpage + 1) % (SLB->partial_page + 1);
		SLB->pageWrite++;
		SLB->writedPageNum++;

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorNSLBpart19(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//if (SectorTableNo == 553722000)//SLB寫過也改了setblock setpage，為何在NSLB裡還是找到1056 73872
	//{
	//	printf("init NSLB, getblock(simPtr, SectorTableNo):%lu ,getpage(simPtr, SectorTableNo):%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo));
	//}
	//if (SectorTableNo == 203944120)
	//{
	//	printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//	//N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;
	//	//printf("N->blocks[SectorTableNo  NC->blockSizeSector].pages[SectorTableNo  NC->blockSizeSector].valid = %lu\n", N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid);
	//}

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//merge the BPLRU band
		N->cnt++;
		N->BPLRU[LBA] = N->cnt;

		if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1 && N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2)
		{

		}
		else if (NSLB->writedPageNum >= NSLB->NSLBpage_num)//當前空間已寫滿
		{
			FASTmergeRWNSLBpart19(simPtr);
			assert(NSLB->writedPageNum < NSLB->NSLBpage_num);
		}

		DWORD page_no = getpage(simPtr, SectorTableNo);
		DWORD old_sector = 0;
		if (getblock(simPtr, SectorTableNo) == NC->PsizeBlock - 1)
		{
			if (0 <= page_no && page_no <= SLB->partial_page)//move from SLB to NSLB
			{
				old_sector = N->blocks[NC->PsizeBlock - 1].pages[page_no].sector;
				assert(old_sector == SectorTableNo);
				assert(0 <= old_sector && old_sector <= NC->LsizeSector);
				//free SLB page
				assert(N->blocks[NC->PsizeBlock - 1].pages[page_no].valid == 16);//decide the write times in SLB

				N->blocks[NC->PsizeBlock - 1].pages[page_no].valid = 0;
				//N->blocks[NC->PsizeBlock - 1].pages[page_no].sector = 0;
				//set data band's page to original mapping
				setblock(simPtr, old_sector, old_sector / NC->blockSizeSector);
				setpage(simPtr, old_sector, old_sector % NC->blockSizeSector);
				//NSLB mapping is valid == 2
				assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 0);
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector = SectorTableNo;
				N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid = 2;

				NSLB->pageRead++;
				NSLB->pageWrite++;
				NSLB->writedPageNum++;
			}
			else if (SLB->partial_page + 1 <= page_no && page_no <= NSLB->partial_page)
			{
				//impossible
				printf("NSLB impossible mapping by st[]\n");
				system("pause");
			}
		}
		else if (0 <= getblock(simPtr, SectorTableNo) && getblock(simPtr, SectorTableNo) <= NC->LsizeBlock - 1)
		{
			//new
			if (N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid == 1)//SLB page be merged by NSLB, thus have valid-is-1 page here
			{
				FASTwriteSectorSLBpart19(simPtr, offsetSector, NC->pageSizeSector);
				return;
			}
			//---------------------

			//NSLB page is going to rewrite
			assert(N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid == 2);//bug!!!
			NSLB->pageRead++;
			NSLB->pageWrite++;
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

void FASTwriteSectorSLBNSLBpart19(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j = 0, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	NFTL *N = &(simPtr->NFTLobj);

	while (1)
	{
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//FASTwriteSectorSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector);

		//FASTwriteSectorNSLBpart(simPtr, offsetSector, NC->pageSizeSector);    //track merge, 不可以#define FreeList_replace_lastRWpage  !!! 如果做會有爆量的page write

		//BPLRU merge, 一定要#define FreeList_replace_lastRWpage  !!!
		//FASTwriteSectorNSLBpart2(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));
		//FASTwriteSectorNSLBpart3(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector)); 

		/*if (SectorTableNo == 553722000)
		{
		printf("data band (%lu %lu) valid:%lu sector:%lu;  ", SectorTableNo / NC->blockSizeSector, SectorTableNo % NC->blockSizeSector, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].valid, N->blocks[SectorTableNo / NC->blockSizeSector].pages[SectorTableNo % NC->blockSizeSector].sector);
		printf("LB(%lu %I64u) valid:%lu sector:%I64u\n", getblock(simPtr, SectorTableNo), getpage(simPtr, SectorTableNo), N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].valid, N->blocks[getblock(simPtr, SectorTableNo)].pages[getpage(simPtr, SectorTableNo)].sector);
		}*/

		if (0 <= getblock(simPtr, offsetSector) && getblock(simPtr, offsetSector) <= NC->LsizeBlock - 1)//in Data Band
		{
			if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 2)
			{
				assert(NSLB->NSLBpage_num != 0);
				FASTwriteSectorNSLBpart19(simPtr, offsetSector, NC->pageSizeSector);//in NSLB write
			}
			else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 1)//First time write to SLB
			{
				FASTwriteSectorSLBpart19(simPtr, offsetSector, NC->pageSizeSector);
			}
			else
			{
				//get block & page will tell you the valid page is where
				printf("%d %lu\n", N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].valid, N->blocks[getblock(simPtr, offsetSector)].pages[(DWORD)getpage(simPtr, offsetSector)].sector);
				printf("%d %lu\n", N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].valid, N->blocks[offsetSector / NC->blockSizeSector].pages[offsetSector % NC->blockSizeSector].sector);
				printf("S NS write error A\n");
				system("pause");
			}
		}
		else if (getblock(simPtr, offsetSector) == NC->PsizeBlock - 1)//from SLB to NSLB
		{
			if (0 <= getpage(simPtr, offsetSector) && getpage(simPtr, offsetSector) <= SLB->partial_page)
			{
				if (1 <= N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid && N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid <= 15)
				{
					FASTwriteSectorSLBpart19(simPtr, offsetSector, NC->pageSizeSector);//We write the same page 12 time in SLB
				}
				else if (N->blocks[getblock(simPtr, offsetSector)].pages[getpage(simPtr, offsetSector)].valid == 16)
				{
					assert(NSLB->NSLBpage_num != 0);
					FASTwriteSectorNSLBpart19(simPtr, offsetSector, NC->pageSizeSector, getblock(simPtr, offsetSector));//change to NSLB write
				}
			}
			else
			{
				printf("S NS write error B\n");
				system("pause");
			}
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0) break;
	}
}

//----------------------------------------------
//end
//----------------------------------------------

#endif