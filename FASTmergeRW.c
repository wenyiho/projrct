#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"
#include <math.h>



#ifdef Time_Overhead
DWORD compare(const DWORD *arg1, const DWORD *arg2){ return  (*(DWORD *)arg1 - *(DWORD *)arg2); }
#endif

#ifdef Simulation_SLB
//使用
//void FASTmergeRWSLBpart(sim *simPtr) {
//	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, PageNo = 0;
//	DWORD	j = 0, k = 0;
//	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	int validIs2 = 0, validIs0 = 0, validIs1 = 0;
//	DWORD block_no = 0, page_no = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
//#endif
//	SLB->mergecnt++;
//	Victim = NC->PsizeBlock - 1;
//	tsp_start = SLB->firstRWpage;// (N->lastRWpage + 1) % (N->partial_page + 1);
//	tsp_end = (SLB->firstRWpage + NC->trackSizePage - 1) % (SLB->partial_page + 1);
//
//	int num = 0; assert(tsp_start <= tsp_end);
//	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
//		SectorTableNo1 = N->blocks[Victim].pages[i].sector / NC->pageSizeSector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
//		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
//
//		if (N->blocks[Victim].pages[i].valid == 1){ // valid page in victim log block need merge with data block
//			assert((N->blocks[Victim].pages[i].sector) < NC->LsizePage);
//			dataBlock = N->blocks[Victim].pages[i].sector / NC->blockSizeSector;
//			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
//#ifdef Time_Overhead
//			merge_ram_size = 0;
//#endif
//			validIs2 = 0; validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
//			DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
//			for (j = 0; j < NC->blockSizePage; j++){
//				SectorTableNo2 = dataBlock*NC->blockSizePage + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//				block_no = getblock(simPtr, SectorTableNo2); page_no = getpage(simPtr, SectorTableNo2);
//				if (N->blocks[dataBlock].pages[j].valid == 1){ //原本就在data block裡的valid page
//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizeSector);
//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 % NC->blockSizeSector == page_no);
//					assert(N->blocks[dataBlock].pages[j].sector == SectorTableNo2);
//					//更新統計資料
//					SLB->pageRead++; SLB->pageWrite++; validIs1++;//DB: read hit, DB: write
//					SLB->merge_count++;
//				}
//				else if (N->blocks[dataBlock].pages[j].valid == 0){ //invalid在LB的該page
//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= page_no && page_no <= SLB->partial_page);
//					if (NC->LsizeBlock <= block_no && block_no < NC->PsizeBlock){ N->blocks[block_no].pages[page_no].valid = 0; }//invalid old
//					//更新統計資料
//					SLB->pageRead += 2; SLB->pageWrite++; validIs0++;//DB: read miss, LB: read hit, DB: write
//					SLB->merge_count++;
//#ifdef Time_Overhead
//					merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
//#endif
//					setblock(simPtr, SectorTableNo2, dataBlock);
//					setpage(simPtr, SectorTableNo2, j);
//					N->blocks[dataBlock].pages[j].sector = SectorTableNo2;
//					N->blocks[dataBlock].pages[j].valid = 1;
//				}else{ printf("SLB merge error\n"); system("pause"); }
//			}
//#ifdef Time_Overhead
//			assert(merge_ram_size <= NC->blockSizeSector);
//			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//			for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j+1]); }
//			//Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1);//電梯法回SLB讀資料
//			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//				if (merge_ram[j - 1] / NC->trackSizePage != merge_ram[j] / NC->trackSizePage){
//					//seek
//					//Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1); 
//				}
//			}
//			//DB seek
//			for (j = 0; j < NC->blockSizePage; j+=NC->trackSizePage){
//				SectorTableNo2 = dataBlock*NC->blockSizePage + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//				//Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//			}
//#endif
//			N->blocks[Victim].pages[i].valid = 0;
//#ifdef MergeAssoc_SLBpart
//			fprintf(fp_SLB, "%lu (%d:%d:%d)", dataBlock, validIs0, validIs1, validIs2);
//#endif
//		}
//		else {
//			assert(SectorTableNo1 < NC->LsizePage);
//			assert(getblock(simPtr, SectorTableNo1) != Victim || getpage(simPtr, SectorTableNo1) != i);
//		}
//		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
//		assert(N->blocks[Victim].pages[i].valid == 0);
//	}
//
//	for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
//	//update SMR LB information
//	SLB->firstRWpage = (SLB->firstRWpage + NC->trackSizePage) % (SLB->partial_page + 1);
//	SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
//	SLB->writedPageNum -= (NC->trackSizePage);
//#ifdef Time_Overhead
//	free(merge_ram);
//#endif
//}

//void FASTmergeRWSLBpart2(sim *simPtr) {//每次只merge一條data track 0.5MB，做128次
//	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0, PageNo = 0;
//	DWORD	j = 0, k = 0;
//	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	DWORD validIs0 = 0, validIs1 = 0;
//	DWORD block_no = 0, section_no = 0;
//	//試驗
//	DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
//	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
//	//試驗
//	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
//	DWORD SourceTracks_count = 0, count = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->trackSizeSection, sizeof(DWORD));
//	DWORD start_DBtrack = 0, end_DBtrack = 0;
//
//	DWORD NoMerge_Band = 0;
//#endif
//	/*舊式算時間*/
//	//			for (j = 0; j < NC->blockSizePage; j++){
//	//				SectorTableNo2 = dataBlock*NC->blockSizePage + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//	//				block_no = getblock(simPtr, SectorTableNo2); page_no = getpage(simPtr, SectorTableNo2);
//	//				if (N->blocks[dataBlock].pages[j].valid == 1){ //原本就在data block裡的valid page
//	//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizeSector);
//	//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 % NC->blockSizeSector == page_no);
//	//					assert(N->blocks[dataBlock].pages[j].sector == SectorTableNo2);
//	//					//更新統計資料
//	//					SLB->pageRead++; SLB->pageWrite++; validIs1++;//DB: read hit, DB: write
//	//					SLB->merge_count++;
//	//				}
//	//				else if (N->blocks[dataBlock].pages[j].valid == 0){ //invalid在LB的該page
//	//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= page_no && page_no <= SLB->partial_page);
//	//					assert(N->blocks[block_no].pages[page_no].valid == 1);
//	//					N->blocks[block_no].pages[page_no].valid = 0;//invalid old
//	//					//更新統計資料
//	//					SLB->pageRead += 2; SLB->pageWrite++; validIs0++;//DB: read miss, LB: read hit, DB: write
//	//					SLB->merge_count++;
//	//					//merge_content[j / NC->trackSizePage]++;
//	//#ifdef Time_Overhead
//	//					merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->trackSizePage);
//	//					if (merge_ram_size == NC->trackSizePage){
//	//						//qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//	//						//for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
//	//						Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 1);//   / NC->CylinderSizeTrack
//	//						for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
//	//							if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){//seek
//	//								Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 1);//   / NC->CylinderSizeTrack
//	//							}
//	//						}
//	//						end_DBtrack = j / NC->trackSizePage; assert(start_DBtrack <= end_DBtrack);
//	//						for (k = start_DBtrack; k <= end_DBtrack; k++){//DB seek
//	//							SectorTableNo3 = dataBlock*NC->blockSizePage + k*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//	//							Calc_TimeOverhead2(simPtr, SectorTableNo3, 1);//寫在DB上   / NC->CylinderSizeTrack
//	//							N->Merge_Trackcount++;//
//	//							Merge_Trackcount++;
//	//						}
//	//						if ((j + 1) % NC->trackSizePage == 0){ start_DBtrack = end_DBtrack + 1; }
//	//						else{ start_DBtrack = end_DBtrack; }
//	//						assert(start_DBtrack > 0);
//	//						merge_ram_size = 0;
//	//						N->merge_ram_writecount++;//
//	//						merge_ram_writecount++;
//	//					}
//	//#endif
//	//					setblock(simPtr, SectorTableNo2, dataBlock);
//	//					setpage(simPtr, SectorTableNo2, j);
//	//					N->blocks[dataBlock].pages[j].sector = SectorTableNo2;
//	//					N->blocks[dataBlock].pages[j].valid = 1;
//	//				}
//	//				else{ printf("SLB merge error\n"); system("pause"); }
//	//			}
//	SLB->mergecnt++;
//	Victim = NC->PsizeBlock - 1;
//	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
//	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
//
//	int num = 0; assert(tsp_start <= tsp_end);
//	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
//		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
//		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
//		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
//		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
//			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
//			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
//			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
//
//			SourceTracks_count = 0;//
//#ifdef Time_Overhead
//			merge_ram_writecount = 0; Merge_Trackcount = 0;
//			N->Merge_Bandcount++;//
//			merge_ram_size = 0;
//			start_DBtrack = 0; end_DBtrack = 0;
//#endif
//			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
//			DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
//			for (j = 0; j < NC->blockSizeTrack; j++){
//				merge_content[j] = 0;
//				for (k = 0; k < NC->trackSizeSection; k++){
//					if (N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid == 0){ merge_content[j]++; }
//				}
//			}
//			for (j = 0; j < NC->blockSizeTrack; j++){
//#ifdef Time_Overhead
//				merge_ram_size = 0;
//#endif
//				for (k = 0; k < NC->trackSizeSection; k++){
//					SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage + k*NC->Section; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//					block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
//					if (N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid == 1){ //原本就在data block裡的valid page
//						assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
//						assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
//						assert(j*NC->trackSizeSection + k == section_no);
//						assert(N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].sector == SectorTableNo2);
//						//更新統計資料
//						SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
//						SLB->merge_count += NC->Section;
//					}
//					else if (N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid == 0){ //invalid在LB的該page
//						assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
//						assert(N->blocks[block_no].sections[section_no].valid == 1);
//						N->blocks[block_no].sections[section_no].valid = 0;//invalid old
//						//更新統計資料
//						SLB->pageRead += 2 * NC->Section; SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
//						SLB->merge_count += NC->Section;
//						DWORD repeat = 0;//
//						for (count = 0; count < SourceTracks_count; count++){//
//							if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
//						}
//						if (repeat == 0){ //
//							assert(SourceTracks_count<NC->blockSizeSection);
//							SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
//						}
//#ifdef Time_Overhead
//						merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= NC->trackSizeSection);
//#endif
//						setblock(simPtr, SectorTableNo2, dataBlock); assert(j*NC->trackSizeSection + k < NC->blockSizeSection);
//						setsection(simPtr, SectorTableNo2, j*NC->trackSizeSection + k);
//						N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].sector = SectorTableNo2;
//						N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid = 1;
//					}
//					else{ printf("SLB merge error\n"); system("pause"); }
//				}
//#ifdef Time_Overhead
//				if (merge_ram_size > 0){
//					assert(merge_ram_size <= NC->trackSizeSector);
//					qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//					if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }
//
//					//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
//					test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
//					//Calc_RotateOverhead2(simPtr, 0, 3);//read SLB track
//					N->content_rotate += (merge_ram_size*NC->Section / (NC->CylinderSizeTrack*NC->trackSizeSector));
//					N->Merge_LB_Trackcount++;
//					for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
//						//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//						//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
//						test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
//						if (merge_ram[k-1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 3); }
//						N->Merge_LB_Trackcount++;//
//					}
//					N->merge_ram_writecount++;//
//					merge_ram_writecount++;
//				}
//				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
//				test(simPtr, SectorTableNo3, 2);
//				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
//				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
//				N->content_rotate+=2;
//
//				N->Merge_Trackcount++;//
//				Merge_Trackcount++;
//				merge_ram_size = 0;
//#endif
//			}
//
//			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
//			N->blocks[Victim].sections[i].valid = 0;
//			N->ValidIs0 += (I64)validIs0;
//			//N->SourceTracks_count += (I64)SourceTracks_count;
//			assert(Merge_Trackcount == NC->blockSizeTrack);
//#ifdef MergeAssoc_SLBpart
//			fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs0, validIs1, merge_ram_writecount, SourceTracks_count);
//#endif
//		}
//		else {
//			NoMerge_Band++;
//			assert(SectorTableNo1 < NC->LsizePage);
//			assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
//		}
//		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
//		assert(N->blocks[Victim].sections[i].valid == 0);
//	}
//
//	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
//	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
//	//update SMR LB information
//	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
//	free(SourceTracks);
//	free(merge_content);
//#ifdef Time_Overhead
//	free(merge_ram);
//#endif
//}
void FASTmergeRWSLBpart2(sim *simPtr) {//一次merge整個data band
	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0, PageNo = 0;
	DWORD	j = 0, k = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
	//試驗
	//DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	//試驗
	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD SourceTracks_count = 0, count = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD start_DBtrack = 0, end_DBtrack = 0;

	DWORD NoMerge_Band = 0;
	//初始化 存活LB section
	//N->l2pmapOvd.round_LB_Live_Section = SLB->SLBsection_num;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	
	//			for (j = 0; j < NC->blockSizePage; j++){
	//				SectorTableNo2 = dataBlock*NC->blockSizePage + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
	//				block_no = getblock(simPtr, SectorTableNo2); page_no = getpage(simPtr, SectorTableNo2);
	//				if (N->blocks[dataBlock].pages[j].valid == 1){ //原本就在data block裡的valid page
	//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizeSector);
	//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 % NC->blockSizeSector == page_no);
	//					assert(N->blocks[dataBlock].pages[j].sector == SectorTableNo2);
	//					//更新統計資料
	//					SLB->pageRead++; SLB->pageWrite++; validIs1++;//DB: read hit, DB: write
	//					SLB->merge_count++;
	//				}
	//				else if (N->blocks[dataBlock].pages[j].valid == 0){ //invalid在LB的該page
	//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= page_no && page_no <= SLB->partial_page);
	//					assert(N->blocks[block_no].pages[page_no].valid == 1);
	//					N->blocks[block_no].pages[page_no].valid = 0;//invalid old
	//					//更新統計資料
	//					SLB->pageRead += 2; SLB->pageWrite++; validIs0++;//DB: read miss, LB: read hit, DB: write
	//					SLB->merge_count++;
	//					//merge_content[j / NC->trackSizePage]++;
	//#ifdef Time_Overhead
	//					merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->trackSizePage);
	//					if (merge_ram_size == NC->trackSizePage){
	//						//qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
	//						//for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
	//						Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 1);//   / NC->CylinderSizeTrack
	//						for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
	//							if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){//seek
	//								Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 1);//   / NC->CylinderSizeTrack
	//							}
	//						}
	//						end_DBtrack = j / NC->trackSizePage; assert(start_DBtrack <= end_DBtrack);
	//						for (k = start_DBtrack; k <= end_DBtrack; k++){//DB seek
	//							SectorTableNo3 = dataBlock*NC->blockSizePage + k*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
	//							Calc_TimeOverhead2(simPtr, SectorTableNo3, 1);//寫在DB上   / NC->CylinderSizeTrack
	//							N->Merge_Trackcount++;//
	//							Merge_Trackcount++;
	//						}
	//						if ((j + 1) % NC->trackSizePage == 0){ start_DBtrack = end_DBtrack + 1; }
	//						else{ start_DBtrack = end_DBtrack; }
	//						assert(start_DBtrack > 0);
	//						merge_ram_size = 0;
	//						N->merge_ram_writecount++;//
	//						merge_ram_writecount++;
	//					}
	//#endif
	//					setblock(simPtr, SectorTableNo2, dataBlock);
	//					setpage(simPtr, SectorTableNo2, j);
	//					N->blocks[dataBlock].pages[j].sector = SectorTableNo2;
	//					N->blocks[dataBlock].pages[j].valid = 1;
	//				}
	//				else{ printf("SLB merge error\n"); system("pause"); }
	//			}

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);

	int num = 0; assert(tsp_start <= tsp_end);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

			SourceTracks_count = 0;//
#ifdef Time_Overhead
			N->Merge_Bandcount++;//
			merge_ram_writecount = 0; Merge_Trackcount = 0;
			merge_ram_size = 0;
			start_DBtrack = 0; end_DBtrack = 0;
#endif
			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
			DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = 0; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, (I64)SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate++;
				N->MR_content_rotate++;///
			}
			//assert(N->Merge_R_SeekCount - Merge_R_SeekCount == NC->blockSizeCylinder);
#endif
			for (j = 0; j < NC->blockSizeSection; j++){
				SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->Section; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
					assert(j == section_no);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					//更新統計資料
					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
					assert(N->blocks[block_no].sections[section_no].valid == 1);
					N->blocks[block_no].sections[section_no].valid = 0;//invalid old
					//更新統計資料
					SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
					DWORD repeat = 0;//
					for (count = 0; count < SourceTracks_count; count++){//
						if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
					}
					if (repeat == 0){ //
						assert(SourceTracks_count<NC->blockSizeSection);
						SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
					}
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					setblock(simPtr, SectorTableNo2, dataBlock); assert(j < NC->blockSizeSection);
					setsection(simPtr, SectorTableNo2, j);
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//統計這個band在SLB活著的section
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

			fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs0, validIs1, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }

			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
			N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 4); }//3
				N->Merge_LB_Trackcount++;//
			}
			N->merge_ram_writecount++;//
			merge_ram_writecount++;

			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = 0; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, (I64)SectorTableNo3, 2);
				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate++;
				N->MW_content_rotate++;///

				N->Merge_Trackcount++;//
				Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			N->ValidIs0 += (I64)validIs0;
			//N->SourceTracks_count += (I64)SourceTracks_count;
			assert(Merge_Trackcount == NC->blockSizeTrack);

		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
	free(SourceTracks);
#ifdef Time_Overhead
	free(merge_ram);
#endif
}

//void FASTmergeRWSLBpart2_BitMap(sim *simPtr) {//每次只merge一條data track 0.5MB，做128次
//	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0, PageNo = 0;
//	DWORD	i = 0, j = 0, k = 0, l = 0;
//	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	DWORD validIs0 = 0, validIs1 = 0;
//	DWORD block_no = 0, section_no = 0;
//	//試驗
//	DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
//	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
//	//試驗
//	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
//	DWORD SourceTracks_count = 0, count = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(SLB->SLBsection_num, sizeof(DWORD));//NC->blockSizeSection
//	DWORD start_DBtrack = 0, end_DBtrack = 0;
//
//	DWORD NoMerge_Band = 0;
//#endif
//	SLB->mergecnt++;
//	Victim = NC->PsizeBlock - 1;
//	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
//	tsp_end = (SLB->firstRWsection + NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
//
//	int num = 0; assert(tsp_start <= tsp_end);
//	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
//
//		//if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
//		//	
//		//}
//		//else {
//		//	NoMerge_Band++;
//		//	assert(SectorTableNo1 < NC->LsizePage); assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
//		//}
//		assert(tsp_start <= i && i <= tsp_end); //if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
//		SectorTableNo1 = N->blocks[Victim].sections[i].sector;
//		dataBlock = SectorTableNo1 / NC->blockSizeSector;
//		if (0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage && N->blocks[dataBlock].Merge == 0)//SectorTableNo1 != 0x3fffffff  
//		{
//			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
//			SourceTracks_count = 0;
//#ifdef Time_Overhead
//			merge_ram_writecount = 0; Merge_Trackcount = 0;
//			N->Merge_Bandcount++;
//			merge_ram_size = 0;
//			start_DBtrack = 0; end_DBtrack = 0;
//#endif
//			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
//			DWORD start = 0, end = 0, CleanTrack = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
//			for (j = 0; j < NC->blockSizeTrack; j++){
//				merge_content[j] = 0;
//				for (k = 0; k < NC->trackSizeSection; k++){
//					if (N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid == 0){ merge_content[j]++; }
//				}
//				if (merge_content[j] == 0){ CleanTrack++; }
//			}
//			assert(CleanTrack < NC->blockSizeTrack);
//			for (j = 0; j < NC->blockSizeTrack; j++){
//#ifdef Time_Overhead
//				merge_ram_size = 0;
//#endif
//				for (k = 0; k < NC->trackSizeSection; k++){
//					SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage + k*NC->Section; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//					block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
//					if (N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid == 1){ //原本就在data block裡的valid page
//						assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
//						assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
//						assert(j*NC->trackSizeSection + k == section_no);
//						assert(N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].sector == SectorTableNo2);
//						//更新統計資料
//						SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
//						SLB->merge_count += NC->Section;
//					}
//					else if (N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid == 0){ //invalid在LB的該page
//						assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
//						assert(N->blocks[block_no].sections[section_no].valid == 1);
//						N->blocks[block_no].sections[section_no].valid = 0;//invalid old
//						N->blocks[block_no].sections[section_no].sector = 0x3fffffff;//讀完標成無效
//#ifdef Time_Overhead
//						merge_ram[merge_ram_size] = section_no; merge_ram_size++; assert(merge_ram_size <= SLB->SLBsection_num);//NC->blockSizeSection
//#endif
//						//更新統計資料
//						SLB->pageRead += 2 * NC->Section; SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
//						SLB->merge_count += NC->Section;
//						//DWORD repeat = 0;//
//						//for (count = 0; count < SourceTracks_count; count++){//
//						//	if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
//						//}
//						//if (repeat == 0){ //
//						//	assert(SourceTracks_count<NC->blockSizeSection);
//						//	SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
//						//}
//						for (l = 0; l <= SLB->Partial_Section; l++){
//							if (N->blocks[Victim].sections[l].sector == SectorTableNo2){
//								N->blocks[Victim].sections[l].valid = 0;
//								N->blocks[Victim].sections[l].sector = 0x3fffffff;//讀完標成無效
//#ifdef Time_Overhead
//								merge_ram[merge_ram_size] = l; merge_ram_size++; assert(merge_ram_size <= SLB->SLBsection_num);//NC->blockSizeSection
//#endif
//							}
//						}
//						setblock(simPtr, SectorTableNo2, dataBlock); assert(j*NC->trackSizeSection + k < NC->blockSizeSection);
//						setsection(simPtr, SectorTableNo2, j*NC->trackSizeSection + k);
//						N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].sector = SectorTableNo2;
//						N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid = 1;
//					}
//					else{ printf("SLB merge error\n"); system("pause"); }
//				}
//#ifdef Time_Overhead
//				DWORD zone_no = 0;
//				if (merge_ram_size > 0){
//					assert(merge_ram_size <= SLB->SLBsection_num);//NC->blockSizeSection
//					qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//					if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }
//
//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//					//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
//					//Calc_TimeOverhead3(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
//					test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
//					Calc_RotateOverhead2(simPtr, 0, 3);
//					N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);//寫多少sector轉多少sector
//					N->Merge_LB_Trackcount++;
//					for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
//						//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//						test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
//						//Calc_TimeOverhead3(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
//						//Calc_TimeOverhead3_SMR(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
//						//Seek_End(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);
//						if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){
//							Calc_RotateOverhead2(simPtr, 0, 3);
//						}
//						N->Merge_LB_Trackcount++;//
//					}
//					N->merge_ram_writecount++;//
//					merge_ram_writecount++;
//				}
//				SectorTableNo3 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
//				//N->content_rotate += ((double)NC->trackSizePage * 2 / (double)NC->trackSizePage);
//				//Calc_TimeOverhead3(simPtr, SectorTableNo3, 2);
//				test(simPtr, SectorTableNo3, 2);
//				N->content_rotate++;//寫多少sector轉多少sector
//				//Seek_End(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);
//				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
//				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
//				N->Merge_Trackcount++;//
//				Merge_Trackcount++;
//				merge_ram_size = 0;
//#endif
//			}
//			//N->blocks[Victim].sections[i].valid = 0; N->blocks[dataBlock].Merge = 1;
//			//N->blocks[Victim].sections[i].sector = 0x3fffffff;//讀完標成無效
//
//			assert(Merge_Trackcount == NC->blockSizeTrack); Merge_Trackcount = 0;
//			N->blocks[dataBlock].Merge = 1;
//		}
//		/*if (N->blocks[dataBlock].Merge == 0){
//		N->blocks[dataBlock].Merge = 1;
//		}*/
//		assert(N->blocks[Victim].sections[i].valid == 0);//N->blocks[Victim].pages[i].sector = 0x7fffffff;
//
//		assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
//		N->blocks[Victim].sections[i].valid = 0;
//		N->ValidIs0 += (I64)validIs0;
//		//N->SourceTracks_count += (I64)SourceTracks_count;
//
//#ifdef MergeAssoc_SLBpart
//		//fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs0, validIs1, merge_ram_writecount, SourceTracks_count);
//#endif
//	}
//
//	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
//	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
//	//update SMR LB information
//	SLB->firstRWsection = (SLB->firstRWsection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	SLB->writedSectionNum -= (NC->trackSizeSection);
//	free(SourceTracks);
//	free(merge_content);
//#ifdef Time_Overhead
//	free(merge_ram);
//#endif
//}
void FASTmergeRWSLBpart2_BitMap(sim *simPtr) {//一次merge整個data band
	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0, PageNo = 0;
	DWORD	j = 0, k = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
	//試驗
	//DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	//試驗
	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD SourceTracks_count = 0, count = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(SLB->SLBsection_num, sizeof(DWORD));
	DWORD start_DBtrack = 0, end_DBtrack = 0;

	DWORD NoMerge_Band = 0;
	//初始化 存活LB section
	//N->l2pmapOvd.round_LB_Live_Section = SLB->SLBsection_num;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);

	int num = 0; assert(tsp_start <= tsp_end);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; //assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage){
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
			//assert(N->blocks[dataBlock].Merge == 0);
			SourceTracks_count = 0;//
#ifdef Time_Overhead
			N->Merge_Bandcount++;//
			merge_ram_writecount = 0; Merge_Trackcount = 0;
			merge_ram_size = 0;
			start_DBtrack = 0; end_DBtrack = 0;
#endif
			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
			DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = 0; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, (I64)SectorTableNo3, 3);
				/*if (SectorTableNo3 / NC->Section % (NC->CylinderSizeTrack*NC->trackSizeSection) == 0){
					printf("%lu->%lf %lu\n", SectorTableNo3, N->Merge_R_SeekTime, N->Merge_R_SeekCount);
				}*/
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate++;
				N->MR_content_rotate++;///
			}
			assert(N->Merge_R_SeekCount - Merge_R_SeekCount == NC->blockSizeCylinder);
			/*if (N->Merge_R_SeekCount - Merge_R_SeekCount != NC->blockSizeCylinder){
				printf("pre %lu %lu\n", N->Merge_R_SeekCount, Merge_R_SeekCount);
				system("pause"); 
			}*/
#endif
			for (j = 0; j < NC->blockSizeSection; j++){
				SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->Section; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
					assert(j == section_no);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					//更新統計資料
					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
					assert(N->blocks[block_no].sections[section_no].valid == 1);
					N->blocks[block_no].sections[section_no].valid = 0;//invalid old
					N->blocks[block_no].sections[section_no].sector = 0x3fffffff;//讀完標成無效
					//更新統計資料
					SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
					DWORD repeat = 0;//
					for (count = 0; count < SourceTracks_count; count++){//
						if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
					}
					if (repeat == 0){ //
						assert(SourceTracks_count<NC->blockSizeSection);
						SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
					}
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= SLB->SLBsection_num);
#endif
					for (k = 0; k <= SLB->Partial_Section; k++){
						if (N->blocks[Victim].sections[k].sector == SectorTableNo2){
							N->blocks[Victim].sections[k].valid = 0;
							N->blocks[Victim].sections[k].sector = 0x3fffffff;//讀完標成無效
							//
							SLB->pageRead += NC->Section;
#ifdef Time_Overhead
							merge_ram[merge_ram_size] = k*NC->Section; merge_ram_size++; assert(merge_ram_size <= SLB->SLBsection_num);//NC->blockSizeSection
#endif
						}
					}
					setblock(simPtr, SectorTableNo2, dataBlock); assert(j < NC->blockSizeSection);
					setsection(simPtr, SectorTableNo2, j);
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//統計這個band在SLB活著的section
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

			fprintf(fp_SLB, "%lu (%lu:%lu), %lu ( %lu)\n", dataBlock, validIs0, validIs1, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= SLB->SLBsection_num);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }

			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
			N->MR_content_rotate += ((double)merge_ram_size/NC->trackSizeSection);///
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 4); }//3
				N->Merge_LB_Trackcount++;//
			}
			N->merge_ram_writecount++;//
			merge_ram_writecount++;

			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = 0; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, (I64)SectorTableNo3, 2);
				/*if (SectorTableNo3 / NC->Section % (NC->CylinderSizeTrack*NC->trackSizeSection) == 0){
					printf("%lu->%lf %lu\n", SectorTableNo3, N->Merge_W_SeekTime, N->Merge_W_SeekCount);
				}*/
				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate++;
				N->MW_content_rotate++;///

				N->Merge_Trackcount++;//
				Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);
			/*if (N->Merge_W_SeekCount - Merge_W_SeekCount != NC->blockSizeCylinder){
				printf("pos %lu %lu\n", N->Merge_W_SeekCount, Merge_W_SeekCount);
				system("pause");
			}*/
#endif

			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			N->ValidIs0 += (I64)validIs0;
			//N->SourceTracks_count += (I64)SourceTracks_count;
			assert(Merge_Trackcount == NC->blockSizeTrack);
			//N->blocks[dataBlock].Merge = 1;
		}
		else {
			NoMerge_Band++;
			//assert(SectorTableNo1 < NC->LsizePage);
			//assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
	free(SourceTracks);
#ifdef Time_Overhead
	free(merge_ram);
#endif
}

void FASTmergeRWSLBpart2_Sr(sim *simPtr) {//一次merge整個data band
	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0, PageNo = 0;
	DWORD	j = 0, k = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
	//試驗
	//DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	//試驗
	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD SourceTracks_count = 0, count = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD start_DBtrack = 0, end_DBtrack = 0;

	DWORD NoMerge_Band = 0;
	//初始化 存活LB section
	//N->l2pmapOvd.round_LB_Live_Section = SLB->SLBsection_num;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif

	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;

	//			for (j = 0; j < NC->blockSizePage; j++){
	//				SectorTableNo2 = dataBlock*NC->blockSizePage + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
	//				block_no = getblock(simPtr, SectorTableNo2); page_no = getpage(simPtr, SectorTableNo2);
	//				if (N->blocks[dataBlock].pages[j].valid == 1){ //原本就在data block裡的valid page
	//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizeSector);
	//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 % NC->blockSizeSector == page_no);
	//					assert(N->blocks[dataBlock].pages[j].sector == SectorTableNo2);
	//					//更新統計資料
	//					SLB->pageRead++; SLB->pageWrite++; validIs1++;//DB: read hit, DB: write
	//					SLB->merge_count++;
	//				}
	//				else if (N->blocks[dataBlock].pages[j].valid == 0){ //invalid在LB的該page
	//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= page_no && page_no <= SLB->partial_page);
	//					assert(N->blocks[block_no].pages[page_no].valid == 1);
	//					N->blocks[block_no].pages[page_no].valid = 0;//invalid old
	//					//更新統計資料
	//					SLB->pageRead += 2; SLB->pageWrite++; validIs0++;//DB: read miss, LB: read hit, DB: write
	//					SLB->merge_count++;
	//					//merge_content[j / NC->trackSizePage]++;
	//#ifdef Time_Overhead
	//					merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->trackSizePage);
	//					if (merge_ram_size == NC->trackSizePage){
	//						//qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
	//						//for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
	//						Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 1);//   / NC->CylinderSizeTrack
	//						for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
	//							if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){//seek
	//								Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 1);//   / NC->CylinderSizeTrack
	//							}
	//						}
	//						end_DBtrack = j / NC->trackSizePage; assert(start_DBtrack <= end_DBtrack);
	//						for (k = start_DBtrack; k <= end_DBtrack; k++){//DB seek
	//							SectorTableNo3 = dataBlock*NC->blockSizePage + k*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
	//							Calc_TimeOverhead2(simPtr, SectorTableNo3, 1);//寫在DB上   / NC->CylinderSizeTrack
	//							N->Merge_Trackcount++;//
	//							Merge_Trackcount++;
	//						}
	//						if ((j + 1) % NC->trackSizePage == 0){ start_DBtrack = end_DBtrack + 1; }
	//						else{ start_DBtrack = end_DBtrack; }
	//						assert(start_DBtrack > 0);
	//						merge_ram_size = 0;
	//						N->merge_ram_writecount++;//
	//						merge_ram_writecount++;
	//					}
	//#endif
	//					setblock(simPtr, SectorTableNo2, dataBlock);
	//					setpage(simPtr, SectorTableNo2, j);
	//					N->blocks[dataBlock].pages[j].sector = SectorTableNo2;
	//					N->blocks[dataBlock].pages[j].valid = 1;
	//				}
	//				else{ printf("SLB merge error\n"); system("pause"); }
	//			}

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);

	int num = 0; assert(tsp_start <= tsp_end);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

			SourceTracks_count = 0;//
#ifdef Time_Overhead
			N->Merge_Bandcount++;//
			merge_ram_writecount = 0; Merge_Trackcount = 0;
			merge_ram_size = 0;
			start_DBtrack = 0; end_DBtrack = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
			DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = 0; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, (I64)SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate++;
				N->MR_content_rotate++;///
			}
			//assert(N->Merge_R_SeekCount - Merge_R_SeekCount == NC->blockSizeCylinder);
#endif
			for (j = 0; j < NC->blockSizeSection; j++){
				SectorTableNo2 = dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
					assert(j == section_no);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					//更新統計資料
					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
					assert(N->blocks[block_no].sections[section_no].valid == 1);
					N->blocks[block_no].sections[section_no].valid = 0;//invalid old
					//更新統計資料
					SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
					DWORD repeat = 0;//
					for (count = 0; count < SourceTracks_count; count++){//
						if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
					}
					if (repeat == 0){ //
						assert(SourceTracks_count<NC->blockSizeSection);
						SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
					}
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					//assert(N->section_cumu[SectorTableNo2 / NC->Section]>0);
					//assert(N->section_cumu[SectorTableNo2 / NC->Section] == N->SLB_section_cumu[SectorTableNo2 / NC->Section] + N->DB_section_cumu[SectorTableNo2 / NC->Section]);
					//Cumu_Dirty_Sectors += N->section_cumu[SectorTableNo2 / NC->Section];
					//N->section_cumu[SectorTableNo2 / NC->Section] = 0;

					setblock(simPtr, SectorTableNo2, dataBlock); assert(j < NC->blockSizeSection);
					setsection(simPtr, SectorTableNo2, j);
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//統計這個band在SLB活著的section
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

			/*FILE *t2 = fopen("DBdirtyInSLB.txt", "a");
			fprintf(t2, "dataBlock %5lu, N->l2pmapOvd.AA %lu\n", dataBlock, N->l2pmapOvd.AA);
			fclose(t2);*/
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

			fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs0, validIs1, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
			N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){ 
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;//
			merge_ram_writecount++;
			//
			N->S_MRS += (I64)merge_ram_size;

			//transfer time
			N->Req_RunTime += (2 * NC->blockSizeTrack*10);
			N->Req_RunTime += ((double)merge_ram_size / NC->trackSizeSector*10);


			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = 0; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				//fprintf(t, "mw db %lu\n", SectorTableNo3);
				test(simPtr, (I64)SectorTableNo3, 2);

				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate++;
				N->MW_content_rotate++;///

				N->Merge_Trackcount++;//
				Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);
			
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//N->SourceTracks_count += (I64)SourceTracks_count;
			assert(Merge_Trackcount == NC->blockSizeTrack);
		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		fprintf(ftime, "Merge_Bands %lu, Dirty_Sectors %I64u, Cumu_Dirty_Sectors %I64u\n", Merge_Bands, Dirty_Sectors, Cumu_Dirty_Sectors);
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
	free(SourceTracks);
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
void DM_SMRmerge(sim *simPtr) {//整個data band用15MB RAM分次merge
	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0, PageNo = 0;
	DWORD	j = 0, k = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
	//試驗
	//DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	//試驗
	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD SourceTracks_count = 0, count = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD start_DBtrack = 0, end_DBtrack = 0;

	DWORD NoMerge_Band = 0;
	//初始化 存活LB section
	//N->l2pmapOvd.round_LB_Live_Section = SLB->SLBsection_num;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif

	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;

	//			for (j = 0; j < NC->blockSizePage; j++){
	//				SectorTableNo2 = dataBlock*NC->blockSizePage + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
	//				block_no = getblock(simPtr, SectorTableNo2); page_no = getpage(simPtr, SectorTableNo2);
	//				if (N->blocks[dataBlock].pages[j].valid == 1){ //原本就在data block裡的valid page
	//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizeSector);
	//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 % NC->blockSizeSector == page_no);
	//					assert(N->blocks[dataBlock].pages[j].sector == SectorTableNo2);
	//					//更新統計資料
	//					SLB->pageRead++; SLB->pageWrite++; validIs1++;//DB: read hit, DB: write
	//					SLB->merge_count++;
	//				}
	//				else if (N->blocks[dataBlock].pages[j].valid == 0){ //invalid在LB的該page
	//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= page_no && page_no <= SLB->partial_page);
	//					assert(N->blocks[block_no].pages[page_no].valid == 1);
	//					N->blocks[block_no].pages[page_no].valid = 0;//invalid old
	//					//更新統計資料
	//					SLB->pageRead += 2; SLB->pageWrite++; validIs0++;//DB: read miss, LB: read hit, DB: write
	//					SLB->merge_count++;
	//					//merge_content[j / NC->trackSizePage]++;
	//#ifdef Time_Overhead
	//					merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->trackSizePage);
	//					if (merge_ram_size == NC->trackSizePage){
	//						//qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
	//						//for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
	//						Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 1);//   / NC->CylinderSizeTrack
	//						for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
	//							if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){//seek
	//								Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 1);//   / NC->CylinderSizeTrack
	//							}
	//						}
	//						end_DBtrack = j / NC->trackSizePage; assert(start_DBtrack <= end_DBtrack);
	//						for (k = start_DBtrack; k <= end_DBtrack; k++){//DB seek
	//							SectorTableNo3 = dataBlock*NC->blockSizePage + k*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
	//							Calc_TimeOverhead2(simPtr, SectorTableNo3, 1);//寫在DB上   / NC->CylinderSizeTrack
	//							N->Merge_Trackcount++;//
	//							Merge_Trackcount++;
	//						}
	//						if ((j + 1) % NC->trackSizePage == 0){ start_DBtrack = end_DBtrack + 1; }
	//						else{ start_DBtrack = end_DBtrack; }
	//						assert(start_DBtrack > 0);
	//						merge_ram_size = 0;
	//						N->merge_ram_writecount++;//
	//						merge_ram_writecount++;
	//					}
	//#endif
	//					setblock(simPtr, SectorTableNo2, dataBlock);
	//					setpage(simPtr, SectorTableNo2, j);
	//					N->blocks[dataBlock].pages[j].sector = SectorTableNo2;
	//					N->blocks[dataBlock].pages[j].valid = 1;
	//				}
	//				else{ printf("SLB merge error\n"); system("pause"); }
	//			}

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);

	int num = 0; assert(tsp_start <= tsp_end);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			N->Merged_Band++;
			
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

			SourceTracks_count = 0;//
#ifdef Time_Overhead
			N->Merge_Bandcount++;//
			merge_ram_writecount = 0; Merge_Trackcount = 0;
			merge_ram_size = 0;
			start_DBtrack = 0; end_DBtrack = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
			DWORD merge_sector = 30720;//sector
			DWORD start = 0, end = merge_sector;
			DWORD cumu_merge_ram_size = 0;
			for (start = 0; start < NC->blockSizeSector; start += merge_sector){
				end = start + merge_sector;
				if (end > NC->blockSizeSector){ end = NC->blockSizeSector; }
#ifdef Time_Overhead
				//Merge_R_SeekCount = N->Merge_R_SeekCount;
				for (j = start; j < end; j += NC->trackSizeSector){//讀DB資料
					SectorTableNo3 = dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizeSector);
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, (I64)SectorTableNo3, 3);
					Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
					N->content_rotate++;
					N->MR_content_rotate++;///
				}
				merge_ram_size = 0;
#endif
				for (j = start; j < end; j++){
					SectorTableNo2 = dataBlock*NC->blockSizeSector + j*NC->Section; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
					block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
					if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
						assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
						assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
						assert(j == section_no);
						assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
						//更新統計資料
						SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
						SLB->merge_count += NC->Section;
					}
					else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
						assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
						assert(N->blocks[block_no].sections[section_no].valid == 1);
						N->blocks[block_no].sections[section_no].valid = 0;//invalid old
						//更新統計資料
						SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
						SLB->merge_count += NC->Section;
						DWORD repeat = 0;//
						for (count = 0; count < SourceTracks_count; count++){//
							if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
						}
						if (repeat == 0){ //
							assert(SourceTracks_count<NC->blockSizeSection);
							SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
						}
#ifdef Time_Overhead
						merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
						//assert(N->section_cumu[SectorTableNo2 / NC->Section]>0);
						//assert(N->section_cumu[SectorTableNo2 / NC->Section] == N->SLB_section_cumu[SectorTableNo2 / NC->Section] + N->DB_section_cumu[SectorTableNo2 / NC->Section]);
						//Cumu_Dirty_Sectors += N->section_cumu[SectorTableNo2 / NC->Section];
						//N->section_cumu[SectorTableNo2 / NC->Section] = 0;

						setblock(simPtr, SectorTableNo2, dataBlock); assert(j < NC->blockSizeSection);
						setsection(simPtr, SectorTableNo2, j);
						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
						N->blocks[dataBlock].sections[j].valid = 1;
						//統計這個band在SLB活著的section
					}
					else{ printf("SLB merge error\n"); system("pause"); }
				}
#ifdef Time_Overhead
				cumu_merge_ram_size += merge_ram_size;
				assert(merge_ram_size <= NC->blockSizeSection);
				if (merge_ram_size > 0){
					qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
					if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
					//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
					test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
					Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
					N->S_CT++;
					N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
					N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
					N->Merge_LB_Trackcount++;
					for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
						//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
						//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
						test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
						if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
							Calc_RotateOverhead2(simPtr, 0, 4);
							N->S_CT++;
						}//3
						N->Merge_LB_Trackcount++;//與S_MRS重複
					}
					N->merge_ram_writecount++;//
					merge_ram_writecount++;
					//
					N->S_MRS += (I64)merge_ram_size;
					merge_ram_size = 0;
				}

				//Merge_W_SeekCount = N->Merge_W_SeekCount;
				//combine DB和SLB資料,寫入備份block
				for (j = start; j < end; j += NC->trackSizeSector){
					SectorTableNo3 = dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, (I64)NC->LsizeSector + SLB->Partial_Section + j, 2);

					Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
					N->content_rotate++;
					N->MW_content_rotate++;///

					N->Merge_Trackcount++;//
					Merge_Trackcount++;
				}
				//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);
#endif
			}
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= cumu_merge_ram_size;

			N->SlotTable_Act -= N->SlotTable_Band[dataBlock].Act;
			N->SlotTable_InAct -= N->SlotTable_Band[dataBlock].InAct;
			N->SlotTable_Band[dataBlock].Act = 0;
			N->SlotTable_Band[dataBlock].InAct = 0;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

			fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs0, validIs1, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			for (start = 0; start < NC->blockSizeSector; start += merge_sector){
				end = start + merge_sector;
				if (end > NC->blockSizeSector){ end = NC->blockSizeSector; }
				for (j = start; j < end; j += NC->trackSizeSector){//讀備份DB
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, (I64)NC->LsizeSector + SLB->Partial_Section + j, 3);

					Calc_RotateOverhead2(simPtr, 0, 3);//write DB track
					N->content_rotate++;
					N->MW_content_rotate++;///

					N->Merge_Trackcount++;//
					Merge_Trackcount++;
				}
				for (j = start; j < end; j += NC->trackSizeSector){//寫原DB
					SectorTableNo3 = dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, (I64)SectorTableNo3, 2);

					Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
					N->content_rotate++;
					N->MW_content_rotate++;///

					N->Merge_Trackcount++;//
					Merge_Trackcount++;
				}
			}
			merge_ram_size = 0;
			//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//N->SourceTracks_count += (I64)SourceTracks_count;
			//assert(Merge_Trackcount == NC->blockSizeTrack);
		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		fprintf(ftime, "Merge_Bands %lu, Dirty_Sectors %I64u, Cumu_Dirty_Sectors %I64u\n", Merge_Bands, Dirty_Sectors, Cumu_Dirty_Sectors);
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
	free(SourceTracks);
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
//------
void FASTmergeRWSLBpart2_Sr_NoSt(sim *simPtr) {//一次merge整個data band
	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0, PageNo = 0;
	DWORD	j = 0, k = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
	//試驗
	//DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	//試驗
	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD SourceTracks_count = 0, count = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD start_DBtrack = 0, end_DBtrack = 0;

	DWORD NoMerge_Band = 0;
	//初始化 存活LB section
	//N->l2pmapOvd.round_LB_Live_Section = SLB->SLBsection_num;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif

	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);

	int num = 0; assert(tsp_start <= tsp_end);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

			SourceTracks_count = 0;//
#ifdef Time_Overhead
			N->Merge_Bandcount++;//
			merge_ram_writecount = 0; Merge_Trackcount = 0;
			merge_ram_size = 0;
			start_DBtrack = 0; end_DBtrack = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
			DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = 0; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, (I64)SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate++;
				N->MR_content_rotate++;///
			}
			//assert(N->Merge_R_SeekCount - Merge_R_SeekCount == NC->blockSizeCylinder);
#endif
//			for (j = 0; j < NC->blockSizeSection; j++){
//				SectorTableNo2 = dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//				block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
//				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
//					assert(j == section_no);
//					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
//					//更新統計資料
//					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
//					SLB->merge_count += NC->Section;
//				}
//				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
//					assert(N->blocks[block_no].sections[section_no].valid == 1);
//					N->blocks[block_no].sections[section_no].valid = 0;//invalid old
//					//更新統計資料
//					SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
//					SLB->merge_count += NC->Section;
//					DWORD repeat = 0;//
//					for (count = 0; count < SourceTracks_count; count++){//
//						if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
//					}
//					if (repeat == 0){ //
//						assert(SourceTracks_count<NC->blockSizeSection);
//						SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
//					}
//#ifdef Time_Overhead
//					merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
//#endif
//					//assert(N->section_cumu[SectorTableNo2 / NC->Section]>0);
//					//assert(N->section_cumu[SectorTableNo2 / NC->Section] == N->SLB_section_cumu[SectorTableNo2 / NC->Section] + N->DB_section_cumu[SectorTableNo2 / NC->Section]);
//					//Cumu_Dirty_Sectors += N->section_cumu[SectorTableNo2 / NC->Section];
//					//N->section_cumu[SectorTableNo2 / NC->Section] = 0;
//
//					setblock(simPtr, SectorTableNo2, dataBlock); assert(j < NC->blockSizeSection);
//					setsection(simPtr, SectorTableNo2, j);
//					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
//					N->blocks[dataBlock].sections[j].valid = 1;
//					//統計這個band在SLB活著的section
//				}
//				else{ printf("SLB merge error\n"); system("pause"); }
//			}

			for (j = 0; j < NC->blockSizeSection; j++){
				SectorTableNo2 = dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				//block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					//更新統計資料
					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料
					SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//統計這個band在SLB活著的section
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}

			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

			/*FILE *t2 = fopen("DBdirtyInSLB.txt", "a");
			fprintf(t2, "dataBlock %5lu, N->l2pmapOvd.AA %lu\n", dataBlock, N->l2pmapOvd.AA);
			fclose(t2);*/
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

			fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs0, validIs1, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
			N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){ 
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;//
			merge_ram_writecount++;
			//
			N->S_MRS += (I64)merge_ram_size;

			//transfer time
			N->Req_RunTime += (2 * NC->blockSizeTrack*10);
			N->Req_RunTime += ((double)merge_ram_size / NC->trackSizeSector*10);


			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = 0; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				//fprintf(t, "mw db %lu\n", SectorTableNo3);
				test(simPtr, (I64)SectorTableNo3, 2);

				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate++;
				N->MW_content_rotate++;///

				N->Merge_Trackcount++;//
				Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);

#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//N->SourceTracks_count += (I64)SourceTracks_count;
			assert(Merge_Trackcount == NC->blockSizeTrack);
		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
			//assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		fprintf(ftime, "Merge_Bands %lu, Dirty_Sectors %I64u, Cumu_Dirty_Sectors %I64u\n", Merge_Bands, Dirty_Sectors, Cumu_Dirty_Sectors);
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
	free(SourceTracks);
#ifdef Time_Overhead
	free(merge_ram);
#endif
}

int bandHasInclude(DWORD *merge_band, DWORD merge_band_num, DWORD bandno)
{
	int i;
	for (i = 0;i < merge_band_num;i++)
		if (merge_band[i] == bandno)
			return 1;
	return 0;
}
void FASTmergeRWSLBpart2_Sr_NoStNoDB(sim *simPtr) {//一次merge整個data band
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	
	//限制merge_band數量
	
	DWORD merge_band[MAX_MERGE_BAND];
	DWORD merge_band_num = 0;
	I64* delay_sector = (I64*)calloc(NC->trackSizeSector, sizeof(I64));
	DWORD delay_sector_num = 0;
	DWORD pre_i = -1;
	for (i = tsp_start;i <= tsp_end;i++)
	{
		dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
		if(bandHasInclude(merge_band,merge_band_num,dataBlock) == 0)
			if (merge_band_num == MAX_MERGE_BAND)
			{
				test(simPtr, (I64)NC->LsizeSector + i, 4);
				if (pre_i == -1 || pre_i / NC->trackSizeSector != i / NC->trackSizeSector)
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + i, 4);

				SLB->pageRead++;
				N->content_rotate++;
				N->WB_content_rotate++;
				N->S_CT++;
				N->Merge_LB_Trackcount++;//??
				N->S_MRS++;//??

				N->blocks[Victim].sections[i].valid = 0;
				delay_sector[delay_sector_num++] = N->blocks[Victim].sections[i].sector;
				pre_i = i;
			}
			else {
				merge_band[merge_band_num++] = dataBlock;
			}
	}
	N->Req_RunTime += (unsigned long long)round((double)delay_sector_num / NC->trackSizeSector * 10);
	printf("-----------------------------------finish1\n");
	//開始做merge
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			DWORD dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = 0; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//SLB統計PM會merge到那些Cylinder上//順便做
				DB_Cylinder[j / NC->CylinderSizeTrack] = 0;
				//更新統計資料
				SLB->pageRead += NC->trackSizeSector;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ assert(DB_Cylinder[j] == 0); }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				//block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					//更新統計資料
					//SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; 
					validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料 SLB->pageRead += (2 * NC->Section);
					//SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; 
					validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock

			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			//Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//更新統計資料
			SLB->pageRead += merge_ram_size;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){ 
					//Calc_RotateOverhead2(simPtr, 0, 4);
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;
			//
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
#ifdef POWERFAIL
			//整個Band做備份, 防止斷電
			PF_DiskCacheWrite(simPtr, 0);
#endif
			//combine DB和SLB資料
			for (j = 0; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				//Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
				//更新統計資料
				SLB->pageWrite += NC->trackSizeSector;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){ assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{ 
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){ assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k); 
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
		//統計Effect Merge會寫多少Data Cylinder
		DWORD EM_DC_Index = Merge_Bands;
		if (EM_DC_Index >= N->EM_DataCylinder_Len){ EM_DC_Index = N->EM_DataCylinder_Len - 1; }
		N->EM_DataCylinder[EM_DC_Index]++;
	}
	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->trackSizeSection);
	printf("-----------------------------------finish2\n");
	//繞寫回SLB尾
	
	DWORD block_no2 = 0, section_no2 = 0;
	I64 SectorTableNo = 0;
	N->content_rotate += delay_sector_num;//寫多少sector轉多少sector
	N->WB_content_rotate += delay_sector_num;//
	N->Req_RunTime += (unsigned long long)round((double)delay_sector_num / NC->trackSizeSector * 10);
	test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 10);
	printf("-----------------------------------finish31\n");
	Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 10);
	printf("-----------------------------------finish32\n");
	printf("%I64d,  %I64d\n",delay_sector_num , NC->trackSizeSector);
	for (i = 0;i < delay_sector_num;i++)
	{
		SectorTableNo = delay_sector[i];
		//printf("-----------------------------------finish321\n");
		block_no2 = SectorTableNo / NC->blockSizeSector;
		//printf("-----------------------------------finish322\n");
		section_no2 = SectorTableNo % NC->blockSizeSector;
		//printf("-----------------------------------finish323\n");
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo;
		//printf("-----------------------------------finish324\n");
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//printf("-----------------------------------finish325\n");
		printf("%I64d  %I64d\n", block_no2, section_no2);
		N->blocks[block_no2].sections[section_no2].sector = SLB->lastRWsection;
		printf("-----------------------------------finish326\n");
		SLB->pageWrite += NC->Section;
		//printf("-----------------------------------finish37\n");
		SLB->writedSectionNum++;
		//printf("-----------------------------------finish38\n");
		SLB->write_count += NC->Section;
		//printf("-----------------------------------finish39\n");
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		//printf("-----------------------------------finish330\n");
	}
	free(delay_sector);
	printf("-----------------------------------finish33\n");
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
void FASTmergeRWSLBpart2_Sr_NoStNoDB_PB(sim *simPtr) {//一次merge整個data band
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	//PB
	DWORD start_sector = 0, start_track = 0, Start_Cylinder = 0;
	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			DWORD dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			//PB
			start_sector = 0; start_track = 0;
			while (N->blocks[dataBlock].sections[start_sector].valid == 1){ start_sector++; }
			assert(start_sector < NC->blockSizeSector);
			assert(N->blocks[dataBlock].sections[start_sector].valid == 0);
			start_track = start_sector / NC->trackSizeSector;
			Start_Cylinder = start_sector / NC->CylinderSizeSector;
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//SLB統計PM會merge到那些Cylinder上//順便做
				DB_Cylinder[j / NC->CylinderSizeTrack] = 0;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = Start_Cylinder; j < NC->blockSizeCylinder; j++){ assert(DB_Cylinder[j] == 0); }
			N->PMstatistic_DB[dataBlock]++;
			for (j = start_sector; j < NC->blockSizeSector; j++){
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				//block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					//更新統計資料
					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料 SLB->pageRead += (2 * NC->Section);
					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){ 
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;
			//
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, (I64)SectorTableNo3, 2);

				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - Start_Cylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			s_DirtyCyl = Start_Cylinder; assert(DB_Cylinder[Start_Cylinder] > 0);
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){ assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{ 
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){ assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k); 
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
#endif

#ifdef Simulation_SLB_SHUFFLE
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
void FASTmergeRWSLB_Observe(sim *simPtr) {//一次merge整個data band
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	DWORD CleanRegionMAx = 0, CleanRegionTmp = 0;//clean region統計

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			DWORD dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			////確認Mapping Table沒有錯
			//char str[10] = ".txt", file_name[30] = "Band_Mapping", tmp[10] = "";
			//sprintf(tmp, "%lu", dataBlock); strcat(file_name, tmp); strcat(file_name, str);
			//FILE *fp_BandMap = fopen(file_name, "a");
			//fprintf(fp_BandMap, "merge %lu:\n", N->PMstatistic_DB[dataBlock]);
			//for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_BandMap, "%lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); } fprintf(fp_BandMap, "\n");
			//for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_BandMap, "%lu ", N->SHUFFLE_TL[dataBlock].L2P_Index[j]); } fprintf(fp_BandMap, "\n");
			//fclose(fp_BandMap);


			//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
			/*char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
			sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
			FILE *fp_FootPrint = fopen(FileName, "a");
			fprintf(fp_FootPrint, "%lu\n", N->PMstatistic_DB[dataBlock]);
			for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
			for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
			for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
			fclose(fp_FootPrint);*/

			//統計Band性質


			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
#ifdef Simulation_SLB_SHUFFLE//Shuffle只是為了找第一名
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
			j = 0;//找出DB上第一條有髒資料的P address Cylinder
			while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
			assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
			//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
			start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
			start_Track = j*NC->CylinderSizeTrack;
			start_Sector = j*NC->CylinderSizeSector;
			N->SHUFFLE_TL_OneTime[dataBlock]++;//Simulation_SLB_SHUFFLE_OBSERVE
			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
				for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= j); assert(j < NC->blockSizeCylinder);
					assert(0 <= k); assert(k < NC->blockSizeCylinder);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }

					if (SHUFFLE_TL_SWAP_SIG == 1){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeCylinder; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
			//恢復成一般情況的merge起點
			start_Sector = 0; start_Track = 0; start_Cylinder = 0;
#endif
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
			//紀錄各Band被merge幾次
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address
				}else{ printf("SLB merge error\n"); system("pause"); }
			}
			//更新統計資料
			assert(NC->blockSizeSector - start_Sector > 0);
			SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - validIs0); assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock
#ifdef Simulation_SLB_SHUFFLE
			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = 0; j < NC->blockSizeCylinder; j++){
				//確認沒有少寫
				assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
				DWORD L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];
				assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] >= 0); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
				//觀察Band間Merge過程的變化
				N->SHUFFLE_TL_OBSERVE[dataBlock].WriteFreq[L_Index] += N->SHUFFLE_TL[dataBlock].WriteFreq[j];
				N->SHUFFLE_TL_OBSERVE[dataBlock].DirtyUnit[L_Index] += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
				N->SHUFFLE_DB_Info.WriteFreq[j] += N->SHUFFLE_TL[dataBlock].WriteFreq[j];
				N->SHUFFLE_DB_Info.DirtyUnit[j] += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
				//觀察Band內段落性聚集寫入特性
				if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0){ N->BAND_MPL[dataBlock].TEMP_TIME[L_Index]++; }
				else if (N->BAND_MPL[dataBlock].PRE_DirtyUnit[L_Index] > 0 && N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){
					N->BAND_MPL[dataBlock].PERIOD[L_Index]++;
					N->BAND_MPL[dataBlock].CUMU_TIME[L_Index] += N->BAND_MPL[dataBlock].TEMP_TIME[L_Index];
					N->BAND_MPL[dataBlock].TEMP_TIME[L_Index] = 0;
				}
				N->BAND_MPL[dataBlock].PRE_DirtyUnit[L_Index] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
#endif
				N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;
			//
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			CleanRegionMAx = s_DirtyCyl + 1; CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}else { NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage); }
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
void FASTmergeRWSLB_Observe2(sim *simPtr) {//一次merge整個data band
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	DWORD CleanRegionMAx = 0, CleanRegionTmp = 0;//clean region統計

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			DWORD dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			////確認Mapping Table沒有錯
			//char str[10] = ".txt", file_name[30] = "Band_Mapping", tmp[10] = "";
			//sprintf(tmp, "%lu", dataBlock); strcat(file_name, tmp); strcat(file_name, str);
			//FILE *fp_BandMap = fopen(file_name, "a");
			//fprintf(fp_BandMap, "merge %lu:\n", N->PMstatistic_DB[dataBlock]);
			//for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_BandMap, "%lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); } fprintf(fp_BandMap, "\n");
			//for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_BandMap, "%lu ", N->SHUFFLE_TL[dataBlock].L2P_Index[j]); } fprintf(fp_BandMap, "\n");
			//fclose(fp_BandMap);


			//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
			/*char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
			sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
			FILE *fp_FootPrint = fopen(FileName, "a");
			fprintf(fp_FootPrint, "%lu\n", N->PMstatistic_DB[dataBlock]);
			for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
			for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
			for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
			fclose(fp_FootPrint);*/

			//統計Band性質


			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
#ifdef Simulation_SLB_SHUFFLE//Shuffle只是為了找第一名
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;
			assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
			j = 0;//找出DB上第一條有髒資料的P address Cylinder
			while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
			assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
			//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
			start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
			start_Track = j*NC->CylinderSizeTrack;
			start_Sector = j*NC->CylinderSizeSector;
			N->SHUFFLE_TL_OneTime[dataBlock]++;//Simulation_SLB_SHUFFLE_OBSERVE
			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
					assert(0 <= k); assert(k < NC->blockSizeCylinder);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeCylinder; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
			//恢復成一般情況的merge起點
			start_Sector = 0; start_Track = 0; start_Cylinder = 0;
#endif
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
			//紀錄各Band被merge幾次
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//更新統計資料
			assert(NC->blockSizeSector - start_Sector > 0);
			SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - validIs0); assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock
#ifdef Simulation_SLB_SHUFFLE
			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = 0; j < NC->blockSizeCylinder; j++){
				//確認沒有少寫
				assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
				DWORD L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];
				assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] >= 0); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
				//觀察Band間Merge過程的變化
				N->SHUFFLE_TL_OBSERVE[dataBlock].WriteFreq[L_Index] += N->SHUFFLE_TL[dataBlock].WriteFreq[j];
				N->SHUFFLE_TL_OBSERVE[dataBlock].DirtyUnit[L_Index] += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
				N->SHUFFLE_DB_Info.WriteFreq[j] += N->SHUFFLE_TL[dataBlock].WriteFreq[j];
				N->SHUFFLE_DB_Info.DirtyUnit[j] += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
				//觀察Band內段落性聚集寫入特性
				if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0){ N->BAND_MPL[dataBlock].TEMP_TIME[L_Index]++; }
				else if (N->BAND_MPL[dataBlock].PRE_DirtyUnit[L_Index] > 0 && N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){
					N->BAND_MPL[dataBlock].PERIOD[L_Index]++;
					N->BAND_MPL[dataBlock].CUMU_TIME[L_Index] += N->BAND_MPL[dataBlock].TEMP_TIME[L_Index];
					N->BAND_MPL[dataBlock].TEMP_TIME[L_Index] = 0;
				}
				N->BAND_MPL[dataBlock].PRE_DirtyUnit[L_Index] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
#endif
				N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;
			//
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			CleanRegionMAx = s_DirtyCyl + 1; CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else { NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage); }
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
#endif
//Cylinder_Shuffle
void FASTmergeRWSLB_SHUFFLE(sim *simPtr) {//每次都Shuffle //一次merge整個data band
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	DWORD dataSector;

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG; 
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
			j = 0;//找出DB上第一條有髒資料的P address Cylinder
			while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
			assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
			//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
			start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
			start_Track = j*NC->CylinderSizeTrack;
			start_Sector = j*NC->CylinderSizeSector;
			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
				for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }

					if (SHUFFLE_TL_SWAP_SIG == 1){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeCylinder; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j]; 
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
#endif
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
				}else{ printf("SLB merge error\n"); system("pause"); }
			}
			//更新統計資料
			assert(NC->blockSizeSector - start_Sector > 0);
			SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - validIs0); assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock
#ifdef Simulation_SLB_SHUFFLE
			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = 0; j < NC->blockSizeCylinder; j++){
				assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
void FASTmergeRWSLB_SHUFFLE2(sim *simPtr) {//每次都Shuffle //一次merge整個data band //有LRU左退
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	DWORD dataSector;

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;
			assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
			j = 0;//找出DB上第一條有髒資料的P address Cylinder
			while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
			assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
			//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
			start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
			start_Track = j*NC->CylinderSizeTrack;
			start_Sector = j*NC->CylinderSizeSector;
			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
					assert(0 <= k); assert(k < NC->blockSizeCylinder);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeCylinder; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
#endif
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//更新統計資料
				SLB->pageRead += NC->trackSizeSector;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//更新統計資料
			assert(NC->blockSizeSector - start_Sector > 0);
			//SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - validIs0); assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock
#ifdef Simulation_SLB_SHUFFLE
			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = 0; j < NC->blockSizeCylinder; j++){
				//DB_Cylinder能透過P2L_Index找到對應的DirtyUnit
				assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE//統計
				if (DB_Cylinder[j] > 0){//MFMBO_INFO紀錄的是Logical位址
					N->MFMBO_INFO[block_no].MergeFreq[j]++;
					N->MFMBO_INFO[block_no].MergeDS[j] += DB_Cylinder[j];
				}
#endif
			}
#endif
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			//Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//更新統計資料
			SLB->pageRead += merge_ram_size;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					//Calc_RotateOverhead2(simPtr, 0, 4);
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;//Statistic Change Track
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;//Statistic Merge Read Sector
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				//Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
				//更新統計資料
				SLB->pageWrite += NC->trackSizeSector;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
//Track_Shuffle
void FASTmergeRWSLB_SHUFFLE_TRACK(sim *simPtr) {//每次都Shuffle //一次merge整個data band //有LRU左退
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); //NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD dataSector;

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;
			assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
			j = 0;//找出DB上第一條有髒資料的P address Cylinder
			while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
			assert(j <= NC->blockSizeTrack - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
			//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
			start_Cylinder = j / NC->CylinderSizeTrack; //找出第一條髒cylinder上的第一條髒track和髒sector
			start_Track = j;
			start_Sector = j * NC->trackSizeSector;
			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeTrack - 1; j >= start_Track; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Track; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeTrack);
					assert(0 <= k); assert(k < NC->blockSizeTrack);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeTrack);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeTrack);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeTrack - 1)*NC->blockSizeTrack / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeTrack; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
#endif
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//更新統計資料
				SLB->pageRead += NC->trackSizeSector;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeTrack; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->trackSizeSector < NC->blockSizeTrack);
					DB_Cylinder[j / NC->trackSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//更新統計資料
			assert(NC->blockSizeSector - start_Sector > 0);
			//SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - validIs0); assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock
#ifdef Simulation_SLB_SHUFFLE
			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = 0; j < NC->blockSizeTrack; j++){
				//DB_Cylinder能透過P2L_Index找到對應的DirtyUnit
				assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE_TK//統計
				if (DB_Cylinder[j] > 0){//MFMBO_INFO紀錄的是Logical位址
					N->MFMBO_INFO[block_no].MergeFreq[j]++;
					N->MFMBO_INFO[block_no].MergeDS[j] += DB_Cylinder[j];
				}
#endif
			}
#endif
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			//Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//更新統計資料
			SLB->pageRead += merge_ram_size;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					//Calc_RotateOverhead2(simPtr, 0, 4);
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;//Statistic Change Track
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;//Statistic Merge Read Sector
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
#ifdef POWERFAIL
			PF_DiskCacheWrite(simPtr, start_Track);
#endif
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				//Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
				//更新統計資料
				SLB->pageWrite += NC->trackSizeSector;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Track / NC->CylinderSizeTrack);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeTrack); s_DirtyCyl = j;
			j = NC->blockSizeTrack - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeTrack, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeTrack - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeTrack){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeTrack){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeTrack){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeTrack >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeTrack - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeTrack - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeTrack - e_DirtyCyl);
				if ((NC->blockSizeTrack - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeTrack - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
void FASTmergeRWSLB_ST_VT(sim *simPtr) {//每次都Shuffle //一次merge整個data band //有LRU左退
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); //NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD dataSector;

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->trackSizeSection - 1) % (SLB->Partial_Section + 1); 
	//(SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->trackSizeSection == 1024);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;
			assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
			j = 0;//找出DB上第一條有髒資料的P address Cylinder
			while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
			assert(j <= NC->blockSizeTrack - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
			//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
			start_Cylinder = j / NC->CylinderSizeTrack; //找出第一條髒cylinder上的第一條髒track和髒sector
			start_Track = j;
			start_Sector = j * NC->trackSizeSector;
			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeTrack - 1; j >= start_Track; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Track; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeTrack);
					assert(0 <= k); assert(k < NC->blockSizeTrack);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeTrack);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeTrack);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeTrack - 1)*NC->blockSizeTrack / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeTrack; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
#endif
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//更新統計資料
				SLB->pageRead += NC->trackSizeSector;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeTrack; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->trackSizeSector < NC->blockSizeTrack);
					DB_Cylinder[j / NC->trackSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//更新統計資料
			assert(NC->blockSizeSector - start_Sector > 0);
			//SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - validIs0); assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock
#ifdef Simulation_SLB_SHUFFLE
			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = 0; j < NC->blockSizeTrack; j++){
				//DB_Cylinder能透過P2L_Index找到對應的DirtyUnit
				assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			//Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//更新統計資料
			SLB->pageRead += merge_ram_size;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					//Calc_RotateOverhead2(simPtr, 0, 4);
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;//Statistic Change Track
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;//Statistic Merge Read Sector
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				//Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
				//更新統計資料
				SLB->pageWrite += NC->trackSizeSector;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Track / NC->CylinderSizeTrack);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeTrack); s_DirtyCyl = j;
			j = NC->blockSizeTrack - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeTrack, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeTrack - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeTrack){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeTrack){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeTrack){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeTrack >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeTrack - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeTrack - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeTrack - e_DirtyCyl);
				if ((NC->blockSizeTrack - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeTrack - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	//SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	//SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	//for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
	//	N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
	//	N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	//}

	//修改
	SLB->firstRWsection = (SLB->firstRWsection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	DWORD tmpSC = SLB->guardTrack1stSection;
	for (i = 0; i < NC->CylinderSizeTrack; i++){
		assert(tmpSC % NC->trackSizeSector == 0);
		for (j = tmpSC; j < tmpSC + NC->trackSizeSector; j++){
			N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0xffffffffffff;
			assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
		}
		tmpSC += NC->trackSizeSector; if (tmpSC > SLB->Partial_Section){ assert(tmpSC == SLB->Partial_Section + 1); tmpSC = 0; }
	}
	SLB->writedSectionNum -= NC->trackSizeSection; 
	//(NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
void FASTmergeRWSLB_ST_VTLMA(sim *simPtr) {//每次都Shuffle //一次merge整個data band //有LRU左退
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); //NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD dataSector;

	//新增
	const DWORD MA = Limited_MergeAssociativity;// 1;
	DWORD *LPB_BUFF = (DWORD*)calloc(NC->trackSizeSector, sizeof(DWORD));//LoopBand
	DWORD LPB_BUFF_Len = 0;

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	//(SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->trackSizeSection == 1024);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

			//新增
			if (Merge_Bands > MA){
				assert(N->blocks[dataBlock].sections[dataSector].sector == i);
				N->blocks[Victim].sections[i].valid = 0;
				N->blocks[dataBlock].sections[dataSector].valid = 1;
				N->blocks[dataBlock].sections[dataSector].sector = dataBlock*NC->blockSizeSector + dataSector;
				assert(LPB_BUFF_Len < NC->trackSizeSector);
				LPB_BUFF[LPB_BUFF_Len] = dataBlock*NC->blockSizeSector + dataSector; LPB_BUFF_Len++;
				continue;
			}

#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;
			assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
			j = 0;//找出DB上第一條有髒資料的P address Cylinder
			while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
			assert(j <= NC->blockSizeTrack - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
			//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
			start_Cylinder = j / NC->CylinderSizeTrack; //找出第一條髒cylinder上的第一條髒track和髒sector
			start_Track = j;
			start_Sector = j * NC->trackSizeSector;
			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeTrack - 1; j >= start_Track; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Track; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeTrack);
					assert(0 <= k); assert(k < NC->blockSizeTrack);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeTrack);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeTrack);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeTrack - 1)*NC->blockSizeTrack / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeTrack; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
#endif
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//更新統計資料
				SLB->pageRead += NC->trackSizeSector;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeTrack; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->trackSizeSector < NC->blockSizeTrack);
					DB_Cylinder[j / NC->trackSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//更新統計資料
			assert(NC->blockSizeSector - start_Sector > 0);
			//SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - validIs0); assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock
#ifdef Simulation_SLB_SHUFFLE
			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = 0; j < NC->blockSizeTrack; j++){
				//DB_Cylinder能透過P2L_Index找到對應的DirtyUnit
				assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			//Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//更新統計資料
			SLB->pageRead += merge_ram_size;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					//Calc_RotateOverhead2(simPtr, 0, 4);
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;//Statistic Change Track
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;//Statistic Merge Read Sector
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				//Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
				//更新統計資料
				SLB->pageWrite += NC->trackSizeSector;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Track / NC->CylinderSizeTrack);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeTrack); s_DirtyCyl = j;
			j = NC->blockSizeTrack - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeTrack, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeTrack - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeTrack){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeTrack){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeTrack){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeTrack >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeTrack - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeTrack - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeTrack - e_DirtyCyl);
				if ((NC->blockSizeTrack - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeTrack - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//新增
	assert(LPB_BUFF_Len <= NC->trackSizeSector);
	for (i = 0; i < LPB_BUFF_Len; i++){
		SectorTableNo2 = LPB_BUFF[i];
		dataBlock = SectorTableNo2 / NC->blockSizeSector;
		dataSector = SectorTableNo2 % NC->blockSizeSector;
		assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
		assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;//維持一般Dirty Sector狀態
		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		N->blocks[dataBlock].sections[dataSector].valid = 0;
		N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
	}
#ifdef Time_Overhead
	assert(LPB_BUFF_Len <= NC->trackSizeSector);
	if (LPB_BUFF_Len > 0){//新增 //N->VC_BUFF_Len > 0
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		//Delay Sector做Write Back, 成本另外算
		test(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section, 10);//Read
		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);//Write
		for (i = 0; i < LPB_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Read
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section + i, 10);//Read
		}
		for (i = 0; i < LPB_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Write
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section + i, 10);//Write
		}//把DBtrack舊資料整條讀到merge_ram裡
		//統計
		SLB->pageRead += LPB_BUFF_Len;
		SLB->pageWrite += LPB_BUFF_Len;
		//統計content_rotate
		N->content_rotate += 2 * LPB_BUFF_Len;
		N->WB_content_rotate += LPB_BUFF_Len;//Read
		N->WW_content_rotate += LPB_BUFF_Len;//Write
		//transfer time
		N->Req_RunTime += 2 * (unsigned long long)round((double)LPB_BUFF_Len / NC->trackSizeSector * 10);
		assert(N->Req_RunTime >= 0);
	}
#endif

	//update SMR LB information
	//SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	//SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	//for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
	//	N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
	//	N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	//}

	//修改
	SLB->firstRWsection = (SLB->firstRWsection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	DWORD tmpSC = SLB->guardTrack1stSection;
	for (i = 0; i < NC->CylinderSizeTrack; i++){
		assert(tmpSC % NC->trackSizeSector == 0);
		for (j = tmpSC; j < tmpSC + NC->trackSizeSector; j++){
			N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0xffffffffffff;
			assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
		}
		tmpSC += NC->trackSizeSector; if (tmpSC > SLB->Partial_Section){ assert(tmpSC == SLB->Partial_Section + 1); tmpSC = 0; }
	}
	SLB->writedSectionNum -= NC->trackSizeSection;
	//(NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
	//新增
	free(LPB_BUFF);
}

//void FASTmergeRWSLB_DELAY_SHUFFLE(sim *simPtr) {//一次merge整個data band
//	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
//	DWORD	i = 0, j = 0, k = 0;
//	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//
//	DWORD validIs0 = 0, validIs1 = 0;
//	DWORD block_no = 0, section_no = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
//	DWORD NoMerge_Band = 0;
//	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
//#endif
//	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
//	DWORD Merge_Bands = 0;
//	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
//	//SLB統計PM會merge到那些Cylinder上
//	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
//	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
//	DWORD dataSector;
//	//Delay clean region統計
//	DWORD DELAY_CleanRegionMAx = 0;//MAX
//	DWORD DELAY_CleanRegion_StartCyl = 0, DELAY_CleanRegion_EndCyl = 0;
//	DWORD DELAY_CleanRegionTmp = 0;//tmp
//	DWORD DELAY_CleanRegion_StartCylTmp = 0;
//	
//	SLB->mergecnt++;
//	Victim = NC->PsizeBlock - 1;
//	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
//	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
//	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
//	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);
//
//	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
//	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
//		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
//		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
//		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
//		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
//			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
//			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
//			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
//			assert(N->DBalloc[dataBlock] == 1);
//			assert(N->blocks[dataBlock].sections != NULL);
//			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
//			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
//#ifdef Time_Overhead
//			N->Merge_Bandcount++;
//			merge_ram_size = 0;
//#endif
//			//紀錄每次有效merge帶走多少band
//			Merge_Bands++;
//			validIs0 = 0; validIs1 = 0;
//			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
//#ifdef Simulation_SLB_DELAY
//			const DWORD DirtyUnit_TD = NC->CylinderSizeSector, SaveCylinder_TD = 8;
//			const DWORD SLB_DelaySpace = NC->blockSizeSector;
//			DWORD DELAY_SIG = 0;
//			//限定Band Merge含第二次以上才可以Delay
//			if (N->PMstatistic_DB[dataBlock] >= 1){ 
//				//找出DB上第一條有髒資料的P address Cylinder
//				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
//				j = 0; while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
//				assert(j < NC->blockSizeCylinder); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
//				start_Cylinder = j;
//				//找出Max CR //MAX
//				DELAY_CleanRegionMAx = start_Cylinder + 1; //紀錄初始最左Max CR的大小
//				DELAY_CleanRegion_StartCyl = 0; DELAY_CleanRegion_EndCyl = start_Cylinder;//紀錄初始最左Max CR的起點, 終點
//				//tmp
//				DELAY_CleanRegionTmp = 0;//從Band剩下的CR裡面找出Max CR
//				DELAY_CleanRegion_StartCylTmp = 0; //用不到 DELAY_CleanRegion_EndCylTmp = 0;
//				//開始找更大的CR
//				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
//					if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0 && j + 1 < NC->blockSizeCylinder){
//						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] >= 0);
//						if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] == 0){ //累積連續CR空間
//							DELAY_CleanRegionTmp++;
//							if (DELAY_CleanRegionTmp == 1){ DELAY_CleanRegion_StartCylTmp = j; }
//						}else{
//							//N->CleanRegion_Count++; N->CleanRegion_CumuSize += DELAY_CleanRegionTmp;
//							if (DELAY_CleanRegionTmp > DELAY_CleanRegionMAx){
//								DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
//								DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
//								DELAY_CleanRegion_EndCyl = j - 1;
//							}
//							DELAY_CleanRegionTmp = 0;//找到,統計完歸0 
//						}
//					}
//				}
//
//				//確保最大CR不是在最左邊, 才有Delay價值
//				DWORD DelaySector = 0, DelaySector_Finish = 0;//以下三條件都符合就確定Delay
//				if (DELAY_CleanRegion_StartCyl != 0){
//					assert(start_Cylinder < DELAY_CleanRegion_StartCyl);
//					for (j = start_Cylinder; j < DELAY_CleanRegion_StartCyl; j++){
//						DelaySector += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
//					}
//					//Max CR以左的Dirty Sector <= Dirty門檻
//					if (DelaySector <= DirtyUnit_TD){ DELAY_SIG++; }
//					//SLB Delay Space還有空間
//					if (N->SLB_DELAY_USE + DelaySector <= N->SLB_DELAY_SPACE){ DELAY_SIG++; }
//					//Delay省的Merge寫入 >= Save Region門檻
//					if (DELAY_CleanRegion_EndCyl + 1 - start_Cylinder >= SaveCylinder_TD){ DELAY_SIG++; }
//				}
//				if (DELAY_SIG == 3){
//					//發動Delay
//					N->SLB_DELAY_USE += DelaySector;
//					//幫新來的標上3, 要被Shuffle的恢復成0
//					DWORD L_Index, L_SectorNo;
//					for (j = start_Cylinder; j < DELAY_CleanRegion_StartCyl; j++){
//						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L
//						for (k = 0; k < NC->CylinderSizeSector; k++){
//							L_SectorNo = L_Index*NC->CylinderSizeSector + k;
//							if (N->blocks[dataBlock].sections[L_SectorNo].valid == 0){}//是新來的 -> SLB_DELAY_USE追加新來的
//							else if (N->blocks[dataBlock].sections[L_SectorNo].valid == 3){}//沒事
//
//							//目前Band的valid 3出現在Victim Cylinder上先讀出來暫存 -> A
//							//assert(VC_BUFF_Len < NC->CylinderSizeSector);
//							//VictimCylinder_BUFF[VC_BUFF_Len] = dataBlock*NC->blockSizeSector + L_SectorNo; VC_BUFF_Len++;
//						}
//					}//確定Max CR裡真的沒東西
//					for (j = DELAY_CleanRegion_StartCyl; j < DELAY_CleanRegion_EndCyl; j++){
//						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L
//						for (k = 0; k < NC->CylinderSizeSector; k++){
//							L_SectorNo = L_Index*NC->CylinderSizeSector + k;
//							assert(N->blocks[dataBlock].sections[L_SectorNo].valid == 1);
//						}
//					}//Shuffle Range有多少是valid 3被Shuffle -> SLB_DELAY_USE扣掉被Shuffe的
//					for (j = DELAY_CleanRegion_EndCyl; j < NC->blockSizeCylinder; j++){
//						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L
//						for (k = 0; k < NC->CylinderSizeSector; k++){
//							L_SectorNo = L_Index*NC->CylinderSizeSector + k;
//							if (N->blocks[dataBlock].sections[L_SectorNo].valid == 3){ 
//								DelaySector_Finish++; 
//								//N->blocks[dataBlock].sections[L_SectorNo].valid = 0;
//							}//上次的Delay Sector這次被merge到, 改成valid 0
//						}
//						N->SLB_DELAY_USE -= DelaySector_Finish;//SLB_DELAY_USE扣掉被Shuffe的
//					}
//
//					//這回合被決定Delay的valid 3留在SLB原位置
//					//shuffle非Delay的剩下Cylinder, 做Merge //改動start_Cylinder, 要等該標的delay標完
//					//start_Cylinder = DELAY_CleanRegion_EndCyl + 1; assert(start_Cylinder < NC->blockSizeCylinder);
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Cylinder] > 0);
//				}
//				else{
//					//如果曾經Delay, 歸還SLB Delay Space
//					for (j = 0; j < NC->blockSizeSector; j++){
//						if (N->blocks[dataBlock].sections[j].valid == 3){ 
//							DelaySector_Finish++; 
//							//N->blocks[dataBlock].sections[j].valid = 0;
//						}//曾經Delay, N->blocks[dataBlock].sections[j].valid = 0
//					}
//					N->SLB_DELAY_USE -= DelaySector_Finish;//加總後扣給SLB_DELAY_USE
//				}
//			}
//#endif
//#ifdef Simulation_SLB_SHUFFLE
//			//找出DB上最左第一個CR, 得出需要merge的cylinder數
//			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
//			DWORD SHUFFLE_TL_SWAP_SIG; 
//			DWORD tmp_WriteFreq, tmp_DirtyUnit;
//			DWORD Lj, Pj, Lk, Pk;
//			assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
//			j = 0;//找出DB上第一條有髒資料的P address Cylinder
//			while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
//			assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
//			//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
//			start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
//			start_Track = j*NC->CylinderSizeTrack;
//			start_Sector = j*NC->CylinderSizeSector;
//			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
//			for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
//				for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
//					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
//					assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
//					if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
//					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//
//					if (SHUFFLE_TL_SWAP_SIG == 1){
//						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
//						Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
//						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
//						//備份K位置上的資料
//						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
//						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
//						//j給k, Pk->Lj, B->A
//						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
//						N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
//						N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
//						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
//						//tmp給j, Pj->Lk, A->B
//						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
//						N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
//						N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
//						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
//						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
//					}
//				}
//				if (j == 0){ break; }//j,k是DWORD, 沒有負數
//			}//檢查每一個L都有唯一對應到P,反之亦是
//			const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
//			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
//			for (j = 0; j < NC->blockSizeCylinder; j++){
//				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j]; 
//				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
//			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
//#endif
//#ifdef Time_Overhead
//			Merge_R_SeekCount = N->Merge_R_SeekCount;
//			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
//				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//				test(simPtr, SectorTableNo3, 3);
//				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
//				N->content_rotate += NC->trackSizeSector;
//				N->MR_content_rotate += NC->trackSizeSector;
//				N->Req_RunTime += 10;
//			}
//#endif
//			//SLB統計PM會merge到那些Cylinder上
//			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
//			N->PMstatistic_DB[dataBlock]++;
//			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
//				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//				block_no = dataBlock; section_no = j;
//				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
//					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
//				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
//					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
//					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
//					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
//					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
//					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
//					//更新統計資料
//					validIs0 += NC->Section;
//#ifdef Time_Overhead
//					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
//#endif
//					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
//					N->blocks[dataBlock].sections[j].valid = 1;
//					//SLB統計PM會merge到那些Cylinder上
//					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
//					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
//				}else{ printf("SLB merge error\n"); system("pause"); }
//			}
//			//更新統計資料
//			assert(NC->blockSizeSector - start_Sector > 0);
//			SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
//			validIs1 += (NC->blockSizeSector - start_Sector - validIs0); assert(NC->blockSizeSector - start_Sector >= validIs0);
//			SLB->merge_count += (NC->blockSizeSector - start_Sector);
//			//歸還DB的sectors記憶體
//			assert(N->DBalloc[dataBlock] == 1);
//			free(N->blocks[dataBlock].sections);
//			N->DBalloc[dataBlock] = 0;
//			N->blocks[block_no].sections = NULL;//dataBlock
//#ifdef Simulation_SLB_SHUFFLE
//			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
//			for (j = 0; j < NC->blockSizeCylinder; j++){
//				assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
//				N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
//				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
//			}
//#endif
//			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
//			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
//			Dirty_Sectors += (I64)merge_ram_size;
//			N->req_w_cumu += Cumu_Dirty_Sectors;
//#ifdef MergeAssoc_SLBpart
//			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
//			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
//			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
//			N->l2pmapOvd.AA -= merge_ram_size;
//			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
//			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
//			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
//			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
//			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
//#endif
//#ifdef Time_Overhead
//			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
//			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
//			//acctime
//			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
//			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
//			N->S_CT++;
//			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
//			N->content_rotate += merge_ram_size;
//			N->MR_content_rotate += merge_ram_size;
//			N->Merge_LB_Trackcount++;
//			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
//				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
//					Calc_RotateOverhead2(simPtr, 0, 4);
//					N->S_CT++;
//				}//3
//				N->Merge_LB_Trackcount++;//與S_MRS重複
//			}
//			N->merge_ram_writecount++; //
//			N->S_MRS += (I64)merge_ram_size;
//			//transfer time
//			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
//			//確認seek次數正確
//			Merge_W_SeekCount = N->Merge_W_SeekCount;
//			//combine DB和SLB資料
//			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
//				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//				test(simPtr, (I64)SectorTableNo3, 2);
//				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
//				N->content_rotate += NC->trackSizeSector;
//				N->MW_content_rotate += NC->trackSizeSector;
//				N->Req_RunTime += 10;
//				//
//				N->Merge_Trackcount++;
//			}
//			merge_ram_size = 0;
//			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);
//#endif
//			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
//			N->blocks[Victim].sections[i].valid = 0;
//			//SLB統計PM會merge到那些Cylinder上
//			j = 0; while (DB_Cylinder[j] == 0){ j++; }
//			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
//			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
//			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
//			//找到s_DirtyCyl, e_DirtyCyl
//			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
//			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
//			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
//			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
//			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
//				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
//					assert(DB_Cylinder[j + 1] >= 0);
//					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
//					else{
//						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
//						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
//						CleanRegionTmp = 0;//找到,統計完歸0 
//					}
//				}
//				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
//					N->LB_DirtySectors += DB_Cylinder[j];
//					N->DirtyInRange_Cylinders++;
//					if (j + 1 < NC->blockSizeCylinder){
//						assert(DB_Cylinder[j + 1] >= 0);
//						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
//							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
//							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
//							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
//							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
//						}
//					}
//				}
//			}
//			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
//				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
//				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
//			}
//			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
//			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
//		}else {
//			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
//		}
//		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
//		//assert(N->blocks[Victim].sections[i].valid == 0);
//		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
//	}
//
//#ifdef Simulation_SLB_DELAY
//	//這回合被決定Delay的valid 3留在SLB原位置
//	//非這輪merge的Band的valid 3暫時跳過, 等merge結束再讀起來 -> B
//	for (i = tsp_start; i <= tsp_end; i++){
//		if (N->blocks[Victim].sections[i].valid == 3){
//			//assert(VC_BUFF_Len < NC->CylinderSizeSector);
//			//VictimCylinder_BUFF[VC_BUFF_Len] = dataBlock*NC->blockSizeSector + L_SectorNo; VC_BUFF_Len++;
//		}
//		assert(N->blocks[Victim].sections[i].valid == 0);
//	}
//	//merge結束, A+B繞寫回SLB lastRW Cylinder
//	/*for (i = 0; i<N->VC_BUFF_Len; i++){
//		
//	}*/
//#endif
//	if (Merge_Bands > 0){
//		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
//		N->Effect_Merge++;
//	}
//	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
//	//update SMR LB information
//	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
//		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
//		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
//	}
//	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
//#ifdef Time_Overhead
//	free(merge_ram);
//#endif
//	//SLB統計PM會merge到那些Cylinder上
//	free(DB_Cylinder);
//}
/*跑MaxCR的Delay預測, Delay Sector沒有留在SLB*/ 
void FASTmergeRWSLB_DELAY_tmp_SHUFFLE(sim *simPtr) {//一次merge整個data band
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	DWORD dataSector;
	DWORD LBsector;
	//Delay clean region統計
	DWORD DELAY_CleanRegionMAx = 0;//MAX
	DWORD DELAY_CleanRegion_StartCyl = 0, DELAY_CleanRegion_EndCyl = 0;
	DWORD DELAY_CleanRegionTmp = 0;//tmp
	DWORD DELAY_CleanRegion_StartCylTmp = 0;

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	//tsp is "trackSizePage"
	const DWORD tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	const DWORD tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);
#ifdef Simulation_SLB_DELAY
	N->VC_BUFF_Len = 0;//暫存Delay Sector
#endif
	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
			DWORD DELAY_SIG = 0;
#ifdef Simulation_SLB_DELAY
			const DWORD DirtyUnit_TD = NC->CylinderSizeSector, SaveCylinder_TD = 8;
			//限定Band Merge含第二次以上才可以Delay
			if (N->PMstatistic_DB[dataBlock] >= 1){ 
				//找出DB上第一條有髒資料的P address Cylinder
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0; while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
				assert(j < NC->blockSizeCylinder); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				start_Cylinder = j;
				//找出Max CR //MAX
				DELAY_CleanRegionMAx = start_Cylinder + 1; //紀錄初始最左Max CR的大小
				DELAY_CleanRegion_StartCyl = 0; DELAY_CleanRegion_EndCyl = start_Cylinder;//紀錄初始最左Max CR的起點, 終點
				//tmp
				DELAY_CleanRegionTmp = 0;//從Band剩下的CR裡面找出Max CR
				DELAY_CleanRegion_StartCylTmp = 0; //用不到 DELAY_CleanRegion_EndCylTmp = 0;
				//開始找更大的CR
				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
					if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0 && j + 1 < NC->blockSizeCylinder){//最右邊01有處理,但是00沒處理
						//保證Dirty_j是0
						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] >= 0);
						if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] == 0){ //累積連續CR空間
							DELAY_CleanRegionTmp++;
							if (DELAY_CleanRegionTmp == 1){ DELAY_CleanRegion_StartCylTmp = j; }
						}else{//Dirty_j+1是>0
							if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
								DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
								DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
								DELAY_CleanRegion_EndCyl = j;
							}
							DELAY_CleanRegionTmp = 0;//找到,統計完歸0 
						}
					}
				}
				assert(DELAY_CleanRegion_StartCyl <= DELAY_CleanRegion_EndCyl);
				assert(DELAY_CleanRegion_EndCyl < NC->blockSizeCylinder);
				//確保最大CR不是在最左邊, 才有Delay價值
				DWORD DelaySector = 0;
				DWORD L_Index, LSN;//邏輯位址, LSN: L_SectorNo
				if (DELAY_CleanRegion_StartCyl != 0){
					assert(start_Cylinder < DELAY_CleanRegion_StartCyl);
					//Delay統計預先跑一次, 真的要做再跑流程
					//換到邏輯位址計算Delay和一般sector的量, 確認Max CR上沒有髒
					for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L, Logical Cylinder No
						for (k = 0; k < NC->CylinderSizeSector; k++){
							LSN = L_Index*NC->CylinderSizeSector + k;
							if (start_Cylinder <= j && j < DELAY_CleanRegion_StartCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 0 || N->blocks[dataBlock].sections[LSN].valid == 1);
								if (N->blocks[dataBlock].sections[LSN].valid == 0){ DelaySector++; }//可能會Delay的段落
							}else if (DELAY_CleanRegion_StartCyl <= j && j < DELAY_CleanRegion_EndCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 1);//確定Max CR裡真的沒東西
							}else{} //要Shuffle的段落
						}
					}
					//以下三條件都符合就確定Delay
					if (DelaySector <= DirtyUnit_TD){ DELAY_SIG++; }//Max CR以左的Dirty Sector <= Dirty門檻
					if (N->SLB_DELAY_USE + DelaySector <= N->SLB_DELAY_SPACE){ DELAY_SIG++; }//SLB Delay Space還有空間
					if (DELAY_CleanRegion_EndCyl + 1 - start_Cylinder >= SaveCylinder_TD){ DELAY_SIG++; }//Delay省的Merge寫入 >= Save Region門檻
				}
				if (DELAY_SIG == 3){
					////N->VC_BUFF_Len = 0;//暫存victim上Delay Sector, 等Merge結束寫到lastRW
					//for (j = start_Cylinder; j < DELAY_CleanRegion_StartCyl; j++){//Delay的段落
					//	L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L
					//	for (k = 0; k < NC->CylinderSizeSector; k++){
					//		LSN = L_Index*NC->CylinderSizeSector + k;
					//		I64 Band_SectorValid = N->blocks[dataBlock].sections[LSN].valid;
					//		SectorTableNo2 = dataBlock*NC->blockSizeSector + LSN;
					//		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
					//		if (Band_SectorValid == 0){
					//			LBsector = N->blocks[dataBlock].sections[LSN].sector;
					//			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					//			//確認DB與SLB對應雙向都正確
					//			assert(N->blocks[Victim].sections[LBsector].valid == 1);
					//			assert(N->blocks[Victim].sections[LBsector].sector = SectorTableNo2);
					//		}
					//	}
					//}assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
					////for (j = DELAY_CleanRegion_EndCyl; j < NC->blockSizeCylinder; j++){}//要Shuffle的段落

					//N->SLB_DELAY_USE = N->SLB_DELAY_USE + DelaySector - DelaySector_Finish;
					//shuffle非Delay的剩下Cylinder, 做Merge //改動start_Cylinder, 要等該標的delay標完
					start_Cylinder = DELAY_CleanRegion_EndCyl + 1; assert(start_Cylinder < NC->blockSizeCylinder);
					start_Track = start_Cylinder*NC->CylinderSizeTrack;
					start_Sector = start_Cylinder*NC->CylinderSizeSector;
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Cylinder] > 0);
				}
				else{
					assert(DELAY_SIG < 3); //如果曾經Delay, 歸還SLB Delay Space
					//歸還: 加總後扣給SLB_DELAY_USE
					//N->SLB_DELAY_USE -= DelaySector_Finish;
				}
			}
#endif

			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 3){}//Delay
				else if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Time_Overhead //不是Delay Sector
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
#ifdef Simulation_SLB_DELAY
					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
					//if (DELAY_SIG == 3 && P_Index < DELAY_CleanRegion_StartCyl){//SHuffle過P_Index < DELAY_CleanRegion_StartCyl不保證是Delay Sector
					//	if (tsp_start <= LBsector && LBsector <= tsp_end){//Victim Cylinder上免費還
					//		N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
					//		N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					//		N->blocks[dataBlock].sections[j].valid = 1;
					//		//暫存等Merge結束寫到lastRW
					//		assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
					//		N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
					//		
					//		assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[N->SHUFFLE_TL[dataBlock].L2P_Index[j]] > 0);
					//		N->SHUFFLE_TL[dataBlock].DirtyUnit[N->SHUFFLE_TL[dataBlock].L2P_Index[j]] -= 1;
					//	}else{ }//不在Victim Cylinder上, 暫不處理
					//	continue;
					//}
#endif
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
				}else{ printf("SLB merge error\n"); system("pause"); }
			}

#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;

			if (DELAY_SIG != 3){
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0;//找出DB上第一條有髒資料的P address Cylinder
				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
				assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
				start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = j*NC->CylinderSizeTrack;
				start_Sector = j*NC->CylinderSizeSector;
				for (j = 0; j < start_Cylinder; j++){
					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
				}
			}

			////剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			//for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
			//	for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
			//		SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
			//		assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
			//		if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }

			//		if (SHUFFLE_TL_SWAP_SIG == 1){
			//			//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
			//			Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
			//			Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
			//			//備份K位置上的資料
			//			tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
			//			//j給k, Pk->Lj, B->A
			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
			//			N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
			//			assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
			//			//tmp給j, Pj->Lk, A->B
			//			assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
			//			N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
			//		}
			//	}
			//	if (j == 0){ break; }//j,k是DWORD, 沒有負數
			//}//檢查每一個L都有唯一對應到P,反之亦是
			
			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
					assert(0 <= k); assert(k < NC->blockSizeCylinder);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeCylinder; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
#endif
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
			}
#endif
#ifdef Simulation_SLB_SHUFFLE
			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = 0; j < NC->blockSizeCylinder; j++){//0 start_Cylinder
				//assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
					printf("%lu: %lu %lu: %lu\n", N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
					system("pause");
				}
				N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//更新統計資料
			assert(start_Sector <= NC->blockSizeSector);//等於是因為Delay
			SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - merge_ram_size); //assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//歸還DB的sectors記憶體
			//assert(N->DBalloc[dataBlock] == 1);
			//free(N->blocks[dataBlock].sections);
			//N->DBalloc[dataBlock] = 0;
			//N->blocks[block_no].sections = NULL;//dataBlock

			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
			}
			//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
			merge_ram_size = 0;
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		//assert(N->blocks[Victim].sections[i].valid == 0);
		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Simulation_SLB_DELAY
	////DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
	//assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
	//for (i = 0; i < N->VC_BUFF_Len; i++){
	//	SectorTableNo2 = N->VictimCylinder_BUFF[i];
	//	dataBlock = SectorTableNo2 / NC->blockSizeSector;
	//	dataSector = SectorTableNo2 % NC->blockSizeSector;
	//	assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
	//	assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);

	//	//更新SLB2DB: 寫到SLB裡
	//	assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
	//	assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
	//	N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
	//	N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
	//	//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
	//	N->blocks[dataBlock].sections[dataSector].valid = 0;
	//	N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
	//	//換下一個SLB Sector
	//	SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
	//}
#endif
#ifdef Time_Overhead
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}

#ifdef Simulation_SLB_DELAY
/*Delay Band*/
//void FASTmergeRWSLB_DELAY_SHUFFLE(sim *simPtr) {//一次merge整個data band
//	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
//	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
//	DWORD	i = 0, j = 0, k = 0;
//
//	DWORD validIs0 = 0, validIs1 = 0;
//	DWORD block_no = 0, section_no = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
//	DWORD NoMerge_Band = 0;
//	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
//#endif
//	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
//	DWORD Merge_Bands = 0;
//	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
//	//SLB統計PM會merge到那些Cylinder上
//	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
//	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
//	DWORD dataSector;
//	DWORD LBsector;
//
//	SLB->mergecnt++;
//	Victim = NC->PsizeBlock - 1; 
//	//tsp is "trackSizePage"
//	const DWORD tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
//	const DWORD tsp_end = (SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
//	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
//#ifdef Simulation_SLB_DELAY
//	N->VC_BUFF_Len = 0;//暫存Delay Sector
//	DWORD DELAY_BAND_merge_ram_size = 0;
//#endif
//	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
//	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
//		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
//		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
//		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
//		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
//			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
//			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
//			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
//			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
//			assert(N->DBalloc[dataBlock] == 1);
//			assert(N->blocks[dataBlock].sections != NULL);
//			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
//#ifdef Time_Overhead
//			N->Merge_Bandcount++;
//			merge_ram_size = 0;
//#endif
//			//紀錄每次有效merge帶走多少band
//			Merge_Bands++;
//			validIs0 = 0; validIs1 = 0;
//			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
//			const DWORD DELAY_BAND_THRESHOLD = 2048;//Delay Band的條件:  1024 2048 4096
//			DWORD L_Block_start = 0;//加速
//			while (N->blocks[dataBlock].sections[L_Block_start].valid == 1){ L_Block_start++; }
//			assert(L_Block_start < NC->blockSizeSector); 
//			assert(N->blocks[dataBlock].sections[L_Block_start].valid == 0);
//#ifdef Simulation_SLB_DELAY
//			DELAY_BAND_merge_ram_size = 0;
//			block_no = dataBlock;
//			for (j = L_Block_start; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
//				assert(N->blocks[dataBlock].sections[j].valid == 0 || N->blocks[dataBlock].sections[j].valid == 1);
//				if (N->blocks[dataBlock].sections[j].valid == 0){ DELAY_BAND_merge_ram_size++; }//在LB裡的Dirty Sector
//			}
//			//如果達成 DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD && SLB Delay空間裝得下
//#endif
//			//SLB統計PM會merge到那些Cylinder上
//			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
//			N->PMstatistic_DB[dataBlock]++;
//			for (j = L_Block_start; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
//				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//				block_no = dataBlock; section_no = j;
//				assert(N->blocks[dataBlock].sections[j].valid == 0 || N->blocks[dataBlock].sections[j].valid == 1);
//				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
//					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
//				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
//					LBsector = N->blocks[dataBlock].sections[j].sector;
//					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
//					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
//					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
//					//SLB統計PM會merge到那些Cylinder上
//					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
//					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
//					//更新統計資料
//					validIs0 += NC->Section;
//#ifdef Simulation_SLB_DELAY
//					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
//					if (DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD){
//						if (tsp_start <= LBsector && LBsector <= tsp_end){
//							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
//							//暫存等Merge結束寫到lastRW
//							assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
//							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
//							//Delay Band的SHUFFLE_TL不歸0,累積更新
//							//還LB
//							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
//							//假寫回原Band, 實際上會在Merge結束寫回lastRW
//							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
//							N->blocks[dataBlock].sections[j].valid = 1;
//						}else{}//Delay Band Sector留原地
//					}else{//Delay失敗, 做Shuffle Merge
//#ifdef Time_Overhead
//						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
//#endif	
//						N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
//						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
//						N->blocks[dataBlock].sections[j].valid = 1;
//					}
//					//N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
//					//N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
//					//N->blocks[dataBlock].sections[j].valid = 1;
//#endif
//				}else{ printf("SLB merge error\n"); system("pause"); }
//			}
//#ifdef Simulation_SLB_SHUFFLE
//			//找出DB上最左第一個CR, 得出需要merge的cylinder數
//			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
//			DWORD SHUFFLE_TL_SWAP_SIG;
//			DWORD tmp_WriteFreq, tmp_DirtyUnit;
//			DWORD Lj, Pj, Lk, Pk;
//			DWORD tmp_MaxIndex;
//			//Delay失敗進入Shuffle
//			if (DELAY_BAND_merge_ram_size > DELAY_BAND_THRESHOLD){
//				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
//				j = 0;//找出DB上第一條有髒資料的P address Cylinder
//				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
//				assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
//				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
//				start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
//				start_Track = j*NC->CylinderSizeTrack;
//				start_Sector = j*NC->CylinderSizeSector;
//				for (j = 0; j < start_Cylinder; j++){
//					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
//				}
//				/*舊的Shuffle, 使用邏輯位址做第三順位的比較*/
//				////剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
//				//for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
//				//	for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
//				//		SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
//				//		assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
//				//		if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
//				//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//				//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//				//		if (SHUFFLE_TL_SWAP_SIG == 1){
//				//			//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
//				//			Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
//				//			Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
//				//			//備份K位置上的資料
//				//			tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
//				//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
//				//			//j給k, Pk->Lj, B->A
//				//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
//				//			N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
//				//			N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
//				//			assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//				//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
//				//			//tmp給j, Pj->Lk, A->B
//				//			assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//				//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
//				//			N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
//				//			N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
//				//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
//				//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
//				//		}
//				//	}
//				//	if (j == 0){ break; }//j,k是DWORD, 沒有負數
//				//}//檢查每一個L都有唯一對應到P,反之亦是
//				/*新的Shuffle, 使用LRU做第三順位的比較*/
//				//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
//				for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
//					tmp_MaxIndex = j;
//					for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
//						SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
//						assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
//						assert(0 <= k); assert(k < NC->blockSizeCylinder);
//						if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
//						else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//						//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//						//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去
//						if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
//					}
//					if (tmp_MaxIndex != j){
//						//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
//						assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
//						for (k = tmp_MaxIndex; k < j; k++){
//							//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
//							Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
//							Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
//							//備份K位置上的資料
//							tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
//							assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
//							//j給k, Pk->Lj, B->A
//							N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
//							N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
//							N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
//							assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//							N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
//							//tmp給j, Pj->Lk, A->B
//							assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//							N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
//							N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
//							N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
//							assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
//							N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
//						}
//					}
//					if (j == 0){ break; }//j,k是DWORD, 沒有負數
//				}//檢查每一個L都有唯一對應到P,反之亦是
//				const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
//				DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
//				for (j = 0; j < NC->blockSizeCylinder; j++){
//					L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
//					P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
//				}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
//				//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
//				for (j = 0; j < NC->blockSizeCylinder; j++){//0 start_Cylinder
//					assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
//					/*if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
//						printf("%lu: %lu %lu: %lu\n", N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
//						printf("%lu\n", dataBlock); system("pause");
//					}*/
//					N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
//					N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
//				}
//			}
//#endif	
//#ifdef Simulation_SLB_DELAY
//			assert(DELAY_BAND_merge_ram_size == validIs0);
//			if (DELAY_BAND_merge_ram_size > DELAY_BAND_THRESHOLD){
//				assert(merge_ram_size > 0);
//				//更新統計資料
//				assert(start_Sector < NC->blockSizeSector);//等於是因為Delay
//				SLB->pageRead += (NC->blockSizeSector - start_Sector); 
//				SLB->pageWrite += (NC->blockSizeSector - start_Sector);
//				validIs1 += (NC->blockSizeSector - start_Sector - validIs0); //assert(NC->blockSizeSector - start_Sector >= validIs0);
//				SLB->merge_count += (NC->blockSizeSector - start_Sector);
//				//更新Delay Info: 歸還Delay Space
//				assert(N->DELAY_SECTORinBAND[dataBlock] >= 0);
//				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
//					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
//					N->DELAY_SECTORinBAND[dataBlock] = 0;
//					N->SLB_DELAY_BAND--;//目前多少Band正在Delay
//				}
//				N->MERGE_cnt++;//一般Merge
//			}else{
//				assert(merge_ram_size == 0);
//				//Delay Band沒有Merge寫入
//				start_Cylinder = NC->blockSizeCylinder; //找出第一條髒cylinder上的第一條髒track和髒sector
//				start_Track = NC->blockSizeTrack;
//				start_Sector = NC->blockSizeSector;
//				//更新Delay Info
//				const DWORD LastTimeSector = N->DELAY_SECTORinBAND[dataBlock]; 
//				assert(0 <= LastTimeSector); assert(LastTimeSector <= DELAY_BAND_THRESHOLD);
//				assert(DELAY_BAND_merge_ram_size >= LastTimeSector);
//				//更新Band上Delay的Sector數
//				N->DELAY_SECTORinBAND[dataBlock] = DELAY_BAND_merge_ram_size;
//				//更新Delay Info: SLB裡Delay的Sector增加
//				N->SLB_DELAY_USE += (DELAY_BAND_merge_ram_size - LastTimeSector);
//				N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
//				//目前多少Band正在Delay
//				if (LastTimeSector == 0){ N->SLB_DELAY_BAND++; }
//				N->CumuSLB_DELAY_BAND += N->SLB_DELAY_BAND;
//				N->DELAY_cnt++;//DELAY
//			}
//#endif
//			//歸還DB的sectors記憶體
//			//assert(N->DBalloc[dataBlock] == 1);
//			//free(N->blocks[dataBlock].sections);
//			//N->DBalloc[dataBlock] = 0;
//			//N->blocks[block_no].sections = NULL;//dataBlock
//
//			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
//			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
//			Dirty_Sectors += (I64)merge_ram_size;
//			N->req_w_cumu += Cumu_Dirty_Sectors;
//#ifdef MergeAssoc_SLBpart
//			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
//			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
//			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
//			N->l2pmapOvd.AA -= merge_ram_size;
//			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
//			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
//			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
//			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
//			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
//#endif
//#ifdef Time_Overhead
//			assert(0 <= merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
//			if (merge_ram_size > 0){//只有Delay失敗做Shuffle Merge
//				assert(DELAY_BAND_merge_ram_size > DELAY_BAND_THRESHOLD);
//				for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
//					SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//					test(simPtr, SectorTableNo3, 3);
//					Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
//					N->content_rotate += NC->trackSizeSector;
//					N->MR_content_rotate += NC->trackSizeSector;
//					N->Req_RunTime += 10;
//				}
//				//讀SLB
//				qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//				if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
//				//acctime
//				test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
//				Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
//				N->S_CT++;
//				//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
//				N->content_rotate += merge_ram_size;
//				N->MR_content_rotate += merge_ram_size;
//				N->Merge_LB_Trackcount++;
//				for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//					test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
//					if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
//						Calc_RotateOverhead2(simPtr, 0, 4);
//						N->S_CT++;
//					}//3
//					N->Merge_LB_Trackcount++;//與S_MRS重複
//				}
//				N->merge_ram_writecount++; //
//				N->S_MRS += (I64)merge_ram_size;
//				//transfer time
//				N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
//				//確認seek次數正確
//				Merge_W_SeekCount = N->Merge_W_SeekCount;
//				//combine DB和SLB資料
//				for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
//					SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//					test(simPtr, (I64)SectorTableNo3, 2);
//					Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
//					N->content_rotate += NC->trackSizeSector;
//					N->MW_content_rotate += NC->trackSizeSector;
//					N->Req_RunTime += 10;
//					//
//					N->Merge_Trackcount++;
//				}
//				assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
//			}
//			merge_ram_size = 0;
//#endif
//			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
//			N->blocks[Victim].sections[i].valid = 0;
//			//SLB統計PM會merge到那些Cylinder上
//			j = 0; while (DB_Cylinder[j] == 0){ j++; }
//			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
//			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
//			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
//			//找到s_DirtyCyl, e_DirtyCyl
//			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
//			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
//			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
//			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
//			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
//				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
//					assert(DB_Cylinder[j + 1] >= 0);
//					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
//					else{
//						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
//						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
//						CleanRegionTmp = 0;//找到,統計完歸0 
//					}
//				}
//				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
//					N->LB_DirtySectors += DB_Cylinder[j];
//					N->DirtyInRange_Cylinders++;
//					if (j + 1 < NC->blockSizeCylinder){
//						assert(DB_Cylinder[j + 1] >= 0);
//						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
//							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
//							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
//							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
//							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
//						}
//					}
//				}
//			}
//			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
//				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
//				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
//			}
//			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
//			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
//		}else {
//			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
//		}
//		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
//		//assert(N->blocks[Victim].sections[i].valid == 0);
//		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
//	}
//
//	if (Merge_Bands > 0){
//		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
//		N->Effect_Merge++;
//	}
//	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
//	//update SMR LB information
//	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);
//	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);
//	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeSector; i++){
//		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
//		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
//	}
//	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
//#ifdef Simulation_SLB_DELAY
//	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
//	assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
//	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
//	if (N->VC_BUFF_Len > 0){
//		for (i = 0; i < N->VC_BUFF_Len; i++){
//			SectorTableNo2 = N->VictimCylinder_BUFF[i];
//			dataBlock = SectorTableNo2 / NC->blockSizeSector;
//			dataSector = SectorTableNo2 % NC->blockSizeSector;
//			assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
//			assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
//			//更新SLB2DB: 寫到SLB裡
//			assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
//			assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
//			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
//			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
//			//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
//			N->blocks[dataBlock].sections[dataSector].valid = 0;
//			N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
//			//換下一個SLB Sector
//			SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
//		}
//	}
//#endif
//#ifdef Time_Overhead
//	if (N->VC_BUFF_Len > 0){
//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
//		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }//把DBtrack舊資料整條讀到merge_ram裡
//		//transfer time
//		N->Req_RunTime += (unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
//		assert(N->Req_RunTime >= 0);
//	}
//	free(merge_ram);
//#endif
//	//SLB統計PM會merge到那些Cylinder上
//	free(DB_Cylinder);
//}
void FASTmergeRWSLB_DELAY_SHUFFLE(sim *simPtr) {//一次merge整個data band
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	DWORD dataSector;
	DWORD LBsector;

	FILE *fp_DELAY_LEFT = fopen("DELAY_LEFT.txt", "a");

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1; 
	//tsp is "trackSizePage"
	const DWORD tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	const DWORD tsp_end = (SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
#ifdef Simulation_SLB_DELAY
	N->VC_BUFF_Len = 0;//暫存Delay Sector
	DWORD DELAY_BAND_merge_ram_size = 0;
#endif
	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
			const DWORD DELAY_BAND_THRESHOLD = 2048;//Delay Band的條件:  1024 2048 4096
			DWORD L_Block_start = 0;//加速
			while (N->blocks[dataBlock].sections[L_Block_start].valid == 1){ L_Block_start++; }
			assert(L_Block_start < NC->blockSizeSector); 
			assert(N->blocks[dataBlock].sections[L_Block_start].valid == 0);
#ifdef Simulation_SLB_DELAY
			DWORD DELAY_BAND_SIGNAL = 0;
			DELAY_BAND_merge_ram_size = 0;
			block_no = dataBlock;//可以改成統計DirtyUnit, 理論上應該一樣
			for (j = L_Block_start; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				assert(N->blocks[dataBlock].sections[j].valid == 0 || N->blocks[dataBlock].sections[j].valid == 1);
				if (N->blocks[dataBlock].sections[j].valid == 0){ DELAY_BAND_merge_ram_size++; }//在LB裡的Dirty Sector
			}
			//如果達成 DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD && SLB Delay空間裝得下
			if (DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD){
				if (N->DELAY_SECTORinBAND[dataBlock] > 0){ assert(DELAY_BAND_merge_ram_size >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DELAY_BAND_merge_ram_size - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE){ DELAY_BAND_SIGNAL = 1; }//DelayBand條件達成
				}else{ assert(N->DELAY_SECTORinBAND[dataBlock] == 0); 
					if (N->SLB_DELAY_USE + DELAY_BAND_merge_ram_size <= N->SLB_DELAY_SPACE){ DELAY_BAND_SIGNAL = 1; }//DelayBand條件達成
				}
			}
			if (DELAY_BAND_SIGNAL == 1){ fprintf(fp_DELAY_LEFT, "%lu, ", DELAY_BAND_merge_ram_size); }
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = L_Block_start; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				assert(N->blocks[dataBlock].sections[j].valid == 0 || N->blocks[dataBlock].sections[j].valid == 1);
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Simulation_SLB_DELAY
					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
					if (DELAY_BAND_SIGNAL == 1){//DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD
						if (tsp_start <= LBsector && LBsector <= tsp_end){
							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
							//暫存等Merge結束寫到lastRW
							assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
							//Delay Band的SHUFFLE_TL不歸0,累積更新
							//還LB
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
							//假寫回原Band, 實際上會在Merge結束寫回lastRW
							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
							N->blocks[dataBlock].sections[j].valid = 1;
						}else{}//Delay Band Sector留原地
					}else{//Delay失敗, 做Shuffle Merge
#ifdef Time_Overhead
						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif	
						N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
						N->blocks[dataBlock].sections[j].valid = 1;
					}
					//N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					//N->blocks[dataBlock].sections[j].valid = 1;
#endif
				}else{ printf("SLB merge error\n"); system("pause"); }
			}
#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;
			//Delay失敗進入Shuffle
			if (DELAY_BAND_SIGNAL == 0){// DELAY_BAND_merge_ram_size > DELAY_BAND_THRESHOLD
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0;//找出DB上第一條有髒資料的P address Cylinder
				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
				assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
				start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = j*NC->CylinderSizeTrack;
				start_Sector = j*NC->CylinderSizeSector;
				for (j = 0; j < start_Cylinder; j++){
					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
				}
				/*舊的Shuffle, 使用邏輯位址做第三順位的比較*/
				////剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
				//for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
				//	for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
				//		SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
				//		assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
				//		if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
				//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
				//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
				//		if (SHUFFLE_TL_SWAP_SIG == 1){
				//			//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
				//			Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
				//			Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
				//			//備份K位置上的資料
				//			tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
				//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
				//			//j給k, Pk->Lj, B->A
				//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
				//			N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
				//			N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
				//			assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
				//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
				//			//tmp給j, Pj->Lk, A->B
				//			assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
				//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
				//			N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
				//			N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
				//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
				//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
				//		}
				//	}
				//	if (j == 0){ break; }//j,k是DWORD, 沒有負數
				//}//檢查每一個L都有唯一對應到P,反之亦是
				/*新的Shuffle, 使用LRU做第三順位的比較*/
				//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
				for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
					tmp_MaxIndex = j;
					for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
						SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
						assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
						assert(0 <= k); assert(k < NC->blockSizeCylinder);
						if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
						else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
						//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
						//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去
						if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
					}
					if (tmp_MaxIndex != j){
						//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
						assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
						for (k = tmp_MaxIndex; k < j; k++){
							//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
							Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
							Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
							//備份K位置上的資料
							tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
							assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
							//j給k, Pk->Lj, B->A
							N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
							N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
							N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
							assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
							N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
							//tmp給j, Pj->Lk, A->B
							assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
							N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
							N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
							N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
							assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
							N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
						}
					}
					if (j == 0){ break; }//j,k是DWORD, 沒有負數
				}//檢查每一個L都有唯一對應到P,反之亦是
				const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
				DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
				for (j = 0; j < NC->blockSizeCylinder; j++){
					L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
					P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
				}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
				//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
				for (j = 0; j < NC->blockSizeCylinder; j++){//0 start_Cylinder
					assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
					/*if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
					printf("%lu: %lu %lu: %lu\n", N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
					printf("%lu\n", dataBlock); system("pause");
					}*/
					N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
					N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
				}
			}
#endif	
#ifdef Simulation_SLB_DELAY
			assert(DELAY_BAND_merge_ram_size == validIs0);
			if (DELAY_BAND_SIGNAL == 0){// DELAY_BAND_merge_ram_size > DELAY_BAND_THRESHOLD
				assert(merge_ram_size > 0);
				//更新統計資料
				assert(start_Sector < NC->blockSizeSector);//等於是因為Delay
				SLB->pageRead += (NC->blockSizeSector - start_Sector); 
				SLB->pageWrite += (NC->blockSizeSector - start_Sector);
				validIs1 += (NC->blockSizeSector - start_Sector - validIs0); //assert(NC->blockSizeSector - start_Sector >= validIs0);
				SLB->merge_count += (NC->blockSizeSector - start_Sector);
				//更新Delay Info: 歸還Delay Space
				assert(N->DELAY_SECTORinBAND[dataBlock] >= 0);
				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
					N->DELAY_SECTORinBAND[dataBlock] = 0;
					N->SLB_DELAY_BAND--;//目前多少Band正在Delay
				}
				N->MERGE_cnt++;//一般Merge
			}else{
				assert(DELAY_BAND_SIGNAL == 1);
				assert(merge_ram_size == 0);
				//Delay Band沒有Merge寫入
				start_Cylinder = NC->blockSizeCylinder; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = NC->blockSizeTrack;
				start_Sector = NC->blockSizeSector;
				//更新Delay Info
				const DWORD LastTimeSector = N->DELAY_SECTORinBAND[dataBlock]; 
				assert(0 <= LastTimeSector); assert(LastTimeSector <= DELAY_BAND_THRESHOLD);
				assert(LastTimeSector <= DELAY_BAND_merge_ram_size);
				assert(DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD);
				//更新Band上Delay的Sector數
				N->DELAY_SECTORinBAND[dataBlock] = DELAY_BAND_merge_ram_size;
				//更新Delay Info: SLB裡Delay的Sector增加
				N->SLB_DELAY_USE += (DELAY_BAND_merge_ram_size - LastTimeSector);
				N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
				//目前多少Band正在Delay
				if (LastTimeSector == 0){ N->SLB_DELAY_BAND++; }
				N->CumuSLB_DELAY_BAND += N->SLB_DELAY_BAND;
				N->DELAY_cnt++;//DELAY
			}
#endif
			//歸還DB的sectors記憶體
			//assert(N->DBalloc[dataBlock] == 1);
			//free(N->blocks[dataBlock].sections);
			//N->DBalloc[dataBlock] = 0;
			//N->blocks[block_no].sections = NULL;//dataBlock

			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 <= merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			if (merge_ram_size > 0){//只有Delay失敗做Shuffle Merge
				//assert(DELAY_BAND_merge_ram_size > DELAY_BAND_THRESHOLD);
				assert(DELAY_BAND_SIGNAL == 0);
				for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
					SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, SectorTableNo3, 3);
					Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
					N->content_rotate += NC->trackSizeSector;
					N->MR_content_rotate += NC->trackSizeSector;
					N->Req_RunTime += 10;
				}
				//讀SLB
				qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
				if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
				//acctime
				test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
				Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
				N->S_CT++;
				//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
				N->content_rotate += merge_ram_size;
				N->MR_content_rotate += merge_ram_size;
				N->Merge_LB_Trackcount++;
				for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
					if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
						Calc_RotateOverhead2(simPtr, 0, 4);
						N->S_CT++;
					}//3
					N->Merge_LB_Trackcount++;//與S_MRS重複
				}
				N->merge_ram_writecount++; //
				N->S_MRS += (I64)merge_ram_size;
				//transfer time
				N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
				//確認seek次數正確
				Merge_W_SeekCount = N->Merge_W_SeekCount;
				//combine DB和SLB資料
				for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
					SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, (I64)SectorTableNo3, 2);
					Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
					N->content_rotate += NC->trackSizeSector;
					N->MW_content_rotate += NC->trackSizeSector;
					N->Req_RunTime += 10;
					//
					N->Merge_Trackcount++;
				}
				assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
			}
			merge_ram_size = 0;
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		//assert(N->blocks[Victim].sections[i].valid == 0);
		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeSector; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Simulation_SLB_DELAY
	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
	assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
	if (N->VC_BUFF_Len > 0){
		for (i = 0; i < N->VC_BUFF_Len; i++){
			SectorTableNo2 = N->VictimCylinder_BUFF[i];
			dataBlock = SectorTableNo2 / NC->blockSizeSector;
			dataSector = SectorTableNo2 % NC->blockSizeSector;
			assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
			assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
			//更新SLB2DB: 寫到SLB裡
			assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
			assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
			//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
			N->blocks[dataBlock].sections[dataSector].valid = 0;
			N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
			//換下一個SLB Sector
			SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		}
	}
#endif
#ifdef Time_Overhead
	if (N->VC_BUFF_Len > 0){
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }//把DBtrack舊資料整條讀到merge_ram裡
		//transfer time
		N->Req_RunTime += (unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
		assert(N->Req_RunTime >= 0);
	}
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);

	fprintf(fp_DELAY_LEFT, " -> %lu, SLB_DELAY_USE %lu\n", N->VC_BUFF_Len, N->SLB_DELAY_USE);
	fclose(fp_DELAY_LEFT);
}
#ifdef Simulation_SLB_DELAY_ROUND
void FASTmergeRWSLB_DELAY_ROUND_SHUFFLE(sim *simPtr) {//一次merge整個data band
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	DWORD dataSector;
	DWORD LBsector;

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1; 
	//tsp is "trackSizePage"
	const DWORD tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	const DWORD tsp_end = (SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
#ifdef Simulation_SLB_DELAY
	N->VC_BUFF_Len = 0;//暫存Delay Sector
	DWORD DELAY_BAND_merge_ram_size = 0;
#endif
	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
			const DWORD DELAY_BAND_THRESHOLD = 2048;//Delay Band的條件:  1024 2048 4096
			DWORD L_Block_start = 0;//加速
			while (N->blocks[dataBlock].sections[L_Block_start].valid == 1){ L_Block_start++; }
			assert(L_Block_start < NC->blockSizeSector); 
			assert(N->blocks[dataBlock].sections[L_Block_start].valid == 0);
#ifdef Simulation_SLB_DELAY
			DWORD DELAY_BAND_SIGNAL = 0;
			DELAY_BAND_merge_ram_size = 0;
			block_no = dataBlock;
			for (j = L_Block_start; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				assert(N->blocks[dataBlock].sections[j].valid == 0 || N->blocks[dataBlock].sections[j].valid == 1);
				if (N->blocks[dataBlock].sections[j].valid == 0){ DELAY_BAND_merge_ram_size++; }//在LB裡的Dirty Sector
			}
			////如果達成 DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD && SLB Delay空間裝得下
			//if (DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD){
			//	if (N->DELAY_SECTORinBAND[dataBlock] > 0){ assert(DELAY_BAND_merge_ram_size >= N->DELAY_SECTORinBAND[dataBlock]);
			//		if (N->SLB_DELAY_USE + DELAY_BAND_merge_ram_size - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE){ DELAY_BAND_SIGNAL = 1; }//DelayBand條件達成
			//	}else{ assert(N->DELAY_SECTORinBAND[dataBlock] == 0); 
			//		if (N->SLB_DELAY_USE + DELAY_BAND_merge_ram_size <= N->SLB_DELAY_SPACE){ DELAY_BAND_SIGNAL = 1; }//DelayBand條件達成
			//	}
			//}

			//如果達成 DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD && SLB Delay空間裝得下
			if (DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD){
				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
					assert(DELAY_BAND_merge_ram_size >= N->DELAY_SECTORinBAND[dataBlock]);
					if (i / NC->CylinderSizeSector != N->DRLB_CYLINDER[dataBlock] && N->SLB_DELAY_USE + DELAY_BAND_merge_ram_size - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE){ DELAY_BAND_SIGNAL = 1; }//DelayBand條件達成
				}
				else{
					assert(N->DELAY_SECTORinBAND[dataBlock] == 0);
					assert(N->DRLB_CYLINDER[dataBlock] == 0);
					if (N->SLB_DELAY_USE + DELAY_BAND_merge_ram_size <= N->SLB_DELAY_SPACE){ 
						DELAY_BAND_SIGNAL = 1; 
						N->DRLB_CYLINDER[dataBlock] = i / NC->CylinderSizeSector;
					}//DelayBand條件達成
				}
			}

#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = L_Block_start; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				assert(N->blocks[dataBlock].sections[j].valid == 0 || N->blocks[dataBlock].sections[j].valid == 1);
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Simulation_SLB_DELAY
					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
					if (DELAY_BAND_SIGNAL == 1){//DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD
						if (tsp_start <= LBsector && LBsector <= tsp_end){
							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
							//暫存等Merge結束寫到lastRW
							assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
							//Delay Band的SHUFFLE_TL不歸0,累積更新
							//還LB
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
							//假寫回原Band, 實際上會在Merge結束寫回lastRW
							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
							N->blocks[dataBlock].sections[j].valid = 1;
						}else{}//Delay Band Sector留原地
					}else{//Delay失敗, 做Shuffle Merge
#ifdef Time_Overhead
						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif	
						N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
						N->blocks[dataBlock].sections[j].valid = 1;
					}
					//N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					//N->blocks[dataBlock].sections[j].valid = 1;
#endif
				}else{ printf("SLB merge error\n"); system("pause"); }
			}
#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;
			//Delay失敗進入Shuffle
			if (DELAY_BAND_SIGNAL == 0){// DELAY_BAND_merge_ram_size > DELAY_BAND_THRESHOLD
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0;//找出DB上第一條有髒資料的P address Cylinder
				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
				assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
				start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = j*NC->CylinderSizeTrack;
				start_Sector = j*NC->CylinderSizeSector;
				for (j = 0; j < start_Cylinder; j++){
					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
				}
				/*舊的Shuffle, 使用邏輯位址做第三順位的比較*/
				////剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
				//for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
				//	for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
				//		SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
				//		assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
				//		if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
				//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
				//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
				//		if (SHUFFLE_TL_SWAP_SIG == 1){
				//			//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
				//			Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
				//			Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
				//			//備份K位置上的資料
				//			tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
				//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
				//			//j給k, Pk->Lj, B->A
				//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
				//			N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
				//			N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
				//			assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
				//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
				//			//tmp給j, Pj->Lk, A->B
				//			assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
				//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
				//			N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
				//			N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
				//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
				//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
				//		}
				//	}
				//	if (j == 0){ break; }//j,k是DWORD, 沒有負數
				//}//檢查每一個L都有唯一對應到P,反之亦是
				/*新的Shuffle, 使用LRU做第三順位的比較*/
				//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
				for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
					tmp_MaxIndex = j;
					for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
						SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
						assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
						assert(0 <= k); assert(k < NC->blockSizeCylinder);
						if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
						else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
						//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
						//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去
						if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
					}
					if (tmp_MaxIndex != j){
						//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
						assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
						for (k = tmp_MaxIndex; k < j; k++){
							//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
							Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
							Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
							//備份K位置上的資料
							tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
							assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
							//j給k, Pk->Lj, B->A
							N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
							N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
							N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
							assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
							N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
							//tmp給j, Pj->Lk, A->B
							assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
							N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
							N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
							N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
							assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
							N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
						}
					}
					if (j == 0){ break; }//j,k是DWORD, 沒有負數
				}//檢查每一個L都有唯一對應到P,反之亦是
				const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
				DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
				for (j = 0; j < NC->blockSizeCylinder; j++){
					L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
					P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
				}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
				//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
				for (j = 0; j < NC->blockSizeCylinder; j++){//0 start_Cylinder
					assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
					/*if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
					printf("%lu: %lu %lu: %lu\n", N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
					printf("%lu\n", dataBlock); system("pause");
					}*/
					N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
					N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
				}
			}
#endif	
#ifdef Simulation_SLB_DELAY
			assert(DELAY_BAND_merge_ram_size == validIs0);
			if (DELAY_BAND_SIGNAL == 0){// DELAY_BAND_merge_ram_size > DELAY_BAND_THRESHOLD
				assert(merge_ram_size > 0);
				//更新統計資料
				assert(start_Sector < NC->blockSizeSector);//等於是因為Delay
				SLB->pageRead += (NC->blockSizeSector - start_Sector); 
				SLB->pageWrite += (NC->blockSizeSector - start_Sector);
				validIs1 += (NC->blockSizeSector - start_Sector - validIs0); //assert(NC->blockSizeSector - start_Sector >= validIs0);
				SLB->merge_count += (NC->blockSizeSector - start_Sector);
				//更新Delay Info: 歸還Delay Space
				assert(N->DELAY_SECTORinBAND[dataBlock] >= 0);
				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
					N->DELAY_SECTORinBAND[dataBlock] = 0;
					N->SLB_DELAY_BAND--;//目前多少Band正在Delay
					N->DRLB_CYLINDER[dataBlock] = 0;
				}
				N->MERGE_cnt++;//一般Merge
			}else{
				assert(DELAY_BAND_SIGNAL == 1);
				assert(merge_ram_size == 0);
				//Delay Band沒有Merge寫入
				start_Cylinder = NC->blockSizeCylinder; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = NC->blockSizeTrack;
				start_Sector = NC->blockSizeSector;
				//更新Delay Info
				const DWORD LastTimeSector = N->DELAY_SECTORinBAND[dataBlock]; 
				assert(0 <= LastTimeSector); assert(LastTimeSector <= DELAY_BAND_THRESHOLD);
				assert(LastTimeSector <= DELAY_BAND_merge_ram_size);
				assert(DELAY_BAND_merge_ram_size <= DELAY_BAND_THRESHOLD);
				//更新Band上Delay的Sector數
				N->DELAY_SECTORinBAND[dataBlock] = DELAY_BAND_merge_ram_size;
				//更新Delay Info: SLB裡Delay的Sector增加
				N->SLB_DELAY_USE += (DELAY_BAND_merge_ram_size - LastTimeSector);
				N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
				//目前多少Band正在Delay
				if (LastTimeSector == 0){ N->SLB_DELAY_BAND++; }
				N->CumuSLB_DELAY_BAND += N->SLB_DELAY_BAND;
				N->DELAY_cnt++;//DELAY
			}
#endif
			//歸還DB的sectors記憶體
			//assert(N->DBalloc[dataBlock] == 1);
			//free(N->blocks[dataBlock].sections);
			//N->DBalloc[dataBlock] = 0;
			//N->blocks[block_no].sections = NULL;//dataBlock

			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 <= merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			if (merge_ram_size > 0){//只有Delay失敗做Shuffle Merge
				//assert(DELAY_BAND_merge_ram_size > DELAY_BAND_THRESHOLD);
				assert(DELAY_BAND_SIGNAL == 0);
				for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
					SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, SectorTableNo3, 3);
					Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
					N->content_rotate += NC->trackSizeSector;
					N->MR_content_rotate += NC->trackSizeSector;
					N->Req_RunTime += 10;
				}
				//讀SLB
				qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
				if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
				//acctime
				test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
				Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
				N->S_CT++;
				//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
				N->content_rotate += merge_ram_size;
				N->MR_content_rotate += merge_ram_size;
				N->Merge_LB_Trackcount++;
				for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
					if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
						Calc_RotateOverhead2(simPtr, 0, 4);
						N->S_CT++;
					}//3
					N->Merge_LB_Trackcount++;//與S_MRS重複
				}
				N->merge_ram_writecount++; //
				N->S_MRS += (I64)merge_ram_size;
				//transfer time
				N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
				//確認seek次數正確
				Merge_W_SeekCount = N->Merge_W_SeekCount;
				//combine DB和SLB資料
				for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
					SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					test(simPtr, (I64)SectorTableNo3, 2);
					Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
					N->content_rotate += NC->trackSizeSector;
					N->MW_content_rotate += NC->trackSizeSector;
					N->Req_RunTime += 10;
					//
					N->Merge_Trackcount++;
				}
				assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
			}
			merge_ram_size = 0;
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		//assert(N->blocks[Victim].sections[i].valid == 0);
		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeSector; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Simulation_SLB_DELAY
	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
	assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
	if (N->VC_BUFF_Len > 0){
		for (i = 0; i < N->VC_BUFF_Len; i++){
			SectorTableNo2 = N->VictimCylinder_BUFF[i];
			dataBlock = SectorTableNo2 / NC->blockSizeSector;
			dataSector = SectorTableNo2 % NC->blockSizeSector;
			assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
			assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
			//更新SLB2DB: 寫到SLB裡
			assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
			assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
			//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
			N->blocks[dataBlock].sections[dataSector].valid = 0;
			N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
			//換下一個SLB Sector
			SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		}
	}
#endif
#ifdef Time_Overhead
	if (N->VC_BUFF_Len > 0){
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }//把DBtrack舊資料整條讀到merge_ram裡
		//transfer time
		N->Req_RunTime += (unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
		assert(N->Req_RunTime >= 0);
	}
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
/*Delay MAxCR Left*/ //error
//void FASTmergeRWSLB_DELAY_LEFT_SHUFFLE(sim *simPtr) {//一次merge整個data band
//	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
//	DWORD	i = 0, j = 0, k = 0;
//
//	DWORD validIs0 = 0, validIs1 = 0;
//	DWORD block_no = 0, section_no = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
//	DWORD NoMerge_Band = 0;
//	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
//#endif
//	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
//	DWORD Merge_Bands = 0;
//	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
//	//SLB統計PM會merge到那些Cylinder上
//	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
//	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
//	DWORD dataSector;
//	DWORD LBsector;
//	//Delay clean region統計
//	DWORD DELAY_CleanRegionMAx = 0;//MAX
//	DWORD DELAY_CleanRegion_StartCyl = 0, DELAY_CleanRegion_EndCyl = 0;
//	DWORD DELAY_CleanRegionTmp = 0;//tmp
//	DWORD DELAY_CleanRegion_StartCylTmp = 0;
//
//	SLB->mergecnt++;
//	Victim = NC->PsizeBlock - 1;
//	//tsp is "trackSizePage"
//	const DWORD tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
//	const DWORD tsp_end = (SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
//	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);//4096
//#ifdef Simulation_SLB_DELAY
//	N->VC_BUFF_Len = 0;//暫存Delay Sector
//#endif
//	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
//	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
//		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
//		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
//		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
//		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
//			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
//			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
//			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
//			assert(N->DBalloc[dataBlock] == 1);
//			assert(N->blocks[dataBlock].sections != NULL);
//			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
//			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
//#ifdef Time_Overhead
//			N->Merge_Bandcount++;
//			merge_ram_size = 0;
//#endif
//			//紀錄每次有效merge帶走多少band
//			Merge_Bands++;
//			validIs0 = 0; validIs1 = 0;
//			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
//			DWORD DELAY_SIG = 0;
//#ifdef Simulation_SLB_DELAY
//			const DWORD DirtyUnit_TD = 64, SaveCylinder_TD = 8;//
//			//限定Band Merge含第二次以上才可以Delay
//			if (N->PMstatistic_DB[dataBlock] >= 1){
//				//找出DB上第一條有髒資料的P address Cylinder
//				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
//				j = 0; while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
//				assert(j < NC->blockSizeCylinder); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
//				start_Cylinder = j;
//				//找出Max CR //MAX
//				DELAY_CleanRegionMAx = start_Cylinder + 1; //紀錄初始最左Max CR的大小
//				DELAY_CleanRegion_StartCyl = 0; DELAY_CleanRegion_EndCyl = start_Cylinder;//紀錄初始最左Max CR的起點, 終點
//				//tmp
//				DELAY_CleanRegionTmp = 0;//從Band剩下的CR裡面找出Max CR
//				DELAY_CleanRegion_StartCylTmp = 0; //用不到 DELAY_CleanRegion_EndCylTmp = 0;
//				//開始找更大的CR
//				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
//					if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0 && j + 1 < NC->blockSizeCylinder){//最右邊01有處理,但是00沒處理
//						//保證Dirty_j是0
//						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] >= 0);
//						if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] == 0){ //累積連續CR空間
//							DELAY_CleanRegionTmp++;
//							if (DELAY_CleanRegionTmp == 1){ DELAY_CleanRegion_StartCylTmp = j; }
//						}else{//Dirty_j+1是>0
//							if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
//								DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
//								DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
//								DELAY_CleanRegion_EndCyl = j;
//							}
//							DELAY_CleanRegionTmp = 0;//找到,統計完歸0 
//						}
//					}
//				}//處理尾段是00
//				if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
//					DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
//					DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
//					DELAY_CleanRegion_EndCyl = NC->blockSizeCylinder - 1;
//				}
//				assert(DELAY_CleanRegion_StartCyl <= DELAY_CleanRegion_EndCyl);
//				assert(DELAY_CleanRegion_EndCyl < NC->blockSizeCylinder);
//				//確保最大CR不是在最左邊, 才有Delay價值
//				DWORD DelaySector = 0;
//				DWORD L_Index, LSN;//邏輯位址, LSN: L_SectorNo
//				if (DELAY_CleanRegion_StartCyl != 0){
//					assert(start_Cylinder < DELAY_CleanRegion_StartCyl);
//					//Delay統計預先跑一次, 真的要做再跑流程
//					//換到邏輯位址計算Delay和一般sector的量, 確認Max CR上沒有髒
//					for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
//						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L, Logical Cylinder No
//						for (k = 0; k < NC->CylinderSizeSector; k++){
//							LSN = L_Index*NC->CylinderSizeSector + k;
//							if (start_Cylinder <= j && j < DELAY_CleanRegion_StartCyl){
//								assert(N->blocks[dataBlock].sections[LSN].valid == 0 || N->blocks[dataBlock].sections[LSN].valid == 1);
//								if (N->blocks[dataBlock].sections[LSN].valid == 0){ DelaySector++; }//可能會Delay的段落
//							}else if (DELAY_CleanRegion_StartCyl <= j && j < DELAY_CleanRegion_EndCyl){
//								assert(N->blocks[dataBlock].sections[LSN].valid == 1);//確定Max CR裡真的沒東西
//							}else{} //要Shuffle的段落
//						}
//					}
//					//以下三條件都符合就確定Delay
//					if (DelaySector <= DirtyUnit_TD){ DELAY_SIG++; }//Max CR以左的Dirty Sector <= Dirty門檻
//					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
//					if (N->SLB_DELAY_USE + DelaySector - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE){ DELAY_SIG++; }//SLB Delay Space還有空間
//					if (DELAY_CleanRegion_EndCyl + 1 - start_Cylinder >= SaveCylinder_TD){ DELAY_SIG++; }//Delay省的Merge寫入 >= Save Region門檻
//				}
//				if (DELAY_SIG == 3){
//					//for (j = start_Cylinder; j < DELAY_CleanRegion_StartCyl; j++){//Delay的段落
//					//	L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L
//					//	for (k = 0; k < NC->CylinderSizeSector; k++){
//					//		LSN = L_Index*NC->CylinderSizeSector + k;
//					//		I64 Band_SectorValid = N->blocks[dataBlock].sections[LSN].valid;
//					//		SectorTableNo2 = dataBlock*NC->blockSizeSector + LSN;
//					//		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
//					//		if (Band_SectorValid == 0){
//					//			LBsector = N->blocks[dataBlock].sections[LSN].sector;
//					//			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
//					//			//確認DB與SLB對應雙向都正確
//					//			assert(N->blocks[Victim].sections[LBsector].valid == 1);
//					//			assert(N->blocks[Victim].sections[LBsector].sector = SectorTableNo2);
//					//		}
//					//	}
//					//}assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
//
//					//N->SLB_DELAY_USE + DELAY_BAND_merge_ram_size - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE
//					//shuffle非Delay的剩下Cylinder, 做Merge //改動start_Cylinder, 要等該標的delay標完
//					start_Cylinder = DELAY_CleanRegion_EndCyl + 1; assert(start_Cylinder <= NC->blockSizeCylinder);
//					start_Track = start_Cylinder*NC->CylinderSizeTrack;
//					start_Sector = start_Cylinder*NC->CylinderSizeSector;
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
//					if (start_Cylinder < NC->blockSizeCylinder){
//						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
//						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Cylinder] > 0);
//					}
//					//
//					if (N->DELAY_SECTORinBAND[dataBlock] > 0){ 
//						//assert(DelaySector >= N->DELAY_SECTORinBAND[dataBlock]); 
//						//if (DelaySector < N->DELAY_SECTORinBAND[dataBlock]){ printf("%lu: %lu %lu\n", dataBlock, DelaySector, N->DELAY_SECTORinBAND[dataBlock]); system("pause"); }
//					}
//					assert(DelaySector > 0);
//					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
//					N->SLB_DELAY_USE += DelaySector;
//					N->DELAY_SECTORinBAND[dataBlock] = DelaySector;
//				}
//				else{
//					assert(DELAY_SIG < 3); //如果曾經Delay, 歸還SLB Delay Space
//					//歸還: 加總後扣給SLB_DELAY_USE
//					if (N->DELAY_SECTORinBAND[dataBlock] > 0){ 
//						N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock]; 
//						N->DELAY_SECTORinBAND[dataBlock] = 0;
//					}
//				}
//			}
//#endif
//			//if (DELAY_SIG == 3){
//			//	//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
//			//	char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
//			//	sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
//			//	FILE *fp_FootPrint = fopen(FileName, "a");
//			//	fprintf(fp_FootPrint, "%lu\n", N->PMstatistic_DB[dataBlock]);
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
//			//	fclose(fp_FootPrint);
//			//}
//
//			//SLB統計PM會merge到那些Cylinder上
//			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
//			N->PMstatistic_DB[dataBlock]++;
//			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
//				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//				block_no = dataBlock; section_no = j;
//				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
//					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
//				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
//					LBsector = N->blocks[dataBlock].sections[j].sector;
//					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
//					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
//					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
//					//SLB統計PM會merge到那些Cylinder上
//					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
//					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
//					//更新統計資料
//					validIs0 += NC->Section;
//#ifdef Time_Overhead
//					if (start_Cylinder < NC->blockSizeCylinder){
//						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
//					}
//#endif
//#ifdef Simulation_SLB_DELAY
//					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
//					if (DELAY_SIG == 3 && P_Index < DELAY_CleanRegion_StartCyl){//SHuffle過P_Index < DELAY_CleanRegion_StartCyl不保證是Delay Sector
//						if (tsp_start <= LBsector && LBsector <= tsp_end){//Victim Cylinder上免費還
//							//歸還SLB
//							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
//							//假寫回Band, 等Merge結束繞寫回lastRW
//							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
//							N->blocks[dataBlock].sections[j].valid = 1;
//							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
//							//暫存等Merge結束寫到lastRW
//							assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
//							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
//							//Delay Band的SHUFFLE_TL不歸0,累積更新
//						}else{ }//不在Victim Cylinder上, 暫不處理
//						continue;
//					}
//#endif
//					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
//					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
//					N->blocks[dataBlock].sections[j].valid = 1;
//				}
//				else{ printf("SLB merge error\n"); system("pause"); }
//			}
//
//#ifdef Simulation_SLB_SHUFFLE
//			//找出DB上最左第一個CR, 得出需要merge的cylinder數
//			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
//			DWORD SHUFFLE_TL_SWAP_SIG;
//			DWORD tmp_WriteFreq, tmp_DirtyUnit;
//			DWORD Lj, Pj, Lk, Pk;
//			DWORD tmp_MaxIndex;
//
//			if (DELAY_SIG != 3){
//				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
//				j = 0;//找出DB上第一條有髒資料的P address Cylinder
//				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
//				assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
//				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
//				start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
//				start_Track = j*NC->CylinderSizeTrack;
//				start_Sector = j*NC->CylinderSizeSector;
//				for (j = 0; j < start_Cylinder; j++){
//					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
//				}
//			}
//
//			////剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
//			//for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
//			//	for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
//			//		SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
//			//		assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
//			//		if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
//			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//
//			//		if (SHUFFLE_TL_SWAP_SIG == 1){
//			//			//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
//			//			Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
//			//			Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
//			//			//備份K位置上的資料
//			//			tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
//			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
//			//			//j給k, Pk->Lj, B->A
//			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
//			//			N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
//			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
//			//			assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
//			//			//tmp給j, Pj->Lk, A->B
//			//			assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
//			//			N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
//			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
//			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
//			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
//			//		}
//			//	}
//			//	if (j == 0){ break; }//j,k是DWORD, 沒有負數
//			//}//檢查每一個L都有唯一對應到P,反之亦是
//
//			if (start_Cylinder < NC->blockSizeCylinder){
//				//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
//				for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
//					tmp_MaxIndex = j;
//					for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
//						SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
//						assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
//						assert(0 <= k); assert(k < NC->blockSizeCylinder);
//						if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
//						else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//						//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//						//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去
//
//						if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
//					}
//					if (tmp_MaxIndex != j){
//						//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
//						assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
//						for (k = tmp_MaxIndex; k < j; k++){
//							//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
//							Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
//							Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
//							//備份K位置上的資料
//							tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
//							assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
//							//j給k, Pk->Lj, B->A
//							N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
//							N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
//							N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
//							assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//							N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
//							//tmp給j, Pj->Lk, A->B
//							assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//							N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
//							N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
//							N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
//							assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
//							N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
//						}
//					}
//					if (j == 0){ break; }//j,k是DWORD, 沒有負數
//				}//檢查每一個L都有唯一對應到P,反之亦是
//				const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
//				DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
//				for (j = 0; j < NC->blockSizeCylinder; j++){
//					L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
//					P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
//				}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
//				//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
//				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){//0 start_Cylinder
//					//assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
//					if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
//						printf("%lu: %lu %lu: %lu\n", N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
//						system("pause");
//					}
//					N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0;
//					N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
//				}
//			}
//#endif
//			if (start_Cylinder < NC->blockSizeCylinder){
//				//更新統計資料
//				assert(start_Sector <= NC->blockSizeSector);//等於是因為Delay
//				SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
//				validIs1 += (NC->blockSizeSector - start_Sector - validIs0); //assert(NC->blockSizeSector - start_Sector >= validIs0);
//				SLB->merge_count += (NC->blockSizeSector - start_Sector);
//				//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
//				assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
//				Dirty_Sectors += (I64)merge_ram_size;
//				N->req_w_cumu += Cumu_Dirty_Sectors;
//			}
//			//歸還DB的sectors記憶體
//			//assert(N->DBalloc[dataBlock] == 1);
//			//free(N->blocks[dataBlock].sections);
//			//N->DBalloc[dataBlock] = 0;
//			//N->blocks[block_no].sections = NULL;//dataBlock
//#ifdef MergeAssoc_SLBpart
//			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
//			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
//			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
//			N->l2pmapOvd.AA -= merge_ram_size;
//			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
//			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
//			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
//			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
//			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
//#endif
//#ifdef Time_Overhead
//			if (start_Cylinder < NC->blockSizeCylinder){
//				assert(merge_ram_size > 0);
//				Merge_R_SeekCount = N->Merge_R_SeekCount;
//				for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
//					SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//					test(simPtr, SectorTableNo3, 3);
//					Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
//					N->content_rotate += NC->trackSizeSector;
//					N->MR_content_rotate += NC->trackSizeSector;
//					N->Req_RunTime += 10;
//				}
//				assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
//				qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//				if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
//				//acctime
//				test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
//				Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
//				N->S_CT++;
//				//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
//				N->content_rotate += merge_ram_size;
//				N->MR_content_rotate += merge_ram_size;
//				N->Merge_LB_Trackcount++;
//				for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//					test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
//					if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
//						Calc_RotateOverhead2(simPtr, 0, 4);
//						N->S_CT++;
//					}//3
//					N->Merge_LB_Trackcount++;//與S_MRS重複
//				}
//				N->merge_ram_writecount++; //
//				N->S_MRS += (I64)merge_ram_size;
//				//transfer time
//				N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
//				//確認seek次數正確
//				Merge_W_SeekCount = N->Merge_W_SeekCount;
//				//combine DB和SLB資料
//				for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
//					SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//					test(simPtr, (I64)SectorTableNo3, 2);
//					Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
//					N->content_rotate += NC->trackSizeSector;
//					N->MW_content_rotate += NC->trackSizeSector;
//					N->Req_RunTime += 10;
//					//
//					N->Merge_Trackcount++;
//				}
//				//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
//				merge_ram_size = 0;
//			}else{ assert(merge_ram_size == 0); }
//#endif
//			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
//			N->blocks[Victim].sections[i].valid = 0;
//			//SLB統計PM會merge到那些Cylinder上
//			j = 0; while (DB_Cylinder[j] == 0){ j++; }
//			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
//			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
//			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
//			//找到s_DirtyCyl, e_DirtyCyl
//			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
//			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
//			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
//			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
//			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
//				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
//					assert(DB_Cylinder[j + 1] >= 0);
//					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
//					else{
//						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
//						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
//						CleanRegionTmp = 0;//找到,統計完歸0 
//					}
//				}
//				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
//					N->LB_DirtySectors += DB_Cylinder[j];
//					N->DirtyInRange_Cylinders++;
//					if (j + 1 < NC->blockSizeCylinder){
//						assert(DB_Cylinder[j + 1] >= 0);
//						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
//							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
//							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
//							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
//							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
//						}
//					}
//				}
//			}
//			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
//				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
//				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
//			}
//			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
//			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
//		}
//		else {
//			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
//		}
//		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
//		//assert(N->blocks[Victim].sections[i].valid == 0);
//		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
//	}
//
//	if (Merge_Bands > 0){
//		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
//		N->Effect_Merge++;
//	}
//	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
//	//update SMR LB information
//	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
//		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
//		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
//	}
//	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
//#ifdef Simulation_SLB_DELAY
//	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
//	assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
//	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
//	for (i = 0; i < N->VC_BUFF_Len; i++){
//		SectorTableNo2 = N->VictimCylinder_BUFF[i];
//		dataBlock = SectorTableNo2 / NC->blockSizeSector;
//		dataSector = SectorTableNo2 % NC->blockSizeSector;
//		assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
//		assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
//		//更新SLB2DB: 寫到SLB裡
//		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
//		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
//		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
//		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
//		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
//		N->blocks[dataBlock].sections[dataSector].valid = 0;
//		N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
//		//換下一個SLB Sector
//		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
//	}
//#endif
//#ifdef Time_Overhead
//	free(merge_ram);
//#endif
//	//SLB統計PM會merge到那些Cylinder上
//	free(DB_Cylinder);
//}

void FASTmergeRWSLB_DELAY_LEFT_SHUFFLE(sim *simPtr) {//一次merge整個data band
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	DWORD dataSector;
	DWORD LBsector;
	//Delay clean region統計
	DWORD DELAY_CleanRegionMAx = 0;//MAX
	DWORD DELAY_CleanRegion_StartCyl = 0, DELAY_CleanRegion_EndCyl = 0;
	DWORD DELAY_CleanRegionTmp = 0;//tmp
	DWORD DELAY_CleanRegion_StartCylTmp = 0;

	FILE *fp_DELAY_LEFT = fopen("DELAY_LEFT.txt", "a");

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	//tsp is "trackSizePage"
	const DWORD tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	const DWORD tsp_end = (SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);//4096
#ifdef Simulation_SLB_DELAY
	N->VC_BUFF_Len = 0;//暫存Delay Sector
#endif
	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 3){}
		else if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
			DWORD DELAY_SIG = 0;
#ifdef Simulation_SLB_DELAY
			DWORD DelaySector = 0;
			const DWORD DirtyUnit_TD = NC->CylinderSizeSector, SaveCylinder_TD = 8;//
			//限定Band Merge含第二次以上才可以Delay
			if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] == 0){//沒有做過Delay //要改2嗎  
				//找出DB上第一條有髒資料的P address Cylinder
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0; while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
				assert(j < NC->blockSizeCylinder); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				start_Cylinder = j;
				//找出Max CR //MAX
				DELAY_CleanRegionMAx = 0;// start_Cylinder + 1; //紀錄初始最左Max CR的大小
				DELAY_CleanRegion_StartCyl = 0; DELAY_CleanRegion_EndCyl = start_Cylinder;//紀錄初始最左Max CR的起點, 終點
				//tmp
				DELAY_CleanRegionTmp = 0;//從Band剩下的CR裡面找出Max CR
				DELAY_CleanRegion_StartCylTmp = 0; //用不到 DELAY_CleanRegion_EndCylTmp = 0;
				//開始找更大的CR
				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
					if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0 && j + 1 < NC->blockSizeCylinder){//最右邊01有處理,但是00沒處理
						//保證Dirty_j是0
						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] >= 0);
						if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] == 0){ //累積連續CR空間
							DELAY_CleanRegionTmp++;
							if (DELAY_CleanRegionTmp == 1){ DELAY_CleanRegion_StartCylTmp = j; }
						}else{//Dirty_j+1是>0
							if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
								DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
								DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
								DELAY_CleanRegion_EndCyl = j;
							}
							DELAY_CleanRegionTmp = 0;//找到,統計完歸0 
						}
					}
				}//處理尾段是00
				//if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
				//	DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
				//	DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
				//	DELAY_CleanRegion_EndCyl = NC->blockSizeCylinder - 1;
				//}
				assert(DELAY_CleanRegion_StartCyl <= DELAY_CleanRegion_EndCyl);
				assert(DELAY_CleanRegion_EndCyl < NC->blockSizeCylinder);
				//確保最大CR不是在最左邊, 才有Delay價值
				DelaySector = 0;
				DWORD L_Index, LSN;//邏輯位址, LSN: L_SectorNo
				if (DELAY_CleanRegion_StartCyl != 0){
					assert(start_Cylinder < DELAY_CleanRegion_StartCyl);
					//Delay統計預先跑一次, 真的要做再跑流程
					//換到邏輯位址計算Delay和一般sector的量, 確認Max CR上沒有髒
					for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L, Logical Cylinder No
						for (k = 0; k < NC->CylinderSizeSector; k++){
							LSN = L_Index*NC->CylinderSizeSector + k;
							if (start_Cylinder <= j && j < DELAY_CleanRegion_StartCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 0 || N->blocks[dataBlock].sections[LSN].valid == 1);
								if (N->blocks[dataBlock].sections[LSN].valid == 0){ DelaySector++; }//可能會Delay的段落
							}else if (DELAY_CleanRegion_StartCyl <= j && j < DELAY_CleanRegion_EndCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 1);//確定Max CR裡真的沒東西
							}else{} //要Shuffle的段落
						}
					}
					//以下三條件都符合就確定Delay
					if (DelaySector <= DirtyUnit_TD){ DELAY_SIG++; }//Max CR以左的Dirty Sector <= Dirty門檻
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DelaySector <= N->SLB_DELAY_SPACE){ DELAY_SIG++; }//SLB Delay Space還有空間// - N->DELAY_SECTORinBAND[dataBlock]
					if (DELAY_CleanRegion_EndCyl + 1 - start_Cylinder >= SaveCylinder_TD){ DELAY_SIG++; }//Delay省的Merge寫入 >= Save Region門檻
				}

			}
			if (DELAY_SIG == 3){
				//for (j = start_Cylinder; j < DELAY_CleanRegion_StartCyl; j++){//Delay的段落
				//	L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L
				//	for (k = 0; k < NC->CylinderSizeSector; k++){
				//		LSN = L_Index*NC->CylinderSizeSector + k;
				//		I64 Band_SectorValid = N->blocks[dataBlock].sections[LSN].valid;
				//		SectorTableNo2 = dataBlock*NC->blockSizeSector + LSN;
				//		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
				//		if (Band_SectorValid == 0){
				//			LBsector = N->blocks[dataBlock].sections[LSN].sector;
				//			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
				//			//確認DB與SLB對應雙向都正確
				//			assert(N->blocks[Victim].sections[LBsector].valid == 1);
				//			assert(N->blocks[Victim].sections[LBsector].sector = SectorTableNo2);
				//		}
				//	}
				//}assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);

				//N->SLB_DELAY_USE + DELAY_BAND_merge_ram_size - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE
				//shuffle非Delay的剩下Cylinder, 做Merge //改動start_Cylinder, 要等該標的delay標完
				start_Cylinder = DELAY_CleanRegion_EndCyl + 1; assert(start_Cylinder < NC->blockSizeCylinder);
				start_Track = start_Cylinder*NC->CylinderSizeTrack;
				start_Sector = start_Cylinder*NC->CylinderSizeSector;
				assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
				assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
				assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Cylinder] > 0);
				//
				assert(N->DELAY_SECTORinBAND[dataBlock] == 0);
				//assert(DelaySector >= N->DELAY_SECTORinBAND[dataBlock]); 
				//if (DelaySector < N->DELAY_SECTORinBAND[dataBlock]){ printf("%lu: %lu %lu\n", dataBlock, DelaySector, N->DELAY_SECTORinBAND[dataBlock]); system("pause"); }

				assert(DelaySector > 0);
				//N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
				N->SLB_DELAY_USE += DelaySector;
				N->DELAY_SECTORinBAND[dataBlock] = DelaySector;

				fprintf(fp_DELAY_LEFT, "D:%lu, ", DelaySector);
			}
			else{
				fprintf(fp_DELAY_LEFT, "r:%lu, ", N->DELAY_SECTORinBAND[dataBlock]);
				assert(DELAY_SIG < 3); //如果曾經Delay, 歸還SLB Delay Space
				//歸還: 加總後扣給SLB_DELAY_USE
				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
					N->DELAY_SECTORinBAND[dataBlock] = 0;
				}
			}
#endif
			//if (DELAY_SIG == 3){
			//	//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
			//	char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
			//	sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
			//	FILE *fp_FootPrint = fopen(FileName, "a");
			//	fprintf(fp_FootPrint, "%lu\n", N->PMstatistic_DB[dataBlock]);
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
			//	fclose(fp_FootPrint);
			//}

			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Simulation_SLB_DELAY
					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
					if (DELAY_SIG == 3 && P_Index < DELAY_CleanRegion_StartCyl){//SHuffle過P_Index < DELAY_CleanRegion_StartCyl不保證是Delay Sector
						assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
						if (tsp_start <= LBsector && LBsector <= tsp_end){//Victim Cylinder上免費還
							//歸還SLB
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
							//假寫回Band, 等Merge結束繞寫回lastRW
							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
							N->blocks[dataBlock].sections[j].valid = 1;
							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
							//暫存等Merge結束寫到lastRW
							assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
							//Delay Band的SHUFFLE_TL不歸0,累積更新
						}else{ //標為Delay Sector
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 3;
						}//不在Victim Cylinder上, 暫不處理
					}else{
#endif
#ifdef Time_Overhead
						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
						assert((DELAY_SIG == 3 && P_Index >= DELAY_CleanRegion_StartCyl) || DELAY_SIG < 3);
						N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
						N->blocks[dataBlock].sections[j].valid = 1;
					}
				}else{ printf("SLB merge error\n"); system("pause"); }
			}
			
#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;

			if (DELAY_SIG != 3){
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0;//找出DB上第一條有髒資料的P address Cylinder
				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
				assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
				start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = j*NC->CylinderSizeTrack;
				start_Sector = j*NC->CylinderSizeSector;
				for (j = 0; j < start_Cylinder; j++){
					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
				}
			}

			////剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			//for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
			//	for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
			//		SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
			//		assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
			//		if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }

			//		if (SHUFFLE_TL_SWAP_SIG == 1){
			//			//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
			//			Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
			//			Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
			//			//備份K位置上的資料
			//			tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
			//			//j給k, Pk->Lj, B->A
			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
			//			N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
			//			assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
			//			//tmp給j, Pj->Lk, A->B
			//			assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
			//			N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
			//		}
			//	}
			//	if (j == 0){ break; }//j,k是DWORD, 沒有負數
			//}//檢查每一個L都有唯一對應到P,反之亦是

			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
					assert(0 <= k); assert(k < NC->blockSizeCylinder);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeCylinder; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);

			//if (dataBlock == 3){
			//	//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
			//	char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
			//	sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
			//	FILE *fp_FootPrint = fopen(FileName, "a");
			//	fprintf(fp_FootPrint, "%lu, DELAY_SIG %lu, start_Cylinder %lu\n", N->PMstatistic_DB[dataBlock], DELAY_SIG, start_Cylinder);
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
			//	fclose(fp_FootPrint);
			//}

			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){//0 start_Cylinder
				//assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
					printf("%lu, %lu: %lu %lu: %lu\n", dataBlock, N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
					system("pause");
				}
				//在Delay期間留住Shuffle段落的WriteFreq, 防止Delay Cylinder在Delay結束的時候放到太前面
				if (N->DELAY_SECTORinBAND[dataBlock] == 0){ N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0; }
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//更新統計資料
			assert(start_Sector <= NC->blockSizeSector);//等於是因為Delay
			SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - merge_ram_size); //assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

			//歸還DB的sectors記憶體
			//assert(N->DBalloc[dataBlock] == 1);
			//free(N->blocks[dataBlock].sections);
			//N->DBalloc[dataBlock] = 0;
			//N->blocks[block_no].sections = NULL;//dataBlock
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(merge_ram_size > 0);
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
			}
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//
				N->Merge_Trackcount++;
			}
			//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
			merge_ram_size = 0;
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		//assert(N->blocks[Victim].sections[i].valid == 0);
		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
#ifdef Simulation_SLB_DELAY
	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
	//assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
	assert(SLB->lastRWsection == SLB->guardTrack1stSection);
	for (i = tsp_start; i <= tsp_end; i++){
		if (N->blocks[NC->PsizeBlock - 1].sections[i].valid == 3){
			//確認mapping正確
			SectorTableNo2 = N->blocks[NC->LsizeBlock].sections[i].sector;
			dataBlock = SectorTableNo2 / NC->blockSizeSector;
			dataSector = SectorTableNo2 % NC->blockSizeSector;
			assert(N->blocks[dataBlock].sections[dataSector].sector == i);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			//
			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
			N->blocks[dataBlock].sections[dataSector].valid = 1;
			N->blocks[dataBlock].sections[dataSector].sector = SectorTableNo2;
			//暫存victim上其他Delay Sector
			assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
			N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
		}
	}
	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
	for (i = 0; i < N->VC_BUFF_Len; i++){
		SectorTableNo2 = N->VictimCylinder_BUFF[i];
		dataBlock = SectorTableNo2 / NC->blockSizeSector;
		dataSector = SectorTableNo2 % NC->blockSizeSector;
		assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
		assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 3;//
		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		N->blocks[dataBlock].sections[dataSector].valid = 0;
		N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
	}
#endif
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	if (N->VC_BUFF_Len > 0){
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }//把DBtrack舊資料整條讀到merge_ram裡
		//transfer time
		N->Req_RunTime += (unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
		assert(N->Req_RunTime >= 0);
	}
	free(merge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
	
	fprintf(fp_DELAY_LEFT, " -> %lu, SLB_DELAY_USE %lu\n", N->VC_BUFF_Len, N->SLB_DELAY_USE);
	fclose(fp_DELAY_LEFT);
}
//void FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE(sim *simPtr) {//參數控制的DELAY LEFT, DELAY PERIOD可調整
//	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
//	DWORD	i = 0, j = 0, k = 0;
//
//	DWORD validIs0 = 0, validIs1 = 0;
//	DWORD block_no = 0, section_no = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
//	DWORD NoMerge_Band = 0;
//	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
//#endif
//	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
//	DWORD Merge_Bands = 0;
//	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
//	//SLB統計PM會merge到那些Cylinder上
//	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
//	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
//	DWORD dataSector;
//	DWORD LBsector;
//	//Delay clean region統計
//	DWORD DELAY_CleanRegionMAx = 0;//MAX
//	DWORD DELAY_CleanRegion_StartCyl = 0, DELAY_CleanRegion_EndCyl = 0;
//	DWORD DELAY_CleanRegionTmp = 0;//tmp
//	DWORD DELAY_CleanRegion_StartCylTmp = 0;
//
//	//FILE *fp_DELAY_LEFT = fopen("DELAY_LEFT.txt", "a");
//
//	SLB->mergecnt++;
//	Victim = NC->PsizeBlock - 1;
//	//tsp is "trackSizePage"
//	const DWORD tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
//	const DWORD tsp_end = (SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
//	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);//4096
//#ifdef Simulation_SLB_DELAY
//	N->VC_BUFF_Len = 0;//暫存Delay Sector
//#endif
//	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
//	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
//		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
//		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
//		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
//		if (N->blocks[Victim].sections[i].valid == 3){}
//		else if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
//			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
//			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
//			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
//			assert(N->DBalloc[dataBlock] == 1);
//			assert(N->blocks[dataBlock].sections != NULL);
//			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
//			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
//#ifdef Time_Overhead
//			N->Merge_Bandcount++;
//			merge_ram_size = 0;
//#endif
//			//紀錄每次有效merge帶走多少band
//			Merge_Bands++;
//			validIs0 = 0; validIs1 = 0;
//			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
//			DWORD DELAY_SIG = 0;
//#ifdef Simulation_SLB_DELAY
//			DWORD DelaySector = 0; DWORD preDelaySector = 0;//檢查舊Delay Sector
//			const DWORD DirtyUnit_TD = N->DirtyUnit_TD, SaveCylinder_TD = N->SaveCylinder_TD;//
//			//限定Band Merge含第二次以上才可以Delay
//			if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] > 0){//上次Merge有做過Delay
//				//舊Delay Sector有確實留在SLB裡
//				for (j = 0; j < NC->blockSizeSector; j++){
//					if (N->blocks[dataBlock].sections[j].valid == 0 && N->blocks[Victim].sections[N->blocks[dataBlock].sections[j].sector].valid == 3){ preDelaySector++; }
//				}assert(preDelaySector == N->DELAY_SECTORinBAND[dataBlock]);
//				//
//				assert(0 < N->SHUFFLE_BOUND[dataBlock]); assert(N->SHUFFLE_BOUND[dataBlock] < NC->blockSizeCylinder);
//				assert(N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_USE);
//				for (j = 0; j < N->SHUFFLE_BOUND[dataBlock]; j++){ 
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
//					DelaySector += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
//				}assert(DelaySector >= N->DELAY_SECTORinBAND[dataBlock]);
//				if (N->SLB_DELAY_USE + DelaySector - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE && DelaySector <= DirtyUnit_TD){
//					DELAY_SIG = 3; //成功繼續Delay
//					//
//					N->SLB_DELAY_USE += (DelaySector - N->DELAY_SECTORinBAND[dataBlock]);
//					N->DELAY_SECTORinBAND[dataBlock] = DelaySector; assert(N->SHUFFLE_BOUND[dataBlock] != 0);
//					//
//					j = N->SHUFFLE_BOUND[dataBlock]; while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; if (j == NC->blockSizeCylinder){ break; } }
//					assert(j <= NC->blockSizeCylinder); //assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
//					start_Cylinder = j;
//					start_Track = start_Cylinder*NC->CylinderSizeTrack;
//					start_Sector = start_Cylinder*NC->CylinderSizeSector;
//					//fprintf(fp_DELAY_LEFT, "D:%lu, ", DelaySector);
//				}
//			}else if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] == 0){//沒有做過Delay  
//				//找出DB上第一條有髒資料的P address Cylinder
//				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
//				j = 0; while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
//				assert(j < NC->blockSizeCylinder); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
//				start_Cylinder = j;
//				//找出Max CR //MAX
//				DELAY_CleanRegionMAx = start_Cylinder + 1; //紀錄初始最左Max CR的大小
//				DELAY_CleanRegion_StartCyl = 0; DELAY_CleanRegion_EndCyl = start_Cylinder;//紀錄初始最左Max CR的起點, 終點
//				//tmp
//				DELAY_CleanRegionTmp = 0;//從Band剩下的CR裡面找出Max CR
//				DELAY_CleanRegion_StartCylTmp = 0; //用不到 DELAY_CleanRegion_EndCylTmp = 0;
//				//開始找更大的CR
//				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
//					if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0 && j + 1 < NC->blockSizeCylinder){//最右邊01有處理,但是00沒處理
//						//保證Dirty_j是0
//						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] >= 0);
//						if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] == 0){ //累積連續CR空間
//							DELAY_CleanRegionTmp++;
//							if (DELAY_CleanRegionTmp == 1){ DELAY_CleanRegion_StartCylTmp = j; }
//						}else{//Dirty_j+1是>0
//							if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
//								DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
//								DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
//								DELAY_CleanRegion_EndCyl = j;
//							}
//							DELAY_CleanRegionTmp = 0;//找到,統計完歸0 
//						}
//					}
//				}//處理尾段是00
//				//if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
//				//	DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
//				//	DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
//				//	DELAY_CleanRegion_EndCyl = NC->blockSizeCylinder - 1;
//				//}
//				assert(DELAY_CleanRegion_StartCyl <= DELAY_CleanRegion_EndCyl);
//				assert(DELAY_CleanRegion_EndCyl < NC->blockSizeCylinder);
//				//確保最大CR不是在最左邊, 才有Delay價值
//				DelaySector = 0;
//				DWORD L_Index, LSN;//邏輯位址, LSN: L_SectorNo
//				if (DELAY_CleanRegion_StartCyl != 0){
//					assert(start_Cylinder < DELAY_CleanRegion_StartCyl);
//					//Delay統計預先跑一次, 真的要做再跑流程
//					//換到邏輯位址計算Delay和一般sector的量, 確認Max CR上沒有髒
//					for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
//						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L, Logical Cylinder No
//						for (k = 0; k < NC->CylinderSizeSector; k++){
//							LSN = L_Index*NC->CylinderSizeSector + k;
//							if (start_Cylinder <= j && j < DELAY_CleanRegion_StartCyl){
//								assert(N->blocks[dataBlock].sections[LSN].valid == 0 || N->blocks[dataBlock].sections[LSN].valid == 1);
//								if (N->blocks[dataBlock].sections[LSN].valid == 0){ DelaySector++; }//可能會Delay的段落
//							}else if (DELAY_CleanRegion_StartCyl <= j && j < DELAY_CleanRegion_EndCyl){
//								assert(N->blocks[dataBlock].sections[LSN].valid == 1);//確定Max CR裡真的沒東西
//							}else{} //要Shuffle的段落
//						}
//					}
//					//以下三條件都符合就確定Delay
//					if (DelaySector <= DirtyUnit_TD){ DELAY_SIG++; }//Max CR以左的Dirty Sector <= Dirty門檻
//					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
//					if (N->SLB_DELAY_USE + DelaySector <= N->SLB_DELAY_SPACE){ DELAY_SIG++; }//SLB Delay Space還有空間// - N->DELAY_SECTORinBAND[dataBlock]
//					if (DELAY_CleanRegion_EndCyl + 1 - start_Cylinder >= SaveCylinder_TD){ DELAY_SIG++; }//Delay省的Merge寫入 >= Save Region門檻
//				}
//				if (DELAY_SIG == 3){//新Delay做初始化
//
//					//for (j = start_Cylinder; j < DELAY_CleanRegion_StartCyl; j++){//Delay的段落
//					//	L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L
//					//	for (k = 0; k < NC->CylinderSizeSector; k++){
//					//		LSN = L_Index*NC->CylinderSizeSector + k;
//					//		I64 Band_SectorValid = N->blocks[dataBlock].sections[LSN].valid;
//					//		SectorTableNo2 = dataBlock*NC->blockSizeSector + LSN;
//					//		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
//					//		if (Band_SectorValid == 0){
//					//			LBsector = N->blocks[dataBlock].sections[LSN].sector;
//					//			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
//					//			//確認DB與SLB對應雙向都正確
//					//			assert(N->blocks[Victim].sections[LBsector].valid == 1);
//					//			assert(N->blocks[Victim].sections[LBsector].sector = SectorTableNo2);
//					//		}
//					//	}
//					//}assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
//
//					//N->SLB_DELAY_USE + DELAY_BAND_merge_ram_size - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE
//					//shuffle非Delay的剩下Cylinder, 做Merge //改動start_Cylinder, 要等該標的delay標完
//					start_Cylinder = DELAY_CleanRegion_EndCyl + 1; assert(start_Cylinder < NC->blockSizeCylinder);
//					start_Track = start_Cylinder*NC->CylinderSizeTrack;
//					start_Sector = start_Cylinder*NC->CylinderSizeSector;
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Cylinder] > 0);
//					//
//					assert(N->DELAY_SECTORinBAND[dataBlock] == 0);
//					//assert(DelaySector >= N->DELAY_SECTORinBAND[dataBlock]); 
//					//if (DelaySector < N->DELAY_SECTORinBAND[dataBlock]){ printf("%lu: %lu %lu\n", dataBlock, DelaySector, N->DELAY_SECTORinBAND[dataBlock]); system("pause"); }
//
//					assert(DelaySector > 0);
//					//N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
//					N->SLB_DELAY_USE += DelaySector;
//					N->DELAY_SECTORinBAND[dataBlock] = DelaySector;
//					N->SHUFFLE_BOUND[dataBlock] = start_Cylinder; assert(N->SHUFFLE_BOUND[dataBlock] != 0);//
//					//fprintf(fp_DELAY_LEFT, "D:%lu, ", DelaySector);
//				}
//			}
//			//Delay失敗
//			if (DELAY_SIG != 3){
//				//fprintf(fp_DELAY_LEFT, "r:%lu, ", N->DELAY_SECTORinBAND[dataBlock]);
//				assert(DELAY_SIG < 3); //如果曾經Delay, 歸還SLB Delay Space
//				//歸還: 加總後扣給SLB_DELAY_USE
//				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
//					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
//					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
//					N->DELAY_SECTORinBAND[dataBlock] = 0;
//					N->SHUFFLE_BOUND[dataBlock] = 0;
//				}
//			}
//#endif
//			//if (DELAY_SIG == 3){
//			//	//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
//			//	char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
//			//	sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
//			//	FILE *fp_FootPrint = fopen(FileName, "a");
//			//	fprintf(fp_FootPrint, "%lu\n", N->PMstatistic_DB[dataBlock]);
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
//			//	fclose(fp_FootPrint);
//			//}
//
//			//SLB統計PM會merge到那些Cylinder上
//			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
//			N->PMstatistic_DB[dataBlock]++;
//			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
//				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//				block_no = dataBlock; section_no = j;
//				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
//					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
//				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
//					LBsector = N->blocks[dataBlock].sections[j].sector;
//					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
//					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
//					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
//					//SLB統計PM會merge到那些Cylinder上
//					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
//					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
//					//更新統計資料
//					validIs0 += NC->Section;
//#ifdef Simulation_SLB_DELAY
//					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
//					//SHuffle過P_Index < N->SHUFFLE_BOUND[dataBlock] 不保證是Delay Sector
//					if (DELAY_SIG == 3 && P_Index < N->SHUFFLE_BOUND[dataBlock]){// DELAY_CleanRegion_StartCyl
//						//assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
//						if (tsp_start <= LBsector && LBsector <= tsp_end){//Victim Cylinder上免費還
//							//歸還SLB
//							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
//							//假寫回Band, 等Merge結束繞寫回lastRW
//							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
//							N->blocks[dataBlock].sections[j].valid = 1;
//							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
//							//暫存等Merge結束寫到lastRW
//							assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
//							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
//							//Delay Band的SHUFFLE_TL不歸0,累積更新
//						}else{ //標為Delay Sector
//							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 3;
//						}//不在Victim Cylinder上, 暫不處理
//					}else{
//#endif
//#ifdef Time_Overhead
//						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
//#endif
//						assert((DELAY_SIG == 3 && P_Index >= DELAY_CleanRegion_StartCyl) || DELAY_SIG < 3);
//						N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
//						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
//						N->blocks[dataBlock].sections[j].valid = 1;
//					}
//				}else{ printf("SLB merge error\n"); system("pause"); }
//			}
//
//#ifdef Simulation_SLB_SHUFFLE
//			//找出DB上最左第一個CR, 得出需要merge的cylinder數
//			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
//			DWORD SHUFFLE_TL_SWAP_SIG;
//			DWORD tmp_WriteFreq, tmp_DirtyUnit;
//			DWORD Lj, Pj, Lk, Pk;
//			DWORD tmp_MaxIndex;
//
//			if (DELAY_SIG != 3){
//				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
//				j = 0;//找出DB上第一條有髒資料的P address Cylinder
//				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
//				assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
//				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
//				start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
//				start_Track = j*NC->CylinderSizeTrack;
//				start_Sector = j*NC->CylinderSizeSector;
//				for (j = 0; j < start_Cylinder; j++){
//					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
//					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
//				}
//			}
//
//			////剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
//			//for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
//			//	for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
//			//		SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
//			//		assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
//			//		if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
//			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//
//			//		if (SHUFFLE_TL_SWAP_SIG == 1){
//			//			//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
//			//			Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
//			//			Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
//			//			//備份K位置上的資料
//			//			tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
//			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
//			//			//j給k, Pk->Lj, B->A
//			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
//			//			N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
//			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
//			//			assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
//			//			//tmp給j, Pj->Lk, A->B
//			//			assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
//			//			N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
//			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
//			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
//			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
//			//		}
//			//	}
//			//	if (j == 0){ break; }//j,k是DWORD, 沒有負數
//			//}//檢查每一個L都有唯一對應到P,反之亦是
//
//			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
//			for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
//				tmp_MaxIndex = j;
//				for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
//					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
//					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
//					assert(0 <= k); assert(k < NC->blockSizeCylinder);
//					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
//					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
//					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去
//
//					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
//				}
//				if (tmp_MaxIndex != j){
//					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
//					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
//					for (k = tmp_MaxIndex; k < j; k++){
//						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
//						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
//						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
//						//備份K位置上的資料
//						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
//						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
//						//j給k, Pk->Lj, B->A
//						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
//						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
//						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
//						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
//						//tmp給j, Pj->Lk, A->B
//						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
//						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
//						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
//						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
//						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
//						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
//					}
//				}
//				if (j == 0){ break; }//j,k是DWORD, 沒有負數
//			}//檢查每一個L都有唯一對應到P,反之亦是
//			const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
//			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
//			for (j = 0; j < NC->blockSizeCylinder; j++){
//				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
//				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
//			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);
//
//			//if (dataBlock == 3){
//			//	//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
//			//	char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
//			//	sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
//			//	FILE *fp_FootPrint = fopen(FileName, "a");
//			//	fprintf(fp_FootPrint, "%lu, DELAY_SIG %lu, start_Cylinder %lu\n", N->PMstatistic_DB[dataBlock], DELAY_SIG, start_Cylinder);
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]]); }fprintf(fp_FootPrint, "\n");
//			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
//			//	fclose(fp_FootPrint);
//			//}
//
//			//對應DB的Shuffle資訊只留L2p, P2L. 因為要重新統計所以把freq, dirty清成0
//			for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){//0 start_Cylinder
//				//assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
//				if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
//					printf("%lu, %lu: %lu %lu: %lu\n", dataBlock, N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
//					system("pause");
//				}
//				//在Delay期間留住Shuffle段落的WriteFreq, 防止Delay Cylinder在Delay結束的時候放到太前面
//				if (N->DELAY_SECTORinBAND[dataBlock] == 0){ assert(N->SHUFFLE_BOUND[dataBlock] == 0); N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0; }
//				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
//			}
//#endif
//			//更新統計資料
//			assert(start_Sector <= NC->blockSizeSector);//等於是因為Delay
//			SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
//			validIs1 += (NC->blockSizeSector - start_Sector - merge_ram_size); //assert(NC->blockSizeSector - start_Sector >= validIs0);
//			SLB->merge_count += (NC->blockSizeSector - start_Sector);
//			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
//			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
//			Dirty_Sectors += (I64)merge_ram_size;
//			N->req_w_cumu += Cumu_Dirty_Sectors;
//
//			//歸還DB的sectors記憶體
//			//assert(N->DBalloc[dataBlock] == 1);
//			//free(N->blocks[dataBlock].sections);
//			//N->DBalloc[dataBlock] = 0;
//			//N->blocks[block_no].sections = NULL;//dataBlock
//#ifdef MergeAssoc_SLBpart
//			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
//			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
//			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
//			N->l2pmapOvd.AA -= merge_ram_size;
//			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
//			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
//			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
//			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
//			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
//#endif
//#ifdef Time_Overhead
//			assert(merge_ram_size >= 0);
//			if (start_Cylinder == NC->blockSizeCylinder){ assert(merge_ram_size == 0); }
//			Merge_R_SeekCount = N->Merge_R_SeekCount;
//			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
//				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//				test(simPtr, SectorTableNo3, 3);
//				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
//				N->content_rotate += NC->trackSizeSector;
//				N->MR_content_rotate += NC->trackSizeSector;
//				N->Req_RunTime += 10;
//			}
//			assert(0 <= merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
//			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
//			//acctime
//			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
//			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
//			N->S_CT++;
//			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
//			N->content_rotate += merge_ram_size;
//			N->MR_content_rotate += merge_ram_size;
//			N->Merge_LB_Trackcount++;
//			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
//				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
//					Calc_RotateOverhead2(simPtr, 0, 4);
//					N->S_CT++;
//				}//3
//				N->Merge_LB_Trackcount++;//與S_MRS重複
//			}
//			N->merge_ram_writecount++; //
//			N->S_MRS += (I64)merge_ram_size;
//			//transfer time
//			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
//			//確認seek次數正確
//			Merge_W_SeekCount = N->Merge_W_SeekCount;
//			//combine DB和SLB資料
//			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
//				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//				test(simPtr, (I64)SectorTableNo3, 2);
//				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
//				N->content_rotate += NC->trackSizeSector;
//				N->MW_content_rotate += NC->trackSizeSector;
//				N->Req_RunTime += 10;
//				//
//				N->Merge_Trackcount++;
//			}
//			//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
//			merge_ram_size = 0;
//#endif
//			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
//			N->blocks[Victim].sections[i].valid = 0;
//			//SLB統計PM會merge到那些Cylinder上
//			j = 0; while (DB_Cylinder[j] == 0){ j++; }
//			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
//			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
//			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
//			//找到s_DirtyCyl, e_DirtyCyl
//			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
//			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
//			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
//			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
//			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
//				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
//					assert(DB_Cylinder[j + 1] >= 0);
//					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
//					else{
//						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
//						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
//						CleanRegionTmp = 0;//找到,統計完歸0 
//					}
//				}
//				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
//					N->LB_DirtySectors += DB_Cylinder[j];
//					N->DirtyInRange_Cylinders++;
//					if (j + 1 < NC->blockSizeCylinder){
//						assert(DB_Cylinder[j + 1] >= 0);
//						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
//							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
//							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
//							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
//							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
//						}
//					}
//				}
//			}
//			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
//				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
//				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
//			}
//			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
//			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
//		}
//		else {
//			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
//		}
//		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
//		//assert(N->blocks[Victim].sections[i].valid == 0);
//		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
//	}
//
//	if (Merge_Bands > 0){
//		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
//		N->Effect_Merge++;
//	}
//	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
//#ifdef Simulation_SLB_DELAY
//	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
//	//assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
//	assert(SLB->lastRWsection == SLB->guardTrack1stSection);
//	for (i = tsp_start; i <= tsp_end; i++){
//		if (N->blocks[NC->PsizeBlock - 1].sections[i].valid == 3){
//			//確認mapping正確
//			SectorTableNo2 = N->blocks[NC->LsizeBlock].sections[i].sector;
//			dataBlock = SectorTableNo2 / NC->blockSizeSector;
//			dataSector = SectorTableNo2 % NC->blockSizeSector;
//			assert(N->blocks[dataBlock].sections[dataSector].sector == i);
//			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
//			//
//			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
//			N->blocks[dataBlock].sections[dataSector].valid = 1;
//			N->blocks[dataBlock].sections[dataSector].sector = SectorTableNo2;
//			//暫存victim上其他Delay Sector
//			assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
//			N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
//		}
//	}
//	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
//	for (i = 0; i < N->VC_BUFF_Len; i++){
//		SectorTableNo2 = N->VictimCylinder_BUFF[i];
//		dataBlock = SectorTableNo2 / NC->blockSizeSector;
//		dataSector = SectorTableNo2 % NC->blockSizeSector;
//		assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
//		assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
//		//更新SLB2DB: 寫到SLB裡
//		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
//		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
//		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
//		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 3;//
//		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
//		N->blocks[dataBlock].sections[dataSector].valid = 0;
//		N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
//		//換下一個SLB Sector
//		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
//	}
//#endif
//	//update SMR LB information
//	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
//		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
//		assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
//	}
//	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
//#ifdef Time_Overhead
//	if (N->VC_BUFF_Len > 0){
//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
//		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }//把DBtrack舊資料整條讀到merge_ram裡
//		//transfer time
//		N->Req_RunTime += (unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
//		assert(N->Req_RunTime >= 0);
//	}
//	free(merge_ram);
//#endif
//	//SLB統計PM會merge到那些Cylinder上
//	free(DB_Cylinder);
//	//fprintf(fp_DELAY_LEFT, " -> %lu, SLB_DELAY_USE %lu\n", N->VC_BUFF_Len, N->SLB_DELAY_USE);
//	//fclose(fp_DELAY_LEFT);
//}
void FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE(sim *simPtr) {//參數控制的DELAY LEFT, DELAY PERIOD可調整
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	DWORD dataSector;
	DWORD LBsector;
	//Delay clean region統計
	DWORD DELAY_CleanRegionMAx = 0;//MAX
	DWORD DELAY_CleanRegion_StartCyl = 0, DELAY_CleanRegion_EndCyl = 0;
	DWORD DELAY_CleanRegionTmp = 0;//tmp
	DWORD DELAY_CleanRegion_StartCylTmp = 0;

	//FILE *fp_DELAY_LEFT = fopen("DELAY_LEFT.txt", "a");

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	//tsp is "trackSizePage"
	const DWORD tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	const DWORD tsp_end = (SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeSector == 4096);
#ifdef Simulation_SLB_DELAY
	N->VC_BUFF_Len = 0;//暫存Delay Sector
#endif
	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 3){}//遇到Delay Sector, 等Victim Merge結束再寫回新LastRW
		else if (N->blocks[Victim].sections[i].valid == 1){ //一般Dirty Sector, 做Band Merge
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
			DWORD DELAY_SIG = 0;
#ifdef Simulation_SLB_DELAY
			DWORD DelaySector = 0; DWORD preDelaySector = 0;//檢查舊Delay Sector
			const DWORD DirtyUnit_TD = N->DirtyUnit_TD, SaveCylinder_TD = N->SaveCylinder_TD;
			/*成功Delay省多少Cylinder*/ DWORD tmpDELAY_SAVE_CYLINDER = 0, FirstDirty_Cylinder = 0;
			//限定Band Merge含第二次以上才可以Delay
			if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] > 0){//上次Merge有做過Delay
				if (N->BAND_DELAY_PERIOD[dataBlock] < N->DELAY_PERIOD){//還在允許的Delay次數內
					//檢查舊Delay Sector有確實留在SLB裡
					for (j = 0; j < NC->blockSizeSector; j++){
						if (N->blocks[dataBlock].sections[j].valid == 0 && N->blocks[Victim].sections[N->blocks[dataBlock].sections[j].sector].valid == 3){ preDelaySector++; }
					}assert(preDelaySector == N->DELAY_SECTORinBAND[dataBlock]);
					//確認SHUFFLE_BOUND的範圍正確
					assert(0 < N->SHUFFLE_BOUND[dataBlock]); assert(N->SHUFFLE_BOUND[dataBlock] < NC->blockSizeCylinder);
					assert(N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_USE);
					//計算繼續Delay總共會有多少Sector: 在DELAY_SECTORinBAND以左的Sector一定比原本DELAY_SECTORinBAND多
					for (j = 0; j < N->SHUFFLE_BOUND[dataBlock]; j++){
						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
						DelaySector += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
					}assert(DelaySector >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DelaySector - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE && DelaySector <= DirtyUnit_TD){
						DELAY_SIG = 3; //成功繼續Delay
						//更新Delay Info: SLB裡Delay的Sector增加
						N->SLB_DELAY_USE += (DelaySector - N->DELAY_SECTORinBAND[dataBlock]);
						N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
						N->DELAY_SECTORinBAND[dataBlock] = DelaySector; assert(N->SHUFFLE_BOUND[dataBlock] != 0);
						//找繼續Delay Band上的當前start_Cylinder
						j = N->SHUFFLE_BOUND[dataBlock]; while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; if (j == NC->blockSizeCylinder){ break; } }
						assert(j <= NC->blockSizeCylinder); //assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
						start_Cylinder = j;
						start_Track = start_Cylinder*NC->CylinderSizeTrack;
						start_Sector = start_Cylinder*NC->CylinderSizeSector;
						//fprintf(fp_DELAY_LEFT, "D:%lu, ", DelaySector);
						//有再次Delay成功 
						assert(N->BAND_DELAY_PERIOD[dataBlock] > 0);
						N->BAND_DELAY_PERIOD[dataBlock]++; 
						/*繼續Delay省寫多少Cylinder*/
						j = 0; while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0); assert(j < N->SHUFFLE_BOUND[dataBlock]);
						FirstDirty_Cylinder = j;
						tmpDELAY_SAVE_CYLINDER = (N->SHUFFLE_BOUND[dataBlock] - FirstDirty_Cylinder);
					}
				}
			}else if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] == 0){//沒有做過Delay  
				//找出DB上第一條有髒資料的P Address Cylinder
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0; while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
				assert(j < NC->blockSizeCylinder); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				start_Cylinder = j;
				//找出Max CR
				DELAY_CleanRegionMAx = 0;//這裡修改, 改從Dirty Range找最右MaxCR// start_Cylinder + 1; //紀錄初始最左Max CR的大小
				DELAY_CleanRegion_StartCyl = 0; DELAY_CleanRegion_EndCyl = start_Cylinder;//紀錄初始最左Max CR的起點, 終點 //DELAY_CleanRegion_EndCyl沒在用
				//tmp 紀錄目前算到的CR
				DELAY_CleanRegionTmp = 0;//從Band剩下的CR裡面找出Max CR
				DELAY_CleanRegion_StartCylTmp = 0; //用不到 DELAY_CleanRegion_EndCylTmp = 0;
				//開始找start_Cylinder以右最大的CR
				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
					if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0 && j + 1 < NC->blockSizeCylinder){//最右邊01有處理,但是00沒處理
						//保證Dirty_j是0
						assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] >= 0);
						if (N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] == 0){ //累積連續CR空間
							DELAY_CleanRegionTmp++;
							if (DELAY_CleanRegionTmp == 1){ DELAY_CleanRegion_StartCylTmp = j; }
						}else{//Dirty_j+1是>0
							if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
								DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
								DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
								DELAY_CleanRegion_EndCyl = j;
							}
							DELAY_CleanRegionTmp = 0;//找到,統計完歸0 
						}
					}
				}//處理尾段是00
				/*if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
					DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
					DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
					DELAY_CleanRegion_EndCyl = NC->blockSizeCylinder - 1;
				}*/
				assert(DELAY_CleanRegion_StartCyl <= DELAY_CleanRegion_EndCyl);
				assert(DELAY_CleanRegion_EndCyl < NC->blockSizeCylinder);
				//確保最大CR不是在最左邊, 才有Delay價值
				DelaySector = 0;
				DWORD L_Index, LSN;//邏輯位址, LSN: L_SectorNo
				if (DELAY_CleanRegion_StartCyl != 0){
					assert(start_Cylinder < DELAY_CleanRegion_StartCyl);
					//Delay統計預先跑一次, 真的要做再跑流程
					//換到邏輯位址計算Delay和一般sector的量, 確認Max CR上沒有髒
					for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L, Logical Cylinder No
						for (k = 0; k < NC->CylinderSizeSector; k++){
							LSN = L_Index*NC->CylinderSizeSector + k;
							if (start_Cylinder <= j && j < DELAY_CleanRegion_StartCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 0 || N->blocks[dataBlock].sections[LSN].valid == 1);
								if (N->blocks[dataBlock].sections[LSN].valid == 0){ DelaySector++; }//可能會Delay的段落: 算出需要Delay多少Sector
							}else if (DELAY_CleanRegion_StartCyl <= j && j < DELAY_CleanRegion_EndCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 1);//確定Max CR裡真的沒東西
							}else{} //要Shuffle的段落
						}
					}
					//以下三條件都符合就確定Delay
					if (DelaySector <= DirtyUnit_TD){ DELAY_SIG++; }//Max CR以左的Dirty Sector <= Dirty門檻
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DelaySector <= N->SLB_DELAY_SPACE){ DELAY_SIG++; }//SLB Delay Space還有空間// - N->DELAY_SECTORinBAND[dataBlock]
					assert(start_Cylinder < DELAY_CleanRegion_EndCyl);
					if (DELAY_CleanRegion_EndCyl + 1 - start_Cylinder >= SaveCylinder_TD){ DELAY_SIG++; }//Delay省的Merge寫入 >= Save Region門檻
				}
				if (DELAY_SIG == 3){
					//for (j = start_Cylinder; j < DELAY_CleanRegion_StartCyl; j++){//Delay的段落
					//	L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L
					//	for (k = 0; k < NC->CylinderSizeSector; k++){
					//		LSN = L_Index*NC->CylinderSizeSector + k;
					//		I64 Band_SectorValid = N->blocks[dataBlock].sections[LSN].valid;
					//		SectorTableNo2 = dataBlock*NC->blockSizeSector + LSN;
					//		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
					//		if (Band_SectorValid == 0){
					//			LBsector = N->blocks[dataBlock].sections[LSN].sector;
					//			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					//			//確認DB與SLB對應雙向都正確
					//			assert(N->blocks[Victim].sections[LBsector].valid == 1);
					//			assert(N->blocks[Victim].sections[LBsector].sector = SectorTableNo2);
					//		}
					//	}
					//}assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);

					/**/FirstDirty_Cylinder = start_Cylinder;
					//第一次Delay 初始化
					//start_Cylinder移到DELAY_CleanRegion_EndCyl後
					//Delay Sector留SLB, start_Cylinder做Shuffle Merge
					start_Cylinder = DELAY_CleanRegion_EndCyl + 1; assert(start_Cylinder < NC->blockSizeCylinder);
					start_Track = start_Cylinder*NC->CylinderSizeTrack;
					start_Sector = start_Cylinder*NC->CylinderSizeSector;
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Cylinder] > 0);
					//確認是第一次Delay
					assert(N->DELAY_SECTORinBAND[dataBlock] == 0);
					assert(DelaySector > 0);
					//更新Delay Info: SLB裡Delay的Sector增加
					N->SLB_DELAY_USE += DelaySector;
					N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
					N->DELAY_SECTORinBAND[dataBlock] = DelaySector;
					N->SHUFFLE_BOUND[dataBlock] = start_Cylinder; assert(N->SHUFFLE_BOUND[dataBlock] != 0);
					//fprintf(fp_DELAY_LEFT, "D:%lu, ", DelaySector);
					//有Delay成功 
					assert(N->BAND_DELAY_PERIOD[dataBlock] == 0);
					N->BAND_DELAY_PERIOD[dataBlock]++;
					/**/
					assert(FirstDirty_Cylinder < N->SHUFFLE_BOUND[dataBlock]);
					tmpDELAY_SAVE_CYLINDER = (N->SHUFFLE_BOUND[dataBlock] - FirstDirty_Cylinder);
				}
			}
			//Delay失敗
			if (DELAY_SIG != 3){
				//fprintf(fp_DELAY_LEFT, "r:%lu, ", N->DELAY_SECTORinBAND[dataBlock]);
				assert(DELAY_SIG < 3); //如果曾經Delay, 歸還SLB Delay Space
				//歸還: 加總後扣給SLB_DELAY_USE
				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					assert(N->SHUFFLE_BOUND[dataBlock] != 0);
					//更新Delay Info: SLB裡Delay的Sector增加
					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
					N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
					N->DELAY_SECTORinBAND[dataBlock] = 0;
					N->SHUFFLE_BOUND[dataBlock] = 0;
					//確認之前Delay過
					assert(N->BAND_DELAY_PERIOD[dataBlock] > 0);
					N->BAND_DELAY_PERIOD[dataBlock] = 0;
				}
			}
			//統計SLB Live Rate
			if (DELAY_SIG == 3){ 
				N->DELAY_cnt++; 
				/**/ N->DELAY_SAVE_CYLINDER += tmpDELAY_SAVE_CYLINDER;
			}
			N->SLB_USE = N->l2pmapOvd.AA;
			N->CumuSLB_USE += N->SLB_USE;
#endif
			//if (DELAY_SIG == 3){
			//	//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
			//	char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
			//	sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
			//	FILE *fp_FootPrint = fopen(FileName, "a");
			//	fprintf(fp_FootPrint, "%lu\n", N->PMstatistic_DB[dataBlock]);
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
			//	fclose(fp_FootPrint);
			//}

			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Simulation_SLB_DELAY
					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
					//SHuffle過P_Index < N->SHUFFLE_BOUND[dataBlock] 不保證是Delay Sector
					if (DELAY_SIG == 3 && P_Index < N->SHUFFLE_BOUND[dataBlock]){// DELAY_CleanRegion_StartCyl
						//assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
						if (tsp_start <= LBsector && LBsector <= tsp_end){//Victim Cylinder上免費還
							//歸還SLB
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
							//假寫回Band, 等Merge結束繞寫回lastRW
							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
							N->blocks[dataBlock].sections[j].valid = 1;
							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
							//暫存等Merge結束寫到lastRW
							assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
							//Delay Band的SHUFFLE_TL不歸0,累積更新
						}else{ //標為Delay Sector
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 3;
						}//不在Victim Cylinder上, 暫不處理
					}else{
#endif
#ifdef Time_Overhead
						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
						/*assert((DELAY_SIG == 3 && P_Index >= DELAY_CleanRegion_StartCyl) || DELAY_SIG < 3);
						if (!((DELAY_SIG == 3 && P_Index >= DELAY_CleanRegion_StartCyl) || DELAY_SIG < 3)){
							printf("%lu %lu %lu %lu\n", DELAY_SIG, P_Index, DELAY_CleanRegion_StartCyl, N->SHUFFLE_BOUND[dataBlock]); system("pause");
						}*/
						assert((DELAY_SIG == 3 && P_Index >= N->SHUFFLE_BOUND[dataBlock]) || DELAY_SIG < 3);
						N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
						N->blocks[dataBlock].sections[j].valid = 1;
					}
				}else{ printf("SLB merge error\n"); system("pause"); }
			}

#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;

			if (DELAY_SIG != 3){//Delay失敗, 找Shuffle範圍
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0;//找出DB上第一條有髒資料的P address Cylinder
				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
				assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
				start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = j*NC->CylinderSizeTrack;
				start_Sector = j*NC->CylinderSizeSector;
				for (j = 0; j < start_Cylinder; j++){
					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
				}
			}
			////剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			//for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
			//	for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
			//		SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
			//		assert(j < NC->blockSizeCylinder); assert(k < NC->blockSizeCylinder);
			//		if (N->SHUFFLE_TL[dataBlock].WriteFreq[j] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
			//		else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[j] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[j] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }

			//		if (SHUFFLE_TL_SWAP_SIG == 1){
			//			//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
			//			Pj = j; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[j]; assert(Lj < NC->blockSizeCylinder);
			//			Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[k]; assert(Lk < NC->blockSizeCylinder);
			//			//備份K位置上的資料
			//			tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[k]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[k];
			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
			//			//j給k, Pk->Lj, B->A
			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
			//			N->SHUFFLE_TL[dataBlock].WriteFreq[k] = N->SHUFFLE_TL[dataBlock].WriteFreq[j];
			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[k] = N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
			//			assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
			//			//tmp給j, Pj->Lk, A->B
			//			assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
			//			N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
			//			N->SHUFFLE_TL[dataBlock].WriteFreq[j] = tmp_WriteFreq;
			//			N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = tmp_DirtyUnit;
			//			assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
			//			N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
			//		}
			//	}
			//	if (j == 0){ break; }//j,k是DWORD, 沒有負數
			//}//檢查每一個L都有唯一對應到P,反之亦是

			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeCylinder - 1; j >= start_Cylinder; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Cylinder; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeCylinder);
					assert(0 <= k); assert(k < NC->blockSizeCylinder);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeCylinder);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeCylinder);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeCylinder - 1)*NC->blockSizeCylinder / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeCylinder; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);

			//if (dataBlock == 3){
			//	//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
			//	char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
			//	sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
			//	FILE *fp_FootPrint = fopen(FileName, "a");
			//	fprintf(fp_FootPrint, "%lu, DELAY_SIG %lu, start_Cylinder %lu\n", N->PMstatistic_DB[dataBlock], DELAY_SIG, start_Cylinder);
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
			//	fclose(fp_FootPrint);
			//}

			//對應DB的Shuffle資訊只留L2P, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){//0 start_Cylinder
				//assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
					printf("%lu, %lu: %lu %lu: %lu\n", dataBlock, N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
					system("pause");
				}
				//在Delay期間留住Shuffle段落的WriteFreq, 防止Delay Cylinder在Delay結束的時候放到太前面
				if (N->DELAY_SECTORinBAND[dataBlock] == 0){ assert(N->SHUFFLE_BOUND[dataBlock] == 0); N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0; }
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//更新統計資料
			assert(start_Sector <= NC->blockSizeSector);//等於是因為Delay
			//SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - merge_ram_size); //assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

			//歸還DB的sectors記憶體
			//assert(N->DBalloc[dataBlock] == 1);
			//free(N->blocks[dataBlock].sections);
			//N->DBalloc[dataBlock] = 0;
			//N->blocks[block_no].sections = NULL;//dataBlock
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
			
#endif
#ifdef Time_Overhead
			assert(merge_ram_size >= 0);
			if (start_Cylinder == NC->blockSizeCylinder){ assert(merge_ram_size == 0); }
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//統計
				SLB->pageRead += NC->trackSizeSector;
			}
			assert(0 <= merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime //有可能merge_ram_size是0, 直接seek和rotate一次造成多算!!
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			//Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//統計
			SLB->pageRead += merge_ram_size;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					//Calc_RotateOverhead2(simPtr, 0, 4);
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				//Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//紀錄Merge寫多少Track
				N->Merge_Trackcount++;
				//統計
				SLB->pageWrite += NC->trackSizeSector;
			}
			//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
			merge_ram_size = 0;
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		//assert(N->blocks[Victim].sections[i].valid == 0);
		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
#ifdef Simulation_SLB_DELAY
	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
	//assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
	assert(SLB->lastRWsection == SLB->guardTrack1stSection);
	for (i = tsp_start; i <= tsp_end; i++){
		if (N->blocks[NC->PsizeBlock - 1].sections[i].valid == 3){
			//確認mapping正確
			SectorTableNo2 = N->blocks[NC->LsizeBlock].sections[i].sector;
			dataBlock = SectorTableNo2 / NC->blockSizeSector;
			dataSector = SectorTableNo2 % NC->blockSizeSector;
			assert(N->blocks[dataBlock].sections[dataSector].sector == i);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			//
			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
			N->blocks[dataBlock].sections[dataSector].valid = 1;
			N->blocks[dataBlock].sections[dataSector].sector = SectorTableNo2;
			//暫存victim上其他Delay Sector
			assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
			N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
		}
	}
	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
	for (i = 0; i < N->VC_BUFF_Len; i++){
		SectorTableNo2 = N->VictimCylinder_BUFF[i];
		dataBlock = SectorTableNo2 / NC->blockSizeSector;
		dataSector = SectorTableNo2 % NC->blockSizeSector;
		assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
		assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 3;//
		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		N->blocks[dataBlock].sections[dataSector].valid = 0;
		N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
	}
	//總共次數SLB->mergecnt, 統計累積多少寫回
	N->cumuVC_BUFF_Len += N->VC_BUFF_Len;
#endif
#ifdef Time_Overhead
	if (N->VC_BUFF_Len > 0){
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		//Delay Sector做Write Back, 成本另外算
		test(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section, 10);//Read
		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);//Write
		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Read
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section + i, 10);//Read
		}
		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Write
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section + i, 10);//Write
		}//把DBtrack舊資料整條讀到merge_ram裡
		//統計
		SLB->pageRead += N->VC_BUFF_Len; 
		SLB->pageWrite += N->VC_BUFF_Len;
		//統計content_rotate
		N->content_rotate += 2 * N->VC_BUFF_Len;
		N->WB_content_rotate += N->VC_BUFF_Len;//Read
		N->WW_content_rotate += N->VC_BUFF_Len;//Write
		//transfer time
		N->Req_RunTime += 2*(unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
		assert(N->Req_RunTime >= 0);
	}
	free(merge_ram);
#endif
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
//#ifdef Time_Overhead
//	if (N->VC_BUFF_Len > 0){
//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//		test(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section, 0);
//		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }//把DBtrack舊資料整條讀到merge_ram裡
//		//transfer time
//		N->Req_RunTime += (unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
//		assert(N->Req_RunTime >= 0);
//	}
//	free(merge_ram);
//#endif

	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
	//fprintf(fp_DELAY_LEFT, " -> %lu, SLB_DELAY_USE %lu\n", N->VC_BUFF_Len, N->SLB_DELAY_USE);
	//fclose(fp_DELAY_LEFT);
}
void FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE_TRACK(sim *simPtr) {//參數控制的DELAY LEFT, DELAY PERIOD可調整
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD dataSector;
	DWORD LBsector;
	//Delay clean region統計
	DWORD DELAY_CleanRegionMAx = 0;//MAX
	DWORD DELAY_CleanRegion_StartCyl = 0, DELAY_CleanRegion_EndCyl = 0;
	DWORD DELAY_CleanRegionTmp = 0;//tmp
	DWORD DELAY_CleanRegion_StartCylTmp = 0;

	//Track Unit轉成Cylinder Unit, 用Cylinder判斷能不能Delay
	DWORD *Cylinder_DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	//tsp is "trackSizePage"
	const DWORD tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	const DWORD tsp_end = (SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);//4096
#ifdef Simulation_SLB_DELAY
	N->VC_BUFF_Len = 0;//暫存Delay Sector
#endif
	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 3){}//遇到Delay Sector, 等Victim Merge結束再寫回新LastRW
		else if (N->blocks[Victim].sections[i].valid == 1){ //一般Dirty Sector, 做Band Merge
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
			DWORD DELAY_SIG = 0;
#ifdef Simulation_SLB_DELAY
			DWORD DelaySector = 0; DWORD preDelaySector = 0;//檢查舊Delay Sector
			const DWORD DirtyUnit_TD = N->DirtyUnit_TD, SaveCylinder_TD = N->SaveCylinder_TD;//

			//Track Unit轉成Cylinder Unit
			for (j = 0; j < NC->blockSizeCylinder; j++){ Cylinder_DirtyUnit[j] = 0; }
			for (j = 0; j < NC->blockSizeTrack; j++){
				assert(j / NC->CylinderSizeTrack < NC->blockSizeCylinder);
				Cylinder_DirtyUnit[j / NC->CylinderSizeTrack] += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
			}

			//限定Band Merge含第二次以上才可以Delay
			if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] > 0){//上次Merge有做過Delay
				if (N->BAND_DELAY_PERIOD[dataBlock] < N->DELAY_PERIOD){//還在允許的Delay次數內
					//檢查舊Delay Sector有確實留在SLB裡
					for (j = 0; j < NC->blockSizeSector; j++){
						if (N->blocks[dataBlock].sections[j].valid == 0 && N->blocks[Victim].sections[N->blocks[dataBlock].sections[j].sector].valid == 3){ preDelaySector++; }
					}assert(preDelaySector == N->DELAY_SECTORinBAND[dataBlock]);
					//確認SHUFFLE_BOUND的範圍正確
					assert(0 < N->SHUFFLE_BOUND[dataBlock]); assert(N->SHUFFLE_BOUND[dataBlock] < NC->blockSizeCylinder);
					assert(N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_USE);
					//計算繼續Delay總共會有多少Sector: 在DELAY_SECTORinBAND以左的Sector一定比原本DELAY_SECTORinBAND多
					for (j = 0; j < N->SHUFFLE_BOUND[dataBlock]; j++){
						//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
						//DelaySector += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
						assert(Cylinder_DirtyUnit[j] >= 0);
						DelaySector += Cylinder_DirtyUnit[j];
					}assert(DelaySector >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DelaySector - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE && DelaySector <= DirtyUnit_TD){
						DELAY_SIG = 3; //成功繼續Delay
						//更新Delay Info: SLB裡Delay的Sector增加
						N->SLB_DELAY_USE += (DelaySector - N->DELAY_SECTORinBAND[dataBlock]);
						N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
						N->DELAY_SECTORinBAND[dataBlock] = DelaySector; assert(N->SHUFFLE_BOUND[dataBlock] != 0);
						//找繼續Delay Band上的當前start_Cylinder
						j = N->SHUFFLE_BOUND[dataBlock]; 
						//while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; if (j == NC->blockSizeCylinder){ break; } }
						while (Cylinder_DirtyUnit[j] == 0){ j++; if (j == NC->blockSizeCylinder){ break; } }
						assert(j <= NC->blockSizeCylinder);
						start_Cylinder = j;
						start_Track = start_Cylinder*NC->CylinderSizeTrack;
						start_Sector = start_Cylinder*NC->CylinderSizeSector;
						//有再次Delay成功 
						assert(N->BAND_DELAY_PERIOD[dataBlock] > 0);
						N->BAND_DELAY_PERIOD[dataBlock]++; 
					}
				}
			}else if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] == 0){//沒有做過Delay  
				//找出DB上第一條有髒資料的P Address Cylinder
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0; //while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
				while (Cylinder_DirtyUnit[j] == 0){ j++; } assert(j < NC->blockSizeCylinder); 
				//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				assert(Cylinder_DirtyUnit[j] > 0);
				start_Cylinder = j;
				//找出Max CR
				DELAY_CleanRegionMAx = 0;//這裡修改, 改從Dirty Range找最右MaxCR// start_Cylinder + 1; //紀錄初始最左Max CR的大小
				DELAY_CleanRegion_StartCyl = 0; DELAY_CleanRegion_EndCyl = start_Cylinder;//紀錄初始最左Max CR的起點, 終點 //DELAY_CleanRegion_EndCyl沒在用
				//tmp 紀錄目前算到的CR
				DELAY_CleanRegionTmp = 0;//從Band剩下的CR裡面找出Max CR
				DELAY_CleanRegion_StartCylTmp = 0; //用不到 DELAY_CleanRegion_EndCylTmp = 0;
				//開始找start_Cylinder以右最大的CR
				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
					assert(Cylinder_DirtyUnit[j] >= 0);
					if (Cylinder_DirtyUnit[j] == 0 && j + 1 < NC->blockSizeCylinder){//最右邊01有處理,但是00沒處理  N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0
						//保證Dirty_j是0
						//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] >= 0);
						assert(Cylinder_DirtyUnit[j + 1] >= 0);
						for (k = 0; k < NC->CylinderSizeTrack; k++){ assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j*NC->CylinderSizeTrack + k] == 0); }
						if (Cylinder_DirtyUnit[j + 1] == 0){ //累積連續CR空間  N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] == 0
							for (k = 0; k < NC->CylinderSizeTrack; k++){ assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[(j+1)*NC->CylinderSizeTrack + k] == 0); }
							DELAY_CleanRegionTmp++;
							if (DELAY_CleanRegionTmp == 1){ DELAY_CleanRegion_StartCylTmp = j; }
						}else{//Dirty_j+1是>0
							BYTE DIRTY = 0; for (k = 0; k < NC->CylinderSizeTrack; k++){ if (N->SHUFFLE_TL[dataBlock].DirtyUnit[(j+1)*NC->CylinderSizeTrack + k] > 0){ DIRTY = 1; } }
							assert(DIRTY == 1);
							if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
								DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
								DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
								DELAY_CleanRegion_EndCyl = j;
							}
							DELAY_CleanRegionTmp = 0;//找到,統計完歸0 
						}
					}
				}//處理尾段是00
				/*if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
				DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
				DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
				DELAY_CleanRegion_EndCyl = NC->blockSizeCylinder - 1;
				}*/
				assert(DELAY_CleanRegion_StartCyl <= DELAY_CleanRegion_EndCyl);
				assert(DELAY_CleanRegion_EndCyl < NC->blockSizeCylinder);
				//確保最大CR不是在最左邊, 才有Delay價值
				DelaySector = 0;
				DWORD L_Index, LSN;//邏輯位址, LSN: L_SectorNo
				if (DELAY_CleanRegion_StartCyl != 0){
					assert(start_Cylinder < DELAY_CleanRegion_StartCyl);
					//Delay統計預先跑一次, 真的要做再跑流程
					//換到邏輯位址計算Delay和一般sector的量, 確認Max CR上沒有髒
					start_Track = start_Cylinder * NC->CylinderSizeTrack; 
					assert(start_Track < NC->blockSizeTrack);
					for (j = start_Track; j < NC->blockSizeTrack; j++){
						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L, Logical Cylinder No
						for (k = 0; k < NC->trackSizeSector; k++){
							LSN = L_Index*NC->trackSizeSector + k;
							if (start_Cylinder <= j / NC->CylinderSizeTrack && j / NC->CylinderSizeTrack < DELAY_CleanRegion_StartCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 0 || N->blocks[dataBlock].sections[LSN].valid == 1);
								if (N->blocks[dataBlock].sections[LSN].valid == 0){ DelaySector++; }//可能會Delay的段落: 算出需要Delay多少Sector
							}else if (DELAY_CleanRegion_StartCyl <= j / NC->CylinderSizeTrack && j / NC->CylinderSizeTrack < DELAY_CleanRegion_EndCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 1);//確定Max CR裡真的沒東西
							}else{} //要Shuffle的段落
						}
					}
					//以下三條件都符合就確定Delay
					if (DelaySector <= DirtyUnit_TD){ DELAY_SIG++; }//Max CR以左的Dirty Sector <= Dirty門檻
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DelaySector <= N->SLB_DELAY_SPACE){ DELAY_SIG++; }//SLB Delay Space還有空間// - N->DELAY_SECTORinBAND[dataBlock]
					assert(start_Cylinder < DELAY_CleanRegion_EndCyl);
					if (DELAY_CleanRegion_EndCyl + 1 - start_Cylinder >= SaveCylinder_TD){ DELAY_SIG++; }//Delay省的Merge寫入 >= Save Region門檻
				}
				if (DELAY_SIG == 3){
					//第一次Delay 初始化
					//start_Cylinder移到DELAY_CleanRegion_EndCyl後
					//Delay Sector留SLB, start_Cylinder做Shuffle Merge
					start_Cylinder = DELAY_CleanRegion_EndCyl + 1; assert(start_Cylinder < NC->blockSizeCylinder);
					start_Track = start_Cylinder*NC->CylinderSizeTrack;
					start_Sector = start_Cylinder*NC->CylinderSizeSector;
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Cylinder] > 0);
					assert(Cylinder_DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
					for (j = 1; j <= NC->CylinderSizeTrack; j++){ assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Track - j] == 0); }
					assert(Cylinder_DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
					assert(Cylinder_DirtyUnit[start_Cylinder] > 0);
					BYTE DIRTY = 0; for (j = 0; j < NC->CylinderSizeTrack; j++){ if (N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Track + j] > 0){ DIRTY = 1; } }
					assert(DIRTY == 1);
					//確認是第一次Delay
					assert(N->DELAY_SECTORinBAND[dataBlock] == 0);
					assert(DelaySector > 0);
					//更新Delay Info: SLB裡Delay的Sector增加
					N->SLB_DELAY_USE += DelaySector;
					N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
					N->DELAY_SECTORinBAND[dataBlock] = DelaySector;
					N->SHUFFLE_BOUND[dataBlock] = start_Cylinder; assert(N->SHUFFLE_BOUND[dataBlock] != 0);
					//有Delay成功 
					assert(N->BAND_DELAY_PERIOD[dataBlock] == 0);
					N->BAND_DELAY_PERIOD[dataBlock]++;
				}
			}
			//Delay失敗
			if (DELAY_SIG != 3){
				assert(DELAY_SIG < 3); //如果曾經Delay, 歸還SLB Delay Space
				//歸還: 加總後扣給SLB_DELAY_USE
				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					assert(N->SHUFFLE_BOUND[dataBlock] != 0);
					//更新Delay Info: SLB裡Delay的Sector增加
					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
					N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
					N->DELAY_SECTORinBAND[dataBlock] = 0;
					N->SHUFFLE_BOUND[dataBlock] = 0;
					//確認之前Delay過
					assert(N->BAND_DELAY_PERIOD[dataBlock] > 0);
					N->BAND_DELAY_PERIOD[dataBlock] = 0;
				}
			}
			//統計SLB Live Rate
			if (DELAY_SIG == 3){ N->DELAY_cnt++; }
			N->SLB_USE = N->l2pmapOvd.AA;
			N->CumuSLB_USE += N->SLB_USE;
#endif
			//if (DELAY_SIG == 3){
			//	//Shuffle前記錄當前Band的WriteFreq, DirtyUnit
			//	char FileName[30] = "FootPrint", BlockNo[10], FileTail[10] = ".txt";
			//	sprintf(BlockNo, "%lu", dataBlock); strcat(FileName, BlockNo); strcat(FileName, FileTail);
			//	FILE *fp_FootPrint = fopen(FileName, "a");
			//	fprintf(fp_FootPrint, "%lu\n", N->PMstatistic_DB[dataBlock]);
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].WriteFreq[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].DirtyUnit[j]); }fprintf(fp_FootPrint, "\n");
			//	for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_FootPrint, "%3lu ", N->SHUFFLE_TL[dataBlock].P2L_Index[j]); }fprintf(fp_FootPrint, "\n");
			//	fclose(fp_FootPrint);
			//}

			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeTrack; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->trackSizeSector < NC->blockSizeTrack);
					DB_Cylinder[j / NC->trackSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Simulation_SLB_DELAY
					//N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->trackSizeSector];
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
					//SHuffle過P_Index < N->SHUFFLE_BOUND[dataBlock] 不保證是Delay Sector
					if (DELAY_SIG == 3 && P_Index / NC->CylinderSizeTrack < N->SHUFFLE_BOUND[dataBlock]){// DELAY_CleanRegion_StartCyl
						//assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
						if (tsp_start <= LBsector && LBsector <= tsp_end){//Victim Cylinder上免費還
							//歸還SLB
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
							//假寫回Band, 等Merge結束繞寫回lastRW
							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
							N->blocks[dataBlock].sections[j].valid = 1;
							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
							//暫存等Merge結束寫到lastRW
							assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
							//Delay Band的SHUFFLE_TL不歸0,累積更新
						}else{ //標為Delay Sector
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 3;
						}//不在Victim Cylinder上, 暫不處理
					}else{
#endif
#ifdef Time_Overhead
						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
						assert((DELAY_SIG == 3 && P_Index / NC->CylinderSizeTrack >= N->SHUFFLE_BOUND[dataBlock]) || DELAY_SIG < 3);
						N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
						N->blocks[dataBlock].sections[j].valid = 1;
					}
				}else{ printf("SLB merge error\n"); system("pause"); }
			}

#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;

			if (DELAY_SIG != 3){//Delay失敗, 找Shuffle範圍
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0;//找出DB上第一條有髒資料的P address Cylinder
				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
				assert(j <= NC->blockSizeTrack - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
				start_Cylinder = j / NC->CylinderSizeTrack; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = j;
				start_Sector = j * NC->trackSizeSector;
				for (j = 0; j < start_Track; j++){
					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
				}
			}

			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeTrack - 1; j >= start_Track; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Track; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeTrack);
					assert(0 <= k); assert(k < NC->blockSizeTrack);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeTrack);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeTrack);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeTrack - 1)*NC->blockSizeTrack / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeTrack; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);

			//對應DB的Shuffle資訊只留L2P, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = start_Track; j < NC->blockSizeTrack; j++){//0 start_Cylinder
				//assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
					printf("%lu, %lu: %lu %lu: %lu\n", dataBlock, N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
					system("pause");
				}
				//在Delay期間留住Shuffle段落的WriteFreq, 防止Delay Cylinder在Delay結束的時候放到太前面
				if (N->DELAY_SECTORinBAND[dataBlock] == 0){ assert(N->SHUFFLE_BOUND[dataBlock] == 0); N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0; }
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//更新統計資料
			assert(start_Sector <= NC->blockSizeSector);//等於是因為Delay
			//SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - merge_ram_size); //assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

			//歸還DB的sectors記憶體
			//assert(N->DBalloc[dataBlock] == 1);
			//free(N->blocks[dataBlock].sections);
			//N->DBalloc[dataBlock] = 0;
			//N->blocks[block_no].sections = NULL;//dataBlock
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,

#endif
#ifdef Time_Overhead
			assert(merge_ram_size >= 0);
			if (start_Cylinder == NC->blockSizeCylinder){ assert(merge_ram_size == 0); }
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//統計
				SLB->pageRead += NC->trackSizeSector;
			}
			assert(0 <= merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime //如果遇到全Band Delay(幾乎不可能), 則merge_ram_size是0, 直接seek和rotate一次造成多算!!
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			//Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//統計
			SLB->pageRead += merge_ram_size;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					//Calc_RotateOverhead2(simPtr, 0, 4);
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				//Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//紀錄Merge寫多少Track
				N->Merge_Trackcount++;
				//統計
				SLB->pageWrite += NC->trackSizeSector;
			}
			//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
			merge_ram_size = 0;
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeTrack); s_DirtyCyl = j;
			j = NC->blockSizeTrack - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeTrack, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeTrack - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeTrack){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeTrack){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeTrack){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeTrack >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeTrack - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeTrack - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeTrack - e_DirtyCyl);
				if ((NC->blockSizeTrack - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeTrack - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		//assert(N->blocks[Victim].sections[i].valid == 0);
		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
#ifdef Simulation_SLB_DELAY
	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
	//assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
	assert(SLB->lastRWsection == SLB->guardTrack1stSection);
	for (i = tsp_start; i <= tsp_end; i++){
		if (N->blocks[NC->PsizeBlock - 1].sections[i].valid == 3){
			//確認mapping正確
			SectorTableNo2 = N->blocks[NC->LsizeBlock].sections[i].sector;
			dataBlock = SectorTableNo2 / NC->blockSizeSector;
			dataSector = SectorTableNo2 % NC->blockSizeSector;
			assert(N->blocks[dataBlock].sections[dataSector].sector == i);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			//
			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
			N->blocks[dataBlock].sections[dataSector].valid = 1;
			N->blocks[dataBlock].sections[dataSector].sector = SectorTableNo2;
			//暫存victim上其他Delay Sector
			assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
			N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
		}
	}
	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
	for (i = 0; i < N->VC_BUFF_Len; i++){
		SectorTableNo2 = N->VictimCylinder_BUFF[i];
		dataBlock = SectorTableNo2 / NC->blockSizeSector;
		dataSector = SectorTableNo2 % NC->blockSizeSector;
		assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
		assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 3;//
		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		N->blocks[dataBlock].sections[dataSector].valid = 0;
		N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
	}
	//總共次數SLB->mergecnt, 統計累積多少寫回
	N->cumuVC_BUFF_Len += N->VC_BUFF_Len;
#endif
#ifdef Time_Overhead
	if (N->VC_BUFF_Len > 0){
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		//Delay Sector做Write Back, 成本另外算
		test(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section, 10);//Read
		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);//Write
		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Read
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section + i, 10);//Read
		}
		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Write
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section + i, 10);//Write
		}//把DBtrack舊資料整條讀到merge_ram裡
		//統計
		SLB->pageRead += N->VC_BUFF_Len; 
		SLB->pageWrite += N->VC_BUFF_Len;
		//統計content_rotate
		N->content_rotate += 2 * N->VC_BUFF_Len;
		N->WB_content_rotate += N->VC_BUFF_Len;//Read
		N->WW_content_rotate += N->VC_BUFF_Len;//Write
		//transfer time
		N->Req_RunTime += 2*(unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
		assert(N->Req_RunTime >= 0);
	}
	free(merge_ram);
#endif
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
	//#ifdef Time_Overhead
	//	if (N->VC_BUFF_Len > 0){
	//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	//		test(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section, 0);
	//		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }//把DBtrack舊資料整條讀到merge_ram裡
	//		//transfer time
	//		N->Req_RunTime += (unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
	//		assert(N->Req_RunTime >= 0);
	//	}
	//	free(merge_ram);
	//#endif

	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
	//Track Unit轉成Cylinder Unit
	free(Cylinder_DirtyUnit);
}
//DLST_VTLMA
void FASTmergeRWSLB_CONT_DLST_VT(sim *simPtr) {//參數控制的DELAY LEFT, DELAY PERIOD可調整
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD dataSector;
	DWORD LBsector;
	//Delay clean region統計
	DWORD DELAY_CleanRegionMAx = 0;//MAX
	DWORD DELAY_CleanRegion_StartCyl = 0, DELAY_CleanRegion_EndCyl = 0;
	DWORD DELAY_CleanRegionTmp = 0;//tmp
	DWORD DELAY_CleanRegion_StartCylTmp = 0;

	//Track Unit轉成Cylinder Unit, 用Cylinder判斷能不能Delay
	DWORD *Cylinder_DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	//tsp is "trackSizePage"
	//修改
	const DWORD tsp_start = SLB->firstRWsection;
	const DWORD tsp_end = (SLB->firstRWsection + NC->trackSizeSection - 1) % (SLB->Partial_Section + 1); //(SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);//4096
	assert(NC->trackSizeSection == 1024);

#ifdef Simulation_SLB_DELAY
	N->VC_BUFF_Len = 0;//暫存Delay Sector
#endif
	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		
		if (N->blocks[Victim].sections[i].valid == 3){}//遇到Delay Sector, 等Victim Merge結束再寫回新LastRW
		else if (N->blocks[Victim].sections[i].valid == 1){ //一般Dirty Sector, 做Band Merge
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
			DWORD DELAY_SIG = 0;
#ifdef Simulation_SLB_DELAY
			DWORD DelaySector = 0; DWORD preDelaySector = 0;//檢查舊Delay Sector
			const DWORD DirtyUnit_TD = N->DirtyUnit_TD, SaveCylinder_TD = N->SaveCylinder_TD;//

			//Track Unit轉成Cylinder Unit
			for (j = 0; j < NC->blockSizeCylinder; j++){ Cylinder_DirtyUnit[j] = 0; }
			for (j = 0; j < NC->blockSizeTrack; j++){
				assert(j / NC->CylinderSizeTrack < NC->blockSizeCylinder);
				Cylinder_DirtyUnit[j / NC->CylinderSizeTrack] += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
			}

			//限定Band Merge含第二次以上才可以Delay
			if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] > 0){//上次Merge有做過Delay
				if (N->BAND_DELAY_PERIOD[dataBlock] < N->DELAY_PERIOD){//還在允許的Delay次數內
					//檢查舊Delay Sector有確實留在SLB裡
					for (j = 0; j < NC->blockSizeSector; j++){
						if (N->blocks[dataBlock].sections[j].valid == 0 && N->blocks[Victim].sections[N->blocks[dataBlock].sections[j].sector].valid == 3){ preDelaySector++; }
					}assert(preDelaySector == N->DELAY_SECTORinBAND[dataBlock]);
					//確認SHUFFLE_BOUND的範圍正確
					assert(0 < N->SHUFFLE_BOUND[dataBlock]); assert(N->SHUFFLE_BOUND[dataBlock] < NC->blockSizeCylinder);
					assert(N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_USE);
					//計算繼續Delay總共會有多少Sector: 在DELAY_SECTORinBAND以左的Sector一定比原本DELAY_SECTORinBAND多
					for (j = 0; j < N->SHUFFLE_BOUND[dataBlock]; j++){
						//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
						//DelaySector += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
						assert(Cylinder_DirtyUnit[j] >= 0);
						DelaySector += Cylinder_DirtyUnit[j];
					}assert(DelaySector >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DelaySector - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE && DelaySector <= DirtyUnit_TD){
						DELAY_SIG = 3; //成功繼續Delay
						//更新Delay Info: SLB裡Delay的Sector增加
						N->SLB_DELAY_USE += (DelaySector - N->DELAY_SECTORinBAND[dataBlock]);
						N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
						N->DELAY_SECTORinBAND[dataBlock] = DelaySector; assert(N->SHUFFLE_BOUND[dataBlock] != 0);
						//找繼續Delay Band上的當前start_Cylinder
						j = N->SHUFFLE_BOUND[dataBlock]; 
						//while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; if (j == NC->blockSizeCylinder){ break; } }
						while (Cylinder_DirtyUnit[j] == 0){ j++; if (j == NC->blockSizeCylinder){ break; } }
						assert(j <= NC->blockSizeCylinder);
						start_Cylinder = j;
						start_Track = start_Cylinder*NC->CylinderSizeTrack;
						start_Sector = start_Cylinder*NC->CylinderSizeSector;
						//有再次Delay成功 
						assert(N->BAND_DELAY_PERIOD[dataBlock] > 0);
						N->BAND_DELAY_PERIOD[dataBlock]++; 
					}
				}
			}else if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] == 0){//沒有做過Delay  
				//找出DB上第一條有髒資料的P Address Cylinder
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0; //while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
				while (Cylinder_DirtyUnit[j] == 0){ j++; } assert(j < NC->blockSizeCylinder); 
				//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				assert(Cylinder_DirtyUnit[j] > 0);
				start_Cylinder = j;
				//找出Max CR
				DELAY_CleanRegionMAx = 0;//這裡修改, 改從Dirty Range找最右MaxCR// start_Cylinder + 1; //紀錄初始最左Max CR的大小
				DELAY_CleanRegion_StartCyl = 0; DELAY_CleanRegion_EndCyl = start_Cylinder;//紀錄初始最左Max CR的起點, 終點 //DELAY_CleanRegion_EndCyl沒在用
				//tmp 紀錄目前算到的CR
				DELAY_CleanRegionTmp = 0;//從Band剩下的CR裡面找出Max CR
				DELAY_CleanRegion_StartCylTmp = 0; //用不到 DELAY_CleanRegion_EndCylTmp = 0;
				//開始找start_Cylinder以右最大的CR
				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
					assert(Cylinder_DirtyUnit[j] >= 0);
					if (Cylinder_DirtyUnit[j] == 0 && j + 1 < NC->blockSizeCylinder){//最右邊01有處理,但是00沒處理  N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0
						//保證Dirty_j是0
						//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] >= 0);
						assert(Cylinder_DirtyUnit[j + 1] >= 0);
						for (k = 0; k < NC->CylinderSizeTrack; k++){ assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j*NC->CylinderSizeTrack + k] == 0); }
						if (Cylinder_DirtyUnit[j + 1] == 0){ //累積連續CR空間  N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] == 0
							for (k = 0; k < NC->CylinderSizeTrack; k++){ assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[(j+1)*NC->CylinderSizeTrack + k] == 0); }
							DELAY_CleanRegionTmp++;
							if (DELAY_CleanRegionTmp == 1){ DELAY_CleanRegion_StartCylTmp = j; }
						}else{//Dirty_j+1是>0
							BYTE DIRTY = 0; for (k = 0; k < NC->CylinderSizeTrack; k++){ if (N->SHUFFLE_TL[dataBlock].DirtyUnit[(j+1)*NC->CylinderSizeTrack + k] > 0){ DIRTY = 1; } }
							assert(DIRTY == 1);
							if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
								DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
								DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
								DELAY_CleanRegion_EndCyl = j;
							}
							DELAY_CleanRegionTmp = 0;//找到,統計完歸0 
						}
					}
				}//處理尾段是00
				/*if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
				DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
				DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
				DELAY_CleanRegion_EndCyl = NC->blockSizeCylinder - 1;
				}*/
				assert(DELAY_CleanRegion_StartCyl <= DELAY_CleanRegion_EndCyl);
				assert(DELAY_CleanRegion_EndCyl < NC->blockSizeCylinder);
				//確保最大CR不是在最左邊, 才有Delay價值
				DelaySector = 0;
				DWORD L_Index, LSN;//邏輯位址, LSN: L_SectorNo
				if (DELAY_CleanRegion_StartCyl != 0){
					assert(start_Cylinder < DELAY_CleanRegion_StartCyl);
					//Delay統計預先跑一次, 真的要做再跑流程
					//換到邏輯位址計算Delay和一般sector的量, 確認Max CR上沒有髒
					start_Track = start_Cylinder * NC->CylinderSizeTrack; 
					assert(start_Track < NC->blockSizeTrack);
					for (j = start_Track; j < NC->blockSizeTrack; j++){
						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L, Logical Cylinder No
						for (k = 0; k < NC->trackSizeSector; k++){
							LSN = L_Index*NC->trackSizeSector + k;
							if (start_Cylinder <= j / NC->CylinderSizeTrack && j / NC->CylinderSizeTrack < DELAY_CleanRegion_StartCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 0 || N->blocks[dataBlock].sections[LSN].valid == 1);
								if (N->blocks[dataBlock].sections[LSN].valid == 0){ DelaySector++; }//可能會Delay的段落: 算出需要Delay多少Sector
							}else if (DELAY_CleanRegion_StartCyl <= j / NC->CylinderSizeTrack && j / NC->CylinderSizeTrack < DELAY_CleanRegion_EndCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 1);//確定Max CR裡真的沒東西
							}else{} //要Shuffle的段落
						}
					}
					//以下三條件都符合就確定Delay
					if (DelaySector <= DirtyUnit_TD){ DELAY_SIG++; }//Max CR以左的Dirty Sector <= Dirty門檻
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DelaySector <= N->SLB_DELAY_SPACE){ DELAY_SIG++; }//SLB Delay Space還有空間// - N->DELAY_SECTORinBAND[dataBlock]
					assert(start_Cylinder < DELAY_CleanRegion_EndCyl);
					if (DELAY_CleanRegion_EndCyl + 1 - start_Cylinder >= SaveCylinder_TD){ DELAY_SIG++; }//Delay省的Merge寫入 >= Save Region門檻
				}
				if (DELAY_SIG == 3){
					//第一次Delay 初始化
					//start_Cylinder移到DELAY_CleanRegion_EndCyl後
					//Delay Sector留SLB, start_Cylinder做Shuffle Merge
					start_Cylinder = DELAY_CleanRegion_EndCyl + 1; assert(start_Cylinder < NC->blockSizeCylinder);
					start_Track = start_Cylinder*NC->CylinderSizeTrack;
					start_Sector = start_Cylinder*NC->CylinderSizeSector;
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Cylinder] > 0);
					assert(Cylinder_DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
					for (j = 1; j <= NC->CylinderSizeTrack; j++){ assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Track - j] == 0); }
					assert(Cylinder_DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
					assert(Cylinder_DirtyUnit[start_Cylinder] > 0);
					BYTE DIRTY = 0; for (j = 0; j < NC->CylinderSizeTrack; j++){ if (N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Track + j] > 0){ DIRTY = 1; } }
					assert(DIRTY == 1);
					//確認是第一次Delay
					assert(N->DELAY_SECTORinBAND[dataBlock] == 0);
					assert(DelaySector > 0);
					//更新Delay Info: SLB裡Delay的Sector增加
					N->SLB_DELAY_USE += DelaySector;
					N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
					N->DELAY_SECTORinBAND[dataBlock] = DelaySector;
					N->SHUFFLE_BOUND[dataBlock] = start_Cylinder; assert(N->SHUFFLE_BOUND[dataBlock] != 0);
					//有Delay成功 
					assert(N->BAND_DELAY_PERIOD[dataBlock] == 0);
					N->BAND_DELAY_PERIOD[dataBlock]++;
				}
			}
			//Delay失敗
			if (DELAY_SIG != 3){
				assert(DELAY_SIG < 3); //如果曾經Delay, 歸還SLB Delay Space
				//歸還: 加總後扣給SLB_DELAY_USE
				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					assert(N->SHUFFLE_BOUND[dataBlock] != 0);
					//更新Delay Info: SLB裡Delay的Sector增加
					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
					N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
					N->DELAY_SECTORinBAND[dataBlock] = 0;
					N->SHUFFLE_BOUND[dataBlock] = 0;
					//確認之前Delay過
					assert(N->BAND_DELAY_PERIOD[dataBlock] > 0);
					N->BAND_DELAY_PERIOD[dataBlock] = 0;
				}
			}
			//統計SLB Live Rate
			if (DELAY_SIG == 3){ N->DELAY_cnt++; }
			N->SLB_USE = N->l2pmapOvd.AA;
			N->CumuSLB_USE += N->SLB_USE;
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeTrack; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->trackSizeSector < NC->blockSizeTrack);
					DB_Cylinder[j / NC->trackSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Simulation_SLB_DELAY
					//N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->trackSizeSector];
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
					//SHuffle過P_Index < N->SHUFFLE_BOUND[dataBlock] 不保證是Delay Sector
					if (DELAY_SIG == 3 && P_Index / NC->CylinderSizeTrack < N->SHUFFLE_BOUND[dataBlock]){// DELAY_CleanRegion_StartCyl
						//assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
						if (tsp_start <= LBsector && LBsector <= tsp_end){//Victim Cylinder上免費還
							//歸還SLB
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
							//假寫回Band, 等Merge結束繞寫回lastRW
							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
							N->blocks[dataBlock].sections[j].valid = 1;
							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
							//暫存等Merge結束寫到lastRW
							assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
							//Delay Band的SHUFFLE_TL不歸0,累積更新
						}else{ //標為Delay Sector
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 3;
						}//不在Victim Cylinder上, 暫不處理
					}else{
#endif
#ifdef Time_Overhead
						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
						assert((DELAY_SIG == 3 && P_Index / NC->CylinderSizeTrack >= N->SHUFFLE_BOUND[dataBlock]) || DELAY_SIG < 3);
						N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
						N->blocks[dataBlock].sections[j].valid = 1;
					}
				}else{ printf("SLB merge error\n"); system("pause"); }
			}

#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;

			if (DELAY_SIG != 3){//Delay失敗, 找Shuffle範圍
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0;//找出DB上第一條有髒資料的P address Cylinder
				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
				assert(j <= NC->blockSizeTrack - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
				start_Cylinder = j / NC->CylinderSizeTrack; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = j;
				start_Sector = j * NC->trackSizeSector;
				for (j = 0; j < start_Track; j++){
					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
				}
			}

			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeTrack - 1; j >= start_Track; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Track; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeTrack);
					assert(0 <= k); assert(k < NC->blockSizeTrack);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeTrack);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeTrack);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeTrack - 1)*NC->blockSizeTrack / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeTrack; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);

			//對應DB的Shuffle資訊只留L2P, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = start_Track; j < NC->blockSizeTrack; j++){//0 start_Cylinder
				//assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
					printf("%lu, %lu: %lu %lu: %lu\n", dataBlock, N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
					system("pause");
				}
				//在Delay期間留住Shuffle段落的WriteFreq, 防止Delay Cylinder在Delay結束的時候放到太前面
				if (N->DELAY_SECTORinBAND[dataBlock] == 0){ assert(N->SHUFFLE_BOUND[dataBlock] == 0); N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0; }
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//更新統計資料
			assert(start_Sector <= NC->blockSizeSector);//等於是因為Delay
			//SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - merge_ram_size); //assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

			//歸還DB的sectors記憶體
			//assert(N->DBalloc[dataBlock] == 1);
			//free(N->blocks[dataBlock].sections);
			//N->DBalloc[dataBlock] = 0;
			//N->blocks[block_no].sections = NULL;//dataBlock
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,

#endif
#ifdef Time_Overhead
			assert(merge_ram_size >= 0);
			if (start_Cylinder == NC->blockSizeCylinder){ assert(merge_ram_size == 0); }
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//統計
				SLB->pageRead += NC->trackSizeSector;
			}
			assert(0 <= merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime //如果遇到全Band Delay(幾乎不可能), 則merge_ram_size是0, 直接seek和rotate一次造成多算!!
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			//Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//統計
			SLB->pageRead += merge_ram_size;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					//Calc_RotateOverhead2(simPtr, 0, 4);
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				//Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//紀錄Merge寫多少Track
				N->Merge_Trackcount++;
				//統計
				SLB->pageWrite += NC->trackSizeSector;
			}
			//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
			merge_ram_size = 0;
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeTrack); s_DirtyCyl = j;
			j = NC->blockSizeTrack - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeTrack, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeTrack - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeTrack){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeTrack){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeTrack){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeTrack >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeTrack - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeTrack - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeTrack - e_DirtyCyl);
				if ((NC->blockSizeTrack - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeTrack - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		//assert(N->blocks[Victim].sections[i].valid == 0);
		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
#ifdef Simulation_SLB_DELAY
	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
	//assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
	assert(SLB->lastRWsection == SLB->guardTrack1stSection);
	for (i = tsp_start; i <= tsp_end; i++){
		if (N->blocks[NC->PsizeBlock - 1].sections[i].valid == 3){
			//確認mapping正確
			SectorTableNo2 = N->blocks[NC->LsizeBlock].sections[i].sector;
			dataBlock = SectorTableNo2 / NC->blockSizeSector;
			dataSector = SectorTableNo2 % NC->blockSizeSector;
			assert(N->blocks[dataBlock].sections[dataSector].sector == i);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			//
			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
			N->blocks[dataBlock].sections[dataSector].valid = 1;
			N->blocks[dataBlock].sections[dataSector].sector = SectorTableNo2;
			//暫存victim上其他Delay Sector
			assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
			N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
		}
	}
	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
	for (i = 0; i < N->VC_BUFF_Len; i++){
		SectorTableNo2 = N->VictimCylinder_BUFF[i];
		dataBlock = SectorTableNo2 / NC->blockSizeSector;
		dataSector = SectorTableNo2 % NC->blockSizeSector;
		assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
		assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 3;//
		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		N->blocks[dataBlock].sections[dataSector].valid = 0;
		N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
	}
	//總共次數SLB->mergecnt, 統計累積多少寫回
	N->cumuVC_BUFF_Len += N->VC_BUFF_Len;
#endif
#ifdef Time_Overhead
	if (N->VC_BUFF_Len > 0){
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		//Delay Sector做Write Back, 成本另外算
		test(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section, 10);//Read
		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);//Write
		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Read
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section + i, 10);//Read
		}
		for (i = 0; i < N->VC_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Write
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section + i, 10);//Write
		}//把DBtrack舊資料整條讀到merge_ram裡
		//統計
		SLB->pageRead += N->VC_BUFF_Len; 
		SLB->pageWrite += N->VC_BUFF_Len;
		//統計content_rotate
		N->content_rotate += 2 * N->VC_BUFF_Len;
		N->WB_content_rotate += N->VC_BUFF_Len;//Read
		N->WW_content_rotate += N->VC_BUFF_Len;//Write
		//transfer time
		N->Req_RunTime += 2*(unsigned long long)round((double)N->VC_BUFF_Len / NC->trackSizeSector * 10);
		assert(N->Req_RunTime >= 0);
	}
	free(merge_ram);
#endif
	//update SMR LB information // 0x3fffffff;
	/*SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;
		assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);*/
	//修改
	SLB->firstRWsection = (SLB->firstRWsection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	//for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
	//	N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
	//	assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
	//}
	DWORD tmpSC = SLB->guardTrack1stSection;
	for (i = 0; i < NC->CylinderSizeTrack; i++){
		assert(tmpSC % NC->trackSizeSector == 0);
		for (j = tmpSC; j < tmpSC + NC->trackSizeSector; j++){
			N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0xffffffffffff;
			assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
		}
		tmpSC += NC->trackSizeSector; if (tmpSC > SLB->Partial_Section){ assert(tmpSC == SLB->Partial_Section + 1); tmpSC = 0; }
	}

	SLB->writedSectionNum -= NC->trackSizeSection;

	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
	//Track Unit轉成Cylinder Unit
	free(Cylinder_DirtyUnit);
}
void FASTmergeRWSLB_CONT_DLST_VTLMA(sim *simPtr) {//參數控制的DELAY LEFT, DELAY PERIOD可調整
	DWORD	dataBlock, Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD dataSector;
	DWORD LBsector;
	//Delay clean region統計
	DWORD DELAY_CleanRegionMAx = 0;//MAX
	DWORD DELAY_CleanRegion_StartCyl = 0, DELAY_CleanRegion_EndCyl = 0;
	DWORD DELAY_CleanRegionTmp = 0;//tmp
	DWORD DELAY_CleanRegion_StartCylTmp = 0;

	//Track Unit轉成Cylinder Unit, 用Cylinder判斷能不能Delay
	DWORD *Cylinder_DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	//tsp is "trackSizePage"
	//修改
	const DWORD tsp_start = SLB->firstRWsection;
	const DWORD tsp_end = (SLB->firstRWsection + NC->trackSizeSection - 1) % (SLB->Partial_Section + 1); //(SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);//4096
	assert(NC->trackSizeSection == 1024);

	//新增
	const DWORD MA = Limited_MergeAssociativity;// 1;
	DWORD *LPB_BUFF = (DWORD*)calloc(NC->trackSizeSector, sizeof(DWORD));//LoopBand
	DWORD LPB_BUFF_Len = 0;
	//紀錄Delay+LoopBand共有多少寫回
	N->WB_BUFF_Len = 0;

#ifdef Simulation_SLB_DELAY
	N->VC_BUFF_Len = 0;//暫存Delay Sector
#endif
	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		
		if (N->blocks[Victim].sections[i].valid == 3){}//遇到Delay Sector, 等Victim Merge結束再寫回新LastRW
		else if (N->blocks[Victim].sections[i].valid == 1){ //一般Dirty Sector, 做Band Merge
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

			//新增
			if (Merge_Bands > MA){
				assert(N->blocks[dataBlock].sections[dataSector].sector == i);
				N->blocks[Victim].sections[i].valid = 0;
				N->blocks[dataBlock].sections[dataSector].valid = 1;
				N->blocks[dataBlock].sections[dataSector].sector = dataBlock*NC->blockSizeSector + dataSector;
				assert(LPB_BUFF_Len < NC->trackSizeSector);
				LPB_BUFF[LPB_BUFF_Len] = dataBlock*NC->blockSizeSector + dataSector; LPB_BUFF_Len++;
				continue;
			}

#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
			DWORD start_Sector = 0, start_Track = 0, start_Cylinder = 0;
			DWORD DELAY_SIG = 0;
#ifdef Simulation_SLB_DELAY
			DWORD DelaySector = 0; DWORD preDelaySector = 0;//檢查舊Delay Sector
			const DWORD DirtyUnit_TD = N->DirtyUnit_TD, SaveCylinder_TD = N->SaveCylinder_TD;//

			//Track Unit轉成Cylinder Unit
			for (j = 0; j < NC->blockSizeCylinder; j++){ Cylinder_DirtyUnit[j] = 0; }
			for (j = 0; j < NC->blockSizeTrack; j++){
				assert(j / NC->CylinderSizeTrack < NC->blockSizeCylinder);
				Cylinder_DirtyUnit[j / NC->CylinderSizeTrack] += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
			}

			//限定Band Merge含第二次以上才可以Delay
			if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] > 0){//上次Merge有做過Delay
				if (N->BAND_DELAY_PERIOD[dataBlock] < N->DELAY_PERIOD){//還在允許的Delay次數內
					//檢查舊Delay Sector有確實留在SLB裡
					for (j = 0; j < NC->blockSizeSector; j++){
						if (N->blocks[dataBlock].sections[j].valid == 0 && N->blocks[Victim].sections[N->blocks[dataBlock].sections[j].sector].valid == 3){ preDelaySector++; }
					}assert(preDelaySector == N->DELAY_SECTORinBAND[dataBlock]);
					//確認SHUFFLE_BOUND的範圍正確
					assert(0 < N->SHUFFLE_BOUND[dataBlock]); assert(N->SHUFFLE_BOUND[dataBlock] < NC->blockSizeCylinder);
					assert(N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_USE);
					//計算繼續Delay總共會有多少Sector: 在DELAY_SECTORinBAND以左的Sector一定比原本DELAY_SECTORinBAND多
					for (j = 0; j < N->SHUFFLE_BOUND[dataBlock]; j++){
						//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
						//DelaySector += N->SHUFFLE_TL[dataBlock].DirtyUnit[j];
						assert(Cylinder_DirtyUnit[j] >= 0);
						DelaySector += Cylinder_DirtyUnit[j];
					}assert(DelaySector >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DelaySector - N->DELAY_SECTORinBAND[dataBlock] <= N->SLB_DELAY_SPACE && DelaySector <= DirtyUnit_TD){
						DELAY_SIG = 3; //成功繼續Delay
						//更新Delay Info: SLB裡Delay的Sector增加
						N->SLB_DELAY_USE += (DelaySector - N->DELAY_SECTORinBAND[dataBlock]);
						N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
						N->DELAY_SECTORinBAND[dataBlock] = DelaySector; assert(N->SHUFFLE_BOUND[dataBlock] != 0);
						//找繼續Delay Band上的當前start_Cylinder
						j = N->SHUFFLE_BOUND[dataBlock]; 
						//while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; if (j == NC->blockSizeCylinder){ break; } }
						while (Cylinder_DirtyUnit[j] == 0){ j++; if (j == NC->blockSizeCylinder){ break; } }
						assert(j <= NC->blockSizeCylinder);
						start_Cylinder = j;
						start_Track = start_Cylinder*NC->CylinderSizeTrack;
						start_Sector = start_Cylinder*NC->CylinderSizeSector;
						//有再次Delay成功 
						assert(N->BAND_DELAY_PERIOD[dataBlock] > 0);
						N->BAND_DELAY_PERIOD[dataBlock]++; 
					}
				}
			}else if (N->PMstatistic_DB[dataBlock] >= 1 && N->DELAY_SECTORinBAND[dataBlock] == 0){//沒有做過Delay  
				//找出DB上第一條有髒資料的P Address Cylinder
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0; //while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }
				while (Cylinder_DirtyUnit[j] == 0){ j++; } assert(j < NC->blockSizeCylinder); 
				//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				assert(Cylinder_DirtyUnit[j] > 0);
				start_Cylinder = j;
				//找出Max CR
				DELAY_CleanRegionMAx = 0;//這裡修改, 改從Dirty Range找最右MaxCR// start_Cylinder + 1; //紀錄初始最左Max CR的大小
				DELAY_CleanRegion_StartCyl = 0; DELAY_CleanRegion_EndCyl = start_Cylinder;//紀錄初始最左Max CR的起點, 終點 //DELAY_CleanRegion_EndCyl沒在用
				//tmp 紀錄目前算到的CR
				DELAY_CleanRegionTmp = 0;//從Band剩下的CR裡面找出Max CR
				DELAY_CleanRegion_StartCylTmp = 0; //用不到 DELAY_CleanRegion_EndCylTmp = 0;
				//開始找start_Cylinder以右最大的CR
				for (j = start_Cylinder; j < NC->blockSizeCylinder; j++){
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] >= 0);
					assert(Cylinder_DirtyUnit[j] >= 0);
					if (Cylinder_DirtyUnit[j] == 0 && j + 1 < NC->blockSizeCylinder){//最右邊01有處理,但是00沒處理  N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0
						//保證Dirty_j是0
						//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] >= 0);
						assert(Cylinder_DirtyUnit[j + 1] >= 0);
						for (k = 0; k < NC->CylinderSizeTrack; k++){ assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j*NC->CylinderSizeTrack + k] == 0); }
						if (Cylinder_DirtyUnit[j + 1] == 0){ //累積連續CR空間  N->SHUFFLE_TL[dataBlock].DirtyUnit[j + 1] == 0
							for (k = 0; k < NC->CylinderSizeTrack; k++){ assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[(j+1)*NC->CylinderSizeTrack + k] == 0); }
							DELAY_CleanRegionTmp++;
							if (DELAY_CleanRegionTmp == 1){ DELAY_CleanRegion_StartCylTmp = j; }
						}else{//Dirty_j+1是>0
							BYTE DIRTY = 0; for (k = 0; k < NC->CylinderSizeTrack; k++){ if (N->SHUFFLE_TL[dataBlock].DirtyUnit[(j+1)*NC->CylinderSizeTrack + k] > 0){ DIRTY = 1; } }
							assert(DIRTY == 1);
							if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
								DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
								DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
								DELAY_CleanRegion_EndCyl = j;
							}
							DELAY_CleanRegionTmp = 0;//找到,統計完歸0 
						}
					}
				}//處理尾段是00
				/*if (DELAY_CleanRegionTmp >= DELAY_CleanRegionMAx){
				DELAY_CleanRegionMAx = DELAY_CleanRegionTmp;
				DELAY_CleanRegion_StartCyl = DELAY_CleanRegion_StartCylTmp;
				DELAY_CleanRegion_EndCyl = NC->blockSizeCylinder - 1;
				}*/
				assert(DELAY_CleanRegion_StartCyl <= DELAY_CleanRegion_EndCyl);
				assert(DELAY_CleanRegion_EndCyl < NC->blockSizeCylinder);
				//確保最大CR不是在最左邊, 才有Delay價值
				DelaySector = 0;
				DWORD L_Index, LSN;//邏輯位址, LSN: L_SectorNo
				if (DELAY_CleanRegion_StartCyl != 0){
					assert(start_Cylinder < DELAY_CleanRegion_StartCyl);
					//Delay統計預先跑一次, 真的要做再跑流程
					//換到邏輯位址計算Delay和一般sector的量, 確認Max CR上沒有髒
					start_Track = start_Cylinder * NC->CylinderSizeTrack; 
					assert(start_Track < NC->blockSizeTrack);
					for (j = start_Track; j < NC->blockSizeTrack; j++){
						L_Index = N->SHUFFLE_TL[dataBlock].P2L_Index[j];//P2L, Logical Cylinder No
						for (k = 0; k < NC->trackSizeSector; k++){
							LSN = L_Index*NC->trackSizeSector + k;
							if (start_Cylinder <= j / NC->CylinderSizeTrack && j / NC->CylinderSizeTrack < DELAY_CleanRegion_StartCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 0 || N->blocks[dataBlock].sections[LSN].valid == 1);
								if (N->blocks[dataBlock].sections[LSN].valid == 0){ DelaySector++; }//可能會Delay的段落: 算出需要Delay多少Sector
							}else if (DELAY_CleanRegion_StartCyl <= j / NC->CylinderSizeTrack && j / NC->CylinderSizeTrack < DELAY_CleanRegion_EndCyl){
								assert(N->blocks[dataBlock].sections[LSN].valid == 1);//確定Max CR裡真的沒東西
							}else{} //要Shuffle的段落
						}
					}
					//以下三條件都符合就確定Delay
					if (DelaySector <= DirtyUnit_TD){ DELAY_SIG++; }//Max CR以左的Dirty Sector <= Dirty門檻
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					if (N->SLB_DELAY_USE + DelaySector <= N->SLB_DELAY_SPACE){ DELAY_SIG++; }//SLB Delay Space還有空間// - N->DELAY_SECTORinBAND[dataBlock]
					assert(start_Cylinder < DELAY_CleanRegion_EndCyl);
					if (DELAY_CleanRegion_EndCyl + 1 - start_Cylinder >= SaveCylinder_TD){ DELAY_SIG++; }//Delay省的Merge寫入 >= Save Region門檻
				}
				if (DELAY_SIG == 3){
					//第一次Delay 初始化
					//start_Cylinder移到DELAY_CleanRegion_EndCyl後
					//Delay Sector留SLB, start_Cylinder做Shuffle Merge
					start_Cylinder = DELAY_CleanRegion_EndCyl + 1; assert(start_Cylinder < NC->blockSizeCylinder);
					start_Track = start_Cylinder*NC->CylinderSizeTrack;
					start_Sector = start_Cylinder*NC->CylinderSizeSector;
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
					//assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Cylinder] > 0);
					assert(Cylinder_DirtyUnit[DELAY_CleanRegion_EndCyl] == 0);
					for (j = 1; j <= NC->CylinderSizeTrack; j++){ assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Track - j] == 0); }
					assert(Cylinder_DirtyUnit[DELAY_CleanRegion_EndCyl + 1] > 0);
					assert(Cylinder_DirtyUnit[start_Cylinder] > 0);
					BYTE DIRTY = 0; for (j = 0; j < NC->CylinderSizeTrack; j++){ if (N->SHUFFLE_TL[dataBlock].DirtyUnit[start_Track + j] > 0){ DIRTY = 1; } }
					assert(DIRTY == 1);
					//確認是第一次Delay
					assert(N->DELAY_SECTORinBAND[dataBlock] == 0);
					assert(DelaySector > 0);
					//更新Delay Info: SLB裡Delay的Sector增加
					N->SLB_DELAY_USE += DelaySector;
					N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
					N->DELAY_SECTORinBAND[dataBlock] = DelaySector;
					N->SHUFFLE_BOUND[dataBlock] = start_Cylinder; assert(N->SHUFFLE_BOUND[dataBlock] != 0);
					//有Delay成功 
					assert(N->BAND_DELAY_PERIOD[dataBlock] == 0);
					N->BAND_DELAY_PERIOD[dataBlock]++;
				}
			}
			//Delay失敗
			if (DELAY_SIG != 3){
				assert(DELAY_SIG < 3); //如果曾經Delay, 歸還SLB Delay Space
				//歸還: 加總後扣給SLB_DELAY_USE
				if (N->DELAY_SECTORinBAND[dataBlock] > 0){
					assert(N->SLB_DELAY_USE >= N->DELAY_SECTORinBAND[dataBlock]);
					assert(N->SHUFFLE_BOUND[dataBlock] != 0);
					//更新Delay Info: SLB裡Delay的Sector增加
					N->SLB_DELAY_USE -= N->DELAY_SECTORinBAND[dataBlock];
					N->CumuSLB_DELAY_USE += N->SLB_DELAY_USE;
					N->DELAY_SECTORinBAND[dataBlock] = 0;
					N->SHUFFLE_BOUND[dataBlock] = 0;
					//確認之前Delay過
					assert(N->BAND_DELAY_PERIOD[dataBlock] > 0);
					N->BAND_DELAY_PERIOD[dataBlock] = 0;
				}
			}
			//統計SLB Live Rate
			if (DELAY_SIG == 3){ N->DELAY_cnt++; }
			N->SLB_USE = N->l2pmapOvd.AA;
			N->CumuSLB_USE += N->SLB_USE;
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeTrack; j++){ DB_Cylinder[j] = 0; }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){//SHUFFLE做過後邏輯位址第一個的start_Sector會不能用
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					//printf("%I64u   %I64u\n", N->blocks[dataBlock].sections[j].sector, SectorTableNo2);
					//assert(N->blocks[dataBlock].sections[j] != NULL);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
				}else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
					
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->trackSizeSector < NC->blockSizeTrack);
					DB_Cylinder[j / NC->trackSizeSector]++;//L_Address //N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector]
					//更新統計資料
					validIs0 += NC->Section;
#ifdef Simulation_SLB_DELAY
					//N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
					DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->trackSizeSector];
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[P_Index] > 0);
					//SHuffle過P_Index < N->SHUFFLE_BOUND[dataBlock] 不保證是Delay Sector
					if (DELAY_SIG == 3 && P_Index / NC->CylinderSizeTrack < N->SHUFFLE_BOUND[dataBlock]){// DELAY_CleanRegion_StartCyl
						//assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
						if (tsp_start <= LBsector && LBsector <= tsp_end){//Victim Cylinder上免費還
							//歸還SLB
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
							//假寫回Band, 等Merge結束繞寫回lastRW
							N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
							N->blocks[dataBlock].sections[j].valid = 1;
							//Victim Cylinder上的Band Drity Sector暫存VictimCylinder_BUFF, 等全部Merge結束WB
							//暫存等Merge結束寫到lastRW
							//assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
							assert(N->VC_BUFF_Len < NC->trackSizeSector);
							N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
							//Delay Band的SHUFFLE_TL不歸0,累積更新
						}else{ //標為Delay Sector
							N->blocks[NC->LsizeBlock].sections[LBsector].valid = 3;
						}//不在Victim Cylinder上, 暫不處理
					}else{
#endif
#ifdef Time_Overhead
						merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
#ifdef SLB_Mapping_Cache
#ifdef CFLRU
						BYTE EntrySizeBit = 57;
						DWORD LookaheadEntry = NC->sectorSizeByte * 8 / EntrySizeBit;
						BYTE write = 0;
						I64 dropEntSecStart = SectorTableNo2 / LookaheadEntry *LookaheadEntry;
						DWORD drop_block, drop_sec, drop_same_block, drop_same_sec;
						drop_block = SectorTableNo2 / NC->blockSizeSector;
						drop_sec = SectorTableNo2 % NC->blockSizeSector;
						if (N->blocks[drop_block].sections[drop_sec].cache_stat == 1){
							N->Cache_Merge_withW++;
							for (BYTE offset = 0; offset < LookaheadEntry; offset++){ // clean the same sector's entry data
								drop_same_block = (DWORD)(dropEntSecStart + offset) / NC->blockSizeSector;
								drop_same_sec = (DWORD)(dropEntSecStart + offset) % NC->blockSizeSector;
								assert(N->DBalloc[drop_same_block] == 1);
								if (N->blocks[drop_same_block].sections[drop_same_sec].inCache != NULL && N->blocks[drop_same_block].sections[drop_same_sec].cache_stat == 1){ //clean the bit for those who are still in the cache
									N->blocks[drop_same_block].sections[drop_same_sec].cache_stat = 0;
								}
							}
						}
#endif			
						CacheDrop(simPtr, SectorTableNo2);
#endif
						assert((DELAY_SIG == 3 && P_Index / NC->CylinderSizeTrack >= N->SHUFFLE_BOUND[dataBlock]) || DELAY_SIG < 3);
						N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
						N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
						N->blocks[dataBlock].sections[j].valid = 1;

					}
				}else{ printf("SLB merge error\n"); system("pause"); }
			}

#ifdef Simulation_SLB_SHUFFLE
			//找出DB上最左第一個CR, 得出需要merge的cylinder數
			//Shuffle: Cylinder依照freq, dirty, L_Address排序. 若相同就看下一個條件, 最後一定在L_Address分出勝負
			DWORD SHUFFLE_TL_SWAP_SIG;
			DWORD tmp_WriteFreq, tmp_DirtyUnit;
			DWORD Lj, Pj, Lk, Pk;
			DWORD tmp_MaxIndex;

			if (DELAY_SIG != 3){//Delay失敗, 找Shuffle範圍
				assert(N->SHUFFLE_TL_Alloc[dataBlock] == 1);
				j = 0;//找出DB上第一條有髒資料的P address Cylinder
				while (N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
				assert(j <= NC->blockSizeTrack - 1); assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] > 0);
				//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
				start_Cylinder = j / NC->CylinderSizeTrack; //找出第一條髒cylinder上的第一條髒track和髒sector
				start_Track = j;
				start_Sector = j * NC->trackSizeSector;
				for (j = 0; j < start_Track; j++){
					assert(N->SHUFFLE_TL[dataBlock].WriteFreq[j] == 0);
					assert(N->SHUFFLE_TL[dataBlock].DirtyUnit[j] == 0);
				}
			}

			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeTrack - 1; j >= start_Track; j--){//j, k 都是P裡的位置
				tmp_MaxIndex = j;
				for (k = start_Track; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < NC->blockSizeTrack);
					assert(0 <= k); assert(k < NC->blockSizeTrack);
					if (N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//else if ((N->SHUFFLE_TL[dataBlock].WriteFreq[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].WriteFreq[k]) && (N->SHUFFLE_TL[dataBlock].DirtyUnit[tmp_MaxIndex] == N->SHUFFLE_TL[dataBlock].DirtyUnit[k]) && (N->SHUFFLE_TL[dataBlock].P2L_Index[tmp_MaxIndex] < N->SHUFFLE_TL[dataBlock].P2L_Index[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//不能讓都沒寫過的因為LA比較大就把曾經寫過但這次沒寫的擠下去

					if (SHUFFLE_TL_SWAP_SIG == 1){ tmp_MaxIndex = k; }
				}
				if (tmp_MaxIndex != j){
					//k是tmp_MaxIndex, k不斷和k+1交換, 最後j-1和j換完把tmp_Max放到j位置上
					assert(0 <= tmp_MaxIndex); assert(tmp_MaxIndex < j);
					for (k = tmp_MaxIndex; k < j; k++){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						Pj = k + 1; Lj = N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]; assert(Lj < NC->blockSizeTrack);
						Pk = k; Lk = N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]; assert(Lk < NC->blockSizeTrack);
						//備份K位置上的資料
						tmp_WriteFreq = N->SHUFFLE_TL[dataBlock].WriteFreq[Pk]; tmp_DirtyUnit = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk];
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]); assert(Lk == N->SHUFFLE_TL[dataBlock].P2L_Index[Pk]);
						//j給k, Pk->Lj, B->A
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pk] = Lj;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pk] = N->SHUFFLE_TL[dataBlock].WriteFreq[Pj];
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pk] = N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj];
						assert(Pj == N->SHUFFLE_TL[dataBlock].L2P_Index[Lj]); assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lj] = Pk;
						//tmp給j, Pj->Lk, A->B
						assert(Lj == N->SHUFFLE_TL[dataBlock].P2L_Index[Pj]);
						N->SHUFFLE_TL[dataBlock].P2L_Index[Pj] = Lk;
						N->SHUFFLE_TL[dataBlock].WriteFreq[Pj] = tmp_WriteFreq;
						N->SHUFFLE_TL[dataBlock].DirtyUnit[Pj] = tmp_DirtyUnit;
						assert(Pk == N->SHUFFLE_TL[dataBlock].L2P_Index[Lk]);
						N->SHUFFLE_TL[dataBlock].L2P_Index[Lk] = Pj;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}//檢查每一個L都有唯一對應到P,反之亦是
			const DWORD check_mapping_value = (NC->blockSizeTrack - 1)*NC->blockSizeTrack / 2;
			DWORD L2P_Index_SUM = 0, P2L_Index_SUM = 0;
			for (j = 0; j < NC->blockSizeTrack; j++){
				L2P_Index_SUM += N->SHUFFLE_TL[dataBlock].L2P_Index[j];
				P2L_Index_SUM += N->SHUFFLE_TL[dataBlock].P2L_Index[j];
			}assert(L2P_Index_SUM == check_mapping_value); assert(P2L_Index_SUM == check_mapping_value);

			//對應DB的Shuffle資訊只留L2P, P2L. 因為要重新統計所以把freq, dirty清成0
			for (j = start_Track; j < NC->blockSizeTrack; j++){//0 start_Cylinder
				//assert(DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] == N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);//確認沒有少寫
				if (DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]] != N->SHUFFLE_TL[dataBlock].DirtyUnit[j]){
					printf("%lu, %lu: %lu %lu: %lu\n", dataBlock, N->SHUFFLE_TL[dataBlock].P2L_Index[j], DB_Cylinder[N->SHUFFLE_TL[dataBlock].P2L_Index[j]], j, N->SHUFFLE_TL[dataBlock].DirtyUnit[j]);
					system("pause");
				}
				//在Delay期間留住Shuffle段落的WriteFreq, 防止Delay Cylinder在Delay結束的時候放到太前面
				if (N->DELAY_SECTORinBAND[dataBlock] == 0){ assert(N->SHUFFLE_BOUND[dataBlock] == 0); N->SHUFFLE_TL[dataBlock].WriteFreq[j] = 0; }
				N->SHUFFLE_TL[dataBlock].DirtyUnit[j] = 0;
			}
#endif
			//更新統計資料
			assert(start_Sector <= NC->blockSizeSector);//等於是因為Delay
			//SLB->pageRead += (NC->blockSizeSector - start_Sector); SLB->pageWrite += (NC->blockSizeSector - start_Sector);
			validIs1 += (NC->blockSizeSector - start_Sector - merge_ram_size); //assert(NC->blockSizeSector - start_Sector >= validIs0);
			SLB->merge_count += (NC->blockSizeSector - start_Sector);
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			//assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

			//歸還DB的sectors記憶體
			//assert(N->DBalloc[dataBlock] == 1);
			//free(N->blocks[dataBlock].sections);
			//N->DBalloc[dataBlock] = 0;
			//N->blocks[block_no].sections = NULL;//dataBlock
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,

#endif
#ifdef Time_Overhead
			assert(merge_ram_size >= 0);
			if (start_Cylinder == NC->blockSizeCylinder){ assert(merge_ram_size == 0); }
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = start_Track; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//統計
				SLB->pageRead += NC->trackSizeSector;
			}
			assert(0 <= merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime //如果遇到全Band Delay(幾乎不可能), 則merge_ram_size是0, 直接seek和rotate一次造成多算!!
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			//Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//統計
			SLB->pageRead += merge_ram_size;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					//Calc_RotateOverhead2(simPtr, 0, 4);
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++; //
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;

#ifdef POWERFAIL
			PF_DiskCacheWrite(simPtr, start_Track);
#endif
			//combine DB和SLB資料
			for (j = start_Track; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				//Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//紀錄Merge寫多少Track
				N->Merge_Trackcount++;
				//統計
				SLB->pageWrite += NC->trackSizeSector;
			}
			//assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder - start_Cylinder);//Delay Band
			merge_ram_size = 0;
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeTrack); s_DirtyCyl = j;
			j = NC->blockSizeTrack - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeTrack, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeTrack - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeTrack){
					assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeTrack){
						assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeTrack){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeTrack >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeTrack - k);
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeTrack - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeTrack - e_DirtyCyl);
				if ((NC->blockSizeTrack - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeTrack - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}//一般BandMerge
		else {
			NoMerge_Band++; assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		//assert(N->blocks[Victim].sections[i].valid == 0);
		assert(N->blocks[Victim].sections[i].valid == 0 || N->blocks[Victim].sections[i].valid == 3);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
#ifdef Simulation_SLB_DELAY
	//DWORD P_Index = N->SHUFFLE_TL[dataBlock].L2P_Index[j / NC->CylinderSizeSector];
	//assert((SLB->lastRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection);
	assert(SLB->lastRWsection == SLB->guardTrack1stSection);
	for (i = tsp_start; i <= tsp_end; i++){
		if (N->blocks[NC->PsizeBlock - 1].sections[i].valid == 3){
			//確認mapping正確
			SectorTableNo2 = N->blocks[NC->LsizeBlock].sections[i].sector;
			dataBlock = SectorTableNo2 / NC->blockSizeSector;
			dataSector = SectorTableNo2 % NC->blockSizeSector;
			assert(N->blocks[dataBlock].sections[dataSector].sector == i);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			//
			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
			N->blocks[dataBlock].sections[dataSector].valid = 1;
			N->blocks[dataBlock].sections[dataSector].sector = SectorTableNo2;
			//暫存victim上其他Delay Sector
			assert(N->VC_BUFF_Len < NC->CylinderSizeSector);
			N->VictimCylinder_BUFF[N->VC_BUFF_Len] = SectorTableNo2; N->VC_BUFF_Len++;
		}
	}
	assert(N->VC_BUFF_Len <= NC->CylinderSizeSector);
	for (i = 0; i < N->VC_BUFF_Len; i++){
		SectorTableNo2 = N->VictimCylinder_BUFF[i];
		dataBlock = SectorTableNo2 / NC->blockSizeSector;
		dataSector = SectorTableNo2 % NC->blockSizeSector;
		assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
		assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 3;//
		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		N->blocks[dataBlock].sections[dataSector].valid = 0;
		N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;

#ifdef SLB_Mapping_Cache
		CacheAccess(simPtr, SectorTableNo2, N->blocks[dataBlock].sections[dataSector].sector, 3);
#endif

		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
	}
	//總共次數SLB->mergecnt, 統計累積多少寫回
	N->cumuVC_BUFF_Len += N->VC_BUFF_Len;
#endif
	//新增
	assert(LPB_BUFF_Len <= NC->trackSizeSector);
	for (i = 0; i < LPB_BUFF_Len; i++){
		SectorTableNo2 = LPB_BUFF[i];
		dataBlock = SectorTableNo2 / NC->blockSizeSector;
		dataSector = SectorTableNo2 % NC->blockSizeSector;
		assert(N->blocks[dataBlock].sections[dataSector].valid == 1);
		assert(N->blocks[dataBlock].sections[dataSector].sector == SectorTableNo2);
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo2;
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;//維持一般Dirty Sector狀態
		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		N->blocks[dataBlock].sections[dataSector].valid = 0;
		N->blocks[dataBlock].sections[dataSector].sector = SLB->lastRWsection;

#ifdef SLB_Mapping_Cache
		CacheAccess(simPtr, SectorTableNo2, N->blocks[dataBlock].sections[dataSector].sector, 3);
#endif
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
	}
#ifdef Time_Overhead
	assert(N->VC_BUFF_Len + LPB_BUFF_Len <= NC->trackSizeSector);
	if (N->VC_BUFF_Len + LPB_BUFF_Len > 0){//新增 //N->VC_BUFF_Len > 0
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		//Delay Sector做Write Back, 成本另外算
		test(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section, 10);//Read
		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);//Write
		for (i = 0; i < N->VC_BUFF_Len + LPB_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Read
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->firstRWsection*NC->Section + i, 10);//Read
		}
		for (i = 0; i < N->VC_BUFF_Len + LPB_BUFF_Len; i += NC->trackSizeSector){
			//Calc_RotateOverhead2(simPtr, 0, 10);//Write
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section + i, 10);//Write
		}//把DBtrack舊資料整條讀到merge_ram裡
		//統計
		SLB->pageRead += (N->VC_BUFF_Len + LPB_BUFF_Len);
		SLB->pageWrite += (N->VC_BUFF_Len + LPB_BUFF_Len);
		//統計content_rotate
		N->content_rotate += 2 * (N->VC_BUFF_Len + LPB_BUFF_Len);
		N->WB_content_rotate += (N->VC_BUFF_Len + LPB_BUFF_Len);//Read
		N->WW_content_rotate += (N->VC_BUFF_Len + LPB_BUFF_Len);//Write
		//transfer time
		N->Req_RunTime += 2 * (unsigned long long)round((double)(N->VC_BUFF_Len + LPB_BUFF_Len) / NC->trackSizeSector * 10);
		assert(N->Req_RunTime >= 0);
	}
	//紀錄Delay+LoopBand共有多少寫回
	N->WB_BUFF_Len = N->VC_BUFF_Len + LPB_BUFF_Len;
	free(merge_ram);
#endif
	//update SMR LB information // 0x3fffffff;
	/*SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
	N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;
	assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);*/
	//修改
	SLB->firstRWsection = (SLB->firstRWsection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
	//for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
	//	N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
	//	assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
	//}
	DWORD tmpSC = SLB->guardTrack1stSection;
	for (i = 0; i < NC->CylinderSizeTrack; i++){
		assert(tmpSC % NC->trackSizeSector == 0);
		for (j = tmpSC; j < tmpSC + NC->trackSizeSector; j++){
			N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0xffffffffffff;
			assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
		}
		tmpSC += NC->trackSizeSector; if (tmpSC > SLB->Partial_Section){ assert(tmpSC == SLB->Partial_Section + 1); tmpSC = 0; }
	}

	SLB->writedSectionNum -= NC->trackSizeSection;

	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
	//Track Unit轉成Cylinder Unit
	free(Cylinder_DirtyUnit);
	//新增
	free(LPB_BUFF);
}
#endif
#endif
#endif

#ifdef SACE
//ICCD 2017 Shingle Aware Cache Management
void SACEmerge_NoStNoDB(sim *simPtr) {//一次merge整個data band
	DWORD	Victim;// = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0, j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	dataBlock, dataSector;
	DWORD	t_dataBlock, t_dataSector;

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band = 0;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
	//SACE
	DWORD SACEmerge_ram_size = 0;
	DWORD *SACEmerge_ram = (DWORD*)calloc(2*NC->blockSizeSection, sizeof(DWORD));
	DWORD MergeBGT_Sector = 0;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	//SLB統計PM會merge到那些Cylinder上
	DWORD s_DirtyCyl = 0, e_DirtyCyl = 0;
	DWORD *DB_Cylinder = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == 4096);

	assert(tsp_start <= tsp_end); assert(NC->blockSizeSector == NC->blockSizeSection);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			assert(N->DBalloc[dataBlock] == 1); assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0); assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
#ifdef SACE
			DWORD SKIP_GC = 1; DWORD DT_index = 0, GT_index = 0;
			t_dataBlock = dataBlock; assert(N->blocks[t_dataBlock].SACE_TKDirtySector != NULL);
			for (j = tsp_start; j <= tsp_end; j++){
				if (N->blocks[Victim].sections[j].valid == 1 && N->blocks[Victim].sections[j].sector / NC->blockSizeSector == t_dataBlock){
					t_dataSector = N->blocks[Victim].sections[j].sector % NC->blockSizeSector;
					assert(N->blocks[t_dataBlock].sections[t_dataSector].valid == 0);
					assert(N->blocks[t_dataBlock].sections[t_dataSector].sector == j);
					GT_index = t_dataSector / NC->trackSizeSector + NC->CylinderSizeTrack; assert(GT_index < NC->blockSizeTrack);
					if (N->blocks[t_dataBlock].SACE_TKDirtySector[GT_index] < NC->trackSizeSector){ SKIP_GC = 0; break; }
				}
			}
			if (SKIP_GC == 1){//只做寫回GT前Dirty Sector就能清理victim track
				N->MergeBGT++; SACEmerge_ram_size = 0;
				for (j = tsp_start; j <= tsp_end; j++){
					if (N->blocks[Victim].sections[j].valid == 1 && N->blocks[Victim].sections[j].sector / NC->blockSizeSector == t_dataBlock){
						t_dataSector = N->blocks[Victim].sections[j].sector % NC->blockSizeSector;
						assert(t_dataBlock*NC->blockSizeSector + t_dataSector == N->blocks[Victim].sections[j].sector);
						assert(N->blocks[Victim].sections[j].sector < NC->LsizeSector);
						//寫回原Band
						N->blocks[t_dataBlock].sections[t_dataSector].sector = N->blocks[Victim].sections[j].sector;
						N->blocks[t_dataBlock].sections[t_dataSector].valid = 1;
						//DB上的Sector
						DT_index = t_dataSector / NC->trackSizeSector; assert(DT_index < NC->blockSizeTrack);
						N->blocks[t_dataBlock].SACE_TKDirtySector[DT_index]--;
						assert(N->blocks[t_dataBlock].SACE_TKDirtySector[DT_index] >= 0);
						//invlaid SLB sector
						N->blocks[Victim].sections[j].valid = 0;
						//暫存位置
						SACEmerge_ram[SACEmerge_ram_size] = N->blocks[Victim].sections[j].sector; SACEmerge_ram_size++; assert(SACEmerge_ram_size <= 2 * NC->blockSizeSection);
						SACEmerge_ram[SACEmerge_ram_size] = (DWORD)NC->LsizeSector + j; SACEmerge_ram_size++; assert(SACEmerge_ram_size <= 2 * NC->blockSizeSection);
						
					}
				}
				//算seek rotation xfer
				assert(0 < SACEmerge_ram_size); assert(SACEmerge_ram_size <= 2*NC->blockSizeSection);
				qsort(SACEmerge_ram, SACEmerge_ram_size, sizeof(DWORD), compare);
				//if (SACEmerge_ram_size > 1){ for (j = 0; j < SACEmerge_ram_size - 1; j++){ assert(SACEmerge_ram[j] <= SACEmerge_ram[j + 1]); } }
				//acctime, response time
				test(simPtr, (I64)SACEmerge_ram[0], 4);//seek
				Calc_Rotate_SkewSMR(simPtr, (I64)SACEmerge_ram[0], 4);//rotation
				N->content_rotate += SACEmerge_ram_size; N->MR_content_rotate += SACEmerge_ram_size;//transfer
				N->Req_RunTime += (unsigned long long)round((double)SACEmerge_ram_size / NC->trackSizeSector * 10);//response time
				//更新統計資料
				SLB->pageRead += SACEmerge_ram_size; 
				for (j = 1; j < SACEmerge_ram_size; j++){//電梯法回SLB讀資料
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					if (SACEmerge_ram[j - 1] / NC->trackSizeSector != SACEmerge_ram[j] / NC->trackSizeSector){
						test(simPtr, (I64)SACEmerge_ram[j], 4); 
						Calc_Rotate_SkewSMR(simPtr, (I64)SACEmerge_ram[j], 4);
					}
				}
#ifdef MergeAssoc_SLBpart
				//統計SLB Live Rate
				if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
				N->l2pmapOvd.AA -= (SACEmerge_ram_size/2);
#endif
				MergeBGT_Sector += SACEmerge_ram_size;
				continue;//跳過Band Merge
			}
#endif

#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0;
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = 0; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, SectorTableNo3, 3);
				//Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 3);
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				//SLB統計PM會merge到那些Cylinder上//順便做
				DB_Cylinder[j / NC->CylinderSizeTrack] = 0;
				//更新統計資料
				SLB->pageRead += NC->trackSizeSector;
			}
#endif
			//SLB統計PM會merge到那些Cylinder上
			for (j = 0; j < NC->blockSizeCylinder; j++){ assert(DB_Cylinder[j] == 0); }
			N->PMstatistic_DB[dataBlock]++;
			for (j = 0; j < NC->blockSizeSector; j++){
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				//block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//SLB統計PM會merge到那些Cylinder上
					assert(j / NC->CylinderSizeSector < NC->blockSizeCylinder);
					DB_Cylinder[j / NC->CylinderSizeSector]++;
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//歸還DB的sectors記憶體
			/*assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[dataBlock].sections = NULL;*/
#ifdef SACE
			/*free(N->blocks[dataBlock].SACE_TKDirtySector);
			N->blocks[dataBlock].SACE_TKDirtySector = NULL;*/
			assert(N->blocks[dataBlock].SACE_TKDirtySector != NULL);
			for (j = 0; j < NC->blockSizeTrack; j++){
				N->blocks[dataBlock].SACE_TKDirtySector[j] = 0;//第一次配, Initialize 0 表示目前沒Dirty Sector
			}
#endif
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size; 
			N->req_w_cumu += Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", dataBlock, validIs0, validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//acctime
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);
			N->S_CT++;
			//更新統計資料
			SLB->pageRead += merge_ram_size;
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){ 
					Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);
			//確認seek次數正確
			Merge_W_SeekCount = N->Merge_W_SeekCount;
#ifdef POWERFAIL
			//整個Band做備份, 防止斷電
			PF_DiskCacheWrite(simPtr, 0);
#endif
			//combine DB和SLB資料
			for (j = 0; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				test(simPtr, (I64)SectorTableNo3, 2);
				Calc_Rotate_SkewSMR(simPtr, SectorTableNo3, 2);
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
				N->Merge_Trackcount++;
				//更新統計資料
				SLB->pageWrite += NC->trackSizeSector;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//SLB統計PM會merge到那些Cylinder上
			j = 0; while (DB_Cylinder[j] == 0){ j++; }
			assert(j < NC->blockSizeCylinder); s_DirtyCyl = j;
			j = NC->blockSizeCylinder - 1; while (DB_Cylinder[j] == 0){ j--; }
			assert(0 <= j); e_DirtyCyl = j; assert(s_DirtyCyl <= e_DirtyCyl);
			//找到s_DirtyCyl, e_DirtyCyl
			DWORD CleanRegionMAx = s_DirtyCyl + 1, CleanRegionTmp = 0;//clean region統計
			DWORD PMG_WriteCost = NC->blockSizeCylinder, tmp_PMG_WriteCost = 0;//clean region統計
			N->PM_NoGuard_Cylinders += (NC->blockSizeCylinder - s_DirtyCyl);//PM_NoGuard寫多少
			N->Range_Cylinders += (e_DirtyCyl + 1 - s_DirtyCyl);//有Dirty Cylinder的Range
			for (j = s_DirtyCyl; j <= e_DirtyCyl; j++){//統計Clean Region, 找最佳Guard切點
				if (DB_Cylinder[j] == 0 && j + 1 < NC->blockSizeCylinder){ assert(DB_Cylinder[j + 1] >= 0);
					if (DB_Cylinder[j + 1] == 0){ CleanRegionTmp++; }//找Clean Region
					else{ 
						N->CleanRegion_Count++; N->CleanRegion_CumuSize += CleanRegionTmp;
						if (CleanRegionTmp > CleanRegionMAx){ CleanRegionMAx = CleanRegionTmp; }
						CleanRegionTmp = 0;//找到,統計完歸0 
					}
				}
				if (DB_Cylinder[j] > 0){//預算PMG最佳多少
					N->LB_DirtySectors += DB_Cylinder[j];
					N->DirtyInRange_Cylinders++;
					if (j + 1 < NC->blockSizeCylinder){ assert(DB_Cylinder[j + 1] >= 0);
						if (DB_Cylinder[j + 1] == 0){//算Guard不同地方寫多少, 找寫最少的情況
							k = j + 1; while (DB_Cylinder[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
							assert(j + 1 > s_DirtyCyl); assert(NC->blockSizeCylinder >= k);
							tmp_PMG_WriteCost = (j + 1 - s_DirtyCyl) + (NC->blockSizeCylinder - k); 
							if (tmp_PMG_WriteCost < PMG_WriteCost){ PMG_WriteCost = tmp_PMG_WriteCost; }
						}
					}
				}
			}
			if (e_DirtyCyl < NC->blockSizeCylinder - 1){ //clean region統計
				N->CleanRegion_Count++; N->CleanRegion_CumuSize += (NC->blockSizeCylinder - e_DirtyCyl);
				if ((NC->blockSizeCylinder - e_DirtyCyl) > CleanRegionMAx){ CleanRegionMAx = NC->blockSizeCylinder - e_DirtyCyl; }
			}
			N->CleanRegionMAx_Count++; N->CleanRegionMAx_CumuSize += CleanRegionMAx;
			N->PMG_WriteCost_Sum += PMG_WriteCost;//紀錄最佳Guard位置需要的寫入量
		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
		//統計Effect Merge會寫多少Data Cylinder
		DWORD EM_DC_Index = Merge_Bands;
		if (EM_DC_Index >= N->EM_DataCylinder_Len){ EM_DC_Index = N->EM_DataCylinder_Len - 1; }
		N->EM_DataCylinder[EM_DC_Index]++;
	}
	//SACE統計有效BGT的GC次數
	if (MergeBGT_Sector > 0){ N->Effect_MergeBGT++; if (Merge_Bands > 0){ N->HiddeInEffect_Merge++; } }
	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
#ifdef Time_Overhead
	free(merge_ram);
	//SACE
	free(SACEmerge_ram);
#endif
	//SLB統計PM會merge到那些Cylinder上
	free(DB_Cylinder);
}
#endif



#ifdef Simulation_SLBPM
void FASTmergeRWSLBpart2_Sr_NoStNoDB(sim *simPtr) {//一次merge整個data band
	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	I64	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	j = 0, k = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
	//試驗
	//DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	//試驗
	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD SourceTracks_count = 0, count = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD start_DBtrack = 0, end_DBtrack = 0;

	DWORD NoMerge_Band = 0;
	//初始化 存活LB section
	//N->l2pmapOvd.round_LB_Live_Section = SLB->SLBsection_num;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif

	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);

	int num = 0; assert(tsp_start <= tsp_end);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			DWORD dataSector = N->blocks[Victim].sections[i].sector % NC->blockSizeSector;
			/*if (N->DBalloc[dataBlock] == 0){
			printf("%lu %lu %lu\n", N->blocks[Victim].sections[i].sector, dataBlock, dataSector); system("pause");
			}*/
			assert(N->DBalloc[dataBlock] == 1);
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

			SourceTracks_count = 0;//
#ifdef Time_Overhead
			N->Merge_Bandcount++;//
			merge_ram_writecount = 0; Merge_Trackcount = 0;
			merge_ram_size = 0;
			start_DBtrack = 0; end_DBtrack = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
			DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = 0; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate += NC->trackSizeSector;
				N->MR_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;
			}
			//assert(N->Merge_R_SeekCount - Merge_R_SeekCount == NC->blockSizeCylinder);
#endif
			//			for (j = 0; j < NC->blockSizeSection; j++){
			//				SectorTableNo2 = dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
			//				block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
			//				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
			//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
			//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
			//					assert(j == section_no);
			//					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
			//					//更新統計資料
			//					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
			//					SLB->merge_count += NC->Section;
			//				}
			//				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
			//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
			//					assert(N->blocks[block_no].sections[section_no].valid == 1);
			//					N->blocks[block_no].sections[section_no].valid = 0;//invalid old
			//					//更新統計資料
			//					SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
			//					SLB->merge_count += NC->Section;
			//					DWORD repeat = 0;//
			//					for (count = 0; count < SourceTracks_count; count++){//
			//						if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
			//					}
			//					if (repeat == 0){ //
			//						assert(SourceTracks_count<NC->blockSizeSection);
			//						SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
			//					}
			//#ifdef Time_Overhead
			//					merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
			//#endif
			//					//assert(N->section_cumu[SectorTableNo2 / NC->Section]>0);
			//					//assert(N->section_cumu[SectorTableNo2 / NC->Section] == N->SLB_section_cumu[SectorTableNo2 / NC->Section] + N->DB_section_cumu[SectorTableNo2 / NC->Section]);
			//					//Cumu_Dirty_Sectors += N->section_cumu[SectorTableNo2 / NC->Section];
			//					//N->section_cumu[SectorTableNo2 / NC->Section] = 0;
			//
			//					setblock(simPtr, SectorTableNo2, dataBlock); assert(j < NC->blockSizeSection);
			//					setsection(simPtr, SectorTableNo2, j);
			//					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
			//					N->blocks[dataBlock].sections[j].valid = 1;
			//					//統計這個band在SLB活著的section
			//				}
			//				else{ printf("SLB merge error\n"); system("pause"); }
			//			}

			for (j = 0; j < NC->blockSizeSection; j++){
				SectorTableNo2 = (I64)dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				//block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				block_no = dataBlock; section_no = j;
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					//更新統計資料
					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					DWORD LBsector = N->blocks[dataBlock].sections[j].sector;
					assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
					assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
					N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
					//更新統計資料
					SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = LBsector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//統計這個band在SLB活著的section
				}
				else{ printf("SLB merge error\n"); system("pause"); }
			}
			//歸還DB的sectors記憶體
			assert(N->DBalloc[dataBlock] == 1);
			free(N->blocks[dataBlock].sections);
			N->DBalloc[dataBlock] = 0;
			N->blocks[block_no].sections = NULL;//dataBlock

			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

			/*FILE *t2 = fopen("DBdirtyInSLB.txt", "a");
			fprintf(t2, "dataBlock %5lu, N->l2pmapOvd.AA %lu\n", dataBlock, N->l2pmapOvd.AA);
			fclose(t2);*/
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

			fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs0, validIs1, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
			N->content_rotate += merge_ram_size;
			N->MR_content_rotate += merge_ram_size;
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;//
			merge_ram_writecount++;
			//
			N->S_MRS += (I64)merge_ram_size;
			//transfer time
			N->Req_RunTime += (unsigned long long)round((double)merge_ram_size / NC->trackSizeSector * 10);

			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = 0; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = (I64)dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				//fprintf(t, "mw db %lu\n", SectorTableNo3);
				test(simPtr, (I64)SectorTableNo3, 2);

				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate += NC->trackSizeSector;
				N->MW_content_rotate += NC->trackSizeSector;
				N->Req_RunTime += 10;

				N->Merge_Trackcount++;//
				Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);
#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//N->SourceTracks_count += (I64)SourceTracks_count;
			assert(Merge_Trackcount == NC->blockSizeTrack);
		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
			//assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		//fprintf(ftime, "Merge_Bands %lu, Dirty_Sectors %I64u, Cumu_Dirty_Sectors %I64u\n", Merge_Bands, Dirty_Sectors, Cumu_Dirty_Sectors);
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
	free(SourceTracks);
#ifdef Time_Overhead
	free(merge_ram);
#endif
}

void FREE_CUTPM_DB(sim *simPtr)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);// NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD j;
	DWORD SectorTableNo3, SectorTableNo2;
	DWORD block_no, section_no;
#ifdef Time_Overhead
	N->Merge_Bandcount++;
	N->SLBMPa.merge_ram_size = 0;
#endif
	//紀錄每次有效merge帶走多少band
	N->SLBMPa.Merge_Bands++;
	N->SLBMPa.validIs0 = 0; N->SLBMPa.validIs1 = 0;
	j = 0;
	while (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1){ j++; }//跳過有效的data sector
	const DWORD start_sector = j, start_track = j / NC->trackSizeSector; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
	assert(N->blocks[N->SLBMPa.dataBlock].sections[start_sector].valid == 0);
	assert(0 <= start_sector); assert(start_sector < NC->blockSizeSector);

	//free CUTPM
	j = NC->blockSizeSector - 1;
#ifdef Band_64MB_128Track
	assert(j == 131071);//131072
#endif
#ifdef Band_128MB_256Track
	assert(j == 262143);//262144
#endif
#ifdef Band_256MB_512Track
	assert(j == 524287);//524288
#endif
	while (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1){ j--; }//跳過有效的data sector
	const DWORD end_sector = j, end_track = j / NC->trackSizeSector;
	assert(N->blocks[N->SLBMPa.dataBlock].sections[end_sector].valid == 0);
	assert(start_sector <= end_sector); assert(end_sector < NC->blockSizeSector);
	//

#ifdef Time_Overhead
	for (j = start_track; j <= end_track; j++){//讀DB資料
		SectorTableNo3 = (I64)N->SLBMPa.dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, SectorTableNo3, 3);
		Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
		N->content_rotate += NC->trackSizeSector;
		N->MR_content_rotate += NC->trackSizeSector;
		N->Req_RunTime += 10;
	}
#endif
	for (j = start_sector; j <= end_sector; j++){
		SectorTableNo2 = (I64)N->SLBMPa.dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
		//block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
		block_no = N->SLBMPa.dataBlock; section_no = j;
		if (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
			assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSector);
			assert(N->blocks[N->SLBMPa.dataBlock].sections[j].sector == SectorTableNo2);
			//更新統計資料
			SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; N->SLBMPa.validIs1 += NC->Section;//DB: read hit, DB: write
			SLB->merge_count += NC->Section;
		}
		else if (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 0){ //invalid在LB的該page
			DWORD LBsector = N->blocks[N->SLBMPa.dataBlock].sections[j].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
			//更新統計資料
			SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; N->SLBMPa.validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
			SLB->merge_count += NC->Section;
#ifdef Time_Overhead
			N->SLBMPa.merge_ram[N->SLBMPa.merge_ram_size] = LBsector; N->SLBMPa.merge_ram_size++; assert(N->SLBMPa.merge_ram_size <= NC->blockSizeSection);
#endif
			N->blocks[N->SLBMPa.dataBlock].sections[j].sector = SectorTableNo2;
			N->blocks[N->SLBMPa.dataBlock].sections[j].valid = 1;
			//統計這個band在SLB valid的section, 然後寫回DB
		}
		else{ printf("SLB merge error\n"); system("pause"); }
	}
	//歸還DB的sectors記憶體
	assert(N->DBalloc[N->SLBMPa.dataBlock] == 1);
	free(N->blocks[N->SLBMPa.dataBlock].sections);
	N->DBalloc[N->SLBMPa.dataBlock] = 0;
	N->blocks[N->SLBMPa.dataBlock].sections = NULL;//dataBlock

	//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
	assert(N->SLBMPa.merge_ram_size == N->SLBMPa.validIs0);//當1 section = 1 sector成立
	N->SLBMPa.Dirty_Sectors += (I64)N->SLBMPa.merge_ram_size;
	N->req_w_cumu += N->SLBMPa.Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
	//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
	if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
	N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
	N->l2pmapOvd.AA -= N->SLBMPa.merge_ram_size;
	fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", N->SLBMPa.dataBlock, N->SLBMPa.validIs0, N->SLBMPa.validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
	assert(0 < N->SLBMPa.merge_ram_size); assert(N->SLBMPa.merge_ram_size <= NC->blockSizeSection);
	qsort(N->SLBMPa.merge_ram, N->SLBMPa.merge_ram_size, sizeof(DWORD), compare);
	if (N->SLBMPa.merge_ram_size > 1){ for (j = 0; j < N->SLBMPa.merge_ram_size - 1; j++){ assert(N->SLBMPa.merge_ram[j] <= N->SLBMPa.merge_ram[j + 1]); } }
	//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
	test(simPtr, (I64)NC->LsizeSector + N->SLBMPa.merge_ram[0], 4);//3
	Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
	//xfer time
	//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
	N->content_rotate += N->SLBMPa.merge_ram_size;
	N->MR_content_rotate += N->SLBMPa.merge_ram_size;
	for (j = 1; j < N->SLBMPa.merge_ram_size; j++){//電梯法回SLB讀資料
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, (I64)NC->LsizeSector + N->SLBMPa.merge_ram[j], 4);//3
		if (N->SLBMPa.merge_ram[j - 1] / NC->trackSizeSector != N->SLBMPa.merge_ram[j] / NC->trackSizeSector){
			Calc_RotateOverhead2(simPtr, 0, 4);
		}
	}
	//紀錄LB寫出多少sector
	N->S_MRS += (I64)N->SLBMPa.merge_ram_size;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)N->SLBMPa.merge_ram_size / NC->trackSizeSector * 10);
	//combine DB和SLB資料
	DWORD Merge_W_RotTime = N->Merge_W_RotTime;//確保寫入量正確
	for (j = start_track; j <= end_track; j++){//寫入DB
		SectorTableNo3 = (I64)N->SLBMPa.dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, (I64)SectorTableNo3, 2);
		Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
		N->content_rotate += NC->trackSizeSector;
		N->MW_content_rotate += NC->trackSizeSector;
		N->Req_RunTime += 10;
		//紀錄merge多少track
		N->Merge_Trackcount++;
	}
	assert(0 < N->Merge_W_RotTime - Merge_W_RotTime);
	assert(N->Merge_W_RotTime - Merge_W_RotTime <= NC->blockSizeTrack);
#endif
}
void PM_DB(sim *simPtr)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);// NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD j;
	DWORD SectorTableNo3, SectorTableNo2;
	DWORD block_no, section_no;
#ifdef Time_Overhead
	N->Merge_Bandcount++;
	N->SLBMPa.merge_ram_size = 0;
#endif
	//紀錄每次有效merge帶走多少band
	N->SLBMPa.Merge_Bands++;
	N->SLBMPa.validIs0 = 0; N->SLBMPa.validIs1 = 0;
	j = 0;
	while (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1){ j++; }//跳過有效的data sector
	const DWORD start_sector = j, start_track = j / NC->trackSizeSector; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
	assert(N->blocks[N->SLBMPa.dataBlock].sections[start_sector].valid == 0);
	assert(0 <= start_sector); assert(start_sector < NC->blockSizeSector);
#ifdef Time_Overhead
	for (j = start_track; j < NC->blockSizeTrack; j++){//讀DB資料
		SectorTableNo3 = (I64)N->SLBMPa.dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, SectorTableNo3, 3);
		Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
		N->content_rotate += NC->trackSizeSector;
		N->MR_content_rotate += NC->trackSizeSector;
		N->Req_RunTime += 10;
	}
#endif
	for (j = start_sector; j < NC->blockSizeSector; j++){
		SectorTableNo2 = (I64)N->SLBMPa.dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
		//block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
		block_no = N->SLBMPa.dataBlock; section_no = j;
		if (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
			assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSector);
			assert(N->blocks[N->SLBMPa.dataBlock].sections[j].sector == SectorTableNo2);
			//更新統計資料
			SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; N->SLBMPa.validIs1 += NC->Section;//DB: read hit, DB: write
			SLB->merge_count += NC->Section;
		}
		else if (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 0){ //invalid在LB的該page
			DWORD LBsector = N->blocks[N->SLBMPa.dataBlock].sections[j].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
			//更新統計資料
			SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; N->SLBMPa.validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
			SLB->merge_count += NC->Section;
#ifdef Time_Overhead
			N->SLBMPa.merge_ram[N->SLBMPa.merge_ram_size] = LBsector; N->SLBMPa.merge_ram_size++; assert(N->SLBMPa.merge_ram_size <= NC->blockSizeSection);
#endif
			N->blocks[N->SLBMPa.dataBlock].sections[j].sector = SectorTableNo2;
			N->blocks[N->SLBMPa.dataBlock].sections[j].valid = 1;
			//統計這個band在SLB valid的section, 然後寫回DB
		}
		else{ printf("SLB merge error\n"); system("pause"); }
	}
	//歸還DB的sectors記憶體
	assert(N->DBalloc[N->SLBMPa.dataBlock] == 1);
	free(N->blocks[N->SLBMPa.dataBlock].sections);
	N->DBalloc[N->SLBMPa.dataBlock] = 0;
	N->blocks[N->SLBMPa.dataBlock].sections = NULL;//dataBlock

	//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
	assert(N->SLBMPa.merge_ram_size == N->SLBMPa.validIs0);//當1 section = 1 sector成立
	N->SLBMPa.Dirty_Sectors += (I64)N->SLBMPa.merge_ram_size;
	N->req_w_cumu += N->SLBMPa.Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
	//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
	if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
	N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
	N->l2pmapOvd.AA -= N->SLBMPa.merge_ram_size;
	fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", N->SLBMPa.dataBlock, N->SLBMPa.validIs0, N->SLBMPa.validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
	assert(0 < N->SLBMPa.merge_ram_size); assert(N->SLBMPa.merge_ram_size <= NC->blockSizeSection);
	qsort(N->SLBMPa.merge_ram, N->SLBMPa.merge_ram_size, sizeof(DWORD), compare);
	if (N->SLBMPa.merge_ram_size > 1){ for (j = 0; j < N->SLBMPa.merge_ram_size - 1; j++){ assert(N->SLBMPa.merge_ram[j] <= N->SLBMPa.merge_ram[j + 1]); } }
	//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
	test(simPtr, (I64)NC->LsizeSector + N->SLBMPa.merge_ram[0], 4);//3
	Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
	//xfer time
	//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
	N->content_rotate += N->SLBMPa.merge_ram_size;
	N->MR_content_rotate += N->SLBMPa.merge_ram_size;
	for (j = 1; j < N->SLBMPa.merge_ram_size; j++){//電梯法回SLB讀資料
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, (I64)NC->LsizeSector + N->SLBMPa.merge_ram[j], 4);//3
		if (N->SLBMPa.merge_ram[j - 1] / NC->trackSizeSector != N->SLBMPa.merge_ram[j] / NC->trackSizeSector){
			Calc_RotateOverhead2(simPtr, 0, 4);
		}
	}
	//紀錄LB寫出多少sector
	N->S_MRS += (I64)N->SLBMPa.merge_ram_size;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)N->SLBMPa.merge_ram_size / NC->trackSizeSector * 10);
	//combine DB和SLB資料
	DWORD Merge_W_RotTime = N->Merge_W_RotTime;//確保寫入量正確
	for (j = start_track; j < NC->blockSizeTrack; j++){//寫入DB
		SectorTableNo3 = (I64)N->SLBMPa.dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, (I64)SectorTableNo3, 2);
		Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
		N->content_rotate += NC->trackSizeSector;
		N->MW_content_rotate += NC->trackSizeSector;
		N->Req_RunTime += 10;
		//紀錄merge多少track
		N->Merge_Trackcount++;
	}
	assert(0 < N->Merge_W_RotTime - Merge_W_RotTime);
	assert(N->Merge_W_RotTime - Merge_W_RotTime <= NC->blockSizeTrack);
#endif
}
void CUTPM_DB_MergeRoutine_Time(sim *simPtr, DWORD start_track, DWORD end_track, DWORD TYPE){
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);// NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD j, SectorTableNo3;
#ifdef Time_Overhead
	for (j = start_track; j <= end_track; j++){//讀DB資料
		SectorTableNo3 = (I64)N->SLBMPa.dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, SectorTableNo3, TYPE);
		Calc_RotateOverhead2(simPtr, 0, TYPE);//把DBtrack舊資料整條讀到merge_ram裡
		N->content_rotate += NC->trackSizeSector;
		N->MR_content_rotate += NC->trackSizeSector;
		N->Req_RunTime += 10;
	}
#endif
}
void CUTPM_DB_MergeRoutine_W(sim *simPtr, DWORD start_sector, DWORD end_sector){
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);// NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD j, SectorTableNo2;
	DWORD block_no, section_no;
	for (j = start_sector; j <= end_sector; j++){
		SectorTableNo2 = (I64)N->SLBMPa.dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
		//block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
		block_no = N->SLBMPa.dataBlock; section_no = j;
		if (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
			assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSector);
			assert(N->blocks[N->SLBMPa.dataBlock].sections[j].sector == SectorTableNo2);
			//更新統計資料
			SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; N->SLBMPa.validIs1 += NC->Section;//DB: read hit, DB: write
			SLB->merge_count += NC->Section;
		}
		else if (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 0){ //invalid在LB的該page
			DWORD LBsector = N->blocks[N->SLBMPa.dataBlock].sections[j].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo2);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//invalid old
			//更新統計資料
			SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; N->SLBMPa.validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
			SLB->merge_count += NC->Section;
#ifdef Time_Overhead
			N->SLBMPa.merge_ram[N->SLBMPa.merge_ram_size] = LBsector; N->SLBMPa.merge_ram_size++; assert(N->SLBMPa.merge_ram_size <= NC->blockSizeSection);
#endif
			N->blocks[N->SLBMPa.dataBlock].sections[j].sector = SectorTableNo2;
			N->blocks[N->SLBMPa.dataBlock].sections[j].valid = 1;
			//統計這個band在SLB valid的section, 然後寫回DB
		}
		else{ printf("SLB merge error\n"); system("pause"); }
	}
}
void PMG_Rank_SWAP(PMG_RANK *left, PMG_RANK *right){
	PMG_RANK tmp;
	tmp.DB_No = left->DB_No; tmp.PMG_SaveCyl = left->PMG_SaveCyl;
	left->DB_No = right->DB_No; left->PMG_SaveCyl = right->PMG_SaveCyl;
	right->DB_No = tmp.DB_No; right->PMG_SaveCyl = tmp.PMG_SaveCyl;
}
DWORD DBPMG_WriteCyl_RankIndex(sim *simPtr){
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD i, j, RankIndex = N->MAX_PM_CYLINDER;
	assert(N->NUM_PM_CYLINDER <= N->MAX_PM_CYLINDER);
	//Bubble Sort回傳BandNo做PMG的排名
	for (i = 0; i < N->NUM_PM_CYLINDER; i++){
		for (j = i + 1; j < N->NUM_PM_CYLINDER; j++){//PMG_WriteCyl 小->大, 效率好->爛
			if (N->PMG_Rank[i].PMG_SaveCyl < N->PMG_Rank[j].PMG_SaveCyl) PMG_Rank_SWAP(&(N->PMG_Rank[i]), &(N->PMG_Rank[j]));
		}
	}
	for (i = 0; i < N->NUM_PM_CYLINDER; i++){
		if (i > 0){ assert(N->PMG_Rank[i - 1].PMG_SaveCyl >= N->PMG_Rank[i].PMG_SaveCyl); }
		if (N->SLBMPa.dataBlock == N->PMG_Rank[i].DB_No){ RankIndex = i; break; }
	}assert(RankIndex < N->NUM_PM_CYLINDER);//正常情況一定會找到
	/*if (RankIndex < N->NUM_PM_CYLINDER - 1){
	i = RankIndex + 1;
	while (N->PMG_Rank[RankIndex].PMG_WriteCyl == N->PMG_Rank[i].PMG_WriteCyl){
	i++; if (i == N->NUM_PM_CYLINDER){ break; }
	}assert(i <= N->NUM_PM_CYLINDER);
	PMG_Rank_SWAP(&(N->PMG_Rank[RankIndex]), &(N->PMG_Rank[i - 1]));
	RankIndex = i - 1;
	}*/
	return RankIndex;
}
void CUTPM_DB(sim *simPtr)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);// NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD j = 0, k = 0;
	//DWORD SectorTableNo3, SectorTableNo2;
	//DWORD block_no, section_no;
#ifdef Time_Overhead
	N->Merge_Bandcount++;
	N->SLBMPa.merge_ram_size = 0;
#endif
	//紀錄每次有效merge帶走多少band
	N->SLBMPa.Merge_Bands++;
	N->SLBMPa.validIs0 = 0; N->SLBMPa.validIs1 = 0;

	////找出victim裡面該DB有merge到的最大sector
	//DWORD max_sector = 0;
	//for (j = N->SLBMPa.tsp_start; j <= N->SLBMPa.tsp_end; j++){
	//	if (N->blocks[NC->LsizeBlock].sections[j].valid == 1 && N->blocks[NC->LsizeBlock].sections[j].sector / NC->blockSizeSector == N->SLBMPa.dataBlock){
	//		if (N->blocks[NC->LsizeBlock].sections[j].sector % NC->blockSizeSector > max_sector){ max_sector = N->blocks[NC->LsizeBlock].sections[j].sector % NC->blockSizeSector; }
	//	}
	//}

	j = 0;//start
	while (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1){ j++; }//跳過有效的data sector
	DWORD start_sector = j, start_track = j / NC->trackSizeSector; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
	DWORD StartCyl_Index = j / NC->CylinderSizeSector;
	assert(N->blocks[N->SLBMPa.dataBlock].sections[start_sector].valid == 0);
	assert(0 <= start_sector); assert(start_sector < NC->blockSizeSector);
	j = NC->blockSizeSector - 1;//end
#ifdef Band_64MB_128Track
	assert(j == 131071);//131072
#endif
#ifdef Band_128MB_256Track
	assert(j == 262143);//262144
#endif
#ifdef Band_256MB_512Track
	assert(j == 524287);//524288
#endif
	while (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1){ j--; }//跳過有效的data sector
	DWORD end_sector = j, end_track = j / NC->trackSizeSector;
	DWORD GuardCyl_Index = j / NC->CylinderSizeSector;
	assert(N->blocks[N->SLBMPa.dataBlock].sections[end_sector].valid == 0);
	assert(start_sector <= end_sector); assert(end_sector < NC->blockSizeSector);
	assert(StartCyl_Index <= GuardCyl_Index);
	////最後一個有dirty data的Cyl的下一個Cyl當Guard
	//if (GuardCyl_Index <= NC->blockSizeCylinder - 2){
	//	//end_sector += NC->CylinderSizeSector; assert(end_sector < NC->blockSizeSector);
	//	//end_track += NC->CylinderSizeTrack; assert(end_track < NC->blockSizeTrack);
	//	GuardCyl_Index++; assert(GuardCyl_Index < NC->blockSizeCylinder);
	//}//GuardCyl_Index已經是DB最後一個Cylinder
	//else{ assert(GuardCyl_Index == NC->blockSizeCylinder - 1); }
	
	//有切的DB兩段merge, 需要第二個起點
	DWORD start_sector2 = NC->blockSizeSector, start_track2 = NC->blockSizeTrack, StartCyl_Index2 = NC->blockSizeCylinder;
	
	//最後一個有dirty data的Cylinder當Guard: 拿它的成本當標準, 更省才換Guard位置
	DWORD PMG_WC = GuardCyl_Index + 1 - StartCyl_Index;;//暫存tmp Guard寫入量 //
	const DWORD PM_WC = NC->blockSizeCylinder - StartCyl_Index;
	DWORD PMG_SaveCyl = NC->blockSizeCylinder - GuardCyl_Index - 1, tmpPMG_SaveCyl; assert(NC->blockSizeCylinder >= GuardCyl_Index + 1);
//#ifdef OPT_CylGuard
//	//GuardCyl_Index = 2 * NC->blockSizeCylinder;
//	if (N->CUTPM_SIG == 0){//只有N->CUTPM_SIG 0會提供放Guard的空間, 算出最佳Guard位置給新切or換刀
//		DWORD *DB_DirtyCyl = calloc(NC->blockSizeCylinder, sizeof(DWORD));
//		for (j = 0; j < NC->blockSizeCylinder; j++){ assert(DB_DirtyCyl[j] == 0); }
//		for (j = start_sector; j <= end_sector; j++){
//			if (N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 0) DB_DirtyCyl[j / NC->CylinderSizeSector]++;
//		}
//		for (j = StartCyl_Index + 1; j <= NC->blockSizeCylinder - 3; j++){
//			if ((DB_DirtyCyl[j - 1] > 0 && DB_DirtyCyl[j] == 0) || (N->blocks[N->SLBMPa.dataBlock].Cut == 1 && j == N->blocks[N->SLBMPa.dataBlock].GuardCylIndex)){//是算用j當Guard的寫入量多少
//				k = j + 1; while (DB_DirtyCyl[k] == 0){ k++; if (k == NC->blockSizeCylinder){ break; } }
//				assert(k <= NC->blockSizeCylinder);
//				//算成本
//				PMG_WC = (j + 1 - StartCyl_Index) + (NC->blockSizeCylinder - k);//PMG寫多少
//				tmpPMG_SaveCyl = PM_WC - PMG_WC; assert(PM_WC >= PMG_WC);//PMG比起PM省多少
//				if (tmpPMG_SaveCyl > PMG_SaveCyl){//更省, 更新GuardCyl_Index
//					PMG_SaveCyl = tmpPMG_SaveCyl;
//					end_sector = (j + 1)*NC->CylinderSizeSector - 1; end_track = (j + 1)*NC->CylinderSizeTrack - 1; GuardCyl_Index = j;
//					start_sector2 = k*NC->CylinderSizeSector; start_track2 = k*NC->CylinderSizeTrack; StartCyl_Index2 = k;
//					DWORD l;
//					for (l = end_sector + 1; l < start_sector2; l++) assert(N->blocks[N->SLBMPa.dataBlock].sections[l].valid == 1);
//					assert(end_sector < NC->blockSizeSector);
//					assert(start_sector2 <= NC->blockSizeSector);
//				}
//			}
//		}
//		free(DB_DirtyCyl);
//	}
//#endif
	// && GuardCyl_Index <= 16
	DWORD D1D2 = 0, RankIndex = 0;
	assert(0 <= N->CUTPM_SIG && N->CUTPM_SIG <= 2);
	if (N->CUTPM_SIG == 0){ N->CUTPM_SIG0_Rate++; }
	else if (N->CUTPM_SIG == 1){ N->CUTPM_SIG1_Rate++; }
	else{ N->CUTPM_SIG2_Rate++; }

	if (N->blocks[N->SLBMPa.dataBlock].Cut == 1){ RankIndex = DBPMG_WriteCyl_RankIndex(simPtr); }//計算排名
	if (N->CUTPM_SIG == 0 || N->CUTPM_SIG == 1){ N->ALLOW_PM++; }//PMG統計
	if (N->CUTPM_SIG == 0 && N->blocks[N->SLBMPa.dataBlock].Cut == 1){//切過的要再切
		//if (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex < GuardCyl_Index){//換刀
		//	////PMG_WC沒贏N-1名, 取消續切, 做PM還刀: N->NUM_PM_CYLINDER <= N->MAX_PM_CYLINDER
		//	//if (RankIndex >= N->MAX_PM_CYLINDER - 1){//N->NUM_PM_CYLINDER >= N->MAX_PM_CYLINDER - 1 && 
		//	//	end_sector = NC->blockSizeSector - 1; end_track = NC->blockSizeTrack - 1;
		//	//	N->blocks[N->SLBMPa.dataBlock].Cut = 0;
		//	//	//清除排名資訊
		//	//	assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
		//	//	for (j = RankIndex; j < N->NUM_PM_CYLINDER - 1; j++){
		//	//		N->PMG_Rank[j].DB_No = N->PMG_Rank[j + 1].DB_No;
		//	//		N->PMG_Rank[j].PMG_SaveCyl = N->PMG_Rank[j + 1].PMG_SaveCyl;
		//	//	}
		//	//	N->PMG_Rank[N->NUM_PM_CYLINDER - 1].PMG_SaveCyl = 0;
		//	//	N->NUM_PM_CYLINDER--; //還刀歸還PM_Space
		//	//	assert(0 <= N->NUM_PM_CYLINDER);
		//	//}
		//	//else{//成功續切
		//	//	N->CUTPM_SIG = 1;//找到Guard Cyl
		//	//	N->CUTPM_CANDIDATE_SIG = N->SLBMPa.dataBlock;
		//	//	N->blocks[N->SLBMPa.dataBlock].GuardCylIndex = GuardCyl_Index;
		//	//	N->blocks[N->SLBMPa.dataBlock].PMG_WriteCyl = PMG_WC;
		//	//	//更新排名資訊
		//	//	assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
		//	//	N->PMG_Rank[RankIndex].PMG_SaveCyl = PMG_SaveCyl;// PMG_WC;
		//	//	D1D2 = 1;
		//	//	N->PM_SUCCESS++;//成功發動
		//	//}
		//	N->CUTPM_SIG = 1;//找到Guard Cyl
		//	N->CUTPM_CANDIDATE_SIG = N->SLBMPa.dataBlock;
		//	N->blocks[N->SLBMPa.dataBlock].GuardCylIndex = GuardCyl_Index;
		//	N->blocks[N->SLBMPa.dataBlock].PMG_WriteCyl = PMG_WC;
		//	//更新排名資訊
		//	assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
		//	N->PMG_Rank[RankIndex].PMG_SaveCyl = PMG_SaveCyl;// PMG_WC;
		//	D1D2 = 1;
		//	N->PM_SUCCESS++;//成功發動
		//}
		//else{//新刀在舊刀左邊, 用舊刀
		//	end_sector = (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex)*NC->CylinderSizeSector - 1;
		//	assert(end_sector < NC->blockSizeSector);
		//	end_track = end_sector / NC->trackSizeSector; assert(end_track < NC->blockSizeTrack);
		//	//可能切的位置後面還有dirty sector
		//	j = (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex + 1)*NC->CylinderSizeSector;
		//	while (j < NC->blockSizeSector && N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1){ j++; if (j == NC->blockSizeSector){ break; } }
		//	assert(j <= NC->blockSizeSector);
		//	start_sector2 = j; start_track2 = j / NC->trackSizeSector;
		//	////PMG_WC沒贏24名, 取消續切, 做PM還刀
		//	//DWORD Old_GuardCyl_Index = N->blocks[N->SLBMPa.dataBlock].GuardCylIndex; assert(Old_GuardCyl_Index + 1 - StartCyl_Index > 0);
		//	StartCyl_Index2 = start_sector2 / NC->CylinderSizeSector; assert(StartCyl_Index2 <= NC->blockSizeCylinder);
		//	assert(NC->blockSizeCylinder - StartCyl_Index2 >= 0);
		//	PMG_WC = (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex + 1 - StartCyl_Index) + (NC->blockSizeCylinder - StartCyl_Index2);
		//	N->blocks[N->SLBMPa.dataBlock].PMG_WriteCyl = PMG_WC;
		//	//更新排名資訊
		//	PMG_SaveCyl = PM_WC - PMG_WC; assert(PM_WC >= PMG_WC);
		//	assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
		//	N->PMG_Rank[RankIndex].PMG_SaveCyl = PMG_SaveCyl;// PMG_WC;
		//	D1D2 = 1;
		//	if (PM_WC <= PMG_WC){
		//	}
		//	//N->PM_SUCCESS++;//成功發動
		//	//if (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex == GuardCyl_Index){ 
		//	//	D1D2 = 1; 
		//	//	/*for (j = GuardCyl_Index*NC->CylinderSizeSector; j < (GuardCyl_Index+1)*NC->CylinderSizeSector; j++){
		//	//		assert(N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 0);
		//	//	}*/
		//	//	end_sector = GuardCyl_Index*NC->CylinderSizeSector - 1; end_track = end_sector / NC->trackSizeSector;
		//	//}
		//	//else{
		//	//	end_sector = NC->blockSizeSector - 1; end_track = NC->blockSizeTrack - 1;
		//	//	N->blocks[N->SLBMPa.dataBlock].Cut = 0;
		//	//	//清除排名資訊
		//	//	assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
		//	//	for (j = RankIndex; j < N->NUM_PM_CYLINDER - 1; j++){
		//	//		N->PMG_Rank[j].DB_No = N->PMG_Rank[j + 1].DB_No;
		//	//		N->PMG_Rank[j].PMG_SaveCyl = N->PMG_Rank[j + 1].PMG_SaveCyl;
		//	//	}
		//	//	N->PMG_Rank[N->NUM_PM_CYLINDER - 1].PMG_SaveCyl = 0;
		//	//	N->NUM_PM_CYLINDER--; //還刀歸還PM_Space
		//	//	assert(0 <= N->NUM_PM_CYLINDER);
		//	//}
		//}
		
		//DWORD guard_start_sector = (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex)*NC->CylinderSizeSector;
		//if (start_sector < guard_start_sector){
		//	if (end_sector < guard_start_sector){ end_sector = guard_start_sector - 1; end_track = end_sector / NC->trackSizeSector; }
		//	else{
		//		end_sector = guard_start_sector - 1; end_track = end_sector / NC->trackSizeSector; assert(start_sector <= end_sector);
		//		start_sector2 = guard_start_sector + NC->CylinderSizeSector;
		//		while (start_sector2 < NC->blockSizeSector && N->blocks[N->SLBMPa.dataBlock].sections[start_sector2].valid == 1){ start_sector2++; }
		//		assert(start_sector2 <= NC->blockSizeSector);
		//		assert(end_sector <= start_sector2);
		//		start_track2 = j / NC->trackSizeSector;
		//		StartCyl_Index2 = j / NC->CylinderSizeSector; assert(StartCyl_Index2 < NC->blockSizeCylinder);
		//		//算成本
		//		PMG_WC = (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex + 1 - StartCyl_Index) + (NC->blockSizeCylinder - StartCyl_Index2);//PMG寫多少
		//		PMG_SaveCyl = PM_WC - PMG_WC;//PMG比起PM省多少
		//		//更新排名資訊
		//		assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
		//		N->PMG_Rank[RankIndex].PMG_SaveCyl = PMG_SaveCyl;
		//		D1D2 = 1;
		//	}
		//}else{
		//	assert(guard_start_sector <= start_sector);
		//	if (guard_start_sector <= start_sector && start_sector <= guard_start_sector + NC->CylinderSizeSector - 1){
		//		start_sector = guard_start_sector + NC->CylinderSizeSector;
		//	}//應該還刀//少數
		//	end_sector = NC->blockSizeSector - 1; end_track = NC->blockSizeTrack - 1;
		//}

		if (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex != GuardCyl_Index){
			end_sector = NC->blockSizeSector - 1; end_track = NC->blockSizeTrack - 1;
			N->blocks[N->SLBMPa.dataBlock].Cut = 0;
			//清除排名資訊
			assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
			for (j = RankIndex; j < N->NUM_PM_CYLINDER - 1; j++){
				N->PMG_Rank[j].DB_No = N->PMG_Rank[j + 1].DB_No;
				N->PMG_Rank[j].PMG_SaveCyl = N->PMG_Rank[j + 1].PMG_SaveCyl;
			}
			N->PMG_Rank[N->NUM_PM_CYLINDER - 1].PMG_SaveCyl = 0;
			N->NUM_PM_CYLINDER--; //還刀歸還PM_Space
			assert(0 <= N->NUM_PM_CYLINDER);
		}
		else{
			end_sector =(GuardCyl_Index+1)*NC->CylinderSizeSector - 1; end_track = end_sector / NC->trackSizeSector;
			assert(end_track / NC->CylinderSizeTrack == GuardCyl_Index);
			assert(start_sector <= end_sector);
		}
		/*else{
			end_sector = GuardCyl_Index*NC->CylinderSizeSector - 1; end_track = end_sector / NC->trackSizeSector;
			assert(end_track / NC->CylinderSizeTrack == GuardCyl_Index-1);
		}*/
	}
	else if (N->CUTPM_SIG == 0 && N->blocks[N->SLBMPa.dataBlock].Cut == 0 && N->NUM_PM_CYLINDER < N->MAX_PM_CYLINDER){//第一次切
		N->CUTPM_SIG = 1;//找到Guard Cyl
		N->CUTPM_CANDIDATE_SIG = N->SLBMPa.dataBlock;
		N->blocks[N->SLBMPa.dataBlock].GuardCylIndex = GuardCyl_Index;
		//N->NUM_PM_CYLINDER++;//做PM的DB消耗一個PM_Space
		N->blocks[N->SLBMPa.dataBlock].Cut = 1;//第一次切 設定Cut狀態
		N->blocks[N->SLBMPa.dataBlock].PM = 1;//統計: 標示做過PM
		assert(N->blocks[N->SLBMPa.dataBlock].GR == 0);
		N->PM_SUCCESS++;//成功發動
		//D1D2 = 1;
	    //
		//如果只剩一個名額, PMG_WC沒贏24名不能切
		N->blocks[N->SLBMPa.dataBlock].PMG_WriteCyl = PMG_WC;
		//新增排名資訊
		assert(N->PMG_Rank[N->NUM_PM_CYLINDER].PMG_SaveCyl == 0);
		N->PMG_Rank[N->NUM_PM_CYLINDER].DB_No = N->SLBMPa.dataBlock;
		N->PMG_Rank[N->NUM_PM_CYLINDER].PMG_SaveCyl = PMG_SaveCyl;// PMG_WC;
		//printf("%lu %lu %lu\n", N->SLBMPa.dataBlock, N->PMG_Rank[N->NUM_PM_CYLINDER].DB_No, N->PMG_Rank[N->NUM_PM_CYLINDER].PMG_WriteCyl);
		N->NUM_PM_CYLINDER++;//做PM的DB消耗一個PM_Space
		assert(N->NUM_PM_CYLINDER <= N->MAX_PM_CYLINDER);
		
		/*if (N->SLBMPa.dataBlock == 172){
			printf("%lu %lu, index %lu ->", start_sector, end_sector, GuardCyl_Index);
		}*/

		//// && PMG_WC + 3 < PM_WC
		//N->CUTPM_SIG = 1;//找到Guard Cyl
		//N->CUTPM_CANDIDATE_SIG = N->SLBMPa.dataBlock;
		//N->blocks[N->SLBMPa.dataBlock].GuardCylIndex = GuardCyl_Index;
		////block PMG資訊
		//N->blocks[N->SLBMPa.dataBlock].Cut = 1;//第一次切 設定Cut狀態
		//N->blocks[N->SLBMPa.dataBlock].PM = 1;//統計: 標示做過PM
		//N->blocks[N->SLBMPa.dataBlock].PMG_WriteCyl = PMG_WC;
		////新增排名資訊
		//PMG_SaveCyl = PM_WC - PMG_WC; assert(PM_WC >= PMG_WC);
		//assert(N->PMG_Rank[N->NUM_PM_CYLINDER].PMG_SaveCyl == 0);
		//N->PMG_Rank[N->NUM_PM_CYLINDER].DB_No = N->SLBMPa.dataBlock;
		//N->PMG_Rank[N->NUM_PM_CYLINDER].PMG_SaveCyl = PMG_SaveCyl;// PMG_WC;
		//N->NUM_PM_CYLINDER++;//做PM的DB消耗一個PM_Space
		//assert(N->NUM_PM_CYLINDER <= N->MAX_PM_CYLINDER);
		//N->PM_SUCCESS++;//成功發動
	}
	//else if (N->CUTPM_SIG == 0 || N->CUTPM_SIG == 1){//切失敗，歸還PM Track
	//	end_sector = NC->blockSizeSector - 1; end_track = NC->blockSizeTrack - 1;
	//	if (N->blocks[N->SLBMPa.dataBlock].Cut == 1){
	//		N->blocks[N->SLBMPa.dataBlock].Cut = 0;
	//		N->blocks[N->SLBMPa.dataBlock].GuardTrackIndex = 0;
	//		assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
	//		//清除排名資訊
	//		for (j = RankIndex; j < N->NUM_PM_CYLINDER - 1; j++){
	//			N->PMG_Rank[j].DB_No = N->PMG_Rank[j + 1].DB_No;
	//			N->PMG_Rank[j].PMG_SaveCyl = N->PMG_Rank[j + 1].PMG_SaveCyl;
	//		}
	//		N->PMG_Rank[N->NUM_PM_CYLINDER - 1].PMG_SaveCyl = 0;
	//		N->NUM_PM_CYLINDER--; //還刀歸還PM_Space
	//		assert(0 <= N->NUM_PM_CYLINDER);
	//	}
	//	N->ALLOW_PM++;
	//}
	else{
		assert(0 <= N->CUTPM_SIG && N->CUTPM_SIG <= 2); // assert(N->CUTPM_SIG == 2);//Normal Merge
		if (N->blocks[N->SLBMPa.dataBlock].Cut == 1){
			DWORD guard_start_sector = (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex)*NC->CylinderSizeSector;
			if (start_sector <= guard_start_sector + NC->CylinderSizeSector - 1){
				if (end_sector <= guard_start_sector + NC->CylinderSizeSector - 1){
					end_sector = guard_start_sector + NC->CylinderSizeSector - 1; end_track = end_sector / NC->trackSizeSector;
					assert(end_track / NC->CylinderSizeTrack == N->blocks[N->SLBMPa.dataBlock].GuardCylIndex);
					assert(start_sector <= end_sector);
				}else{
					//start_sector2 = guard_start_sector + NC->CylinderSizeSector;
					//while (start_sector2 < NC->blockSizeSector && N->blocks[N->SLBMPa.dataBlock].sections[start_sector2].valid == 1){ start_sector2++; }
					//assert(start_sector2 <= NC->blockSizeSector);
					//assert(end_sector <= start_sector2);
					//start_track2 = j / NC->trackSizeSector;
					//StartCyl_Index2 = j / NC->CylinderSizeSector; assert(StartCyl_Index2 < NC->blockSizeCylinder);
					////算成本
					//PMG_WC = (N->blocks[N->SLBMPa.dataBlock].GuardCylIndex + 1 - StartCyl_Index) + (NC->blockSizeCylinder - StartCyl_Index2);//PMG寫多少
					//PMG_SaveCyl = PM_WC - PMG_WC;//PMG比起PM省多少
					////更新排名資訊
					//assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
					//N->PMG_Rank[RankIndex].PMG_SaveCyl = PMG_SaveCyl;
					//D1D2 = 1;//多數

					end_sector = NC->blockSizeSector - 1; end_track = NC->blockSizeTrack - 1;
					N->blocks[N->SLBMPa.dataBlock].Cut = 0;
					//清除排名資訊
					assert(N->PMG_Rank[RankIndex].DB_No == N->SLBMPa.dataBlock);
					for (j = RankIndex; j < N->NUM_PM_CYLINDER - 1; j++){
						N->PMG_Rank[j].DB_No = N->PMG_Rank[j + 1].DB_No;
						N->PMG_Rank[j].PMG_SaveCyl = N->PMG_Rank[j + 1].PMG_SaveCyl;
					}
					N->PMG_Rank[N->NUM_PM_CYLINDER - 1].PMG_SaveCyl = 0;
					N->NUM_PM_CYLINDER--; //還刀歸還PM_Space
					assert(0 <= N->NUM_PM_CYLINDER);
				}
			}
			//else{
			//	assert(guard_start_sector <= start_sector);
			//	if (guard_start_sector <= start_sector && start_sector <= guard_start_sector + NC->CylinderSizeSector - 1){
			//		start_sector = guard_start_sector;// +NC->CylinderSizeSector;
			//	}//應該還刀//少數
			//	end_sector = NC->blockSizeSector - 1; end_track = NC->blockSizeTrack - 1;
			//}//如果SIG是0, 1 而且RankIndex是倒數1,2名 PM還刀
		}
		else{ end_sector = NC->blockSizeSector - 1; end_track = NC->blockSizeTrack - 1; }
	}
#ifdef Time_Overhead
	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	CUTPM_DB_MergeRoutine_Time(simPtr, start_track, end_track, 3);//N->CUTPM_SIG == 2 && N->blocks[N->SLBMPa.dataBlock].Cut == 1
	//if (D1D2 == 1){ CUTPM_DB_MergeRoutine_Time(simPtr, start_track2, NC->blockSizeTrack - 1, 3); }
	CUTPM_DB_MergeRoutine_W(simPtr, start_sector, end_sector);
	//if (D1D2 == 1){ CUTPM_DB_MergeRoutine_W(simPtr, start_sector2, NC->blockSizeSector - 1); }
	/*if (N->SLBMPa.dataBlock == 172){
		for (j = start_sector; j <= end_sector; j++) assert(N->blocks[N->SLBMPa.dataBlock].sections[j].valid == 1);
		printf("%lu %lu\n", start_sector, end_sector);
	}*/
#endif
	//歸還DB的sectors記憶體
	assert(N->DBalloc[N->SLBMPa.dataBlock] == 1);
	if (N->blocks[N->SLBMPa.dataBlock].Cut == 0){
		free(N->blocks[N->SLBMPa.dataBlock].sections);
		N->DBalloc[N->SLBMPa.dataBlock] = 0;
		N->blocks[N->SLBMPa.dataBlock].sections = NULL;//dataBlock
	}

	//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
	assert(N->SLBMPa.merge_ram_size == N->SLBMPa.validIs0);//當1 section = 1 sector成立
	N->SLBMPa.Dirty_Sectors += (I64)N->SLBMPa.merge_ram_size;
	N->req_w_cumu += N->SLBMPa.Cumu_Dirty_Sectors;
#ifdef MergeAssoc_SLBpart
	//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
	if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
	N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
	N->l2pmapOvd.AA -= N->SLBMPa.merge_ram_size;
	fprintf(fp_SLB, "%lu (%lu:%lu), %lu\n", N->SLBMPa.dataBlock, N->SLBMPa.validIs0, N->SLBMPa.validIs1, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
	N->NUM_PM_CYLINDER_Rate += N->NUM_PM_CYLINDER;
#endif
#ifdef Time_Overhead
	assert(0 < N->SLBMPa.merge_ram_size); assert(N->SLBMPa.merge_ram_size <= NC->blockSizeSection);
	qsort(N->SLBMPa.merge_ram, N->SLBMPa.merge_ram_size, sizeof(DWORD), compare);
	if (N->SLBMPa.merge_ram_size > 1){ for (j = 0; j < N->SLBMPa.merge_ram_size - 1; j++){ assert(N->SLBMPa.merge_ram[j] <= N->SLBMPa.merge_ram[j + 1]); } }
	//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
	test(simPtr, (I64)NC->LsizeSector + N->SLBMPa.merge_ram[0], 4);//3
	Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
	//xfer time
	//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection)
	N->content_rotate += N->SLBMPa.merge_ram_size;
	N->MR_content_rotate += N->SLBMPa.merge_ram_size;
	for (j = 1; j < N->SLBMPa.merge_ram_size; j++){//電梯法回SLB讀資料
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		test(simPtr, (I64)NC->LsizeSector + N->SLBMPa.merge_ram[j], 4);//3
		if (N->SLBMPa.merge_ram[j - 1] / NC->trackSizeSector != N->SLBMPa.merge_ram[j] / NC->trackSizeSector){
			Calc_RotateOverhead2(simPtr, 0, 4);
		}
	}
	//紀錄LB寫出多少sector
	N->S_MRS += (I64)N->SLBMPa.merge_ram_size;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)N->SLBMPa.merge_ram_size / NC->trackSizeSector * 10);
	//combine DB和SLB資料
	DWORD Merge_W_RotTime = N->Merge_W_RotTime;//確保寫入量正確
	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	CUTPM_DB_MergeRoutine_Time(simPtr, start_track, end_track, 2);
	//if (D1D2 = 1){ CUTPM_DB_MergeRoutine_Time(simPtr, start_track2, NC->blockSizeTrack - 1, 2); }
	//紀錄merge多少track
	N->Merge_Trackcount += (end_track + 1 - start_track + NC->blockSizeTrack - start_track2);
	assert(0 < N->Merge_W_RotTime - Merge_W_RotTime);
	//assert(N->Merge_W_RotTime - Merge_W_RotTime <= NC->blockSizeTrack);
#endif
}
void GuardCyl_WB2SMR(sim *simPtr) { // Handle a write of sectors to the MLC flash
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	DWORD block_no = 0, section_no = 0;
	I64 SectorTableNo = 0;
#ifdef Time_Overhead
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += N->WB2SMR_TASK_LEN;//寫多少sector轉多少sector
	N->WW_content_rotate += N->WB2SMR_TASK_LEN;//
#endif
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)N->WB2SMR_TASK_LEN / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);
	//統計
	SLB->pageWrite += N->WB2SMR_TASK_LEN;
	test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
	Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
	DWORD WB2SMR_cnt = 0;
	for (i = 0; i < N->WB2SMR_TASK_LEN; i++){
		SectorTableNo = N->WB2SMR_TASK[i]; assert(SectorTableNo < NC->LsizeSector);
		block_no = SectorTableNo / NC->blockSizeSector; assert(0 <= block_no && block_no < NC->LsizeBlock);
		section_no = SectorTableNo % NC->blockSizeSector; assert(0 <= section_no && section_no < NC->blockSizeSector);
		//assert(N->DBalloc[block_no] == 1);//有配置的DB
		//assert(section_no / NC->CylinderSizeSector == N->blocks[block_no].GuardCylIndex);
		if (N->DBalloc[block_no] == 1){
			assert(SLB->lastRWsection != SLB->guardTrack1stSection);
			////換GTC位置
			//if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//	//不可能遇到guard track右邊是fRW而發動merge
			//	assert((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) != SLB->firstRWsection);
			//	//guard track右邊不是fRW，右移gt一格
			//	for (j = SLB->guardTrack1stSection; j < SLB->guardTrack1stSection + NC->CylinderSizeSector; j++){
			//		//means the sector is not yet mapping to data band
			//		// 0x3fffffff;//0x7fffffff;
			//		assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0xffffffffffff);
			//		assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
			//	}
			//	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);
			//	for (j = SLB->guardTrack1stSection; j < SLB->guardTrack1stSection + NC->CylinderSizeSector; j++){
			//		assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
			//		N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0xffffffffffff;
			//	}
			//}
			assert(N->blocks[block_no].Cut == 1);
			//LB紀錄
			assert(0 <= SLB->lastRWsection); assert(SLB->lastRWsection <= SLB->Partial_Section);
			assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
			//DB紀錄
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;
			N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;
			//換下一個section做寫入
			SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
			//增加存活sector
			N->l2pmapOvd.AA++;
			//計算寫回LB多少Sector
			WB2SMR_cnt++;

			N->WB_LB_GCyl_sector++;
		}
	}
	N->WB2SMR_TASK_LEN = 0;
	//計算得到多少LB Free Sector
	assert(WB2SMR_cnt >= 0); assert(N->GET_FREE_LB_SECT >= WB2SMR_cnt);
	N->GET_FREE_LB_SECT -= WB2SMR_cnt;
}
void FASTmergeRWSLBpart2_PM(sim *simPtr) {//一次merge整個data band
	DWORD	dataBlock, dataSector;// , BlockNo, RWLBA, , validPages = 0, invalidPages = 0, ret
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);// NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD Victim = NC->PsizeBlock - 1;
	I64	SectorTableNo1 = 0;// , SectorTableNo2 = 0, SectorTableNo3 = 0;
	DWORD	i = 0;// , j = 0, k = 0;
	DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"

	//	DWORD validIs0 = 0, validIs1 = 0;
	//	DWORD block_no = 0, section_no = 0;
	//	//試驗
	//	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	//	//試驗
	//	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	//	DWORD SourceTracks_count = 0, count = 0;
	//#ifdef Time_Overhead
	//	DWORD merge_ram_size = 0;
	//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	//	DWORD start_DBtrack = 0, end_DBtrack = 0;
	//	DWORD NoMerge_Band = 0;
	//	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
	//#endif
	//	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	//	DWORD Merge_Bands = 0;
	//	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;
	
	//merge參數初始化
	N->SLBMPa.validIs0 = 0; N->SLBMPa.validIs1 = 0;
	//N->SLBMPa.block_no = 0; N->SLBMPa.section_no = 0;
	N->SLBMPa.merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	N->SLBMPa.merge_ram_size = 0;
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	N->SLBMPa.Merge_Bands = 0; N->SLBMPa.NoMerge_Band = 0;
	N->SLBMPa.Dirty_Sectors = 0; N->SLBMPa.Cumu_Dirty_Sectors = 0;
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	SLB->mergecnt++;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeSector - 1) % (SLB->Partial_Section + 1);//NC->CylinderSizeTrack*NC->trackSizeSection
	//確認merge cylinder上面的sector正確
	assert(tsp_start <= tsp_end);
	assert(tsp_end - tsp_start == NC->CylinderSizeSector - 1);
	//
	N->WB2SMR_TASK_LEN = 0;
	N->SLBMPa.tsp_start = tsp_start;
	N->SLBMPa.tsp_end = tsp_end;
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		//if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		assert(tsp_start <= i && i <= tsp_end);
		dataBlock = SectorTableNo1 / NC->blockSizeSector;
		dataSector = SectorTableNo1 % NC->blockSizeSector;
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1 && (N->blocks[dataBlock].Cut == 1 && dataSector / NC->CylinderSizeSector == N->blocks[dataBlock].GuardCylIndex)){
			assert(N->DBalloc[dataBlock] == 1);//有配置的DB
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(N->blocks[dataBlock].sections[dataSector].sector == i);
			N->blocks[dataBlock].sections[dataSector].valid = 1;
			N->blocks[dataBlock].sections[dataSector].sector = SectorTableNo1;
			N->blocks[Victim].sections[i].valid = 0;
			//遇到Guard Cylinder Sector，等一下寫回LB
			N->WB2SMR_TASK[N->WB2SMR_TASK_LEN] = SectorTableNo1; N->WB2SMR_TASK_LEN++;
			assert(N->WB2SMR_TASK_LEN <= NC->CylinderSizeSector);
			//暫時先扣掉，等Write Back再把確實還留著的加回來
			N->l2pmapOvd.AA--;

			N->LB_GCyl_sector++;
		}
		else if (N->blocks[Victim].sections[i].valid == 1){
			//確認dataBlock正確
			assert(N->DBalloc[dataBlock] == 1);//有配置的DB
			assert(N->blocks[dataBlock].sections != NULL);
			assert(N->blocks[dataBlock].sections[dataSector].valid == 0);
			assert(N->blocks[dataBlock].sections[dataSector].sector == i);
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
			//開始merge DB
			N->SLBMPa.dataBlock = dataBlock;

			//FREE_CUTPM_DB(simPtr);
			//PM_DB(simPtr);
			CUTPM_DB(simPtr);

			//確保merge正確
			assert(0 <= N->SLBMPa.validIs0); assert(N->SLBMPa.validIs0 <= NC->blockSizeSector);
			assert(N->blocks[Victim].sections[i].valid == 0);
		}
		else {
			N->SLBMPa.NoMerge_Band++;
			assert(N->blocks[Victim].sections[i].valid == 0);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
		//計算得到多少LB Free Sector
		N->GET_FREE_LB_SECT++;
	}

	if (N->SLBMPa.Merge_Bands > 0){
		//fprintf(ftime, "Merge_Bands %lu, Dirty_Sectors %I64u, Cumu_Dirty_Sectors %I64u\n", Merge_Bands, Dirty_Sectors, Cumu_Dirty_Sectors);
		N->ValidIs0 += N->SLBMPa.Dirty_Sectors; N->Cumu_Dirty_Sectors += N->SLBMPa.Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	if (N->SLBMPa.NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);// NC->CylinderSizeTrack*NC->trackSizeSection
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeSector; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= NC->CylinderSizeSector;// (NC->CylinderSizeTrack*NC->trackSizeSection);

	//把遇到的DB Guard Cylinder移到SLB lastRW Cylinder上
	if (N->WB2SMR_TASK_LEN > 0){ GuardCyl_WB2SMR(simPtr); }
#ifdef Time_Overhead
	free(N->SLBMPa.merge_ram);
#endif
}

#endif

#ifdef Simulation_NSLB
/*使用*/
void FASTmergeRWNSLBpart3_3(sim *simPtr)//一次merge整個data band
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD flag = 0, NSLBdirtyTrack = 0;

	DWORD tmp = 0;
	//試驗
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
#endif
	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		flag = 0; NSLBdirtyTrack = 0;
		validIs1 = 0; validIs2 = 0;
#ifdef Time_Overhead
		N->Merge_Bandcount++;
		merge_ram_size = 0;
		for (k = 0; k < NC->blockSizeTrack; k++){//讀DB資料
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
			N->content_rotate++;
			N->MR_content_rotate++;///
		}
#endif
		for (k = 0; k < NC->blockSizeSection; k++){
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->Section; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			assert(N->blocks[dataBlock].sections[k].valid == 1 || N->blocks[dataBlock].sections[k].valid == 2);
			if (N->blocks[dataBlock].sections[k].valid == 1){
				assert(N->blocks[dataBlock].sections[k].sector == SectorTableNo);
				//統計資料更新
				NSLB->pageRead++; NSLB->pageWrite++; validIs1++; //read: DB hit, write: DB write
				NSLB->merge_count++;
			}
			else if (N->blocks[dataBlock].sections[k].valid == 2){
				validIs2++; flag = 1;
				NSLBdirtyTrack = N->blocks[dataBlock].sections[k].sector / NC->trackSizeSector;
				if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectionTable[NSLBdirtyTrack]++;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = N->blocks[dataBlock].sections[k].sector; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
#endif
				//寫回原DB
				N->blocks[dataBlock].sections[k].valid = 1;
				N->blocks[dataBlock].sections[k].sector = SectorTableNo;
				//統計資料更新
				NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
				NSLB->merge_count++;
			}

		}
		//#ifdef Time_Overhead
		//		assert(merge_ram_size <= NC->blockSizeSector);
		//		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		//		for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
		//		//Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 3);//電梯法回SLB讀資料
		//		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
		//			if (merge_ram[k - 1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){
		//				//seek
		//				//Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[k] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 3); 
		//			}
		//		}
		//		//DB seek
		//		for (k = 0; k < NC->blockSizeSector; k += NC->trackSizePage){
		//			SectorTableNo = dataBlock*NC->blockSizeSector + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
		//			//Calc_TimeOverhead(simPtr, SectorTableNo / NC->trackSizePage / NC->CylinderSizeTrack, 3);//寫在DB上
		//		}
		//		//seek
		//		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
		//#endif
#ifdef MergeAssoc_SLBpart
		//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
		N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / NSLB->NSLBsection_num);
		N->l2pmapOvd.AA -= merge_ram_size;
		//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
		//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
		//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
		//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

		fprintf(fp_NSLB, "%lu (%lu:%lu), %lu ( %lu)\n", dataBlock, validIs1, validIs2, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
		assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }

		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		Calc_RotateOverhead2(simPtr, 0, 3);//read SLB track
		N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
		N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
		N->Merge_LB_Trackcount++;
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			if (merge_ram[k - 1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 3); }
			N->Merge_LB_Trackcount++;//
		}
		N->merge_ram_writecount++;//
		merge_ram_writecount++;
		//combine DB和SLB資料
		for (k = 0; k < NC->blockSizeTrack; k++){//寫入DB
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
			N->content_rotate++;
			N->MW_content_rotate++;///

			N->Merge_Trackcount++;//
			Merge_Trackcount++;
		}
		merge_ram_size = 0;
		//seek
		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
#endif
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	NSLB->writedSectionNum -= validIs2;
	//#ifdef MergeAssoc_NSLBpart
	//	fprintf(fp_NSLB, "[%lu](%lu:%lu)%I64u ", dataBlock, validIs1, validIs2, tmp_time);
	//#endif
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
void DM_NSLBmerge(sim *simPtr)//一次merge整個data band
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD CurrentSectorNo = 0;
	DWORD mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD flag = 0, NSLBdirtyTrack = 0;

	DWORD tmp = 0;
	//試驗
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
#endif
	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		flag = 0; NSLBdirtyTrack = 0;
		validIs1 = 0; validIs2 = 0;
#ifdef Time_Overhead
		N->Merge_Bandcount++;
		merge_ram_size = 0;
		for (k = 0; k < NC->blockSizeTrack; k++){//讀DB資料
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
			N->content_rotate++;
			N->MR_content_rotate++;///
			N->Req_RunTime += 10;
			//統計資料更新
			NSLB->pageRead += NC->trackSizeSector;
		}
#endif
		for (k = 0; k < NC->blockSizeSection; k++){
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->Section; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			assert(N->blocks[dataBlock].sections[k].valid == 1 || N->blocks[dataBlock].sections[k].valid == 2);
			if (N->blocks[dataBlock].sections[k].valid == 1){
				assert(N->blocks[dataBlock].sections[k].sector == SectorTableNo);
				//統計資料更新
				//NSLB->pageRead++; NSLB->pageWrite++; 
				validIs1++; //read: DB hit, write: DB write
			}
			else if (N->blocks[dataBlock].sections[k].valid == 2){
				validIs2++; flag = 1;
				NSLBdirtyTrack = N->blocks[dataBlock].sections[k].sector / NC->trackSizeSector;
				if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectionTable[NSLBdirtyTrack]++;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = N->blocks[dataBlock].sections[k].sector; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
#endif
				//寫回原DB
				N->blocks[dataBlock].sections[k].valid = 1;
				N->blocks[dataBlock].sections[k].sector = SectorTableNo;
				//統計資料更新
				//NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
				N->NSLBwriteout_sector++;
			}
		}
#ifdef MergeAssoc_SLBpart
		//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
		N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / NSLB->NSLBsection_num);
		N->l2pmapOvd.AA -= merge_ram_size;
		//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
		//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
		//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
		//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

		fprintf(fp_NSLB, "%lu (%lu:%lu), %lu ( %lu)\n", dataBlock, validIs1, validIs2, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
		assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
		if (merge_ram_size > 0){ NSLB->merge_count++; }
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }

		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		Calc_RotateOverhead2(simPtr, 0, 3);//read SLB track
		N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
		N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
		N->Merge_LB_Trackcount++;
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			if (merge_ram[k - 1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 3); }
			N->Merge_LB_Trackcount++;//
		}
		N->merge_ram_writecount++;//
		merge_ram_writecount++;
		//xfer time
		N->Req_RunTime += ((double)(merge_ram_size) / NC->trackSizeSector * 10);
		//combine DB和SLB資料
		for (k = 0; k < NC->blockSizeTrack; k++){//寫入DB
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
			N->content_rotate++;
			N->MW_content_rotate++;///
			N->Req_RunTime += 10;

			N->Merge_Trackcount++;//
			Merge_Trackcount++;
			//
			NSLB->pageWrite += NC->trackSizeSector;
		}
		merge_ram_size = 0;
		//seek
		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
#endif
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	NSLB->writedSectionNum -= validIs2;
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
void DM_NSLBmerge2(sim *simPtr)//一次merge整個data band
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD CurrentSectorNo = 0;
	DWORD mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD flag = 0, NSLBdirtyTrack = 0;

	DWORD tmp = 0;
	//試驗
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
#endif
	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		flag = 0; NSLBdirtyTrack = 0;
		validIs1 = 0; validIs2 = 0;
#ifdef Time_Overhead
		N->Merge_Bandcount++;
		merge_ram_size = 0;
		for (k = 0; k < NC->blockSizeTrack; k++){//讀DB資料
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
			N->content_rotate++;
			N->MR_content_rotate++;///
			N->Req_RunTime += 10;
			//統計資料更新
			NSLB->pageRead += NC->trackSizeSector;
		}
#endif
		for (k = 0; k < NC->blockSizeSection; k++){
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->Section; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			assert(N->blocks[dataBlock].sections[k].valid == 1 || N->blocks[dataBlock].sections[k].valid == 2);
			if (N->blocks[dataBlock].sections[k].valid == 1){
				assert(N->blocks[dataBlock].sections[k].sector == SectorTableNo);
				//統計資料更新
				//NSLB->pageRead++; NSLB->pageWrite++; 
				validIs1++; //read: DB hit, write: DB write
			}
			else if (N->blocks[dataBlock].sections[k].valid == 2){
				validIs2++; flag = 1;
				NSLBdirtyTrack = N->blocks[dataBlock].sections[k].sector / NC->trackSizeSector;
				if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectionTable[NSLBdirtyTrack]++;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = N->blocks[dataBlock].sections[k].sector; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
#endif
				//寫回原DB
				N->blocks[dataBlock].sections[k].valid = 1;
				N->blocks[dataBlock].sections[k].sector = SectorTableNo;
				//統計資料更新
				//NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
				N->NSLBwriteout_sector++;
			}
		}
#ifdef NSLB_LocalUse
		N->DB2curNSLB_TK[dataBlock] = 2 * N->NSLB_tracks;
#endif
#ifdef MergeAssoc_SLBpart
		//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
		N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / NSLB->NSLBsection_num);
		N->l2pmapOvd.AA -= merge_ram_size;
		//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
		//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
		//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
		//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

		fprintf(fp_NSLB, "%lu (%lu:%lu), %lu ( %lu)\n", dataBlock, validIs1, validIs2, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
		assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
		if (merge_ram_size > 0){ NSLB->merge_count++; }
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }

		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		Calc_RotateOverhead2(simPtr, 0, 3);//read SLB track
		N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
		N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
		N->Merge_LB_Trackcount++;
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			if (merge_ram[k - 1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 3); }
			N->Merge_LB_Trackcount++;//
		}
		N->merge_ram_writecount++;//
		merge_ram_writecount++;
		//xfer time
		N->Req_RunTime += ((double)(merge_ram_size) / NC->trackSizeSector * 10);
		//combine DB和SLB資料
		for (k = 0; k < NC->blockSizeTrack; k++){//寫入DB
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
			N->content_rotate++;
			N->MW_content_rotate++;///
			N->Req_RunTime += 10;

			N->Merge_Trackcount++;//
			Merge_Trackcount++;
			//
			NSLB->pageWrite += NC->trackSizeSector;
		}
		merge_ram_size = 0;
		//seek
		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
#endif
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	NSLB->writedSectionNum -= validIs2;
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
void DM_NSLBmerge3_ExcluNSLBTK(sim *simPtr)//一次merge整個data band
{
	DWORD i, j, k, idx, x, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD CurrentSectorNo = 0;
	DWORD mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD flag = 0, NSLBdirtyTrack = 0;

	DWORD tmp = 0;
	//試驗
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
#endif
	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		flag = 0; NSLBdirtyTrack = 0;
		validIs1 = 0; validIs2 = 0;
#ifdef Time_Overhead
		N->Merge_Bandcount++;
		merge_ram_size = 0;
		for (k = 0; k < NC->blockSizeTrack; k++){//讀DB資料
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
			N->content_rotate++;
			N->MR_content_rotate++;///
			N->Req_RunTime += 10;
			//統計資料更新
			NSLB->pageRead += NC->trackSizeSector;
		}
#endif
		for (k = 0; k < NC->blockSizeSection; k++){
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->Section; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			assert(N->blocks[dataBlock].sections[k].valid == 1 || N->blocks[dataBlock].sections[k].valid == 2);
			if (N->blocks[dataBlock].sections[k].valid == 1){
				assert(N->blocks[dataBlock].sections[k].sector == SectorTableNo);
				//統計資料更新
				//NSLB->pageRead++; NSLB->pageWrite++; 
				validIs1++; //read: DB hit, write: DB write
			}
			else if (N->blocks[dataBlock].sections[k].valid == 2){
				validIs2++; flag = 1;
				NSLBdirtyTrack = N->blocks[dataBlock].sections[k].sector / NC->trackSizeSector;
				if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectionTable[NSLBdirtyTrack]++;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = N->blocks[dataBlock].sections[k].sector; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
#endif
				//寫回原DB
				N->blocks[dataBlock].sections[k].valid = 1;
				N->blocks[dataBlock].sections[k].sector = SectorTableNo;
				//統計資料更新
				//NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
				N->NSLBwriteout_sector++;
			}
		}
#ifdef NSLB_LocalUse
		N->DB2curNSLB_TK[dataBlock] = 2 * N->NSLB_tracks;
#endif
#ifdef MergeAssoc_SLBpart
		//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
		N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / NSLB->NSLBsection_num);
		N->l2pmapOvd.AA -= merge_ram_size;
		//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
		//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
		//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
		//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

		fprintf(fp_NSLB, "%lu (%lu:%lu), %lu ( %lu)\n", dataBlock, validIs1, validIs2, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
		assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
		if (merge_ram_size > 0){ NSLB->merge_count++; }
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }
#endif
#ifdef NSLB_LocalUse
		NSLBdirtyTrack = merge_ram[0] / NC->trackSizeSector;
		assert(0 <= NSLBdirtyTrack && NSLBdirtyTrack < N->NSLB_tracks);
		DWORD t_NSLBTK_Share = N->NSLBTK_Share[NSLBdirtyTrack];
		DWORD find = 0;
		assert(t_NSLBTK_Share >= 1);
		if (t_NSLBTK_Share == 1){
			assert(N->ExcluNSLBTK[NSLBdirtyTrack][0] == dataBlock);
			free(N->ExcluNSLBTK[NSLBdirtyTrack]);
			N->NSLBTK_Share[NSLBdirtyTrack] = 0;
			find = 1;
		}
		else if (t_NSLBTK_Share > 1){
			for (idx = 0; idx < t_NSLBTK_Share; idx++){
				if (N->ExcluNSLBTK[NSLBdirtyTrack][idx] == dataBlock){
					for (x = idx + 1; x < t_NSLBTK_Share; x++){
						N->ExcluNSLBTK[NSLBdirtyTrack][x - 1] = N->ExcluNSLBTK[NSLBdirtyTrack][x];
					}
					N->NSLBTK_Share[NSLBdirtyTrack]--;
					find = 1;
					break;
				}
			}
		}
		assert(find == 1);
		for (k = 1; k < merge_ram_size; k++){
			if (merge_ram[k - 1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){
				NSLBdirtyTrack = merge_ram[k] / NC->trackSizeSector;
				assert(0 <= NSLBdirtyTrack && NSLBdirtyTrack < N->NSLB_tracks);
				t_NSLBTK_Share = N->NSLBTK_Share[NSLBdirtyTrack];
				find = 0;
				assert(t_NSLBTK_Share >= 1);
				if (t_NSLBTK_Share == 1){
					assert(N->ExcluNSLBTK[NSLBdirtyTrack][0] == dataBlock);
					free(N->ExcluNSLBTK[NSLBdirtyTrack]);
					N->NSLBTK_Share[NSLBdirtyTrack] = 0;
					find = 1;
				}
				else if (t_NSLBTK_Share > 1){
					for (idx = 0; idx < t_NSLBTK_Share; idx++){
						if (N->ExcluNSLBTK[NSLBdirtyTrack][idx] == dataBlock){
							for (x = idx + 1; x < t_NSLBTK_Share; x++){
								N->ExcluNSLBTK[NSLBdirtyTrack][x - 1] = N->ExcluNSLBTK[NSLBdirtyTrack][x];
							}
							N->NSLBTK_Share[NSLBdirtyTrack]--;
							find = 1;
							break;
						}
					}
				}
				assert(find == 1);
			}
		}
#endif
#ifdef Time_Overhead
		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		Calc_RotateOverhead2(simPtr, 0, 3);//read SLB track
		N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
		N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
		N->Merge_LB_Trackcount++;
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			if (merge_ram[k - 1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 3); }
			N->Merge_LB_Trackcount++;//
		}
		N->merge_ram_writecount++;//
		merge_ram_writecount++;
		//xfer time
		N->Req_RunTime += ((double)(merge_ram_size) / NC->trackSizeSector * 10);
		//combine DB和SLB資料
		for (k = 0; k < NC->blockSizeTrack; k++){//寫入DB
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
			N->content_rotate++;
			N->MW_content_rotate++;///
			N->Req_RunTime += 10;

			N->Merge_Trackcount++;//
			Merge_Trackcount++;
			//
			NSLB->pageWrite += NC->trackSizeSector;
		}
		merge_ram_size = 0;
		//seek
		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
#endif
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	NSLB->writedSectionNum -= validIs2;
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
/*使用*/
//void FASTmergeRWNSLBpart23_3(sim *simPtr)//每次只merge一條data track 0.5MB，做128次
//{
//	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	DWORD	CurrentSectorNo = 0;
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD validIs1 = 0, validIs2 = 0;
//	DWORD small = 0, goal = 0; //small time: means the band have the earliest time
//	I64 tmp_time;
//	DWORD BPLRUstartBand = 0;
//	DWORD flag = 0, NSLBdirtyTrack = 0;
//	DWORD tmp = 0;
//
//	//試驗
//	DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
//	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
//	DWORD start_DBtrack = 0, end_DBtrack = 0;
//	DWORD SectorTableNo2 = 0, SectorTableNo3 = 0;
//	DWORD block_no = 0, section_no = 0;
//	//試驗
//	DWORD *SourceTracks = (DWORD*)calloc(NC->trackSizePage, sizeof(DWORD));
//	DWORD SourceTracks_count = 0, count = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
//	/*if (N->WriteBuff_Size > 0){
//	NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / NC->SubTrack); N->WriteBuff_Size = 0;
//	}*/
//#endif
//	NSLB->mergecnt++;
//	do
//	{
//#ifdef BPLRULlist_RunLen
//		N->BPLRUcurrent_len = 1;
//#endif
//		small = 0;
//		while (N->BPLRU[small] == 0){ small++; }
//		assert(small <= NC->LsizeBlock - 1);
//		if (small != NC->LsizeBlock - 1){
//			BPLRUstartBand = small + 1;
//			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
//			{
//				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
//#ifdef BPLRULlist_RunLen
//				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
//#endif
//			}
//		}
//#ifdef BPLRULlist_RunLen
//		N->BPLRUtotal_len += N->BPLRUcurrent_len;
//		N->BPLRUtotal_count++;
//		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
//		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
//#endif
//		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
//		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
//		flag = 0; NSLBdirtyTrack = 0;
//		validIs1 = 0; validIs2 = 0;
//		//#ifdef Time_Overhead
//		//		merge_ram_size = 0;
//		//#endif
//		//		for (k = 0; k < NC->blockSizeSector; k++){//
//		//			SectorTableNo = dataBlock*NC->blockSizeSector + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
//		//			assert(N->blocks[dataBlock].pages[k].valid == 1 || N->blocks[dataBlock].pages[k].valid == 2);
//		//			if (N->blocks[dataBlock].pages[k].valid == 1){
//		//				assert(N->blocks[dataBlock].pages[k].sector == SectorTableNo);
//		//				//統計資料更新
//		//				NSLB->pageRead++; NSLB->pageWrite++; validIs1++; //read: DB hit, write: DB write
//		//				NSLB->merge_count++;
//		//			}else if (N->blocks[dataBlock].pages[k].valid == 2){
//		//				validIs2++; flag = 1;
//		//				NSLBdirtyTrack = N->blocks[dataBlock].pages[k].sector / NC->trackSizePage;
//		//				if (N->NSLBremainSectorTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
//		//				N->NSLBremainSectorTable[NSLBdirtyTrack]++;
//		//#ifdef Time_Overhead
//		//				merge_ram[merge_ram_size] = NSLBdirtyTrack*2*NC->trackSizePage; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
//		//				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
//		//#endif
//		//				//寫回原DB
//		//				N->blocks[dataBlock].pages[k].valid = 1;
//		//				N->blocks[dataBlock].pages[k].sector = SectorTableNo;
//		//				//統計資料更新
//		//				NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
//		//				NSLB->merge_count++;
//		//			}
//		//		}
//		//		N->ValidIs2 += (I64)validIs2; assert(0 < validIs2 && validIs2 <= NC->blockSizeSector);
//		//#ifdef Time_Overhead
//		//		assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSector);
//		//		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//		//		for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
//		//		Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]) / NC->trackSizePage, 3);//電梯法回SLB讀資料   / NC->CylinderSizeTrack
//		//		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
//		//			if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){ //seek
//		//				Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]) / NC->trackSizePage, 3);//    / NC->CylinderSizeTrack
//		//			}
//		//		}
//		//		//DB seek
//		//		for (k = 0; k < NC->blockSizePage; k += NC->trackSizePage){
//		//			SectorTableNo = dataBlock*NC->blockSizePage + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
//		//			Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 3);//寫在DB上   / NC->CylinderSizeTrack
//		//		}
//		//		//N->preNSLB_WriteSector = 0;
//		//		//seek
//		//		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
//		//#endif
//
//#ifdef Time_Overhead
//		merge_ram_writecount = 0; Merge_Trackcount = 0;
//		N->Merge_Bandcount++;//
//		merge_ram_size = 0;
//		start_DBtrack = 0; end_DBtrack = 0;
//#endif
//		DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
//		for (j = 0; j < NC->blockSizeTrack; j++){
//			merge_content[j] = 0;
//			for (k = 0; k < NC->trackSizeSection; k++){
//				if (N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid == 2){ merge_content[j]++; }
//			}
//		}
//		for (j = 0; j < NC->blockSizeTrack; j++){
//			//#ifdef Time_Overhead
//			//			if (merge_ram_size + merge_content[j] > NC->trackSizePage){
//			//				//qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//			//				//for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
//			//				Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);//   / NC->CylinderSizeTrack
//			//				Calc_RotateOverhead2(simPtr, 0, 3);
//			//				for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
//			//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//			//					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);//   / NC->CylinderSizeTrack
//			//					if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){ Calc_RotateOverhead2(simPtr, 0, 3); }
//			//					N->Merge_LB_Trackcount++;//
//			//				}
//			//				end_DBtrack = j - 1; assert(start_DBtrack <= end_DBtrack);
//			//				for (k = start_DBtrack; k <= end_DBtrack; k++){//DB seek
//			//					SectorTableNo3 = dataBlock*NC->blockSizePage + k*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//			//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//			//					Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
//			//					Calc_RotateOverhead2(simPtr, 0, 2);
//			//					N->Merge_Trackcount++;//
//			//					Merge_Trackcount++;//
//			//				}
//			//				start_DBtrack = j;
//			//				assert(start_DBtrack > 0);
//			//				merge_ram_size = 0;
//			//				N->merge_ram_writecount++;//
//			//				merge_ram_writecount++;//
//			//			}
//			//#endif
//#ifdef Time_Overhead
//			merge_ram_size = 0;
//#endif
//			for (k = 0; k < NC->trackSizeSection; k++){
//				SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage + k*NC->Section; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//				block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
//				assert(block_no == dataBlock); assert(j*NC->trackSizeSection + k == section_no);
//				assert(N->blocks[dataBlock].sections[section_no].valid == 1 || N->blocks[dataBlock].sections[section_no].valid == 2);
//				if (N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid == 1){ //原本就在data block裡的valid page
//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
//					assert(N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].sector == SectorTableNo2);
//					//更新統計資料
//					NSLB->pageRead += NC->Section; NSLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
//					NSLB->merge_count++;
//				}
//				else if (N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid == 2){
//					validIs2++; flag = 1;
//					NSLBdirtyTrack = N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].sector / NC->trackSizePage;
//					if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
//					N->NSLBremainSectionTable[NSLBdirtyTrack]++;
//
//					//DWORD repeat = 0;//
//					//for (count = 0; count < SourceTracks_count; count++){//
//					//	if (SourceTracks[count] == page_no / NC->trackSizePage){ repeat = 1; break; }
//					//}
//					//if (repeat == 0){ //
//					//	assert(SourceTracks_count<NC->trackSizePage);
//					//	SourceTracks[SourceTracks_count] = page_no / NC->trackSizePage; SourceTracks_count++;
//					//}
//
//#ifdef Time_Overhead
//					merge_ram[merge_ram_size] = NSLBdirtyTrack * 2 * NC->trackSizePage; merge_ram_size++;
//					assert(merge_ram_size <= NC->trackSizePage);
//#endif
//					//寫回原DB
//					N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].valid = 1;
//					N->blocks[dataBlock].sections[j*NC->trackSizeSection + k].sector = SectorTableNo2;
//					//統計資料更新
//					NSLB->pageRead += 2 * NC->Section; NSLB->pageWrite += NC->Section; //read: DB miss + NSLB hit, write: DB write
//					NSLB->merge_count++;
//				}
//			}
//#ifdef Time_Overhead
//			if (merge_ram_size > 0){
//				assert(merge_ram_size <= NC->trackSizePage);
//				qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//				if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }
//
//				//Seek_Seagate_C15k5(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
//				test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
//				N->content_rotate += ((double)merge_ram_size*NC->Section*N->Disk.DiskSizeMB / ((double)NC->LsizeMB * 2880));//寫多少sector轉多少sector
//				for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//					//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
//					test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
//					if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){
//						Calc_RotateOverhead2(simPtr, 0, 3);
//					}
//					N->Merge_LB_Trackcount++;//
//				}
//				N->merge_ram_writecount++;//
//				merge_ram_writecount++;
//			}
//			SectorTableNo3 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//			//Seek_Seagate_C15k5(simPtr, SectorTableNo3, 2);
//			test(simPtr, SectorTableNo3, 2);
//			N->content_rotate += 2;//寫多少sector轉多少sector
//			Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
//			N->Merge_Trackcount++;//
//			Merge_Trackcount++;
//			merge_ram_size = 0;
//#endif
//		}
//		//#ifdef Time_Overhead
//		//		if (merge_ram_size > 0){
//		//			//qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//		//			//if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }
//		//			Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);//   / NC->CylinderSizeTrack
//		//			Calc_RotateOverhead2(simPtr, 0, 3);
//		//			for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
//		//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//		//				Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);//   / NC->CylinderSizeTrack
//		//				if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){ Calc_RotateOverhead2(simPtr, 0, 3); }
//		//				N->Merge_LB_Trackcount++;//
//		//			}
//		//			assert(start_DBtrack <= NC->blockSizeTrack - 1);
//		//			for (k = start_DBtrack; k < NC->blockSizeTrack; k++){//DB seek
//		//				SectorTableNo3 = dataBlock*NC->blockSizePage + k*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
//		//				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//		//				Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
//		//				Calc_RotateOverhead2(simPtr, 0, 2);
//		//				N->Merge_Trackcount++;//
//		//				Merge_Trackcount++;
//		//			}
//		//			merge_ram_size = 0;
//		//			N->merge_ram_writecount++;//
//		//			merge_ram_writecount++;
//		//		}
//		//#endif
//
//		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
//		assert(validIs2 > 0);
//
//		N->SourceTracks_count += (I64)SourceTracks_count;
//		assert(Merge_Trackcount == NC->blockSizeTrack);
//#ifdef MergeAssoc_SLBpart
//		fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs1, validIs2, merge_ram_writecount, SourceTracks_count);
//#endif
//	} while (validIs2 == 0);
//	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
//	//NSLB統計資料
//	//NSLB->writedPageNum -= validIs2;
//	NSLB->writedSectionNum -= validIs2;
//	free(SourceTracks);
//	free(merge_content);
//#ifdef Time_Overhead
//	free(merge_ram);
//#endif
//}
/*SE*/
//void FASTmergeRWNSLBpart23_3_SE(sim *simPtr)
//{
//	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	DWORD	CurrentSectorNo = 0;
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD validIs1 = 0, validIs2 = 0;
//	DWORD small = 0, goal = 0; //small time: means the band have the earliest time
//	I64 tmp_time;
//	DWORD BPLRUstartBand = 0;
//	DWORD flag = 0, NSLBdirtyTrack = 0;
//	DWORD tmp = 0;
//
//	DWORD StartTrack = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
//#endif
//	NSLB->mergecnt++;
//	do
//	{
//#ifdef BPLRULlist_RunLen
//		N->BPLRUcurrent_len = 1;
//#endif
//		small = 0;
//		while (N->BPLRU[small] == 0){ small++; }
//		assert(small <= NC->LsizeBlock - 1);
//		if (small != NC->LsizeBlock - 1){
//			BPLRUstartBand = small + 1;
//			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
//			{
//				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
//#ifdef BPLRULlist_RunLen
//				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
//#endif
//			}
//		}
//#ifdef BPLRULlist_RunLen
//		N->BPLRUtotal_len += N->BPLRUcurrent_len;
//		N->BPLRUtotal_count++;
//		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
//		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
//#endif
//		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
//		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
//		flag = 0; NSLBdirtyTrack = 0;
//		validIs1 = 0; validIs2 = 0;
//#ifdef Time_Overhead
//		merge_ram_size = 0;
//#endif
//		k = 0;
//		while (N->blocks[dataBlock].pages[k].valid == 1){ k++; }
//		StartTrack = k / NC->trackSizePage; assert(0 <= k && k < NC->blockSizeSector);
//		for (; k < NC->blockSizeSector; k++){//k = 0
//			SectorTableNo = dataBlock*NC->blockSizeSector + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
//			assert(N->blocks[dataBlock].pages[k].valid == 1 || N->blocks[dataBlock].pages[k].valid == 2);
//			if (N->blocks[dataBlock].pages[k].valid == 1){
//				assert(N->blocks[dataBlock].pages[k].sector == SectorTableNo);
//				//統計資料更新
//				NSLB->pageRead++; NSLB->pageWrite++; validIs1++; //read: DB hit, write: DB write
//				NSLB->merge_count++;
//			}
//			else if (N->blocks[dataBlock].pages[k].valid == 2){
//				validIs2++; flag = 1;
//				NSLBdirtyTrack = N->blocks[dataBlock].pages[k].sector / NC->trackSizePage;
//				if (N->NSLBremainSectorTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
//				N->NSLBremainSectorTable[NSLBdirtyTrack]++;
//#ifdef Time_Overhead
//				merge_ram[merge_ram_size] = NSLBdirtyTrack*NC->trackSizePage; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
//				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
//#endif
//				//寫回原DB
//				N->blocks[dataBlock].pages[k].valid = 1;
//				N->blocks[dataBlock].pages[k].sector = SectorTableNo;
//				//統計資料更新
//				NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
//				NSLB->merge_count++;
//			}
//
//		}
//		N->ValidIs2 += (I64)validIs2;
//		//if ((I64)validIs2 > N->ValidIs2_max){ N->ValidIs2_max = (I64)validIs2; }
////#ifdef Time_Overhead
////		assert(merge_ram_size <= NC->blockSizeSector);
////		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
////		for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
////		Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 3);//電梯法回SLB讀資料
////		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
////			if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){
////				//seek
////				Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[k] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 3);
////			}
////		}
////		//DB seek
////		for (k = StartTrack*NC->trackSizePage; k < NC->blockSizePage; k += NC->trackSizePage){
////			SectorTableNo = dataBlock*NC->blockSizePage + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
////			Calc_TimeOverhead(simPtr, SectorTableNo / NC->trackSizePage / NC->CylinderSizeTrack, 3);//寫在DB上
////		}
////		//seek
////		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
////#endif
//
//#ifdef Time_Overhead
//		assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSector);
//		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//		for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
//		Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]) / NC->trackSizePage, 3);//電梯法回SLB讀資料   / NC->CylinderSizeTrack
//		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
//			if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){ //seek
//				Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]) / NC->trackSizePage, 3);//    / NC->CylinderSizeTrack
//			}
//		}
//		//DB seek
//		for (k = StartTrack*NC->trackSizePage; k < NC->blockSizePage; k += NC->trackSizePage){
//			SectorTableNo = dataBlock*NC->blockSizePage + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
//			Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage / NC->CylinderSizeTrack, 3);//寫在DB上
//		}
//		//seek
//		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
//#endif
//
//		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
//		assert(validIs2 > 0);
//	} while (validIs2 == 0);
//	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
//	//NSLB統計資料
//	NSLB->writedPageNum -= validIs2;
//#ifdef MergeAssoc_NSLBpart
//	fprintf(fp_NSLB, "[%lu](%lu:%lu)%I64u ", dataBlock, validIs1, validIs2, tmp_time);
//#endif
//#ifdef Time_Overhead
//	free(merge_ram);
//#endif
//}

//------
void DM_NSLBmerge_NoDB(sim *simPtr)//一次merge整個data band
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD CurrentSectorNo = 0;
	DWORD mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 SectorTableNo;

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD flag = 0, NSLBdirtyTrack = 0;
	DWORD tmp = 0;
	//試驗
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
#endif
	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		assert(N->DBalloc[dataBlock] == 1);
		flag = 0; NSLBdirtyTrack = 0;
		validIs1 = 0; validIs2 = 0;
#ifdef Time_Overhead
		N->Merge_Bandcount++;
		merge_ram_size = 0;
		for (k = 0; k < NC->blockSizeTrack; k++){//讀DB資料
			SectorTableNo = (I64)dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
			N->content_rotate += NC->trackSizeSector;
			N->MR_content_rotate += NC->trackSizeSector;
			N->Req_RunTime += 10;
			//統計資料更新
			NSLB->pageRead += NC->trackSizeSector;
		}
#endif
		for (k = 0; k < NC->blockSizeSection; k++){
			SectorTableNo = (I64)dataBlock*NC->blockSizeSector + k*NC->Section; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			assert(N->blocks[dataBlock].sections[k].valid == 1 || N->blocks[dataBlock].sections[k].valid == 2);
			if (N->blocks[dataBlock].sections[k].valid == 1){
				assert(N->blocks[dataBlock].sections[k].sector == SectorTableNo);
				//統計資料更新
				//NSLB->pageRead++; NSLB->pageWrite++; 
				validIs1++; //read: DB hit, write: DB write
			}
			else if (N->blocks[dataBlock].sections[k].valid == 2){
				validIs2++; flag = 1;
				NSLBdirtyTrack = N->blocks[dataBlock].sections[k].sector / NC->trackSizeSector;
				if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectionTable[NSLBdirtyTrack]++;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = N->blocks[dataBlock].sections[k].sector; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
#endif
				//寫回原DB
				N->blocks[dataBlock].sections[k].valid = 1;
				N->blocks[dataBlock].sections[k].sector = SectorTableNo;
				//統計資料更新
				//NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
				N->NSLBwriteout_sector++;
			}
		}
		//歸還DB的sectors記憶體
		assert(N->DBalloc[dataBlock] == 1);
		free(N->blocks[dataBlock].sections);
		N->DBalloc[dataBlock] = 0;
		N->blocks[dataBlock].sections = NULL;
#ifdef MergeAssoc_SLBpart
		//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
		if (N->l2pmapOvd.AA > N->l2pmapOvd.max_AA){ N->l2pmapOvd.max_AA = N->l2pmapOvd.AA; }
		N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / NSLB->NSLBsection_num);
		N->l2pmapOvd.AA -= merge_ram_size;
		//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
		//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
		//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
		//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
		fprintf(fp_NSLB, "%lu (%lu:%lu), %lu ( %lu)\n", dataBlock, validIs1, validIs2, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
		assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
		if (merge_ram_size > 0){ NSLB->merge_count++; }
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }

		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		Calc_RotateOverhead2(simPtr, 0, 3);//read SLB track
		//(unsigned long long)ceil((double)merge_ram_size / NC->trackSizeSection);
		N->content_rotate += merge_ram_size;
		N->MR_content_rotate += merge_ram_size;
		N->Merge_LB_Trackcount++;
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			if (merge_ram[k - 1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 3); }
			N->Merge_LB_Trackcount++;//
		}
		N->merge_ram_writecount++;//
		merge_ram_writecount++;
		//xfer time
		N->Req_RunTime += (unsigned long long)round((double)(merge_ram_size) / NC->trackSizeSector * 10);
		//combine DB和SLB資料
		for (k = 0; k < NC->blockSizeTrack; k++){//寫入DB
			SectorTableNo = (I64)dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
			test(simPtr, SectorTableNo, 2);
			Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
			N->content_rotate += NC->trackSizeSector;
			N->MW_content_rotate += NC->trackSizeSector;
			N->Req_RunTime += 10;

			N->Merge_Trackcount++;//
			Merge_Trackcount++;
			//
			NSLB->pageWrite += NC->trackSizeSector;
		}
		merge_ram_size = 0;
		//seek
		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
#endif
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0 < validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	NSLB->writedSectionNum -= validIs2;
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
#endif

#ifdef Simulation_HLB
void DM_SLBmerge(sim *simPtr) {//FASTmergeRWSLBpart2_Sr, 一次merge整個data band
	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0, PageNo = 0;
	DWORD	j = 0, k = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD validIs0 = 0, validIs1 = 0;
	DWORD block_no = 0, section_no = 0;
	//試驗
	//DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	//試驗
	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD SourceTracks_count = 0, count = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD start_DBtrack = 0, end_DBtrack = 0;

	DWORD NoMerge_Band = 0;
	//初始化 存活LB section
	//N->l2pmapOvd.round_LB_Live_Section = SLB->SLBsection_num;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif

	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;

	//			for (j = 0; j < NC->blockSizePage; j++){
	//				SectorTableNo2 = dataBlock*NC->blockSizePage + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
	//				block_no = getblock(simPtr, SectorTableNo2); page_no = getpage(simPtr, SectorTableNo2);
	//				if (N->blocks[dataBlock].pages[j].valid == 1){ //原本就在data block裡的valid page
	//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizeSector);
	//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 % NC->blockSizeSector == page_no);
	//					assert(N->blocks[dataBlock].pages[j].sector == SectorTableNo2);
	//					//更新統計資料
	//					SLB->pageRead++; SLB->pageWrite++; validIs1++;//DB: read hit, DB: write
	//					SLB->merge_count++;
	//				}
	//				else if (N->blocks[dataBlock].pages[j].valid == 0){ //invalid在LB的該page
	//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= page_no && page_no <= SLB->partial_page);
	//					assert(N->blocks[block_no].pages[page_no].valid == 1);
	//					N->blocks[block_no].pages[page_no].valid = 0;//invalid old
	//					//更新統計資料
	//					SLB->pageRead += 2; SLB->pageWrite++; validIs0++;//DB: read miss, LB: read hit, DB: write
	//					SLB->merge_count++;
	//					//merge_content[j / NC->trackSizePage]++;
	//#ifdef Time_Overhead
	//					merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->trackSizePage);
	//					if (merge_ram_size == NC->trackSizePage){
	//						//qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
	//						//for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
	//						Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 1);//   / NC->CylinderSizeTrack
	//						for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
	//							if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){//seek
	//								Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 1);//   / NC->CylinderSizeTrack
	//							}
	//						}
	//						end_DBtrack = j / NC->trackSizePage; assert(start_DBtrack <= end_DBtrack);
	//						for (k = start_DBtrack; k <= end_DBtrack; k++){//DB seek
	//							SectorTableNo3 = dataBlock*NC->blockSizePage + k*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
	//							Calc_TimeOverhead2(simPtr, SectorTableNo3, 1);//寫在DB上   / NC->CylinderSizeTrack
	//							N->Merge_Trackcount++;//
	//							Merge_Trackcount++;
	//						}
	//						if ((j + 1) % NC->trackSizePage == 0){ start_DBtrack = end_DBtrack + 1; }
	//						else{ start_DBtrack = end_DBtrack; }
	//						assert(start_DBtrack > 0);
	//						merge_ram_size = 0;
	//						N->merge_ram_writecount++;//
	//						merge_ram_writecount++;
	//					}
	//#endif
	//					setblock(simPtr, SectorTableNo2, dataBlock);
	//					setpage(simPtr, SectorTableNo2, j);
	//					N->blocks[dataBlock].pages[j].sector = SectorTableNo2;
	//					N->blocks[dataBlock].pages[j].valid = 1;
	//				}
	//				else{ printf("SLB merge error\n"); system("pause"); }
	//			}

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);

	int num = 0; assert(tsp_start <= tsp_end);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

			SourceTracks_count = 0;//
#ifdef Time_Overhead
			N->Merge_Bandcount++;//
			merge_ram_writecount = 0; Merge_Trackcount = 0;
			merge_ram_size = 0;
			start_DBtrack = 0; end_DBtrack = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
			DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = 0; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, (I64)SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate++;
				N->MR_content_rotate++;///
			}
			//assert(N->Merge_R_SeekCount - Merge_R_SeekCount == NC->blockSizeCylinder);
#endif
			for (j = 0; j < NC->blockSizeSection; j++){
				SectorTableNo2 = dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
					assert(j == section_no);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					//更新統計資料
					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
					assert(N->blocks[block_no].sections[section_no].valid == 1);
					N->blocks[block_no].sections[section_no].valid = 0;//invalid old
					//更新統計資料
					SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
					DWORD repeat = 0;//
					for (count = 0; count < SourceTracks_count; count++){//
						if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
					}
					if (repeat == 0){ //
						assert(SourceTracks_count<NC->blockSizeSection);
						SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
					}
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					//assert(N->section_cumu[SectorTableNo2 / NC->Section]>0);
					//assert(N->section_cumu[SectorTableNo2 / NC->Section] == N->SLB_section_cumu[SectorTableNo2 / NC->Section] + N->DB_section_cumu[SectorTableNo2 / NC->Section]);
					//Cumu_Dirty_Sectors += N->section_cumu[SectorTableNo2 / NC->Section];
					//N->section_cumu[SectorTableNo2 / NC->Section] = 0;

					setblock(simPtr, SectorTableNo2, dataBlock); assert(j < NC->blockSizeSection);
					setsection(simPtr, SectorTableNo2, j);
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//統計這個band在SLB活著的section
				}
				//else{ printf("SLB merge error\n"); system("pause"); }
			}
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

			/*FILE *t2 = fopen("DBdirtyInSLB.txt", "a");
			fprintf(t2, "dataBlock %5lu, N->l2pmapOvd.AA %lu\n", dataBlock, N->l2pmapOvd.AA);
			fclose(t2);*/
#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

			fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs0, validIs1, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
			N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;//
			merge_ram_writecount++;
			//
			N->S_MRS += (I64)merge_ram_size;

			//transfer time
			N->Req_RunTime += (2 * NC->blockSizeTrack * 10);
			N->Req_RunTime += ((double)merge_ram_size / NC->trackSizeSector * 10);


			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = 0; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				//fprintf(t, "mw db %lu\n", SectorTableNo3);
				test(simPtr, (I64)SectorTableNo3, 2);

				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate++;
				N->MW_content_rotate++;///

				N->Merge_Trackcount++;//
				Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);

#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//N->SourceTracks_count += (I64)SourceTracks_count;
			assert(Merge_Trackcount == NC->blockSizeTrack);
		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		fprintf(ftime, "Merge_Bands %lu, Dirty_Sectors %I64u, Cumu_Dirty_Sectors %I64u\n", Merge_Bands, Dirty_Sectors, Cumu_Dirty_Sectors);
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
	free(SourceTracks);
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
void DM_NSLBmerge(sim *simPtr)//一次merge整個data band
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD CurrentSectorNo = 0;
	DWORD mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD flag = 0, NSLBdirtyTrack = 0;

	DWORD tmp = 0;
	//試驗
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
#endif
	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		flag = 0; NSLBdirtyTrack = 0;
		validIs1 = 0; validIs2 = 0;

		N->N2S_LBsector_len = 0;
		for (k = 0; k < NC->blockSizeSection; k++){
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->Section; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			assert(0<=N->blocks[dataBlock].sections[k].valid && N->blocks[dataBlock].sections[k].valid <= 2);
			if (N->blocks[dataBlock].sections[k].valid == 1){
				assert(N->blocks[dataBlock].sections[k].sector == SectorTableNo);
				//統計資料更新
				//NSLB->pageRead++; NSLB->pageWrite++; 
				validIs1++; //read: DB hit, write: DB write
			}
			else if (N->blocks[dataBlock].sections[k].valid == 2){
				validIs2++; flag = 1;
				NSLBdirtyTrack = N->blocks[dataBlock].sections[k].sector / NC->trackSizeSector;
				if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectionTable[NSLBdirtyTrack]++;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = N->blocks[dataBlock].sections[k].sector; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
#endif
				N->N2S_LBsector[N->N2S_LBsector_len] = SectorTableNo; N->N2S_LBsector_len++; assert(N->N2S_LBsector_len <= NC->blockSizeSector);
				//寫回原DB
				N->blocks[dataBlock].sections[k].valid = 1;
				N->blocks[dataBlock].sections[k].sector = SectorTableNo;
				//統計資料更新
				//NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
				N->NSLBwriteout_sector++;
			}
		}
#ifdef MergeAssoc_SLBpart
		//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
		N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / NSLB->NSLBsection_num);
		N->l2pmapOvd.AA -= merge_ram_size;
		//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
		//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
		//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
		//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

		fprintf(fp_NSLB, "%lu (%lu:%lu), %lu ( %lu)\n", dataBlock, validIs1, validIs2, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
		assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
		if (merge_ram_size > 0){ NSLB->merge_count++; }
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }

		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
		Calc_RotateOverhead2(simPtr, 0, 3);//read SLB track
		N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
		N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
		N->Merge_LB_Trackcount++;
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
			if (merge_ram[k - 1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 3); }
			N->Merge_LB_Trackcount++;//
		}
		N->merge_ram_writecount++;//
		merge_ram_writecount++;
		merge_ram_size = 0;
		//seek
		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
#endif
		if (N->N2S_LBsector_len <= NC->blockSizeSector / 2){
			for (k = 0; k<N->N2S_LBsector_len; k++){ assert(N->N2S_LBsector[i]<NC->LsizeSector); }
			DM_N2SLBw(simPtr);
		}
		else{
#ifdef Time_Overhead
			N->Merge_Bandcount++;
			merge_ram_size = 0;
			for (k = 0; k < NC->blockSizeTrack; k++){//讀DB資料
				SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, SectorTableNo, 2);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate++;
				N->MR_content_rotate++;///
				N->Req_RunTime += 10;
				//統計資料更新
				NSLB->pageRead += NC->trackSizeSector;
			}
#endif
#ifdef Time_Overhead
			//xfer time
			N->Req_RunTime += ((double)(merge_ram_size) / NC->trackSizeSector * 10);
			//combine DB和SLB資料
			for (k = 0; k < NC->blockSizeTrack; k++){//寫入DB
				SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, SectorTableNo, 2);
				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate++;
				N->MW_content_rotate++;///
				N->Req_RunTime += 10;

				N->Merge_Trackcount++;//
				Merge_Trackcount++;
				//
				NSLB->pageWrite += NC->trackSizeSector;
			}
#endif
		}

		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	NSLB->writedSectionNum -= validIs2;
#ifdef Time_Overhead
	free(merge_ram);
#endif
}

void DM_SLBmerge2(sim *simPtr) {//FASTmergeRWSLBpart2_Sr, 一次merge整個data band
	DWORD	RWLBA, dataBlock, newBlock, BlockNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo1 = 0, SectorTableNo2 = 0, SectorTableNo3 = 0, PageNo = 0;
	DWORD	j = 0, k = 0;
	DWORD	i = 0, tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD validIs0 = 0, validIs1 = 0, validIs2 = 0;
	DWORD block_no = 0, section_no = 0;
	//試驗
	//DWORD *merge_content = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;
	//試驗
	DWORD *SourceTracks = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD SourceTracks_count = 0, count = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD start_DBtrack = 0, end_DBtrack = 0;

	DWORD NoMerge_Band = 0;
	//初始化 存活LB section
	//N->l2pmapOvd.round_LB_Live_Section = SLB->SLBsection_num;
	DWORD Merge_R_SeekCount = 0, Merge_W_SeekCount = 0;
#endif

	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands = 0;
	I64 Dirty_Sectors = 0, Cumu_Dirty_Sectors = 0;

	//			for (j = 0; j < NC->blockSizePage; j++){
	//				SectorTableNo2 = dataBlock*NC->blockSizePage + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
	//				block_no = getblock(simPtr, SectorTableNo2); page_no = getpage(simPtr, SectorTableNo2);
	//				if (N->blocks[dataBlock].pages[j].valid == 1){ //原本就在data block裡的valid page
	//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizeSector);
	//					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 % NC->blockSizeSector == page_no);
	//					assert(N->blocks[dataBlock].pages[j].sector == SectorTableNo2);
	//					//更新統計資料
	//					SLB->pageRead++; SLB->pageWrite++; validIs1++;//DB: read hit, DB: write
	//					SLB->merge_count++;
	//				}
	//				else if (N->blocks[dataBlock].pages[j].valid == 0){ //invalid在LB的該page
	//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= page_no && page_no <= SLB->partial_page);
	//					assert(N->blocks[block_no].pages[page_no].valid == 1);
	//					N->blocks[block_no].pages[page_no].valid = 0;//invalid old
	//					//更新統計資料
	//					SLB->pageRead += 2; SLB->pageWrite++; validIs0++;//DB: read miss, LB: read hit, DB: write
	//					SLB->merge_count++;
	//					//merge_content[j / NC->trackSizePage]++;
	//#ifdef Time_Overhead
	//					merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->trackSizePage);
	//					if (merge_ram_size == NC->trackSizePage){
	//						//qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
	//						//for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
	//						Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 1);//   / NC->CylinderSizeTrack
	//						for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
	//							if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){//seek
	//								Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 1);//   / NC->CylinderSizeTrack
	//							}
	//						}
	//						end_DBtrack = j / NC->trackSizePage; assert(start_DBtrack <= end_DBtrack);
	//						for (k = start_DBtrack; k <= end_DBtrack; k++){//DB seek
	//							SectorTableNo3 = dataBlock*NC->blockSizePage + k*NC->trackSizePage; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
	//							Calc_TimeOverhead2(simPtr, SectorTableNo3, 1);//寫在DB上   / NC->CylinderSizeTrack
	//							N->Merge_Trackcount++;//
	//							Merge_Trackcount++;
	//						}
	//						if ((j + 1) % NC->trackSizePage == 0){ start_DBtrack = end_DBtrack + 1; }
	//						else{ start_DBtrack = end_DBtrack; }
	//						assert(start_DBtrack > 0);
	//						merge_ram_size = 0;
	//						N->merge_ram_writecount++;//
	//						merge_ram_writecount++;
	//					}
	//#endif
	//					setblock(simPtr, SectorTableNo2, dataBlock);
	//					setpage(simPtr, SectorTableNo2, j);
	//					N->blocks[dataBlock].pages[j].sector = SectorTableNo2;
	//					N->blocks[dataBlock].pages[j].valid = 1;
	//				}
	//				else{ printf("SLB merge error\n"); system("pause"); }
	//			}

	SLB->mergecnt++;
	Victim = NC->PsizeBlock - 1;
	tsp_start = SLB->firstRWsection;// (N->lastRWpage + 1) % (N->partial_page + 1);
	tsp_end = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection - 1) % (SLB->Partial_Section + 1);

	int num = 0; assert(tsp_start <= tsp_end);
	for (i = tsp_start; i <= tsp_end; i++){	//SLB的victim track做merge
		SectorTableNo1 = N->blocks[Victim].sections[i].sector; assert(0 <= SectorTableNo1 && SectorTableNo1 < NC->LsizePage);
		if (!(tsp_start <= i && i <= tsp_end)){ printf("%lu %lu %lu\n", tsp_start, i, tsp_end); system("pause"); }
		//有一張P2L的SLB section mapping table，協助搜尋SLB裡存放的victim band的dirty section
		if (N->blocks[Victim].sections[i].valid == 1){ // valid page in victim log block need merge with data block
			assert((N->blocks[Victim].sections[i].sector) < NC->LsizePage);
			dataBlock = N->blocks[Victim].sections[i].sector / NC->blockSizeSector;
			assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);

			SourceTracks_count = 0;//
#ifdef Time_Overhead
			N->Merge_Bandcount1++;//
			merge_ram_writecount = 0; Merge_Trackcount = 0;
			merge_ram_size = 0;
			start_DBtrack = 0; end_DBtrack = 0;
#endif
			//紀錄每次有效merge帶走多少band
			Merge_Bands++;
			validIs0 = 0; validIs1 = 0; validPages = 0; invalidPages = 0;
			validIs2 = 0;
			DWORD start = 0, end = 0; //on the merge tergat data band, 0~blocksize找，sub-band的起點和終點
#ifdef Time_Overhead
			Merge_R_SeekCount = N->Merge_R_SeekCount;
			for (j = 0; j < NC->blockSizeTrack; j++){//讀DB資料
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, (I64)SectorTableNo3, 3);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate++;
				N->MR_content_rotate++;///
			}
			//assert(N->Merge_R_SeekCount - Merge_R_SeekCount == NC->blockSizeCylinder);
#endif
			for (j = 0; j < NC->blockSizeSection; j++){
				SectorTableNo2 = dataBlock*NC->blockSizeSector + j; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
				block_no = getblock(simPtr, SectorTableNo2); section_no = getsection(simPtr, SectorTableNo2);
				if (N->blocks[dataBlock].sections[j].valid == 1){ //原本就在data block裡的valid page
					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
					assert(SectorTableNo2 / NC->blockSizeSector == block_no); assert(SectorTableNo2 / NC->Section % NC->blockSizeSection == section_no);
					assert(j == section_no);
					assert(N->blocks[dataBlock].sections[j].sector == SectorTableNo2);
					//更新統計資料
					SLB->pageRead += NC->Section; SLB->pageWrite += NC->Section; validIs1 += NC->Section;//DB: read hit, DB: write
					SLB->merge_count += NC->Section;
				}
				else if (N->blocks[dataBlock].sections[j].valid == 0){ //invalid在LB的該page
					assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
					assert(N->blocks[block_no].sections[section_no].valid == 1);
					N->blocks[block_no].sections[section_no].valid = 0;//invalid old
					//更新統計資料
					SLB->pageRead += (2 * NC->Section); SLB->pageWrite += NC->Section; validIs0 += NC->Section;//DB: read miss, LB: read hit, DB: write
					SLB->merge_count += NC->Section;
					DWORD repeat = 0;//
					for (count = 0; count < SourceTracks_count; count++){//
						if (SourceTracks[count] == section_no / NC->trackSizeSection){ repeat = 1; break; }
					}
					if (repeat == 0){ //
						assert(SourceTracks_count<NC->blockSizeSection);
						SourceTracks[SourceTracks_count] = section_no / NC->trackSizeSection; SourceTracks_count++;
					}
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
#endif
					//assert(N->section_cumu[SectorTableNo2 / NC->Section]>0);
					//assert(N->section_cumu[SectorTableNo2 / NC->Section] == N->SLB_section_cumu[SectorTableNo2 / NC->Section] + N->DB_section_cumu[SectorTableNo2 / NC->Section]);
					//Cumu_Dirty_Sectors += N->section_cumu[SectorTableNo2 / NC->Section];
					//N->section_cumu[SectorTableNo2 / NC->Section] = 0;

					setblock(simPtr, SectorTableNo2, dataBlock); assert(j < NC->blockSizeSection);
					setsection(simPtr, SectorTableNo2, j);
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					N->blocks[dataBlock].sections[j].valid = 1;
					//統計這個band在SLB活著的section
				}else if (N->blocks[dataBlock].sections[j].valid == 2){
					validIs2++;
					DWORD NSLBdirtyTrack = N->blocks[dataBlock].sections[j].sector / NC->trackSizeSector;
					if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
					N->NSLBremainSectionTable[NSLBdirtyTrack]++;
					NSLB->writedSectionNum--;
#ifdef Time_Overhead
					merge_ram[merge_ram_size] = SLB->Partial_Section + 1 + N->blocks[dataBlock].sections[k].sector; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
					assert(merge_ram_size <= NC->blockSizeSector); //assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
#endif
					//寫回原DB
					N->blocks[dataBlock].sections[j].valid = 1;
					N->blocks[dataBlock].sections[j].sector = SectorTableNo2;
					//統計資料更新
					NSLB->pageRead++; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
					N->NSLBwriteout_sector++;
				}
				//else{ printf("SLB merge error\n"); system("pause"); }
			}
			//紀錄Effect Merge有多少dirty sectors,多少cumu dirty sectors
			assert(merge_ram_size == validIs0 + validIs2);//當1 section = 1 sector成立
			Dirty_Sectors += (I64)merge_ram_size;
			N->req_w_cumu += Cumu_Dirty_Sectors;

#ifdef MergeAssoc_SLBpart
			//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
			N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			N->l2pmapOvd.AA -= merge_ram_size;
			//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
			//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
			//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
			//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);
			fprintf(fp_SLB, "%lu (%lu:%lu), %lu, %lu\n", dataBlock, validIs0, validIs1, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
#ifdef Time_Overhead
			assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSection);
			qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
			if (merge_ram_size > 1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
			test(simPtr, (I64)NC->LsizeSector + merge_ram[0], 4);//3
			Calc_RotateOverhead2(simPtr, 0, 4);//read SLB track 3
			N->S_CT++;
			N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
			N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
			N->Merge_LB_Trackcount++;
			for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
				test(simPtr, (I64)NC->LsizeSector + merge_ram[j], 4);//3
				if (merge_ram[j - 1] / NC->trackSizeSector != merge_ram[j] / NC->trackSizeSector){
					Calc_RotateOverhead2(simPtr, 0, 4);
					N->S_CT++;
				}//3
				N->Merge_LB_Trackcount++;//與S_MRS重複
			}
			N->merge_ram_writecount++;//
			merge_ram_writecount++;
			//
			N->S_MRS += (I64)merge_ram_size;

			//transfer time
			N->Req_RunTime += (2 * NC->blockSizeTrack * 10);
			N->Req_RunTime += ((double)merge_ram_size / NC->trackSizeSector * 10);

			Merge_W_SeekCount = N->Merge_W_SeekCount;
			//combine DB和SLB資料
			for (j = 0; j < NC->blockSizeTrack; j++){//寫入DB
				SectorTableNo3 = dataBlock*NC->blockSizeSector + j*NC->trackSizeSector; assert(0 <= SectorTableNo3 && SectorTableNo3 < NC->LsizePage);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo3, 2);//寫在DB上   / NC->CylinderSizeTrack
				//fprintf(t, "mw db %lu\n", SectorTableNo3);
				test(simPtr, (I64)SectorTableNo3, 2);

				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate++;
				N->MW_content_rotate++;///

				N->Merge_Trackcount++;//
				Merge_Trackcount++;
			}
			merge_ram_size = 0;
			assert(N->Merge_W_SeekCount - Merge_W_SeekCount == NC->blockSizeCylinder);

#endif
			assert(0 <= validIs0); assert(validIs0 <= NC->blockSizeSector);
			N->blocks[Victim].sections[i].valid = 0;
			//N->SourceTracks_count += (I64)SourceTracks_count;
			assert(Merge_Trackcount == NC->blockSizeTrack);
		}
		else {
			NoMerge_Band++;
			assert(SectorTableNo1 < NC->LsizePage);
			assert(getblock(simPtr, SectorTableNo1) != Victim || getsection(simPtr, SectorTableNo1) != i);
		}
		//N->blocks[Victim].pages[i].sector = 0x7fffffff;
		assert(N->blocks[Victim].sections[i].valid == 0);
	}

	if (Merge_Bands > 0){
		fprintf(ftime, "Merge_Bands %lu, Dirty_Sectors %I64u, Cumu_Dirty_Sectors %I64u\n", Merge_Bands, Dirty_Sectors, Cumu_Dirty_Sectors);
		N->ValidIs0 += Dirty_Sectors; N->Cumu_Dirty_Sectors += Cumu_Dirty_Sectors;
		N->Effect_Merge++;
	}
	//for (j = tsp_start; j <= tsp_end; j++){ assert(N->blocks[Victim].pages[j].valid == 0); }//new變valid
	if (NoMerge_Band == NC->trackSizePage){ N->NoMerge_Band++; }
	//update SMR LB information
	SLB->firstRWsection = (SLB->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
	for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
		N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;
		N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
	}
	SLB->writedSectionNum -= (NC->CylinderSizeTrack*NC->trackSizeSection);
	free(SourceTracks);
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
void DM_NSLBmerge2(sim *simPtr)//一次merge整個data band
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD CurrentSectorNo = 0;
	DWORD mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD flag = 0, NSLBdirtyTrack = 0;

	DWORD tmp = 0;
	//試驗
	DWORD merge_ram_writecount = 0, Merge_Trackcount = 0;

#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
#endif
	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		flag = 0; NSLBdirtyTrack = 0;
		validIs1 = 0; validIs2 = 0;

		N->N2S_LBsector_len = 0;
		for (k = 0; k < NC->blockSizeSection; k++){
			SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->Section; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			assert(0<=N->blocks[dataBlock].sections[k].valid && N->blocks[dataBlock].sections[k].valid <= 2);
			if (N->blocks[dataBlock].sections[k].valid == 1){
				assert(N->blocks[dataBlock].sections[k].sector == SectorTableNo);
				//統計資料更新
				//NSLB->pageRead++; NSLB->pageWrite++; 
				validIs1++; //read: DB hit, write: DB write
			}
			else if (N->blocks[dataBlock].sections[k].valid == 2){
				validIs2++; flag = 1;
				NSLBdirtyTrack = N->blocks[dataBlock].sections[k].sector / NC->trackSizeSector;
				if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectionTable[NSLBdirtyTrack]++;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = SLB->Partial_Section + 1 + N->blocks[dataBlock].sections[k].sector; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
#endif
				N->N2S_LBsector[N->N2S_LBsector_len] = SectorTableNo; N->N2S_LBsector_len++; assert(N->N2S_LBsector_len <= NC->blockSizeSector);
				//寫回原DB
				N->blocks[dataBlock].sections[k].valid = 1;
				N->blocks[dataBlock].sections[k].sector = SectorTableNo;
				//統計資料更新
				//NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
				N->NSLBwriteout_sector++;
			}
		}
#ifdef MergeAssoc_SLBpart
		//N->l2pmapOvd.round_LB_Live_Section -= merge_ram_size;
		N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / NSLB->NSLBsection_num);
		N->l2pmapOvd.AA -= merge_ram_size;
		//assert(0 <= N->l2pmapOvd.round_LB_Live_Section);
		//assert(N->l2pmapOvd.round_LB_Live_Section <= SLB->SLBsection_num);
		//N->l2pmapOvd.LB_Live_Rate += ((double)N->l2pmapOvd.round_LB_Live_Section / SLB->SLBsection_num);
		//N->l2pmapOvd.AA_Rate += ((double)N->l2pmapOvd.AA / SLB->SLBsection_num);

		fprintf(fp_NSLB, "%lu (%lu:%lu), %lu ( %lu)\n", dataBlock, validIs1, validIs2, merge_ram_writecount, N->l2pmapOvd.AA);//SourceTracks_count, N->l2pmapOvd.round_LB_Live_Section,
#endif
		if (0 < N->N2S_LBsector_len && N->N2S_LBsector_len <= 2 * NC->trackSizeSector){
			N->Merge_Bandcount2_1++;
			for (k = 0; k<N->N2S_LBsector_len; k++){ assert(N->N2S_LBsector[i]<NC->LsizeSector); }
			DM_N2SLBw2(simPtr);
		}
		else if (0 < N->N2S_LBsector_len){
			N->Merge_Bandcount2_2++;
#ifdef Time_Overhead
			merge_ram_size = 0;
			for (k = 0; k < NC->blockSizeTrack; k++){//讀DB資料
				SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, SectorTableNo, 2);
				Calc_RotateOverhead2(simPtr, 0, 3);//把DBtrack舊資料整條讀到merge_ram裡
				N->content_rotate++;
				N->MR_content_rotate++;///
				N->Req_RunTime += 10;
				//統計資料更新
				NSLB->pageRead += NC->trackSizeSector;
			}
#endif
#ifdef Time_Overhead
			//xfer time
			N->Req_RunTime += ((double)(merge_ram_size) / NC->trackSizeSector * 10);
			//combine DB和SLB資料
			for (k = 0; k < NC->blockSizeTrack; k++){//寫入DB
				SectorTableNo = dataBlock*NC->blockSizeSector + k*NC->trackSizeSector; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, SectorTableNo, 2);//寫在DB上   / NC->CylinderSizeTrack
				test(simPtr, SectorTableNo, 2);
				Calc_RotateOverhead2(simPtr, 0, 2);//write DB track
				N->content_rotate++;
				N->MW_content_rotate++;///
				N->Req_RunTime += 10;

				N->Merge_Trackcount++;//
				Merge_Trackcount++;
				//
				NSLB->pageWrite += NC->trackSizeSector;
			}
#endif
			/*NSLB merge順便帶走SLB sector, 會過早*/
//			for (k = 0; k < NC->blockSizeSection; k++){
//				if (N->blocks[dataBlock].sections[k].valid == 0){ //invalid在LB的該page
//					SectorTableNo = dataBlock*NC->blockSizeSector + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
//					DWORD block_no = getblock(simPtr, SectorTableNo), section_no = getsection(simPtr, SectorTableNo);
//					assert(block_no == NC->PsizeBlock - 1); assert(0 <= section_no && section_no <= SLB->Partial_Section);
//					assert(N->blocks[block_no].sections[section_no].valid == 1);
//					N->blocks[block_no].sections[section_no].valid = 0;//invalid old
//					//更新統計資料
//					SLB->pageRead++; SLB->pageWrite++;
//#ifdef Time_Overhead
//					merge_ram[merge_ram_size] = section_no*NC->Section; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSection);
//#endif
//					setblock(simPtr, SectorTableNo, dataBlock);
//					setsection(simPtr, SectorTableNo, k);
//					N->blocks[dataBlock].sections[k].sector = SectorTableNo;
//					N->blocks[dataBlock].sections[k].valid = 1;
//					//統計這個band在SLB活著的section
//				}
//			}
		}
#ifdef Time_Overhead
		//assert(0 < merge_ram_size); 
		assert(merge_ram_size <= NC->blockSizeSection);
		if (merge_ram_size > 0){ NSLB->merge_count++; }
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }
		if (merge_ram_size > 0){
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
			test(simPtr, (NC->LsizeSector + merge_ram[0]), 3);
			Calc_RotateOverhead2(simPtr, 0, 3);//read SLB track
			N->content_rotate += ((double)merge_ram_size / NC->trackSizeSection);
			N->MR_content_rotate += ((double)merge_ram_size / NC->trackSizeSection);///
			N->Merge_LB_Trackcount++;
			for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
				test(simPtr, (NC->LsizeSector + merge_ram[k]), 3);
				if (merge_ram[k - 1] / NC->trackSizeSector != merge_ram[k] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 3); }
				N->Merge_LB_Trackcount++;//
			}
			N->merge_ram_writecount++;//
			merge_ram_writecount++;
			merge_ram_size = 0;
		}
		//seek
		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
#endif
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		//assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	NSLB->writedSectionNum -= validIs2;
#ifdef Time_Overhead
	free(merge_ram);
#endif
}
#endif

#ifdef SLBNSLBPM_Tmerge
//----------------------------------------------
//SLBNSLBPMe: 只要有Band能做PM就清空間給它做
//----------------------------------------------
//Push to HEAD
void PMQ_Push(sim *simPtr, DWORD Band_No, DWORD SLB_PageIndex) //BlockNo, SectorTableNo
{
	NFTL *N = &(simPtr->NFTLobj);
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;

	//-------------
	DWORD tmp_len0 = 0, tmp_len1 = 0;
	struct PM_NODE *ct0 = N->FreeQ, *ct1 = N->PMQ_HEAD;
	while (ct0 != NULL){
		tmp_len0++; ct0 = ct0->next;
	}
	while (ct1 != NULL){
		tmp_len1++; ct1 = ct1->next;
	}
	assert(tmp_len0 == N->Free_Tnum); assert(tmp_len1 == N->PMQ_Tnum);
	//printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); assert(tmp_len0 + tmp_len1 == N->PM_Tracks);// system("pause");
	//-------------

	//assert(N->FreeQ != NULL); assert(N->Free_Tnum > 0); assert(N->Free_Tnum + N->PMQ_Tnum == N->PM_Tracks);
	if (N->FreeQ == NULL){
		printf("Free_Tnum:%lu, PMQ_Tnum:%lu\n", N->Free_Tnum, N->PMQ_Tnum); system("pause");
	}
	struct PM_NODE *tmp = N->FreeQ;
	N->FreeQ = N->FreeQ->next;
	N->Free_Tnum = N->Free_Tnum - 1;

	assert(tmp->Band_No == 0); assert(tmp->SLB_PageIndex == 0);
	tmp->Band_No = Band_No;
	tmp->SLB_PageIndex = SLB_PageIndex;
	if (N->PMQ_Tnum == 0){//First time Push
		assert(N->PMQ_HEAD == NULL && N->PMQ_TAIL == NULL);
		N->PMQ_HEAD = tmp;
		N->PMQ_TAIL = tmp;
		tmp->next = NULL;
	}
	else{
		tmp->next = N->PMQ_HEAD;
		N->PMQ_HEAD = tmp;
	}
	N->PMQ_Tnum = N->PMQ_Tnum + 1;
	assert(N->PMQ_Tnum <= N->PM_Tracks); assert(N->Free_Tnum + N->PMQ_Tnum == N->PM_Tracks);

	//-------------
	tmp_len0 = 0; tmp_len1 = 0;
	ct0 = N->FreeQ; ct1 = N->PMQ_HEAD;
	while (ct0 != NULL){
		tmp_len0++; ct0 = ct0->next;
	}
	while (ct1 != NULL){
		tmp_len1++; ct1 = ct1->next;
	}
	assert(tmp_len0 == N->Free_Tnum); assert(tmp_len1 == N->PMQ_Tnum);
	//printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); assert(tmp_len0 + tmp_len1 == N->PM_Tracks); //system("pause");
	//-------------
}
//Pop from TAIL
DWORD PMQ_Pop(sim *simPtr)
{
	NFTL *N = &(simPtr->NFTLobj);
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	struct PM_NODE *pre = N->PMQ_HEAD, *cur = N->PMQ_TAIL;
	DWORD SLB_PageIndex, BandNo;

	//-------------
	DWORD tmp_len0 = 0, tmp_len1 = 0;
	struct PM_NODE *ct0 = N->FreeQ, *ct1 = N->PMQ_HEAD;
	while (ct0 != NULL){
		tmp_len0++; ct0 = ct0->next;
	}
	while (ct1 != NULL){
		tmp_len1++; ct1 = ct1->next;
	}
	assert(tmp_len0 == N->Free_Tnum); assert(tmp_len1 == N->PMQ_Tnum);
	//printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); assert(tmp_len0 + tmp_len1 == N->PM_Tracks);// system("pause");
	//-------------

	assert(N->PMQ_Tnum > 0); assert(N->PMQ_HEAD != NULL);
	assert(N->Free_Tnum + N->PMQ_Tnum == N->PM_Tracks);

	if (N->PMQ_Tnum == 1){
		assert(pre == cur);
		assert(pre == N->PMQ_TAIL);
		cur->next = N->FreeQ;
		N->FreeQ = cur;
		N->PMQ_HEAD = NULL;
		N->PMQ_TAIL = NULL;
		N->PMQ_Tnum--; assert(N->PMQ_Tnum == 0);
		N->Free_Tnum++; assert(N->Free_Tnum == N->PM_Tracks);
	}
	else{
		while (pre->next != cur) pre = pre->next;

		cur->next = N->FreeQ;
		N->FreeQ = cur;

		pre->next = NULL;
		N->PMQ_TAIL = pre;
		N->PMQ_Tnum--;
		N->Free_Tnum++;
		assert(N->Free_Tnum + N->PMQ_Tnum == N->PM_Tracks);
	}
	assert(cur != NULL);
	DWORD i;
	//清除PM info，此資料等到下次自己再被切時才會更新
	assert(N->blocks[cur->Band_No].Cut == 1);
	N->blocks[cur->Band_No].Cut = 0;
	N->blocks[cur->Band_No].GuardTrackIndex = 0;

	/*DWORD start = 0, end = NC->blockSizePage - 1;
	while (N->blocks[cur->Band_No].pages[start].valid == 1) start++;

	SLB->pageRead += (end - start + 1);
	SLB->pageWrite += (end - start + 1);*/
	SLB_PageIndex = cur->SLB_PageIndex;
	BandNo = cur->Band_No;
	assert(cur == N->FreeQ);
	cur->Band_No = 0; assert(N->FreeQ->Band_No == 0);
	cur->SLB_PageIndex = 0; assert(N->FreeQ->SLB_PageIndex == 0);

	//-------------
	tmp_len0 = 0; tmp_len1 = 0;
	ct0 = N->FreeQ; ct1 = N->PMQ_HEAD;
	while (ct0 != NULL){
		tmp_len0++; ct0 = ct0->next;
	}
	while (ct1 != NULL){
		tmp_len1++; ct1 = ct1->next;
	}
	assert(tmp_len0 == N->Free_Tnum); assert(tmp_len1 == N->PMQ_Tnum);
	//printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); assert(tmp_len0 + tmp_len1 == N->PM_Tracks); //system("pause");
	//-------------

	return BandNo;
}
//Move second-time meet but no PM Band from PMQ to FreeQ
void PMQ_Delete(sim *simPtr, DWORD Band_No)
{
	NFTL *N = &(simPtr->NFTLobj);
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	struct PM_NODE *pre = NULL, *find = N->PMQ_HEAD;

	//-------------
	DWORD tmp_len0 = 0, tmp_len1 = 0;
	struct PM_NODE *ct0 = N->FreeQ, *ct1 = N->PMQ_HEAD;
	while (ct0 != NULL){
		tmp_len0++; ct0 = ct0->next;
	}
	while (ct1 != NULL){
		tmp_len1++; ct1 = ct1->next;
	}
	assert(tmp_len0 == N->Free_Tnum); assert(tmp_len1 == N->PMQ_Tnum);
	//printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); assert(tmp_len0 + tmp_len1 == N->PM_Tracks);// system("pause");
	//-------------

	assert(0 < N->PMQ_Tnum);
	assert(N->PMQ_Tnum <= N->PM_Tracks); assert(N->Free_Tnum + N->PMQ_Tnum == N->PM_Tracks);

	if (N->PMQ_Tnum == 1){
		assert(find == N->PMQ_HEAD);
		assert(find->Band_No == Band_No);
		find->next = N->FreeQ;
		N->FreeQ = find;
		N->Free_Tnum++; assert(N->Free_Tnum == N->PM_Tracks);
		N->FreeQ->Band_No = 0; N->FreeQ->SLB_PageIndex = 0; assert(N->FreeQ == N->PMQ_HEAD);

		N->PMQ_HEAD = NULL;
		N->PMQ_TAIL = NULL;
		N->PMQ_Tnum--; assert(N->PMQ_Tnum == 0);
	}
	else{
		while (find->Band_No != Band_No){
			pre = find; find = find->next;
		}
		assert(find != NULL);
		if (find == N->PMQ_HEAD){
			N->PMQ_HEAD = find->next;
			N->PMQ_Tnum--;
		}
		else if (find == N->PMQ_TAIL){
			pre->next = NULL;
			N->PMQ_TAIL = pre;
			N->PMQ_Tnum--;
		}
		else{
			pre->next = find->next;
			N->PMQ_Tnum--;
		}
		find->next = N->FreeQ;
		N->FreeQ = find;
		N->Free_Tnum++;
		find->Band_No = 0; find->SLB_PageIndex = 0;
		assert(N->Free_Tnum + N->PMQ_Tnum == N->PM_Tracks);
	}

	//-------------
	tmp_len0 = 0; tmp_len1 = 0;
	ct0 = N->FreeQ; ct1 = N->PMQ_HEAD;
	while (ct0 != NULL){
		tmp_len0++; ct0 = ct0->next;
	}
	while (ct1 != NULL){
		tmp_len1++; ct1 = ct1->next;
	}
	assert(tmp_len0 == N->Free_Tnum); assert(tmp_len1 == N->PMQ_Tnum);
	//printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); assert(tmp_len0 + tmp_len1 == N->PM_Tracks); //system("pause");
	//-------------
}
//Move second-time PM Band from PMQ middle to HEAD
DWORD PMQ_DeletePush(sim *simPtr, DWORD Band_No)
{
	NFTL *N = &(simPtr->NFTLobj);
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	struct PM_NODE *pre = NULL, *find = N->PMQ_HEAD;

	//-------------
	DWORD tmp_len0 = 0, tmp_len1 = 0;
	struct PM_NODE *ct0 = N->FreeQ, *ct1 = N->PMQ_HEAD;
	while (ct0 != NULL){
		tmp_len0++; ct0 = ct0->next;
	}
	while (ct1 != NULL){
		tmp_len1++; ct1 = ct1->next;
	}
	assert(tmp_len0 == N->Free_Tnum); assert(tmp_len1 == N->PMQ_Tnum);
	//printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); assert(tmp_len0 + tmp_len1 == N->PM_Tracks);// system("pause");
	//-------------

	assert(1 <= N->PMQ_Tnum); assert(N->PMQ_Tnum <= N->PM_Tracks); assert(N->Free_Tnum + N->PMQ_Tnum == N->PM_Tracks);
	while (find != NULL){
		if (find->Band_No == Band_No) break;

		pre = find;
		find = find->next;
	}
	assert(find != NULL);

	if (pre == NULL){
		assert(find == N->PMQ_HEAD);
	}
	else{
		assert(N->PMQ_Tnum > 1); assert(pre != NULL);

		if (find == N->PMQ_TAIL){
			pre->next = NULL;
			N->PMQ_TAIL = pre;
		}
		else{
			pre->next = find->next;
		}
		find->next = N->PMQ_HEAD;
		N->PMQ_HEAD = find;
	}
	assert(N->PMQ_Tnum <= N->PM_Tracks); assert(N->Free_Tnum + N->PMQ_Tnum == N->PM_Tracks);

	//-------------
	tmp_len0 = 0; tmp_len1 = 0;
	ct0 = N->FreeQ; ct1 = N->PMQ_HEAD;
	while (ct0 != NULL){
		tmp_len0++; ct0 = ct0->next;
	}
	while (ct1 != NULL){
		tmp_len1++; ct1 = ct1->next;
	}
	assert(tmp_len0 == N->Free_Tnum); assert(tmp_len1 == N->PMQ_Tnum);
	//printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); assert(tmp_len0 + tmp_len1 == N->PM_Tracks); //system("pause");
	//-------------

	return find->SLB_PageIndex;
}
//全band單干擾方向Merge，但沒清除SLB中相關page的valid
DWORD WriteDB_SE(sim *simPtr, DWORD PMmerge, DWORD BandNo)
{
	DWORD	RWLBA, RWLPA, newBlock, dataBlock, BlockNo, Victim, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo = 0, PageNo = 0;
	DWORD	i = 0, j = 0;
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD start = 0, end = 0, new_start = 0;
	DWORD block_no = 0, page_no = 0;

	DWORD tmpSLB_Read = 0, tmpSLB_Write = 0, tmpNSLB_Read = 0, tmpNSLB_Write = 0;
	DWORD SectorTableNo2 = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD validIs2 = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
	merge_ram_size = 0;
#endif

	Victim = NC->PsizeBlock - 1;
	dataBlock = BandNo;
	end = NC->blockSizePage - 1;
	start = 0;
	//單向往後干擾，從頭開始找第一個invalid page往後面做GC。
	while (N->blocks[dataBlock].pages[start].valid == 1){ start++; }
	assert(start != NC->blockSizePage - 1); new_start = start;
	for (j = start; j <= end; j++){ //sub-band的起點到終點
		SectorTableNo = dataBlock*NC->blockSizePage + j;
		block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
		if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
			assert(block_no == dataBlock); assert(page_no == j);
			tmpSLB_Read++; tmpSLB_Write++;
		}
		else if (N->blocks[dataBlock].pages[j].valid == 2){//clear the page in NSLB
			assert(block_no == dataBlock);
			assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
			NSLB->writedPageNum = NSLB->writedPageNum - 1;

			NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
			N->NSLBremainSectorTable[NSLBfreeTrack]++;

			tmpNSLB_Read++; tmpNSLB_Write++; validIs2++;
#ifdef Time_Overhead
			merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
#endif
		}
		else if (N->blocks[dataBlock].pages[j].valid == 0){ //在log band or victim_backup 裡的dirty page 被 clear
			assert(block_no == NC->PsizeBlock - 1);
			//invalid在LB的該page 
			if (getblock(simPtr, dataBlock*NC->blockSizePage + j) >= NC->LsizeBlock && getblock(simPtr, dataBlock*NC->blockSizePage + j) < NC->PsizeBlock){
				N->blocks[getblock(simPtr, dataBlock*NC->blockSizePage + j)].pages[getpage(simPtr, dataBlock*NC->blockSizePage + j)].valid = 0;	//old變invalid
			}
			tmpSLB_Read++; tmpSLB_Write++;
#ifdef Time_Overhead
			merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
#endif
		}
		else if (N->blocks[dataBlock].pages[j].valid == 3){//PM的GT
			assert(block_no == dataBlock); assert(page_no == j);
			//tmpSLB_Read++; tmpSLB_Write++;
		}
		else{ printf("SLB GC error\n"); system("pause"); }

		SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
		setblock(simPtr, SectorTableNo, dataBlock);
		setpage(simPtr, SectorTableNo, j);
		assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
		N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
		N->blocks[dataBlock].pages[j].valid = 1; //new變valid
	}
#ifdef Time_Overhead
	assert(merge_ram_size <= NC->blockSizeSector);
	//seek: PM Space放GT
	//Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (SLB->SLBpage_num / NC->trackSizePage) + N->PM_Tracks_Use + 1) / NC->CylinderSizeTrack, 1);
	qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
	if (merge_ram_size>1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
	assert(0 <= (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack && (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack < N->Cylinders);
	Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1);//電梯法回SLB讀資料
	for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
		if (merge_ram[j - 1] / NC->trackSizePage != merge_ram[j] / NC->trackSizePage){
			//seek
			assert(0 <= (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack && (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack < N->Cylinders);
			Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1);
		}
	}
	free(merge_ram);
#endif
	if (PMmerge != -1){
		assert(N->blocks[dataBlock].Cut == 1);
		SLB->pageRead += PMmerge; SLB->pageWrite += PMmerge;
		NSLB->pageRead += PMmerge; NSLB->pageWrite += PMmerge;
#ifdef Time_Overhead
		//DB seek
		for (j = N->pre_start; j < N->blocks[dataBlock].GuardTrackIndex; j++){
			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
		}
		for (j = N->pos_start; j < NC->blockSizeTrack; j++){
			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
		}
#endif
	}
	else{
		SLB->pageRead += tmpSLB_Read; SLB->pageWrite += tmpSLB_Write;
		NSLB->pageRead += tmpNSLB_Read; NSLB->pageWrite += tmpNSLB_Write;
#ifdef Time_Overhead
		//DB seek
		for (j = new_start / NC->trackSizePage; j < NC->blockSizeTrack; j++){
			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
		}
#endif
	}
	return validIs2;
}
//統計SLB做MergeDB的寫入量。PM_Condition:0是一般merge，大於0是Mini_TrackWrite的值
void MergeSLB_Statistics(sim *simPtr, DWORD dataBlock, DWORD PM_Condition)
{
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD start = 0, end = 0, PageWriteAmount = 0, PageReadAmount = 0;
	DWORD Mini_TrackWrite = 0;
	/*if (PM_Condition < 0){
	printf("PM_Condition = %lu\n", PM_Condition); system("pause");
	}*/
	//#ifdef Time_Overhead
	//	double dScale = 0;
	//#endif
	assert(PM_Condition >= 0);
	if (PM_Condition == 0)
	{
		if (N->blocks[dataBlock].Cut == 1){
			start = 0; end = N->blocks[dataBlock].GuardTrackIndex*NC->trackSizePage - 1;
			for (start = 0; start < end; start++){ if (N->blocks[dataBlock].pages[start].valid == 0) break; }
			PageWriteAmount = (end - start + 1);
			start = N->blocks[dataBlock].GuardTrackIndex*NC->trackSizePage + NC->trackSizePage; end = NC->blockSizePage - 1;
			for (; start < end; start++){ if (N->blocks[dataBlock].pages[start].valid == 0) break; }
			PageWriteAmount += (end - start + 1);
			PageReadAmount = PageWriteAmount = NC->blockSizePage - N->blocks[dataBlock].GuardTrackIndex*NC->trackSizePage;//??? 應該用(end - start + 1)吧
		}
		else{
			assert(N->blocks[dataBlock].Cut == 0);
			start = 0; end = NC->blockSizePage - 1;//單向往後干擾，從頭開始找第一個invalid page往後面做GC。 don't write the previous valid pages
			while (N->blocks[dataBlock].pages[start].valid == 1) start++;

			PageReadAmount = PageWriteAmount = end - start + 1;
		}
		SLB->pageRead += PageWriteAmount; //assert(PageWriteAmount > 0); //if (!(end - start + 1 > 0)){ printf("3.%lu %lu\n", start, end); system("pause"); }
		SLB->pageWrite += PageWriteAmount;

	}
	else{
		Mini_TrackWrite = PM_Condition;
		//只讀寫PM預算的量
		SLB->pageRead += (Mini_TrackWrite*NC->trackSizePage); assert(Mini_TrackWrite*NC->trackSizePage > 0);
		SLB->pageWrite += (Mini_TrackWrite*NC->trackSizePage);

	}
}

//-------------------------------------以下使用
//NSLBPM
void FASTmergeNormalNSLB(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD tmp = 0;

	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
	DWORD SectorTableNo1 = 0;
	DWORD block_no = 0, page_no = 0;
	DWORD start = 0;
	DWORD l = 0;
	BYTE Dirty[256] = { 0 };
	DWORD Dirty_Num = 0;

	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
#ifdef NEWGC64trackWRITE
		DWORD TrackIndex = 0, GuardTrackIndex = -1;
		DWORD pre_start, pos_start;
		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
#ifdef Band_64MB_8Track
		assert(Mini_TrackWrite == 64);
		DirtyTrack_Group = 8; TrackSearch_End = 61;
#endif
#ifdef Band_128MB_16Track
		assert(Mini_TrackWrite == 128);
		DirtyTrack_Group = 16; TrackSearch_End = 125;
#endif
#ifdef Band_256MB_32Track
		assert(Mini_TrackWrite == 256);
		DirtyTrack_Group = 32; TrackSearch_End = 253;
#endif
		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
		for (k = 0; k < TrackSearch_End + 3; k++){
			Dirty[k] = 0;
			for (l = 0; l < NC->trackSizePage; l++){
				if (N->blocks[dataBlock].pages[k*NC->trackSizePage + l].valid == 2){
					Dirty[k] = 1; Dirty_Num++; break;
				}
			}
		}
		//預先計算PM寫多track
		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			if (tmp_TrackWrite < Mini_TrackWrite){
				Mini_Index = GuardTrackIndex;// TrackIndex;
				Mini_TrackWrite = tmp_TrackWrite;
				N->pre_start = pre_start; N->pos_start = pos_start;
			}
		}
		N->All_Merge++;
		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
			N->pre_start = pre_start; N->pos_start = pos_start;
		}
		assert(Dirty_Num <= Mini_TrackWrite); N->Miss_Rel_len += Dirty_Num;
		if ((Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1) || (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Tracks_Use < N->PM_Tracks)){
			N->Allow_PM++; //N->DO_PM = 0; //N->PM_len = 0;
			N->Miss_PM_len += Mini_TrackWrite;
		}
#endif
		validIs2 += WriteDB_SE(simPtr, -1, dataBlock);
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	//NSLB->writedPageNum -= validIs2;
#ifdef MergeAssoc_NSLBpart
	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
#endif
}

void FASTmergeRWNSLBPM(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD tmp = 0;

	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
	DWORD SectorTableNo1 = 0;
	DWORD block_no = 0, page_no = 0;
	DWORD start = 0;
	DWORD l = 0;
	BYTE Dirty[256] = { 0 };
	DWORD Dirty_Num = 0;

	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
#ifdef NEWGC64trackWRITE
		DWORD TrackIndex = 0, GuardTrackIndex = -1;
		DWORD pre_start, pos_start;
		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
#ifdef Band_64MB_8Track
		assert(Mini_TrackWrite == 64);
		DirtyTrack_Group = 8; TrackSearch_End = 61;
#endif
#ifdef Band_128MB_16Track
		assert(Mini_TrackWrite == 128);
		DirtyTrack_Group = 16; TrackSearch_End = 125;
#endif
#ifdef Band_256MB_32Track
		assert(Mini_TrackWrite == 256);
		DirtyTrack_Group = 32; TrackSearch_End = 253;
#endif
		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
		for (k = 0; k < TrackSearch_End + 3; k++){
			Dirty[k] = 0;
			for (l = 0; l < NC->trackSizePage; l++){
				if (N->blocks[dataBlock].pages[k*NC->trackSizePage + l].valid == 2){
					Dirty[k] = 1; Dirty_Num++; break;
				}
			}
		}
		//預先計算PM寫多track
		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			if (tmp_TrackWrite < Mini_TrackWrite){
				Mini_Index = GuardTrackIndex;// TrackIndex;
				Mini_TrackWrite = tmp_TrackWrite;
				N->pre_start = pre_start; N->pos_start = pos_start;
			}
		}
		N->All_Merge++;
		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
			N->pre_start = pre_start; N->pos_start = pos_start;
		}
		assert(Dirty_Num <= Mini_TrackWrite); N->Rel_len += Dirty_Num;

		if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1){
			if (N->blocks[dataBlock].GuardTrackIndex < Mini_Index){//GT右移
				//PM info: 更新track位置 //舊GT的所有page的valid恢復成1
				for (k = 0; k < NC->trackSizePage; k++){
					if (N->blocks[dataBlock].pages[N->blocks[dataBlock].GuardTrackIndex*NC->trackSizePage + k].valid == 3){
						N->blocks[dataBlock].pages[N->blocks[dataBlock].GuardTrackIndex*NC->trackSizePage + k].valid = 1;
					}
				}
				N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
				//新GT的所有page的valid設成3
				for (k = 0; k < NC->trackSizePage; k++){
					if (N->blocks[dataBlock].pages[N->blocks[dataBlock].GuardTrackIndex*NC->trackSizePage + k].valid == 1){
						N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid = 3;
					}
				}
			}
			//只讀寫PM預算的量 //SLB->pageWrite += (Mini_TrackWrite*NC->trackSizePage);
			validIs2 += WriteDB_SE(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock);
			N->PM_len += Mini_TrackWrite;
			N->DO_PM++;
		}
		else if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Tracks_Use < N->PM_Tracks){
			//設定PM info
			N->blocks[dataBlock].Cut = 1;
			N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
			N->PM_Tracks_Use++; assert(N->PM_Tracks_Use <= N->PM_Tracks);
			//紀錄GT sector，把DB上的GT設成valid=3
			for (k = 0; k < NC->trackSizePage; k++){
				SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
				assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
				block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
				assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
				if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
					assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1);
					N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid = 3;
				}
			}
			//只讀寫PM預算的量 //SLB->pageWrite += (Mini_TrackWrite*NC->trackSizePage);
			validIs2 += WriteDB_SE(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock);
			N->PM_len += Mini_TrackWrite;
			N->DO_PM++;
			N->PM_Round++;
		}
		else{//歸還PM Track
			if (N->blocks[dataBlock].Cut == 1){ N->blocks[dataBlock].Cut = 0; N->PM_Tracks_Use--; assert(0 <= N->PM_Tracks_Use); }
			validIs2 += WriteDB_SE(simPtr, -1, dataBlock);
		}
#endif

		//		start = 0;
		//		while (N->blocks[dataBlock].pages[start].valid == 1){ start++; }
		//		assert(start < NC->blockSizeSector);
		//		for (k = start; k < NC->blockSizeSector; k++){
		//			SectorTableNo = dataBlock*NC->blockSizeSector + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
		//			assert(N->blocks[dataBlock].pages[k].valid == 1 || N->blocks[dataBlock].pages[k].valid == 2);
		//			if (N->blocks[dataBlock].pages[k].valid == 1){
		//				assert(N->blocks[dataBlock].pages[k].sector == SectorTableNo);
		//				//統計資料更新
		//				NSLB->pageRead++; NSLB->pageWrite++; validIs1++; //read: DB hit, write: DB write
		//				NSLB->merge_count++;
		//			}
		//			else if (N->blocks[dataBlock].pages[k].valid == 2){
		//				validIs2++;
		//				NSLBdirtyTrack = N->blocks[dataBlock].pages[k].sector / NC->trackSizePage;
		//				if (N->NSLBremainSectorTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
		//				N->NSLBremainSectorTable[NSLBdirtyTrack]++;
		//#ifdef Time_Overhead
		//				merge_ram[merge_ram_size] = NSLBdirtyTrack*NC->trackSizePage; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
		//				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
		//#endif
		//				//寫回原DB
		//				N->blocks[dataBlock].pages[k].valid = 1;
		//				N->blocks[dataBlock].pages[k].sector = SectorTableNo;
		//				//統計資料更新
		//				NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
		//				NSLB->merge_count++;
		//			}
		//
		//		}
		//#ifdef Time_Overhead
		//		assert(merge_ram_size <= NC->blockSizeSector);
		//		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		//		for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
		//		Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 3);//電梯法回SLB讀資料
		//		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
		//			if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){
		//				//seek
		//				Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[k] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 3);
		//			}
		//		}
		//		//DB seek
		//		for (k = 0; k < NC->blockSizePage; k += NC->trackSizePage){
		//			SectorTableNo = dataBlock*NC->blockSizePage + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
		//			Calc_TimeOverhead(simPtr, SectorTableNo / NC->trackSizePage / NC->CylinderSizeTrack, 3);//寫在DB上
		//		}
		//		//seek
		//		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
		//#endif
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	//NSLB->writedPageNum -= validIs2;
#ifdef MergeAssoc_NSLBpart
	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
#endif
}






void FASTmergeNormalNSLB2(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD tmp = 0;

	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
	DWORD SectorTableNo1 = 0;
	DWORD block_no = 0, page_no = 0;
	DWORD start = 0;
	DWORD l = 0;
	BYTE Dirty[256] = { 0 };
	DWORD Dirty_Num = 0;

	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
#ifdef NEWGC64trackWRITE
		DWORD TrackIndex = 0, GuardTrackIndex = -1;
		DWORD pre_start, pos_start;
		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
#ifdef Band_64MB_8Track
		assert(Mini_TrackWrite == 64);
		DirtyTrack_Group = 8; TrackSearch_End = 61;
#endif
#ifdef Band_128MB_16Track
		assert(Mini_TrackWrite == 128);
		DirtyTrack_Group = 16; TrackSearch_End = 125;
#endif
#ifdef Band_256MB_32Track
		assert(Mini_TrackWrite == 256);
		DirtyTrack_Group = 32; TrackSearch_End = 253;
#endif
		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
		for (k = 0; k < TrackSearch_End + 3; k++){
			Dirty[k] = 0;
			for (l = 0; l < NC->trackSizePage; l++){
				if (N->blocks[dataBlock].pages[k*NC->trackSizePage + l].valid == 2){
					Dirty[k] = 1; Dirty_Num++; break;
				}
			}
		}
		//預先計算PM寫多track
		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			if (tmp_TrackWrite < Mini_TrackWrite){
				Mini_Index = GuardTrackIndex;// TrackIndex;
				Mini_TrackWrite = tmp_TrackWrite;
				N->pre_start = pre_start; N->pos_start = pos_start;
			}
		}
		N->All_Merge++;
		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
			N->pre_start = pre_start; N->pos_start = pos_start;
		}
		assert(Dirty_Num <= Mini_TrackWrite); N->Miss_Rel_len += Dirty_Num;
		if ((Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1) || (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Tracks_Use < N->PM_Tracks)){
			N->Allow_PM++; //N->DO_PM = 0; //N->PM_len = 0;
			N->Miss_PM_len += Mini_TrackWrite;
		}
#endif
		validIs2 += WriteDB_SE2(simPtr, -1, dataBlock);
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	//NSLB->writedPageNum -= validIs2;
#ifdef MergeAssoc_NSLBpart
	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
#endif
}

void FASTmergeRWNSLBPM2(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD tmp = 0;

	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
	DWORD SectorTableNo1 = 0;
	DWORD block_no = 0, page_no = 0;
	DWORD start = 0;
	DWORD l = 0;
	BYTE Dirty[256] = { 0 };
	DWORD Dirty_Num = 0;

	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
#ifdef NEWGC64trackWRITE
		DWORD TrackIndex = 0, GuardTrackIndex = -1;
		DWORD pre_start, pos_start;
		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
#ifdef Band_64MB_8Track
		assert(Mini_TrackWrite == 64);
		DirtyTrack_Group = 8; TrackSearch_End = 61;
#endif
#ifdef Band_128MB_16Track
		assert(Mini_TrackWrite == 128);
		DirtyTrack_Group = 16; TrackSearch_End = 125;
#endif
#ifdef Band_256MB_32Track
		assert(Mini_TrackWrite == 256);
		DirtyTrack_Group = 32; TrackSearch_End = 253;
#endif
		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
		for (k = 0; k < TrackSearch_End + 3; k++){
			Dirty[k] = 0;
			for (l = 0; l < NC->trackSizePage; l++){
				if (N->blocks[dataBlock].pages[k*NC->trackSizePage + l].valid == 2){
					Dirty[k] = 1; Dirty_Num++; break;
				}
			}
		}
		//預先計算PM寫多track
		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			if (tmp_TrackWrite < Mini_TrackWrite){
				Mini_Index = GuardTrackIndex;// TrackIndex;
				Mini_TrackWrite = tmp_TrackWrite;
				N->pre_start = pre_start; N->pos_start = pos_start;
			}
		}
		N->All_Merge++;
		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
			N->pre_start = pre_start; N->pos_start = pos_start;
		}
		assert(Dirty_Num <= Mini_TrackWrite); N->Rel_len += Dirty_Num;

		if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1){
			if (N->blocks[dataBlock].GuardTrackIndex < Mini_Index){//GT右移
				//PM info: 更新track位置 //舊GT的所有page的valid恢復成1
				for (k = 0; k < NC->trackSizePage; k++){
					if (N->blocks[dataBlock].pages[N->blocks[dataBlock].GuardTrackIndex*NC->trackSizePage + k].valid == 3){
						N->blocks[dataBlock].pages[N->blocks[dataBlock].GuardTrackIndex*NC->trackSizePage + k].valid = 1;
					}
				}
				N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
				//新GT的所有page的valid設成3
				for (k = 0; k < NC->trackSizePage; k++){
					if (N->blocks[dataBlock].pages[N->blocks[dataBlock].GuardTrackIndex*NC->trackSizePage + k].valid == 1){
						N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid = 3;
					}
				}
			}
			//只讀寫PM預算的量 //SLB->pageWrite += (Mini_TrackWrite*NC->trackSizePage);
			validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock);
			N->PM_len += Mini_TrackWrite;
			N->DO_PM++;
		}
		else if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Tracks_Use < N->PM_Tracks){
			//設定PM info
			N->blocks[dataBlock].Cut = 1;
			N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
			N->PM_Tracks_Use++; assert(N->PM_Tracks_Use <= N->PM_Tracks);
			//紀錄GT sector，把DB上的GT設成valid=3
			for (k = 0; k < NC->trackSizePage; k++){
				SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
				assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
				block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
				assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
				if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
					assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1);
					N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid = 3;
				}
			}
			//只讀寫PM預算的量 //SLB->pageWrite += (Mini_TrackWrite*NC->trackSizePage);
			validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock);
			N->PM_len += Mini_TrackWrite;
			N->DO_PM++;
			N->PM_Round++;
		}
		else{//歸還PM Track
			if (N->blocks[dataBlock].Cut == 1){ N->blocks[dataBlock].Cut = 0; N->PM_Tracks_Use--; assert(0 <= N->PM_Tracks_Use); }
			validIs2 += WriteDB_SE2(simPtr, -1, dataBlock);
		}
#endif
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	//NSLB->writedPageNum -= validIs2;
#ifdef MergeAssoc_NSLBpart
	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
#endif
}


//DWORD WriteDB_SE2(sim *simPtr, DWORD PMmerge, DWORD BandNo)
//{
//	DWORD	RWLBA, RWLPA, newBlock, dataBlock, BlockNo, Victim, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD	SectorTableNo = 0, PageNo = 0;
//	DWORD	i = 0, j = 0;
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD start = 0, end = 0, new_start = 0;
//	DWORD block_no = 0, page_no = 0;
//
//	DWORD tmpSLB_Read = 0, tmpSLB_Write = 0, tmpNSLB_Read = 0, tmpNSLB_Write = 0;
//	DWORD SectorTableNo2 = 0;
//	DWORD NSLBfreeTrack = 0;
//	DWORD validIs2 = 0, validIs3 = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
//	merge_ram_size = 0;
//#endif
//
//	Victim = NC->PsizeBlock - 1;
//	dataBlock = BandNo;
//	start = 0; end = NC->blockSizePage - 1;
//	//單向往後干擾，從頭開始找第一個invalid page往後面做GC。
//	while (N->blocks[dataBlock].pages[start].valid == 1){ start++; }
//	assert(start != NC->blockSizePage - 1); new_start = start;
//	for (j = new_start; j <= end; j++){ //start //sub-band的起點到終點
//		SectorTableNo = dataBlock*NC->blockSizePage + j;
//		block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//		if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
//			assert(block_no == dataBlock); assert(page_no == j);
//			tmpSLB_Read++; tmpSLB_Write++;
//		}
//		else if (N->blocks[dataBlock].pages[j].valid == 2){//clear the page in NSLB
//			assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//			NSLB->writedPageNum = NSLB->writedPageNum - 1;
//
//			NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
//			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//			N->NSLBremainSectorTable[NSLBfreeTrack]++;
//
//			tmpNSLB_Read++; tmpNSLB_Write++; validIs2++;
//#ifdef Time_Overhead
//			merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
//#endif
//		}
//		else if (N->blocks[dataBlock].pages[j].valid == 0){ //在log band or victim_backup 裡的dirty page 被 clear
//			assert(block_no == NC->PsizeBlock - 1);
//			//invalid在LB的該page 
//			if (getblock(simPtr, dataBlock*NC->blockSizePage + j) >= NC->LsizeBlock && getblock(simPtr, dataBlock*NC->blockSizePage + j) < NC->PsizeBlock){
//				N->blocks[getblock(simPtr, dataBlock*NC->blockSizePage + j)].pages[getpage(simPtr, dataBlock*NC->blockSizePage + j)].valid = 0;	//old變invalid
//			}
//			tmpSLB_Read++; tmpSLB_Write++;
//#ifdef Time_Overhead
//			merge_ram[merge_ram_size] = page_no; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
//#endif
//		}
//		else if (N->blocks[dataBlock].pages[j].valid == 3){//PM的GT在normal merge歸還
//			if (PMmerge == -1){//N->PM_Tracks_Use < N->PM_Tracks_Threshold &&    || (0 < PMmerge && PMmerge < NC->trackSizePage)
//				assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//				NSLB->writedPageNum = NSLB->writedPageNum - 1;
//
//				NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
//				if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//				N->NSLBremainSectorTable[NSLBfreeTrack]++;
//
//				SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
//				setblock(simPtr, SectorTableNo, dataBlock);
//				setpage(simPtr, SectorTableNo, j);
//				assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
//				N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
//				N->blocks[dataBlock].pages[j].valid = 1; //new變valid
//
//				tmpSLB_Read++; tmpSLB_Write++; validIs3++;
//				//N->OrgPM_Cost++;
//				//N->PM_Sectors_Use--;
//			}
//		}
//		else{ printf("SLB GC error\n"); system("pause"); }
//
//		if (N->blocks[dataBlock].pages[j].valid != 3){
//			SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
//			setblock(simPtr, SectorTableNo, dataBlock);
//			setpage(simPtr, SectorTableNo, j);
//			assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
//			N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
//			N->blocks[dataBlock].pages[j].valid = 1; //new變valid
//		}
//	}
//#ifdef Time_Overhead
//	assert(merge_ram_size <= NC->blockSizeSector);
//	//seek: PM Space放GT
//	//Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (SLB->SLBpage_num / NC->trackSizePage) + N->PM_Tracks_Use + 1) / NC->CylinderSizeTrack, 1);
//	qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//	if (merge_ram_size>1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
//	assert(0 <= (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack && (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack < N->Cylinders);
//	Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1);//電梯法回SLB讀資料
//	for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//		if (merge_ram[j - 1] / NC->trackSizePage != merge_ram[j] / NC->trackSizePage){
//			//seek
//			assert(0 <= (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack && (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1);
//		}
//	}
//	free(merge_ram);
//#endif
//	if (PMmerge != -1){
//		assert(N->blocks[dataBlock].Cut == 1);
//		if (PMmerge < NC->trackSizePage){
//			////if (N->PM_Tracks_Use > N->PM_Tracks_Threshold){ N->blocks[dataBlock].Cut = 0; N->PM_Tracks_Use--; assert(0 <= N->PM_Tracks_Use); }
//			PMmerge *= NC->trackSizePage;
//		}
//		SLB->pageRead += PMmerge; SLB->pageWrite += PMmerge;
//		NSLB->pageRead += PMmerge; NSLB->pageWrite += PMmerge;
//#ifdef Time_Overhead
//		//DB seek
//		for (j = N->pre_start; j < N->blocks[dataBlock].GuardTrackIndex; j++){
//			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//		}
//		for (j = N->pos_start; j < NC->blockSizeTrack; j++){
//			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//		}
//#endif
//	}
//	else{
//		SLB->pageRead += tmpSLB_Read; SLB->pageWrite += tmpSLB_Write;
//		NSLB->pageRead += tmpNSLB_Read; NSLB->pageWrite += tmpNSLB_Write;
//#ifdef Time_Overhead
//		//DB seek
//		for (j = new_start / NC->trackSizePage; j < NC->blockSizeTrack; j++){
//			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//		}
//#endif
//	}
//	return validIs2 + validIs3;
//}
//void FASTmergeNormalNSLB3(sim *simPtr)
//{
//	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	DWORD	CurrentSectorNo = 0;
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD validIs1 = 0, validIs2 = 0;
//	DWORD small = 0, goal = 0; //small time: means the band have the earliest time
//
//	I64 tmp_time;
//	DWORD BPLRUstartBand = 0;
//	DWORD tmp = 0;
//
//	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
//	DWORD SectorTableNo1 = 0;
//	DWORD block_no = 0, page_no = 0;
//	DWORD start = 0;
//	DWORD l = 0;
//	BYTE Dirty[256] = { 0 };
//	DWORD Dirty_Num = 0;
//
//	NSLB->mergecnt++;
//	do
//	{
//#ifdef BPLRULlist_RunLen
//		N->BPLRUcurrent_len = 1;
//#endif
//		small = 0;
//		while (N->BPLRU[small] == 0){ small++; }
//		assert(small <= NC->LsizeBlock - 1);
//		if (small != NC->LsizeBlock - 1){
//			BPLRUstartBand = small + 1;
//			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
//			{
//				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
//#ifdef BPLRULlist_RunLen
//				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
//#endif
//			}
//		}
//#ifdef BPLRULlist_RunLen
//		N->BPLRUtotal_len += N->BPLRUcurrent_len;
//		N->BPLRUtotal_count++;
//		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
//		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
//#endif
//		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
//		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
//#ifdef NEWGC64trackWRITE
//		DWORD TrackIndex = 0, GuardTrackIndex = -1;
//		DWORD pre_start, pos_start;
//		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
//		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
//#ifdef Band_64MB_8Track
//		assert(Mini_TrackWrite == 64);
//		DirtyTrack_Group = 8; TrackSearch_End = 61;
//#endif
//#ifdef Band_128MB_16Track
//		assert(Mini_TrackWrite == 128);
//		DirtyTrack_Group = 16; TrackSearch_End = 125;
//#endif
//#ifdef Band_256MB_32Track
//		assert(Mini_TrackWrite == 256);
//		DirtyTrack_Group = 32; TrackSearch_End = 253;
//#endif
//		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
//		for (k = 0; k < TrackSearch_End + 3; k++){
//			Dirty[k] = 0;
//			for (l = 0; l < NC->trackSizePage; l++){
//				if (N->blocks[dataBlock].pages[k*NC->trackSizePage + l].valid == 2){
//					Dirty[k] = 1; Dirty_Num++; break;
//				}
//			}
//		}
//		//預先計算PM寫多track
//		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
//			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
//			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
//			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			if (tmp_TrackWrite < Mini_TrackWrite){
//				Mini_Index = GuardTrackIndex;// TrackIndex;
//				Mini_TrackWrite = tmp_TrackWrite;
//				N->pre_start = pre_start; N->pos_start = pos_start;
//			}
//		}
//		N->All_Merge++;
//		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
//		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
//		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
//			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
//			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
//			N->pre_start = pre_start; N->pos_start = pos_start;
//		}
//		assert(Dirty_Num <= Mini_TrackWrite); N->Miss_Rel_len += Dirty_Num;
//		if ((Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1) || (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Tracks_Use < N->PM_Tracks)){
//			N->Allow_PM++; //N->DO_PM = 0; //N->PM_len = 0;
//			N->Miss_PM_len += Mini_TrackWrite;
//		}
//#endif
//		validIs2 += WriteDB_SE2(simPtr, -1, dataBlock);
//		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
//		assert(validIs2 > 0);
//	} while (validIs2 == 0);
//	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
//	//NSLB統計資料
//	//NSLB->writedPageNum -= validIs2;
//#ifdef MergeAssoc_NSLBpart
//	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
//#endif
//}
//
//void FASTmergeRWNSLBPM3(sim *simPtr)
//{
//	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	DWORD	CurrentSectorNo = 0;
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD validIs1 = 0, validIs2 = 0;
//	DWORD small = 0, goal = 0; //small time: means the band have the earliest time
//
//	I64 tmp_time;
//	DWORD BPLRUstartBand = 0;
//	DWORD tmp = 0;
//
//	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
//	DWORD SectorTableNo1 = 0;
//	DWORD block_no = 0, page_no = 0;
//	DWORD start = 0;
//	DWORD l = 0;
//	BYTE Dirty[256] = { 0 };
//	DWORD Dirty_Num = 0;
//
//	NSLB->mergecnt++;
//	do
//	{
//#ifdef BPLRULlist_RunLen
//		N->BPLRUcurrent_len = 1;
//#endif
//		small = 0;
//		while (N->BPLRU[small] == 0){ small++; }
//		assert(small <= NC->LsizeBlock - 1);
//		if (small != NC->LsizeBlock - 1){
//			BPLRUstartBand = small + 1;
//			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
//			{
//				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
//#ifdef BPLRULlist_RunLen
//				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
//#endif
//			}
//		}
//#ifdef BPLRULlist_RunLen
//		N->BPLRUtotal_len += N->BPLRUcurrent_len;
//		N->BPLRUtotal_count++;
//		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
//		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
//#endif
//		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
//		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
//#ifdef NEWGC64trackWRITE
//		DWORD TrackIndex = 0, GuardTrackIndex = -1;
//		DWORD pre_start, pos_start;
//		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
//		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
//#ifdef Band_64MB_8Track
//		assert(Mini_TrackWrite == 64);
//		DirtyTrack_Group = 8; TrackSearch_End = 61;
//#endif
//#ifdef Band_128MB_16Track
//		assert(Mini_TrackWrite == 128);
//		DirtyTrack_Group = 16; TrackSearch_End = 125;
//#endif
//#ifdef Band_256MB_32Track
//		assert(Mini_TrackWrite == 256);
//		DirtyTrack_Group = 32; TrackSearch_End = 253;
//#endif
//		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
//		for (k = 0; k < TrackSearch_End + 3; k++){
//			Dirty[k] = 0;
//			for (l = 0; l < NC->trackSizePage; l++){
//				if (N->blocks[dataBlock].pages[k*NC->trackSizePage + l].valid == 2){
//					Dirty[k] = 1; Dirty_Num++; break;
//				}
//			}
//		}
//		//預先計算PM寫多track
//		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
//			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
//			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
//			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			if (tmp_TrackWrite < Mini_TrackWrite){
//				Mini_Index = GuardTrackIndex;// TrackIndex;
//				Mini_TrackWrite = tmp_TrackWrite;
//				N->pre_start = pre_start; N->pos_start = pos_start;
//			}
//		}
//		N->All_Merge++;
//		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
//		assert((Mini_Index - N->pre_start + 1) + (TrackSearch_End + 3 - N->pos_start) == Mini_TrackWrite);
//		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
//		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
//			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
//			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
//			N->pre_start = pre_start; N->pos_start = pos_start;
//		}
//		assert(Dirty_Num <= Mini_TrackWrite); N->Rel_len += Dirty_Num;
//		
//		if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1){//切過的要再切
//			//只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
//			validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite, dataBlock);
//
//			N->PM_len += Mini_TrackWrite;
//			N->DO_PM++;
//
//			//if (N->blocks[dataBlock].GuardTrackIndex < Mini_Index){//換刀 //&& N->PM_Tracks_Use < N->PM_Tracks_Threshold
//			//	
//			//}
//
//			////紀錄GT sector，把DB上的GT設成valid=3 //GT右移
//			//for (k = 0; k < NC->trackSizePage; k++){
//			//	SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
//			//	assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
//			//	block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
//			//	assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
//			//	if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
//			//		assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1);
//			//		//N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid = 3;//只把valid1設成3
//			//		//可以PM的band的GT放入NSLB緩衝區
//			//		assert(N->PM_GTsector < N->PM_Tracks*NC->trackSizePage);
//			//		N->PMCandidate[N->PM_GTsector] = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;//PM GT sector valid 3
//			//		N->PM_GTsector++;
//			//		//N->PM_Sectors_Use++; assert(N->PM_Sectors_Use <= N->PM_Tracks*NC->trackSizePage);
//			//	}
//			//}
//			N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
//
//			//if (N->PM_Sectors_Use - N->OrgPM_Cost + NC->trackSizePage > N->PM_Tracks*NC->trackSizePage){//空間不足無法續切
//			//	if (N->blocks[dataBlock].Cut == 1){
//			//		N->blocks[dataBlock].Cut = 0; 
//			//	}
//			//	//補成一般merge
//			//	NSLB->pageRead += (TrackSearch_End + 3 - N->pos_start)*NC->trackSizePage; 
//			//	NSLB->pageWrite += (TrackSearch_End + 3 - N->pos_start)*NC->trackSizePage;
//			//}else{//續切
//			//	
//			//}
//		}
//		else if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Tracks_Use < N->PM_Tracks){//第一次切  (N->PM_Sectors_Use + NC->trackSizePage) <= N->PM_Tracks*NC->trackSizePage
//			//設定PM info
//			N->blocks[dataBlock].Cut = 1;
//			N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
//			N->PM_Tracks_Use++; assert(N->PM_Tracks_Use <= N->PM_Tracks);
//			//紀錄GT sector，把DB上的GT設成valid=3
//			for (k = 0; k < NC->trackSizePage; k++){
//				SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
//				assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
//				block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
//				assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
//				if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
//					assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1);
//					//N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid = 3;//只把valid1設成3
//					
//					//可以PM的band的GT放入NSLB緩衝區
//					assert(N->PM_GTsector < N->PM_Tracks*NC->trackSizePage);
//					N->PMCandidate[N->PM_GTsector] = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;//PM GT sector valid 3
//					N->PM_GTsector++;
//					NSLB->pageRead++; NSLB->pageWrite++; NSLB->write_count++;
//				}
//			}
//			//只讀寫PM預算的量 //SLB->pageWrite += (Mini_TrackWrite*NC->trackSizePage);
//			validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock);
//			N->PM_len += Mini_TrackWrite;
//			N->DO_PM++;
//		}
//		else{//歸還PM Track
//			if (N->blocks[dataBlock].Cut == 1){ 
//				N->blocks[dataBlock].Cut = 0; N->PM_Tracks_Use--; assert(0 <= N->PM_Tracks_Use);
//			}
//			validIs2 += WriteDB_SE2(simPtr, -1, dataBlock);
//		}
//#endif
//		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
//		assert(validIs2 > 0);
//	} while (validIs2 == 0);
//	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
//	//NSLB統計資料
//	//NSLB->writedPageNum -= validIs2;
//#ifdef MergeAssoc_NSLBpart
//	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
//#endif
//}


//DWORD WriteDB_SE2(sim *simPtr, DWORD PMmerge, DWORD BandNo)
//{
//	DWORD	RWLBA, RWLPA, newBlock, dataBlock, BlockNo, Victim, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD	SectorTableNo = 0, PageNo = 0;
//	DWORD	i = 0, j = 0;
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD start = 0, end = 0, new_start = 0;
//	DWORD block_no = 0, page_no = 0;
//
//	DWORD tmpSLB_Read = 0, tmpSLB_Write = 0, tmpNSLB_Read = 0, tmpNSLB_Write = 0;
//	DWORD SectorTableNo2 = 0;
//	DWORD NSLBfreeTrack = 0;
//	DWORD validIs2 = 0, validIs3 = 0, validIs0 = 0;
//
//	DWORD SLBfreeTrack = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
//	merge_ram_size = 0;
//#endif
//	DWORD SE_TrackWrite = 0;
//
//	Victim = NC->PsizeBlock - 1;
//	dataBlock = BandNo;
//	start = 0; end = NC->blockSizePage - 1;
//	//單向往後干擾，從頭開始找第一個invalid page往後面做GC。
//	while (N->blocks[dataBlock].pages[start].valid == 1){ start++; }
//
//	/*SE_TrackWrite = NC->blockSizeSector / NC->trackSizePage - start / NC->trackSizePage;
//	if (SE_TrackWrite == 0){ printf("%lu %lu\n", N->blocks[dataBlock].Cut, dataBlock); system("pause"); }
//	assert(SE_TrackWrite <= NC->blockSizeTrack);*/
//
//	assert(start <= NC->blockSizePage - 1); new_start = start;
//	for (j = new_start; j <= end; j++){ //start //sub-band的起點到終點
//		SectorTableNo = dataBlock*NC->blockSizePage + j;
//		block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//		if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
//			assert(block_no == dataBlock); assert(page_no == j);
//			tmpSLB_Read++; tmpSLB_Write++;
//		}
//		else if (N->blocks[dataBlock].pages[j].valid == 2){//clear the page in NSLB
//			assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//			NSLB->writedPageNum = NSLB->writedPageNum - 1;
//#ifdef Time_Overhead
//			merge_ram[merge_ram_size] = N->blocks[dataBlock].pages[j].sector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
//#endif
//			NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
//			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//			N->NSLBremainSectorTable[NSLBfreeTrack]++; assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);//
//			tmpNSLB_Read++; tmpNSLB_Write++; validIs2++;
//		}
//		else if (N->blocks[dataBlock].pages[j].valid == 0){ //和SLB的valid0不一樣，這邊用SLBtable取代
//			if (PMmerge == -1 || (0 < PMmerge && PMmerge < NC->trackSizePage)){
//				assert(block_no == dataBlock); assert(SLB->writedPageNum <= SLB->SLBpage_num);
//				SLB->writedPageNum = SLB->writedPageNum - 1;
//#ifdef Time_Overhead
//				merge_ram[merge_ram_size] = N->blocks[dataBlock].pages[j].sector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
//#endif
//				SLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
//				if (N->SLBremainSectorTable[SLBfreeTrack] == 0){ N->SLBexhaustedTrack--; }
//				N->SLBremainSectorTable[SLBfreeTrack]++; assert(0 < N->SLBremainSectorTable[SLBfreeTrack] && N->SLBremainSectorTable[SLBfreeTrack] <= NC->trackSizePage);
//				tmpNSLB_Read++; tmpNSLB_Write++;
//				validIs0++;
//
//				SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
//				setblock(simPtr, SectorTableNo, dataBlock);
//				setpage(simPtr, SectorTableNo, j);
//				assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
//				N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
//				N->blocks[dataBlock].pages[j].valid = 1; //new變valid
//				N->PM_Sectors_Use--;
//			}
//		}
//		else if (N->blocks[dataBlock].pages[j].valid == 3){//PM的GT在normal merge歸還
//			if (PMmerge == -1 || (0 < PMmerge && PMmerge < NC->trackSizePage)){
//				assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//				NSLB->writedPageNum = NSLB->writedPageNum - 1;
//#ifdef Time_Overhead
//				merge_ram[merge_ram_size] = N->blocks[dataBlock].pages[j].sector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
//#endif
//				NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
//				if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//				N->NSLBremainSectorTable[NSLBfreeTrack]++; assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);
//				tmpNSLB_Read++; tmpNSLB_Write++;
//				validIs3++;
//
//				SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
//				setblock(simPtr, SectorTableNo, dataBlock);
//				setpage(simPtr, SectorTableNo, j);
//				assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
//				N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
//				N->blocks[dataBlock].pages[j].valid = 1; //new變valid
//				N->PM_Sectors_Use--;
//			}
//		}
//
//		if (N->blocks[dataBlock].pages[j].valid != 0 || N->blocks[dataBlock].pages[j].valid != 3){
//			SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
//			setblock(simPtr, SectorTableNo, dataBlock);
//			setpage(simPtr, SectorTableNo, j);
//			assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
//			N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
//			N->blocks[dataBlock].pages[j].valid = 1; //new變valid
//		}
//	}
//
//	if (validIs2 == 0){ free(merge_ram); return 0; }
//#ifdef Time_Overhead
//	assert(merge_ram_size <= NC->blockSizeSector);
//	qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//	if (merge_ram_size>1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
//	assert(0 <= (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack && (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack < N->Cylinders);
//	Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1);//電梯法回SLB讀資料
//	for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//		if (merge_ram[j - 1] / NC->trackSizePage != merge_ram[j] / NC->trackSizePage){
//			//seek
//			assert(0 <= (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack && (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1);
//		}
//	}
//	free(merge_ram);
//#endif
//	assert(0 < tmpSLB_Write + tmpNSLB_Write && tmpSLB_Write + tmpNSLB_Write <= NC->blockSizeSector);
//	if (PMmerge != -1){
//		if (PMmerge < NC->trackSizePage){ PMmerge *= NC->trackSizePage; }
//		assert(N->blocks[dataBlock].Cut == 1);
//		SLB->pageRead += PMmerge; SLB->pageWrite += PMmerge;
//		NSLB->pageRead += PMmerge; NSLB->pageWrite += PMmerge; NSLB->merge_count += PMmerge;
//#ifdef Time_Overhead
//		//DB seek
//		for (j = N->pre_start; j < N->blocks[dataBlock].GuardTrackIndex; j++){
//			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//		}
//		for (j = N->pos_start; j < NC->blockSizeTrack; j++){
//			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//		}
//		//GT 寫入LB
//		//Calc_TimeOverhead(simPtr, (NC->LsizeSector + N->preNSLB_WriteSector )/ NC->trackSizePage / NC->CylinderSizeTrack, 1);
//#endif
//	}
//	else{
//		SLB->pageRead += tmpSLB_Read; SLB->pageWrite += tmpSLB_Write;
//		NSLB->pageRead += tmpNSLB_Read; NSLB->pageWrite += tmpNSLB_Write; NSLB->merge_count += tmpNSLB_Write;
//#ifdef Time_Overhead
//		//DB seek
//		for (j = new_start / NC->trackSizePage; j < NC->blockSizeTrack; j++){
//			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//		}
//#endif
//	}
//	return validIs2 + validIs3;//validIs2;// 
//}
//void FASTmergeNormalNSLB3(sim *simPtr)
//{
//	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	DWORD	CurrentSectorNo = 0;
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD validIs1 = 0, validIs2 = 0;
//	DWORD small = 0, goal = 0; //small time: means the band have the earliest time
//
//	I64 tmp_time;
//	DWORD BPLRUstartBand = 0;
//	DWORD tmp = 0;
//
//	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
//	DWORD SectorTableNo1 = 0;
//	DWORD block_no = 0, page_no = 0;
//	DWORD start = 0;
//	DWORD l = 0;
//	BYTE Dirty[256] = { 0 };
//	DWORD Dirty_Num = 0;
//
//	NSLB->mergecnt++;
//	do
//	{
//#ifdef BPLRULlist_RunLen
//		N->BPLRUcurrent_len = 1;
//#endif
//		small = 0;
//		while (N->BPLRU[small] == 0){ small++; }
//		assert(small <= NC->LsizeBlock - 1);
//		if (small != NC->LsizeBlock - 1){
//			BPLRUstartBand = small + 1;
//			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
//			{
//				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
//#ifdef BPLRULlist_RunLen
//				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
//#endif
//			}
//		}
//#ifdef BPLRULlist_RunLen
//		N->BPLRUtotal_len += N->BPLRUcurrent_len;
//		N->BPLRUtotal_count++;
//		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
//		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
//#endif
//		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
//		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
//#ifdef NEWGC64trackWRITE
//		DWORD TrackIndex = 0, GuardTrackIndex = -1;
//		DWORD pre_start, pos_start;
//		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
//		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
//#ifdef Band_64MB_8Track
//		assert(Mini_TrackWrite == 64);
//		DirtyTrack_Group = 8; TrackSearch_End = 61;
//#endif
//#ifdef Band_128MB_16Track
//		assert(Mini_TrackWrite == 128);
//		DirtyTrack_Group = 16; TrackSearch_End = 125;
//#endif
//#ifdef Band_256MB_32Track
//		assert(Mini_TrackWrite == 256);
//		DirtyTrack_Group = 32; TrackSearch_End = 253;
//#endif
//		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
//		for (k = 0; k < TrackSearch_End + 3; k++){
//			Dirty[k] = 0;
//			for (l = 0; l < NC->trackSizePage; l++){
//				if (N->blocks[dataBlock].pages[k*NC->trackSizePage + l].valid == 2){
//					Dirty[k] = 1; Dirty_Num++; break;
//				}
//			}
//		}
//		//預先計算PM寫多track
//		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
//			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
//			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
//			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			if (tmp_TrackWrite < Mini_TrackWrite){
//				Mini_Index = GuardTrackIndex;// TrackIndex;
//				Mini_TrackWrite = tmp_TrackWrite;
//				N->pre_start = pre_start; N->pos_start = pos_start;
//			}
//		}
//		N->All_Merge++;
//		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
//		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
//		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
//			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
//			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
//			N->pre_start = pre_start; N->pos_start = pos_start;
//		}
//		assert(Dirty_Num <= Mini_TrackWrite); N->Miss_Rel_len += Dirty_Num;
//		if ((Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1) || (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Tracks_Use < N->PM_Tracks)){
//			N->Allow_PM++; //N->DO_PM = 0; //N->PM_len = 0;
//			N->Miss_PM_len += Mini_TrackWrite;
//		}
//#endif
//		//validIs2 += WriteDB_SE3(simPtr, Mini_TrackWrite, Mini_Index, dataBlock);
//		validIs2 += WriteDB_SE2(simPtr, -1, dataBlock);
//		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
//		assert(validIs2 >= 0);
//	} while (validIs2 == 0);
//	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
//	//NSLB統計資料
//	//NSLB->writedPageNum -= validIs2;
//#ifdef MergeAssoc_NSLBpart
//	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
//#endif
//}
//
//void FASTmergeRWNSLBPM3(sim *simPtr)
//{
//	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	DWORD	CurrentSectorNo = 0;
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD validIs1 = 0, validIs2 = 0;
//	DWORD small = 0, goal = 0; //small time: means the band have the earliest time
//
//	I64 tmp_time;
//	DWORD BPLRUstartBand = 0;
//	DWORD tmp = 0;
//
//	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
//	DWORD SectorTableNo1 = 0;
//	DWORD block_no = 0, page_no = 0;
//	DWORD start = 0;
//	DWORD l = 0;
//	BYTE Dirty[256] = { 0 };
//	DWORD Dirty_Num = 0;
//
//	NSLB->mergecnt++;
//	do
//	{
//#ifdef BPLRULlist_RunLen
//		N->BPLRUcurrent_len = 1;
//#endif
//		small = 0;
//		while (N->BPLRU[small] == 0){ small++; }
//		assert(small <= NC->LsizeBlock - 1);
//		if (small != NC->LsizeBlock - 1){
//			BPLRUstartBand = small + 1;
//			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
//			{
//				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
//#ifdef BPLRULlist_RunLen
//				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
//#endif
//			}
//		}
//#ifdef BPLRULlist_RunLen
//		N->BPLRUtotal_len += N->BPLRUcurrent_len;
//		N->BPLRUtotal_count++;
//		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
//		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
//#endif
//		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
//		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
//#ifdef NEWGC64trackWRITE
//		DWORD TrackIndex = 0, GuardTrackIndex = -1;
//		DWORD pre_start, pos_start;
//		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
//		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
//#ifdef Band_64MB_8Track
//		assert(Mini_TrackWrite == 64);
//		DirtyTrack_Group = 8; TrackSearch_End = 61;
//#endif
//#ifdef Band_128MB_16Track
//		assert(Mini_TrackWrite == 128);
//		DirtyTrack_Group = 16; TrackSearch_End = 125;
//#endif
//#ifdef Band_256MB_32Track
//		assert(Mini_TrackWrite == 256);
//		DirtyTrack_Group = 32; TrackSearch_End = 253;
//#endif
//		for (k = 0; k < TrackSearch_End + 3; k++){
//			Dirty[k] = 0;
//			for (l = 0; l < NC->trackSizePage; l++){
//				if (N->blocks[dataBlock].pages[k*NC->trackSizePage + l].valid == 2){
//					Dirty[k] = 1; Dirty_Num++; break;
//				}
//			}
//		}
//		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
//		//預先計算PM寫多track
//		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
//			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
//			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
//			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			if (tmp_TrackWrite < Mini_TrackWrite){
//				Mini_Index = GuardTrackIndex;// TrackIndex;
//				Mini_TrackWrite = tmp_TrackWrite;
//				N->pre_start = pre_start; N->pos_start = pos_start;
//			}
//		}
//		N->All_Merge++;
//		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
//		assert((Mini_Index - N->pre_start + 1) + (TrackSearch_End + 3 - N->pos_start) == Mini_TrackWrite);
//		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
//		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
//			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
//			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
//			N->pre_start = pre_start; N->pos_start = pos_start;
//		}
//		assert(Dirty_Num <= Mini_TrackWrite); N->Rel_len += Dirty_Num;
//		
//		if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1){//切過的要再切 //&& Mini_TrackWrite < SE_TrackWrite
//			if (N->blocks[dataBlock].GuardTrackIndex < Mini_Index){//換刀
//				//只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
//				validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite, dataBlock); //assert(validIs2 > 0);
//
//				N->PM_Tracks_Allocated2Use_Condition = 1;//續切做換刀
//				N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
//
//				//紀錄GT sector，把DB上的GT設成valid=3
//				for (k = 0; k < NC->trackSizePage; k++){
//					SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
//					assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
//					block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
//					assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
//					assert(1 <= N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid && N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid <= 3);
//					if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
//						assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1);
//						if (N->PM_Sectors_Use < N->PM_Tracks*NC->trackSizePage){//可以PM的band的GT放入NSLB緩衝區
//							assert(N->PM_GTsector < N->PM_Tracks*NC->trackSizePage);
//							N->PMCandidate[N->PM_GTsector] = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;//PM GT sector valid 3
//							N->PM_GTsector++;
//							NSLB->pageRead++; NSLB->pageWrite++; NSLB->write_count++;
//							N->PM_Sectors_Use++;
//						}
//					}
//				}
//			}else{
//				//只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
//				validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock); //assert(validIs2 > 0);
//
//				N->PM_Tracks_Allocated2Use_Condition = 1;//沒換刀
//				//Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
//			}
//			N->PM_len += Mini_TrackWrite;
//			N->DO_PM++;
//		}else if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Sectors_Use + NC->trackSizePage <= N->PM_Tracks*NC->trackSizePage){//第一次切   && Mini_TrackWrite < SE_TrackWrite  N->PM_Tracks_Use < N->PM_Tracks 
//			N->PM_Tracks_Allocated2Use_Condition = 1;
//			//設定PM info
//			N->blocks[dataBlock].Cut = 1;
//			N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
//			
//			//只讀寫PM預算的量 //SLB->pageWrite += (Mini_TrackWrite*NC->trackSizePage);
//			validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock); assert(validIs2 > 0);
//			N->PM_len += Mini_TrackWrite;
//			N->DO_PM++;
//
//			//紀錄GT sector，把DB上的GT設成valid=3
//			for (k = 0; k < NC->trackSizePage; k++){
//				SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
//				assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
//				block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
//				assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
//				assert(1 <= N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid && N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid <= 3);
//				if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
//					assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1); //N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid = 3;//只把valid1設成3
//					//可以PM的band的GT放入NSLB緩衝區
//					assert(N->PM_GTsector < N->PM_Tracks*NC->trackSizePage);
//					N->PMCandidate[N->PM_GTsector] = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;//PM GT sector valid 3
//					N->PM_GTsector++;
//					NSLB->pageRead++; NSLB->pageWrite++; NSLB->write_count++;
//					N->PM_Sectors_Use++;
//				}
//			}
//		}
//		else{//歸還PM Track
//			if (N->blocks[dataBlock].Cut == 1){
//				N->blocks[dataBlock].Cut = 0; N->PM_Tracks_Use--; assert(0 <= N->PM_Tracks_Use);
//			}
//			validIs2 += WriteDB_SE2(simPtr, -1, dataBlock); assert(validIs2 > 0);
//		}
//#endif
//		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
//		assert(validIs2 >= 0);
//	} while (validIs2 == 0);
//	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
//	//NSLB統計資料
//	//NSLB->writedPageNum -= validIs2;
//#ifdef MergeAssoc_NSLBpart
//	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
//#endif
//}

////對整個band一次性RMW的merge
//DWORD WriteDB_SE2(sim *simPtr, DWORD PMmerge, DWORD BandNo)
//{
//	DWORD	RWLBA, RWLPA, newBlock, dataBlock, BlockNo, Victim, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD	SectorTableNo = 0, PageNo = 0;
//	DWORD	i = 0, j = 0;
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD start = 0, end = 0, new_start = 0;
//	DWORD block_no = 0, page_no = 0;
//
//	DWORD tmpSLB_Read = 0, tmpSLB_Write = 0, tmpNSLB_Read = 0, tmpNSLB_Write = 0;
//	DWORD SectorTableNo2 = 0;
//	DWORD NSLBfreeTrack = 0;
//	DWORD validIs2 = 0, validIs3 = 0, validIs0 = 0;
//
//	DWORD SLBfreeTrack = 0;
//#ifdef Time_Overhead
//	DWORD merge_ram_size = 0;
//	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
//	merge_ram_size = 0;
//#endif
//	//DWORD SE_TrackWrite = 0;
//
//#ifdef Time_Overhead
//	if (N->WriteBuff_Size > 0){
//		SLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
//	}
//#endif
//	Victim = NC->PsizeBlock - 1;
//	dataBlock = BandNo;
//	start = 0; end = NC->blockSizePage - 1;
//	//單向往後干擾，從頭開始找第一個invalid page往後面做GC。
//	while (N->blocks[dataBlock].pages[start].valid == 1){ start++; }
//
//	/*SE_TrackWrite = NC->blockSizeSector / NC->trackSizePage - start / NC->trackSizePage;
//	if (SE_TrackWrite == 0){ printf("%lu %lu\n", N->blocks[dataBlock].Cut, dataBlock); system("pause"); }
//	assert(SE_TrackWrite <= NC->blockSizeTrack);*/
//
//	assert(start <= NC->blockSizePage - 1); new_start = start;
//	for (j = new_start; j <= end; j++){ //start //sub-band的起點到終點
//		SectorTableNo = dataBlock*NC->blockSizePage + j;
//		block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//		if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
//			assert(block_no == dataBlock); assert(page_no == j);
//			tmpSLB_Read++; tmpSLB_Write++;
//		}
//		else if (N->blocks[dataBlock].pages[j].valid == 2){//clear the page in NSLB
//			assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//			NSLB->writedPageNum = NSLB->writedPageNum - 1;
//#ifdef Time_Overhead
//			merge_ram[merge_ram_size] = N->blocks[dataBlock].pages[j].sector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
//#endif
//			NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
//			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//			N->NSLBremainSectorTable[NSLBfreeTrack]++; assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);//
//			tmpNSLB_Read++; tmpNSLB_Write++; validIs2++;
//		}
//		else if (N->blocks[dataBlock].pages[j].valid == 0){ //和SLB的valid0不一樣，這邊用SLBtable取代
//			if (PMmerge == -1){ //   || (0 < PMmerge && PMmerge < NC->trackSizePage)
//				assert(block_no == dataBlock); assert(SLB->writedPageNum <= SLB->SLBpage_num);
//				SLB->writedPageNum = SLB->writedPageNum - 1;
//#ifdef Time_Overhead
//				merge_ram[merge_ram_size] = N->blocks[dataBlock].pages[j].sector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
//#endif
//				SLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
//				if (N->SLBremainSectorTable[SLBfreeTrack] == 0){ N->SLBexhaustedTrack--; }
//				N->SLBremainSectorTable[SLBfreeTrack]++; assert(0 < N->SLBremainSectorTable[SLBfreeTrack] && N->SLBremainSectorTable[SLBfreeTrack] <= NC->trackSizePage);
//				tmpNSLB_Read++; tmpNSLB_Write++;
//				validIs0++;
//
//				SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
//				setblock(simPtr, SectorTableNo, dataBlock);
//				setpage(simPtr, SectorTableNo, j);
//				assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
//				N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
//				N->blocks[dataBlock].pages[j].valid = 1; //new變valid
//				N->PM_Sectors_Use--;
//			}
//		}
//		else if (N->blocks[dataBlock].pages[j].valid == 3){//PM的GT在normal merge歸還
//			//printf("merge valid 3, error\n"); system("pause");
//			if (PMmerge == -1){ //  || (0 < PMmerge && PMmerge < NC->trackSizePage)
//				assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//				NSLB->writedPageNum = NSLB->writedPageNum - 1;
//#ifdef Time_Overhead
//				merge_ram[merge_ram_size] = N->blocks[dataBlock].pages[j].sector; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
//#endif
//				NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
//				if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//				N->NSLBremainSectorTable[NSLBfreeTrack]++; assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);
//				tmpNSLB_Read++; tmpNSLB_Write++;
//				validIs3++;
//
//				SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
//				setblock(simPtr, SectorTableNo, dataBlock);
//				setpage(simPtr, SectorTableNo, j);
//				assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
//				N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
//				N->blocks[dataBlock].pages[j].valid = 1; //new變valid
//				N->PM_Sectors_Use--;
//			}
//		}
//
//		if (N->blocks[dataBlock].pages[j].valid != 0 || N->blocks[dataBlock].pages[j].valid != 3){ // 
//			SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
//			setblock(simPtr, SectorTableNo, dataBlock);
//			setpage(simPtr, SectorTableNo, j);
//			assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
//			N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
//			N->blocks[dataBlock].pages[j].valid = 1; //new變valid
//		}
//
//
//	}
//	if (validIs2 + validIs3 == 0){ free(merge_ram); return 0; }
//#ifdef Time_Overhead
//	if (N->WriteBuff_Size > 0){
//		SLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
//	}
//
//	assert(merge_ram_size <= NC->blockSizeSector);
//	qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
//	if (merge_ram_size>1){ for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); } }
//	assert(0 <= (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack && (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack < N->Cylinders);
//	Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1);//電梯法回SLB讀資料
//	for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//		if (merge_ram[j - 1] / NC->trackSizePage != merge_ram[j] / NC->trackSizePage){
//			//seek
//			assert(0 <= (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack && (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 1);
//		}
//	}
//	free(merge_ram);
//#endif
//	assert(0 < tmpSLB_Write + tmpNSLB_Write && tmpSLB_Write + tmpNSLB_Write <= NC->blockSizeSector);
//	if (PMmerge != -1){
//		if (PMmerge < NC->trackSizePage){ PMmerge *= NC->trackSizePage; }
//		assert(N->blocks[dataBlock].Cut == 1);
//		SLB->pageRead += PMmerge; SLB->pageWrite += PMmerge;
//		NSLB->pageRead += PMmerge; NSLB->pageWrite += PMmerge; NSLB->merge_count += PMmerge;
//#ifdef Time_Overhead
//		//DB seek
//		for (j = N->pre_start; j <= N->blocks[dataBlock].GuardTrackIndex; j++){//merge時順便把PMGT讀出來，等一下在FASTwriteSectorNSLBPMRAM3_STM寫入NSLB
//			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//		}
//		for (j = N->pos_start; j < NC->blockSizeTrack; j++){
//			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//		}
//		//GT 寫入LB
//		//Calc_TimeOverhead(simPtr, (NC->LsizeSector + N->preNSLB_WriteSector )/ NC->trackSizePage / NC->CylinderSizeTrack, 1);
//#endif
//	}
//	else{
//		SLB->pageRead += tmpSLB_Read; SLB->pageWrite += tmpSLB_Write;
//		NSLB->pageRead += tmpNSLB_Read; NSLB->pageWrite += tmpNSLB_Write; NSLB->merge_count += tmpNSLB_Write;
//#ifdef Time_Overhead
//		//DB seek
//		for (j = new_start / NC->trackSizePage; j < NC->blockSizeTrack; j++){
//			SectorTableNo2 = dataBlock*NC->blockSizePage + j*NC->trackSizePage; assert(0 <= SectorTableNo2 && SectorTableNo2 < NC->LsizePage);
//			assert(0 <= SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack && SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, SectorTableNo2 / NC->trackSizePage / NC->CylinderSizeTrack, 1);//寫在DB上
//		}
//#endif
//	}
//	return validIs2 + validIs3;//validIs2;// 
//}

DWORD WriteDB_SE2(sim *simPtr, DWORD PMmerge, DWORD BandNo)
{
	DWORD	RWLBA, RWLPA, newBlock, dataBlock, BlockNo, Victim, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD	SectorTableNo = 0, PageNo = 0;
	DWORD	i = 0, j = 0, k = 0;
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD start = 0, end = 0, new_start = 0;
	DWORD block_no = 0, page_no = 0;

	DWORD tmpSLB_Read = 0, tmpSLB_Write = 0, tmpNSLB_Read = 0, tmpNSLB_Write = 0;
	DWORD SectorTableNo2 = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD validIs2 = 0, validIs3 = 0, validIs0 = 0;

	DWORD SLBfreeTrack = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
	merge_ram_size = 0;
#endif
	//DWORD SE_TrackWrite = 0;

#ifdef Time_Overhead
	if (N->WriteBuff_Size > 0){
		NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
	}
#endif
	Victim = NC->PsizeBlock - 1;
	dataBlock = BandNo;
	start = 0; end = NC->blockSizePage - 1;
	//單向往後干擾，從頭開始找第一個invalid page往後面做GC。
	while (N->blocks[dataBlock].pages[start].valid == 1){ start++; }

	assert(start < NC->blockSizePage - 1); new_start = start / NC->trackSizePage * NC->trackSizePage;

	//NewPM
	/*if (N->blocks[dataBlock].NewPM == 1){
	while (N->blocks[dataBlock].pages[end].valid == 1){ end--; }
	assert(start < end);
	}*/

	for (j = new_start; j <= end; j++){ //start //sub-band的起點到終點 
		SectorTableNo = dataBlock*NC->blockSizePage + j;
		block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
		if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
			assert(block_no == dataBlock); assert(page_no == j);
			tmpSLB_Read++; tmpSLB_Write++;
		}
		else if (N->blocks[dataBlock].pages[j].valid == 2){//clear the page in NSLB
			assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
			NSLB->writedPageNum = NSLB->writedPageNum - 1;
			NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
#ifdef Time_Overhead
			merge_ram[merge_ram_size] = NSLBfreeTrack * 2 * NC->trackSizePage; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
#endif
			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
			N->NSLBremainSectorTable[NSLBfreeTrack]++; assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);//
			tmpNSLB_Read++; tmpNSLB_Write++; validIs2++;
		}
		else if (N->blocks[dataBlock].pages[j].valid == 0){ //和SLB的valid0不一樣，這邊用SLBtable取代
			if (PMmerge == -1){ //   || (0 < PMmerge && PMmerge < NC->trackSizePage)
				assert(block_no == dataBlock); assert(SLB->writedPageNum <= SLB->SLBpage_num);
				SLB->writedPageNum = SLB->writedPageNum - 1;
				SLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = SLBfreeTrack * NC->trackSizePage; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
#endif
				if (N->SLBremainSectorTable[SLBfreeTrack] == 0){ N->SLBexhaustedTrack--; }
				N->SLBremainSectorTable[SLBfreeTrack]++; assert(0 < N->SLBremainSectorTable[SLBfreeTrack] && N->SLBremainSectorTable[SLBfreeTrack] <= NC->trackSizePage);
				tmpNSLB_Read++; tmpNSLB_Write++;
				validIs0++;

				SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
				setblock(simPtr, SectorTableNo, dataBlock);
				setpage(simPtr, SectorTableNo, j);
				assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
				N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
				N->blocks[dataBlock].pages[j].valid = 1; //new變valid
				N->PM_Sectors_Use--;
			}
		}
		else if (N->blocks[dataBlock].pages[j].valid == 3){//PM的GT在normal merge歸還
			if (PMmerge == -1){ //  || (0 < PMmerge && PMmerge < NC->trackSizePage)
				assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
				NSLB->writedPageNum = NSLB->writedPageNum - 1;
				NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = NSLBfreeTrack * 2 * NC->trackSizePage; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
#endif
				if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectorTable[NSLBfreeTrack]++; assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);
				tmpNSLB_Read++; tmpNSLB_Write++;
				validIs3++;

				SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
				setblock(simPtr, SectorTableNo, dataBlock);
				setpage(simPtr, SectorTableNo, j);
				assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
				N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
				N->blocks[dataBlock].pages[j].valid = 1; //new變valid
				N->PM_Sectors_Use--;
			}
		}

		if (N->blocks[dataBlock].pages[j].valid != 0 || N->blocks[dataBlock].pages[j].valid != 3){ // 
			SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
			setblock(simPtr, SectorTableNo, dataBlock);
			setpage(simPtr, SectorTableNo, j);
			assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
			N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
			N->blocks[dataBlock].pages[j].valid = 1; //new變valid
		}
		//#ifdef Time_Overhead
		//		if (j / NC->trackSizePage != (j + 1) / NC->trackSizePage){
		//			if (merge_ram_size > 0){
		//				qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		//				if (merge_ram_size>1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }
		//				Calc_TimeOverhead2(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1), 3);//電梯法回SLB讀資料 //   / NC->CylinderSizeTrack
		//				N->Merge++;
		//				for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
		//					if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){ //seek
		//						Calc_TimeOverhead2(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[k] / NC->trackSizePage) + 1), 3);//   / NC->CylinderSizeTrack
		//						N->Merge++;
		//					}
		//				}
		//				merge_ram_size = 0;
		//			}
		//			Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 3);
		//			N->Merge++;
		//		}
		//#endif
	}

	if (validIs2 + validIs3 == 0){ free(merge_ram); return 0; }
#ifdef Time_Overhead
	if (merge_ram_size > 0){
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }
		Calc_TimeOverhead2(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1), 3);//電梯法回SLB讀資料 //   / NC->CylinderSizeTrack
		N->Merge++;
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
			if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){ //seek
				Calc_TimeOverhead2(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[k] / NC->trackSizePage) + 1), 3);//   / NC->CylinderSizeTrack
				N->Merge++;
			}
		}
	}
	merge_ram_size = 0;
	free(merge_ram);
#endif
	assert(0 < tmpSLB_Write + tmpNSLB_Write && tmpSLB_Write + tmpNSLB_Write <= NC->blockSizeSector);
	if (PMmerge != -1){
		if (PMmerge < NC->trackSizePage){ PMmerge *= NC->trackSizePage; }
		assert(N->blocks[dataBlock].Cut == 1);
		SLB->pageRead += PMmerge; SLB->pageWrite += PMmerge;
		NSLB->pageRead += PMmerge; NSLB->pageWrite += PMmerge; NSLB->merge_count += PMmerge;
#ifdef Time_Overhead
		for (k = N->pre_start; k <= N->blocks[dataBlock].GuardTrackIndex; k++){
			SectorTableNo = dataBlock*NC->blockSizePage + k * NC->trackSizePage; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
			Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 3);//寫在DB上   / NC->CylinderSizeTrack
		}
		for (N->pos_start; k <= NC->blockSizeTrack; k++){
			SectorTableNo = dataBlock*NC->blockSizePage + k * NC->trackSizePage; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
			Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 3);//寫在DB上   / NC->CylinderSizeTrack
		}
#endif
	}
	else{
		SLB->pageRead += tmpSLB_Read; NSLB->pageRead += tmpNSLB_Read;
		SLB->pageWrite += tmpSLB_Write; NSLB->pageWrite += tmpNSLB_Write;
		NSLB->merge_count += tmpNSLB_Write;
		//NewPM
		/*if (N->blocks[dataBlock].NewPM == 1){ end = end / NC->trackSizePage; }
		else{ end = NC->blockSizePage; }*/

#ifdef Time_Overhead
		for (k = new_start; k < NC->blockSizePage; k += NC->trackSizePage){//end
			SectorTableNo = dataBlock*NC->blockSizePage + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
			Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 3);//寫在DB上   / NC->CylinderSizeTrack
		}
#endif
	}
	return validIs2 + validIs3;//validIs2;// 
}

void FASTmergeNormalNSLB3(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;

	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD tmp = 0;

	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
	DWORD SectorTableNo1 = 0;
	DWORD block_no = 0, page_no = 0;
	DWORD l = 0;
	BYTE Dirty[256] = { 0 };
	DWORD Dirty_Num = 0;

	//試驗
	DWORD tmpSLB_Read = 0, tmpSLB_Write = 0, tmpNSLB_Read = 0, tmpNSLB_Write = 0;
	DWORD start = 0, end = 0, new_start = 0;
	DWORD NSLBfreeTrack = 0, SLBfreeTrack = 0;
	DWORD validIs0 = 0, validIs1 = 0, validIs2 = 0, validIs3 = 0;
	DWORD t_validIs2 = 0, t_validIs3 = 0;
#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
	merge_ram_size = 0;
	DWORD victim_size = 0;
	DWORD *victim = (DWORD*)calloc(NC->trackSizePage, sizeof(DWORD));
	DWORD PMmerge = 0;
#endif

	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(tmp_time > 0);
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
#ifdef NEWGC64trackWRITE
		DWORD TrackIndex = 0, GuardTrackIndex = -1;
		DWORD pre_start, pos_start;
		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
#ifdef Band_64MB_8Track
		assert(Mini_TrackWrite == 64);
		DirtyTrack_Group = 8; TrackSearch_End = 61;
#endif
#ifdef Band_128MB_16Track
		assert(Mini_TrackWrite == 128);
		DirtyTrack_Group = 16; TrackSearch_End = 125;
#endif
#ifdef Band_256MB_32Track
		assert(Mini_TrackWrite == 256);
		DirtyTrack_Group = 32; TrackSearch_End = 253;
#endif
		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
		for (k = 0; k < TrackSearch_End + 3; k++){
			Dirty[k] = 0;
			for (l = 0; l < NC->trackSizePage; l++){
				if (N->blocks[dataBlock].pages[k*NC->trackSizePage + l].valid == 2){
					Dirty[k] = 1; Dirty_Num++; break;
				}
			}
		}
		//預先計算PM寫多track
		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			if (tmp_TrackWrite < Mini_TrackWrite){
				Mini_Index = GuardTrackIndex;// TrackIndex;
				Mini_TrackWrite = tmp_TrackWrite;
				N->pre_start = pre_start; N->pos_start = pos_start;
			}
		}
		N->All_Merge++;
		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
			N->pre_start = pre_start; N->pos_start = pos_start;
		}
		assert(Dirty_Num <= Mini_TrackWrite); N->Miss_Rel_len += Dirty_Num;
		if ((Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1) || (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Tracks_Use < N->PM_Tracks)){
			N->Allow_PM++; //N->DO_PM = 0; //N->PM_len = 0;
			N->Miss_PM_len += Mini_TrackWrite;
		}
#endif

		//validIs2 += WriteDB_SE2(simPtr, -1, dataBlock);
		//N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU

		//DWORD track_validIs2 = 0;
		start = 0; end = NC->blockSizePage - 1;
		//單向往後干擾，從頭開始找第一個invalid page往後面做GC。
		while (N->blocks[dataBlock].pages[start].valid == 1){ start++; }
		assert(start < NC->blockSizePage - 1); new_start = start / NC->trackSizePage * NC->trackSizePage;
		t_validIs2 = 0;
		//fprintf(fPM_content, "%5lu", dataBlock); assert(0 <= dataBlock && dataBlock < NC->LsizeBlock);
		for (j = new_start; j <= end; j++){ //start //sub-band的起點到終點 

			//if (j % NC->trackSizePage==0){ track_validIs2 = 0; }

			SectorTableNo = dataBlock*NC->blockSizePage + j;
			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
			if (N->blocks[dataBlock].pages[j].valid == 1) { //原本就在data block裡的valid page
				assert(block_no == dataBlock); assert(page_no == j);
				tmpSLB_Read++; tmpSLB_Write++;
			}
			else if (N->blocks[dataBlock].pages[j].valid == 2){//clear the page in NSLB
				validIs2++;
				assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
				NSLB->writedPageNum = NSLB->writedPageNum - 1;
				NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = NSLBfreeTrack * 2 * NC->trackSizePage; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
#endif
				if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectorTable[NSLBfreeTrack]++; assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);//
				tmpNSLB_Read++; tmpNSLB_Write++; t_validIs2++;
				//track_validIs2++;
			}
			else if (N->blocks[dataBlock].pages[j].valid == 0){ //和SLB的valid0不一樣，這邊用SLBtable取代
				/*assert(N->blocks[dataBlock].Cut == 1);
				assert(block_no == dataBlock); assert(SLB->writedPageNum <= SLB->SLBpage_num);
				SLB->writedPageNum = SLB->writedPageNum - 1;
				SLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
				#ifdef Time_Overhead
				merge_ram[merge_ram_size] = SLBfreeTrack * NC->trackSizePage; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
				#endif
				if (N->SLBremainSectorTable[SLBfreeTrack] == 0){ N->SLBexhaustedTrack--; }
				N->SLBremainSectorTable[SLBfreeTrack]++; assert(0 < N->SLBremainSectorTable[SLBfreeTrack] && N->SLBremainSectorTable[SLBfreeTrack] <= NC->trackSizePage);
				tmpNSLB_Read++; tmpNSLB_Write++;
				validIs0++;
				N->PM_Sectors_Use--;*/
			}
			else if (N->blocks[dataBlock].pages[j].valid == 3){//PM的GT在normal merge歸還
				/*assert(N->blocks[dataBlock].Cut == 1);
				assert(block_no == dataBlock); assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
				NSLB->writedPageNum = NSLB->writedPageNum - 1;
				NSLBfreeTrack = N->blocks[dataBlock].pages[j].sector / NC->trackSizePage;
				#ifdef Time_Overhead
				merge_ram[merge_ram_size] = NSLBfreeTrack * 2 * NC->trackSizePage; merge_ram_size++; assert(merge_ram_size <= NC->blockSizeSector);
				#endif
				if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectorTable[NSLBfreeTrack]++; assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);
				tmpNSLB_Read++; tmpNSLB_Write++;
				validIs3++; t_validIs3++;
				N->PM_Sectors_Use--;*/
			}

			SectorTableNo = dataBlock*NC->blockSizePage + j; assert(SectorTableNo < NC->LsizePage);
			setblock(simPtr, SectorTableNo, dataBlock);
			setpage(simPtr, SectorTableNo, j);
			assert(dataBlock*NC->blockSizeSector + j*NC->pageSizeSector < NC->LsizePage);
			N->blocks[dataBlock].pages[j].sector = dataBlock*NC->blockSizeSector + j;	//new
			N->blocks[dataBlock].pages[j].valid = 1; //new變valid

			//if ((j + 1) % NC->trackSizePage == 0){ fprintf(fPM_content, " %lu", track_validIs2); }
		}
		//fprintf(fPM_content, "\n");

		//normal merge不干涉切，不修改N->blocks[dataBlock].Cut
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		if (N->blocks[dataBlock].Cut == 1){ PMmerge += Mini_TrackWrite*NC->trackSizePage; }
		else{ N->blocks[dataBlock].GuardTrackIndex = new_start / NC->trackSizePage; assert(N->blocks[dataBlock].GuardTrackIndex == new_start / NC->trackSizePage); }

		if (t_validIs2 > 0){
			//NSLB->mergecnt++;
			victim[victim_size] = dataBlock; victim_size++; assert(victim_size <= NC->trackSizePage);
			if (N->blocks[dataBlock].Cut == 1){
				N->blocks[dataBlock].PreIndex = N->pre_start;
				N->blocks[dataBlock].PosIndex = N->pos_start;
			}
			else{ N->blocks[dataBlock].PreIndex = new_start / NC->trackSizePage; }
		}
		assert(validIs2 >= 0); // + validIs3
	} while (NSLB->writedPageNum + N->PM_GTsector / 2 >= NSLB->NSLBpage_num);//              validIs2 == 0       
	//assert(validIs2 > 0);//     + validIs3
	if (validIs2 <= 0){ printf("FASTmergeNormalNSLB3 %lu %lu %lu %lu\n", validIs2, NSLB->writedPageNum, N->PM_GTsector, NSLB->NSLBpage_num); /*system("pause");*/ }

#ifdef Time_Overhead
	if (merge_ram_size > 0){
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		if (merge_ram_size > 1){ for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); } }
		Calc_TimeOverhead2(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1), 3);//電梯法回SLB讀資料 //   / NC->CylinderSizeTrack
		N->Merge++;
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
			if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){ //seek
				Calc_TimeOverhead2(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[k] / NC->trackSizePage) + 1), 3);//   / NC->CylinderSizeTrack
				N->Merge++;
			}
		}
		merge_ram_size = 0;
	}
	qsort(victim, victim_size, sizeof(DWORD), compare);
	if (victim_size > 1){ for (i = 1; i < victim_size; i++){ assert(victim[i - 1] <= victim[i]); } }
	NSLB->pageRead += PMmerge; NSLB->pageWrite += PMmerge;
	NSLB->mergecnt += victim_size;
	for (i = 0; i < victim_size; i++){
		dataBlock = victim[i];
		if (N->blocks[dataBlock].Cut == 1){
			for (k = N->blocks[dataBlock].PreIndex; k <= N->blocks[dataBlock].GuardTrackIndex; k++){
				SectorTableNo = dataBlock*NC->blockSizePage + k * NC->trackSizePage; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
				Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 3);//寫在DB上   / NC->CylinderSizeTrack
			}
			for (k = N->blocks[dataBlock].PosIndex; k <= NC->blockSizeTrack; k++){
				SectorTableNo = dataBlock*NC->blockSizePage + k * NC->trackSizePage; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
				Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 3);//寫在DB上   / NC->CylinderSizeTrack
			}
		}
		else{
			new_start = N->blocks[dataBlock].GuardTrackIndex;
			for (k = new_start; k < NC->blockSizeTrack; k++){
				SectorTableNo = dataBlock*NC->blockSizePage + k * NC->trackSizePage; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
				Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 3);//寫在DB上   / NC->CylinderSizeTrack
			}
			NSLB->pageRead += ((NC->blockSizeTrack - new_start + 1)*NC->trackSizePage); NSLB->pageWrite += ((NC->blockSizeTrack - new_start + 1)*NC->trackSizePage);
		}
	}
	free(merge_ram);
	free(victim);
#endif

	//NSLB統計資料 //NSLB->writedPageNum -= validIs2;
#ifdef MergeAssoc_NSLBpart
	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
#endif
}
//void FASTmergeRWNSLBPM3(sim *simPtr)
//{
//	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
//	DWORD	CurrentSectorNo = 0;
//	DWORD	mergedDBstart = 0, mergedDBend = 0;
//	DWORD validIs1 = 0, validIs2 = 0;
//	DWORD small = 0, goal = 0; //small time: means the band have the earliest time
//
//	I64 tmp_time;
//	DWORD BPLRUstartBand = 0;
//	DWORD tmp = 0;
//
//	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
//	DWORD SectorTableNo1 = 0;
//	DWORD block_no = 0, page_no = 0;
//	DWORD start = 0;
//	DWORD l = 0;
//	BYTE Dirty[256] = { 0 };
//	DWORD Dirty_Num = 0;
//
//	NSLB->mergecnt++;
//	do
//	{
//#ifdef BPLRULlist_RunLen
//		N->BPLRUcurrent_len = 1;
//#endif
//		small = 0;
//		while (N->BPLRU[small] == 0){ small++; }
//		assert(small <= NC->LsizeBlock - 1);
//		if (small != NC->LsizeBlock - 1){
//			BPLRUstartBand = small + 1;
//			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
//			{
//				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
//#ifdef BPLRULlist_RunLen
//				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
//#endif
//			}
//		}
//#ifdef BPLRULlist_RunLen
//		N->BPLRUtotal_len += N->BPLRUcurrent_len;
//		N->BPLRUtotal_count++;
//		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
//		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
//#endif
//		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
//		assert(tmp_time > 0);
//		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
//#ifdef NEWGC64trackWRITE
//		DWORD TrackIndex = 0, GuardTrackIndex = -1;
//		DWORD pre_start, pos_start;
//		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
//		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
//#ifdef Band_64MB_8Track
//		assert(Mini_TrackWrite == 64);
//		DirtyTrack_Group = 8; TrackSearch_End = 61;
//#endif
//#ifdef Band_128MB_16Track
//		assert(Mini_TrackWrite == 128);
//		DirtyTrack_Group = 16; TrackSearch_End = 125;
//#endif
//#ifdef Band_256MB_32Track
//		assert(Mini_TrackWrite == 256);
//		DirtyTrack_Group = 32; TrackSearch_End = 253;
//#endif
//		for (i = 0; i < TrackSearch_End + 3; i++){
//			Dirty[i] = 0;
//			for (j = 0; j < NC->trackSizePage; j++){
//				if (N->blocks[dataBlock].pages[i*NC->trackSizePage + j].valid == 2){
//					Dirty[i] = 1; Dirty_Num++; break;
//				}
//			}
//		}
//		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
//		//預先計算PM寫多track
//		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
//			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
//			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
//			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			if (tmp_TrackWrite < Mini_TrackWrite){
//				Mini_Index = GuardTrackIndex;// TrackIndex;
//				Mini_TrackWrite = tmp_TrackWrite;
//				N->pre_start = pre_start; N->pos_start = pos_start;
//			}
//		}
//		N->All_Merge++;
//		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
//		assert((Mini_Index - N->pre_start + 1) + (TrackSearch_End + 3 - N->pos_start) == Mini_TrackWrite);
//		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
//		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
//			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
//				if (Dirty[pre_start] == 1){ break; }
//			}
//			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
//				if (Dirty[pos_start] == 1){ break; }
//			}
//			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
//			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
//			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
//			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
//			N->pre_start = pre_start; N->pos_start = pos_start;
//		}
//		assert(Dirty_Num <= Mini_TrackWrite);
//		N->Rel_len += Dirty_Num;
//
//		if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1){//切過的要再切 //&& Mini_TrackWrite < SE_TrackWrite
//			if (N->blocks[dataBlock].GuardTrackIndex < Mini_Index){//換刀
//				//只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
//				validIs2 += WriteDB_SE2(simPtr, -1, dataBlock); assert(validIs2 > 0);
//				N->blocks[dataBlock].Cut = 0;
//
//				////只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
//				//validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite, dataBlock); //assert(validIs2 > 0);
//				//N->PM_Tracks_Allocated2Use_Condition = 1;//續切做換刀
//				//N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
//				////紀錄GT sector，把DB上的GT設成valid=3
//				//for (k = 0; k < NC->trackSizePage; k++){
//				//	SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
//				//	assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
//				//	block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
//				//	assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
//				//	assert(1 <= N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid && N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid <= 3);
//				//	if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
//				//		assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1);
//				//		if (N->PM_Sectors_Use < N->PM_Tracks*NC->trackSizePage){//可以PM的band的GT放入NSLB緩衝區
//				//			assert(N->PM_GTsector < N->PM_Tracks*NC->trackSizePage);
//				//			N->PMCandidate[N->PM_GTsector] = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;//PM GT sector valid 3
//				//			N->PM_GTsector++;
//				//			//NSLB->pageRead++; NSLB->pageWrite++; NSLB->write_count++;
//				//			N->PM_Sectors_Use++;
//				//		}
//				//	}
//				//}
//			}
//			else{
//				//只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
//				validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock); assert(validIs2 >= 0); //Mini_TrackWrite*NC->trackSizePage
//
//				//N->PM_Tracks_Allocated2Use_Condition = 1;//沒換刀
//				//Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
//			}
//			N->PM_len += Mini_TrackWrite;
//			N->DO_PM++;
//		}
//		else if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Sectors_Use + NC->trackSizePage <= N->PM_Tracks*NC->trackSizePage){//第一次切   && Mini_TrackWrite < SE_TrackWrite  N->PM_Tracks_Use < N->PM_Tracks 
//			DWORD tmp_PMGT_Signal = 0;
//			//判斷是不是真的允許PM
//			for (i = 0; i < N->SLB_tracks-1; i++){//N->SLB_tracks == N->PM_Tracks
//				if (N->SLBremainSectorTable[i] > 0 && N->SLBremainSectorTable[i + 1] == NC->trackSizePage){ tmp_PMGT_Signal = 1; break; }
//			}
//
//			if (tmp_PMGT_Signal == 1){
//
//				N->PM_Tracks_Allocated2Use_Condition = 1;
//				//設定PM info
//				N->blocks[dataBlock].Cut = 1;
//				N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
//
//				//只讀寫PM預算的量 //SLB->pageWrite += (Mini_TrackWrite*NC->trackSizePage);
//				validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock); assert(validIs2 > 0);
//				N->PM_len += Mini_TrackWrite;
//				N->DO_PM++;
//
//				//DWORD tmpNSLBw = 0;
//				//紀錄GT sector，把DB上的GT設成valid=3
//				for (k = 0; k < NC->trackSizePage; k++){
//					SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
//					assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
//					block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
//					assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
//					assert(1 <= N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid && N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid <= 3);
//					if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
//						assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1); //N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid = 3;//只把valid1設成3
//						//可以PM的band的GT放入NSLB緩衝區
//						assert(N->PM_GTsector < N->PM_Tracks*NC->trackSizePage);
//						N->PMCandidate[N->PM_GTsector] = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;//PM GT sector valid 3
//						N->PM_GTsector++;
//						NSLB->pageRead++; NSLB->pageWrite++; NSLB->write_count++;//計算PMGT寫進NSLB的write成本
//						N->PM_Sectors_Use++;
//					}
//				}
//			}
//			else{ validIs2 += WriteDB_SE2(simPtr, -1, dataBlock); assert(validIs2 > 0); }
//		}
//		else{//歸還PM Track
//			if (N->blocks[dataBlock].Cut == 1){
//				N->blocks[dataBlock].Cut = 0; //N->PM_Tracks_Use--; assert(0 <= N->PM_Tracks_Use);
//			}
//			validIs2 += WriteDB_SE2(simPtr, -1, dataBlock); assert(validIs2 > 0);
//		}
//#endif
//		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
//		assert(validIs2 >= 0);
//	} while (validIs2 == 0);
//	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
//	//NSLB統計資料
//	//NSLB->writedPageNum -= validIs2;
//#ifdef MergeAssoc_NSLBpart
//	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
//#endif
//}
void FASTmergeRWNSLBPM3(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time

	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD tmp = 0;

	DWORD Mini_Index, Mini_TrackWrite, tmp_Index, tmp_TrackWrite;
	DWORD SectorTableNo1 = 0;
	DWORD block_no = 0, page_no = 0;
	DWORD start = 0;
	DWORD l = 0;
	BYTE Dirty[256] = { 0 };
	DWORD Dirty_Num = 0;

	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0){ N->BPLRUcurrent_len++; }
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(tmp_time > 0);
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
#ifdef NEWGC64trackWRITE
		DWORD TrackIndex = 0, GuardTrackIndex = -1;
		DWORD pre_start, pos_start;
		DWORD DirtyTrack_Group = 0, TrackSearch_End = 0;
		Mini_Index = -1; Mini_TrackWrite = NC->blockSizeTrack; tmp_Index = -1; tmp_TrackWrite = 0;//初始化PM資訊
#ifdef Band_64MB_8Track
		assert(Mini_TrackWrite == 64);
		DirtyTrack_Group = 8; TrackSearch_End = 61;
#endif
#ifdef Band_128MB_16Track
		assert(Mini_TrackWrite == 128);
		DirtyTrack_Group = 16; TrackSearch_End = 125;
#endif
#ifdef Band_256MB_32Track
		assert(Mini_TrackWrite == 256);
		DirtyTrack_Group = 32; TrackSearch_End = 253;
#endif
		for (i = 0; i < TrackSearch_End + 3; i++){
			Dirty[i] = 0;
			for (j = 0; j < NC->trackSizePage; j++){
				if (N->blocks[dataBlock].pages[i*NC->trackSizePage + j].valid == 2){
					Dirty[i] = 1; Dirty_Num++; break;
				}
			}
		}
		N->pre_start = 0; N->pos_start = 0; Dirty_Num = 0;
		//預先計算PM寫多track
		for (TrackIndex = 0; TrackIndex <= TrackSearch_End; TrackIndex++){//61
			GuardTrackIndex = TrackIndex + 1; assert(1 <= GuardTrackIndex && GuardTrackIndex <= TrackSearch_End + 1);
			for (pre_start = 0; pre_start < GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= GuardTrackIndex);
			assert(GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			tmp_TrackWrite = (GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= tmp_TrackWrite && tmp_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			if (tmp_TrackWrite < Mini_TrackWrite){
				Mini_Index = GuardTrackIndex;// TrackIndex;
				Mini_TrackWrite = tmp_TrackWrite;
				N->pre_start = pre_start; N->pos_start = pos_start;
			}
		}
		N->All_Merge++;
		assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//64
		assert((Mini_Index - N->pre_start + 1) + (TrackSearch_End + 3 - N->pos_start) == Mini_TrackWrite);
		if (Mini_TrackWrite <= N->PartialMerge_Threshold){ N->Allow_PM++; }
		if (N->blocks[dataBlock].Cut == 1 && Mini_Index < N->blocks[dataBlock].GuardTrackIndex){//處理續切的mini index比之前的GT小
			for (pre_start = 0; pre_start < N->blocks[dataBlock].GuardTrackIndex; pre_start++){
				if (Dirty[pre_start] == 1){ break; }
			}
			for (pos_start = N->blocks[dataBlock].GuardTrackIndex + 1; pos_start <= TrackSearch_End + 2; pos_start++){//63
				if (Dirty[pos_start] == 1){ break; }
			}
			assert(0 <= pre_start && pre_start <= N->blocks[dataBlock].GuardTrackIndex);
			assert(N->blocks[dataBlock].GuardTrackIndex < pos_start && pos_start <= TrackSearch_End + 3);//64
			Mini_TrackWrite = (N->blocks[dataBlock].GuardTrackIndex - pre_start + 1) + (TrackSearch_End + 3 - pos_start); assert(0 <= Mini_TrackWrite && Mini_TrackWrite <= TrackSearch_End + 3);//該bandGC應該至少寫一條track
			Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
			N->pre_start = pre_start; N->pos_start = pos_start;
		}
		assert(Dirty_Num <= Mini_TrackWrite);
		N->Rel_len += Dirty_Num;

		if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 1){//切過的要再切 //&& Mini_TrackWrite < SE_TrackWrite
			//if (N->blocks[dataBlock].GuardTrackIndex < Mini_Index){//換刀
			//	//只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
			//	validIs2 += WriteDB_SE2(simPtr, -1, dataBlock); assert(validIs2 > 0);
			//	N->blocks[dataBlock].Cut = 0;

			//	////只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
			//	//validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite, dataBlock); //assert(validIs2 > 0);
			//	//N->PM_Tracks_Allocated2Use_Condition = 1;//續切做換刀
			//	//N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
			//	////紀錄GT sector，把DB上的GT設成valid=3
			//	//for (k = 0; k < NC->trackSizePage; k++){
			//	//	SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
			//	//	assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
			//	//	block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
			//	//	assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
			//	//	assert(1 <= N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid && N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid <= 3);
			//	//	if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
			//	//		assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1);
			//	//		if (N->PM_Sectors_Use < N->PM_Tracks*NC->trackSizePage){//可以PM的band的GT放入NSLB緩衝區
			//	//			assert(N->PM_GTsector < N->PM_Tracks*NC->trackSizePage);
			//	//			N->PMCandidate[N->PM_GTsector] = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;//PM GT sector valid 3
			//	//			N->PM_GTsector++;
			//	//			//NSLB->pageRead++; NSLB->pageWrite++; NSLB->write_count++;
			//	//			N->PM_Sectors_Use++;
			//	//		}
			//	//	}
			//	//}
			//}
			//else{
			//	//只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
			//	validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock); assert(validIs2 >= 0); //Mini_TrackWrite*NC->trackSizePage

			//	//N->PM_Tracks_Allocated2Use_Condition = 1;//沒換刀
			//	//Mini_Index = N->blocks[dataBlock].GuardTrackIndex;
			//}

			//只讀寫PM預算的量 //Mini_TrackWrite*NC->trackSizePage Mini_TrackWrite
			validIs2 += WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock); assert(validIs2 >= 0); //Mini_TrackWrite*NC->trackSizePage

			N->PM_len += Mini_TrackWrite;
			N->DO_PM++;
		}
		else if (Mini_TrackWrite <= N->PartialMerge_Threshold && N->blocks[dataBlock].Cut == 0 && N->PM_Sectors_Use + NC->trackSizePage <= N->PM_Tracks*NC->trackSizePage){//第一次切   && Mini_TrackWrite < SE_TrackWrite  N->PM_Tracks_Use < N->PM_Tracks 
			DWORD tmp_PMGT_Signal = 0;
			//判斷是不是真的允許PM
			for (i = 0; i < N->SLB_tracks - 1; i++){//N->SLB_tracks == N->PM_Tracks
				if (N->SLBremainSectorTable[i] > 0 && N->SLBremainSectorTable[i + 1] == NC->trackSizePage){ tmp_PMGT_Signal = 1; break; }
			}

			if (tmp_PMGT_Signal == 1){
				DWORD tmp_validIs2 = 0, result_validIs2 = 0;
				for (i = 0; i < NC->blockSizeSector; i++){
					if (N->blocks[dataBlock].pages[i].valid == 2){ tmp_validIs2++; }
				}

				N->PM_Tracks_Allocated2Use_Condition = 1;
				//設定PM info
				N->blocks[dataBlock].Cut = 1;
				N->blocks[dataBlock].GuardTrackIndex = Mini_Index;
				//只讀寫PM預算的量 //SLB->pageWrite += (Mini_TrackWrite*NC->trackSizePage);
				result_validIs2 = WriteDB_SE2(simPtr, Mini_TrackWrite*NC->trackSizePage, dataBlock);
				assert(result_validIs2 > 0); assert(tmp_validIs2 == result_validIs2);
				validIs2 += result_validIs2;
				N->PM_len += Mini_TrackWrite;
				N->DO_PM++;
				//DWORD tmpNSLBw = 0;
				//紀錄GT sector，把DB上的GT設成valid=3
				for (k = 0; k < NC->trackSizePage; k++){
					SectorTableNo1 = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;
					assert(0 <= Mini_Index*NC->trackSizePage + k && Mini_Index*NC->trackSizePage + k < NC->blockSizeSector);
					block_no = getblock(simPtr, SectorTableNo1); page_no = getpage(simPtr, SectorTableNo1);
					assert(block_no == dataBlock); assert(page_no == Mini_Index*NC->trackSizePage + k);
					assert(1 <= N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid && N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid <= 3);
					if (N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid == 1){
						assert(N->blocks[block_no].pages[page_no].sector == SectorTableNo1); //N->blocks[dataBlock].pages[Mini_Index*NC->trackSizePage + k].valid = 3;//只把valid1設成3
						//可以PM的band的GT放入NSLB緩衝區
						assert(N->PM_GTsector < N->PM_Tracks*NC->trackSizePage);
						N->PMCandidate[N->PM_GTsector] = dataBlock*NC->blockSizeSector + Mini_Index*NC->trackSizePage + k;//PM GT sector valid 3
						N->PM_GTsector++;
						NSLB->pageRead++; NSLB->pageWrite++; NSLB->write_count++;//計算PMGT寫進NSLB的write成本
						N->PM_Sectors_Use++;
					}
				}

				//if (tmp_validIs2 <= NC->trackSizePage){
				//	//delay
				//}
				//else if (Mini_Index <= NC->blockSizeTrack / 2){ //delay tmp_validIs2 <= 2 * NC->trackSizePage &&       if ()
				//	//if (Mini_Index < NC->blockSizeTrack / 2){//切
				//	//}
				//	//else{//delay
				//	//}
				//	
				//}
				//else{//SE
				//	validIs2 += WriteDB_SE2(simPtr, -1, dataBlock); assert(validIs2 > 0);
				//}
			}
			else{ validIs2 += WriteDB_SE2(simPtr, -1, dataBlock); assert(validIs2 > 0); }
		}
		else{//歸還PM Track
			if (N->blocks[dataBlock].Cut == 1){
				N->blocks[dataBlock].Cut = 0; //N->PM_Tracks_Use--; assert(0 <= N->PM_Tracks_Use);
			}
			validIs2 += WriteDB_SE2(simPtr, -1, dataBlock); assert(validIs2 > 0);
		}
#endif
		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 >= 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	//NSLB->writedPageNum -= validIs2;
#ifdef MergeAssoc_NSLBpart
	fprintf(fp_NSLB, "[%lu](x:%lu)%I64u ", dataBlock, validIs2, tmp_time);
#endif
}





void FASTmergeRWNSLBpart23_3(sim *simPtr)
{
	DWORD	i, j, k, RWLBA, dataBlock, newBlock, SectorTableNo, BlockNo, PageNo, Victim = simPtr->NFTLobj.firstRWblock, rDataBlk = 0, validPages = 0, invalidPages = 0, ret;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	//DWORD	tsp_start = 0, tsp_end = 0;//tsp is "trackSizePage"
	DWORD	CurrentSectorNo = 0;
	DWORD	mergedDBstart = 0, mergedDBend = 0;
	DWORD validIs1 = 0, validIs2 = 0;
	DWORD small = 0, goal = 0; //small time: means the band have the earliest time
	I64 tmp_time;
	DWORD BPLRUstartBand = 0;
	DWORD flag = 0, NSLBdirtyTrack = 0;
	DWORD tmp = 0;

#ifdef Time_Overhead
	DWORD merge_ram_size = 0;
	DWORD *merge_ram = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
	if (N->WriteBuff_Size > 0){
		NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
	}
#endif
	NSLB->mergecnt++;
	do
	{
#ifdef BPLRULlist_RunLen
		N->BPLRUcurrent_len = 1;
#endif
		small = 0;
		while (N->BPLRU[small] == 0){ small++; }
		assert(small <= NC->LsizeBlock - 1);
		if (small != NC->LsizeBlock - 1){
			BPLRUstartBand = small + 1;
			for (i = BPLRUstartBand; i < NC->LsizeBlock; i++)//之前用BPLRUstartBand+1, 要重跑
			{
				if (N->BPLRU[i] < N->BPLRU[small] && N->BPLRU[i] > 0){ small = i; }
#ifdef BPLRULlist_RunLen
				if (N->BPLRU[i] > 0) N->BPLRUcurrent_len++;
#endif
			}
		}
#ifdef BPLRULlist_RunLen
		N->BPLRUtotal_len += N->BPLRUcurrent_len;
		N->BPLRUtotal_count++;
		if (N->BPLRUcurrent_max < N->BPLRUcurrent_len) N->BPLRUcurrent_max = N->BPLRUcurrent_len;
		if (N->BPLRUcurrent_min > N->BPLRUcurrent_len) N->BPLRUcurrent_min = N->BPLRUcurrent_len;
#endif
		tmp_time = N->BPLRU[small]; dataBlock = small;//BPLRU
		assert(0 <= dataBlock && dataBlock <= NC->LsizeBlock - 1);
		flag = 0; NSLBdirtyTrack = 0;
		validIs1 = 0; validIs2 = 0;
#ifdef Time_Overhead
		merge_ram_size = 0;
#endif
		for (k = 0; k < NC->blockSizeSector; k++){//
			SectorTableNo = dataBlock*NC->blockSizeSector + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			assert(N->blocks[dataBlock].pages[k].valid == 1 || N->blocks[dataBlock].pages[k].valid == 2);
			if (N->blocks[dataBlock].pages[k].valid == 1){
				assert(N->blocks[dataBlock].pages[k].sector == SectorTableNo);
				//統計資料更新
				NSLB->pageRead++; NSLB->pageWrite++; validIs1++; //read: DB hit, write: DB write
				NSLB->merge_count++;
			}
			else if (N->blocks[dataBlock].pages[k].valid == 2){
				validIs2++; flag = 1;
				NSLBdirtyTrack = N->blocks[dataBlock].pages[k].sector / NC->trackSizePage;
				if (N->NSLBremainSectorTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectorTable[NSLBdirtyTrack]++;
#ifdef Time_Overhead
				merge_ram[merge_ram_size] = NSLBdirtyTrack * 2 * NC->trackSizePage; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
				assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
#endif
				//寫回原DB
				N->blocks[dataBlock].pages[k].valid = 1;
				N->blocks[dataBlock].pages[k].sector = SectorTableNo;
				//統計資料更新
				NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
				NSLB->merge_count++;
			}
		}
		//N->ValidIs2 += (I64)validIs2; 
		assert(0 < validIs2 && validIs2 <= NC->blockSizeSector);
#ifdef Time_Overhead
		Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]) / NC->trackSizePage, 2);

		assert(0 < merge_ram_size); assert(merge_ram_size <= NC->blockSizeSector);
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
		Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[0]) / NC->trackSizePage, 3);//電梯法回SLB讀資料   / NC->CylinderSizeTrack
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
			if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){ //seek
				Calc_TimeOverhead2(simPtr, (NC->LsizeSector + merge_ram[k]) / NC->trackSizePage, 3);//    / NC->CylinderSizeTrack
			}
		}
		//DB seek
		for (k = 0; k < NC->blockSizePage; k += NC->trackSizePage){
			SectorTableNo = dataBlock*NC->blockSizePage + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
			Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 3);//寫在DB上   / NC->CylinderSizeTrack
		}
		//N->preNSLB_WriteSector = 0;
		//seek
		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
#endif


		//#ifdef Time_Overhead
		//		if (N->WriteBuff_Size > 0){
		//			SLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
		//		}
		//#endif
		//		for (i = 0; i < NC->blockSizeTrack; i++){
		//#ifdef Time_Overhead
		//			merge_ram_size = 0;
		//#endif
		//			for (j = 0; j < NC->trackSizePage; j++){
		//				SectorTableNo = dataBlock*NC->blockSizeSector + i*NC->trackSizePage + j; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
		//				assert(N->blocks[dataBlock].pages[i*NC->trackSizePage + j].valid == 1 || N->blocks[dataBlock].pages[i*NC->trackSizePage + j].valid == 2);
		//				if (N->blocks[dataBlock].pages[i*NC->trackSizePage + j].valid == 1){
		//					assert(N->blocks[dataBlock].pages[i*NC->trackSizePage + j].sector == SectorTableNo);
		//					//統計資料更新
		//					NSLB->pageRead++; NSLB->pageWrite++; validIs1++; //read: DB hit, write: DB write
		//					NSLB->merge_count++;
		//				}
		//				else if (N->blocks[dataBlock].pages[i*NC->trackSizePage + j].valid == 2){
		//					validIs2++; flag = 1;
		//					NSLBdirtyTrack = N->blocks[dataBlock].pages[i*NC->trackSizePage + j].sector / NC->trackSizePage;
		//					if (N->NSLBremainSectorTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
		//					N->NSLBremainSectorTable[NSLBdirtyTrack]++;
		//#ifdef Time_Overhead
		//					merge_ram[merge_ram_size] = NSLBdirtyTrack * 2 * NC->trackSizePage; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
		//					assert(merge_ram_size <= NC->blockSizeSector); assert(merge_ram[merge_ram_size - 1] <= N->partial_page);
		//#endif
		//					//寫回原DB
		//					N->blocks[dataBlock].pages[i*NC->trackSizePage + j].valid = 1;
		//					N->blocks[dataBlock].pages[i*NC->trackSizePage + j].sector = SectorTableNo;
		//					//統計資料更新
		//					NSLB->pageRead += 2; NSLB->pageWrite++; //read: DB miss + NSLB hit, write: DB write
		//					NSLB->merge_count++;
		//				}
		//			}
		//#ifdef Time_Overhead
		//			if (merge_ram_size > 0){
		//				assert(merge_ram_size <= NC->blockSizeSector);
		//				qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		//				for (j = 0; j < merge_ram_size - 1; j++){ assert(merge_ram[j] <= merge_ram[j + 1]); }
		//				Calc_TimeOverhead2(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1), 1);//電梯法回SLB讀資料 //   / NC->CylinderSizeTrack
		//				N->Merge++;
		//				for (j = 1; j < merge_ram_size; j++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
		//					if (merge_ram[j - 1] / NC->trackSizePage != merge_ram[j] / NC->trackSizePage){//seek
		//						Calc_TimeOverhead2(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[j] / NC->trackSizePage) + 1), 1);//   / NC->CylinderSizeTrack
		//						N->Merge++;
		//					}
		//				}
		//			}
		//			Calc_TimeOverhead2(simPtr, (dataBlock*NC->blockSizePage + j*NC->trackSizePage) / NC->trackSizePage, 1);//SLB結束   / NC->CylinderSizeTrack
		//			N->Merge++;
		//#endif
		//		}
		//N->ValidIs2 += (I64)validIs2;


		N->BPLRU[small] = 0;//merge完，時間歸0以防止空band參與BPLRU
		assert(validIs2 > 0);
	} while (validIs2 == 0);
	assert(0<validIs2 && validIs2 <= NC->blockSizeSector);//本來用assert(0<=validIs2 && validIs2 <= NC->blockSizeSector);
	//NSLB統計資料
	NSLB->writedPageNum -= validIs2;
#ifdef MergeAssoc_NSLBpart
	fprintf(fp_NSLB, "[%lu](%lu:%lu)%I64u ", dataBlock, validIs1, validIs2, tmp_time);
#endif
#ifdef Time_Overhead
	free(merge_ram);
#endif
}

#endif