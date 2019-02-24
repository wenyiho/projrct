#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"
#include <math.h>


#ifdef SLB_Mapping_Cache
#ifdef CFLRU
void InsertLRU(sim *simPtr, I64 nwLA, I64 nwPA, BYTE level){//no care for misscnt
	//printf("Insert start\n");
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD block_no, section_no;
	block_no = (DWORD)nwLA / NC->blockSizeSector; section_no = (DWORD)nwLA % NC->blockSizeSector;
	LRU_Map_Entry *newEntry = (LRU_Map_Entry*)calloc(1, sizeof(LRU_Map_Entry));
	assert(newEntry != NULL);
	newEntry->LA = nwLA;
	newEntry->PA = nwPA;
	assert(N->blocks[block_no].sections[section_no].sector == nwPA);
	assert(N->DBalloc[block_no] == 1);
	assert(N->blocks[block_no].sections[section_no].inCache == NULL);
	N->blocks[block_no].sections[section_no].inCache = newEntry;
	N->blocks[block_no].sections[section_no].cache_lev = level;
	N->blocks[block_no].sections[section_no].cache_stat = 1;
	if (N->SLB_Map_Cache[level].cache_used == 0){
		newEntry->next = N->SLB_Map_Cache[level].LRU_Head;
		N->SLB_Map_Cache[level].LRU_Head->prev = newEntry;
	}
	else{
		N->SLB_Map_Cache[level].LRU_Head->next->prev = newEntry;
		newEntry->next = N->SLB_Map_Cache[level].LRU_Head->next;
	}
	N->SLB_Map_Cache[level].LRU_Head->next = newEntry;
	newEntry->prev = N->SLB_Map_Cache[level].LRU_Head;
	assert(N->SLB_Map_Cache[level].LRU_Head->next == newEntry);
	assert(newEntry->prev == N->SLB_Map_Cache[level].LRU_Head);
	
	N->SLB_Map_Cache[level].cache_used++;
}
void CacheDrop(sim *simPtr, I64 LA){
	//printf("drop start\n");
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD block_no, section_no;
	LRU_Map_Entry *victim;
	BYTE level ;
	if (LA == -1){//By Replace only drop L2
		level = 1;
		assert(N->SLB_Map_Cache[level].LRU_Head->prev->next == N->SLB_Map_Cache[level].LRU_Head);
		victim = N->SLB_Map_Cache[level].LRU_Head->prev;
		N->Cache_Replace++;
	}
	else{//By Merge
		N->Cache_Merge++;
		block_no = (DWORD)LA / NC->blockSizeSector; section_no = (DWORD)LA % NC->blockSizeSector;
		assert(N->DBalloc[block_no] == 1);
		victim = N->blocks[block_no].sections[section_no].inCache;
		if (victim != NULL) { level = N->blocks[block_no].sections[section_no].cache_lev;}
	}
	if (victim != NULL){ 
		victim->prev->next = victim->next;
		victim->next->prev = victim->prev;
		block_no = (DWORD)victim->LA / NC->blockSizeSector; section_no = (DWORD)victim->LA % NC->blockSizeSector;
		assert(N->DBalloc[block_no] == 1);
		assert(N->blocks[block_no].sections[section_no].inCache != NULL);
		N->blocks[block_no].sections[section_no].inCache = NULL;
		N->blocks[block_no].sections[section_no].cache_stat = 0;
		N->blocks[block_no].sections[section_no].cache_lev = 0;
		free(victim);
		N->SLB_Map_Cache[level].cache_used--;
		assert(N->SLB_Map_Cache[level].LRU_Head->prev->next == N->SLB_Map_Cache[level].LRU_Head);
	}
	//printf("drop end\n");
}
void LRUpull2Head(sim *simPtr, LRU_Map_Entry *moveEntry, BYTE nwCacheLev){
	//printf("pull start\n");
	assert(moveEntry != NULL);
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD block_no, section_no;
	block_no = (DWORD)moveEntry->LA / NC->blockSizeSector; section_no = (DWORD)moveEntry->LA % NC->blockSizeSector;
	BYTE oldlevel;
	assert(N->blocks[block_no].sections[section_no].inCache == moveEntry);
	oldlevel = N->blocks[block_no].sections[section_no].cache_lev;

	if (oldlevel != nwCacheLev){//fall to level 2, no care for oversize
		N->Cache_L1toL2++;
		assert(oldlevel == 0 && nwCacheLev == 1);
		assert(N->SLB_Map_Cache[oldlevel].LRU_Head->prev == moveEntry);
		assert(moveEntry->next == N->SLB_Map_Cache[oldlevel].LRU_Head);
		moveEntry->prev->next = moveEntry->next;
		moveEntry->next->prev = moveEntry->prev;
		N->SLB_Map_Cache[oldlevel].cache_used --;
		//printf("fall");

		assert(N->SLB_Map_Cache[nwCacheLev].LRU_Head->next->prev == N->SLB_Map_Cache[nwCacheLev].LRU_Head);
		if (N->SLB_Map_Cache[nwCacheLev].cache_used == 0) {
			N->SLB_Map_Cache[nwCacheLev].LRU_Head->next = moveEntry;
			N->SLB_Map_Cache[nwCacheLev].LRU_Head->prev = moveEntry;
			moveEntry->next = N->SLB_Map_Cache[nwCacheLev].LRU_Head;
			moveEntry->prev = N->SLB_Map_Cache[nwCacheLev].LRU_Head;
		}
		else {
			N->SLB_Map_Cache[nwCacheLev].LRU_Head->next->prev = moveEntry;
			moveEntry->next = N->SLB_Map_Cache[nwCacheLev].LRU_Head->next;
			N->SLB_Map_Cache[nwCacheLev].LRU_Head->next = moveEntry;
			moveEntry->prev = N->SLB_Map_Cache[nwCacheLev].LRU_Head;
		}
		N->SLB_Map_Cache[nwCacheLev].cache_used++;
		N->blocks[block_no].sections[section_no].cache_lev = 1;
		assert(N->SLB_Map_Cache[nwCacheLev].LRU_Head->next == moveEntry);
		assert(N->SLB_Map_Cache[nwCacheLev].LRU_Head->next->prev == N->SLB_Map_Cache[nwCacheLev].LRU_Head);
		assert(N->SLB_Map_Cache[oldlevel].LRU_Head->prev->next == N->SLB_Map_Cache[oldlevel].LRU_Head);
		assert(N->SLB_Map_Cache[oldlevel].LRU_Head->next->prev == N->SLB_Map_Cache[oldlevel].LRU_Head);
	}
	else{ // same level LRU Update
		assert((nwCacheLev == 0) || (nwCacheLev == 1));
		if (N->SLB_Map_Cache[nwCacheLev].LRU_Head->next != moveEntry){
			moveEntry->prev->next = moveEntry->next;
			moveEntry->next->prev = moveEntry->prev;
			N->SLB_Map_Cache[nwCacheLev].LRU_Head->next->prev = moveEntry;
			moveEntry->next = N->SLB_Map_Cache[nwCacheLev].LRU_Head->next;
			N->SLB_Map_Cache[nwCacheLev].LRU_Head->next = moveEntry;
			moveEntry->prev = N->SLB_Map_Cache[nwCacheLev].LRU_Head;
			assert(moveEntry->prev == N->SLB_Map_Cache[nwCacheLev].LRU_Head);
			assert(N->SLB_Map_Cache[nwCacheLev].LRU_Head->next == moveEntry);
		}
	}
	//printf("pull end\n");

}
void CacheAccess(sim *simPtr, I64 nwLA, I64 nwPA, DWORD op){ //op 1:read 2:write 3:delay+LMA
	//printf("Access start\n");
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD block_no, section_no;
	block_no = (DWORD)nwLA / NC->blockSizeSector; section_no = (DWORD)nwLA % NC->blockSizeSector;
	LRU_Map_Entry *find = NULL;
	assert(N->DBalloc[block_no] == 1);
	find = N->blocks[block_no].sections[section_no].inCache ;
	if(find != NULL) { //Hit
		assert(find->LA == nwLA);
		BYTE level=N->blocks[block_no].sections[section_no].cache_lev;

		if (op == 1 || op == 2){
			if(level == 0) N->Cache_Hit++;
			else N->Cache_Hit2++;
			LRUpull2Head(simPtr, find, level);
			if (op == 1) { 
				assert(find->PA == nwPA); 
				if(level == 0)N->Cache_Read++;
				else N->Cache_Read2++;
			}
			else { //write
				N->blocks[block_no].sections[section_no].cache_stat = 1;//dirty bit
				find->PA = nwPA; 
				if(level == 0)N->Cache_Write++;
				else N->Cache_Write2++;
			}
		}
		else{ //delay + LMA
			assert(find != NULL);
			find->PA = nwPA;
			if(level == 0) N->Cache_Alldelay ++;
			else N->Cache_Alldelay2++;
		}
	}//endif hit
	else{ // Miss & read ahead
		BYTE EntrySizeBit = 57;
		DWORD LookaheadEntry = NC->sectorSizeByte * 8 / EntrySizeBit ;
		if (op != 3){
			/*L1*/
			if(op == 1) N->Cache_Read_Miss++;
			else N->Cache_Write_Miss++;
			
			assert(N->SLB_Map_Cache != NULL && N->SLB_Map_Cache[0].LRU_Head != NULL);
			if(N->SLB_Map_Cache[0].cache_used % 1000000 == 0)
				printf("%lu  %lu\n", N->SLB_Map_Cache[0].cache_used, N->SLB_Map_Cache[0].cache_size);
			
			if (N->SLB_Map_Cache[0].cache_used == N->SLB_Map_Cache[0].cache_size){//Lev 1 full
				assert(N->DBalloc[N->SLB_Map_Cache[0].LRU_Head->prev->LA / NC->blockSizeSector] == 1);
				LRUpull2Head(simPtr, N->SLB_Map_Cache[0].LRU_Head->prev, 1);
			}
			if (N->DBalloc[block_no] == 0){
				assert(N->blocks[block_no].sections == NULL);
				N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
				assert(N->blocks[block_no].sections != NULL);
				for (DWORD i = 0; i < NC->blockSizeSector; i++) {
					N->blocks[block_no].sections[i].valid = 1;
					N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
					N->blocks[block_no].sections[i].inCache = NULL;
					N->blocks[block_no].sections[i].cache_stat = 0;
					N->blocks[block_no].sections[i].cache_lev = 0;
				}
				N->DBalloc[block_no] = 1;
			}
				
			
			InsertLRU(simPtr, nwLA, nwPA, 0);
			/*Read ahead to L2*/
			I64 readaheadStart = nwLA / LookaheadEntry *LookaheadEntry;
			I64 nwPA;
			for (BYTE offset = 0; offset < LookaheadEntry; offset++){ //lookahead to L2
				DWORD ahead_block, ahead_sec;//17bit need
				if ((readaheadStart + offset) != nwLA && ((readaheadStart + offset) / NC->blockSizeSector) < NC->LsizeBlock){ //避免read ahead衝到LB
					ahead_block = (DWORD)(readaheadStart + offset) / NC->blockSizeSector; ahead_sec = (DWORD)(readaheadStart + offset) % NC->blockSizeSector;
					if (N->DBalloc[ahead_block] == 0) {
						assert(N->blocks[ahead_block].sections == NULL);
						N->blocks[ahead_block].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
						assert(N->blocks[ahead_block].sections != NULL);
						for (DWORD i = 0; i < NC->blockSizeSector; i++) {
							N->blocks[ahead_block].sections[i].valid = 1;
							N->blocks[ahead_block].sections[i].sector = (I64)ahead_block*NC->blockSizeSector + i;
							N->blocks[ahead_block].sections[i].inCache = NULL;
							N->blocks[ahead_block].sections[i].cache_stat = 0;
							N->blocks[ahead_block].sections[i].cache_lev = 0;
						}
						N->DBalloc[ahead_block] = 1;
					}
					assert(N->DBalloc[ahead_block] == 1 && N->blocks[ahead_block].sections != NULL);
					nwPA = N->blocks[ahead_block].sections[ahead_sec].sector;
					if (N->blocks[ahead_block].sections[ahead_sec].inCache == NULL) {
						InsertLRU(simPtr, readaheadStart + offset, nwPA, 1);
					}
					assert(N->blocks[ahead_block].sections[ahead_sec].inCache != NULL);
				}//endif((readaheadStart + offset) != nwLA ...
			}//endfor
		}
		while (N->SLB_Map_Cache[1].cache_used > N->SLB_Map_Cache[1].cache_size){//drop the redundant entries
			assert(N->SLB_Map_Cache[1].LRU_Head->prev != NULL);
			I64 dropEntSecStart = N->SLB_Map_Cache[1].LRU_Head->prev->LA / LookaheadEntry *LookaheadEntry;
			DWORD drop_block, drop_sec, drop_same_block, drop_same_sec;
			drop_block = N->SLB_Map_Cache[1].LRU_Head->prev->LA / NC->blockSizeSector;
			drop_sec = N->SLB_Map_Cache[1].LRU_Head->prev->LA % NC->blockSizeSector;
			if (N->blocks[drop_block].sections[drop_sec].cache_stat == 1){
				N->Cache_Replace_withW++;
				for (BYTE offset = 0; offset < LookaheadEntry; offset++){ // clean the same sector's entry data
					drop_same_block = (DWORD)(dropEntSecStart + offset) / NC->blockSizeSector;
					drop_same_sec = (DWORD)(dropEntSecStart + offset) % NC->blockSizeSector;
					assert(N->DBalloc[drop_same_block] == 1);

					if (N->blocks[drop_same_block].sections[drop_same_sec].inCache != NULL && N->blocks[drop_same_block].sections[drop_same_sec].cache_stat == 1){ //clean the bit for those who are still in the cache
						N->blocks[drop_same_block].sections[drop_same_sec].cache_stat = 0;
					}
				}
			}
			CacheDrop(simPtr, -1);
			
		}
		assert(N->SLB_Map_Cache[1].cache_used <= N->SLB_Map_Cache[1].cache_size);
	}
	//printf("Access end\n");

}

#else //LRU cache
void CacheDrop(sim *simPtr, I64 LA){ //full, drop it
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD block_no, section_no;
	LRU_Map_Entry *victim;
	if (LA == -1){//By Replace, drop tail
		N->Cache_Replace ++;
		assert(N->SLB_Map_Cache->cache_size == N->SLB_Map_Cache->cache_used);
		victim = N->SLB_Map_Cache->LRU_Head->prev;
		assert(victim != N->SLB_Map_Cache->LRU_Head);//used > 0
		assert(N->SLB_Map_Cache->cache_used > 0);
	}
	else{//By Merge, could be anywhere in list
		N->Cache_Merge ++;
		block_no = (DWORD)LA / NC->blockSizeSector; section_no = LA % NC->blockSizeSector;
		victim = N->blocks[block_no].sections[section_no].inCache;
	}
	if (victim != NULL){
		victim->prev->next = victim->next;
		victim->next->prev = victim->prev;
		block_no = (DWORD)victim->LA / NC->blockSizeSector; section_no = (DWORD)victim->LA % NC->blockSizeSector;
		N->blocks[block_no].sections[section_no].inCache = NULL;
		free(victim);
		N->SLB_Map_Cache->cache_used--;
		assert(N->SLB_Map_Cache->LRU_Head->prev->next == N->SLB_Map_Cache->LRU_Head);
	}
	
}

void CacheAccess(sim *simPtr, I64 nwLA, I64 nwPA, DWORD op){//RorW=1, Read request; 2 Write request; 3 Delay(delay+LMA)
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	LRU_Map_Entry *find = NULL;
	DWORD block_no, section_no;
	block_no = (DWORD)nwLA / NC->blockSizeSector; section_no = (DWORD)nwLA % NC->blockSizeSector;
	find = N->blocks[block_no].sections[section_no].inCache = N->blocks[block_no].sections[section_no].inCache;
	//printf("Access start\n");
	if (find != NULL){//In cache then update
		assert(find->LA == nwLA);
		if (op == 1 || op == 2){ //read or write hit
			N->Cache_Hit++;
			if (N->SLB_Map_Cache->LRU_Head->next != find){ ///Update LRU
				find->prev->next = find->next;
				find->next->prev = find->prev;
				N->SLB_Map_Cache->LRU_Head->next->prev = find;
				find->next = N->SLB_Map_Cache->LRU_Head->next;
				N->SLB_Map_Cache->LRU_Head->next = find;
				find->prev = N->SLB_Map_Cache->LRU_Head;
				assert(find->prev == N->SLB_Map_Cache->LRU_Head);
				assert(N->SLB_Map_Cache->LRU_Head->next == find);
			}
			if (op == 1) {
				assert(find->PA == nwPA); N->Cache_Read++;
			}
			else {
				find->PA = nwPA; N->Cache_Write++;
			}
		}
		else{
			assert(find != NULL);
			find->PA = nwPA;
			N->Cache_Alldelay++;
		}
	}
	else{ // not in cache then allocate a new entry, put it to the first place
		if (op != 3){
			if (N->SLB_Map_Cache->cache_used >= N->SLB_Map_Cache->cache_size){ CacheDrop(N, -1); }
			LRU_Map_Entry *newEntry = (LRU_Map_Entry*)calloc(1, sizeof(LRU_Map_Entry));
			assert(newEntry != NULL);
			newEntry->LA = nwLA;
			newEntry->PA = nwPA;
			assert(N->blocks[block_no].sections[section_no].sector == nwPA);
			N->blocks[block_no].sections[section_no].inCache = newEntry;

			if (N->SLB_Map_Cache->cache_used == 0){
				newEntry->next = N->SLB_Map_Cache->LRU_Head;
				N->SLB_Map_Cache->LRU_Head->prev = newEntry;
			}
			else{
				N->SLB_Map_Cache->LRU_Head->next->prev = newEntry;
				newEntry->next = N->SLB_Map_Cache->LRU_Head->next;
			}
			N->SLB_Map_Cache->LRU_Head->next = newEntry;
			newEntry->prev = N->SLB_Map_Cache->LRU_Head;
			assert(N->SLB_Map_Cache->LRU_Head->next == newEntry);
			assert(newEntry->prev == N->SLB_Map_Cache->LRU_Head);
			N->SLB_Map_Cache->cache_used++;
			assert(N->SLB_Map_Cache->cache_used <= N->SLB_Map_Cache->cache_size);

			if (op == 1) { N->Cache_Read++; N->Cache_Read_Miss++; }
			else if (op == 2) {
				N->Cache_Write++; N->Cache_Write_Miss++;
			}

		}
	}
}


#endif
#endif

#ifdef Time_Overhead
//void Calc_TimeOverhead(sim *simPtr, I64 cur_HeadLocatedCylinder, DWORD Type)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
//	N->cur_HeadLocatedCylinder = cur_HeadLocatedCylinder;
//	assert(0 <= N->cur_HeadLocatedCylinder && N->cur_HeadLocatedCylinder < N->Cylinders);//N->Cylinders == N->Scale_down
//	N->dScale = fabs(N->cur_HeadLocatedCylinder - N->pre_HeadLocatedCylinder)*N->Scale_up / N->Scale_down;  assert(N->dScale < N->Scale_up);
//
//	switch (Type){
//	case 0://SLB write
//		if (N->dScale <= N->SeekModelBoundary){ N->SLBWrite_SeekTime += (3.24 + 0.0143*sqrt(N->dScale)); }
//		else{ N->SLBWrite_SeekTime += (4.1225 + 0.0000585*N->dScale); }
//		break;
//	case 1://SLB merge
//		if (N->dScale <= N->SeekModelBoundary){ N->SLBMerge_SeekTime += (3.24 + 0.0143*sqrt(N->dScale)); }
//		else{ N->SLBMerge_SeekTime += (4.1225 + 0.0000585*N->dScale); }
//		break;
//	case 2://NSLB write
//		if (N->dScale <= N->SeekModelBoundary){ N->NSLBWrite_SeekTime += (3.24 + 0.0143*sqrt(N->dScale)); }
//		else{ N->NSLBWrite_SeekTime += (4.1225 + 0.0000585*N->dScale); }
//		break;
//	case 3://NSLB merge
//		if (N->dScale <= N->SeekModelBoundary){ N->NSLBMerge_SeekTime += (3.24 + 0.0143*sqrt(N->dScale)); }
//		else{ N->NSLBMerge_SeekTime += (4.1225 + 0.0000585*N->dScale); }
//		break;
//	default:
//		printf("error Time Overhead Type\n"); system("pause");
//	}
//	N->pre_HeadLocatedCylinder = N->cur_HeadLocatedCylinder;
//}

void Calc_RotateOverhead2(sim *simPtr, I64 cur_HeadSector, DWORD Type)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	unsigned long long SMR_Rotate = 5;//10.16//一般設定: 5900 RPM
	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	switch (Type){
	case 0://Write_W_SeekTime
		N->Write_W_RotTime++;// += SMR_Rotate;
		break;
	case 1://Write_R_SeekTime
		N->Write_R_RotTime++;// += SMR_Rotate;
		break;
	case 2://Merge_W_SeekTime
		N->Merge_W_RotTime++;// += SMR_Rotate;
		break;
	case 3://Merge_R_SeekTime
		N->Merge_R_RotTime++;// += SMR_Rotate;
		break;
	case 4://Merge_R_SeekTime
		N->Merge_RLB_RotTime++;
		N->Merge_R_RotTime++;// += SMR_Rotate;
		break;
	case 10://Merge_R_SeekTime
		N->WB_RotTime++;
		break;
	default:
		printf("error Time Overhead Type\n"); system("pause");
	}
	if (N->RAM_WAIT == 1){ N->Wait_RotTime++; }
	N->Req_RunTime += SMR_Rotate;
	assert(N->Req_RunTime >= SMR_Rotate);
}
void Calc_Rotate_SkewSMR(sim *simPtr, I64 cur_HeadSector, DWORD Type)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	unsigned long long SMR_Rotate = 5;//10.16//一般設定: 5900 RPM
	unsigned long long Skew_SMR_Rotate = 2;
	DWORD Crren_LTA = cur_HeadSector / NC->trackSizeSector;
	DWORD Current_Band = cur_HeadSector / NC->blockSizeSector, Previous_Band = N->Previous_LTA / NC->blockSizeTrack;
	DWORD RotTime_cnt = 0, SkewRotTime_cnt = 0;
	//寫入SLB的request, Merge讀SLB相鄰Track(預設平均從Track中間讀起)都保證5ms position time
	if ((Type == 0 && N->FirstSection == 1) || Type == 4){ RotTime_cnt = 1; }// 6/16更新
	else if (N->Previous_LTA + 1 == Crren_LTA || N->Previous_LTA == Crren_LTA + 1){
		//連續讀寫相鄰Track
		if (Current_Band == Previous_Band || (cur_HeadSector >= NC->LsizeSector && N->Previous_LTA * NC->trackSizeSector >= NC->LsizeSector)){
			SkewRotTime_cnt = 1;
		}else{ RotTime_cnt = 1; }
	}else{ RotTime_cnt = 1; }
	//檢查
	if (SkewRotTime_cnt == 1){ assert(RotTime_cnt == 0); }
	else{ assert(SkewRotTime_cnt == 0); }
	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	switch (Type){
	case 0:
		N->Write_W_SkewRotTime += SkewRotTime_cnt;
		N->Write_W_RotTime += RotTime_cnt;
		break;
	case 1:
		N->Write_R_SkewRotTime += SkewRotTime_cnt;
		N->Write_R_RotTime += RotTime_cnt;
		break;
	case 2:
		N->Merge_W_SkewRotTime += SkewRotTime_cnt;
		N->Merge_W_RotTime += RotTime_cnt;
		break;
	case 3:
		N->Merge_R_SkewRotTime += SkewRotTime_cnt;
		N->Merge_R_RotTime += RotTime_cnt;
		break;
	case 4:
		//Read SLB就算讀相鄰Track也不易有機會連續, 預設中間讀起用5m position time
		/*N->Merge_RLB_SkewRotTime += SkewRotTime_cnt;
		N->Merge_R_SkewRotTime += SkewRotTime_cnt;*/
		N->Merge_RLB_RotTime += RotTime_cnt;
		N->Merge_R_RotTime += RotTime_cnt;
		break;
	case 10:
		N->WB_SkewRotTime += SkewRotTime_cnt;
		N->WB_RotTime += RotTime_cnt;
		break;
#ifdef READ_REQUEST
	case 20:
		N->Read_SkewRotTime += SkewRotTime_cnt;
		N->Read_RotTime += RotTime_cnt;
		break;
	case 21:
		//這裡重置RotTime_cnt, SkewRotTime_cnt 所以Req_RunTime的計算不能往前移
		RotTime_cnt = 1; SkewRotTime_cnt = 0;
		N->Read_RotTime += RotTime_cnt;
		break;
#endif
#ifdef POWERFAIL
	case 30:
		N->PF_SkewRotTime += SkewRotTime_cnt;
		N->PF_RotTime += RotTime_cnt;
		break;
#endif
	default:
		printf("error Time Overhead Type\n"); system("pause");
	}
	//if (N->RAM_WAIT == 1){ N->Wait_RotTime++; }//不使用RAM, 之後刪掉
	//只會加一次, Skew_SMR_Rotate或SMR_Rotate
	N->Req_RunTime += (Skew_SMR_Rotate*SkewRotTime_cnt + SMR_Rotate*RotTime_cnt);
	N->Previous_LTA = Crren_LTA;//紀錄最新Track位置
	assert(N->Req_RunTime >= Skew_SMR_Rotate);
}

void Calc_TimeOverhead2_old(sim *simPtr, I64 cur_HeadSector, DWORD Type)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	N->cur_HeadSector = cur_HeadSector;
	//N->d = fabs(N->cur_HeadLocatedCylinder - N->pre_HeadLocatedCylinder); fabs()不能用

	NC->CylinderSizeTrack = 8;
	N->Cylinders = NC->PsizeSector / NC->trackSizePage / NC->CylinderSizeTrack;

	N->Cylinders = 355637279 / NC->trackSizePage / NC->CylinderSizeTrack;
	N->Scale_up = 81928;
	N->Scale_down = N->Cylinders;//(83303, 635907):default Archive HDD 635907 cylinders
	N->SeekModelBoundary = 34829;// 15993;// 383;// 15000; //616; // 15000;
	NC->CylinderSizeTrack = 8;//head num 12;

	I64 preCylinder = 0;
	I64 curCylinder = 0;// N->Cylinders

	preCylinder = N->pre_HeadSector / NC->trackSizePage / NC->CylinderSizeTrack;
	//(N->pre_HeadSector / NC->trackSizePage) / (CylinderGroup * NC->CylinderSizeTrack) * CylinderGroup + (N->pre_HeadSector / NC->trackSizePage) % CylinderGroup;
	curCylinder = N->cur_HeadSector / NC->trackSizePage / NC->CylinderSizeTrack;
	//(N->cur_HeadSector / NC->trackSizePage) / (CylinderGroup * NC->CylinderSizeTrack) * CylinderGroup + (N->cur_HeadSector / NC->trackSizePage) % CylinderGroup;

	assert(0 <= preCylinder); assert(preCylinder <= N->Cylinders);
	//assert(0 <= curCylinder); assert(curCylinder < N->Cylinders);
	if (curCylinder > N->Cylinders){ printf("%I64u %I64u\n", curCylinder, N->Cylinders); system("pause"); }

	N->d = 0;
	if (curCylinder >= preCylinder){ N->d = curCylinder - preCylinder; }
	else{ N->d = preCylinder - curCylinder; }

	double seek_time = 0;
	N->d = N->d * N->Scale_up / N->Scale_down;
	if (preCylinder != curCylinder){
		if (N->d <= N->SeekModelBoundary){ seek_time = (3.45 + 0.0794 * sqrt(N->d)); }//3.24
		else{ seek_time = (10.8 + 0.000212*N->d); }//8 0.018
		
		//if (N->d < N->SeekModelBoundary){ seek_time = (3.75 + 0.33 * sqrt((double)N->d)); }//3.24
		//else{ seek_time = (7.69 + 0.0059*(double)N->d); }//8 0.018
		switch (Type){
		case 0://Write_W_SeekTime
			N->Write_W_SeekTime += seek_time;
			N->Write_W_SeekCount++;
			break;
		case 1://Write_R_SeekTime
			N->Write_R_SeekTime += seek_time;
			N->Write_R_SeekCount++;
			break;
		case 2://Merge_W_SeekTime
			N->Merge_W_SeekTime += seek_time;
			N->Merge_W_SeekCount++;
			break;
		case 3://Merge_R_SeekTime
			N->Merge_R_SeekTime += seek_time;
			N->Merge_R_SeekCount++;
			break;
		default:
			printf("error Time Overhead Type\n"); system("pause");
		}
		N->pre_HeadSector = N->cur_HeadSector;
		/*FILE *t = fopen("seek.txt", "a");
		fprintf(t, "%I64u %I64u, d = %I64u, seek = %lf\n", preCylinder, curCylinder, N->d, seek_time);
		fclose(t);*/
	}
	else{ N->seek_0++; }

}
void Calc_TimeOverhead2(sim *simPtr, I64 cur_HeadSector, DWORD Type)//my disk
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	//N->d = fabs(N->cur_HeadLocatedCylinder - N->pre_HeadLocatedCylinder); fabs()不能用
	NC->CylinderSizeTrack = 8; //N->Scale_up = 81928; N->Scale_down = N->Cylinders;
	N->Cylinders = 81928;//83303
	N->SeekModelBoundary = 34829;
	DWORD Time_Disk = 300 * 1024;//MB
	N->cur_HeadSector = cur_HeadSector * Time_Disk / NC->PsizeMB;
	N->cur_HeadTrack = N->cur_HeadSector / NC->trackSizePage;
	N->cur_HeadCylinder = N->cur_HeadTrack / NC->CylinderSizeTrack;

	N->d = 0;
	if (N->cur_HeadCylinder >= N->pre_HeadCylinder){ N->d = N->cur_HeadCylinder - N->pre_HeadCylinder; }
	else{ N->d = N->pre_HeadCylinder - N->cur_HeadCylinder; }
	N->seek_distance += N->d;
	double seek_time = 0;
	if (N->pre_HeadCylinder != N->cur_HeadCylinder){
		if (N->d <= N->SeekModelBoundary){ seek_time = (3.45 + 0.0794 * sqrt(N->d)); }//10.86
		else{ seek_time = (10.8 + 0.000212*N->d); }
		switch (Type){
		case 0://Write_W_SeekTime
			N->Write_W_SeekTime += seek_time;
			N->Write_W_SeekCount++;
			break;
		case 1://Write_R_SeekTime
			N->Write_R_SeekTime += seek_time;
			N->Write_R_SeekCount++;
			break;
		case 2://Merge_W_SeekTime
			N->Merge_W_SeekTime += seek_time;
			N->Merge_W_SeekCount++;
			break;
		case 3://Merge_R_SeekTime
			N->Merge_R_SeekTime += seek_time;
			N->Merge_R_SeekCount++;
			break;
		default:
			printf("error Time Overhead Type\n"); system("pause");
		}
		//N->pre_HeadSector = N->cur_HeadSector;
		/*FILE *t = fopen("seek.txt", "a");
		fprintf(t, "%I64u %I64u, d = %I64u, seek = %lf\n", N->pre_HeadCylinder, N->cur_HeadCylinder, N->d, seek_time);
		fclose(t);*/
	}
	else{ 
		N->seek_0++;
		switch (Type){
		case 0://Write_W_SeekTime
			N->seek_0_WW++;
			break;
		case 1://Write_R_SeekTime
			N->seek_0_WR++;
			break;
		case 2://Merge_W_SeekTime
			N->seek_0_MW++;
			break;
		case 3://Merge_R_SeekTime
			N->seek_0_MR++;
			break;
		default:
			printf("error Time Overhead Type\n"); system("pause");
		}
	}
	N->pre_HeadSector = N->cur_HeadSector;
	N->pre_HeadTrack = N->cur_HeadTrack;
	N->pre_HeadCylinder = N->cur_HeadCylinder;
}

DWORD Calc_TimeOverhead3(sim *simPtr, I64 cur_HeadSector, DWORD Type)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD zone_no = 0;
	//float ftmp_cur_HeadSector = cur_HeadSector*((float)N->Disk.DiskSizeMB / TraceDiskMB);//HP 3323 size 1031 MB
	//I64 tmp_cur_HeadSector = cur_HeadSector*N->Disk.DiskSizeMB / (NC->LsizeSector / 2048);//HP 3323 size 1031 MB
	I64 tmp_cur_HeadSector = cur_HeadSector * N->Disk.DiskSizeMB / NC->PsizeMB;//HP 3323 size 1031 MB
	N->cur_HeadSector = tmp_cur_HeadSector;
	N->cur_HeadTrack = 0;
	assert(0 <= Type && Type <=3);
	assert(0 <= tmp_cur_HeadSector); assert(tmp_cur_HeadSector < NC->LsizeSector);
	while (tmp_cur_HeadSector > N->Disk.Zone[zone_no].ZoneSizeSector - 1){
		tmp_cur_HeadSector -= N->Disk.Zone[zone_no].ZoneSizeSector;
		zone_no++;
	}
	//assert(zone_no < N->Disk.numZones);
	if (zone_no >= N->Disk.numZones){ 
		printf("cur_HeadSector %I64u, N->cur_HeadSector %I64u, N->Disk.DiskSizeMB %lu, NC->LsizeSector %lu\n", cur_HeadSector, N->cur_HeadSector, N->Disk.DiskSizeMB, NC->LsizeSector); 
		system("pause");
	}
	tmp_cur_HeadSector += N->Disk.Zone[zone_no].deadspace;
	assert(tmp_cur_HeadSector >= 0);
	N->cur_HeadTrack = (tmp_cur_HeadSector / N->Disk.Zone[zone_no].TrackSizeSector);
	N->cur_HeadCylinder = N->Disk.Zone[zone_no].FirstCylNo + tmp_cur_HeadSector / (NC->CylinderSizeTrack*(I64)N->Disk.Zone[zone_no].TrackSizeSector);

	/*FILE *t = NULL;
	if (N->pre_HeadCylinder == N->cur_HeadCylinder && N->pre_HeadTrack == N->cur_HeadTrack){
		t = fopen("seek.txt", "a");
		fprintf(t, "%lf\n", N->Write_W_SeekTime + N->Write_R_SeekTime + N->Merge_W_SeekTime + N->Merge_R_SeekTime);
		fclose(t);
		return;
	}*/

	N->SeekModelBoundary = 300;
	N->d = 0;
	if (N->cur_HeadCylinder >= N->pre_HeadCylinder){ N->d = N->cur_HeadCylinder - N->pre_HeadCylinder; }
	else{ N->d = N->pre_HeadCylinder - N->cur_HeadCylinder; }
	assert(0 <= N->d); assert(N->d <= N->Disk.Zone[N->Disk.numZones-1].LastCylNo);// 14 // 找出到底tracksize是多少，修改N->Cylinders
	N->seek_distance += N->d;

	/*FILE *fp = fopen("distance.txt", "a");
	fprintf(fp, "%I64u ", N->d); fclose(fp);*/
	/*FILE *fp1 = fopen("addr.txt", "a");
	fprintf(fp1, "%I64u: %I64u = %lu + %I64u, ", cur_HeadSector, N->cur_HeadCylinder, N->Disk.Zone[zone_no].FirstCylNo, tmp_cur_HeadSector / (NC->CylinderSizeTrack*N->Disk.Zone[zone_no].TrackSizeSector));
	fprintf(fp1, "%I64u %I64u %lu\n", tmp_cur_HeadSector, NC->CylinderSizeTrack, N->Disk.Zone[zone_no].TrackSizeSector);
	fclose(fp1);*/

	double seek_time = 0;
	if (N->d == 0){
		N->seek_0++;
		switch (Type){
		case 0://Write_W_SeekTime
			N->seek_0_WW++;
			break;
		case 1://Write_R_SeekTime
			N->seek_0_WR++;
			break;
		case 2://Merge_W_SeekTime
			N->seek_0_MW++;
			break;
		case 3://Merge_R_SeekTime
			N->seek_0_MR++;
			break;
		}
	}
	else{
		//if (N->d <= N->SeekModelBoundary){ N->Write_W_SeekTime += (3.45 + 0.0794 * sqrt(N->d)); }//3.24
		//else{ N->Write_W_SeekTime += (10.93 + 0.000212*N->d); }//8 0.018
		//seek_time = 13.232; N->Merge_W_SeekTime += seek_time;
		if (N->d < N->SeekModelBoundary){ seek_time = (3.75 + 0.33 * sqrt((double)N->d)); }//3.24
		else{ seek_time = (7.69 + 0.0059*(double)N->d); }//8 0.018
		seek_time += N->Disk.seekwritedelta;
		//fprintf(t, "%lf %I64u,%lf->", seek_time, N->cur_HeadCylinder, N->Write_W_SeekTime + N->Write_R_SeekTime + N->Merge_W_SeekTime + N->Merge_R_SeekTime);
		switch (Type){
		case 0:
			N->Write_W_SeekTime += seek_time;
			N->Write_W_SeekCount++;
			break;
		case 1:
			N->Write_R_SeekTime += seek_time;
			N->Write_R_SeekCount++;
			break;
		case 2:
			N->Merge_W_SeekTime += seek_time;
			N->Merge_W_SeekCount++;
			break;
		case 3:
			N->Merge_R_SeekTime += seek_time;
			N->Merge_R_SeekCount++;
			break;
		default:
			printf("error Time Overhead Type\n"); system("pause");
		}
		N->SeekTime += seek_time;
		/*FILE *t = fopen("seek.txt", "a");
		fprintf(t, "%I64u %I64u, d = %I64u, seek = %lf, N->SeekTime = %lf\n", N->pre_HeadCylinder, N->cur_HeadCylinder, N->d, seek_time, N->SeekTime);
		fclose(t);*/
	}

	N->pre_HeadSector = N->cur_HeadSector;
	N->pre_HeadTrack = N->cur_HeadTrack;
	N->pre_HeadCylinder = N->cur_HeadCylinder;
	return zone_no;
}
void Seek_End(sim *simPtr, I64 cur_HeadSector, DWORD Type)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);

	DWORD zone_no = 0;
	I64 tmp_cur_HeadSector = cur_HeadSector*(I64)N->Disk.DiskSizeMB / (I64)(NC->LsizeSector / 2048);//HP 3323 size 1031 MB
	N->cur_HeadSector = tmp_cur_HeadSector;
	assert(0 <= Type && Type <= 3);
	assert(0 <= tmp_cur_HeadSector); assert(tmp_cur_HeadSector < NC->LsizeSector); //assert(NC->CylinderSizeTrack == 17);
	while (tmp_cur_HeadSector > N->Disk.Zone[zone_no].ZoneSizeSector - 1){//N->Disk.Zone[zone_no].ZoneSizeCyl*NC->CylinderSizeTrack*N->Disk.Zone[zone_no].TrackSizeSector 
		tmp_cur_HeadSector -= N->Disk.Zone[zone_no].ZoneSizeSector;// (N->Disk.Zone[zone_no].ZoneSizeCyl*NC->CylinderSizeTrack*N->Disk.Zone[zone_no].TrackSizeSector);
		N->cur_HeadTrack += (N->Disk.Zone[zone_no].ZoneSizeCyl*NC->CylinderSizeTrack);
		zone_no++;
	}
	assert(zone_no < N->Disk.numZones);// 14
	tmp_cur_HeadSector += N->Disk.Zone[zone_no].deadspace;
	assert(tmp_cur_HeadSector >= 0);
	N->cur_HeadTrack += (tmp_cur_HeadSector / N->Disk.Zone[zone_no].TrackSizeSector);
	N->cur_HeadCylinder = N->Disk.Zone[zone_no].FirstCylNo + tmp_cur_HeadSector / (NC->CylinderSizeTrack*(I64)N->Disk.Zone[zone_no].TrackSizeSector);
	//if (tmp_cur_HeadSector % (NC->CylinderSizeTrack*N->Disk.Zone[zone_no].TrackSizeSector) != 0){ N->cur_HeadCylinder++; }
	
	N->pre_HeadSector = N->cur_HeadSector;
	N->pre_HeadTrack = N->cur_HeadTrack;
	N->pre_HeadCylinder = N->cur_HeadCylinder;
}

//DWORD Seek_Seagate_C15k5(sim *simPtr, I64 cur_HeadSector, DWORD Type)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
//	DWORD zone_no = 0;
//	//float ftmp_cur_HeadSector = cur_HeadSector*((float)N->Disk.DiskSizeMB / TraceDiskMB);//HP 3323 size 1031 MB
//	//I64 tmp_cur_HeadSector = cur_HeadSector*N->Disk.DiskSizeMB / (NC->LsizeSector / 2048);//HP 3323 size 1031 MB
//
//	I64 tmp_cur_HeadSector = cur_HeadSector * N->Disk.DiskSizeMB / NC->LsizeMB;
//	N->cur_HeadSector = tmp_cur_HeadSector;
//	N->cur_HeadTrack = 0;
//	assert(0 <= Type && Type <= 3);
//	assert(0 <= tmp_cur_HeadSector); //assert(tmp_cur_HeadSector < N->Disk.DiskSizeSector);//NC->LsizeSector
//	if (tmp_cur_HeadSector >= N->Disk.DiskSizeSector){
//		printf("%I64u %I64u<=%lu, %I64u %I64u\n", cur_HeadSector, tmp_cur_HeadSector, N->Disk.DiskSizeSector, N->Disk.DiskSizeMB, NC->LsizeMB); system("pause");
//	}
//	while (tmp_cur_HeadSector > N->Disk.Zone[zone_no].ZoneSizeSector - 1){
//		tmp_cur_HeadSector -= N->Disk.Zone[zone_no].ZoneSizeSector;
//		zone_no++;
//	}
//	//assert(zone_no < N->Disk.numZones);
//	if (zone_no >= N->Disk.numZones){
//		printf("cur_HeadSector %I64u, N->cur_HeadSector %I64u, N->Disk.DiskSizeMB %lu, NC->LsizeSector %lu\n", cur_HeadSector, N->cur_HeadSector, N->Disk.DiskSizeMB, NC->LsizeSector);
//		system("pause");
//	}
//	tmp_cur_HeadSector += N->Disk.Zone[zone_no].deadspace;
//	assert(tmp_cur_HeadSector >= 0);
//	N->cur_HeadTrack = (tmp_cur_HeadSector / N->Disk.Zone[zone_no].TrackSizeSector);
//	N->cur_HeadCylinder = N->Disk.Zone[zone_no].FirstCylNo + tmp_cur_HeadSector / (NC->CylinderSizeTrack*(I64)N->Disk.Zone[zone_no].TrackSizeSector);
//
//	//FILE *fp_addr = fopen("addr.txt", "a");
//	//fprintf(fp_addr, "%I64u->%I64u (c:%I64u,h:%I64u)\n", cur_HeadSector, N->cur_HeadSector, N->cur_HeadCylinder, N->cur_HeadTrack); fclose(fp_addr);
//
//	N->d = 0;
//	if (N->cur_HeadCylinder >= N->pre_HeadCylinder){ N->d = N->cur_HeadCylinder - N->pre_HeadCylinder; }
//	else{ N->d = N->pre_HeadCylinder - N->cur_HeadCylinder; }
//	assert(0 <= N->d); assert(N->d <= N->Disk.Zone[N->Disk.numZones - 1].LastCylNo);// 14 // 找出到底tracksize是多少，修改N->Cylinders
//	N->seek_distance += N->d;
//
//	double seek_time = 0;
//	if (N->d == 0){
//		N->seek_0++;
//		switch (Type){
//		case 0: N->seek_0_WW++; break; //Write_W_SeekTime
//		case 1: N->seek_0_WR++; break; //Write_R_SeekTime
//		case 2: N->seek_0_MW++; break; //Merge_W_SeekTime
//		case 3: N->seek_0_MR++; break; //Merge_R_SeekTime
//		}
//	}
//	else{//N->SeekModelBoundary: 7261 == 300/2982*72171, 9.459
//		if (N->d < N->SeekModelBoundary){ seek_time = (3.75 + 0.067 * sqrt((double)N->d)); }//3.75 + 0.33
//		else{ seek_time = (7.69 + 0.0002438*(double)N->d); }//7.69 + 0.0059 
//		seek_time += N->Disk.seekwritedelta;
//		switch (Type){
//		case 0: N->Write_W_SeekTime += seek_time; N->Write_W_SeekCount++; break;
//		case 1: N->Write_R_SeekTime += seek_time; N->Write_R_SeekCount++; break;
//		case 2: N->Merge_W_SeekTime += seek_time; N->Merge_W_SeekCount++; break;
//		case 3: N->Merge_R_SeekTime += seek_time; N->Merge_R_SeekCount++; break;
//		}
//		N->SeekTime += seek_time;
//	}
//
//	N->pre_HeadSector = N->cur_HeadSector;
//	N->pre_HeadTrack = N->cur_HeadTrack;
//	N->pre_HeadCylinder = N->cur_HeadCylinder;
//	return zone_no;
//}
DWORD Seek_Seagate_C15k5(sim *simPtr, I64 cur_HeadSector, DWORD Type)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD zone_no = 0;
	//float ftmp_cur_HeadSector = cur_HeadSector*((float)N->Disk.DiskSizeMB / TraceDiskMB);//HP_C3323 1031MB
	//I64 tmp_cur_HeadSector = cur_HeadSector*N->Disk.DiskSizeMB / (NC->LsizeSector / 2048);
#ifdef Simulation_HDD
	zone_no = 0;
#endif
#if defined(Simulation_SLB) || defined(Simulation_NSLB)
	if (cur_HeadSector >= NC->LsizeSector){ 
		zone_no = 14; 
		cur_HeadSector -= NC->LsizeSector;
	}//SMR OVP
	else{ zone_no = 0; }
	//用平均值2MB cylinder算距離在scale到HP_C3323的2982 cylinder中
#endif
	I64 tmp_cur_HeadSector = cur_HeadSector * N->Disk.DiskSizeMB / NC->LsizeMB;//scale sector address
	N->cur_HeadSector = tmp_cur_HeadSector;
	N->cur_HeadTrack = 0;
	assert(0 <= Type && Type <= 3);
	assert(0 <= tmp_cur_HeadSector); //assert(tmp_cur_HeadSector < N->Disk.DiskSizeSector);//NC->LsizeSector
	if (tmp_cur_HeadSector >= N->Disk.DiskSizeSector){
		printf("%I64u %I64u<=%lu, %I64u %I64u\n", cur_HeadSector, tmp_cur_HeadSector, N->Disk.DiskSizeSector, N->Disk.DiskSizeMB, NC->LsizeMB); system("pause");
	}
	while (tmp_cur_HeadSector > N->Disk.Zone[zone_no].ZoneSizeSector - 1){
		tmp_cur_HeadSector -= N->Disk.Zone[zone_no].ZoneSizeSector;
		zone_no++;
	}
	if (zone_no >= N->Disk.numZones){
		printf("cur_HeadSector %I64u, N->cur_HeadSector %I64u, N->Disk.DiskSizeMB %lu, NC->LsizeSector %lu\n", cur_HeadSector, N->cur_HeadSector, N->Disk.DiskSizeMB, NC->LsizeSector);
		system("pause");
	}

	tmp_cur_HeadSector += N->Disk.Zone[zone_no].deadspace;
	assert(tmp_cur_HeadSector >= 0);
	N->cur_HeadTrack = (tmp_cur_HeadSector / N->Disk.Zone[zone_no].TrackSizeSector);
	N->cur_HeadCylinder = N->Disk.Zone[zone_no].FirstCylNo + tmp_cur_HeadSector / (NC->CylinderSizeTrack*(I64)N->Disk.Zone[zone_no].TrackSizeSector);
	//FILE *fp_addr = fopen("addr.txt", "a");
	//fprintf(fp_addr, "%I64u->%I64u (c:%I64u,h:%I64u)\n", cur_HeadSector, N->cur_HeadSector, N->cur_HeadCylinder, N->cur_HeadTrack); fclose(fp_addr);

	N->d = 0;
	if (N->cur_HeadCylinder >= N->pre_HeadCylinder){ N->d = N->cur_HeadCylinder - N->pre_HeadCylinder; }
	else{ N->d = N->pre_HeadCylinder - N->cur_HeadCylinder; }
	assert(0 <= N->d); assert(N->d <= N->Disk.Zone[N->Disk.numZones - 1].LastCylNo);// 14 // 找出到底tracksize是多少，修改N->Cylinders
	N->seek_distance += N->d;

	double seek_time = 0;
	if (N->d == 0){
		N->seek_0++;
		switch (Type){
		case 0: N->seek_0_WW++; break; //Write_W_SeekTime
		case 1: N->seek_0_WR++; break; //Write_R_SeekTime
		case 2: N->seek_0_MW++; break; //Merge_W_SeekTime
		case 3: N->seek_0_MR++; break; //Merge_R_SeekTime
		}
	}
	else{//N->SeekModelBoundary: 7261 == 300/2982*72171, 9.459
		if (N->d < N->SeekModelBoundary){ seek_time = (3.75 + 0.067 * sqrt((double)N->d)); }//3.75 + 0.33
		else{ seek_time = (7.69 + 0.0002438*(double)N->d); }//7.69 + 0.0059 
		seek_time += N->Disk.seekwritedelta;
		switch (Type){
		case 0: N->Write_W_SeekTime += seek_time; N->Write_W_SeekCount++; break;
		case 1: N->Write_R_SeekTime += seek_time; N->Write_R_SeekCount++; break;
		case 2: N->Merge_W_SeekTime += seek_time; N->Merge_W_SeekCount++; break;
		case 3: N->Merge_R_SeekTime += seek_time; N->Merge_R_SeekCount++; break;
		}
		N->SeekTime += seek_time;
		//arrtime
		N->Req_RunTime += seek_time;
	}

	N->pre_HeadSector = N->cur_HeadSector;
	N->pre_HeadTrack = N->cur_HeadTrack;
	N->pre_HeadCylinder = N->cur_HeadCylinder;
	return zone_no;
}
DWORD test(sim *simPtr, I64 cur_HeadSector, DWORD Type)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD zone_no = 0;

	/*if (cur_HeadSector > NC->LsizeSector){
		N->cur_HeadSector = NC->LsizeSector + (cur_HeadSector - NC->LsizeSector) / 109;
		N->cur_HeadCylinder = N->cur_HeadSector / (NC->CylinderSizeTrack*NC->trackSizeSector);
	}
	else{
		N->cur_HeadSector = cur_HeadSector;
		N->cur_HeadCylinder = cur_HeadSector / (NC->CylinderSizeTrack*NC->trackSizeSector);
	}*/
	N->cur_HeadSector = cur_HeadSector;
	N->cur_HeadCylinder = cur_HeadSector / (NC->CylinderSizeTrack*NC->trackSizeSector);
	
	N->d = 0;
	if (N->cur_HeadCylinder >= N->pre_HeadCylinder){ N->d = N->cur_HeadCylinder - N->pre_HeadCylinder; }
	else{ N->d = N->pre_HeadCylinder - N->cur_HeadCylinder; }
	assert(N->d == _abs64(N->cur_HeadCylinder - N->pre_HeadCylinder));
	DWORD cur_Block = N->cur_HeadCylinder / NC->blockSizeCylinder, pre_Block = N->pre_HeadCylinder / NC->blockSizeCylinder;
	DWORD Block_GTs = 0;
	if (cur_Block >= pre_Block){ Block_GTs = cur_Block - pre_Block; }
	else{ Block_GTs = pre_Block - cur_Block; }
	assert(Block_GTs == abs(cur_Block - pre_Block));
	N->d = N->d + Block_GTs;
	double NonScale_d = N->d;

	N->d = N->d * N->Disk.DiskSizeCylinder / NC->LsizeCylinder;
	assert(N->d >= (DWORD)N->d);
	N->d = (DWORD)N->d + ((N->d > (DWORD)N->d) ? 1 : 0);
	N->seek_distance += N->d;

	double seek_time = 0;
	if (N->d == 0){
		N->seek_0++;
		switch (Type){
		case 0: N->seek_0_WW++; break; //Write_W_SeekTime
		case 1: N->seek_0_WR++; break; //Write_R_SeekTime
		case 2: N->seek_0_MW++; break; //Merge_W_SeekTime
		case 3: N->seek_0_MR++; break; //Merge_R_SeekTime
		case 4: N->seek_0_MRLB++; break; //Merge_R_SeekTime
#ifdef READ_REQUEST
		case 20: N->seek_0_R++; break;//Read
#endif
#ifdef POWERFAIL
		case 30: N->seek_0_PF++; break;
#endif
		}
#ifdef Simulation_NSLB
		N->Cylinder_Change = 0;
#endif
	}
	else{//N->SeekModelBoundary: 7261(來自300/2982*72171) -> 9.459ms //72172 -> 25.2855336ms
		//(3.75 + 0.067 * sqrt(d)) - SeekModelBoundary - (7.69 + 0.0002438*d)

		/*
		HP 97560
		1962 cylinder
		boundary 383
		3.24 + 0.400√d
		8.00 + 0.008d
		0.400*√(1962/72171) = 0.0659
		0.008*(1962/72171) = 0.0002175
		N->SeekModelBoundary = 72172 * 383 / 1962 = 14088
		*/


		if (N->d < N->SeekModelBoundary){ seek_time = (3.24 + 0.0659 * sqrt((double)N->d)); }
		else{ seek_time = (8 + 0.0002175*(double)N->d); }
		seek_time += N->Disk.seekwritedelta;
		assert(seek_time > 0);
		FILE *t=NULL;
		//Resp
		unsigned long long ull_seek_time = (unsigned long long)round(seek_time);
		N->Req_RunTime += ull_seek_time;//紀錄Running Time, 算Response
		switch (Type){
		case 0: N->Write_W_SeekTime += ull_seek_time; N->Write_W_SeekCount++;
			N->W_seek_distance += N->d;
			N->rW_seek_distance += NonScale_d;
			//printf("N->d %lf, NonScale_d %lf. N->Disk.DiskSizeCylinder %lf, NC->LsizeCylinder %lu, N->Disk.DiskSizeCylinder / NC->LsizeCylinder %lf\n", N->d, NonScale_d, N->Disk.DiskSizeCylinder, NC->LsizeCylinder, N->Disk.DiskSizeCylinder / NC->LsizeCylinder); system("pause");
			/*static int cnt = 0;
			if (NonScale_d != 1){
				t = fopen("HP.txt", "a");
				fprintf(t, "W cnt =%3d, N->d, NonScale_d: %.1lf %.1lf\n", cnt, N->d, NonScale_d);
				fclose(t);
				cnt = 0;
			}
			cnt++;*/
			//printf("LSector:%I64u TKsector:%lu,pre_HeadSector:%I64u cur_HeadSector:%I64u, d:%lf, seek time:%llu\n", NC->LsizeSector, NC->trackSizeSection, N->pre_HeadSector, N->cur_HeadSector, N->d, ull_seek_time); system("pause");
			break;
		case 1:
			if (cur_HeadSector < NC->LsizeSector){ N->Write_RDB_SeekTime += ull_seek_time; N->Write_RDB_SeekCount++; }
			else{ N->Write_RLB_SeekTime += ull_seek_time; N->Write_RLB_SeekCount++; }
			N->Write_R_SeekTime += ull_seek_time; N->Write_R_SeekCount++;
			break;
		case 2: N->Merge_W_SeekTime += ull_seek_time; N->Merge_W_SeekCount++;
			N->MWseek_distance += N->d;
			N->rMWseek_distance += NonScale_d;
			break;
		case 3: N->Merge_R_SeekTime += ull_seek_time; N->Merge_R_SeekCount++; //R DB
			N->MR_DB_seek_distance += N->d;
			N->rMR_DB_seek_distance += NonScale_d;
			break;
		case 4: //Read SLB的時間和Count一併算在Merge Read, 比較好複製貼上
			N->Merge_RLB_SeekTime += ull_seek_time; N->Merge_RLB_SeekCount++; //R LB
			N->Merge_R_SeekTime += ull_seek_time; N->Merge_R_SeekCount++;
			N->MR_SLB_seek_distance += N->d;
			N->rMR_SLB_seek_distance += NonScale_d;
			break;
		case 10:
			N->WB_SeekTime += ull_seek_time; N->WB_SeekCount++; //Delay Sector WB
			N->WB_seek_distance += N->d;
			break;
#ifdef READ_REQUEST
		case 20:
			N->Read_SeekTime += ull_seek_time; N->R_SeekCount++;
			N->R_seek_distance += N->d;
			break;
#endif
#ifdef POWERFAIL
		case 30: N->PF_SeekTime += ull_seek_time; N->PF_SeekCount++;
			N->PF_seek_distance += N->d; 
			break;
#endif
		}
		N->SeekTime += ull_seek_time;
#ifdef Simulation_NSLB
		N->Cylinder_Change = 1;
#endif
		N->pre_HeadSector = N->cur_HeadSector;
		N->pre_HeadTrack = N->cur_HeadTrack;
		N->pre_HeadCylinder = N->cur_HeadCylinder;
	}
	return zone_no;
}
void Calc_TimeOverhead(sim *simPtr, I64 cur_HeadSector, DWORD Type)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	N->cur_HeadSector = cur_HeadSector;
	assert(0 <= N->cur_HeadCylinder && N->cur_HeadCylinder < N->Cylinders);//N->Cylinders == N->Scale_down
	//N->d = fabs(N->cur_HeadLocatedCylinder - N->pre_HeadLocatedCylinder); fabs()不能用

	//  NC->Disk_trackSizePage
	/*if (N->cur_HeadSector >= N->pre_HeadSector){ N->d = N->cur_HeadSector / NC->trackSizePage / NC->CylinderSizeTrack - N->pre_HeadSector / NC->trackSizePage / NC->CylinderSizeTrack; }
	else{ N->d = N->pre_HeadSector / NC->trackSizePage / NC->CylinderSizeTrack - N->cur_HeadSector / NC->trackSizePage / NC->CylinderSizeTrack; }*/
	DWORD CylinderGroup = 25;
	I64 preCylinder = 0;
	I64 curCylinder = 0;// N->Cylinders

	preCylinder = (N->pre_HeadSector / NC->trackSizePage) / (CylinderGroup * NC->CylinderSizeTrack) * CylinderGroup + (N->pre_HeadSector / NC->trackSizePage) % CylinderGroup;
	curCylinder = (N->cur_HeadSector / NC->trackSizePage) / (CylinderGroup * NC->CylinderSizeTrack) * CylinderGroup + (N->cur_HeadSector / NC->trackSizePage) % CylinderGroup;

	assert(0 <= preCylinder); assert(preCylinder < N->Cylinders);
	//assert(0 <= curCylinder); assert(curCylinder < N->Cylinders);
	if (curCylinder >= N->Cylinders){ printf("%I64u %I64u\n", curCylinder, N->Cylinders); system("pause"); }

	N->d = 0;
	if (curCylinder >= preCylinder){ N->d = curCylinder - preCylinder; }
	else{ N->d = preCylinder - curCylinder; }

	//if (N->pre_HeadSector / NC->trackSizePage != N->cur_HeadSector / NC->trackSizePage){ Calc_RotateOverhead2(simPtr, cur_HeadSector, Type); }

	N->d = N->d * N->Scale_up / N->Scale_down;
	if (preCylinder != curCylinder){
		//N->FirstTime = 1;
		switch (Type){
		case 0://Write_W_SeekTime
			if (N->d <= N->SeekModelBoundary){ N->Write_W_SeekTime += (3.45 + 0.0794 * sqrt(N->d)); }//3.24
			else{ N->Write_W_SeekTime += (10.8 + 0.000212*N->d); }//8 0.018
			N->Write_W_SeekCount++;
			break;
		case 1://Write_R_SeekTime
			if (N->d <= N->SeekModelBoundary){ N->Write_R_SeekTime += (3.45 + 0.0794*sqrt(N->d)); }
			else{ N->Write_R_SeekTime += (10.8 + 0.000212*N->d); }
			N->Write_R_SeekCount++;
			break;
		case 2://Merge_W_SeekTime
			if (N->d <= N->SeekModelBoundary){ N->Merge_W_SeekTime += (3.45 + 0.0794*sqrt(N->d)); }
			else{ N->Merge_W_SeekTime += (10.8 + 0.000212*N->d); }
			N->Merge_W_SeekCount++;
			break;
		case 3://Merge_R_SeekTime
			if (N->d <= N->SeekModelBoundary){ N->Merge_R_SeekTime += (3.45 + 0.0794*sqrt(N->d)); }
			else{ N->Merge_R_SeekTime += (10.8 + 0.000212*N->d); }
			N->Merge_R_SeekCount++;
			break;
		default:
			printf("error Time Overhead Type\n"); system("pause");
		}
		N->pre_HeadSector = N->cur_HeadSector;
	}
	else{ N->seek_0++; }

}

#ifdef POWERFAIL
void PF_DiskCacheWrite(sim *simPtr, DWORD start_Track)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	assert(start_Track <= NC->blockSizeTrack);
	for (i = start_Track; i < NC->blockSizeTrack; i++){
		SectorTableNo = NC->PsizeSector + i * NC->trackSizeSector;
		//30: Power Fail Write
		test(simPtr, SectorTableNo, 30);
		Calc_Rotate_SkewSMR(simPtr, SectorTableNo, 30);
		N->content_rotate += NC->trackSizeSector;
		N->PF_content_rotate += NC->trackSizeSector;
		N->Req_RunTime += 10;
		//更新統計資料
		SLB->pageWrite += NC->trackSizeSector;
		N->PF_pageWrite += NC->trackSizeSector;
	}
}
#endif
#endif

DWORD Two_Expo(DWORD n){
	//0  1  2  3  4   5   6   7    8    9    10    11    12    13    14     15     16     17      18      19      20       21       22       23       24        25        26        27         28         29         30          31
	DWORD ans[32] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304, 8388608, 16777216, 33554432, 67108864, 134217728, 268435456, 536870912, 1073741824, 2147483648 };
	assert(0 <= n && n <= 31);
	return ans[n];
}
#ifdef READ_REQUEST
//SLB, Shuffle, Delay共用一種Read
void DM_SMRreadSector(sim *simPtr, I64 offsetSector, DWORD lenSector){
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD j;
	DWORD SectorTableNo, block_no, section_no;
	DWORD LBsector;
	DWORD *SMR_ReadSector = (DWORD*)calloc(NC->CylinderSizeSector, sizeof(DWORD));
	DWORD SMR_ReadSector_Len = 0;
	I64 i;
	I64 start_offsetSector = offsetSector;
	I64 end_offsetSector = offsetSector + lenSector - 1;
	//初始化Req_RunTime
	N->Req_RunTime = 0;
#ifdef Time_Overhead
	while (offsetSector <= end_offsetSector){
		SectorTableNo = (DWORD)offsetSector;
		block_no = SectorTableNo / NC->blockSizeSector; section_no = SectorTableNo % NC->blockSizeSector;
		//判斷Sector在Data Band或SLB
		assert(N->DBalloc[block_no] == 0 || N->DBalloc[block_no] == 1);
		if (N->DBalloc[block_no] == 1 && N->blocks[block_no].sections[section_no].valid == 0){
			LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
			//裝進SMR_ReadSector, 等結束做排序小到大, 再全部Read
			assert(NC->LsizeSector + LBsector < NC->PsizeSector); assert(SMR_ReadSector_Len < NC->CylinderSizeSector);
			SMR_ReadSector[SMR_ReadSector_Len] = NC->LsizeSector + LBsector; SMR_ReadSector_Len++;
		}
		offsetSector++; //lenSector--; if (lenSector <= 0){ break; }
	}
	//讀Data Band上Sector //acctime
	for (i = start_offsetSector; i <= end_offsetSector; i += NC->trackSizeSector){
		test(simPtr, i, 20); Calc_Rotate_SkewSMR(simPtr, i, 20);
	}
	//讀LB資料 //acctime
	assert(0 <= SMR_ReadSector_Len); assert(SMR_ReadSector_Len <= NC->CylinderSizeSector);
	if (SMR_ReadSector_Len > 0){
		qsort(SMR_ReadSector, SMR_ReadSector_Len, sizeof(DWORD), compare);
		if (SMR_ReadSector_Len > 1){ for (j = 0; j < SMR_ReadSector_Len - 1; j++){ assert(SMR_ReadSector[j] <= SMR_ReadSector[j + 1]); } }
		test(simPtr, (I64)SMR_ReadSector[0], 20);
		Calc_Rotate_SkewSMR(simPtr, (I64)SMR_ReadSector[0], 21);
		for (j = 1; j < SMR_ReadSector_Len; j++){//電梯法回SLB讀資料
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3, R:20
			test(simPtr, (I64)SMR_ReadSector[j], 20);//3
			if (SMR_ReadSector[j - 1] / NC->trackSizeSector != SMR_ReadSector[j] / NC->trackSizeSector){
				Calc_Rotate_SkewSMR(simPtr, (I64)SMR_ReadSector[j], 21); //Calc_RotateOverhead2(simPtr, 0, 4);
			}
		}
	}
	assert(SMR_ReadSector_Len <= lenSector);
	N->ReqR_pageRead += lenSector;//更新統計資料
	//transfer time
	N->content_rotate += lenSector;
	N->R_content_rotate += lenSector;
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
#endif
	//Resp
	//assert(FirstSection == 0); 
	assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime; 
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
	//Read Response Time
	N->R_RunTime += N->Req_RunTime;
	N->R_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->R_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

	free(SMR_ReadSector);
}

//void DM_SMRreadSector2(sim *simPtr, I64 offsetSector, DWORD lenSector){
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
//	DWORD j;
//	DWORD SectorTableNo, block_no, section_no, LBsector;
//	DWORD *SMR_ReadSector = (DWORD*)calloc(lenSector, sizeof(DWORD));
//	DWORD SMR_ReadSector_Len = 0;
//	//用sector unit計算
//	I64 tmp_sector = offsetSector;
//	const I64 start_offsetSector = offsetSector, end_offsetSector = offsetSector + lenSector - 1;
//	//初始化Req_RunTime
//	N->Req_RunTime = 0;
//#ifdef Time_Overhead
//	while (tmp_sector <= end_offsetSector){
//		SectorTableNo = (DWORD)tmp_sector;
//		block_no = SectorTableNo / NC->blockSizeSector; section_no = SectorTableNo % NC->blockSizeSector;
//		//判斷Sector在Data Band或SLB
//		assert(N->DBalloc[block_no] == 0 || N->DBalloc[block_no] == 1);
//		if (N->DBalloc[block_no] == 1 && N->blocks[block_no].sections[section_no].valid == 0){
//			LBsector = N->blocks[block_no].sections[section_no].sector;
//			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
//			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
//			//裝進SMR_ReadSector, 等結束做排序小到大, 再全部Read
//			assert(NC->LsizeSector + LBsector < NC->PsizeSector); assert(SMR_ReadSector_Len < lenSector);
//			SMR_ReadSector[SMR_ReadSector_Len] = NC->LsizeSector + LBsector; SMR_ReadSector_Len++;
//		}
//		else{
//			assert(SMR_ReadSector_Len < lenSector);
//			SMR_ReadSector[SMR_ReadSector_Len] = SectorTableNo; SMR_ReadSector_Len++;
//		}
//		tmp_sector++;
//	}
//	//讀DB+LB資料 //acctime
//	assert(SMR_ReadSector_Len == lenSector);
//	qsort(SMR_ReadSector, SMR_ReadSector_Len, sizeof(DWORD), compare);
//	if (SMR_ReadSector_Len > 1){ for (j = 0; j < SMR_ReadSector_Len - 1; j++){ assert(SMR_ReadSector[j] <= SMR_ReadSector[j + 1]); } }
//	//算seek, rotation時間
//	test(simPtr, (I64)SMR_ReadSector[0], 20);
//	if ((I64)SMR_ReadSector[0] < NC->LsizeSector){ Calc_Rotate_SkewSMR(simPtr, (I64)SMR_ReadSector[0], 20); }
//	else{ Calc_Rotate_SkewSMR(simPtr, (I64)SMR_ReadSector[0], 21); }
//	for (j = 1; j < SMR_ReadSector_Len; j++){//電梯法回SLB讀資料
//		if (SMR_ReadSector[j - 1] / NC->trackSizeSector != SMR_ReadSector[j] / NC->trackSizeSector){
//			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3, R:20
//			test(simPtr, (I64)SMR_ReadSector[j], 20);
//			if ((I64)SMR_ReadSector[j] < NC->LsizeSector){ Calc_Rotate_SkewSMR(simPtr, (I64)SMR_ReadSector[j], 20); }//Calc_RotateOverhead2(simPtr, 0, 4);
//			else{ Calc_Rotate_SkewSMR(simPtr, (I64)SMR_ReadSector[j], 21); }
//		}
//	}
//	//更新統計資料
//	N->ReqR_pageRead += lenSector;
//	//transfer time
//	N->content_rotate += lenSector;
//	N->R_content_rotate += lenSector;
//	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
//#endif
//	//Resp
//	//assert(FirstSection == 0); 
//	assert(N->Req_RunTime >= 2);
//	N->RunTime += N->Req_RunTime;
//	N->Finish_Time += N->Req_RunTime; 
//	assert(N->Finish_Time - N->Arr_Time >= 2);
//	N->RespTIme += N->Finish_Time - N->Arr_Time;
//	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
//	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
//	//Read Response Time
//	N->R_RunTime += N->Req_RunTime;
//	N->R_RespTIme += (N->Finish_Time - N->Arr_Time);
//	N->R_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
//
//	free(SMR_ReadSector);
//}
void DM_SMRreadSector2(sim *simPtr, I64 offsetSector, DWORD lenSector){
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); 
	DWORD block_no, section_no, LBsector;
	DWORD i, j;
	//用sector unit計算
	const I64 end_sector = offsetSector + lenSector - 1; assert(offsetSector <= end_sector);
	I64 tmp_sector = offsetSector, pre_tmp_sector = -1; assert(pre_tmp_sector < 0);
	int P_Track, preP_Track = -1; assert(preP_Track < 0);//
	//Read Buffer
	const DWORD start_Track = (DWORD)offsetSector / NC->trackSizeSector, end_Track = (DWORD)(offsetSector + lenSector - 1) / NC->trackSizeSector;
	const DWORD Max_Len = NC->CylinderSizeSector + end_Track + 1 - start_Track;
	DWORD SMR_ReadSector_Len = 0;
	DWORD *SMR_ReadSector = (DWORD*)calloc(Max_Len, sizeof(DWORD));
	//初始化Req_RunTime
	N->Req_RunTime = 0;
#ifdef Time_Overhead
	while (tmp_sector <= end_sector){
		block_no = (DWORD)tmp_sector / NC->blockSizeSector; section_no = (DWORD)tmp_sector % NC->blockSizeSector;

#ifdef SLB_Mapping_Cache
		//find in cache
		//printf("%I64u\n", tmp_sector);
		if (N->DBalloc[block_no] == 1 && N->blocks[block_no].sections[section_no].valid != 1)
			CacheAccess(simPtr, tmp_sector, N->blocks[block_no].sections[section_no].sector, 1);
#endif 

		//判斷Sector在Data Band或SLB
		assert(N->DBalloc[block_no] == 0 || N->DBalloc[block_no] == 1);
		if (N->DBalloc[block_no] == 1 && N->blocks[block_no].sections[section_no].valid == 0){
			LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
			//裝進SMR_ReadSector, 等結束做排序小到大, 再全部Read
			assert(NC->LsizeSector + LBsector < NC->PsizeSector); assert(SMR_ReadSector_Len < Max_Len);
			SMR_ReadSector[SMR_ReadSector_Len] = NC->LsizeSector + LBsector; SMR_ReadSector_Len++;
		}
		else{ //assert(SMR_ReadSector_Len < lenSector); //SMR_ReadSector[SMR_ReadSector_Len] = SectorTableNo; SMR_ReadSector_Len++;
			/*if (pre_tmp_sector == -1 || tmp_sector / NC->trackSizeSector != pre_tmp_sector / NC->trackSizeSector){
				test(simPtr, tmp_sector, 20);
				Calc_Rotate_SkewSMR(simPtr, tmp_sector, 20);
				pre_tmp_sector = tmp_sector; assert(pre_tmp_sector >= 0);
			}*/
#ifdef Simulation_SLB 
			//SLB
			if (pre_tmp_sector == -1 || tmp_sector / NC->trackSizeSector != pre_tmp_sector / NC->trackSizeSector){
				test(simPtr, tmp_sector, 20);
				Calc_Rotate_SkewSMR(simPtr, tmp_sector, 20);
				pre_tmp_sector = tmp_sector; assert(pre_tmp_sector >= 0);
			}
#elif Simulation_SLB_SHUFFLE
			//Shuffle, Delay
			if (N->SHUFFLE_TL_Alloc[block_no] == 1){
#ifdef SHUFFLE_CYLINDER
				//L address經由TL層的L2P得到P_Index
				DWORD L_Index = section_no / NC->CylinderSizeSector; DWORD P_Index = N->SHUFFLE_TL[block_no].L2P_Index[L_Index];
				assert(P_Index < NC->blockSizeCylinder);//P_Index在正常範圍
				assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == L_Index);//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
				P_Track = (int)(P_Index * NC->CylinderSizeTrack + section_no / NC->trackSizeSector % NC->CylinderSizeTrack);
#elif SHUFFLE_TRACK
				//L address經由TL層的L2P得到P_Index
				DWORD L_Index = section_no / NC->trackSizeSector; DWORD P_Index = N->SHUFFLE_TL[block_no].L2P_Index[L_Index];
				assert(P_Index < NC->blockSizeTrack);//P_Index在正常範圍
				assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == L_Index);//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
				P_Track = (int)P_Index;
#endif

				assert(0 <= P_Track); assert((DWORD)P_Track < NC->blockSizeTrack);
				if (preP_Track == -1 || P_Track != preP_Track){
					//裝進SMR_ReadSector, 等結束做排序小到大, 再全部Read
					DWORD Shuffle_L2P_Sector = block_no*NC->blockSizeSector + (DWORD)P_Track*NC->trackSizeSector;
					assert(Shuffle_L2P_Sector < NC->LsizeSector); assert(SMR_ReadSector_Len < Max_Len);
					SMR_ReadSector[SMR_ReadSector_Len] = Shuffle_L2P_Sector; SMR_ReadSector_Len++;

					preP_Track = P_Track; assert(preP_Track >= 0);
				}
			}
			else{//如果if和else都做表示跨越Band
				if (pre_tmp_sector == -1 || tmp_sector / NC->trackSizeSector != pre_tmp_sector / NC->trackSizeSector){
					test(simPtr, tmp_sector, 20);
					Calc_Rotate_SkewSMR(simPtr, tmp_sector, 20);
					pre_tmp_sector = tmp_sector; assert(pre_tmp_sector >= 0);
				}
			}
#endif 
#ifdef SACE
			//SLB
			if (pre_tmp_sector == -1 || tmp_sector / NC->trackSizeSector != pre_tmp_sector / NC->trackSizeSector){
				test(simPtr, tmp_sector, 20);
				Calc_Rotate_SkewSMR(simPtr, tmp_sector, 20);
				pre_tmp_sector = tmp_sector; assert(pre_tmp_sector >= 0);
			}
#endif
		}
		tmp_sector++;
	}
	//讀暫存在SMR_ReadSector的資料: SLB資料或Shuffle Band資料 //acctime
	assert(SMR_ReadSector_Len <= Max_Len);
	if (SMR_ReadSector_Len > 0){
		//qsort(SMR_ReadSector, SMR_ReadSector_Len, sizeof(DWORD), compare);
		for (i = 0; i < SMR_ReadSector_Len - 1; i++){
			for (j = i + 1; j < SMR_ReadSector_Len; j++){
				if (SMR_ReadSector[i] > SMR_ReadSector[j]){ DWORD tmp = SMR_ReadSector[j]; SMR_ReadSector[j] = SMR_ReadSector[i]; SMR_ReadSector[i] = tmp; }
			}
		}
		if (SMR_ReadSector_Len > 1){ for (j = 0; j < SMR_ReadSector_Len - 1; j++){ assert(SMR_ReadSector[j] <= SMR_ReadSector[j + 1]); } }
		//算seek, rotation時間
		test(simPtr, (I64)SMR_ReadSector[0], 20); Calc_Rotate_SkewSMR(simPtr, (I64)SMR_ReadSector[0], 21);
		for (j = 1; j < SMR_ReadSector_Len; j++){//電梯法回SLB讀資料
			if (SMR_ReadSector[j - 1] / NC->trackSizeSector != SMR_ReadSector[j] / NC->trackSizeSector){
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3, R:20
				test(simPtr, (I64)SMR_ReadSector[j], 20); Calc_Rotate_SkewSMR(simPtr, (I64)SMR_ReadSector[j], 21);//Calc_RotateOverhead2(simPtr, 0, 4);
			}
		}
	}
	//更新統計資料
	N->ReqR_pageRead += lenSector;
	//transfer time
	N->content_rotate += lenSector;
	N->R_content_rotate += lenSector;
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
#endif
	//Resp
	//assert(FirstSection == 0); 
	assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime; 
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
	//Read Response Time
	N->R_RunTime += N->Req_RunTime;
	N->R_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->R_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

	free(SMR_ReadSector);
}

#endif

#ifdef Simulation_HDD
void HDD(sim *simPtr, I64 offsetSector, DWORD lenSector){
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);

	I64 SectorTableNo = offsetSector / NC->trackSizePage * NC->trackSizePage;
	assert(offsetSector - offsetSector % NC->trackSizePage == SectorTableNo);
	assert(SectorTableNo <= offsetSector);
	assert(0 <= SectorTableNo); assert(SectorTableNo < NC->LsizeSector);

	const I64 start_section = offsetSector / NC->Section;
	const I64 end_section = (offsetSector + lenSector - 1) / NC->Section;
	const I64 section_len = end_section - start_section + 1;
	I64 tmp_start = offsetSector / NC->Section;
	DWORD section2SLB = -1;
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section);
	assert(section_len >= 1);

	//use
	I64 start_sector = SectorTableNo, end_sector = offsetSector + lenSector - 1;
	DWORD zone_no = 0;

//#ifdef Time_Overhead
//	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//	//Calc_TimeOverhead2(simPtr, start_sector, 0); // NC->CylinderSizeTrack
//	Seek_Seagate_C15k5(simPtr, start_sector, 0);
//	Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
//	N->content_rotate += ((double)lenSector / N->Disk.AvgTrackSize);//寫多少sector轉多少sector
//#endif
//	N->NMR_Rotate++;
	while (start_sector <= end_sector){
#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		//Calc_TimeOverhead2(simPtr, start_sector, 0); // NC->CylinderSizeTrack
		Seek_Seagate_C15k5(simPtr, start_sector, 0);
		Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
		N->content_rotate += ((double)lenSector / N->Disk.AvgTrackSize);//寫多少sector轉多少sector
#endif
		N->NMR_Rotate++;
		start_sector += NC->trackSizePage;
	}


	SLB->pageWrite += lenSector;
	//transfer
	N->Req_RunTime += ((double)lenSector / N->Disk.AvgTrackSize * 10);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

	////old
	//DWORD start_sector = offsetSector, end_sector = offsetSector + lenSector - 1;
	////Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	////Calc_TimeOverhead2_old(simPtr, start_sector, 0);
	//Calc_TimeOverhead2(simPtr, start_sector, 0);
	//Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
	//N->content_rotate += lenSector;// lenSector;//寫多少sector轉多少sector
	//N->NMR_Rotate++;
	//start_sector++;
	//SLB->pageWrite++;
	//while (start_sector <= end_sector){
	//	I64 pre_HeadTrack = N->pre_HeadTrack;
	//	//Calc_TimeOverhead2_old(simPtr, start_sector, 0);
	//	Calc_TimeOverhead2(simPtr, start_sector, 0);
	//	if ((start_sector - 1) / NC->trackSizePage != start_sector / NC->trackSizePage){
	//		Calc_RotateOverhead2(simPtr, 0, 0);
	//	}
	//	start_sector++;
	//	SLB->pageWrite++;
	//}
}

void HDD_RAM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//section - group out//紀錄各track上的section寫入量 有RMW
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	//試驗
	DWORD LRUmove = 0;
#ifdef Time_Overhead
	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
#endif
	DWORD *tmp_NSLB_RAM = NULL;
	DWORD tmp_NSLB_RAM_request = 0;
	const DWORD start_section = offsetSector / NC->Section;
	const DWORD end_section = (offsetSector + lenSector - 1) / NC->Section;
	const DWORD section_len = end_section - start_section + 1;
	DWORD tmp_start = offsetSector / NC->Section;
	DWORD section2SLB = -1;

	DWORD LRU_band = 0, old_NSLB_RAM_request = 0;
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section);
	assert(section_len >= 1);

	N->RAM_WAIT = 1;

	if (N->NSLB_RAM_request + section_len > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
		while (N->NSLB_RAM_request + section_len > N->NSLB_RAM_SIZE){
			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
			LRU_band = N->NSLB_RAM[0] * NC->Section / NC->blockSizeSector;
			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
			for (i = 0; i < N->NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] * NC->Section / NC->blockSizeSector == LRU_band){
					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
					N->NSLB_RAM[i] = -1;//原RAM失效

					//if (N->NSLB_RAM_request - tmp_NSLB_RAM_request + section_len <= N->NSLB_RAM_SIZE){ break; }
				}
			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
			//用section mapping寫入NSLB
			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
			if (tmp_NSLB_RAM_request > 1){ for (i = 1; i < tmp_NSLB_RAM_request; i++){ assert(tmp_NSLB_RAM[i - 1] <= tmp_NSLB_RAM[i]); } }
			for (i = 0; i < tmp_NSLB_RAM_request; i++){
				//assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1);
				assert(tmp_NSLB_RAM[i] != -1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
#ifdef Time_Overhead
				assert(0 <= N->WriteBuff_section);
				N->WriteBuff[N->WriteBuff_section] = tmp_NSLB_RAM[i] * NC->Section; N->RDB++;//在DB //WriteBuff裝各section的第一個sector的SectorTableNo
				N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
				if (N->WriteBuff_section == N->WriteBuff_SIZE){
					//for (j = 0; j < N->WriteBuff_section - 1; j++){ assert(N->WriteBuff[j] <= N->WriteBuff[j + 1]); }
					//DWORD cur_track = N->WriteBuff[0] / NC->trackSizePage;

					//DWORD s = 0, e = 0;
					//qsort(N->WriteBuff, N->WriteBuff_section, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
					//if (N->WriteBuff_section > 1){ for (j = 1; j < N->WriteBuff_section; j++){ assert(N->WriteBuff[j - 1] <= N->WriteBuff[j]); } }

					DWORD min = N->WriteBuff[0], max = N->WriteBuff[0];
					for (j = 1; j < N->WriteBuff_section; j++){
						if (N->WriteBuff[j] > max){ max = N->WriteBuff[j]; }
						if (N->WriteBuff[j] < min){ min = N->WriteBuff[j]; }
					}
					N->content_rotate += ((max - min + 1)*NC->Section);
					N->Wait_content_rotate += ((max - min + 1)*NC->Section);

					Calc_TimeOverhead2(simPtr, N->WriteBuff[0], 0);
					//Calc_TimeOverhead3(simPtr, N->WriteBuff[0], 0);
					Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
					//s = N->WriteBuff[0];
					N->WriteBuff_WriteOut++;//
					N->NMR_Rotate++;
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
					for (j = 1; j < N->WriteBuff_section; j++){
						Calc_TimeOverhead2(simPtr, N->WriteBuff[j], 0);
						//Calc_TimeOverhead3(simPtr, N->WriteBuff[j], 0);
						if (N->WriteBuff[j - 1] / NC->trackSizePage != N->WriteBuff[j] / NC->trackSizePage){
							Calc_RotateOverhead2(simPtr, 0, 0);//read
							//e = N->WriteBuff[j - 1];
							//N->writesector_rotate += ((e - s + 1)*NC->Section);//寫多少sector轉多少sector
							//s = N->WriteBuff[j];
							N->NMR_Rotate++;
						}
						N->WriteBuff_WriteOut++;//
					}
					//N->writesector_rotate += ((N->WriteBuff[N->WriteBuff_section - 1] - s + 1)*NC->Section);//寫多少sector轉多少sector
					N->WriteBuff_section = 0;
					N->WriteBuff_writecount++;//
				}
#endif
				N->RAM_WriteOut += NC->Section;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;
				SLB->pageWrite += NC->Section;
			}
			//更新NSLB_RAM_request資訊
			old_NSLB_RAM_request = N->NSLB_RAM_request;
			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
			//整理NSLB_RAM，清掉-1的資料
			j = 0;
			for (i = 0; i < old_NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
			}assert(j == N->NSLB_RAM_request);
			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
			free(tmp_NSLB_RAM);
		}assert(N->NSLB_RAM_request + section_len <= N->NSLB_RAM_SIZE);
	}
	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);

	//request切成section送進RAM
	tmp_start = start_section;//offsetSector / NC->Section;// 
	DWORD cur_track = 0;
	while (tmp_start <= end_section){
		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
		assert(N->RAM_Valid[tmp_start] == 0 || N->RAM_Valid[tmp_start] == 1);

		if (N->RAM_Valid[tmp_start] == 0){// skip == 0
			//N->RAM_Valid[tmp_start] = 1;
			//N->NSLB_RAM[N->NSLB_RAM_request] = tmp_start;
			//N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			N->RAM_Miss++;
			//N->req_write_section++;

			////遇到section miss去DB做RMW
			//if (cur_track == 0 || cur_track!=tmp_start*NC->Section / NC->trackSizePage){
			//	cur_track = tmp_start*NC->Section / NC->trackSizePage;
			//	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//	Calc_TimeOverhead2(simPtr, tmp_start*NC->Section, 1);
			//	Calc_RotateOverhead2(simPtr, 0, 1);//read
			//}
		}
		else{
			assert(N->RAM_Valid[tmp_start] == 1);
			N->RAM_Hit++;
		}

		N->RAM_Valid[tmp_start] = 1;
		N->NSLB_RAM[N->NSLB_RAM_request] = tmp_start;
		N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
		N->req_write_section++;

		tmp_start++;
		N->Host2RAM++;
	}
}
void HDD_RAM_WriteAllOut(sim *simPtr)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	DWORD i = 0, j = 0;
	DWORD start_section = 0, Section_Update = 0;
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);

	N->RAM_WAIT = 0;

	if (N->NSLB_RAM_request > 0){
#ifdef Time_Overhead
		DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
		assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE); //qsort(N->NSLB_RAM, N->NSLB_RAM_request, sizeof(DWORD), compare);
		qsort(N->NSLB_RAM, N->NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
		if (N->NSLB_RAM_request > 1){ for (i = 0; i < N->NSLB_RAM_request - 1; i++){ assert(N->NSLB_RAM[i] <= N->NSLB_RAM[i + 1]); } }
		for (i = 0; i < N->NSLB_RAM_request; i++){

			//assert(N->RAM_Valid[N->NSLB_RAM[i]] == 1);
			//assert(N->NSLB_RAM[i] != -1); N->RAM_Valid[N->NSLB_RAM[i]] = 0;
			//Calc_TimeOverhead2(simPtr, N->NSLB_RAM[i] * NC->Section, 0); // NC->CylinderSizeTrack

			assert(0 <= N->WriteBuff_section);
			N->WriteBuff[N->WriteBuff_section] = N->NSLB_RAM[i] * NC->Section; N->RDB++;//在DB
			N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
			if (N->WriteBuff_section == N->WriteBuff_SIZE){
				Calc_TimeOverhead2(simPtr, N->WriteBuff[0], 0);
				//Calc_TimeOverhead3(simPtr, N->WriteBuff[0], 0);
				Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
				N->content_rotate += (N->WriteBuff_section*NC->Section);//寫多少sector轉多少sector

				N->WriteBuff_WriteOut++;//
				N->NMR_Rotate++;
				//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
				for (j = 1; j < N->WriteBuff_section; j++){
					Calc_TimeOverhead2(simPtr, N->WriteBuff[j], 0);
					//Calc_TimeOverhead3(simPtr, N->WriteBuff[j], 0);
					if (N->WriteBuff[j - 1] / NC->trackSizePage != N->WriteBuff[j] / NC->trackSizePage){
						Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
						N->NMR_Rotate++;
					}
					N->WriteBuff_WriteOut++;//
				}
				N->WriteBuff_section = 0;
				N->WriteBuff_writecount++;//
			}

			N->RAM_WriteOut += NC->Section;
			SLB->pageWrite += NC->Section;
			N->NSLB_RAM[i] = -1;
		}
		N->NSLB_RAM_request = 0;
		if (N->WriteBuff_section > 0){
			Calc_TimeOverhead2(simPtr, N->WriteBuff[0], 0);
			//Calc_TimeOverhead3(simPtr, N->WriteBuff[0], 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			N->content_rotate += (N->WriteBuff_section*NC->Section);//寫多少sector轉多少sector

			N->WriteBuff_WriteOut++;//
			N->NMR_Rotate++;
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			for (j = 1; j < N->WriteBuff_section; j++){
				Calc_TimeOverhead2(simPtr, N->WriteBuff[j], 0);
				//Calc_TimeOverhead3(simPtr, N->WriteBuff[j], 0);
				if (N->WriteBuff[j - 1] / NC->trackSizePage != N->WriteBuff[j] / NC->trackSizePage){
					Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
					N->NMR_Rotate++;
				}
				N->WriteBuff_WriteOut++;//
			}
			N->WriteBuff_section = 0;
			N->WriteBuff_writecount++;//
		}
#endif
	}
	assert(N->NSLB_RAM_request == 0);
}
#endif

#ifdef Simulation_SLB
//void FASTwriteSectorSLBpart2(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
//	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
//	DWORD cur_8subband = -1, prev_8subband = -1;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD i = 0;
//	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
//	DWORD block_no = 0, page_no = 0;
//	DWORD curSectionRMW = 0, curSection = -1;
//	DWORD oldData_PTrack = 0;
//	DWORD goRMW = 0, OneTimeRotation = 0;
//	DWORD goDB = 0;
//	while (1) {
//		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector / NC->pageSizeSector;
//		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
//		if (SLB->lastRWpage == SLB->guardTrack1stPage){//當前空間已寫滿
//			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage){//發現guard track右邊是fRW，只能merge
//				assert(SLB->writedPageNum == SLB->SLBpage_num);
//				FASTmergeRWSLBpart2(simPtr);
//			}
//			else{//guard track右邊不是fRW，右移gt一格
//				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++){
//					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
//					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
//				}
//				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
//				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++){
//					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
//					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
//				}
//			}
//		}
//		assert(SLB->lastRWpage >= 0); assert(SLB->lastRWpage <= SLB->partial_page);
//		assert(offsetSector <  NC->LsizePage); assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);//
//		N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
//		N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;
//
//		//invalid the same page(old page) in log buffer or in data band
//		block_no = getblock(simPtr, SectorTableNo);
//		page_no = getpage(simPtr, SectorTableNo);
//		assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
//		if (block_no == NC->PsizeBlock - 1){
//			assert(0 <= page_no && page_no <= SLB->partial_page);
//			assert(N->blocks[block_no].pages[page_no].valid == 1);
//			N->blocks[block_no].pages[page_no].valid = 0;
//			N->SLB_Hit++;
//			N->cycleSLB_Hit++;
//		}
//		else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
//			assert(N->blocks[block_no].pages[page_no].valid == 1);
//			N->blocks[block_no].pages[page_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
//			N->SLB_Miss++;
//			N->cycleSLB_Miss++;
//		}//in Data Band
//		//更新L2P mapping table
//		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
//		setpage(simPtr, SectorTableNo, SLB->lastRWpage);
//		//更新寫入量統計資料
//		SLB->pageWrite++; SLB->writedPageNum++;
//		SLB->write_count++;
//		SLB->lastRWpage = (SLB->lastRWpage + 1) % (SLB->partial_page + 1);
//
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0){ break; }
//	}
//}
//void FASTwriteSectorSLBRAM2_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
//	DWORD i = 0, j = 0;
//	//section mapping
//	DWORD start_section = -1;
//	//試驗
//	DWORD LRUmove = 0;
//#ifdef Time_Overhead
//	DWORD WriteBuff_Size = 0;
//	DWORD *WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//	DWORD RMW_WriteBuff_Size = 0;
//	DWORD *RMW_WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//#endif
//#ifdef SLB_RAM_LRUsector_Group
//	//sector LRU + hit
//	DWORD skip = 0;
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	DWORD LRU_band = -1;
//	DWORD old_NSLB_RAM_request = 0;
//	//DWORD start = 0;
//
//	if (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
//		while (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){
//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
//			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
//			for (i = 0; i < N->NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//					N->NSLB_RAM[i] = -1;//原RAM失效
//				}
//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//
//			//用section mapping寫入NSLB //assert(tmp_NSLB_RAM[0] == N->NSLB_RAM[0]);
//			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//			assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
//			assert(tmp_NSLB_RAM[0] != -1); start_section = tmp_NSLB_RAM[0] / N->SubTrack;
//			//fprintf(fp_CR, "%lu ", start_section);
//#ifdef Time_Overhead
//			WriteBuff_Size = 0; RMW_WriteBuff_Size = 0;
//			SectorTableNo = start_section*N->SubTrack;
//			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//			if (0 <= block_no && block_no < NC->LsizeBlock){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//			else{ RMW_WriteBuff[RMW_WriteBuff_Size] = NC->LsizeSector + page_no; RMW_WriteBuff_Size++; assert(RMW_WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB //block_no*NC->blockSizeSector + 
//#endif
//			FASTwriteSectorSLBpart2(simPtr, start_section*N->SubTrack, N->SubTrack); //tmp_NSLB_RAM做寫入
//
//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
//				assert(tmp_NSLB_RAM[i] != -1);
//				assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;//更新strat_sector
//					//fprintf(fp_CR, "%lu ", start_section);
//#ifdef Time_Overhead
//					SectorTableNo = start_section*N->SubTrack;
//					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//					if (0 <= block_no && block_no < NC->LsizeBlock){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//					else{ RMW_WriteBuff[RMW_WriteBuff_Size] = NC->LsizeSector + page_no; RMW_WriteBuff_Size++; assert(RMW_WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB //block_no*NC->blockSizeSector + 
//
//					/*if (WriteBuff_Size + RMW_WriteBuff_Size >= NC->trackSizePage / N->SubTrack){
//						SLB2_CalculateTime(simPtr, WriteBuff, WriteBuff_Size, RMW_WriteBuff, RMW_WriteBuff_Size);
//						WriteBuff_Size = 0; RMW_WriteBuff_Size = 0;
//					}*/
//#endif
//					FASTwriteSectorSLBpart2(simPtr, start_section*N->SubTrack, N->SubTrack); //tmp_NSLB_RAM做寫入
//				}
//			}
//
//			/*if (WriteBuff_Size + RMW_WriteBuff_Size > 0){
//				SLB2_CalculateTime(simPtr, WriteBuff, WriteBuff_Size, RMW_WriteBuff, RMW_WriteBuff_Size);
//			}
//			WriteBuff_Size = 0; RMW_WriteBuff_Size = 0;*/
//#ifdef Time_Overhead
//			assert(0 < WriteBuff_Size + RMW_WriteBuff_Size);  assert(WriteBuff_Size + RMW_WriteBuff_Size <= N->NSLB_RAM_SIZE);
//			qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
//			if (WriteBuff_Size > 1){ for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); } }
//			Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
//			for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//				if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){
//					//seek
//					Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//				}
//			}
//			if (RMW_WriteBuff_Size > 0){//做RMW
//				qsort(RMW_WriteBuff, RMW_WriteBuff_Size, sizeof(DWORD), compare);
//				if (RMW_WriteBuff_Size > 1){ for (i = 0; i < RMW_WriteBuff_Size - 1; i++){ assert(RMW_WriteBuff[i] <= RMW_WriteBuff[i + 1]); } }
//				Calc_TimeOverhead(simPtr, RMW_WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
//				for (i = 1; i < RMW_WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//					if (RMW_WriteBuff[i - 1] / NC->trackSizePage != RMW_WriteBuff[i] / NC->trackSizePage){
//						//seek
//						Calc_TimeOverhead(simPtr, RMW_WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//					}
//				}
//			}
//#endif			
//
//			//統計RAM Hit吸收的寫入
//			N->RAM_WriteOut += tmp_NSLB_RAM_request;
//
//			//更新NSLB_RAM_request資訊
//			old_NSLB_RAM_request = N->NSLB_RAM_request;
//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//			//整理NSLB_RAM，清掉-1的資料
//			j = 0;
//			for (i = 0; i < old_NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
//			}assert(j == N->NSLB_RAM_request);
//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
//			free(tmp_NSLB_RAM);
//		}assert(N->NSLB_RAM_request + lenSector <= N->NSLB_RAM_SIZE);
//	}
//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//	while (1) {
//		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		if (N->RAM_Valid[offsetSector] == 0){// skip == 0
//			N->RAM_Valid[offsetSector] = 1;
//			N->NSLB_RAM[N->NSLB_RAM_request] = offsetSector;
//			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		}
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0) break;
//	}
//#endif//LRUsector找Group //之前叫NSLB_RAM_FastSectLRU_HIT //使用
//#ifdef Time_Overhead
//	free(WriteBuff);
//	free(RMW_WriteBuff);
//#endif
//}
//void FASTwriteSectorSLBRAM2_WriteAllOut(sim *simPtr)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	DWORD i = 0, j = 0;
//	DWORD start_section = 0;
//
//	//sector LRU + hit
//	DWORD skip = 0;
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	DWORD LRU_band = -1;
//	DWORD old_NSLB_RAM_request = 0;
//	if (N->NSLB_RAM_request > 0){
//
//		/*qsort(N->NSLB_RAM, N->NSLB_RAM_request, sizeof(DWORD), compare);
//		assert(N->NSLB_RAM[0] != -1); assert(N->RAM_Valid[N->NSLB_RAM[0]] == 1); N->RAM_Valid[N->NSLB_RAM[0]] = 0;
//		start_section = N->NSLB_RAM[0] / N->SubTrack;
//		N->NSLB_RAM[0] = -1;
//		FASTwriteSectorSLBpart2(simPtr, start_section*N->SubTrack, N->SubTrack);
//		for (i = 1; i < N->NSLB_RAM_request; i++){
//			assert(N->NSLB_RAM[i] != -1); assert(N->RAM_Valid[N->NSLB_RAM[i]] == 1); N->RAM_Valid[N->NSLB_RAM[i]] = 0;
//			if (N->NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//				FASTwriteSectorSLBpart2(simPtr, start_section*N->SubTrack, N->SubTrack);
//				start_section = N->NSLB_RAM[i] / N->SubTrack;
//			}
//			N->NSLB_RAM[i] = -1;
//		}
//		N->NSLB_RAM_request = 0;*/
//
//#ifdef Time_Overhead
//		DWORD WriteBuff_Size = 0;
//		DWORD *WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//		DWORD RMW_WriteBuff_Size = 0;
//		DWORD *RMW_WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//		DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//#endif
//		while (N->NSLB_RAM_request > 0){
//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
//			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
//			for (i = 0; i < N->NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//					N->NSLB_RAM[i] = -1;//原RAM失效
//				}
//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//
//			//用section mapping寫入NSLB //assert(tmp_NSLB_RAM[0] == N->NSLB_RAM[0]);
//			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//			assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
//			assert(tmp_NSLB_RAM[0] != -1); start_section = tmp_NSLB_RAM[0] / N->SubTrack;
//			//fprintf(fp_CR, "%lu ", start_section);
//#ifdef Time_Overhead
//			SectorTableNo = start_section*N->SubTrack;
//			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//			if (0 <= block_no && block_no < NC->LsizeBlock){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//			else{ RMW_WriteBuff[RMW_WriteBuff_Size] = NC->LsizeSector + page_no; RMW_WriteBuff_Size++; assert(RMW_WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB //block_no*NC->blockSizeSector + 
//#endif
//			FASTwriteSectorSLBpart2(simPtr, start_section*N->SubTrack, N->SubTrack); //tmp_NSLB_RAM做寫入
//
//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
//				assert(tmp_NSLB_RAM[i] != -1);
//				assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;//更新strat_sector
//					//fprintf(fp_CR, "%lu ", start_section);
//#ifdef Time_Overhead
//					SectorTableNo = start_section*N->SubTrack;
//					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//					if (0 <= block_no && block_no < NC->LsizeBlock){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//					else{ RMW_WriteBuff[RMW_WriteBuff_Size] = NC->LsizeSector + page_no; RMW_WriteBuff_Size++; assert(RMW_WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB //block_no*NC->blockSizeSector + 
//
//					/*if (WriteBuff_Size + RMW_WriteBuff_Size >= NC->trackSizePage / N->SubTrack){
//						SLB2_CalculateTime(simPtr, WriteBuff, WriteBuff_Size, RMW_WriteBuff, RMW_WriteBuff_Size);
//						WriteBuff_Size = 0; RMW_WriteBuff_Size = 0;
//					}*/
//#endif
//					FASTwriteSectorSLBpart2(simPtr, start_section*N->SubTrack, N->SubTrack); //tmp_NSLB_RAM做寫入
//				}
//			}
//			//統計RAM Hit吸收的寫入
//			N->RAM_WriteOut += tmp_NSLB_RAM_request;
//
//			//更新NSLB_RAM_request資訊
//			old_NSLB_RAM_request = N->NSLB_RAM_request;
//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//			//整理NSLB_RAM，清掉-1的資料
//			j = 0;
//			for (i = 0; i < old_NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
//			}assert(j == N->NSLB_RAM_request);
//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
//			free(tmp_NSLB_RAM);
//		}assert(N->NSLB_RAM_request == 0);
////#ifdef Time_Overhead
////		if (WriteBuff_Size + RMW_WriteBuff_Size > 0){
////			SLB2_CalculateTime(simPtr, WriteBuff, WriteBuff_Size, RMW_WriteBuff, RMW_WriteBuff_Size);
////		}
////		WriteBuff_Size = 0; RMW_WriteBuff_Size = 0;
////		free(WriteBuff);
////		free(RMW_WriteBuff);
////#endif
//
//#ifdef Time_Overhead
//		assert(0 < WriteBuff_Size + RMW_WriteBuff_Size);  assert(WriteBuff_Size + RMW_WriteBuff_Size <= N->NSLB_RAM_SIZE);
//		qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
//		if (WriteBuff_Size > 1){ for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); } }
//		Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
//		for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//			if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){
//				//seek
//				Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//			}
//		}
//		if (RMW_WriteBuff_Size > 0){//做RMW
//			qsort(RMW_WriteBuff, RMW_WriteBuff_Size, sizeof(DWORD), compare);
//			if (RMW_WriteBuff_Size > 1){ for (i = 0; i < RMW_WriteBuff_Size - 1; i++){ assert(RMW_WriteBuff[i] <= RMW_WriteBuff[i + 1]); } }
//			Calc_TimeOverhead(simPtr, RMW_WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
//			for (i = 1; i < RMW_WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//				if (RMW_WriteBuff[i - 1] / NC->trackSizePage != RMW_WriteBuff[i] / NC->trackSizePage){
//					//seek
//					Calc_TimeOverhead(simPtr, RMW_WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//				}
//			}
//		}
//		free(WriteBuff);
//		free(RMW_WriteBuff);
//#endif
//	}
//}

void SLB2_TrackRMWtime(sim *simPtr, DWORD Max_Size, DWORD type){
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	DWORD tmp = 0; DWORD CylinderGroup = 25;
#ifdef Time_Overhead
	assert(0 <= N->WriteBuff_section);  assert(N->WriteBuff_section <= Max_Size);
	qsort(N->WriteBuff, N->WriteBuff_section, sizeof(DWORD), compare);
	if (N->WriteBuff_section > 1){ for (i = 0; i < N->WriteBuff_section - 1; i++){ assert(N->WriteBuff[i] <= N->WriteBuff[i + 1]); } }

	//for (i = 0; i < N->WriteBuff_section; i++){
	//	for (j = i + 1; j < N->WriteBuff_section; j++){
	//		/*if ((N->WriteBuff[i] / NC->trackSizePage) / (CylinderGroup * NC->CylinderSizeTrack) * CylinderGroup + (N->WriteBuff[i] / NC->trackSizePage) % CylinderGroup > (N->WriteBuff[j] / NC->trackSizePage) / (CylinderGroup * NC->CylinderSizeTrack) * CylinderGroup + (N->WriteBuff[j] / NC->trackSizePage) % CylinderGroup){
	//			tmp = N->WriteBuff[i]; 
	//			N->WriteBuff[i] = N->WriteBuff[j];
	//			N->WriteBuff[j] = tmp;
	//		}*/
	//		if (N->WriteBuff[i]>N->WriteBuff[j]){
	//			tmp = N->WriteBuff[i];
	//			N->WriteBuff[i] = N->WriteBuff[j];
	//			N->WriteBuff[j] = tmp;
	//		}
	//	}
	//}
	//for (i = 0; i < N->WriteBuff_section; i++){ printf("%lu ", (N->WriteBuff[i] / NC->trackSizePage) / (CylinderGroup * NC->CylinderSizeTrack) * CylinderGroup + (N->WriteBuff[i] / NC->trackSizePage) % CylinderGroup); } printf("\n"); system("pause");

	N->content_rotate += ((double)N->WriteBuff_section / NC->trackSizeSection);//read
	N->WR_content_rotate += ((double)N->WriteBuff_section / NC->trackSizeSection);
	test(simPtr, (I64)N->WriteBuff[0], type);
	Calc_RotateOverhead2(simPtr, 0, type);
	for (i = 1; i < N->WriteBuff_section; i++){
		//type Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		//Calc_TimeOverhead2(simPtr, N->WriteBuff[i], type); //Calc_TimeOverhead3(simPtr, N->WriteBuff[i], 0);
		test(simPtr, (I64)N->WriteBuff[i], type);
		if (N->WriteBuff[i - 1] / NC->trackSizeSector != N->WriteBuff[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, type); }//1024
		//N->WriteBuff_WriteOut++;
	}
#endif //回SLB讀資料  //讀SLB消耗的rotation
}
void FASTwriteSectorSLBpart2(sim *simPtr, DWORD offsetSector, DWORD lenSector) {
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD *LRWSn = (DWORD*)calloc(NC->trackSizeSection, sizeof(DWORD));
	DWORD LRWSn_Num = 0;
	N->WriteBuff_section = 0;
#endif
	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//		if (SLB->lastRWpage == SLB->guardTrack1stPage){//當前空間已寫滿
		//			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage){//發現guard track右邊是fRW，只能merge
		//				assert(SLB->writedPageNum == SLB->SLBpage_num);
		//				FASTmergeRWSLBpart2(simPtr);
		//#ifdef Time_Overhead
		//				N->SLBtrackChange++;
		//#endif
		//			}
		//			else{//guard track右邊不是fRW，右移gt一格
		//				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++){
		//					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
		//					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
		//				}
		//				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
		//				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++){
		//					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
		//					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
		//				}
		//			}
		//		}
		//assert(SLB->lastRWpage >= 0); assert(SLB->lastRWpage <= SLB->partial_page);
		//assert(offsetSector <  NC->LsizePage); assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);//
		//N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
		//N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;

		if ((SLB->lastRWsection + (NC->CylinderSizeTrack - 1)*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
				//assert(SLB->writedSectionNum == SLB->SLBsection_num - NC->CylinderSizeTrack*NC->trackSizeSection);
				if (SLB->writedSectionNum != SLB->SLBsection_num){ 
					printf("%lu %lu\n", SLB->writedSectionNum, SLB->SLBsection_num);
					printf("%lu %lu %lu %lu\n", SLB->guardTrack1stSection, SLB->lastRWsection, SLB->firstRWsection, SLB->Partial_Section);
					system("pause"); 
				}
				FASTmergeRWSLBpart2(simPtr);
			}
			else{//guard track右邊不是fRW，右移gt一格
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x7fffffff;
					N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
					N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
				}
			}
		}
		assert(0 <= SLB->lastRWsection); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage); assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);//
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = getblock(simPtr, SectorTableNo);
		section_no = getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);

#ifdef Time_Overhead
		assert(0 <= N->WriteBuff_section); assert(N->blocks[block_no].sections[section_no].valid == 1);
		if (0 <= block_no && block_no < NC->LsizeBlock){ N->WriteBuff[N->WriteBuff_section] = SectorTableNo; N->RDB++; }//在DB
		else{ N->WriteBuff[N->WriteBuff_section] = NC->LsizeSector + section_no*NC->Section; N->RMW++; }//在NSLB
		N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);

		if (SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection + 1) / NC->trackSizeSection){
			assert(LRWSn_Num < NC->trackSizeSection);
			LRWSn[LRWSn_Num] = SLB->lastRWsection*NC->Section;
			LRWSn_Num++; 
		}
		if (lenSector - NC->Section == 0 || N->WriteBuff_section == N->WriteBuff_SIZE || SLB->writedSectionNum + 1 == SLB->SLBsection_num){
			assert(N->WriteBuff_section > 0);
			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1);
			for (i = 0; i < LRWSn_Num; i++){
				test(simPtr, (I64)NC->LsizeSector + LRWSn[i], 0);
				Calc_RotateOverhead2(simPtr, 0, 0);//SLB的firstRWtrack做寫入
			}
			LRWSn_Num = 0;

			N->WriteBuff_writecount++;//
			N->content_rotate += ((double)N->WriteBuff_section / NC->trackSizeSection);//write
			N->WW_content_rotate += ((double)N->WriteBuff_section / NC->trackSizeSection);///
			N->WriteBuff_section = 0;
		}

		//if (((lenSector - NC->Section == 0 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection + 1) / NC->trackSizeSection || SLB->writedSectionNum + 1 == SLB->SLBsection_num) && N->WriteBuff_section > 0) || N->WriteBuff_section == N->WriteBuff_SIZE){
		//	assert(N->WriteBuff_section > 0);
		//	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		//	SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1);
		//	//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);
		//	test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
		//	Calc_RotateOverhead2(simPtr, 0, 0);//SLB的firstRWtrack做寫入
		//	N->WriteBuff_writecount++;//
		//	N->content_rotate += ((double)N->WriteBuff_section / NC->trackSizeSection);//write
		//	N->WW_content_rotate += ((double)N->WriteBuff_section / NC->trackSizeSection);///
		//	N->WriteBuff_section = 0;
		//}
#endif
		if (block_no == NC->PsizeBlock - 1){
			assert(0 <= section_no && section_no <= SLB->Partial_Section);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
		}
		else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活section
			N->l2pmapOvd.AA++;
		}//in Data Band
		//更新L2P mapping table
		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageRead += NC->Section;
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;

		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
#ifdef Time_Overhead
	free(LRWSn);
#endif
//#ifdef Time_Overhead
//	if (N->WriteBuff_SIZE > 0){
//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3//N->WriteBuff_WriteOut += N->WriteBuff_section;
//		SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1);
//		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);
//		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
//		Calc_RotateOverhead2(simPtr, 0, 0);//SLB的firstRWtrack做寫入
//		N->WriteBuff_writecount++;//
//		N->content_rotate += ((double)N->WriteBuff_section / NC->trackSizeSection);//write
//		N->WW_content_rotate += ((double)N->WriteBuff_section / NC->trackSizeSection);///
//		N->WriteBuff_section = 0;
//	}
//#endif
}
void FASTwriteSectorSLBpart2_BitMap(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1;
	N->content_rotate += ((double)lenSector / NC->trackSizeSector);//寫多少sector轉多少sector
	N->WW_content_rotate += ((double)lenSector / NC->trackSizeSector);///
#endif
	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//		if (SLB->lastRWpage == SLB->guardTrack1stPage){//當前空間已寫滿
		//			if ((SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1) == SLB->firstRWpage){//發現guard track右邊是fRW，只能merge
		//				assert(SLB->writedPageNum == SLB->SLBpage_num);
		//				FASTmergeRWSLBpart2(simPtr);
		//#ifdef Time_Overhead
		//				N->SLBtrackChange++;
		//#endif
		//			}
		//			else{//guard track右邊不是fRW，右移gt一格
		//				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++){
		//					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;
		//					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
		//				}
		//				SLB->guardTrack1stPage = (SLB->guardTrack1stPage + NC->trackSizePage) % (SLB->partial_page + 1);
		//				for (i = SLB->guardTrack1stPage; i < SLB->guardTrack1stPage + NC->trackSizePage; i++){
		//					N->blocks[NC->PsizeBlock - 1].pages[i].sector = 0x7fffffff;//means the sector is not yet mapping to data band
		//					N->blocks[NC->PsizeBlock - 1].pages[i].valid = 0;
		//				}
		//			}
		//		}
		//assert(SLB->lastRWpage >= 0); assert(SLB->lastRWpage <= SLB->partial_page);
		//assert(offsetSector <  NC->LsizePage); assert(N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid == 0);//
		//N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].sector = offsetSector;
		//N->blocks[NC->PsizeBlock - 1].pages[SLB->lastRWpage].valid = 1;

		if (SLB->lastRWsection + (NC->CylinderSizeTrack - 1)*NC->trackSizeSection == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
				assert(SLB->writedSectionNum == SLB->SLBsection_num);
				//printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
				FASTmergeRWSLBpart2_BitMap(simPtr);
				//FirstSection = 1;
			}
			else{//guard track右邊不是fRW，右移gt一格
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;
					N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
					N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
				}
			}
		}
#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0); // NC->CylinderSizeTrack
		//Calc_TimeOverhead3(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);
		test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1)/ NC->trackSizeSection){
			FirstSection = 0;
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);

		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = getblock(simPtr, SectorTableNo);
		section_no = getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		if (block_no == NC->PsizeBlock - 1){
			assert(0 <= section_no && section_no <= SLB->Partial_Section);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
			N->blocks[block_no].sections[section_no].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
		}
		else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
		}//in Data Band
		N->l2pmapOvd.AA++;
		//更新L2P mapping table
		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
			printf("%lu ,", SLB->writedSectionNum);
			printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		//N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
}
void FASTwriteSectorSLBpart2_Sr(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1;
	N->content_rotate += ((double)lenSector / NC->trackSizeSector);//寫多少sector轉多少sector
	N->WW_content_rotate += ((double)lenSector / NC->trackSizeSector);///
#endif

	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//if (SLB->lastRWsection + (NC->CylinderSizeTrack - 1)*NC->trackSizeSection == SLB->guardTrack1stSection){//當前空間已寫滿
		//	if ((SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
		//		assert(SLB->writedSectionNum == SLB->SLBsection_num);
		//		//printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		//		FASTmergeRWSLBpart2_Sr(simPtr);
		//		//FirstSection = 1;
		//	}
		//	else{//guard track右邊不是fRW，右移gt一格
		//		for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
		//			N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;
		//			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
		//		}
		//		SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
		//		for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
		//			N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
		//			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
		//		}
		//	}
		//}
		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
				//assert(SLB->writedSectionNum == SLB->SLBsection_num);
				//|| N->l2pmapOvd.AA > 23000
				FASTmergeRWSLBpart2_Sr(simPtr);
				
			}
			else{//guard track右邊不是fRW，右移gt一格
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0x3fffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

		////計算同SLB cyl上平均寫多少不同DB
		//N->b[SectorTableNo / NC->blockSizeSector]=1;
		//if (SLB->lastRWsection / (NC->CylinderSizeTrack*NC->trackSizeSection) != (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1) / (NC->CylinderSizeTrack*NC->trackSizeSection)){
		//	for (i = 0; i < NC->LsizeBlock; i++){ 
		//		if (N->b[i] == 1){ N->DBs++; }
		//		N->b[i] = 0;
		//	}
		//	N->write_cyls++; 
		//}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = getblock(simPtr, SectorTableNo);
		section_no = getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		if (block_no == NC->PsizeBlock - 1){
			assert(0 <= section_no && section_no <= SLB->Partial_Section);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
			N->blocks[block_no].sections[section_no].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
		}
		else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			//有不同的section寫入
			//N->blocks[block_no].sections[section_no].unique = 1;
		}//in Data Band
		//N->section_cumu[SectorTableNo / NC->Section]++;
		//更新L2P mapping table
		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		//N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
}
void DM_SMRwriteSector(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
	N->content_rotate += ((double)lenSector / NC->trackSizeSector);//寫多少sector轉多少sector
	N->WW_content_rotate += ((double)lenSector / NC->trackSizeSector);///
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += ((double)lenSector / NC->trackSizeSector*10);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//if (SLB->lastRWsection + (NC->CylinderSizeTrack - 1)*NC->trackSizeSection == SLB->guardTrack1stSection){//當前空間已寫滿
		//	if ((SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
		//		assert(SLB->writedSectionNum == SLB->SLBsection_num);
		//		//printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		//		FASTmergeRWSLBpart2_Sr(simPtr);
		//		//FirstSection = 1;
		//	}
		//	else{//guard track右邊不是fRW，右移gt一格
		//		for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
		//			N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;
		//			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
		//		}
		//		SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
		//		for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
		//			N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
		//			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
		//		}
		//	}
		//}
		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//DWORD n1 = 23000, n2 = 34500; /**(I64)SLB->SLBsection_num / 50331648*/ /** (I64)SLB->SLBsection_num / 50331648;*/
			//|| N->SlotTable_Act > n1 || (N->SlotTable_Act + N->SlotTable_InAct > n2)
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
				//assert(SLB->writedSectionNum == SLB->SLBsection_num);
				
				//N->Merged_Band = 0;
				//while (N->Merged_Band < 2){ DM_SMRmerge(simPtr); }
				
				FASTmergeRWSLBpart2_Sr(simPtr);
				
				//static int cnt = 0;
				//printf("%d ", cnt); cnt++; 
				// || N->l2pmapOvd.AA > 23000 || N->l2pmapOvd.Used > 184000
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0x3fffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

		////計算同SLB cyl上平均寫多少不同DB
		//N->b[SectorTableNo / NC->blockSizeSector]=1;
		//if (SLB->lastRWsection / (NC->CylinderSizeTrack*NC->trackSizeSection) != (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1) / (NC->CylinderSizeTrack*NC->trackSizeSection)){
		//	for (i = 0; i < NC->LsizeBlock; i++){ 
		//		if (N->b[i] == 1){ N->DBs++; }
		//		N->b[i] = 0;
		//	}
		//	N->write_cyls++; 
		//}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = getblock(simPtr, SectorTableNo);
		section_no = getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		if (block_no == NC->PsizeBlock - 1){
			assert(0 <= section_no && section_no <= SLB->Partial_Section);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
			N->blocks[block_no].sections[section_no].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection==1){ 
				SLOTTABLE_BAND_FirstSection = 0; 
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			//有不同的section寫入
			//N->blocks[block_no].sections[section_no].unique = 1;
			if (SLOTTABLE_BAND_FirstSection == 1){ 
				SLOTTABLE_BAND_FirstSection = 0; 
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
		}//in Data Band

		//N->section_cumu[SectorTableNo / NC->Section]++;
		//更新L2P mapping table
		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		//N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
/*sectors - group out*/
//void FASTwriteSectorSLBRAM2_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//sectors - group out
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
//	DWORD i = 0, j = 0;
//	//試驗
//	DWORD LRUmove = 0;
//#ifdef Time_Overhead
//	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//	N->MergeCondition = 0;
//#endif
//	DWORD start_sector = offsetSector, end_sector = offsetSector + lenSector - 1;
//
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	DWORD LRU_band = -1;
//	DWORD old_NSLB_RAM_request = 0;
//	/*if (free_len > N->NSLB_RAM_SIZE){
//		SLB_NoRAM(simPtr, offsetSector, lenSector);
//		return;
//	}*/
//
//	if (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
//		while (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){
//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
//			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
//			for (i = 0; i < N->NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//					N->NSLB_RAM[i] = -1;//原RAM失效
//				}
//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//
//			//用section mapping寫入NSLB //assert(tmp_NSLB_RAM[0] == N->NSLB_RAM[0]);
//			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//			//printf("ram write out: ");
//			for (i = 0; i < tmp_NSLB_RAM_request; i++){
//				assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//				assert(tmp_NSLB_RAM[i] != -1);
//#ifdef Time_Overhead
//				if (N->WriteBuff_Size == NC->trackSizePage / NC->SubTrack){
//					SLB2_TrackRMWtime(simPtr, NC->trackSizePage / NC->SubTrack); N->WriteBuff_Size = 0;
//					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
//				}
//				SectorTableNo = tmp_NSLB_RAM[i]; block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//				assert(0 <= N->WriteBuff_Size); assert(N->WriteBuff_Size < NC->trackSizePage / NC->SubTrack);
//				if (0 <= block_no && block_no < NC->LsizeBlock){ N->WriteBuff[N->WriteBuff_Size] = SectorTableNo; N->RDB++; }//在DB
//				else{ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + page_no; N->RMW++; }//在SLB
//				N->WriteBuff_Size++;
//#endif
//				FASTwriteSectorSLBpart2(simPtr, SectorTableNo, NC->pageSizeSector); //tmp_NSLB_RAM做寫入
//				N->RAM_WriteOut++;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;
//#ifdef Time_Overhead
//				if (N->MergeCondition > 0){
//					assert(N->MergeCondition == 1); N->MergeCondition = 0;
//					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
//				}
//#endif
//			}
//#ifdef Time_Overhead
//			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
//#endif
//			//更新NSLB_RAM_request資訊
//			old_NSLB_RAM_request = N->NSLB_RAM_request;
//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//			//整理NSLB_RAM，清掉-1的資料
//			j = 0;
//			for (i = 0; i < old_NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
//			}assert(j == N->NSLB_RAM_request);
//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
//			free(tmp_NSLB_RAM);
//		}assert(N->NSLB_RAM_request + lenSector <= N->NSLB_RAM_SIZE);
//	}
//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//
//	//request切成section送進RAM
//	while (start_sector <= end_sector) {
//		//fprintf(fPM_content, "%lu ", start_section);
//		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		if (N->RAM_Valid[start_sector] == 0){// skip == 0
//			N->RAM_Valid[start_sector] = 1;
//			N->NSLB_RAM[N->NSLB_RAM_request] = start_sector;
//			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			N->RAM_Miss++;
//		}else{ 
//			assert(N->RAM_Valid[start_sector] == 1);
//			N->RAM_Hit++; 
//		}
//		/*N->NSLB_RAM[N->NSLB_RAM_request] = start_section*N->SubTrack;
//		N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);*/
//		/*DWORD tmp_hit = 0;
//		for (i = 0; i < N->NSLB_RAM_request; i++){
//			if (N->NSLB_RAM[i] == start_section*N->SubTrack){ tmp_hit = 1; break; }
//		}
//		if (tmp_hit == 0){
//			N->NSLB_RAM[N->NSLB_RAM_request] = start_section*N->SubTrack;
//			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			N->RAM_Miss++;
//		}
//		else{ N->RAM_Hit++; }*/
//		start_sector++;
//		N->Host2RAM++;
//	}
//}
/*sector - single out*/
//void FASTwriteSectorSLBRAM2_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//sector - single out
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
//	DWORD i = 0, j = 0;
//	//試驗
//	DWORD LRUmove = 0;
//
//#ifdef Time_Overhead
//	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//	N->MergeCondition = 0;
//#endif
//	DWORD start_sector = offsetSector, end_sector = offsetSector + lenSector - 1;
//	DWORD tmp_len = 0;
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	DWORD start_section = -1;
//
//	if (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
//		tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//		for (i = 0; i < N->NSLB_RAM_request; i++){
//			assert(N->NSLB_RAM[i] != -1); tmp_NSLB_RAM[i] = N->NSLB_RAM[i]; N->NSLB_RAM[i] = -1;
//			if (N->NSLB_RAM_request + lenSector - (i + 1) <= N->NSLB_RAM_SIZE){
//				tmp_NSLB_RAM_request = i + 1; break;
//			}
//		}assert(N->NSLB_RAM_request + lenSector - tmp_NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		//用section mapping寫入NSLB //assert(tmp_NSLB_RAM[0] == N->NSLB_RAM[0]);
//		qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//		for (i = 0; i < tmp_NSLB_RAM_request; i++){
//			assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//			if (start_section == -1 || start_section < tmp_NSLB_RAM[i] / NC->SubTrack){
//				start_section = tmp_NSLB_RAM[i] / NC->SubTrack;
//#ifdef Time_Overhead
//				if (N->WriteBuff_Size == NC->trackSizePage / NC->SubTrack){
//					SLB2_TrackRMWtime(simPtr, NC->trackSizePage / NC->SubTrack); N->WriteBuff_Size = 0;
//					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
//				}
//				SectorTableNo = start_section*NC->SubTrack;// tmp_NSLB_RAM[i];
//				block_no = getblock(simPtr, SectorTableNo);
//				page_no = getpage(simPtr, SectorTableNo);
//				assert(0 <= N->WriteBuff_Size); assert(N->WriteBuff_Size < NC->trackSizePage / NC->SubTrack);
//				if (0 <= block_no && block_no < NC->LsizeBlock){ N->WriteBuff[N->WriteBuff_Size] = SectorTableNo; N->RDB++; }//在DB
//				else{ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + page_no; N->RMW++; }//在SLB
//				N->WriteBuff_Size++;
//#endif
//				FASTwriteSectorSLBpart2(simPtr, start_section*NC->SubTrack, NC->SubTrack); //tmp_NSLB_RAM做寫入
//				N->RAM_WriteOut += NC->SubTrack;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;
//#ifdef Time_Overhead
//				if (N->MergeCondition > 0){
//					assert(N->MergeCondition == 1); N->MergeCondition = 0;
//					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
//				}
//#endif	
//			}
//			N->NSLB_RAM[i] = -1;//原RAM失效
//		}assert(N->NSLB_RAM_request + lenSector - tmp_NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//#ifdef Time_Overhead
//		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
//#endif
//		//更新NSLB_RAM_request資訊
//		N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//		//整理NSLB_RAM，清掉-1的資料
//		for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = N->NSLB_RAM[i + tmp_NSLB_RAM_request]; }//把NSLB RAM撈出來的放回NSLB RAM
//		free(tmp_NSLB_RAM);
//	}
//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//
//	//request切成section送進RAM
//	while (start_sector <= end_sector){
//		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		if (N->RAM_Valid[start_sector] == 0){// skip == 0
//			N->RAM_Valid[start_sector] = 1;
//			N->NSLB_RAM[N->NSLB_RAM_request] = start_sector;
//			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			N->RAM_Miss++;
//		}
//		else{
//			assert(N->RAM_Valid[start_sector] == 1);
//			N->RAM_Hit++;
//		}
//		start_sector++;
//		N->Host2RAM++;
//	}
//}
/*section - group out*/ //SLB bit map
//void FASTwriteSectorSLBRAM2_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//section - group out
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
//	DWORD i = 0, j = 0;
//	//試驗
//	DWORD LRUmove = 0;
//#ifdef Time_Overhead
//	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//#endif
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	const DWORD start_section = offsetSector / NC->Section;
//	const DWORD end_section = (offsetSector + lenSector - 1) / NC->Section;
//	const DWORD section_len = end_section - start_section + 1;
//	DWORD tmp_start = offsetSector / NC->Section;
//
//	DWORD LRU_band = 0, old_NSLB_RAM_request = 0;
//	tmp_start = start_section;
//	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
//	assert(tmp_start == end_section);
//	assert(section_len >= 1);
//
//	if (N->NSLB_RAM_request + section_len > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
//		while (N->NSLB_RAM_request + section_len > N->NSLB_RAM_SIZE){
//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
//			LRU_band = N->NSLB_RAM[0] * NC->Section / NC->blockSizeSector;//有改，補上 * NC->Section
//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
//			for (i = 0; i < N->NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i]*NC->Section / NC->blockSizeSector == LRU_band){
//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//					N->NSLB_RAM[i] = -1;//原RAM失效
//				}
//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//
//			//用section mapping寫入NSLB //assert(tmp_NSLB_RAM[0] == N->NSLB_RAM[0]);
//			//qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//			//if (tmp_NSLB_RAM_request > 1){ for (i = 1; i < tmp_NSLB_RAM_request; i++){ assert(tmp_NSLB_RAM[i - 1] <= tmp_NSLB_RAM[i]); } }
//			for (i = 0; i < tmp_NSLB_RAM_request; i++){
//				assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//				assert(tmp_NSLB_RAM[i] != -1);
//				SectorTableNo = tmp_NSLB_RAM[i] * NC->Section;
//#ifdef Time_Overhead
//				assert(0 <= N->WriteBuff_section);
//				block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//				assert(N->blocks[block_no].pages[page_no].valid == 1);
//				if (0 <= block_no && block_no < NC->LsizeBlock){ N->WriteBuff[N->WriteBuff_section] = SectorTableNo; N->RDB++; }//在DB
//				else{ N->WriteBuff[N->WriteBuff_section] = NC->LsizeSector + page_no; N->RMW++; }//在NSLB
//				N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
//				if (N->WriteBuff_section == N->WriteBuff_SIZE){
//					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3//N->WriteBuff_WriteOut += N->WriteBuff_section;
//					SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1); N->WriteBuff_section = 0;
//					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage), 0);//    / NC->CylinderSizeTrack
//					Calc_RotateOverhead2(simPtr, 0, 0);
//					N->WriteBuff_writecount++;//
//				}
//#endif
//				FASTwriteSectorSLBpart2(simPtr, SectorTableNo, NC->Section); //tmp_NSLB_RAM做寫入
//				N->RAM_WriteOut += NC->Section;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;
//			}
//			//更新NSLB_RAM_request資訊
//			old_NSLB_RAM_request = N->NSLB_RAM_request;
//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//			//整理NSLB_RAM，清掉-1的資料
//			j = 0;
//			for (i = 0; i < old_NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
//			}assert(j == N->NSLB_RAM_request);
//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
//			free(tmp_NSLB_RAM);
//		}assert(N->NSLB_RAM_request + section_len <= N->NSLB_RAM_SIZE);
//	}
//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//
//	//request切成section送進RAM
//	tmp_start = start_section;
//	while (tmp_start <= end_section){
//		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		if (N->RAM_Valid[tmp_start] == 0){// skip == 0
//			N->RAM_Valid[tmp_start] = 1;
//			N->NSLB_RAM[N->NSLB_RAM_request] = tmp_start;
//			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			N->RAM_Miss++;
//		}
//		else{
//			assert(N->RAM_Valid[tmp_start] == 1);
//			N->RAM_Hit++;
//		}
//		tmp_start++;
//		N->Host2RAM++;
//	}
//}
/*section - group out*/ //RMW
void FASTwriteSectorSLBRAM2_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//section - group out
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	//試驗
	DWORD LRUmove = 0;
#ifdef Time_Overhead
	DWORD SectorTableNo = 0, block_no = 0, section_no = 0;
#endif
	DWORD *tmp_NSLB_RAM = NULL;
	DWORD tmp_NSLB_RAM_request = 0;
	const DWORD start_section = offsetSector / NC->Section;
	const DWORD end_section = (offsetSector + lenSector - 1) / NC->Section;
	const DWORD section_len = end_section - start_section + 1;
	DWORD tmp_start = offsetSector / NC->Section;

	DWORD LRU_band = 0, old_NSLB_RAM_request = 0;
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section);
	assert(section_len >= 1);

	if (N->NSLB_RAM_request + section_len > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
		while (N->NSLB_RAM_request + section_len > N->NSLB_RAM_SIZE){
			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
			LRU_band = N->NSLB_RAM[0] * NC->Section / NC->blockSizeSector;//有改，補上 * NC->Section
			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
			for (i = 0; i < N->NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] * NC->Section / NC->blockSizeSector == LRU_band){
					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
					N->NSLB_RAM[i] = -1;//原RAM失效
				}
			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);

			//用section mapping寫入NSLB //assert(tmp_NSLB_RAM[0] == N->NSLB_RAM[0]);
			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
			if (tmp_NSLB_RAM_request > 1){ for (i = 1; i < tmp_NSLB_RAM_request; i++){ assert(tmp_NSLB_RAM[i - 1] <= tmp_NSLB_RAM[i]); } }
			for (i = 0; i < tmp_NSLB_RAM_request; i++){
				assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
				assert(tmp_NSLB_RAM[i] != -1);
				SectorTableNo = tmp_NSLB_RAM[i] * NC->Section;
#ifdef Time_Overhead
				//assert(0 <= N->WriteBuff_section);
				//block_no = getblock(simPtr, SectorTableNo); section_no = getsection(simPtr, SectorTableNo);
				//assert(N->blocks[block_no].sections[section_no].valid == 1);
				//if (0 <= block_no && block_no < NC->LsizeBlock){ N->WriteBuff[N->WriteBuff_section] = SectorTableNo; N->RDB++; }//在DB
				//else{ N->WriteBuff[N->WriteBuff_section] = NC->LsizeSector + section_no*NC->Section; N->RMW++; }//在NSLB
				//N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
				if (N->WriteBuff_section == N->WriteBuff_SIZE){// || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection + 1) / NC->trackSizeSection
					////Write_W:0, Write_R:1, Merge_W:2, Merge_R:3//N->WriteBuff_WriteOut += N->WriteBuff_section;
					//SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1); N->WriteBuff_section = 0;
					//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);//    / NC->CylinderSizeTrack
					//Calc_RotateOverhead2(simPtr, 0, 0);//SLB的firstRWtrack做寫入
					N->WriteBuff_writecount++;//
					N->content_rotate += (N->WriteBuff_section / NC->trackSizeSection);//write
				}
#endif
				//FASTwriteSectorSLBpart2(simPtr, SectorTableNo, NC->Section); //tmp_NSLB_RAM做寫入
				FASTwriteSectorSLBpart2_BitMap(simPtr, SectorTableNo, NC->Section);
				N->RAM_WriteOut += NC->Section;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;
			}
			//更新NSLB_RAM_request資訊
			old_NSLB_RAM_request = N->NSLB_RAM_request;
			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
			//整理NSLB_RAM，清掉-1的資料
			j = 0;
			for (i = 0; i < old_NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
			}assert(j == N->NSLB_RAM_request);
			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
			free(tmp_NSLB_RAM);
		}assert(N->NSLB_RAM_request + section_len <= N->NSLB_RAM_SIZE);
	}
	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);

	//request切成section送進RAM
	DWORD cur_track = 0;
	tmp_start = start_section;
	while (tmp_start <= end_section){
		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
		if (N->RAM_Valid[tmp_start] == 0){// skip == 0
			N->RAM_Valid[tmp_start] = 1;
			N->NSLB_RAM[N->NSLB_RAM_request] = tmp_start;
			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			N->RAM_Miss++;
			////遇到section miss去DB做RMW
			//if (cur_track == 0 || cur_track != tmp_start*NC->Section / NC->trackSizePage){
			//	cur_track = tmp_start*NC->Section / NC->trackSizePage;
			//	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
			//	Calc_TimeOverhead2(simPtr, tmp_start*NC->Section, 1);
			//	Calc_RotateOverhead2(simPtr, 0, 1);//read
			//}
		}
		else{
			assert(N->RAM_Valid[tmp_start] == 1);
			N->RAM_Hit++;
		}
		tmp_start++;
		N->Host2RAM++;
	}
}
/*section - single out*/
//void FASTwriteSectorSLBRAM2_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//section - single out
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
//	DWORD i = 0, j = 0;
//	//試驗
//	DWORD LRUmove = 0;
//
//#ifdef Time_Overhead
//	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//	N->MergeCondition = 0;
//#endif
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	const DWORD start_section = offsetSector / NC->SubTrack;
//	const DWORD end_section = (offsetSector + lenSector - 1) / NC->SubTrack;
//	const DWORD section_len = end_section - start_section + 1;
//	DWORD tmp_start = offsetSector / NC->SubTrack;
//	DWORD section2SLB = -1;
//	tmp_start = start_section;
//	while (tmp_start*NC->SubTrack + NC->SubTrack - 1 < offsetSector + lenSector - 1){ tmp_start++; }
//	assert(tmp_start == end_section);
//	assert(section_len >= 1);
//
//	if (N->NSLB_RAM_request + section_len > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
//		tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//		for (i = 0; i < N->NSLB_RAM_request; i++){
//			assert(N->NSLB_RAM[i] != -1); tmp_NSLB_RAM[i] = N->NSLB_RAM[i]; N->NSLB_RAM[i] = -1;
//			if (N->NSLB_RAM_request + section_len - (i + 1) <= N->NSLB_RAM_SIZE){
//				tmp_NSLB_RAM_request = i + 1; break;
//			}
//		}assert(N->NSLB_RAM_request + section_len - tmp_NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		//用section mapping寫入NSLB //assert(tmp_NSLB_RAM[0] == N->NSLB_RAM[0]);
//		qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//		section2SLB = -1;
//		for (i = 0; i < tmp_NSLB_RAM_request; i++){
//			assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//			if (section2SLB == -1 || section2SLB < tmp_NSLB_RAM[i]){
//				section2SLB = tmp_NSLB_RAM[i];
//#ifdef Time_Overhead
//				if (N->WriteBuff_Size == NC->trackSizePage / NC->SubTrack){
//					SLB2_TrackRMWtime(simPtr, NC->trackSizePage / NC->SubTrack); N->WriteBuff_Size = 0;
//					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
//				}
//				SectorTableNo = section2SLB * NC->SubTrack;
//				block_no = getblock(simPtr, SectorTableNo);
//				page_no = getpage(simPtr, SectorTableNo);
//				assert(0 <= N->WriteBuff_Size); assert(N->WriteBuff_Size < NC->trackSizePage / NC->SubTrack);
//				if (0 <= block_no && block_no < NC->LsizeBlock){ N->WriteBuff[N->WriteBuff_Size] = SectorTableNo; N->RDB++; }//在DB
//				else{ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + page_no; N->RMW++; }//在SLB
//				N->WriteBuff_Size++;
//#endif
//				FASTwriteSectorSLBpart2(simPtr, section2SLB*NC->SubTrack, NC->SubTrack); //tmp_NSLB_RAM做寫入
//				N->RAM_WriteOut += NC->SubTrack;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;
//#ifdef Time_Overhead
//				if (N->MergeCondition > 0){
//					assert(N->MergeCondition == 1); N->MergeCondition = 0;
//					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
//				}
//#endif	
//			}
//		}assert(N->NSLB_RAM_request + section_len - tmp_NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//#ifdef Time_Overhead
//		//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWpage) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
//#endif
//		//更新NSLB_RAM_request資訊
//		N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//		//整理NSLB_RAM，清掉-1的資料
//		for (i = 0; i < tmp_NSLB_RAM_request; i++){ assert(N->NSLB_RAM[i] == -1); }
//		for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = N->NSLB_RAM[i + tmp_NSLB_RAM_request]; }//把NSLB RAM撈出來的放回NSLB RAM
//		free(tmp_NSLB_RAM);
//	}
//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//
//	//request切成section送進RAM
//	tmp_start = start_section;
//	while (tmp_start <= end_section){
//		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		if (N->RAM_Valid[tmp_start] == 0){// skip == 0
//			N->RAM_Valid[tmp_start] = 1;
//			N->NSLB_RAM[N->NSLB_RAM_request] = tmp_start;
//			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			N->RAM_Miss++;
//		}
//		else{
//			assert(N->RAM_Valid[tmp_start] == 1);
//			N->RAM_Hit++;
//		}
//		tmp_start++;
//		N->Host2RAM++;
//	}
//}

void FASTwriteSectorSLBRAM2_WriteAllOut(sim *simPtr)//沒修改成新版
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	DWORD i = 0, j = 0;
	DWORD start_section = 0, Section_Update = 0;
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);

	if (N->NSLB_RAM_request > 0){
#ifdef Time_Overhead
		DWORD SectorTableNo = 0, block_no = 0, section_no = 0;
#endif
		assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE); //qsort(N->NSLB_RAM, N->NSLB_RAM_request, sizeof(DWORD), compare);
		for (i = 0; i < N->NSLB_RAM_request; i++){
			assert(N->NSLB_RAM[i] != -1); start_section = N->NSLB_RAM[i];
			SectorTableNo = start_section*NC->Section; block_no = getblock(simPtr, SectorTableNo);
			if (block_no == NC->LsizeBlock){//在SLB裡
				section_no = getsection(simPtr, SectorTableNo);
#ifdef Time_Overhead
				//assert(0 <= N->WriteBuff_section); 
				//N->WriteBuff[N->WriteBuff_section] = NC->LsizeSector + section_no*NC->Section; N->RMW++;//在SLB
				//N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
				if (N->WriteBuff_section == N->WriteBuff_SIZE){// || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection + 1) / NC->trackSizeSection
					////Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
					//SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1); N->WriteBuff_section = 0;
					//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);//    / NC->CylinderSizeTrack  / NC->trackSizePage
					//Calc_RotateOverhead2(simPtr, 0, 0);//write
					N->WriteBuff_writecount++;//
					N->content_rotate += (N->WriteBuff_section / NC->trackSizeSection);//write
				}
#endif
				//FASTwriteSectorSLBpart2(simPtr, start_section*NC->Section, NC->Section);
				FASTwriteSectorSLBpart2_BitMap(simPtr, start_section*NC->Section, NC->Section);
				N->RAM_WriteOut += NC->Section;
				N->NSLB_RAM[i] = -1;
			}
		}
		for (i = 0; i < N->NSLB_RAM_request; i++){
			if (N->NSLB_RAM[i] != -1){
				start_section = N->NSLB_RAM[i] / NC->Section;
#ifdef Time_Overhead
				//SectorTableNo = start_section*NC->Section; block_no = getblock(simPtr, SectorTableNo); section_no = getsection(simPtr, SectorTableNo);
				//assert(0 <= N->WriteBuff_section); 
				//N->WriteBuff[N->WriteBuff_section] = SectorTableNo; N->RDB++; //在DB
				//N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
				if (N->WriteBuff_section == N->WriteBuff_SIZE){// || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection + 1) / NC->trackSizeSection
					////Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
					//SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1); N->WriteBuff_section = 0;
					//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);//    / NC->CylinderSizeTrack  / NC->trackSizePage
					//Calc_RotateOverhead2(simPtr, 0, 0);
					N->WriteBuff_writecount++;//
					N->content_rotate += (N->WriteBuff_section / NC->trackSizeSection);//write
				}
#endif
				//FASTwriteSectorSLBpart2(simPtr, start_section*NC->Section, NC->Section);
				FASTwriteSectorSLBpart2_BitMap(simPtr, start_section*NC->Section, NC->Section);
				N->RAM_WriteOut += NC->Section;
			}
			N->NSLB_RAM[i] = -1;
		}
		N->NSLB_RAM_request = 0;
#ifdef Time_Overhead
		if (N->WriteBuff_section > 0){
			////Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
			//SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1); N->WriteBuff_section = 0;
			//Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);//    / NC->CylinderSizeTrack
			//Calc_RotateOverhead2(simPtr, 0, 0);
			N->WriteBuff_writecount++;//
			N->content_rotate += (N->WriteBuff_section / NC->trackSizeSection);//write
		}
#endif
	}
	assert(N->NSLB_RAM_request == 0);
}

void SLB_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//write buff有問題
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	DWORD SectorTableNo = 0, block_no = 0, section_no = 0;
	//轉成section len的request
	const DWORD start_section = offsetSector / NC->Section;
	const DWORD end_section = (offsetSector + lenSector - 1) / NC->Section;
	const DWORD section_len = end_section - start_section + 1;
	DWORD tmp_start = offsetSector / NC->Section; assert(tmp_start >= 0);
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section);
	assert(section_len >= 1);
	tmp_start = start_section;
	assert(tmp_start*NC->Section <= offsetSector + lenSector - 1);

	//RMW寫入
	//FASTwriteSectorSLBpart2(simPtr, tmp_start*NC->Section, section_len*NC->Section);
	//Bit-Map寫入
	//FASTwriteSectorSLBpart2_BitMap(simPtr, tmp_start*NC->Section, section_len*NC->Section);
	//sector SLB
	if (NC->Section == 1){
		assert(tmp_start == offsetSector); assert(section_len == lenSector);
	}
	//FASTwriteSectorSLBpart2_Sr(simPtr, tmp_start*NC->Section, section_len*NC->Section);
	simStat			*SS = &(simPtr->simStatObj);
#ifdef Time_Overhead
	if (SS->writeReq % 240 == 0){
		test(simPtr, (I64)NC->LsizeSector/2, 0);
	}
#endif
	DM_SMRwriteSector(simPtr, tmp_start*NC->Section, section_len*NC->Section);
	/*DWORD LBA, LBA_end;
	DWORD subtrack = 32;
	LBA = offsetSector / subtrack;
	LBA_end = (offsetSector + lenSector - 1) / subtrack;
	FASTwriteSectorSLBpart2_Sr(simPtr, LBA*subtrack, (LBA_end - LBA + 1)*subtrack);*/

	//	while (tmp_start <= end_section){
	//		N->Host2RAM++;
	//		SectorTableNo = tmp_start*NC->Section;
	//#ifdef Time_Overhead
	//		assert(0 <= N->WriteBuff_section);
	//		block_no = getblock(simPtr, SectorTableNo); section_no = getsection(simPtr, SectorTableNo);
	//		assert(N->blocks[block_no].sections[section_no].valid == 1);
	//		if (0 <= block_no && block_no < NC->LsizeBlock){ N->WriteBuff[N->WriteBuff_section] = SectorTableNo; N->RDB++; }//在DB
	//		else{ N->WriteBuff[N->WriteBuff_section] = NC->LsizeSector + section_no*NC->Section; N->RMW++; }//在NSLB
	//		N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
	//		if (N->WriteBuff_section == N->WriteBuff_SIZE){//  || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection + NC->Section) / NC->trackSizeSection
	//			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
	//			SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1); N->WriteBuff_section = 0;
	//			Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);//    / NC->CylinderSizeTrack
	//			Calc_RotateOverhead2(simPtr, 0, 0);//write
	//			N->WriteBuff_writecount++;//
	//			N->content_rotate += (N->WriteBuff_section*NC->Section);
	//		}
	//#endif
	//		//FASTwriteSectorSLBpart2(simPtr, tmp_start*NC->Section, NC->Section);
	//		FASTwriteSectorSLBpart2_BitMap(simPtr, tmp_start*NC->Section, NC->Section);
	//		tmp_start++;
	//		N->RAM_WriteOut += NC->Section;
	//	}
	//#ifdef Time_Overhead
	//	if (N->WriteBuff_section > 0){
	//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
	//		SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 1); N->WriteBuff_section = 0;
	//		Calc_TimeOverhead2(simPtr, (NC->LsizeSector + SLB->lastRWsection*NC->Section), 0);//    / NC->CylinderSizeTrack
	//		Calc_RotateOverhead2(simPtr, 0, 0);//write
	//		N->WriteBuff_writecount++;//
	//		N->content_rotate += (N->WriteBuff_section*NC->Section);
	//	}
	//#endif
	//	assert(tmp_start == end_section + 1);
}

//------
void DM_SMRwriteSector_NoSt(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
	N->content_rotate += ((double)lenSector / NC->trackSizeSector);//寫多少sector轉多少sector
	N->WW_content_rotate += ((double)lenSector / NC->trackSizeSector);///
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += ((double)lenSector / NC->trackSizeSector * 10);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//DWORD n1 = 23000, n2 = 34500; /**(I64)SLB->SLBsection_num / 50331648*/ /** (I64)SLB->SLBsection_num / 50331648;*/
			//|| N->SlotTable_Act > n1 || (N->SlotTable_Act + N->SlotTable_InAct > n2)
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge

				FASTmergeRWSLBpart2_Sr_NoSt(simPtr);
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0x3fffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo);
		section_no = SectorTableNo % NC->blockSizeSector;// getsection(simPtr, SectorTableNo);
		//assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		assert(0 <= block_no && block_no < NC->LsizeBlock);

		//if (block_no == NC->PsizeBlock - 1){
		//	assert(0 <= section_no && section_no <= SLB->Partial_Section);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
		//	N->blocks[block_no].sections[section_no].valid = 0;
		//	N->SLB_Hit++;
		//	N->cycleSLB_Hit++;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].InAct++;
		//		N->SlotTable_InAct++;
		//	}
		//}
		//else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
		//	assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
		//	N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
		//	N->SLB_Miss++;
		//	N->cycleSLB_Miss++;
		//	//增加存活sector
		//	N->l2pmapOvd.AA++;
		//	//有不同的section寫入
		//	//N->blocks[block_no].sections[section_no].unique = 1;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].Act++;
		//		N->SlotTable_Act++;
		//	}
		//}//in Data Band

		assert(N->blocks[block_no].sections[section_no].valid == 0 || N->blocks[block_no].sections[section_no].valid == 1);
		if (N->blocks[block_no].sections[section_no].valid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
		}
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;

		//N->section_cumu[SectorTableNo / NC->Section]++;
		////更新L2P mapping table
		//setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		//setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		//N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
void SLB_NoRAM_NoSt(sim *simPtr, DWORD offsetSector, DWORD lenSector)//write buff有問題
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	DWORD SectorTableNo = 0, block_no = 0, section_no = 0;
	//轉成section len的request
	const DWORD start_section = offsetSector / NC->Section;
	const DWORD end_section = (offsetSector + lenSector - 1) / NC->Section;
	const DWORD section_len = end_section - start_section + 1;
	DWORD tmp_start = offsetSector / NC->Section; assert(tmp_start >= 0);
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section);
	assert(section_len >= 1);
	tmp_start = start_section;
	assert(tmp_start*NC->Section <= offsetSector + lenSector - 1);

	if (NC->Section == 1){
		assert(tmp_start == offsetSector); assert(section_len == lenSector);
	}
	simStat			*SS = &(simPtr->simStatObj);
#ifdef Time_Overhead
	if (SS->writeReq % 240 == 0){
		test(simPtr, (I64)NC->LsizeSector/2, 0);
	}
#endif
	DM_SMRwriteSector_NoSt(simPtr, tmp_start*NC->Section, section_len*NC->Section);
	
}

void DM_SMRwriteSector_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD SLOTTABLE_BAND_FirstSection = 1;
	N->FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	//N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	//N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//DWORD n1 = 23000, n2 = 34500; /**(I64)SLB->SLBsection_num / 50331648*/ /** (I64)SLB->SLBsection_num / 50331648;*/
			//|| N->SlotTable_Act > n1 || (N->SlotTable_Act + N->SlotTable_InAct > n2)
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
				FASTmergeRWSLBpart2_Sr_NoStNoDB(simPtr);
				//FASTmergeRWSLBpart2_Sr_NoStNoDB_PB(simPtr);

				//Merge後如果又再寫應該視為初寫
				N->FirstSection = 1;
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (N->FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			//Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);//第一次寫or換軌都要rotation
			N->FirstSection = 0;//SLB Write的Rotation可能是第一次寫, 或換track做rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo);
		section_no = SectorTableNo % NC->blockSizeSector;// getsection(simPtr, SectorTableNo);
		//assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		assert(0 <= block_no && block_no < NC->LsizeBlock);

		//if (block_no == NC->PsizeBlock - 1){
		//	assert(0 <= section_no && section_no <= SLB->Partial_Section);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
		//	N->blocks[block_no].sections[section_no].valid = 0;
		//	N->SLB_Hit++;
		//	N->cycleSLB_Hit++;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].InAct++;
		//		N->SlotTable_InAct++;
		//	}
		//}
		//else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
		//	assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
		//	N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
		//	N->SLB_Miss++;
		//	N->cycleSLB_Miss++;
		//	//增加存活sector
		//	N->l2pmapOvd.AA++;
		//	//有不同的section寫入
		//	//N->blocks[block_no].sections[section_no].unique = 1;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].Act++;
		//		N->SlotTable_Act++;
		//	}
		//}//in Data Band

		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
			}
			N->DBalloc[block_no] = 1;
		}

		assert(N->blocks[block_no].sections[section_no].valid == 0 || N->blocks[block_no].sections[section_no].valid == 1);
		if (N->blocks[block_no].sections[section_no].valid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
		}
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;

		//N->section_cumu[SectorTableNo / NC->Section]++;
		////更新L2P mapping table
		//setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		//setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		//N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(N->FirstSection == 0); assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime; 
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#ifdef READ_REQUEST
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	N->W_RunTime += N->Req_RunTime;
	N->W_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->W_WaitingTime += (N->Finish_Time- N->Arr_Time - N->Req_RunTime);
#endif
}
void SLB_NoRAM_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector)//write buff有問題
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	//轉成section len的request
	const I64 start_section = offsetSector / NC->Section;
	const I64 end_section = (offsetSector + lenSector - 1) / NC->Section;
	const I64 section_len = end_section - start_section + 1;
	I64 tmp_start = offsetSector / NC->Section; assert(tmp_start >= 0);
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section); assert(section_len >= 1);
	tmp_start = start_section;
	assert(tmp_start*NC->Section <= offsetSector + lenSector - 1);

	if (NC->Section == 1){
		assert(tmp_start == offsetSector); assert(section_len == lenSector);
	}
	simStat *SS = &(simPtr->simStatObj);
//#ifdef Time_Overhead
//	if (SS->writeReq % 240 == 0){
//		test(simPtr, NC->LsizeSector/2, 0);
//	}
//#endif
	DM_SMRwriteSector_NoStNoDB(simPtr, tmp_start*NC->Section, section_len*NC->Section);
}
#endif

#ifdef Simulation_SLB_SHUFFLE
//Cylinder_Shuffle
void DM_SMR_SHUFFLEwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD SLOTTABLE_BAND_FirstSection = 1;
	N->FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
#ifdef Simulation_SLB_SHUFFLE
	DWORD P_Index;
	BYTE *WriteFreq = (BYTE*)calloc(NC->blockSizeCylinder, sizeof(BYTE));//保證新寫入或Merge回來後, Band各Cylinder最多增加一次WriteFreq
	for (i = 0; i < NC->blockSizeCylinder; i++){ assert(WriteFreq[i] == 0); }
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
#ifdef Simulation_SLB_SHUFFLE
				for (i = 0; i < NC->blockSizeCylinder; i++){ WriteFreq[i] = 0; }
#endif
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
				//FASTmergeRWSLB_Observe(simPtr);
				//FASTmergeRWSLB_Observe2(simPtr);//Shuffle用優先權+LRU
#endif
				//FASTmergeRWSLB_SHUFFLE(simPtr);//All Shuffle
				//FASTmergeRWSLB_SHUFFLE(simPtr);//預測多merge一次清空間放delay

				FASTmergeRWSLB_SHUFFLE2(simPtr);//每次都Shuffle, 有LRU左退
				//Merge後如果又再寫應該視為初寫
				N->FirstSection = 1;
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (N->FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			//Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			N->FirstSection = 0;//等roation結束才標示這筆Write Request第一次position 5ms結束
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo);
		section_no = SectorTableNo % NC->blockSizeSector;// getsection(simPtr, SectorTableNo);
		//assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		assert(0 <= block_no && block_no < NC->LsizeBlock);

		//if (block_no == NC->PsizeBlock - 1){
		//	assert(0 <= section_no && section_no <= SLB->Partial_Section);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
		//	N->blocks[block_no].sections[section_no].valid = 0;
		//	N->SLB_Hit++;
		//	N->cycleSLB_Hit++;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].InAct++;
		//		N->SlotTable_InAct++;
		//	}
		//}
		//else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
		//	assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
		//	N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
		//	N->SLB_Miss++;
		//	N->cycleSLB_Miss++;
		//	//增加存活sector
		//	N->l2pmapOvd.AA++;
		//	//有不同的section寫入
		//	//N->blocks[block_no].sections[section_no].unique = 1;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].Act++;
		//		N->SlotTable_Act++;
		//	}
		//}//in Data Band

		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
			}
			N->DBalloc[block_no] = 1;
		}
#ifdef Simulation_SLB_SHUFFLE //給從未寫過DB配置TRANSFOR_LAYER記憶體
		if (N->SHUFFLE_TL_Alloc[block_no] == 0){
			N->SHUFFLE_TL[block_no].L2P_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].P2L_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
			N->SHUFFLE_TL[block_no].DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
			for (i = 0; i < NC->blockSizeCylinder; i++){
				N->SHUFFLE_TL[block_no].L2P_Index[i] = i;
				N->SHUFFLE_TL[block_no].P2L_Index[i] = i;
				N->SHUFFLE_TL[block_no].WriteFreq[i] = 0;
				N->SHUFFLE_TL[block_no].DirtyUnit[i] = 0;
			}
			N->SHUFFLE_TL_Alloc[block_no] = 1;
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE//配置MFMBO記憶體
			//MFMBO_INFO紀錄的是Logical Cylinder有沒有Merge的時候被寫
			N->MFMBO_INFO[block_no].MergeFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
			N->MFMBO_INFO[block_no].MergeDS = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
			for (i = 0; i < NC->blockSizeCylinder; i++){
				N->MFMBO_INFO[block_no].MergeFreq[i] = 0;
				N->MFMBO_INFO[block_no].MergeDS[i] = 0;
			}
#endif
		}assert(N->SHUFFLE_TL_Alloc[block_no] == 1);
		//檢查當前L address正確性
		assert(section_no / NC->CylinderSizeSector < NC->blockSizeCylinder);
		//L address經由TL層的L2P得到P_Index
		P_Index = N->SHUFFLE_TL[block_no].L2P_Index[section_no / NC->CylinderSizeSector];
		assert(0 <= P_Index); assert(P_Index < NC->blockSizeCylinder);//P_Index在正常範圍
		//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
		assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == section_no / NC->CylinderSizeSector);
		//統計WriteFreq
		//N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++;
		if (WriteFreq[P_Index] == 0){ N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++; WriteFreq[P_Index] = 1; }
#endif
		assert(N->blocks[block_no].sections[section_no].valid == 0 || N->blocks[block_no].sections[section_no].valid == 1);
		if (N->blocks[block_no].sections[section_no].valid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
#ifdef Simulation_SLB_SHUFFLE
			//統計DirtyUnit
			N->SHUFFLE_TL[block_no].DirtyUnit[P_Index]++;
#endif
		}
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;

		//N->section_cumu[SectorTableNo / NC->Section]++;
		////更新L2P mapping table
		//setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		//setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		//N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(N->FirstSection == 0); assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#ifdef READ_REQUEST
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	N->W_RunTime += N->Req_RunTime;
	N->W_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->W_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#endif
#ifdef Simulation_SLB_SHUFFLE
	free(WriteFreq);
#endif
}
void SLB_NoRAM_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector)//write buff有問題
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	//轉成section len的request
	const I64 start_section = offsetSector / NC->Section;
	const I64 end_section = (offsetSector + lenSector - 1) / NC->Section;
	const I64 section_len = end_section - start_section + 1;
	I64 tmp_start = offsetSector / NC->Section; assert(tmp_start >= 0);
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section); assert(section_len >= 1);
	tmp_start = start_section;
	assert(tmp_start*NC->Section <= offsetSector + lenSector - 1);

	if (NC->Section == 1){
		assert(tmp_start == offsetSector); assert(section_len == lenSector);
	}
	DM_SMR_SHUFFLEwriteSector(simPtr, tmp_start*NC->Section, section_len*NC->Section);
}
//Track_Shuffle
void DM_SMR_SHUFFLE_TRACKwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, newBlock, dataBlock;
	//DWORD cur_8subband = -1, prev_8subband = -1;
	//simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); 
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD SLOTTABLE_BAND_FirstSection = 1;
	N->FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
#ifdef Simulation_SLB_SHUFFLE
	DWORD P_Index;
	BYTE *WriteFreq = (BYTE*)calloc(NC->blockSizeTrack, sizeof(BYTE));//保證新寫入或Merge回來後, Band各Cylinder最多增加一次WriteFreq
	for (i = 0; i < NC->blockSizeTrack; i++){ assert(WriteFreq[i] == 0); }
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
#ifdef Simulation_SLB_SHUFFLE
				for (i = 0; i < NC->blockSizeTrack; i++){ WriteFreq[i] = 0; }
#endif
				//FASTmergeRWSLB_SHUFFLE2(simPtr);//每次都Shuffle, 有LRU左退
				FASTmergeRWSLB_SHUFFLE_TRACK(simPtr);//來自FASTmergeRWSLB_SHUFFLE2//每次都Shuffle, 有LRU左退
				//Merge後如果又再寫應該視為初寫
				N->FirstSection = 1;
			}
			else{//guard track右邊不是fRW，右移gt一格
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (N->FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			//Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			N->FirstSection = 0;//等roation結束才標示這筆Write Request第一次position 5ms結束
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo);
		section_no = SectorTableNo % NC->blockSizeSector;// getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no < NC->LsizeBlock);

		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
			}
			N->DBalloc[block_no] = 1;
		}
#ifdef Simulation_SLB_SHUFFLE //給從未寫過DB配置TRANSFOR_LAYER記憶體
		if (N->SHUFFLE_TL_Alloc[block_no] == 0){
			N->SHUFFLE_TL[block_no].L2P_Index = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].P2L_Index = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].WriteFreq = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));//由P維護
			N->SHUFFLE_TL[block_no].DirtyUnit = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));//由P維護
			for (i = 0; i < NC->blockSizeTrack; i++){
				N->SHUFFLE_TL[block_no].L2P_Index[i] = i;
				N->SHUFFLE_TL[block_no].P2L_Index[i] = i;
				N->SHUFFLE_TL[block_no].WriteFreq[i] = 0;
				N->SHUFFLE_TL[block_no].DirtyUnit[i] = 0;
			}
			N->SHUFFLE_TL_Alloc[block_no] = 1;
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE_TK//配置MFMBO記憶體
			//MFMBO_INFO紀錄的是Logical Cylinder有沒有Merge的時候被寫
			N->MFMBO_INFO[block_no].MergeFreq = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			N->MFMBO_INFO[block_no].MergeDS = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			for (i = 0; i < NC->blockSizeTrack; i++){
				N->MFMBO_INFO[block_no].MergeFreq[i] = 0;
				N->MFMBO_INFO[block_no].MergeDS[i] = 0;
			}
#endif
		}assert(N->SHUFFLE_TL_Alloc[block_no] == 1);
		//檢查當前L address正確性: 找Physical Track Address
		assert(section_no / NC->trackSizeSector < NC->blockSizeTrack);
		//L address經由TL層的L2P得到P_Index
		P_Index = N->SHUFFLE_TL[block_no].L2P_Index[section_no / NC->trackSizeSector];
		assert(0 <= P_Index); assert(P_Index < NC->blockSizeTrack);//P_Index在正常範圍
		//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
		assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == section_no / NC->trackSizeSector);
		//統計WriteFreq
		//N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++;
		if (WriteFreq[P_Index] == 0){ N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++; WriteFreq[P_Index] = 1; }
#endif
		assert(N->blocks[block_no].sections[section_no].valid == 0 || N->blocks[block_no].sections[section_no].valid == 1);
		if (N->blocks[block_no].sections[section_no].valid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
#ifdef Simulation_SLB_SHUFFLE
			//統計DirtyUnit
			N->SHUFFLE_TL[block_no].DirtyUnit[P_Index]++;
#endif
		}
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;

		//N->section_cumu[SectorTableNo / NC->Section]++;
		//不更新L2P mapping table: setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1); setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(N->FirstSection == 0); assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#ifdef READ_REQUEST
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	N->W_RunTime += N->Req_RunTime;
	N->W_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->W_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#endif
#ifdef Simulation_SLB_SHUFFLE
	free(WriteFreq);
#endif
}
//ST: Shuffle Track,  VTLMA: Victim Track + Limited Merge Associativity
void DM_SMR_ST_VTLMAwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, newBlock, dataBlock;
	//DWORD cur_8subband = -1, prev_8subband = -1;
	//simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); 
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD SLOTTABLE_BAND_FirstSection = 1;
	N->FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
#ifdef Simulation_SLB_SHUFFLE
	DWORD P_Index;
	BYTE *WriteFreq = (BYTE*)calloc(NC->blockSizeTrack, sizeof(BYTE));//保證新寫入或Merge回來後, Band各Cylinder最多增加一次WriteFreq
	for (i = 0; i < NC->blockSizeTrack; i++){ assert(WriteFreq[i] == 0); }
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
#ifdef Simulation_SLB_SHUFFLE
				for (i = 0; i < NC->blockSizeTrack; i++){ WriteFreq[i] = 0; }
#endif
				//FASTmergeRWSLB_SHUFFLE2 -> FASTmergeRWSLB_SHUFFLE_TRACK -> FASTmergeRWSLB_ST_VT
				//FASTmergeRWSLB_ST_VT(simPtr);
				FASTmergeRWSLB_ST_VTLMA(simPtr);
				//Merge後如果又再寫應該視為初寫
				N->FirstSection = 1;
			}
			else{//guard track右邊不是fRW，右移gt一格
				//for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
				//	assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
				//	assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
				//}
				//SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				//for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
				//	assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
				//	N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				//}

				//修改
				DWORD tmpSC = SLB->guardTrack1stSection;
				for (i = 0; i < NC->CylinderSizeTrack; i++){
					assert(tmpSC % NC->trackSizeSector == 0);
					for (j = tmpSC; j < tmpSC + NC->trackSizeSector; j++){
						assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0xffffffffffff);//0x7fffffff;
						assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
					}
					tmpSC += NC->trackSizeSector; if (tmpSC > SLB->Partial_Section){ assert(tmpSC == SLB->Partial_Section + 1); tmpSC = 0; }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
				tmpSC = SLB->guardTrack1stSection;
				for (i = 0; i < NC->CylinderSizeTrack; i++){
					assert(tmpSC % NC->trackSizeSector == 0);
					for (j = tmpSC; j < tmpSC + NC->trackSizeSector; j++){
						assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
						N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0xffffffffffff;
						// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
					}
					tmpSC += NC->trackSizeSector; if (tmpSC > SLB->Partial_Section){ assert(tmpSC == SLB->Partial_Section + 1); tmpSC = 0; }
				}
			}
		}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (N->FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			//Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			N->FirstSection = 0;//等roation結束才標示這筆Write Request第一次position 5ms結束
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo);
		section_no = SectorTableNo % NC->blockSizeSector;// getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no < NC->LsizeBlock);

		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
			}
			N->DBalloc[block_no] = 1;
		}
#ifdef Simulation_SLB_SHUFFLE //給從未寫過DB配置TRANSFOR_LAYER記憶體
		if (N->SHUFFLE_TL_Alloc[block_no] == 0){
			N->SHUFFLE_TL[block_no].L2P_Index = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].P2L_Index = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].WriteFreq = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));//由P維護
			N->SHUFFLE_TL[block_no].DirtyUnit = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));//由P維護
			for (i = 0; i < NC->blockSizeTrack; i++){
				N->SHUFFLE_TL[block_no].L2P_Index[i] = i;
				N->SHUFFLE_TL[block_no].P2L_Index[i] = i;
				N->SHUFFLE_TL[block_no].WriteFreq[i] = 0;
				N->SHUFFLE_TL[block_no].DirtyUnit[i] = 0;
			}
			N->SHUFFLE_TL_Alloc[block_no] = 1;
		}assert(N->SHUFFLE_TL_Alloc[block_no] == 1);
		//檢查當前L address正確性: 找Physical Track Address
		assert(section_no / NC->trackSizeSector < NC->blockSizeTrack);
		//L address經由TL層的L2P得到P_Index
		P_Index = N->SHUFFLE_TL[block_no].L2P_Index[section_no / NC->trackSizeSector];
		assert(0 <= P_Index); assert(P_Index < NC->blockSizeTrack);//P_Index在正常範圍
		//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
		assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == section_no / NC->trackSizeSector);
		//統計WriteFreq
		//N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++;
		if (WriteFreq[P_Index] == 0){ N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++; WriteFreq[P_Index] = 1; }
#endif
		assert(N->blocks[block_no].sections[section_no].valid == 0 || N->blocks[block_no].sections[section_no].valid == 1);
		if (N->blocks[block_no].sections[section_no].valid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
#ifdef Simulation_SLB_SHUFFLE
			//統計DirtyUnit
			N->SHUFFLE_TL[block_no].DirtyUnit[P_Index]++;
#endif
		}
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;

		//N->section_cumu[SectorTableNo / NC->Section]++;
		//不更新L2P mapping table: setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1); setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(N->FirstSection == 0); assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#ifdef READ_REQUEST
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	N->W_RunTime += N->Req_RunTime;
	N->W_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->W_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#endif
#ifdef Simulation_SLB_SHUFFLE
	free(WriteFreq);
#endif
}

//void DM_SMR_DELAY_SHUFFLEwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
//	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
//	DWORD cur_8subband = -1, prev_8subband = -1;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD i = 0;
//	I64 SectorTableNo = 0;
//	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
//	DWORD block_no = 0, section_no = 0;
//	DWORD curSectionRMW = 0, curSection = -1;
//	DWORD oldData_PTrack = 0;
//	DWORD goRMW = 0, OneTimeRotation = 0;
//	DWORD goDB = 0;
//#ifdef Time_Overhead
//	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
//	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
//	N->content_rotate += lenSector;//寫多少sector轉多少sector
//	N->WW_content_rotate += lenSector;//
//#endif
//#ifdef Simulation_SLB_SHUFFLE
//	DWORD P_Index;
//#endif
//	//
//	DWORD t_time = N->time;
//	N->time++; assert(t_time < N->time);
//	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
//	N->b[offsetSector / NC->blockSizeSector]++;
//	//transfer time
//	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
//	assert(N->Req_RunTime >= 0);
//
//	while (1) {
//		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
//		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
//
//		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
//			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
//				FASTmergeRWSLB_DELAY_SHUFFLE(simPtr);
//			}else{//guard track右邊不是fRW，右移gt一格
//				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
//					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
//					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
//				}
//				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
//					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
//					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;
//					// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
//				}
//			}
//		}
//
//		//初始化block_no, section_no
//		assert(SectorTableNo % NC->Section == 0);
//		block_no = SectorTableNo / NC->blockSizeSector; section_no = SectorTableNo % NC->blockSizeSector;
//		assert(0 <= block_no && block_no < NC->LsizeBlock);
//		//給新DB配置sectors記憶體
//		if (N->DBalloc[block_no] == 0){
//			assert(N->blocks[block_no].sections == NULL);
//			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
//			assert(N->blocks[block_no].sections != NULL);
//			for (i = 0; i < NC->blockSizeSector; i++){
//				N->blocks[block_no].sections[i].valid = 1;
//				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
//			}
//			N->DBalloc[block_no] = 1;
//		}
//#ifdef Simulation_SLB_SHUFFLE //給從未寫過DB配置TRANSFOR_LAYER記憶體
//		if (N->SHUFFLE_TL_Alloc[block_no] == 0){
//			N->SHUFFLE_TL[block_no].L2P_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
//			N->SHUFFLE_TL[block_no].P2L_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
//			N->SHUFFLE_TL[block_no].WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
//			N->SHUFFLE_TL[block_no].DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
//			for (i = 0; i < NC->blockSizeCylinder; i++){
//				N->SHUFFLE_TL[block_no].L2P_Index[i] = i;
//				N->SHUFFLE_TL[block_no].P2L_Index[i] = i;
//				N->SHUFFLE_TL[block_no].WriteFreq[i] = 0;
//				N->SHUFFLE_TL[block_no].DirtyUnit[i] = 0;
//			}
//			N->SHUFFLE_TL_Alloc[block_no] = 1;
//		}assert(N->SHUFFLE_TL_Alloc[block_no] == 1);
//		//檢查當前L address正確性
//		assert(section_no / NC->CylinderSizeSector < NC->blockSizeCylinder);
//		//L address經由TL層的L2P得到P_Index
//		P_Index = N->SHUFFLE_TL[block_no].L2P_Index[section_no / NC->CylinderSizeSector];
//		assert(0 <= P_Index); assert(P_Index < NC->blockSizeCylinder);//P_Index在正常範圍
//		//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
//		assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == section_no / NC->CylinderSizeSector);
//		//統計WriteFreq
//		N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++;
//#endif
//		//更新Band或SLB裡的舊sector, 讓他們失效(因為新的write接下來要寫進SLB)
//		assert(N->blocks[block_no].sections[section_no].valid == 0 || N->blocks[block_no].sections[section_no].valid == 1);
//		if (N->blocks[block_no].sections[section_no].valid == 0){
//			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
//			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
//			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
//			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
//			N->SLB_Hit++;
//			N->cycleSLB_Hit++;
//			if (SLOTTABLE_BAND_FirstSection == 1){
//				SLOTTABLE_BAND_FirstSection = 0;
//				N->SlotTable_Band[block_no].InAct++;
//				N->SlotTable_InAct++;
//			}
//		}else{
//			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
//			assert(N->blocks[block_no].sections[section_no].valid == 1);
//			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
//			N->SLB_Miss++;
//			N->cycleSLB_Miss++;
//			//增加存活sector
//			N->l2pmapOvd.AA++;
//			if (SLOTTABLE_BAND_FirstSection == 1){
//				SLOTTABLE_BAND_FirstSection = 0;
//				N->SlotTable_Band[block_no].Act++;
//				N->SlotTable_Act++;
//			}
//#ifdef Simulation_SLB_SHUFFLE
//			//統計DirtyUnit
//			N->SHUFFLE_TL[block_no].DirtyUnit[P_Index]++;
//#endif
//		}
//#ifdef Time_Overhead
//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
//			FirstSection = 0;
//			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
//			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
//		}
//#endif
//		//更新SLB2DB: 寫到SLB裡
//		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
//		assert(offsetSector <  NC->LsizePage);
//		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
//		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
//		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
//		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
//		assert(N->blocks[block_no].sections[section_no].valid == 0);
//		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;
//		//更新寫入量統計資料
//		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
//		SLB->write_count += NC->Section;
//		//換下一個SLB Sector
//		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
//		//換下一個寫入的sector
//		offsetSector += NC->Section;
//		lenSector -= NC->Section;
//		if (lenSector <= 0){ break; }
//	}
//	//Resp
//	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
//	assert(FirstSection == 0); assert(N->Req_RunTime >= 5);
//	N->RunTime += N->Req_RunTime;
//	N->Finish_Time += N->Req_RunTime;
//	assert(N->Finish_Time - N->Arr_Time >= 5);
//	N->RespTIme += N->Finish_Time - N->Arr_Time;
//	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
//	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
//}
void DM_SMR_DELAY_SHUFFLEwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
#ifdef Simulation_SLB_SHUFFLE
	DWORD P_Index;
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
#ifdef Simulation_SLB_DELAY
				FASTmergeRWSLB_DELAY_SHUFFLE(simPtr);
				while (N->VC_BUFF_Len == NC->CylinderSizeSector){
					assert(SLB->lastRWsection == SLB->guardTrack1stSection);
					FASTmergeRWSLB_DELAY_SHUFFLE(simPtr);
				}
#endif
#ifdef Simulation_SLB_DELAY_ROUND
				/*FASTmergeRWSLB_DELAY_ROUND_SHUFFLE(simPtr);
				while (N->VC_BUFF_Len == NC->CylinderSizeSector){
					assert(SLB->lastRWsection == SLB->guardTrack1stSection);
					FASTmergeRWSLB_DELAY_ROUND_SHUFFLE(simPtr);
				}*/
#endif
			}else{//guard track右邊不是fRW，右移gt一格
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;
					// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

		//初始化block_no, section_no
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector; section_no = SectorTableNo % NC->blockSizeSector;
		assert(0 <= block_no && block_no < NC->LsizeBlock);
		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
			}
			N->DBalloc[block_no] = 1;
		}
#ifdef Simulation_SLB_SHUFFLE //給從未寫過DB配置TRANSFOR_LAYER記憶體
		if (N->SHUFFLE_TL_Alloc[block_no] == 0){
			N->SHUFFLE_TL[block_no].L2P_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].P2L_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
			N->SHUFFLE_TL[block_no].DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
			for (i = 0; i < NC->blockSizeCylinder; i++){
				N->SHUFFLE_TL[block_no].L2P_Index[i] = i;
				N->SHUFFLE_TL[block_no].P2L_Index[i] = i;
				N->SHUFFLE_TL[block_no].WriteFreq[i] = 0;
				N->SHUFFLE_TL[block_no].DirtyUnit[i] = 0;
			}
			N->SHUFFLE_TL_Alloc[block_no] = 1;
		}assert(N->SHUFFLE_TL_Alloc[block_no] == 1);
		//檢查當前L address正確性
		assert(section_no / NC->CylinderSizeSector < NC->blockSizeCylinder);
		//L address經由TL層的L2P得到P_Index
		P_Index = N->SHUFFLE_TL[block_no].L2P_Index[section_no / NC->CylinderSizeSector];
		assert(0 <= P_Index); assert(P_Index < NC->blockSizeCylinder);//P_Index在正常範圍
		//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
		assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == section_no / NC->CylinderSizeSector);
		//統計WriteFreq
		N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++;
#endif
		//更新Band或SLB裡的舊sector, 讓他們失效(因為新的write接下來要寫進SLB)
		I64 Band_SectorValid = N->blocks[block_no].sections[section_no].valid;//Band Valid
		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
		if (Band_SectorValid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//1或3都清成0, 這個SLB Sector不使用
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
#ifdef Simulation_SLB_SHUFFLE
			//統計DirtyUnit
			N->SHUFFLE_TL[block_no].DirtyUnit[P_Index]++;
#endif
		}
#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		
		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;
		
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		//換下一個寫入的sector
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(FirstSection == 0); assert(N->Req_RunTime >= 5);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	assert(N->Finish_Time - N->Arr_Time >= 5);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
//void DM_SMR_DELAY_LEFT_SHUFFLEwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
//	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
//	DWORD cur_8subband = -1, prev_8subband = -1;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD i = 0;
//	I64 SectorTableNo = 0;
//	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
//	DWORD block_no = 0, section_no = 0;
//	DWORD curSectionRMW = 0, curSection = -1;
//	DWORD oldData_PTrack = 0;
//	DWORD goRMW = 0, OneTimeRotation = 0;
//	DWORD goDB = 0;
//#ifdef Time_Overhead
//	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
//	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
//	N->content_rotate += lenSector;//寫多少sector轉多少sector
//	N->WW_content_rotate += lenSector;//
//#endif
//#ifdef Simulation_SLB_SHUFFLE
//	DWORD P_Index;
//#endif
//	//
//	DWORD t_time = N->time;
//	N->time++; assert(t_time < N->time);
//	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
//	N->b[offsetSector / NC->blockSizeSector]++;
//	//transfer time
//	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
//	assert(N->Req_RunTime >= 0);
//	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
//	while (1) {
//		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
//		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
//		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
//			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
//#ifdef Simulation_SLB_DELAY_ROUND
//				//FASTmergeRWSLB_DELAY_tmp_SHUFFLE(simPtr); //預估Delay Left最佳情況
//				//FASTmergeRWSLB_DELAY_LEFT_SHUFFLE(simPtr);
//				FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE(simPtr);//四個參數找最佳範圍
//				while (N->VC_BUFF_Len == NC->CylinderSizeSector){
//					assert(SLB->lastRWsection == SLB->guardTrack1stSection);
//					//FASTmergeRWSLB_DELAY_tmp_SHUFFLE(simPtr); //預估Delay Left最佳情況
//					//FASTmergeRWSLB_DELAY_LEFT_SHUFFLE(simPtr);
//					FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE(simPtr);
//				}
//#endif
//			}else{//guard track右邊不是fRW，右移gt一格
//				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
//					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
//					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
//				}
//				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
//				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
//					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
//					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;
//					// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
//				}
//			}
//		}
//
//		//初始化block_no, section_no
//		assert(SectorTableNo % NC->Section == 0);
//		block_no = SectorTableNo / NC->blockSizeSector; section_no = SectorTableNo % NC->blockSizeSector;
//		assert(0 <= block_no && block_no < NC->LsizeBlock);
//		//給新DB配置sectors記憶體
//		if (N->DBalloc[block_no] == 0){
//			assert(N->blocks[block_no].sections == NULL);
//			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
//			assert(N->blocks[block_no].sections != NULL);
//			for (i = 0; i < NC->blockSizeSector; i++){
//				N->blocks[block_no].sections[i].valid = 1;
//				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
//			}
//			N->DBalloc[block_no] = 1;
//		}
//#ifdef Simulation_SLB_SHUFFLE //給從未寫過DB配置TRANSFOR_LAYER記憶體
//		if (N->SHUFFLE_TL_Alloc[block_no] == 0){
//			N->SHUFFLE_TL[block_no].L2P_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
//			N->SHUFFLE_TL[block_no].P2L_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
//			N->SHUFFLE_TL[block_no].WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
//			N->SHUFFLE_TL[block_no].DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
//			for (i = 0; i < NC->blockSizeCylinder; i++){
//				N->SHUFFLE_TL[block_no].L2P_Index[i] = i;
//				N->SHUFFLE_TL[block_no].P2L_Index[i] = i;
//				N->SHUFFLE_TL[block_no].WriteFreq[i] = 0;
//				N->SHUFFLE_TL[block_no].DirtyUnit[i] = 0;
//			}
//			N->SHUFFLE_TL_Alloc[block_no] = 1;
//		}assert(N->SHUFFLE_TL_Alloc[block_no] == 1);
//		//檢查當前L address正確性
//		assert(section_no / NC->CylinderSizeSector < NC->blockSizeCylinder);
//		//L address經由TL層的L2P得到P_Index
//		P_Index = N->SHUFFLE_TL[block_no].L2P_Index[section_no / NC->CylinderSizeSector];
//		assert(0 <= P_Index); assert(P_Index < NC->blockSizeCylinder);//P_Index在正常範圍
//		//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
//		assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == section_no / NC->CylinderSizeSector);
//		//統計WriteFreq
//		N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++;
//#endif
//		I64 Sector_Valid = 1;
//		//更新Band或SLB裡的舊sector, 讓他們失效(因為新的write接下來要寫進SLB)
//		I64 Band_SectorValid = N->blocks[block_no].sections[section_no].valid;//Band Valid
//		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
//		if (Band_SectorValid == 0){
//			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
//			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
//			assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo);
//			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
//			//保留sector特性, 3是delay sector
//			Sector_Valid = N->blocks[NC->LsizeBlock].sections[LBsector].valid;
//			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//1或3都清成0, 這個SLB Sector不使用
//			N->SLB_Hit++;
//			N->cycleSLB_Hit++;
//			if (SLOTTABLE_BAND_FirstSection == 1){
//				SLOTTABLE_BAND_FirstSection = 0;
//				N->SlotTable_Band[block_no].InAct++;
//				N->SlotTable_InAct++;
//			}
//		}else{
//			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
//			assert(N->blocks[block_no].sections[section_no].valid == 1);
//			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
//			N->SLB_Miss++;
//			N->cycleSLB_Miss++;
//			assert(Sector_Valid == 1);
//			//增加存活sector
//			N->l2pmapOvd.AA++;
//			if (SLOTTABLE_BAND_FirstSection == 1){
//				SLOTTABLE_BAND_FirstSection = 0;
//				N->SlotTable_Band[block_no].Act++;
//				N->SlotTable_Act++;
//			}
//#ifdef Simulation_SLB_SHUFFLE
//			//統計DirtyUnit
//			N->SHUFFLE_TL[block_no].DirtyUnit[P_Index]++;
//#endif
//		}
//#ifdef Time_Overhead
//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
//			FirstSection = 0;
//			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
//			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
//		}
//#endif
//		//更新SLB2DB: 寫到SLB裡
//		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
//		assert(offsetSector <  NC->LsizePage);
//		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
//		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
//		assert(Sector_Valid == 1 || Sector_Valid == 3);//
//		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = Sector_Valid;// 1;
//
//		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
//		assert(N->blocks[block_no].sections[section_no].valid == 0);
//		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;
//
//		//更新寫入量統計資料
//		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
//		SLB->write_count += NC->Section;
//		//換下一個SLB Sector
//		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
//		//換下一個寫入的sector
//		offsetSector += NC->Section;
//		lenSector -= NC->Section;
//		if (lenSector <= 0){ break; }
//	}
//	//Resp
//	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
//	assert(FirstSection == 0); assert(N->Req_RunTime >= 5);
//	N->RunTime += N->Req_RunTime;
//	N->Finish_Time += N->Req_RunTime;
//	assert(N->Finish_Time - N->Arr_Time >= 5);
//	N->RespTIme += N->Finish_Time - N->Arr_Time;
//	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
//	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
//}
void DM_SMR_DELAY_LEFT_SHUFFLEwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD SLOTTABLE_BAND_FirstSection = 1;
	N->FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
#ifdef Simulation_SLB_SHUFFLE
	DWORD P_Index;
	BYTE *WriteFreq = (BYTE*)calloc(NC->blockSizeCylinder, sizeof(BYTE));
	for (i = 0; i < NC->blockSizeCylinder; i++){ assert(WriteFreq[i] == 0); }
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
#ifdef Simulation_SLB_SHUFFLE
				for (i = 0; i < NC->blockSizeCylinder; i++){ WriteFreq[i] = 0; }
#endif
#ifdef Simulation_SLB_DELAY_ROUND
				//FASTmergeRWSLB_DELAY_tmp_SHUFFLE(simPtr); //預估Delay Left最佳情況
				//FASTmergeRWSLB_DELAY_LEFT_SHUFFLE(simPtr);
				FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE(simPtr);//四個參數找最佳範圍
				while (N->VC_BUFF_Len == NC->CylinderSizeSector){
					assert(SLB->lastRWsection == SLB->guardTrack1stSection);
					//FASTmergeRWSLB_DELAY_tmp_SHUFFLE(simPtr); //預估Delay Left最佳情況
					//FASTmergeRWSLB_DELAY_LEFT_SHUFFLE(simPtr);
					FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE(simPtr);
				}
#endif
				//Merge後如果又再寫應該視為初寫
				N->FirstSection = 1;
			}else{//guard track右邊不是fRW，右移gt一格
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;
					// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

		//初始化block_no, section_no
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector; section_no = SectorTableNo % NC->blockSizeSector;
		assert(0 <= block_no && block_no < NC->LsizeBlock);
		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
			}
			N->DBalloc[block_no] = 1;
		}
#ifdef Simulation_SLB_SHUFFLE //給從未寫過DB配置TRANSFOR_LAYER記憶體
		if (N->SHUFFLE_TL_Alloc[block_no] == 0){
			N->SHUFFLE_TL[block_no].L2P_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].P2L_Index = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
			N->SHUFFLE_TL[block_no].DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//由P維護
			for (i = 0; i < NC->blockSizeCylinder; i++){
				N->SHUFFLE_TL[block_no].L2P_Index[i] = i;
				N->SHUFFLE_TL[block_no].P2L_Index[i] = i;
				N->SHUFFLE_TL[block_no].WriteFreq[i] = 0;
				N->SHUFFLE_TL[block_no].DirtyUnit[i] = 0;
			}
			N->SHUFFLE_TL_Alloc[block_no] = 1;
		}assert(N->SHUFFLE_TL_Alloc[block_no] == 1);
		//檢查當前L address正確性
		assert(section_no / NC->CylinderSizeSector < NC->blockSizeCylinder);
		//L address經由TL層的L2P得到P_Index
		P_Index = N->SHUFFLE_TL[block_no].L2P_Index[section_no / NC->CylinderSizeSector];
		assert(0 <= P_Index); assert(P_Index < NC->blockSizeCylinder);//P_Index在正常範圍
		//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
		assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == section_no / NC->CylinderSizeSector);
		//統計WriteFreq
		if (WriteFreq[P_Index] == 0){ N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++; WriteFreq[P_Index] = 1; }
#endif
		I64 Sector_Valid = 1;
		//更新Band或SLB裡的舊sector, 讓他們失效(因為新的write接下來要寫進SLB)
		I64 Band_SectorValid = N->blocks[block_no].sections[section_no].valid;//Band Valid
		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
		if (Band_SectorValid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
			//保留sector特性, 3是delay sector
			Sector_Valid = N->blocks[NC->LsizeBlock].sections[LBsector].valid;
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//1或3都清成0, 這個SLB Sector不使用
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			assert(Sector_Valid == 1);
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
#ifdef Simulation_SLB_SHUFFLE
			//統計DirtyUnit
			N->SHUFFLE_TL[block_no].DirtyUnit[P_Index]++;
#endif
		}
#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (N->FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			//Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			N->FirstSection = 0;//做完rotate 標示這筆Write Request第一次position 5ms結束
		}
#endif
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		assert(Sector_Valid == 1 || Sector_Valid == 3);//
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = Sector_Valid;//保留Sector特性, 一般Dirty Sector或Delay Sector

		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;

		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		//換下一個寫入的sector
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(N->FirstSection == 0); assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#ifdef READ_REQUEST
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	N->W_RunTime += N->Req_RunTime;
	N->W_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->W_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#endif
#ifdef Simulation_SLB_SHUFFLE
	free(WriteFreq);
#endif
}
void DM_SMR_DELAY_LEFT_SHUFFLE_TRACKwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD SLOTTABLE_BAND_FirstSection = 1;
	N->FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
#ifdef Simulation_SLB_SHUFFLE
	DWORD P_Index;
	BYTE *WriteFreq = (BYTE*)calloc(NC->blockSizeTrack, sizeof(BYTE));
	for (i = 0; i < NC->blockSizeTrack; i++){ assert(WriteFreq[i] == 0); }
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
#ifdef Simulation_SLB_SHUFFLE
				for (i = 0; i < NC->blockSizeTrack; i++){ WriteFreq[i] = 0; }
#endif
#ifdef Simulation_SLB_DELAY_ROUND
				FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE_TRACK(simPtr);//四個參數找最佳範圍 //來自FASTmergeRWSLB_DELAY_LEFT_SHUFFLE
				while (N->VC_BUFF_Len == NC->CylinderSizeSector){
					assert(SLB->lastRWsection == SLB->guardTrack1stSection);
					FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE_TRACK(simPtr);
				}
#endif
				//Merge後如果又再寫應該視為初寫
				N->FirstSection = 1;
			}else{//guard track右邊不是fRW，右移gt一格
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;
					// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

		//初始化block_no, section_no
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector; section_no = SectorTableNo % NC->blockSizeSector;
		assert(0 <= block_no && block_no < NC->LsizeBlock);
		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
			}
			N->DBalloc[block_no] = 1;
		}
#ifdef Simulation_SLB_SHUFFLE //給從未寫過DB配置TRANSFOR_LAYER記憶體
		if (N->SHUFFLE_TL_Alloc[block_no] == 0){
			N->SHUFFLE_TL[block_no].L2P_Index = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].P2L_Index = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].WriteFreq = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));//由P維護
			N->SHUFFLE_TL[block_no].DirtyUnit = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));//由P維護
			for (i = 0; i < NC->blockSizeTrack; i++){
				N->SHUFFLE_TL[block_no].L2P_Index[i] = i;
				N->SHUFFLE_TL[block_no].P2L_Index[i] = i;
				N->SHUFFLE_TL[block_no].WriteFreq[i] = 0;
				N->SHUFFLE_TL[block_no].DirtyUnit[i] = 0;
			}
			N->SHUFFLE_TL_Alloc[block_no] = 1;
		}assert(N->SHUFFLE_TL_Alloc[block_no] == 1);
		//檢查當前L address正確性
		assert(section_no / NC->trackSizeSector < NC->blockSizeTrack);
		//L address經由TL層的L2P得到P_Index
		P_Index = N->SHUFFLE_TL[block_no].L2P_Index[section_no / NC->trackSizeSector];
		assert(0 <= P_Index); assert(P_Index < NC->blockSizeTrack);//P_Index在正常範圍
		//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
		assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == section_no / NC->trackSizeSector);
		//統計WriteFreq
		if (WriteFreq[P_Index] == 0){ N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++; WriteFreq[P_Index] = 1; }
#endif
		I64 Sector_Valid = 1;
		//更新Band或SLB裡的舊sector, 讓他們失效(因為新的write接下來要寫進SLB)
		I64 Band_SectorValid = N->blocks[block_no].sections[section_no].valid;//Band Valid
		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
		if (Band_SectorValid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
			//保留sector特性, 3是delay sector
			Sector_Valid = N->blocks[NC->LsizeBlock].sections[LBsector].valid;
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//1或3都清成0, 這個SLB Sector不使用
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			assert(Sector_Valid == 1);
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
#ifdef Simulation_SLB_SHUFFLE
			//統計DirtyUnit
			N->SHUFFLE_TL[block_no].DirtyUnit[P_Index]++;
#endif
		}
#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (N->FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			//Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			N->FirstSection = 0;//做完rotate 標示這筆Write Request第一次position 5ms結束
		}
#endif
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		assert(Sector_Valid == 1 || Sector_Valid == 3);//
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = Sector_Valid;//保留Sector特性, 一般Dirty Sector或Delay Sector

		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;

		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		//換下一個寫入的sector
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(N->FirstSection == 0); assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#ifdef READ_REQUEST
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	N->W_RunTime += N->Req_RunTime;
	N->W_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->W_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#endif
#ifdef Simulation_SLB_SHUFFLE
	free(WriteFreq);
#endif
}
//DLST: DELAY_LEFT_SHUFFLE_TRACK, VTLMA: Victim Track + Limited Merge Associativity
void DM_SMR_DLST_VTLMAwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD SLOTTABLE_BAND_FirstSection = 1;
	N->FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
#ifdef Simulation_SLB_SHUFFLE
	DWORD P_Index;
	BYTE *WriteFreq = (BYTE*)calloc(NC->blockSizeTrack, sizeof(BYTE));
	for (i = 0; i < NC->blockSizeTrack; i++){ assert(WriteFreq[i] == 0); }
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	assert(NC->trackSizeSector == 1024); assert(NC->CylinderSizeTrack == 4);
	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
#ifdef Simulation_SLB_SHUFFLE
				for (i = 0; i < NC->blockSizeTrack; i++){ WriteFreq[i] = 0; }
#endif
#ifdef Simulation_SLB_DELAY_ROUND
				//FASTmergeRWSLB_CONT_DLST_VT(simPtr);
				FASTmergeRWSLB_CONT_DLST_VTLMA(simPtr);//四個參數找最佳範圍 //來自FASTmergeRWSLB_DELAY_LEFT_SHUFFLE
				/*while (N->VC_BUFF_Len == NC->CylinderSizeSector){
					assert(SLB->lastRWsection == SLB->guardTrack1stSection);
					FASTmergeRWSLB_CONT_DLST_VTLMA(simPtr);
				}*/
				//修改
				while (N->WB_BUFF_Len == NC->trackSizeSection){//N->WB_BUFF_Len: 紀錄Delay+LoopBand共有多少寫回
					assert(SLB->lastRWsection == SLB->guardTrack1stSection);
					//FASTmergeRWSLB_CONT_DLST_VT(simPtr);
					FASTmergeRWSLB_CONT_DLST_VTLMA(simPtr);
				}
#endif
				//Merge後如果又再寫應該視為初寫
				N->FirstSection = 1;
			}else{//guard track右邊不是fRW，右移gt一格

				//for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
				//	assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
				//	assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
				//}
				//SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				//for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
				//	assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
				//	N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;
				//	// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				//}

				//修改
				DWORD tmpSC = SLB->guardTrack1stSection;
				for (i = 0; i < NC->CylinderSizeTrack; i++){
					assert(tmpSC % NC->trackSizeSector == 0);
					for (j = tmpSC; j < tmpSC + NC->trackSizeSector; j++){
						assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0xffffffffffff);//0x7fffffff;
						assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
					}
					tmpSC += NC->trackSizeSector; if (tmpSC > SLB->Partial_Section){ assert(tmpSC == SLB->Partial_Section + 1); tmpSC = 0; }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
				tmpSC = SLB->guardTrack1stSection;
				for (i = 0; i < NC->CylinderSizeTrack; i++){
					assert(tmpSC % NC->trackSizeSector == 0);
					for (j = tmpSC; j < tmpSC + NC->trackSizeSector; j++){
						assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
						N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0xffffffffffff;
						// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
					}
					tmpSC += NC->trackSizeSector; if (tmpSC > SLB->Partial_Section){ assert(tmpSC == SLB->Partial_Section + 1); tmpSC = 0; }
				}
				
			}
		}

		//初始化block_no, section_no
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector; section_no = SectorTableNo % NC->blockSizeSector;
		assert(0 <= block_no && block_no < NC->LsizeBlock);
		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
#ifdef SLB_Mapping_Cache
				N->blocks[block_no].sections[i].inCache = NULL;
#ifdef CFLRU
				N->blocks[block_no].sections[i].cache_stat = 0;
				N->blocks[block_no].sections[i].cache_lev = 0;
#endif
#endif
			}
			N->DBalloc[block_no] = 1;
		}
#ifdef Simulation_SLB_SHUFFLE //給從未寫過DB配置TRANSFOR_LAYER記憶體
		if (N->SHUFFLE_TL_Alloc[block_no] == 0){
			N->SHUFFLE_TL[block_no].L2P_Index = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].P2L_Index = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
			N->SHUFFLE_TL[block_no].WriteFreq = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));//由P維護
			N->SHUFFLE_TL[block_no].DirtyUnit = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));//由P維護
			for (i = 0; i < NC->blockSizeTrack; i++){
				N->SHUFFLE_TL[block_no].L2P_Index[i] = i;
				N->SHUFFLE_TL[block_no].P2L_Index[i] = i;
				N->SHUFFLE_TL[block_no].WriteFreq[i] = 0;
				N->SHUFFLE_TL[block_no].DirtyUnit[i] = 0;
			}
			N->SHUFFLE_TL_Alloc[block_no] = 1;
		}assert(N->SHUFFLE_TL_Alloc[block_no] == 1);
		//檢查當前L address正確性
		assert(section_no / NC->trackSizeSector < NC->blockSizeTrack);
		//L address經由TL層的L2P得到P_Index
		P_Index = N->SHUFFLE_TL[block_no].L2P_Index[section_no / NC->trackSizeSector];
		assert(0 <= P_Index); assert(P_Index < NC->blockSizeTrack);//P_Index在正常範圍
		//P_Index經由TL層的P2L得到L_Index, 確認L_Index就是L address
		assert(N->SHUFFLE_TL[block_no].P2L_Index[P_Index] == section_no / NC->trackSizeSector);
		//統計WriteFreq
		if (WriteFreq[P_Index] == 0){ N->SHUFFLE_TL[block_no].WriteFreq[P_Index]++; WriteFreq[P_Index] = 1; }
#endif
		I64 Sector_Valid = 1;
		//更新Band或SLB裡的舊sector, 讓他們失效(因為新的write接下來要寫進SLB)
		I64 Band_SectorValid = N->blocks[block_no].sections[section_no].valid;//Band Valid
		assert(Band_SectorValid == 0 || Band_SectorValid == 1);
		if (Band_SectorValid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].sector == SectorTableNo);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1 || N->blocks[NC->LsizeBlock].sections[LBsector].valid == 3);
			//保留sector特性, 3是delay sector
			Sector_Valid = N->blocks[NC->LsizeBlock].sections[LBsector].valid;
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;//1或3都清成0, 這個SLB Sector不使用
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			assert(Sector_Valid == 1);
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
#ifdef Simulation_SLB_SHUFFLE
			//統計DirtyUnit
			N->SHUFFLE_TL[block_no].DirtyUnit[P_Index]++;
#endif
		}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (N->FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			//Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
			Calc_Rotate_SkewSMR(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			N->FirstSection = 0;//做完rotate 標示這筆Write Request第一次position 5ms結束
		}
#endif
		//更新SLB2DB: 寫到SLB裡
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		assert(Sector_Valid == 1 || Sector_Valid == 3);//
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = Sector_Valid;//保留Sector特性, 一般Dirty Sector或Delay Sector

		//更新DB2SLB: Data從Band上sector指到SLB裡的最新sector
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;

#ifdef SLB_Mapping_Cache
		CacheAccess(simPtr, SectorTableNo, N->blocks[block_no].sections[section_no].sector, 2);
#endif
		
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		//換下一個SLB Sector
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		//換下一個寫入的sector
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(N->FirstSection == 0); assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#ifdef READ_REQUEST
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	N->W_RunTime += N->Req_RunTime;
	N->W_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->W_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#endif
#ifdef Simulation_SLB_SHUFFLE
	free(WriteFreq);
#endif
}
#endif

#ifdef SACE
//ICCD 2017 Shingle Aware Cache Management
void SACEwriteSector_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector) {
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);

	DWORD LBA, LPA, flag = 0, newBlock, dataBlock;
	DWORD i = 0, j = 0, k = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0; DWORD block_no = 0, section_no = 0;
	DWORD LBsector;
	DWORD curSectionRMW = 0, curSection = -1; DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0; DWORD goDB = 0; 
#ifdef Time_Overhead
	DWORD SLOTTABLE_BAND_FirstSection = 1;
	N->FirstSection = 1;
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;
	//SACE
	DWORD end_sector = (DWORD)offsetSector + lenSector - 1;
	DWORD SACE_Sector_Len = 0, Max_Len = lenSector;
	DWORD *SACE_Sector = (DWORD*)calloc(lenSector, sizeof(DWORD));
#endif
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizeSector; 
		SectorTableNo = offsetSector; assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		block_no = SectorTableNo / NC->blockSizeSector; section_no = SectorTableNo % NC->blockSizeSector;
		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSector);
#ifdef SACE
		//分情況寫入
		if (section_no / NC->trackSizeSector >= NC->blockSizeTrack - NC->CylinderSizeTrack){//寫到最後cylinder, 原地寫
			assert(SACE_Sector_Len < Max_Len); SACE_Sector[SACE_Sector_Len] = SectorTableNo; SACE_Sector_Len++;
			N->WriteBandLC++;
		}
		else if (N->DBalloc[block_no] == 1 && N->blocks[block_no].SACE_TKDirtySector[section_no / NC->trackSizeSector + NC->CylinderSizeTrack] == NC->trackSizeSector){//寫到後面有GT的地方, 原地寫
			assert(section_no / NC->trackSizeSector + NC->CylinderSizeTrack < NC->blockSizeTrack);
			if (N->blocks[block_no].sections[section_no].valid == 0){
				LBsector = N->blocks[block_no].sections[section_no].sector;
				assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
				assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
				N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
				//
				N->blocks[block_no].sections[section_no].valid = 1;
				N->blocks[block_no].sections[section_no].sector = block_no*NC->blockSizeSector + section_no;
				//DB上的Sector
				assert(section_no / NC->trackSizeSector < NC->blockSizeTrack);
				N->blocks[block_no].SACE_TKDirtySector[section_no / NC->trackSizeSector]--;
				assert(N->blocks[block_no].SACE_TKDirtySector[section_no / NC->trackSizeSector] >= 0);
#ifdef MergeAssoc_SLBpart
				N->l2pmapOvd.AA--;//統計SLB Live Rate
#endif
			}
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(SACE_Sector_Len < Max_Len); SACE_Sector[SACE_Sector_Len] = SectorTableNo; SACE_Sector_Len++;
			N->WriteBandBGT++;
		}
		else{//沒有GT擋寫干擾, 寫進SLB
			N->WriteSLB++;
			if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
				if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
					SACEmerge_NoStNoDB(simPtr); N->FirstSection = 1;//Merge後如果又再寫應該視為初寫
				}
				else{//guard track右邊不是fRW，右移gt一格
					for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
						assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff); assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					}
					SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
					for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
						assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0); N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;
					}
				}
			}
			//寫進SLB
			assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
			assert(offsetSector <  NC->LsizePage);
			assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
			//給新DB配置sectors記憶體
			if (N->DBalloc[block_no] == 0){
				assert(N->blocks[block_no].sections == NULL);
				N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
				assert(N->blocks[block_no].sections != NULL);
				for (i = 0; i < NC->blockSizeSector; i++){
					N->blocks[block_no].sections[i].valid = 1;
					N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
				}
				N->DBalloc[block_no] = 1;
#ifdef SACE
				assert(N->blocks[block_no].SACE_TKDirtySector == NULL);
				N->blocks[block_no].SACE_TKDirtySector = (DWORD*)calloc(NC->blockSizeTrack, sizeof(DWORD));
				assert(N->blocks[block_no].SACE_TKDirtySector != NULL);
				for (i = 0; i < NC->blockSizeTrack; i++){
					N->blocks[block_no].SACE_TKDirtySector[i] = 0;//第一次配, Initialize 0 表示目前沒Dirty Sector
				}
#endif
			}
			//檢查block_no上的track dirty和SACE_TKDirtySector統計保持一致
			const DWORD check_start_sector = section_no / NC->trackSizeSector * NC->trackSizeSector, check_end_sector = check_start_sector + NC->trackSizeSector - 1;
			DWORD DirtySector = 0;
			for (i = check_start_sector; i <= check_end_sector; i++){ if (N->blocks[block_no].sections[i].valid == 0){ DirtySector++; } } 
			assert(DirtySector == N->blocks[block_no].SACE_TKDirtySector[section_no / NC->trackSizeSector]);
			//invalid在SLB或DB上的舊page
			assert(N->blocks[block_no].sections[section_no].valid == 0 || N->blocks[block_no].sections[section_no].valid == 1);
			if (N->blocks[block_no].sections[section_no].valid == 0){
				LBsector = N->blocks[block_no].sections[section_no].sector; assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section); assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
				N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
				/*紀錄資料Hit*/ N->SLB_Hit++; N->cycleSLB_Hit++;
				if (SLOTTABLE_BAND_FirstSection == 1){ SLOTTABLE_BAND_FirstSection = 0; N->SlotTable_Band[block_no].InAct++; N->SlotTable_InAct++; }
			}
			else{
				assert(0 <= section_no); assert(section_no < NC->blockSizeSection); assert(N->blocks[block_no].sections[section_no].valid == 1);
				//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
				N->blocks[block_no].sections[section_no].valid = 0;
				/*紀錄資料Miss*/N->SLB_Miss++; N->cycleSLB_Miss++;
#ifdef MergeAssoc_SLBpart
				N->l2pmapOvd.AA++;//統計SLB Live Rate: 增加存活sector
#endif
				if (SLOTTABLE_BAND_FirstSection == 1){ SLOTTABLE_BAND_FirstSection = 0; N->SlotTable_Band[block_no].Act++; N->SlotTable_Act++; }
#ifdef SACE
				//DB上的Sector第一次被寫到
				assert(section_no / NC->trackSizeSector < NC->blockSizeTrack);
				N->blocks[block_no].SACE_TKDirtySector[section_no / NC->trackSizeSector]++; 
				assert(N->blocks[block_no].SACE_TKDirtySector[section_no / NC->trackSizeSector] <= NC->trackSizeSector);
#endif
			}
			assert(N->blocks[block_no].sections[section_no].valid == 0);
			N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;
			//SACE_Sector紀錄寫到SLB哪裡
			assert(SACE_Sector_Len < Max_Len); SACE_Sector[SACE_Sector_Len] = (DWORD)NC->LsizeSector + SLB->lastRWsection*NC->Section; SACE_Sector_Len++;
			//換下一個section做寫入
			SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
			//更新寫入量統計資料
			SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		}
#endif
		SLB->write_count += NC->Section;
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}

#ifdef Time_Overhead
	assert(0 < SACE_Sector_Len); assert(SACE_Sector_Len <= Max_Len);
	qsort(SACE_Sector, SACE_Sector_Len, sizeof(DWORD), compare);
	//if (SACE_Sector_Len > 1){ for (i = 0; i < SACE_Sector_Len - 1; i++){ assert(SACE_Sector[i] <= SACE_Sector[i + 1]); } }
	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	test(simPtr, (I64)SACE_Sector[0], 0);
	Calc_Rotate_SkewSMR(simPtr, (I64)SACE_Sector[0], 0);
	for (i = 1; i < SACE_Sector_Len; i++){
		if (SACE_Sector[i] / NC->trackSizeSector != SACE_Sector[i - 1] / NC->trackSizeSector){
			test(simPtr, (I64)SACE_Sector[i], 0);
			Calc_Rotate_SkewSMR(simPtr, (I64)SACE_Sector[i], 0);
		}
	}
	//SACE
	free(SACE_Sector);
#endif
	//Resp
	assert(N->Req_RunTime >= 2);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime; 
	assert(N->Finish_Time - N->Arr_Time >= 2);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#ifdef READ_REQUEST
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	N->W_RunTime += N->Req_RunTime;
	N->W_RespTIme += (N->Finish_Time - N->Arr_Time);
	N->W_WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
#endif
}
#endif



#ifdef Simulation_SLBPM
void DM_SMRwriteSector_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//DWORD n1 = 23000, n2 = 34500; /**(I64)SLB->SLBsection_num / 50331648*/ /** (I64)SLB->SLBsection_num / 50331648;*/
			//|| N->SlotTable_Act > n1 || (N->SlotTable_Act + N->SlotTable_InAct > n2)
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
				FASTmergeRWSLBpart2_Sr_NoStNoDB(simPtr);
				FASTmergeRWSLBpart2_Sr_NoStNoDB(simPtr);
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo);
		section_no = SectorTableNo % NC->blockSizeSector;// getsection(simPtr, SectorTableNo);
		//assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		assert(0 <= block_no && block_no < NC->LsizeBlock);

		//if (block_no == NC->PsizeBlock - 1){
		//	assert(0 <= section_no && section_no <= SLB->Partial_Section);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
		//	N->blocks[block_no].sections[section_no].valid = 0;
		//	N->SLB_Hit++;
		//	N->cycleSLB_Hit++;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].InAct++;
		//		N->SlotTable_InAct++;
		//	}
		//}
		//else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
		//	assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
		//	N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
		//	N->SLB_Miss++;
		//	N->cycleSLB_Miss++;
		//	//增加存活sector
		//	N->l2pmapOvd.AA++;
		//	//有不同的section寫入
		//	//N->blocks[block_no].sections[section_no].unique = 1;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].Act++;
		//		N->SlotTable_Act++;
		//	}
		//}//in Data Band

		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
			}
			N->DBalloc[block_no] = 1;
		}

		assert(N->blocks[block_no].sections[section_no].valid == 0 || N->blocks[block_no].sections[section_no].valid == 1);
		if (N->blocks[block_no].sections[section_no].valid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
		}
		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;

		//N->section_cumu[SectorTableNo / NC->Section]++;
		////更新L2P mapping table
		//setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		//setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		//N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(FirstSection == 0); assert(N->Req_RunTime >= 5);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	assert(N->Finish_Time - N->Arr_Time >= 5);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
void SLB_NoRAM_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector)//write buff有問題
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	//轉成section len的request
	const I64 start_section = offsetSector / NC->Section;
	const I64 end_section = (offsetSector + lenSector - 1) / NC->Section;
	const I64 section_len = end_section - start_section + 1;
	I64 tmp_start = offsetSector / NC->Section; assert(tmp_start >= 0);
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section); assert(section_len >= 1);
	tmp_start = start_section;
	assert(tmp_start*NC->Section <= offsetSector + lenSector - 1);

	if (NC->Section == 1){
		assert(tmp_start == offsetSector); assert(section_len == lenSector);
	}
	simStat *SS = &(simPtr->simStatObj);
	//#ifdef Time_Overhead
	//	if (SS->writeReq % 240 == 0){
	//		test(simPtr, NC->LsizeSector/2, 0);
	//	}
	//#endif

	//DM_SMRwriteSector_NoStNoDB(simPtr, tmp_start*NC->Section, section_len*NC->Section);
	DM_SMRwriteSector_PM(simPtr, tmp_start*NC->Section, section_len*NC->Section);
}

void GuardCyl2SMR(sim *simPtr) { // Handle a write of sectors to the MLC flash
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	DWORD block_no = 0, section_no = 0;
	I64 SectorTableNo = 0;
	DWORD start_sector = 0, end_sector = 0;
#ifdef Time_Overhead
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += NC->CylinderSizeSector;//寫多少sector轉多少sector
	N->WW_content_rotate += NC->CylinderSizeSector;//
#endif
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)NC->CylinderSizeSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 40);
	//統計
	SLB->pageWrite += N->WB2SMR_TASK_LEN;
	test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
	Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
	//初始化dataBlock
	DWORD dataBlock = N->CUTPM_CANDIDATE_SIG;
	assert(N->DBalloc[dataBlock] == 1);//有配置的DB
	assert(N->blocks[dataBlock].Cut == 1);
	//DB上的GuardCyl搬運範圍
	start_sector = N->blocks[dataBlock].GuardCylIndex*NC->CylinderSizeSector;
	end_sector = start_sector + NC->CylinderSizeSector - 1;
	assert(end_sector < NC->blockSizeSector);
	assert(end_sector - start_sector + 1 == NC->CylinderSizeSector);
	assert(NC->CylinderSizeTrack*NC->trackSizeSection == NC->CylinderSizeSector);
	/*for (i = start_sector; i <= end_sector; i++){
	if (N->blocks[dataBlock].sections[i].valid != 1){
	printf("%lu: %lu %lu, %lu\n", dataBlock, start_sector, end_sector, i);
	printf("%lu %lu %lu\n", N->blocks[dataBlock].sections[i - 2].valid, N->blocks[dataBlock].sections[i - 1].valid, N->blocks[dataBlock].sections[i].valid);
	system("pause");
	}
	}*/

	//搬運
	for (i = start_sector; i <= end_sector; i++){
		assert(i < NC->blockSizeSector); assert(i / NC->CylinderSizeSector == N->blocks[dataBlock].GuardCylIndex);
		SectorTableNo = dataBlock*NC->blockSizeSector + i;
		//選用的GuardCyl是最後一個有Dirty Sector的Cylinder
		//assert(N->blocks[dataBlock].sections[i].valid == 1);
		//0328開會後發現: 不可能有這回合merge找到的Guard Cylinder會先被寫進LB
		//0409: 有可能正在寫的req是在Guard上的
		if (N->blocks[dataBlock].sections[i].valid == 1){//有可能被LB的Write Back先搬進去了，這裡只搬剩下的
			//換GTC位置
			if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
				//不可能遇到guard track右邊是fRW而發動merge
				assert((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) != SLB->firstRWsection);
				//guard track右邊不是fRW，右移gt一格
				for (j = SLB->guardTrack1stSection; j < SLB->guardTrack1stSection + NC->CylinderSizeSector; j++){
					// 0x3fffffff;//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0xffffffffffff);//空sector
					assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeSector) % (SLB->Partial_Section + 1);
				for (j = SLB->guardTrack1stSection; j < SLB->guardTrack1stSection + NC->CylinderSizeSector; j++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0xffffffffffff;
				}
			}
			//LB紀錄
			assert(SLB->lastRWsection != SLB->guardTrack1stSection);
			assert(0 <= SLB->lastRWsection); assert(SLB->lastRWsection <= SLB->Partial_Section);
			assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo;
			N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
			//DB紀錄
			N->blocks[dataBlock].sections[i].valid = 0;
			N->blocks[dataBlock].sections[i].sector = SLB->lastRWsection;
			//換下一個section做寫入
			SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
			//增加存活sector
			N->l2pmapOvd.AA++;

			N->GCyl_sector++;
		}
	}
}
void DM_SMRwriteSector_PM(sim *simPtr, I64 offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	I64 SectorTableNo = 0;
	DWORD BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
	//(unsigned long long)ceil((double)lenSector / NC->trackSizeSector);
	N->content_rotate += lenSector;//寫多少sector轉多少sector
	N->WW_content_rotate += lenSector;//
#endif
	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += (unsigned long long)round((double)lenSector / NC->trackSizeSector * 10);
	assert(N->Req_RunTime >= 0);
	const I64 req_offsetSector = offsetSector, req_lenSector = lenSector;
	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizeSector; SectorTableNo = offsetSector;
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		/*if (req_offsetSector == 22566184){
		printf("check %lu %lu, %I64u\n", LBA, LPA, N->blocks[LBA].sections[LPA].valid);
		}*/
		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//DWORD n1 = 23000, n2 = 34500; /**(I64)SLB->SLBsection_num / 50331648*/ /** (I64)SLB->SLBsection_num / 50331648;*/
			//|| N->SlotTable_Act > n1 || (N->SlotTable_Act + N->SlotTable_InAct > n2)
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
				//找到一個適合CUTPM的DB
				N->CUTPM_SIG = 0;
				N->GET_FREE_LB_SECT = 0;//確保清出一個Free Cylinder
				while (N->GET_FREE_LB_SECT < NC->CylinderSizeSector){ FASTmergeRWSLBpart2_PM(simPtr); }
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0xffffffffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0xffffffffffff;// 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}assert(SLB->lastRWsection != SLB->guardTrack1stSection);

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(offsetSector <  NC->LsizePage);
		//assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		if (N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid != 0){ printf("%lu %lu %lu, %lu\n", SLB->firstRWsection, SLB->lastRWsection, SLB->guardTrack1stSection, NC->CylinderSizeSector); system("pause"); }

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo);
		section_no = SectorTableNo % NC->blockSizeSector;// getsection(simPtr, SectorTableNo);
		//assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		assert(0 <= block_no && block_no < NC->LsizeBlock);

		//if (block_no == NC->PsizeBlock - 1){
		//	assert(0 <= section_no && section_no <= SLB->Partial_Section);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
		//	N->blocks[block_no].sections[section_no].valid = 0;
		//	N->SLB_Hit++;
		//	N->cycleSLB_Hit++;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].InAct++;
		//		N->SlotTable_InAct++;
		//	}
		//}
		//else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
		//	assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
		//	assert(N->blocks[block_no].sections[section_no].valid == 1);
		//	assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
		//	N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
		//	N->SLB_Miss++;
		//	N->cycleSLB_Miss++;
		//	//增加存活sector
		//	N->l2pmapOvd.AA++;
		//	//有不同的section寫入
		//	//N->blocks[block_no].sections[section_no].unique = 1;
		//	if (SLOTTABLE_BAND_FirstSection == 1){
		//		SLOTTABLE_BAND_FirstSection = 0;
		//		N->SlotTable_Band[block_no].Act++;
		//		N->SlotTable_Act++;
		//	}
		//}//in Data Band

		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1; assert(N->blocks[block_no].sections[i].valid == 1);
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
			}
			N->DBalloc[block_no] = 1;
		}

		assert(N->blocks[block_no].sections[section_no].valid == 0 || N->blocks[block_no].sections[section_no].valid == 1);
		if (N->blocks[block_no].sections[section_no].valid == 0){
			DWORD LBsector = N->blocks[block_no].sections[section_no].sector;
			assert(0 <= LBsector); assert(LBsector <= SLB->Partial_Section);
			assert(N->blocks[NC->LsizeBlock].sections[LBsector].valid == 1);
			N->blocks[NC->LsizeBlock].sections[LBsector].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else{
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
		}

		assert(N->blocks[block_no].sections[section_no].valid == 0);
		N->blocks[block_no].sections[section_no].sector = SLB->lastRWsection;
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		//標示為剛寫過，有這種section的DB才能被merge
		//N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}

	//if (req_offsetSector / NC->blockSizeSector == 172){
	//	printf("w: %lu %lu\n", req_offsetSector % NC->blockSizeSector, (req_offsetSector + req_lenSector - 1) % NC->blockSizeSector);
	//	//日志显示(文件名：行号}+日期  
	//	//LOG(); ERROR(); 
	//	assert_printf(0, "%lu %lu\n", req_offsetSector % NC->blockSizeSector, (req_offsetSector + req_lenSector - 1) % NC->blockSizeSector);
	//	//system("pause");
	//}

	//處理寫入LB的Cylinder
	if (N->CUTPM_SIG == 1){//0238開會: 這裡的成本要加到當前merge裡，solid time計算
		N->CUTPM_SIG = 2;//做normal merge
		N->GET_FREE_LB_SECT = 0;//要一個空Cylinder
		//N->Req_RunTime = 0;//算時間
		while (N->GET_FREE_LB_SECT < NC->CylinderSizeSector){ FASTmergeRWSLBpart2_PM(simPtr); }
		GuardCyl2SMR(simPtr);//GuardCyl裝入LB
		//N->Finish_Time += N->Req_RunTime;//算時間
		N->CUTPM_SIG = 0;//原態
		//統計
		N->GET_FREE_LB_SECT_SUM += N->GET_FREE_LB_SECT;
		N->GET_FREE_LB_SECT_cnt++;
	}

	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	assert(FirstSection == 0); assert(N->Req_RunTime >= 5);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	assert(N->Finish_Time - N->Arr_Time >= 5);
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
#endif

#ifdef Simulation_NSLB
void FASTwriteSectorNSLBpart23_3_RMW(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD section_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;

	DWORD go_OldSection_NSLBtrack = 0;
	DWORD curSection = offsetSector / NC->Section;
	DWORD cur_Section_Update = 0;

	DWORD goRMW = 0;
	DWORD oldSection = -1;
	DWORD goDB = 0;
#ifdef Time_Overhead
	N->content_rotate += ((double)lenSector / NC->trackSizeSector);//寫多少sector轉多少sector
#endif
	//transfer time
	N->Req_RunTime += ((double)lenSector / NC->trackSizeSector * 10);

	N->cnt++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedSectionNum <= NSLB->NSLBsection_num);
		if (NSLB->writedSectionNum == NSLB->NSLBsection_num){
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			//FASTmergeRWNSLBpart23_3(simPtr);
			//FASTmergeRWNSLBpart23_3_SE(simPtr); //SE merge
			FASTmergeRWNSLBpart3_3(simPtr);
		}//當前空間已寫滿
		assert(NSLB->writedSectionNum < NSLB->NSLBsection_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//一般寫入流程
		//新寫入sector的DB更新BPLRU時間
		LBA = offsetSector / NC->blockSizeSector; assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector; assert(SectorTableNo % NC->Section == 0);
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;
		//確認該sector的L2P mapping table正確
		block_no = getblock(simPtr, SectorTableNo);
		section_no = getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= section_no && section_no < NC->blockSizeSection);
		assert(SectorTableNo / NC->blockSizeSector == block_no); assert(SectorTableNo / NC->Section % NC->blockSizeSection == section_no);
		//新寫入或第二次以上寫入的情況分別處理
		assert(N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 2);
		if (N->blocks[block_no].sections[section_no].valid == 2){//第二次以上寫入
			//NSLB舊page找尋最佳寫入位置
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
			oldNSLBfreeTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
			//把舊section寫到cur track(preNSLBfreeTrack)
			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			//	}else{
			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
			//		for (i = 0; i < N->NSLB_tracks; i++){
			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
			//				NSLBfreeTrack = i;
			//				mini_TKdist = tmp_TKdist;
			//			}
			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//	//歸還舊NSLBremainSectorTable
			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
			//	//更新NSLB page位置
			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
			//	//更新NSLBremainSectorTable資訊
			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//}

			//寫入量統計
			NSLB->pageRead += NC->Section; NSLB->pageWrite += NC->Section;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count += NC->Section;
			N->NSLB_Hit++;
			N->cycleNSLB_Hit++;
#ifdef Time_Overhead
			//Seek_Seagate_C15k5(simPtr, (NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizePage), 0);
			//test(simPtr, NC->LsizeSector + (oldNSLBfreeTrack + (oldNSLBfreeTrack / NC->CylinderSizeTrack)*NC->CylinderSizeTrack)*NC->trackSizeSector, 0);
			test(simPtr, NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector, 0);
#ifdef Simulation_NSLB
			if (N->Cylinder_Change == 1){
				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			}
#endif
#endif
		}
		else if (N->blocks[block_no].sections[section_no].valid == 1){//新寫入
			//寫入量統計
			NSLB->pageRead += NC->Section; NSLB->pageWrite += NC->Section; NSLB->writedSectionNum++;
			NSLB->write_count += NC->Section;
			N->NSLB_Miss++;
			N->cycleNSLB_Miss++;
			//找尋可以寫入的NSLBremainSector
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			if (N->NSLBremainSectionTable[preNSLBfreeTrack]>0){
				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			}
			else{
				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
				for (i = 0; i < N->NSLB_tracks; i++){
					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
					if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist <= mini_TKdist){
						NSLBfreeTrack = i;
						mini_TKdist = tmp_TKdist;
					}
				}assert(NSLBfreeTrack != preNSLBfreeTrack);
			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].sections[section_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			N->blocks[block_no].sections[section_no].sector = NSLBfreeTrack*NC->trackSizeSector + (N->NSLBremainSectionTable[NSLBfreeTrack] - 1)*NC->Section;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].sections[section_no].sector && N->blocks[block_no].sections[section_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			N->NSLBremainSectionTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectionTable[NSLBfreeTrack] && N->NSLBremainSectionTable[NSLBfreeTrack] < NC->trackSizeSection);
			if (N->NSLBremainSectionTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//增加存活section
			N->l2pmapOvd.AA++;

#ifdef Time_Overhead
			//Seek_Seagate_C15k5(simPtr, (NC->LsizeSector + NSLBfreeTrack*NC->trackSizePage), 0);
			//test(simPtr, (NC->LsizeSector + NSLBfreeTrack*NC->trackSizePage), 0);
			test(simPtr, NC->LsizeSector + NSLBfreeTrack*NC->trackSizeSector, 0);
#ifdef Simulation_NSLB
			if (N->Cylinder_Change == 1){
				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			}
#endif
#endif
		}
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}


	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
void DM_NSLBwriteSector(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD section_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;

	/*DWORD go_OldSection_NSLBtrack = 0;
	DWORD curSection = offsetSector / NC->Section;
	DWORD cur_Section_Update = 0;
	DWORD goRMW = 0;
	DWORD oldSection = -1;
	DWORD goDB = 0;*/
#ifdef Time_Overhead
	DWORD ReadTask_len = 0, WriteTask_len = 0;
	DWORD req_len = lenSector;
	DWORD *ReadTask = (DWORD*)calloc(req_len, sizeof(DWORD));
	DWORD *WriteTask = (DWORD*)calloc(req_len, sizeof(DWORD));
#endif
	N->cnt++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedSectionNum <= NSLB->NSLBsection_num);
		if (NSLB->writedSectionNum == NSLB->NSLBsection_num){//當前空間已寫滿
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			DM_NSLBmerge(simPtr);
		}

		//保證有free sector做寫入
		assert(NSLB->writedSectionNum < NSLB->NSLBsection_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//設定位置變數
		assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector; assert(NC->Section == 1);
		LBA = SectorTableNo / NC->blockSizeSector; assert(0 <= LBA && LBA < NC->LsizeBlock);
		//確認該sector的L2P mapping table正確
		block_no = getblock(simPtr, SectorTableNo); assert(block_no == LBA);
		section_no = getsection(simPtr, SectorTableNo); assert(0 <= section_no && section_no < NC->blockSizeSection);
		//確認此section不是新寫入就是第二次以上寫入
		assert(N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 2);

		//更新該band寫入的時間, GC會挑LRU時間最小的(最老)band做NSLB寫回
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;

		//正式寫入
		if (N->blocks[block_no].sections[section_no].valid == 2){//第二次以上寫入
			//NSLB舊page找尋最佳寫入位置
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
			oldNSLBfreeTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
			//把舊section寫到cur track(preNSLBfreeTrack)
			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			//	}else{
			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
			//		for (i = 0; i < N->NSLB_tracks; i++){
			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
			//				NSLBfreeTrack = i;
			//				mini_TKdist = tmp_TKdist;
			//			}
			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//	//歸還舊NSLBremainSectorTable
			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
			//	//更新NSLB page位置
			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
			//	//更新NSLBremainSectorTable資訊
			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//}

			//寫入量統計
			NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count++;
			N->NSLB_Hit++;
			N->cycleNSLB_Hit++;
			//#ifdef Time_Overhead
			//			test(simPtr, NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector, 0);
			//#ifdef Simulation_NSLB
			//			if (N->Cylinder_Change == 1){
			//				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			//			}
			//#endif
			//#endif
			WriteTask[WriteTask_len] = NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		else if (N->blocks[block_no].sections[section_no].valid == 1){//新寫入
			//寫入量統計
			NSLB->pageRead++;
			NSLB->pageWrite++;
			NSLB->writedSectionNum++;
			NSLB->write_count++;
			N->NSLB_Miss++;
			N->cycleNSLB_Miss++;
			//找尋可以寫入的NSLBremainSector
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			if (N->NSLBremainSectionTable[preNSLBfreeTrack]>0){
				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			}
			else{
				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
				for (i = 0; i < N->NSLB_tracks; i++){
					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
					if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist <= mini_TKdist){
						NSLBfreeTrack = i;
						mini_TKdist = tmp_TKdist;
					}
				}assert(NSLBfreeTrack != preNSLBfreeTrack);
			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].sections[section_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			N->blocks[block_no].sections[section_no].sector = NSLBfreeTrack*NC->trackSizeSector + (N->NSLBremainSectionTable[NSLBfreeTrack] - 1)*NC->Section;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].sections[section_no].sector && N->blocks[block_no].sections[section_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			N->NSLBremainSectionTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectionTable[NSLBfreeTrack] && N->NSLBremainSectionTable[NSLBfreeTrack] < NC->trackSizeSection);
			if (N->NSLBremainSectionTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//增加存活section
			N->l2pmapOvd.AA++;
			//#ifdef Time_Overhead
			//			test(simPtr, NC->LsizeSector + NSLBfreeTrack*NC->trackSizeSector, 0);
			//#ifdef Simulation_NSLB
			//			if (N->Cylinder_Change == 1){
			//				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			//			}
			//#endif
			//#endif
			ReadTask[ReadTask_len] = SectorTableNo;
			ReadTask_len++; assert(ReadTask_len <= req_len);
			WriteTask[WriteTask_len] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
#ifdef Time_Overhead
	//transfer time
	qsort(ReadTask, ReadTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 1);
	for (i = 1; i < ReadTask_len; i++){
		assert(ReadTask[i] < NC->LsizeSector);
		test(simPtr, ReadTask[i], 0);
		if (ReadTask[i - 1] / NC->trackSizeSector != ReadTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 1); }
	}
	qsort(WriteTask, WriteTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 0);
	for (i = 1; i < WriteTask_len; i++){
		assert(WriteTask[i] >= NC->LsizeSector);
		test(simPtr, WriteTask[i], 0);
		if (WriteTask[i - 1] / NC->trackSizeSector != WriteTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }
	}
	N->Req_RunTime += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);
	N->content_rotate += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);//寫多少sector轉多少sector
	free(ReadTask);
	free(WriteTask);
#endif
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

}
void DM_NSLBwriteSector2(sim *simPtr, DWORD offsetSector, DWORD lenSector)//找剩下最多free sector的NSLB Track做寫入 目前最有效
{
	DWORD LBA, LPA, i, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD section_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;
#ifdef Time_Overhead
	DWORD ReadTask_len = 0, WriteTask_len = 0;
	DWORD req_len = lenSector;
	DWORD *ReadTask = (DWORD*)calloc(req_len, sizeof(DWORD));
	DWORD *WriteTask = (DWORD*)calloc(req_len, sizeof(DWORD));
#endif
	N->cnt++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedSectionNum <= NSLB->NSLBsection_num);
		if (NSLB->writedSectionNum == NSLB->NSLBsection_num){//當前空間已寫滿
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			DM_NSLBmerge2(simPtr);
		}

		//保證有free sector做寫入
		assert(NSLB->writedSectionNum < NSLB->NSLBsection_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//設定位置變數
		assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector; assert(NC->Section == 1);
		LBA = SectorTableNo / NC->blockSizeSector; assert(0 <= LBA && LBA < NC->LsizeBlock);
		//確認該sector的L2P mapping table正確
		//block_no = getblock(simPtr, SectorTableNo); assert(block_no == LBA);
		//section_no = getsection(simPtr, SectorTableNo); assert(0 <= section_no && section_no < NC->blockSizeSection);
		block_no = LBA; section_no = SectorTableNo % NC->blockSizeSector;
		//確認此section不是新寫入就是第二次以上寫入
		assert(N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 2);

		//更新該band寫入的時間, GC會挑LRU時間最小的(最老)band做NSLB寫回
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;

		//正式寫入
		if (N->blocks[block_no].sections[section_no].valid == 2){//第二次以上寫入
			//確認舊NSLB TRack的位置是合法的
			oldNSLBfreeTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizePage;
			assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);

			//寫入量統計
			NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count++;
			N->NSLB_Hit++;
			N->cycleNSLB_Hit++;
			//
			WriteTask[WriteTask_len] = NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		else if (N->blocks[block_no].sections[section_no].valid == 1){//新寫入
#ifdef NSLB_LocalUse
			//找到配置給此DB的NSLB Track
			NSLBfreeTrack = N->DB2curNSLB_TK[block_no];
			assert(NSLBfreeTrack == 2 * N->NSLB_tracks || (0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks));
			//如果沒配過or用盡, 配新的
			if (NSLBfreeTrack == 2 * N->NSLB_tracks || (NSLBfreeTrack < N->NSLB_tracks && N->NSLBremainSectionTable[NSLBfreeTrack] == 0)){
				DWORD TK_freeSector = 0;
				for (i = 0; i < N->NSLB_tracks; i++){
					if (N->NSLBremainSectionTable[i] > TK_freeSector){
						NSLBfreeTrack = i;
						//if (N->NSLBremainSectionTable[i] == NC->trackSizeSector){ break; }
					}
				}
				assert(0 <= NSLBfreeTrack); assert(NSLBfreeTrack < N->NSLB_tracks);
				assert(N->NSLBremainSectionTable[NSLBfreeTrack] > 0);
				N->DB2curNSLB_TK[block_no] = NSLBfreeTrack;
			}
			assert(0 <= NSLBfreeTrack); assert(NSLBfreeTrack < N->NSLB_tracks);
			assert(N->NSLBremainSectionTable[NSLBfreeTrack] > 0);
			//寫到新配的NSLB Track上
#endif

			//寫入量統計
			NSLB->pageRead++;
			NSLB->pageWrite++;
			NSLB->writedSectionNum++;
			NSLB->write_count++;
			N->NSLB_Miss++;
			N->cycleNSLB_Miss++;

			////找尋可以寫入的NSLBremainSector
			//NSLBfreeTrack = -1;
			//preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			//assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			//if (N->NSLBremainSectionTable[preNSLBfreeTrack]>0){
			//	NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			//}
			//else{
			//	mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
			//	for (i = 0; i < N->NSLB_tracks; i++){
			//		tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
			//		if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist <= mini_TKdist){
			//			NSLBfreeTrack = i;
			//			mini_TKdist = tmp_TKdist;
			//		}
			//	}assert(NSLBfreeTrack != preNSLBfreeTrack);
			//}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);

			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].sections[section_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			N->blocks[block_no].sections[section_no].sector = NSLBfreeTrack*NC->trackSizeSector + (N->NSLBremainSectionTable[NSLBfreeTrack] - 1)*NC->Section;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].sections[section_no].sector && N->blocks[block_no].sections[section_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			N->NSLBremainSectionTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectionTable[NSLBfreeTrack] && N->NSLBremainSectionTable[NSLBfreeTrack] < NC->trackSizeSection);
			if (N->NSLBremainSectionTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//增加存活section
			N->l2pmapOvd.AA++;
			//
			ReadTask[ReadTask_len] = SectorTableNo;
			ReadTask_len++; assert(ReadTask_len <= req_len);
			WriteTask[WriteTask_len] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
#ifdef Time_Overhead
	//transfer time
	qsort(ReadTask, ReadTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 1);
	for (i = 1; i < ReadTask_len; i++){
		assert(ReadTask[i] < NC->LsizeSector);
		test(simPtr, ReadTask[i], 0);
		if (ReadTask[i - 1] / NC->trackSizeSector != ReadTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 1); }
	}
	qsort(WriteTask, WriteTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 0);
	for (i = 1; i < WriteTask_len; i++){
		assert(WriteTask[i] >= NC->LsizeSector);
		test(simPtr, WriteTask[i], 0);
		if (WriteTask[i - 1] / NC->trackSizeSector != WriteTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }
	}
	N->Req_RunTime += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);
	N->content_rotate += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);//寫多少sector轉多少sector
	free(ReadTask);
	free(WriteTask);
#endif
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

}
void DM_NSLBwriteSector3_ExcluNSLBTK(sim *simPtr, DWORD offsetSector, DWORD lenSector)//每個DB盡量配到一個全新NSLB Track
{
	DWORD LBA, LPA, i, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD section_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;
#ifdef Time_Overhead
	DWORD ReadTask_len = 0, WriteTask_len = 0;
	DWORD req_len = lenSector;
	DWORD *ReadTask = (DWORD*)calloc(req_len, sizeof(DWORD));
	DWORD *WriteTask = (DWORD*)calloc(req_len, sizeof(DWORD));
#endif
	N->cnt++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedSectionNum <= NSLB->NSLBsection_num);
		if (NSLB->writedSectionNum == NSLB->NSLBsection_num){//當前空間已寫滿
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			DM_NSLBmerge3_ExcluNSLBTK(simPtr);
		}

		//保證有free sector做寫入
		assert(NSLB->writedSectionNum < NSLB->NSLBsection_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//設定位置變數
		assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector; assert(NC->Section == 1);
		LBA = SectorTableNo / NC->blockSizeSector; assert(0 <= LBA && LBA < NC->LsizeBlock);
		//確認該sector的L2P mapping table正確
		//block_no = getblock(simPtr, SectorTableNo); assert(block_no == LBA);
		//section_no = getsection(simPtr, SectorTableNo); assert(0 <= section_no && section_no < NC->blockSizeSection);
		block_no = LBA; section_no = SectorTableNo % NC->blockSizeSector;
		//確認此section不是新寫入就是第二次以上寫入
		assert(N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 2);

		//更新該band寫入的時間, GC會挑LRU時間最小的(最老)band做NSLB寫回
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;

		//正式寫入
		if (N->blocks[block_no].sections[section_no].valid == 2){//第二次以上寫入
			//確認舊NSLB TRack的位置是合法的
			oldNSLBfreeTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizePage;
			assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);

			//寫入量統計
			NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count++;
			N->NSLB_Hit++;
			N->cycleNSLB_Hit++;
			//
			WriteTask[WriteTask_len] = NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		else if (N->blocks[block_no].sections[section_no].valid == 1){//新寫入
#ifdef NSLB_LocalUse
			//找到配置給此DB的NSLB Track
			NSLBfreeTrack = N->DB2curNSLB_TK[block_no];
			assert(NSLBfreeTrack == 2 * N->NSLB_tracks || (0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks));
			//如果沒配過or用盡, 配新的
			if (NSLBfreeTrack == 2 * N->NSLB_tracks || (NSLBfreeTrack < N->NSLB_tracks && N->NSLBremainSectionTable[NSLBfreeTrack] == 0)){
				DWORD TK_freeSector = 0;
				NSLBfreeTrack = 2 * N->NSLB_tracks;
				for (i = 0; i < N->NSLB_tracks; i++){
					if (N->NSLBTK_Share[i] == 0){
						assert(N->NSLBremainSectionTable[i] == NC->trackSizeSector);
						NSLBfreeTrack = i;
						//
						assert(N->NSLBTK_Share[i] == 0);
						N->ExcluNSLBTK[i] = (DWORD*)calloc(N->NSLBTK_Share[i] + 1, sizeof(DWORD));
						N->ExcluNSLBTK[i][0] = block_no;
						N->NSLBTK_Share[i]++;
						break;
					}
				}
				if (NSLBfreeTrack == 2 * N->NSLB_tracks){
					mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
					for (i = 0; i < N->NSLB_tracks; i++){
						tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
						if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist <= mini_TKdist){
							NSLBfreeTrack = i;
							mini_TKdist = tmp_TKdist;
						}
					}

					/*for (i = 0; i < N->NSLB_tracks; i++){
					if (N->NSLBremainSectionTable[i] > TK_freeSector){
					NSLBfreeTrack = i;
					}
					}*/
					assert(0 <= NSLBfreeTrack); assert(NSLBfreeTrack < N->NSLB_tracks);
					assert(N->NSLBremainSectionTable[NSLBfreeTrack] > 0);

					assert(N->NSLBTK_Share[NSLBfreeTrack] > 0);
					DWORD *t_ExcluNSLBTK_DB = (DWORD*)calloc(N->NSLBTK_Share[NSLBfreeTrack] + 1, sizeof(DWORD));
					DWORD t_NSLBTK_Share = N->NSLBTK_Share[NSLBfreeTrack];
					for (i = 0; i < t_NSLBTK_Share; i++){ t_ExcluNSLBTK_DB[i] = N->ExcluNSLBTK[NSLBfreeTrack][i]; }
					t_ExcluNSLBTK_DB[t_NSLBTK_Share] = block_no;
					free(N->ExcluNSLBTK[NSLBfreeTrack]);
					N->ExcluNSLBTK[NSLBfreeTrack] = t_ExcluNSLBTK_DB;
					N->NSLBTK_Share[NSLBfreeTrack]++;
				}
				assert(0 <= NSLBfreeTrack); assert(NSLBfreeTrack < N->NSLB_tracks);
				assert(N->NSLBremainSectionTable[NSLBfreeTrack] > 0);
				N->DB2curNSLB_TK[block_no] = NSLBfreeTrack;
			}
			assert(0 <= NSLBfreeTrack); assert(NSLBfreeTrack < N->NSLB_tracks);
			assert(N->NSLBremainSectionTable[NSLBfreeTrack] > 0);
			//寫到新配的NSLB Track上
#endif

			//寫入量統計
			NSLB->pageRead++;
			NSLB->pageWrite++;
			NSLB->writedSectionNum++;
			NSLB->write_count++;
			N->NSLB_Miss++;
			N->cycleNSLB_Miss++;

			////找尋可以寫入的NSLBremainSector
			//NSLBfreeTrack = -1;
			//preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			//assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			//if (N->NSLBremainSectionTable[preNSLBfreeTrack]>0){
			//	NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			//}
			//else{
			//	mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
			//	for (i = 0; i < N->NSLB_tracks; i++){
			//		tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
			//		if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist <= mini_TKdist){
			//			NSLBfreeTrack = i;
			//			mini_TKdist = tmp_TKdist;
			//		}
			//	}assert(NSLBfreeTrack != preNSLBfreeTrack);
			//}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);

			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].sections[section_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			N->blocks[block_no].sections[section_no].sector = NSLBfreeTrack*NC->trackSizeSector + (N->NSLBremainSectionTable[NSLBfreeTrack] - 1)*NC->Section;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].sections[section_no].sector && N->blocks[block_no].sections[section_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			N->NSLBremainSectionTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectionTable[NSLBfreeTrack] && N->NSLBremainSectionTable[NSLBfreeTrack] < NC->trackSizeSection);
			if (N->NSLBremainSectionTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//增加存活section
			N->l2pmapOvd.AA++;
			//
			ReadTask[ReadTask_len] = SectorTableNo;
			ReadTask_len++; assert(ReadTask_len <= req_len);
			WriteTask[WriteTask_len] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
#ifdef Time_Overhead
	//transfer time
	qsort(ReadTask, ReadTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 1);
	for (i = 1; i < ReadTask_len; i++){
		assert(ReadTask[i] < NC->LsizeSector);
		test(simPtr, ReadTask[i], 0);
		if (ReadTask[i - 1] / NC->trackSizeSector != ReadTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 1); }
	}
	qsort(WriteTask, WriteTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 0);
	for (i = 1; i < WriteTask_len; i++){
		assert(WriteTask[i] >= NC->LsizeSector);
		test(simPtr, WriteTask[i], 0);
		if (WriteTask[i - 1] / NC->trackSizeSector != WriteTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }
	}
	N->Req_RunTime += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);
	N->content_rotate += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);//寫多少sector轉多少sector
	free(ReadTask);
	free(WriteTask);
#endif
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

}
void FASTwriteSectorNSLBRAM2_RMW_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//section - group out
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	//試驗
	DWORD LRUmove = 0;
#ifdef Time_Overhead
	DWORD SectorTableNo = 0, block_no = 0, section_no = 0;
#endif
	DWORD *tmp_NSLB_RAM = NULL;
	DWORD tmp_NSLB_RAM_request = 0;
	const DWORD start_section = offsetSector / NC->Section;
	const DWORD end_section = (offsetSector + lenSector - 1) / NC->Section;
	const DWORD section_len = end_section - start_section + 1;
	DWORD tmp_start = offsetSector / NC->Section;

	DWORD LRU_band = 0, old_NSLB_RAM_request = 0;
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section);
	assert(section_len >= 1);

	if (N->NSLB_RAM_request + section_len > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
		while (N->NSLB_RAM_request + section_len > N->NSLB_RAM_SIZE){
			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
			LRU_band = N->NSLB_RAM[0] * NC->Section / NC->blockSizeSector;
			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
			for (i = 0; i < N->NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] * NC->Section / NC->blockSizeSector == LRU_band){
					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
					N->NSLB_RAM[i] = -1;//原RAM失效
				}
			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);

			//用section mapping寫入NSLB //assert(tmp_NSLB_RAM[0] == N->NSLB_RAM[0]);
			//qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
			//if (tmp_NSLB_RAM_request > 1){ for (i = 1; i < tmp_NSLB_RAM_request; i++){ assert(tmp_NSLB_RAM[i - 1] <= tmp_NSLB_RAM[i]); } }
			for (i = 0; i < tmp_NSLB_RAM_request; i++){
				assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
				assert(tmp_NSLB_RAM[i] != -1);
				SectorTableNo = tmp_NSLB_RAM[i] * NC->Section;
#ifdef Time_Overhead
				assert(0 <= N->WriteBuff_section);
				block_no = getblock(simPtr, SectorTableNo); section_no = getsection(simPtr, SectorTableNo);
				assert(N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 2);
				if (N->blocks[block_no].sections[section_no].valid == 1){ N->WriteBuff[N->WriteBuff_section] = SectorTableNo; N->RDB++; }//在DB
				else{ N->WriteBuff[N->WriteBuff_section] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector; N->RMW++; }//在NSLB
				N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
				if (N->WriteBuff_section == N->WriteBuff_SIZE){//  || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection + NC->Section) / NC->trackSizeSection
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
					//SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 0); N->WriteBuff_section = 0;
					N->WriteBuff_writecount++;//
					N->content_rotate += (N->WriteBuff_section / NC->trackSizeSection);
				}
#endif
				FASTwriteSectorNSLBpart23_3_RMW(simPtr, SectorTableNo, NC->Section); //tmp_NSLB_RAM做寫入
				N->RAM_WriteOut += NC->Section;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;
			}
			//更新NSLB_RAM_request資訊
			old_NSLB_RAM_request = N->NSLB_RAM_request;
			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
			//整理NSLB_RAM，清掉-1的資料
			j = 0;
			for (i = 0; i < old_NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
			}assert(j == N->NSLB_RAM_request);
			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
			free(tmp_NSLB_RAM);
		}assert(N->NSLB_RAM_request + section_len <= N->NSLB_RAM_SIZE);
	}
	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);

	//request切成section送進RAM
	tmp_start = start_section;
	while (tmp_start <= end_section){
		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
		if (N->RAM_Valid[tmp_start] == 0){// skip == 0
			N->RAM_Valid[tmp_start] = 1;
			N->NSLB_RAM[N->NSLB_RAM_request] = tmp_start;
			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			N->RAM_Miss++;
		}
		else{
			assert(N->RAM_Valid[tmp_start] == 1);
			N->RAM_Hit++;
		}
		tmp_start++;
		N->Host2RAM++;
	}
}
void FASTwriteSectorNSLBRAM2_WriteAllOut_RMW(sim *simPtr)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	DWORD i = 0, j = 0;
	DWORD start_section = 0, Section_Update = 0;
	//	//sector LRU + hit
	//	DWORD skip = 0;
	//	DWORD *tmp_NSLB_RAM = NULL;
	//	DWORD tmp_NSLB_RAM_request = 0;
	//	DWORD LRU_band = -1;
	//	DWORD old_NSLB_RAM_request = 0;
	//
	//	if (N->NSLB_RAM_request > 0){
	//#ifdef Time_Overhead
	//		DWORD WriteBuff_Size = 0;
	//		DWORD *WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//		DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
	//#endif
	//		while (N->NSLB_RAM_request > 0){
	//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
	//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
	//			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
	//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
	//			for (i = 0; i < N->NSLB_RAM_request; i++){
	//				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
	//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
	//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
	//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
	//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
	//					N->NSLB_RAM[i] = -1;//原RAM失效
	//				}
	//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
	//
	//			//tmp_NSLB_RAM用section mapping寫入NSLB
	//			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
	//			assert(tmp_NSLB_RAM[0] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
	//			start_section = tmp_NSLB_RAM[0] / N->SubTrack;
	//			/*Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[0] % N->SubTrack);*/
	//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
	//				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
	//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
	//					//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//正式寫前面section的
	//#ifdef Time_Overhead
	//					SectorTableNo = start_section*N->SubTrack;
	//					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
	//					assert(block_no == SectorTableNo / NC->blockSizeSector); assert(page_no == SectorTableNo % NC->blockSizeSector);
	//					if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
	//					else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector*2; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
	//#endif
	//					FASTwriteSectorNSLBpart23_3_RMW(simPtr, start_section*N->SubTrack, N->SubTrack);
	//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
	//					//Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);
	//				}
	//				else{ /*Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);*/ }
	//			}
	//			//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//寫最後一個section的
	//#ifdef Time_Overhead
	//			SectorTableNo = start_section*N->SubTrack;
	//			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
	//			assert(block_no == SectorTableNo / NC->blockSizeSector); assert(page_no == SectorTableNo % NC->blockSizeSector);
	//			if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
	//			else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector*2; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
	//
	//#endif
	//			FASTwriteSectorNSLBpart23_3_RMW(simPtr, start_section*N->SubTrack, N->SubTrack);
	//			//統計RAM Hit吸收的寫入
	//			N->RAM_WriteOut += tmp_NSLB_RAM_request;
	//
	//			//更新NSLB_RAM_request資訊
	//			old_NSLB_RAM_request = N->NSLB_RAM_request;
	//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
	//			//整理NSLB_RAM，清掉-1的資料
	//			j = 0;
	//			for (i = 0; i < old_NSLB_RAM_request; i++){
	//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
	//			}assert(j == N->NSLB_RAM_request);
	//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
	//			free(tmp_NSLB_RAM);
	//		}assert(N->NSLB_RAM_request == 0);
	//#ifdef Time_Overhead
	//		//assert(0 < WriteBuff_Size);  assert(WriteBuff_Size <= N->NSLB_RAM_SIZE);
	//		if (WriteBuff_Size>0){
	//			qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
	//			if (WriteBuff_Size > 1){ for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); } }
	//			Calc_TimeOverhead2(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 2);//電梯法回SLB讀資料
	//			for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
	//				if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){
	//					//seek
	//					Calc_TimeOverhead2(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 2);
	//				}
	//			}
	//		}
	//		free(WriteBuff);
	//#endif
	//	}

	if (N->NSLB_RAM_request > 0){
#ifdef Time_Overhead
		DWORD SectorTableNo = 0, block_no = 0, section_no = 0;
#endif
		assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE); //qsort(N->NSLB_RAM, N->NSLB_RAM_request, sizeof(DWORD), compare);
		for (i = 0; i < N->NSLB_RAM_request; i++){
			assert(N->NSLB_RAM[i] != -1); start_section = N->NSLB_RAM[i];
			SectorTableNo = start_section*NC->Section; block_no = getblock(simPtr, SectorTableNo); section_no = getsection(simPtr, SectorTableNo);
			assert(0 <= block_no && block_no < NC->LsizeBlock); assert(N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 2);
			if (N->blocks[block_no].sections[section_no].valid == 2){//在NSLB裡
#ifdef Time_Overhead
				assert(0 <= N->WriteBuff_section);
				N->WriteBuff[N->WriteBuff_section] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector; N->RMW++; //在NSLB
				N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
				if (N->WriteBuff_section == N->WriteBuff_SIZE){//  || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection + NC->Section) / NC->trackSizeSection
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
					//SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 0); N->WriteBuff_section = 0;
					N->WriteBuff_writecount++;//
					N->content_rotate += (N->WriteBuff_section / NC->trackSizeSection);
				}
#endif
				FASTwriteSectorNSLBpart23_3_RMW(simPtr, SectorTableNo, NC->Section);
				N->RAM_WriteOut += NC->Section;
				N->NSLB_RAM[i] = -1;
			}
		}
		for (i = 0; i < N->NSLB_RAM_request; i++){
			if (N->NSLB_RAM[i] != -1){
				SectorTableNo = N->NSLB_RAM[i] * NC->Section;
#ifdef Time_Overhead
				assert(0 <= N->WriteBuff_section);
				block_no = getblock(simPtr, SectorTableNo); section_no = getsection(simPtr, SectorTableNo);
				assert(N->blocks[block_no].sections[section_no].valid == 1);
				N->WriteBuff[N->WriteBuff_section] = SectorTableNo; N->RDB++; //在DB
				N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
				if (N->WriteBuff_section == N->WriteBuff_SIZE){
					//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
					//SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 0); N->WriteBuff_section = 0;
					N->WriteBuff_writecount++;//
					N->content_rotate += (N->WriteBuff_section / NC->trackSizeSection);
				}
#endif
				FASTwriteSectorNSLBpart23_3_RMW(simPtr, SectorTableNo, NC->Section);
				N->RAM_WriteOut += NC->Section;
			}
			N->NSLB_RAM[i] = -1;
		}
		N->NSLB_RAM_request = 0;
#ifdef Time_Overhead
		if (N->WriteBuff_section > 0){
			//SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 0); N->WriteBuff_section = 0;
			N->WriteBuff_writecount++;//
			N->content_rotate += (N->WriteBuff_section / NC->trackSizeSection);
		}
#endif
	}
	assert(N->NSLB_RAM_request == 0);
}

//void NSLB_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	//NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	//NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
//	//DWORD i = 0, j = 0;
//	//DWORD start_section = offsetSector / NC->Section, end_section = 0;
//	//DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//	//if ((offsetSector + lenSector) % NC->Section != 0){ end_section = (offsetSector + lenSector) / NC->Section + 1; }
//	//else{ end_section = (offsetSector + lenSector) / NC->Section; }
//	//assert(start_section <= end_section);
//	//while (start_section <= end_section){
//	//	//統計hit多少
//	//	if (N->RAM_Valid[start_section*NC->Section] == 0){ N->RAM_Miss++; }
//	//	else{ N->RAM_Hit++; }
//	//	FASTwriteSectorNSLBpart23_3_RMW(simPtr, start_section * NC->Section, NC->Section);
//	//	N->RAM_WriteOut += NC->Section;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;
//	//	start_section++;
//	//}
//
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	DWORD i = 0, j = 0;
//	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//
//	const DWORD start_section = offsetSector / NC->Section;
//	const DWORD end_section = (offsetSector + lenSector - 1) / NC->Section;
//	const DWORD section_len = end_section - start_section + 1;
//	DWORD tmp_start = offsetSector / NC->Section;
//	tmp_start = start_section;
//	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
//	assert(tmp_start == end_section);
//	assert(section_len >= 1);
//	tmp_start = start_section;
//
//	while (tmp_start <= end_section){
//		N->Host2RAM++;
//		SectorTableNo = tmp_start*NC->Section;
//#ifdef Time_Overhead
//		assert(0 <= N->WriteBuff_section);
//		block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//		assert(N->blocks[block_no].pages[page_no].valid == 1 || N->blocks[block_no].pages[page_no].valid == 2);
//		if (N->blocks[block_no].pages[page_no].valid == 1){ N->WriteBuff[N->WriteBuff_section] = SectorTableNo; N->RDB++; }//在DB
//		else{ N->WriteBuff[N->WriteBuff_section] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; N->RMW++; }//在NSLB
//		N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
//		if (N->WriteBuff_section == N->WriteBuff_SIZE){
//			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
//			SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 0); N->WriteBuff_section = 0;
//			N->WriteBuff_writecount++;//
//		}
//#endif
//		FASTwriteSectorNSLBpart23_3_RMW(simPtr, SectorTableNo, NC->Section);
//		tmp_start++;
//		N->RAM_WriteOut += NC->Section;
//	}
//	assert(tmp_start == end_section + 1);
//#ifdef Time_Overhead
//	if (N->WriteBuff_section > 0){
//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
//		SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 0); N->WriteBuff_section = 0;
//		N->WriteBuff_writecount++;//
//	}
//#endif
//}
void NSLB_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//write buff有問題
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	DWORD SectorTableNo = 0, block_no = 0, section_no = 0;

	const DWORD start_section = offsetSector / NC->Section;
	const DWORD end_section = (offsetSector + lenSector - 1) / NC->Section;
	const DWORD section_len = end_section - start_section + 1;
	DWORD tmp_start = offsetSector / NC->Section; assert(tmp_start >= 0);
	tmp_start = start_section;
	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
	assert(tmp_start == end_section);
	assert(section_len >= 1);
	tmp_start = start_section;
	assert(tmp_start*NC->Section <= offsetSector + lenSector - 1);

	FASTwriteSectorNSLBpart23_3_RMW(simPtr, tmp_start*NC->Section, section_len*NC->Section);

	//	while (tmp_start <= end_section){
	//		N->Host2RAM++;
	//		SectorTableNo = tmp_start*NC->Section;
	////#ifdef Time_Overhead
	////		assert(0 <= N->WriteBuff_section);
	////		block_no = getblock(simPtr, SectorTableNo); section_no = getsection(simPtr, SectorTableNo);
	////		assert(N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 2);
	////		if (N->blocks[block_no].sections[section_no].valid == 1){ N->WriteBuff[N->WriteBuff_section] = SectorTableNo; N->RDB++; }//在DB
	////		else{ N->WriteBuff[N->WriteBuff_section] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector; N->RMW++; }//在NSLB
	////		N->WriteBuff_section++; assert(N->WriteBuff_section <= N->WriteBuff_SIZE);
	////		if (N->WriteBuff_section == N->WriteBuff_SIZE){//  || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection + NC->Section) / NC->trackSizeSection
	////			//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
	////			SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 0); N->WriteBuff_section = 0;
	////			N->WriteBuff_writecount++;//
	////			N->content_rotate += (N->WriteBuff_section*NC->Section);
	////		}
	////#endif
	//		FASTwriteSectorNSLBpart23_3_RMW(simPtr, tmp_start*NC->Section, NC->Section);
	//		tmp_start++;
	//		N->RAM_WriteOut += NC->Section;
	//	}
	//#ifdef Time_Overhead
	//	if (N->WriteBuff_section > 0){
	//		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3 //N->WriteBuff_WriteOut += N->WriteBuff_section;
	//		SLB2_TrackRMWtime(simPtr, N->WriteBuff_SIZE, 0); N->WriteBuff_section = 0;
	//		N->WriteBuff_writecount++;//
	//		N->content_rotate += (N->WriteBuff_section*NC->Section);
	//	}
	//#endif
	//	assert(tmp_start == end_section + 1);

}
//------
void DM_NSLBwriteSector_NoDB(sim *simPtr, I64 offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, flag = 0, newBlock, dataBlock, BlockNo, PageNo;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD section_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;

	I64 SectorTableNo;

	/*DWORD go_OldSection_NSLBtrack = 0;
	DWORD curSection = offsetSector / NC->Section;
	DWORD cur_Section_Update = 0;
	DWORD goRMW = 0;
	DWORD oldSection = -1;
	DWORD goDB = 0;*/
#ifdef Time_Overhead
	DWORD ReadTask_len = 0, WriteTask_len = 0;
	DWORD req_len = lenSector;
	I64 *WriteTask = (I64*)calloc(req_len, sizeof(I64));
#endif
	N->cnt++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedSectionNum <= NSLB->NSLBsection_num);
		if (NSLB->writedSectionNum == NSLB->NSLBsection_num){//當前空間已寫滿
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			DM_NSLBmerge_NoDB(simPtr);
		}

		//保證有free sector做寫入
		assert(NSLB->writedSectionNum < NSLB->NSLBsection_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//設定位置變數
		assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector; assert(NC->Section == 1);
		LBA = SectorTableNo / NC->blockSizeSector; assert(0 <= LBA && LBA < NC->LsizeBlock);
		//確認該sector的L2P mapping table正確
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo); 
		assert(block_no == LBA);
		section_no = SectorTableNo % NC->blockSizeSector;//getsection(simPtr, SectorTableNo); 
		assert(0 <= section_no && section_no < NC->blockSizeSection);

		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
				assert(N->blocks[block_no].sections[i].valid == 1);
				assert(N->blocks[block_no].sections[i].sector == (I64)block_no*NC->blockSizeSector + i);
			}
			N->DBalloc[block_no] = 1;
		}
		assert(N->DBalloc[block_no] == 1);
		//確認此section不是新寫入就是第二次以上寫入
		assert(N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 2);
		//更新該band寫入的時間, GC會挑LRU時間最小的(最老)band做NSLB寫回
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;

		//正式寫入
		if (N->blocks[block_no].sections[section_no].valid == 2){//第二次以上寫入
			//NSLB舊page找尋最佳寫入位置
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
			oldNSLBfreeTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
			//把舊section寫到cur track(preNSLBfreeTrack)
			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			//	}else{
			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
			//		for (i = 0; i < N->NSLB_tracks; i++){
			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
			//				NSLBfreeTrack = i;
			//				mini_TKdist = tmp_TKdist;
			//			}
			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//	//歸還舊NSLBremainSectorTable
			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
			//	//更新NSLB page位置
			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
			//	//更新NSLBremainSectorTable資訊
			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//}

			//寫入量統計
			NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count++;
			N->NSLB_Hit++;
			N->cycleNSLB_Hit++;
			//#ifdef Time_Overhead
			//			test(simPtr, NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector, 0);
			//#ifdef Simulation_NSLB
			//			if (N->Cylinder_Change == 1){
			//				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			//			}
			//#endif
			//#endif
			WriteTask[WriteTask_len] = NC->LsizeSector + (I64)oldNSLBfreeTrack*NC->trackSizeSector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		else if (N->blocks[block_no].sections[section_no].valid == 1){//新寫入
			//增加存活sector
			N->l2pmapOvd.AA++;
			//寫入量統計
			NSLB->pageRead++;
			NSLB->pageWrite++;
			NSLB->writedSectionNum++;
			NSLB->write_count++;
			N->NSLB_Miss++;
			N->cycleNSLB_Miss++;
			//找尋可以寫入的NSLBremainSector
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			if (N->NSLBremainSectionTable[preNSLBfreeTrack]>0){
				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			}
			else{
				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
				for (i = 0; i < N->NSLB_tracks; i++){
					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
					if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist <= mini_TKdist){
						NSLBfreeTrack = i;
						mini_TKdist = tmp_TKdist;
					}
				}assert(NSLBfreeTrack != preNSLBfreeTrack);
			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].sections[section_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			N->blocks[block_no].sections[section_no].sector = NSLBfreeTrack*NC->trackSizeSector + (N->NSLBremainSectionTable[NSLBfreeTrack] - 1)*NC->Section;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].sections[section_no].sector && N->blocks[block_no].sections[section_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			N->NSLBremainSectionTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectionTable[NSLBfreeTrack] && N->NSLBremainSectionTable[NSLBfreeTrack] < NC->trackSizeSection);
			if (N->NSLBremainSectionTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//#ifdef Time_Overhead
			//			test(simPtr, NC->LsizeSector + NSLBfreeTrack*NC->trackSizeSector, 0);
			//#ifdef Simulation_NSLB
			//			if (N->Cylinder_Change == 1){
			//				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			//			}
			//#endif
			//#endif
			assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			//ReadTask[ReadTask_len] = SectorTableNo;
			//ReadTask_len++; assert(ReadTask_len <= req_len);
			WriteTask[WriteTask_len] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
#ifdef Time_Overhead
	//transfer time
	/*qsort(ReadTask, ReadTask_len, sizeof(I64), compare);
	Calc_RotateOverhead2(simPtr, 0, 1);
	for (i = 1; i < ReadTask_len; i++){
	assert(ReadTask[i] < NC->LsizeSector);
	test(simPtr, ReadTask[i], 0);
	if (ReadTask[i - 1] / NC->trackSizeSector != ReadTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 1); }
	}*/
	qsort(WriteTask, WriteTask_len, sizeof(I64), compare);
	Calc_RotateOverhead2(simPtr, 0, 0);
	for (i = 1; i < WriteTask_len; i++){
		assert(WriteTask[i] >= NC->LsizeSector);
		test(simPtr, WriteTask[i], 0);
		if (WriteTask[i - 1] / NC->trackSizeSector != WriteTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }
	}
	N->Req_RunTime += (unsigned long long)round((double)WriteTask_len / NC->trackSizeSector * 10);
	//(unsigned long long)ceil((double)WriteTask_len / NC->trackSizeSector);
	N->content_rotate += WriteTask_len;//寫多少sector轉多少sector
	free(WriteTask);
#endif
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime; assert(N->Req_RunTime >= 5);
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time; assert(N->Finish_Time - N->Arr_Time >= 5);
	assert(N->Finish_Time - N->Arr_Time - N->Req_RunTime >= 0);
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

}
void DM_NSLBwriteSector_NoDB_WriteCurr(sim *simPtr, I64 offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, flag = 0, newBlock, dataBlock, BlockNo, PageNo;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD section_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;

	I64 SectorTableNo;

	/*DWORD go_OldSection_NSLBtrack = 0;
	DWORD curSection = offsetSector / NC->Section;
	DWORD cur_Section_Update = 0;
	DWORD goRMW = 0;
	DWORD oldSection = -1;
	DWORD goDB = 0;*/
#ifdef Time_Overhead
	DWORD ReadTask_len = 0, WriteTask_len = 0;
	DWORD req_len = lenSector;
	I64 *ReadTask = (I64*)calloc(req_len, sizeof(I64));
	I64 *WriteTask = (I64*)calloc(req_len, sizeof(I64));
#endif
	N->cnt++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedSectionNum <= NSLB->NSLBsection_num);
		if (NSLB->writedSectionNum == NSLB->NSLBsection_num){//當前空間已寫滿
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			DM_NSLBmerge_NoDB(simPtr);
		}

		//保證有free sector做寫入
		assert(NSLB->writedSectionNum < NSLB->NSLBsection_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//設定位置變數
		assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector; assert(NC->Section == 1);
		LBA = SectorTableNo / NC->blockSizeSector; assert(0 <= LBA && LBA < NC->LsizeBlock);
		//確認該sector的L2P mapping table正確
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo); 
		assert(block_no == LBA);
		section_no = SectorTableNo % NC->blockSizeSector;//getsection(simPtr, SectorTableNo); 
		assert(0 <= section_no && section_no < NC->blockSizeSection);

		//給新DB配置sectors記憶體
		if (N->DBalloc[block_no] == 0){
			assert(N->blocks[block_no].sections == NULL);
			N->blocks[block_no].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
			assert(N->blocks[block_no].sections != NULL);
			for (i = 0; i < NC->blockSizeSector; i++){
				N->blocks[block_no].sections[i].valid = 1;
				N->blocks[block_no].sections[i].sector = (I64)block_no*NC->blockSizeSector + i;
				assert(N->blocks[block_no].sections[i].valid == 1);
				assert(N->blocks[block_no].sections[i].sector == (I64)block_no*NC->blockSizeSector + i);
			}
			N->DBalloc[block_no] = 1;
		}
		assert(N->DBalloc[block_no] == 1);
		//確認此section不是新寫入就是第二次以上寫入
		assert(N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 2);
		//更新該band寫入的時間, GC會挑LRU時間最小的(最老)band做NSLB寫回
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;

		//正式寫入
		if (N->blocks[block_no].sections[section_no].valid == 2){//第二次以上寫入
			//NSLB舊page找尋最佳寫入位置
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
			oldNSLBfreeTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
			//把舊section寫到cur track(preNSLBfreeTrack)
			if (oldNSLBfreeTrack != preNSLBfreeTrack){
				if (N->NSLBremainSectionTable[preNSLBfreeTrack]>0){
					NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
				}//else{
				//	mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
				//	for (i = 0; i < N->NSLB_tracks; i++){
				//		tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
				//		if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist < mini_TKdist){
				//			NSLBfreeTrack = i;
				//			mini_TKdist = tmp_TKdist;
				//		}
				//	}assert(NSLBfreeTrack != preNSLBfreeTrack);
				//}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
				////歸還舊NSLBremainSectorTable
				//if (N->NSLBremainSectionTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
				//N->NSLBremainSectionTable[oldNSLBfreeTrack]++;
				////更新NSLB page位置
				//N->blocks[block_no].sections[section_no].sector = NSLBfreeTrack * NC->trackSizePage;
				////更新NSLBremainSectorTable資訊
				//N->NSLBremainSectionTable[NSLBfreeTrack]--;
				//if (N->NSLBremainSectionTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			}

			//寫入量統計
			NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count++;
			N->NSLB_Hit++;
			N->cycleNSLB_Hit++;
			//#ifdef Time_Overhead
			//			test(simPtr, NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector, 0);
			//#ifdef Simulation_NSLB
			//			if (N->Cylinder_Change == 1){
			//				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			//			}
			//#endif
			//#endif
			WriteTask[WriteTask_len] = NC->LsizeSector + (I64)preNSLBfreeTrack*NC->trackSizeSector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		else if (N->blocks[block_no].sections[section_no].valid == 1){//新寫入
			//寫入量統計
			NSLB->pageRead++;
			NSLB->pageWrite++;
			NSLB->writedSectionNum++;
			NSLB->write_count++;
			N->NSLB_Miss++;
			N->cycleNSLB_Miss++;
			//找尋可以寫入的NSLBremainSector
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			if (N->NSLBremainSectionTable[preNSLBfreeTrack]>0){
				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			}
			else{
				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
				for (i = 0; i < N->NSLB_tracks; i++){
					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
					if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist <= mini_TKdist){
						NSLBfreeTrack = i;
						mini_TKdist = tmp_TKdist;
					}
				}assert(NSLBfreeTrack != preNSLBfreeTrack);
			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].sections[section_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			N->blocks[block_no].sections[section_no].sector = NSLBfreeTrack*NC->trackSizeSector + (N->NSLBremainSectionTable[NSLBfreeTrack] - 1)*NC->Section;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].sections[section_no].sector && N->blocks[block_no].sections[section_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			N->NSLBremainSectionTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectionTable[NSLBfreeTrack] && N->NSLBremainSectionTable[NSLBfreeTrack] < NC->trackSizeSection);
			if (N->NSLBremainSectionTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//增加存活section
			N->l2pmapOvd.AA++;
			//#ifdef Time_Overhead
			//			test(simPtr, NC->LsizeSector + NSLBfreeTrack*NC->trackSizeSector, 0);
			//#ifdef Simulation_NSLB
			//			if (N->Cylinder_Change == 1){
			//				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			//			}
			//#endif
			//#endif
			assert(0 <= SectorTableNo && SectorTableNo < NC->LsizeSector);
			ReadTask[ReadTask_len] = SectorTableNo;
			ReadTask_len++; assert(ReadTask_len <= req_len);
			WriteTask[WriteTask_len] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
#ifdef Time_Overhead
	//transfer time
	qsort(ReadTask, ReadTask_len, sizeof(I64), compare);
	Calc_RotateOverhead2(simPtr, 0, 1);
	for (i = 1; i < ReadTask_len; i++){
		assert(ReadTask[i] < NC->LsizeSector);
		test(simPtr, ReadTask[i], 0);
		if (ReadTask[i - 1] / NC->trackSizeSector != ReadTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 1); }
	}
	qsort(WriteTask, WriteTask_len, sizeof(I64), compare);
	Calc_RotateOverhead2(simPtr, 0, 0);
	for (i = 1; i < WriteTask_len; i++){
		assert(WriteTask[i] >= NC->LsizeSector);
		test(simPtr, WriteTask[i], 0);
		if (WriteTask[i - 1] / NC->trackSizeSector != WriteTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }
	}
	N->Req_RunTime += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);
	N->content_rotate += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);//寫多少sector轉多少sector
	free(ReadTask);
	free(WriteTask);
#endif
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

}
#endif

#ifdef Simulation_HLB
void DM_SLBwriteSector(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
	N->content_rotate += ((double)lenSector / NC->trackSizeSector);//寫多少sector轉多少sector
	N->WW_content_rotate += ((double)lenSector / NC->trackSizeSector);///
#endif

	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	//N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	//N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += ((double)lenSector / NC->trackSizeSector * 10);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(SectorTableNo < NC->LsizeSector);
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//if (SLB->lastRWsection + (NC->CylinderSizeTrack - 1)*NC->trackSizeSection == SLB->guardTrack1stSection){//當前空間已寫滿
		//	if ((SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
		//		assert(SLB->writedSectionNum == SLB->SLBsection_num);
		//		//printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		//		FASTmergeRWSLBpart2_Sr(simPtr);
		//		//FirstSection = 1;
		//	}
		//	else{//guard track右邊不是fRW，右移gt一格
		//		for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
		//			N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;
		//			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
		//		}
		//		SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
		//		for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
		//			N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
		//			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
		//		}
		//	}
		//}
		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//DWORD n1 = 23000, n2 = 34500; /**(I64)SLB->SLBsection_num / 50331648*/ /** (I64)SLB->SLBsection_num / 50331648;*/
			//|| N->SlotTable_Act > n1 || (N->SlotTable_Act + N->SlotTable_InAct > n2)
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge

				DM_SLBmerge(simPtr);
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0x3fffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

		////計算同SLB cyl上平均寫多少不同DB
		//N->b[SectorTableNo / NC->blockSizeSector]=1;
		//if (SLB->lastRWsection / (NC->CylinderSizeTrack*NC->trackSizeSection) != (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1) / (NC->CylinderSizeTrack*NC->trackSizeSection)){
		//	for (i = 0; i < NC->LsizeBlock; i++){ 
		//		if (N->b[i] == 1){ N->DBs++; }
		//		N->b[i] = 0;
		//	}
		//	N->write_cyls++; 
		//}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = getblock(simPtr, SectorTableNo);
		section_no = getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		if (block_no == NC->PsizeBlock - 1){
			assert(0 <= section_no && section_no <= SLB->Partial_Section);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
			N->blocks[block_no].sections[section_no].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			//有不同的section寫入
			//N->blocks[block_no].sections[section_no].unique = 1;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
		}//in Data Band

		//N->section_cumu[SectorTableNo / NC->Section]++;
		//更新L2P mapping table
		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
//void HLB_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector)//write buff有問題
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
//	DWORD i = 0, j = 0;
//	DWORD SectorTableNo = 0, block_no = 0, section_no = 0;
//	//轉成section len的request
//	const DWORD start_section = offsetSector / NC->Section;
//	const DWORD end_section = (offsetSector + lenSector - 1) / NC->Section;
//	const DWORD section_len = end_section - start_section + 1;
//	DWORD tmp_start = offsetSector / NC->Section; assert(tmp_start >= 0);
//	tmp_start = start_section;
//	while (tmp_start*NC->Section + NC->Section - 1 < offsetSector + lenSector - 1){ tmp_start++; }
//	assert(tmp_start == end_section);
//	assert(section_len >= 1);
//	tmp_start = start_section;
//	assert(tmp_start*NC->Section <= offsetSector + lenSector - 1);
//
//	//RMW寫入
//	//FASTwriteSectorSLBpart2(simPtr, tmp_start*NC->Section, section_len*NC->Section);
//	//Bit-Map寫入
//	//FASTwriteSectorSLBpart2_BitMap(simPtr, tmp_start*NC->Section, section_len*NC->Section);
//	//sector SLB
//	if (NC->Section == 1){
//		assert(tmp_start == offsetSector); assert(section_len == lenSector);
//	}
//	//FASTwriteSectorSLBpart2_Sr(simPtr, tmp_start*NC->Section, section_len*NC->Section);
//	simStat			*SS = &(simPtr->simStatObj);
//#ifdef Time_Overhead
//	if (SS->writeReq % 240 == 0){
//		test(simPtr, (I64)NC->LsizeSector/2, 0);
//	}
//#endif
//	DM_SLBwriteSector(simPtr, tmp_start*NC->Section, section_len*NC->Section);
//}

//
void DM_N2SLBw(sim *simPtr) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
	N->content_rotate += ((double)N->N2S_LBsector_len / NC->trackSizeSector);//寫多少sector轉多少sector
	N->WW_content_rotate += ((double)N->N2S_LBsector_len / NC->trackSizeSector);///
#endif

	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	//N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	//N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += ((double)N->N2S_LBsector_len / NC->trackSizeSector * 10);
	DWORD index = 0; assert(index <= N->N2S_LBsector_len);
	while (1) {
		LBA = N->N2S_LBsector[index] / NC->blockSizeSector; LPA = N->N2S_LBsector[index] % NC->blockSizePage; SectorTableNo = N->N2S_LBsector[index];
		//assert(SectorTableNo < NC->LsizeSector);
		if (SectorTableNo >= NC->LsizeSector){ printf("SectorTableNo %lu NC->LsizeSector %lu, index %lu, N->N2S_LBsector_len %lu\n", SectorTableNo, NC->LsizeSector, index, N->N2S_LBsector_len); system("pause"); }
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		//if (SLB->lastRWsection + (NC->CylinderSizeTrack - 1)*NC->trackSizeSection == SLB->guardTrack1stSection){//當前空間已寫滿
		//	if ((SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
		//		assert(SLB->writedSectionNum == SLB->SLBsection_num);
		//		//printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		//		FASTmergeRWSLBpart2_Sr(simPtr);
		//		//FirstSection = 1;
		//	}
		//	else{//guard track右邊不是fRW，右移gt一格
		//		for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
		//			N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;
		//			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
		//		}
		//		SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->trackSizeSection) % (SLB->Partial_Section + 1);
		//		for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->trackSizeSection; i++){
		//			N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
		//			N->blocks[NC->PsizeBlock - 1].sections[i].valid = 0;
		//		}
		//	}
		//}
		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//DWORD n1 = 23000, n2 = 34500; /**(I64)SLB->SLBsection_num / 50331648*/ /** (I64)SLB->SLBsection_num / 50331648;*/
			//|| N->SlotTable_Act > n1 || (N->SlotTable_Act + N->SlotTable_InAct > n2)
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge

				DM_SLBmerge(simPtr);
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0x3fffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

		////計算同SLB cyl上平均寫多少不同DB
		//N->b[SectorTableNo / NC->blockSizeSector]=1;
		//if (SLB->lastRWsection / (NC->CylinderSizeTrack*NC->trackSizeSection) != (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1) / (NC->CylinderSizeTrack*NC->trackSizeSection)){
		//	for (i = 0; i < NC->LsizeBlock; i++){ 
		//		if (N->b[i] == 1){ N->DBs++; }
		//		N->b[i] = 0;
		//	}
		//	N->write_cyls++; 
		//}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = getblock(simPtr, SectorTableNo);
		section_no = getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		if (block_no == NC->PsizeBlock - 1){
			assert(0 <= section_no && section_no <= SLB->Partial_Section);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
			N->blocks[block_no].sections[section_no].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			//有不同的section寫入
			//N->blocks[block_no].sections[section_no].unique = 1;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
		}//in Data Band

		//N->section_cumu[SectorTableNo / NC->Section]++;
		//更新L2P mapping table
		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		//offsetSector += NC->Section;
		//lenSector -= NC->Section;
		//if (lenSector <= 0){ break; }
		index++; assert(index <= N->N2S_LBsector_len);
		if (index == N->N2S_LBsector_len){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
void DM_NSLBwriteSector(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD section_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;

	/*DWORD go_OldSection_NSLBtrack = 0;
	DWORD curSection = offsetSector / NC->Section;
	DWORD cur_Section_Update = 0;
	DWORD goRMW = 0;
	DWORD oldSection = -1;
	DWORD goDB = 0;*/
#ifdef Time_Overhead
	DWORD ReadTask_len = 0, WriteTask_len = 0;
	DWORD req_len = lenSector;
	DWORD *ReadTask = (DWORD*)calloc(req_len, sizeof(DWORD));
	DWORD *WriteTask = (DWORD*)calloc(req_len, sizeof(DWORD));
#endif
	N->cnt++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedSectionNum <= NSLB->NSLBsection_num);
		if (NSLB->writedSectionNum == NSLB->NSLBsection_num){//當前空間已寫滿
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			DM_NSLBmerge(simPtr);
		}

		//保證有free sector做寫入
		assert(NSLB->writedSectionNum < NSLB->NSLBsection_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//設定位置變數
		assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector; assert(NC->Section == 1);
		LBA = SectorTableNo / NC->blockSizeSector; assert(0 <= LBA && LBA < NC->LsizeBlock);
		//確認該sector的L2P mapping table正確
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo); assert(block_no == LBA);
		section_no = SectorTableNo % NC->blockSizeSector; //getsection(simPtr, SectorTableNo); assert(0 <= section_no && section_no < NC->blockSizeSection);
		//確認此section不是新寫入就是第二次以上寫入
		assert(0 <= N->blocks[block_no].sections[section_no].valid && N->blocks[block_no].sections[section_no].valid <= 2);

		//更新該band寫入的時間, GC會挑LRU時間最小的(最老)band做NSLB寫回
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;

		//正式寫入
		if (N->blocks[block_no].sections[section_no].valid == 2){//第二次以上寫入
			//NSLB舊page找尋最佳寫入位置
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
			oldNSLBfreeTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
			//把舊section寫到cur track(preNSLBfreeTrack)
			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			//	}else{
			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
			//		for (i = 0; i < N->NSLB_tracks; i++){
			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
			//				NSLBfreeTrack = i;
			//				mini_TKdist = tmp_TKdist;
			//			}
			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//	//歸還舊NSLBremainSectorTable
			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
			//	//更新NSLB page位置
			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
			//	//更新NSLBremainSectorTable資訊
			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//}

			//寫入量統計
			NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count++;
			N->NSLB_Hit++;
			N->cycleNSLB_Hit++;
			//#ifdef Time_Overhead
			//			test(simPtr, NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector, 0);
			//#ifdef Simulation_NSLB
			//			if (N->Cylinder_Change == 1){
			//				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			//			}
			//#endif
			//#endif
			WriteTask[WriteTask_len] = NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		else if (N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 0){//新寫入
			if (N->blocks[block_no].sections[section_no].valid == 0){
				DWORD SLB_block_no = getblock(simPtr, SectorTableNo);
				DWORD SLB_section_no = getsection(simPtr, SectorTableNo);
				assert(SLB_block_no == NC->LsizeBlock);
				assert(0 <= SLB_section_no && SLB_section_no <= SLB->Partial_Section);
				assert(N->blocks[SLB_block_no].sections[SLB_section_no].valid == 1);
				assert(N->blocks[SLB_block_no].sections[SLB_section_no].sector == SectorTableNo);
				N->blocks[SLB_block_no].sections[SLB_section_no].valid = 0;

				setblock(simPtr, SectorTableNo, block_no);
				setsection(simPtr, SectorTableNo, section_no);

				N->blocks[block_no].sections[section_no].valid = 1;
				N->blocks[block_no].sections[section_no].sector = SectorTableNo;
			}
			//寫入量統計
			NSLB->pageRead++;
			NSLB->pageWrite++;
			NSLB->writedSectionNum++;
			NSLB->write_count++;
			N->NSLB_Miss++;
			N->cycleNSLB_Miss++;
			//找尋可以寫入的NSLBremainSector
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			if (N->NSLBremainSectionTable[preNSLBfreeTrack]>0){
				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			}
			else{
				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
				for (i = 0; i < N->NSLB_tracks; i++){
					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
					if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist <= mini_TKdist){
						NSLBfreeTrack = i;
						mini_TKdist = tmp_TKdist;
					}
				}assert(NSLBfreeTrack != preNSLBfreeTrack);
			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].sections[section_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			N->blocks[block_no].sections[section_no].sector = NSLBfreeTrack*NC->trackSizeSector + (N->NSLBremainSectionTable[NSLBfreeTrack] - 1)*NC->Section;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].sections[section_no].sector && N->blocks[block_no].sections[section_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			N->NSLBremainSectionTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectionTable[NSLBfreeTrack] && N->NSLBremainSectionTable[NSLBfreeTrack] < NC->trackSizeSection);
			if (N->NSLBremainSectionTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//增加存活section
			N->l2pmapOvd.AA++;
			//#ifdef Time_Overhead
			//			test(simPtr, NC->LsizeSector + NSLBfreeTrack*NC->trackSizeSector, 0);
			//#ifdef Simulation_NSLB
			//			if (N->Cylinder_Change == 1){
			//				N->Cylinder_Change = 0; Calc_RotateOverhead2(simPtr, 0, 0);//head找第一個要寫的sector
			//			}
			//#endif
			//#endif
			ReadTask[ReadTask_len] = SectorTableNo;
			ReadTask_len++; assert(ReadTask_len <= req_len);
			WriteTask[WriteTask_len] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
#ifdef Time_Overhead
	//transfer time
	qsort(ReadTask, ReadTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 1);
	for (i = 1; i < ReadTask_len; i++){
		assert(ReadTask[i] < NC->LsizeSector);
		test(simPtr, ReadTask[i], 0);
		if (ReadTask[i - 1] / NC->trackSizeSector != ReadTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 1); }
	}
	qsort(WriteTask, WriteTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 0);
	for (i = 1; i < WriteTask_len; i++){
		assert(WriteTask[i] >= NC->LsizeSector);
		test(simPtr, WriteTask[i], 0);
		if (WriteTask[i - 1] / NC->trackSizeSector != WriteTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }
	}
	N->Req_RunTime += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);
	N->content_rotate += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);//寫多少sector轉多少sector
	free(ReadTask);
	free(WriteTask);
#endif
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

}


void DM_SLBw2(sim *simPtr, DWORD offsetSector, DWORD lenSector) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
	N->content_rotate += ((double)lenSector / NC->trackSizeSector);//寫多少sector轉多少sector
	N->WW_content_rotate += ((double)lenSector / NC->trackSizeSector);///
#endif

	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	//N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	//N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += ((double)lenSector / NC->trackSizeSector * 10);

	while (1) {
		LBA = offsetSector / NC->blockSizeSector; LPA = offsetSector % NC->blockSizePage; SectorTableNo = offsetSector;
		assert(SectorTableNo < NC->LsizeSector);
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//DWORD n1 = 23000, n2 = 34500; /**(I64)SLB->SLBsection_num / 50331648*/ /** (I64)SLB->SLBsection_num / 50331648;*/
			//|| N->SlotTable_Act > n1 || (N->SlotTable_Act + N->SlotTable_InAct > n2)
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
				//printf("S ");
				DM_SLBmerge2(simPtr);
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0x3fffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = getblock(simPtr, SectorTableNo);
		section_no = getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		if (block_no == NC->PsizeBlock - 1){
			assert(0 <= section_no && section_no <= SLB->Partial_Section);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
			N->blocks[block_no].sections[section_no].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			if (N->blocks[block_no].sections[section_no].valid == 2){
				DWORD NSLBdirtyTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector;
				if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectionTable[NSLBdirtyTrack]++;
				NSLB->writedSectionNum--;

				N->blocks[block_no].sections[section_no].valid = 1;
				N->blocks[block_no].sections[section_no].sector = SectorTableNo;
			}
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			//有不同的section寫入
			//N->blocks[block_no].sections[section_no].unique = 1;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
		}//in Data Band

		//N->section_cumu[SectorTableNo / NC->Section]++;
		//更新L2P mapping table
		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
void DM_N2SLBw2(sim *simPtr) { // Handle a write of sectors to the MLC flash
	DWORD LBA, LPA, j = 0, k, flag = 0, newBlock, dataBlock;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD i = 0;
	DWORD SectorTableNo = 0, BlockNo = 0, PageNo = 0, TrackIndex = 0;
	DWORD block_no = 0, section_no = 0;
	DWORD curSectionRMW = 0, curSection = -1;
	DWORD oldData_PTrack = 0;
	DWORD goRMW = 0, OneTimeRotation = 0;
	DWORD goDB = 0;
#ifdef Time_Overhead
	DWORD FirstSection = 1, SLOTTABLE_BAND_FirstSection = 1;
	N->content_rotate += ((double)N->N2S_LBsector_len / NC->trackSizeSector);//寫多少sector轉多少sector
	N->WW_content_rotate += ((double)N->N2S_LBsector_len / NC->trackSizeSector);///
#endif

	//
	DWORD t_time = N->time;
	N->time++; assert(t_time < N->time);
	//N->b_time[offsetSector / NC->blockSizeSector] = N->time;
	//N->b[offsetSector / NC->blockSizeSector]++;
	//transfer time
	N->Req_RunTime += ((double)N->N2S_LBsector_len / NC->trackSizeSector * 10);
	DWORD index = 0; assert(index <= N->N2S_LBsector_len);
	while (1) {
		LBA = N->N2S_LBsector[index] / NC->blockSizeSector; LPA = N->N2S_LBsector[index] % NC->blockSizePage; SectorTableNo = N->N2S_LBsector[index];
		//assert(SectorTableNo < NC->LsizeSector);
		if (SectorTableNo >= NC->LsizeSector){ printf("SectorTableNo %lu NC->LsizeSector %lu, index %lu, N->N2S_LBsector_len %lu\n", SectorTableNo, NC->LsizeSector, index, N->N2S_LBsector_len); system("pause"); }
		assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);

		if (SLB->lastRWsection == SLB->guardTrack1stSection){//當前空間已寫滿
			//DWORD n1 = 23000, n2 = 34500; /**(I64)SLB->SLBsection_num / 50331648*/ /** (I64)SLB->SLBsection_num / 50331648;*/
			//|| N->SlotTable_Act > n1 || (N->SlotTable_Act + N->SlotTable_InAct > n2)
			if ((SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1) == SLB->firstRWsection){//發現guard track右邊是fRW，只能merge
				//printf("NS ");
				DM_SLBmerge2(simPtr);
			}
			else{//guard track右邊不是fRW，右移gt一格
				//printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].sector == 0x3fffffff);//0x7fffffff;
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					//if (N->blocks[NC->PsizeBlock - 1].sections[i].sector != 0x3fffffff){ printf("(%lu)%lu %lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[i].sector, SLB->guardTrack1stSection, SLB->firstRWsection, SLB->lastRWsection); system("pause"); }
				}
				SLB->guardTrack1stSection = (SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection) % (SLB->Partial_Section + 1);
				for (i = SLB->guardTrack1stSection; i < SLB->guardTrack1stSection + NC->CylinderSizeTrack*NC->trackSizeSection; i++){
					assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0);
					N->blocks[NC->PsizeBlock - 1].sections[i].sector = 0x3fffffff;//0x7fffffff;//means the sector is not yet mapping to data band
				}
			}
		}

#ifdef Time_Overhead
		//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
		if (FirstSection == 1 || SLB->lastRWsection / NC->trackSizeSection != (SLB->lastRWsection - 1) / NC->trackSizeSection){
			FirstSection = 0;
			test(simPtr, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section, 0);
			Calc_RotateOverhead2(simPtr, 0, 0);//第一次寫or換軌都要rotation
		}
#endif
		assert(SLB->lastRWsection >= 0); assert(SLB->lastRWsection <= SLB->Partial_Section);
		assert(N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid == 0);
		//printf("%lu %lu\n", N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid,N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector);

		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].sector = SectorTableNo; assert(SectorTableNo % NC->Section == 0);
		N->blocks[NC->PsizeBlock - 1].sections[SLB->lastRWsection].valid = 1;
		//invalid the same page(old page) in log buffer or in data band
		assert(SectorTableNo % NC->Section == 0);
		block_no = getblock(simPtr, SectorTableNo);
		section_no = getsection(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no <= NC->PsizeBlock - 1);
		if (block_no == NC->PsizeBlock - 1){
			assert(0 <= section_no && section_no <= SLB->Partial_Section);
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == SectorTableNo);
			N->blocks[block_no].sections[section_no].valid = 0;
			N->SLB_Hit++;
			N->cycleSLB_Hit++;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].InAct++;
				N->SlotTable_InAct++;
			}
		}
		else if (0 <= block_no && block_no <= NC->LsizeBlock - 1){
			assert(0 <= section_no); assert(section_no < NC->blockSizeSection);
			if (N->blocks[block_no].sections[section_no].valid == 2){
				DWORD NSLBdirtyTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector;
				if (N->NSLBremainSectionTable[NSLBdirtyTrack] == 0){ N->NSLBexhaustedTrack--; }
				N->NSLBremainSectionTable[NSLBdirtyTrack]++;
				NSLB->writedSectionNum--;

				N->blocks[block_no].sections[section_no].valid = 1;
				N->blocks[block_no].sections[section_no].sector = SectorTableNo;
			}
			assert(N->blocks[block_no].sections[section_no].valid == 1);
			assert(N->blocks[block_no].sections[section_no].sector == block_no*NC->blockSizeSector + section_no*NC->Section);
			N->blocks[block_no].sections[section_no].valid = 0;//最新資料寫入SLB，DB上原資料Invalid，head不用回去讀
			N->SLB_Miss++;
			N->cycleSLB_Miss++;
			//增加存活sector
			N->l2pmapOvd.AA++;
			//有不同的section寫入
			//N->blocks[block_no].sections[section_no].unique = 1;
			if (SLOTTABLE_BAND_FirstSection == 1){
				SLOTTABLE_BAND_FirstSection = 0;
				N->SlotTable_Band[block_no].Act++;
				N->SlotTable_Act++;
			}
		}//in Data Band

		//N->section_cumu[SectorTableNo / NC->Section]++;
		//更新L2P mapping table
		setblock(simPtr, SectorTableNo, NC->PsizeBlock - 1);
		setsection(simPtr, SectorTableNo, SLB->lastRWsection);
		//更新寫入量統計資料
		SLB->pageWrite += NC->Section; SLB->writedSectionNum++;
		SLB->write_count += NC->Section;
		/*if (SLB->writedSectionNum % (NC->CylinderSizeTrack*NC->trackSizeSection) == 1){
		printf("%lu ,", SLB->writedSectionNum);
		printf("%lf %lu %lu,%lu %lu %I64u\n", N->Write_W_SeekTime, N->Write_W_SeekCount, N->seek_0_WW, NC->LsizeSector, SLB->lastRWsection*NC->Section, (I64)NC->LsizeSector + SLB->lastRWsection*NC->Section); system("pause");
		}*/
		//標示為剛寫過，有這種section的DB才能被merge
		N->blocks[LBA].Merge = 0;
		//換下一個section做寫入
		SLB->lastRWsection = (SLB->lastRWsection + 1) % (SLB->Partial_Section + 1);
		//offsetSector += NC->Section;
		//lenSector -= NC->Section;
		//if (lenSector <= 0){ break; }
		index++; assert(index <= N->N2S_LBsector_len);
		if (index == N->N2S_LBsector_len){ break; }
	}
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);
}
void DM_NSLBw2(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD section_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;

	/*DWORD go_OldSection_NSLBtrack = 0;
	DWORD curSection = offsetSector / NC->Section;
	DWORD cur_Section_Update = 0;
	DWORD goRMW = 0;
	DWORD oldSection = -1;
	DWORD goDB = 0;*/
#ifdef Time_Overhead
	DWORD ReadTask_len = 0, WriteTask_len = 0;
	DWORD req_len = lenSector;
	DWORD *ReadTask = (DWORD*)calloc(req_len, sizeof(DWORD));
	DWORD *WriteTask = (DWORD*)calloc(req_len, sizeof(DWORD));
#endif
	N->cnt++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedSectionNum <= NSLB->NSLBsection_num);
		if (NSLB->writedSectionNum == NSLB->NSLBsection_num){//當前空間已寫滿
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			//printf("N ");
			DM_NSLBmerge2(simPtr);
		}

		//保證有free sector做寫入
		assert(NSLB->writedSectionNum < NSLB->NSLBsection_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//設定位置變數
		assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector; assert(NC->Section == 1);
		LBA = SectorTableNo / NC->blockSizeSector; assert(0 <= LBA && LBA < NC->LsizeBlock);
		//確認該sector的L2P mapping table正確
		block_no = SectorTableNo / NC->blockSizeSector;// getblock(simPtr, SectorTableNo); assert(block_no == LBA);
		section_no = SectorTableNo % NC->blockSizeSector; //getsection(simPtr, SectorTableNo); assert(0 <= section_no && section_no < NC->blockSizeSection);
		//確認此section不是新寫入就是第二次以上寫入
		assert(0 <= N->blocks[block_no].sections[section_no].valid && N->blocks[block_no].sections[section_no].valid <= 2);

		//更新該band寫入的時間, GC會挑LRU時間最小的(最老)band做NSLB寫回
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;

		//正式寫入
		if (N->blocks[block_no].sections[section_no].valid == 2){//第二次以上寫入
			//NSLB舊page找尋最佳寫入位置
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
			oldNSLBfreeTrack = N->blocks[block_no].sections[section_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
			//把舊section寫到cur track(preNSLBfreeTrack)
			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			//	}else{
			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
			//		for (i = 0; i < N->NSLB_tracks; i++){
			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
			//				NSLBfreeTrack = i;
			//				mini_TKdist = tmp_TKdist;
			//			}
			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//	//歸還舊NSLBremainSectorTable
			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
			//	//更新NSLB page位置
			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
			//	//更新NSLBremainSectorTable資訊
			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//}

			//寫入量統計
			NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count++;
			N->NSLB_Hit++;
			N->cycleNSLB_Hit++;

			WriteTask[WriteTask_len] = NC->LsizeSector + oldNSLBfreeTrack*NC->trackSizeSector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		else if (N->blocks[block_no].sections[section_no].valid == 1 || N->blocks[block_no].sections[section_no].valid == 0){//新寫入
			if (N->blocks[block_no].sections[section_no].valid == 0){
				DWORD SLB_block_no = getblock(simPtr, SectorTableNo);
				DWORD SLB_section_no = getsection(simPtr, SectorTableNo);
				assert(SLB_block_no == NC->LsizeBlock);
				assert(0 <= SLB_section_no && SLB_section_no <= SLB->Partial_Section);
				assert(N->blocks[SLB_block_no].sections[SLB_section_no].valid == 1);
				assert(N->blocks[SLB_block_no].sections[SLB_section_no].sector == SectorTableNo);
				N->blocks[SLB_block_no].sections[SLB_section_no].valid = 0;

				setblock(simPtr, SectorTableNo, block_no);
				setsection(simPtr, SectorTableNo, section_no);

				N->blocks[block_no].sections[section_no].valid = 1;
				N->blocks[block_no].sections[section_no].sector = SectorTableNo;
			}
			//寫入量統計
			NSLB->pageRead++;
			NSLB->pageWrite++;
			NSLB->writedSectionNum++;
			NSLB->write_count++;
			N->NSLB_Miss++;
			N->cycleNSLB_Miss++;
			//找尋可以寫入的NSLBremainSector
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			if (N->NSLBremainSectionTable[preNSLBfreeTrack]>0){
				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			}
			else{
				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
				for (i = 0; i < N->NSLB_tracks; i++){
					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
					if (N->NSLBremainSectionTable[i] > 0 && tmp_TKdist <= mini_TKdist){
						NSLBfreeTrack = i;
						mini_TKdist = tmp_TKdist;
					}
				}assert(NSLBfreeTrack != preNSLBfreeTrack);
			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].sections[section_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			N->blocks[block_no].sections[section_no].sector = NSLBfreeTrack*NC->trackSizeSector + (N->NSLBremainSectionTable[NSLBfreeTrack] - 1)*NC->Section;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].sections[section_no].sector && N->blocks[block_no].sections[section_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].sections[section_no].sector / NC->trackSizeSector == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			N->NSLBremainSectionTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectionTable[NSLBfreeTrack] && N->NSLBremainSectionTable[NSLBfreeTrack] < NC->trackSizeSection);
			if (N->NSLBremainSectionTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//增加存活section
			N->l2pmapOvd.AA++;

			ReadTask[ReadTask_len] = SectorTableNo;
			ReadTask_len++; assert(ReadTask_len <= req_len);
			WriteTask[WriteTask_len] = NC->LsizeSector + N->blocks[block_no].sections[section_no].sector;
			WriteTask_len++; assert(WriteTask_len <= req_len);
		}
		offsetSector += NC->Section;
		lenSector -= NC->Section;
		if (lenSector <= 0){ break; }
	}
#ifdef Time_Overhead
	//transfer time
	qsort(ReadTask, ReadTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 1);
	for (i = 1; i < ReadTask_len; i++){
		assert(ReadTask[i] < NC->LsizeSector);
		test(simPtr, ReadTask[i], 0);
		if (ReadTask[i - 1] / NC->trackSizeSector != ReadTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 1); }
	}
	qsort(WriteTask, WriteTask_len, sizeof(DWORD), compare);
	Calc_RotateOverhead2(simPtr, 0, 0);
	for (i = 1; i < WriteTask_len; i++){
		assert(WriteTask[i] >= NC->LsizeSector);
		test(simPtr, WriteTask[i], 0);
		if (WriteTask[i - 1] / NC->trackSizeSector != WriteTask[i] / NC->trackSizeSector){ Calc_RotateOverhead2(simPtr, 0, 0); }
	}
	N->Req_RunTime += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);
	N->content_rotate += ((double)(ReadTask_len + WriteTask_len) / NC->trackSizeSector * 10);//寫多少sector轉多少sector
	free(ReadTask);
	free(WriteTask);
#endif
	//Resp
	//printf("N->Finish_Time %llu, N->Req_RunTime %lu\n", N->Finish_Time, N->Req_RunTime);
	N->RunTime += N->Req_RunTime;
	N->Finish_Time += N->Req_RunTime;
	N->RespTIme += N->Finish_Time - N->Arr_Time;
	N->WaitingTime += (N->Finish_Time - N->Arr_Time - N->Req_RunTime);

}
void DM_ReqDependw2(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD dataBlock, SectorTableNo;
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD block_no, section_no;
	DWORD start_sector = offsetSector, end_sector = offsetSector + lenSector - 1;
	DWORD valid[3] = { 0, 0, 0 };
	assert(valid[0] == 0 && valid[1] == 0 && valid[2] == 0);
	while (start_sector <= end_sector){
		block_no = start_sector / NC->blockSizeSector;
		section_no = start_sector % NC->blockSizeSector;
		assert(0 <= N->blocks[block_no].sections[section_no].valid && N->blocks[block_no].sections[section_no].valid <= 2);
		valid[N->blocks[block_no].sections[section_no].valid]++;
		start_sector++;
	}
	if (valid[0] + valid[2] >= valid[1]){ DM_NSLBw2(simPtr, offsetSector, lenSector); }
	else{ DM_SLBw2(simPtr, offsetSector, lenSector); }
}
#endif

#ifdef SLBNSLBPM_Tmerge
#ifdef NEWGC64trackWRITE
BYTE track_offset(DWORD n)
{
	switch (n)
	{
	case 0:
		return 1;
	case 1:
		return 2;
	case 2:
		return 4;
	case 3:
		return 8;
	case 4:
		return 16;
	case 5:
		return 32;
	case 6:
		return 64;
	case 7:
		return 128;
	default:
		printf("bit error %d\n", n);
		system("pause");
		break;
	}
}
#endif

//NSLBPM
//void FASTwriteSectorNSLBPM3(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
//	DWORD cur_8subband = -1, prev_8subband = -1;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	DWORD page_no = 0, block_no = 0;
//	DWORD NSLBfreeTrack = 0;
//	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
//	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;
//
//	DWORD go_OldSection_NSLBtrack = 0;
//	DWORD curSection = offsetSector / N->SubTrack;
//	DWORD cur_Section_Update = 0;
//
//	DWORD goRMW = 0;
//	DWORD oldSection = -1;
//	DWORD goDB = 0;
//
//	DWORD GT_StartIndex = 0, GT_EndIndex = 0;
//
//	//找尋可以寫入的NSLBremainSector
//	/*NSLBfreeTrack = -1;
//	for (i = 0; i < N->NSLB_tracks; i++){ if (N->NSLBremainSectorTable[i]>0){ NSLBfreeTrack = i; break; } }
//	assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);*/
//	N->cnt++;
//	while (1) {
//		//確認merge條件
//		assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//		if (NSLB->writedPageNum == NSLB->NSLBpage_num){
//			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
//			//FASTmergeRWNSLBPM(simPtr); 
//
//			N->PM_GTsector = 0;
//			FASTmergeRWNSLBPM3(simPtr);
//			//FASTmergeRWNSLBPM已標示那些GT寫入NSLB，清出GT需要的NSLB空間
//			while (NSLB->writedPageNum + N->PM_GTsector >= NSLB->NSLBpage_num){
//				FASTmergeNormalNSLB3(simPtr);
//			}
//			assert(NSLB->writedPageNum + N->PM_GTsector < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
//			if (N->PM_GTsector > 0){//有做PM
//				//printf("--- NSLB->writedPageNum %lu, N->PM_GTsector %lu, NSLB->NSLBpage_num %lu ---\n", NSLB->writedPageNum, N->PM_GTsector, NSLB->NSLBpage_num);
//				preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
//				for (i = 0; i < N->PM_GTsector; i++){
//					BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
//					PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
//					assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage);
//					assert(N->blocks[BlockNo].Cut == 1); assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
//					N->blocks[BlockNo].pages[PageNo].valid = 3; 
//					if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){
//						NSLBfreeTrack = -1;
//						mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;
//						for (j = 0; j < N->NSLB_tracks; j++){
//							tmp_TKdist = abs(j - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//							if (N->NSLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//								NSLBfreeTrack = j;
//								mini_TKdist = tmp_TKdist;
//							}
//						}assert(NSLBfreeTrack != preNSLBfreeTrack);
//						assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//						/*if (!(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks)){
//							printf("error NSLBfreeTrack %lu, N->NSLB_tracks %lu\n", NSLBfreeTrack, N->NSLB_tracks); 
//							printf("NSLB->writedPageNum %lu, NSLB->NSLBpage_num %lu\n", NSLB->writedPageNum, NSLB->NSLBpage_num);
//							printf("N->NSLBexhaustedTrack %lu\n", N->NSLBexhaustedTrack); system("pause");
//						}*/
//						preNSLBfreeTrack = NSLBfreeTrack;
//						//目前寫的NSLB track給preNSLBfreeTrack紀錄
//						N->preNSLB_WriteSector = preNSLBfreeTrack*NC->trackSizePage;
//					}
//					N->blocks[BlockNo].pages[PageNo].sector = preNSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[preNSLBfreeTrack] - 1;
//					assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < NSLB->NSLBpage_num);
//					assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preNSLBfreeTrack);
//					NSLB->writedPageNum++;
//					//更新NSLBremainSectorTable資訊
//					N->NSLBremainSectorTable[preNSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[preNSLBfreeTrack] && N->NSLBremainSectorTable[preNSLBfreeTrack] < NC->trackSizePage);
//					if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//					//printf("ok ");
//				}
//				//printf("\nend--- NSLB->writedPageNum %lu, NSLB->NSLBpage_num %lu ---\n\n", NSLB->writedPageNum, NSLB->NSLBpage_num);
//				N->PM_GTsector = 0;
//			}
//		}//當前空間已寫滿
//		assert(NSLB->writedPageNum < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
//		//一般寫入流程
//		//新寫入sector的DB更新BPLRU時間
//		LBA = offsetSector / NC->blockSizeSector; assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
//		SectorTableNo = offsetSector / NC->pageSizeSector; assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
//		assert(N->BPLRU[LBA] <= N->cnt);
//		N->BPLRU[LBA] = N->cnt;
//		//確認該sector的L2P mapping table正確
//		block_no = getblock(simPtr, SectorTableNo);
//		page_no = getpage(simPtr, SectorTableNo);
//		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizePage);
//		assert(SectorTableNo / NC->blockSizeSector == block_no); assert(SectorTableNo % NC->blockSizeSector == page_no);
//		//新寫入或第二次以上寫入的情況分別處理
//		assert(1 <= N->blocks[block_no].pages[page_no].valid && N->blocks[block_no].pages[page_no].valid <= 3);
//		if (N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3){//第二次以上寫入 //GT上的section有資料寫入
//			//NSLB舊page找尋最佳寫入位置
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
//			oldNSLBfreeTrack = N->blocks[block_no].pages[page_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
//
//			//把舊section寫到cur track(preNSLBfreeTrack)
//			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
//			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			//	}else{
//			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//			//		for (i = 0; i < N->NSLB_tracks; i++){
//			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
//			//				NSLBfreeTrack = i;
//			//				mini_TKdist = tmp_TKdist;
//			//			}
//			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
//			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//			//	//歸還舊NSLBremainSectorTable
//			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
//			//	//更新NSLB page位置
//			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
//			//	//更新NSLBremainSectorTable資訊
//			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
//			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//			//}
//
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
//			NSLB->write_count++;
//
//			N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//		}
//		else if (N->blocks[block_no].pages[page_no].valid == 1){//新寫入
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++; NSLB->writedPageNum++;
//			NSLB->write_count++;
//			//找尋可以寫入的NSLBremainSector
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
//			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
//			if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			}
//			else{
//				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//				for (i = 0; i < N->NSLB_tracks; i++){
//					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//					if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
//						NSLBfreeTrack = i;
//						mini_TKdist = tmp_TKdist;
//					}
//				}assert(NSLBfreeTrack != preNSLBfreeTrack);
//			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//			//在DB上標記為寫入NSLB的page
//			N->blocks[block_no].pages[page_no].valid = 2;
//			//把寫入的NSLB track記錄到原DB的sector上
//			N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[NSLBfreeTrack] - 1;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
//			assert(0 <= N->blocks[block_no].pages[page_no].sector && N->blocks[block_no].pages[page_no].sector < NSLB->NSLBpage_num);
//			assert(N->blocks[block_no].pages[page_no].sector / NC->trackSizePage == NSLBfreeTrack);
//			//更新NSLBremainSectorTable資訊
//			N->NSLBremainSectorTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] < NC->trackSizePage);
//			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//			
//			N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//		}
//
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0){ break; }
//	}
//}
//
//void FASTwriteSectorNSLBPMRAM3_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	DWORD i = 0, j = 0;
//	//section mapping
//	DWORD start_section = -1;
//	DWORD Section_Update = 0;
//#ifdef Time_Overhead
//	DWORD WriteBuff_Size = 0;
//	DWORD *WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//#endif
//#ifdef NSLB_RAM_FastSectLRU_HIT
//	//sector LRU + hit
//	DWORD skip = 0;
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	DWORD LRU_band = -1;
//	DWORD old_NSLB_RAM_request = 0;
//	//DWORD start = 0;
//	if (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
//		while (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){
//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
//			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
//			for (i = 0; i < N->NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//					N->NSLB_RAM[i] = -1;//原RAM失效
//				}
//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//			//tmp_NSLB_RAM用section mapping寫入NSLB
//			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//			assert(tmp_NSLB_RAM[0] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
//			start_section = tmp_NSLB_RAM[0] / N->SubTrack;
//			/*Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[0] % N->SubTrack);*/
//#ifdef Time_Overhead
//			WriteBuff_Size = 0;
//#endif
//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
//				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//					//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//正式寫前面section的
//#ifdef Time_Overhead
//					SectorTableNo = start_section*N->SubTrack;
//					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//					if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//					else if (N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
//#endif
//					FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
//					//Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);
//				}
//				else{ /*Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);*/ }
//			}
//			//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//寫最後一個section的
//#ifdef Time_Overhead
//			SectorTableNo = start_section*N->SubTrack;
//			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//			if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//			else if (N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
//#endif
//			FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
//#ifdef Time_Overhead
//			assert(0 < WriteBuff_Size); assert(WriteBuff_Size <= N->NSLB_RAM_SIZE);
//			qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
//			for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); }
//			assert(0 <= WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
//			for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//				if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){
//					//seek
//					assert(0 <= WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//					Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//				}
//			}
//#endif
//			//統計RAM Hit吸收的寫入
//			N->RAM_WriteOut += tmp_NSLB_RAM_request;
//
//			//更新NSLB_RAM_request資訊
//			old_NSLB_RAM_request = N->NSLB_RAM_request;
//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//			//整理NSLB_RAM，清掉-1的資料
//			j = 0;
//			for (i = 0; i < old_NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
//			}assert(j == N->NSLB_RAM_request);
//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
//			free(tmp_NSLB_RAM);
//		}assert(N->NSLB_RAM_request + lenSector <= N->NSLB_RAM_SIZE);
//	}
//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//	while (1) {
//		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		if (N->RAM_Valid[offsetSector] == 0){// skip == 0
//			N->RAM_Valid[offsetSector] = 1;
//			N->NSLB_RAM[N->NSLB_RAM_request] = offsetSector;
//			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		}
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0) break;
//	}
//#endif//WC2
//#ifdef Time_Overhead
//	free(WriteBuff);
//#endif
//}
//
//void FASTwriteSectorNSLBPMRAM3_WriteAllOut(sim *simPtr)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	DWORD i = 0, j = 0;
//	DWORD start_section = 0, Section_Update = 0;
//	//sector LRU + hit
//	DWORD skip = 0;
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	DWORD LRU_band = -1;
//	DWORD old_NSLB_RAM_request = 0;
//	if (N->NSLB_RAM_request > 0){
//
//		/*qsort(N->NSLB_RAM, N->NSLB_RAM_request, sizeof(DWORD), compare);
//		assert(N->NSLB_RAM[0] != -1); assert(N->RAM_Valid[N->NSLB_RAM[0]] == 1); N->RAM_Valid[N->NSLB_RAM[0]] = 0;
//		start_section = N->NSLB_RAM[0] / N->SubTrack;
//		N->NSLB_RAM[0] = -1;
//		for (i = 1; i < N->NSLB_RAM_request; i++){
//		assert(N->NSLB_RAM[i] != -1); assert(N->RAM_Valid[N->NSLB_RAM[i]] == 1); N->RAM_Valid[N->NSLB_RAM[i]] = 0;
//		if (N->NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//		FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
//		start_section = N->NSLB_RAM[i] / N->SubTrack;
//		}
//		N->NSLB_RAM[i] = -1;
//		}
//		FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
//		N->NSLB_RAM_request = 0;*/
//
//#ifdef Time_Overhead
//		DWORD WriteBuff_Size = 0;
//		DWORD *WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//		DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//#endif
//		while (N->NSLB_RAM_request > 0){
//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
//			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
//			for (i = 0; i < N->NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//					N->NSLB_RAM[i] = -1;//原RAM失效
//				}
//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//			//tmp_NSLB_RAM用section mapping寫入NSLB
//			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//			assert(tmp_NSLB_RAM[0] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
//			start_section = tmp_NSLB_RAM[0] / N->SubTrack;
//			/*Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[0] % N->SubTrack);*/
//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
//				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//					//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//正式寫前面section的
//#ifdef Time_Overhead
//					SectorTableNo = start_section*N->SubTrack;
//					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//					if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//					else if (N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
//#endif
//					FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
//					//Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);
//				}
//				else{ /*Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);*/ }
//			}
//			//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//寫最後一個section的
//#ifdef Time_Overhead
//			SectorTableNo = start_section*N->SubTrack;
//			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
//			if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//			else if (N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
//#endif
//			FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
//			//統計RAM Hit吸收的寫入
//			N->RAM_WriteOut += tmp_NSLB_RAM_request;
//
//			//更新NSLB_RAM_request資訊
//			old_NSLB_RAM_request = N->NSLB_RAM_request;
//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//			//整理NSLB_RAM，清掉-1的資料
//			j = 0;
//			for (i = 0; i < old_NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
//			}assert(j == N->NSLB_RAM_request);
//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
//			free(tmp_NSLB_RAM);
//		}assert(N->NSLB_RAM_request == 0);
//#ifdef Time_Overhead
//		assert(0 < WriteBuff_Size);  assert(WriteBuff_Size <= N->NSLB_RAM_SIZE);
//		qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
//		for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); }
//		assert(0 <= WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//		Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
//		for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//			if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){
//				//seek
//				assert(0 <= WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//				Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//			}
//		}
//#endif
//	}
//}



//void FASTwriteSectorNSLBPM3(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
//	DWORD cur_8subband = -1, prev_8subband = -1;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	DWORD page_no = 0, block_no = 0;
//	DWORD NSLBfreeTrack = 0;
//	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
//	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;
//
//	DWORD go_OldSection_NSLBtrack = 0;
//	DWORD curSection = offsetSector / N->SubTrack;
//	DWORD cur_Section_Update = 0;
//
//	DWORD goRMW = 0;
//	DWORD oldSection = -1;
//	DWORD goDB = 0;
//
//	DWORD GT_StartIndex = 0, GT_EndIndex = 0;
//	DWORD preSLBfreeTrack = 0;
//	DWORD SLBfreeTrack = 0;
//	//找尋可以寫入的NSLBremainSector
//	/*NSLBfreeTrack = -1;
//	for (i = 0; i < N->NSLB_tracks; i++){ if (N->NSLBremainSectorTable[i]>0){ NSLBfreeTrack = i; break; } }
//	assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);*/
//	N->cnt++;
//	while (1) {
//		//確認merge條件
//		assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//		if (NSLB->writedPageNum == NSLB->NSLBpage_num){
//			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
//			//FASTmergeRWNSLBPM(simPtr); 
//
//			N->PM_GTsector = 0; N->PM_Tracks_Allocated2Use_Condition = 0;
//			FASTmergeRWNSLBPM3(simPtr);
//
//			//FASTmergeRWNSLBPM已標示那些GT寫入NSLB，清出GT需要的NSLB空間
//			while (NSLB->writedPageNum + N->PM_GTsector/2 >= NSLB->NSLBpage_num){
//				FASTmergeNormalNSLB3(simPtr);
//			}
//			assert(NSLB->writedPageNum + N->PM_GTsector/2 < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
//
//			//assert(NSLB->writedPageNum < NSLB->NSLBpage_num);// assert(N->SLBexhaustedTrack < N->SLB_tracks);
//			if (N->PM_GTsector > 0 && N->PM_Tracks_Allocated2Use_Condition == 1){//有做PM //表示剛才有做NSLB轉SLB
//				//把NSLB被轉成SLB的紀錄起來，之後才能歸還
//				preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
//				mini_TKdist = 0; tmp_TKdist = 0;//N->NSLB_tracks
//				for (i = 0; i < N->NSLB_tracks; i++){//挑最遠的NSLBtrack轉成SLBtrack
//					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//					if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist > mini_TKdist){
//						NSLBfreeTrack = i; mini_TKdist = tmp_TKdist;
//					}
//				}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//				preNSLBfreeTrack = NSLBfreeTrack;
//				for (i = 0; i < N->PM_GTsector / 2; i++){//1個NSLNtrack是2個SLBtrack，所以剛剛只清理N->PM_GTsector / 2空間
//					if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){//當之前挑到的NSLBtrack用完，再挑一條離最近的轉SLBtrack
//						NSLBfreeTrack = -1; mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;//N->NSLB_tracks
//						for (j = 0; j < N->NSLB_tracks; j++){
//							tmp_TKdist = abs(j - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//							if (N->NSLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//								NSLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//							}
//						}assert(NSLBfreeTrack != preNSLBfreeTrack); assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//						preNSLBfreeTrack = NSLBfreeTrack; assert(N->NSLBremainSectorTable[preNSLBfreeTrack]>0);
//					}
//					BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
//					PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
//					assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1);
//					assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
//					assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
//					N->blocks[BlockNo].pages[PageNo].valid = 3;
//					//GT放入NSLB
//					assert(0 < N->NSLBremainSectorTable[preNSLBfreeTrack]); assert(N->NSLBremainSectorTable[preNSLBfreeTrack] <= NC->trackSizePage);
//					N->blocks[BlockNo].pages[PageNo].sector = preNSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[preNSLBfreeTrack] - 1;
//					assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < NSLB->NSLBpage_num);
//					assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preNSLBfreeTrack);
//					NSLB->writedPageNum++;
//					//更新NSLBremainSectorTable資訊
//					assert(N->NSLBremainSectorTable[preNSLBfreeTrack]>0);
//					N->NSLBremainSectorTable[preNSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[preNSLBfreeTrack] && N->NSLBremainSectorTable[preNSLBfreeTrack] < NC->trackSizePage);
//					if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//				}
//				//寫入新配的SLB
//				preSLBfreeTrack = N->preSLB_WriteSector / NC->trackSizePage; assert(0 <= preSLBfreeTrack && preSLBfreeTrack < N->SLB_tracks); //紀錄NSLB上之前寫到的track
//				for (i = N->PM_GTsector / 2; i < N->PM_GTsector; i++){//另一半送入SLB // N->PM_GTsector / 2
//					if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){
//						SLBfreeTrack = -1; mini_TKdist = N->SLB_tracks; tmp_TKdist = N->SLB_tracks;//N->NSLB_tracks
//						for (j = 0; j < N->SLB_tracks; j++){
//							tmp_TKdist = abs(j - preSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->SLB_tracks);
//							if (N->SLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//								SLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//							}
//						}assert(SLBfreeTrack != preSLBfreeTrack); assert(0 <= SLBfreeTrack && SLBfreeTrack < N->SLB_tracks);
//						preSLBfreeTrack = SLBfreeTrack; assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//					}
//					BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
//					PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
//					assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1); 
//					assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
//					assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
//					N->blocks[BlockNo].pages[PageNo].valid = 0;
//					//GT放入SLB
//					assert(0 < N->SLBremainSectorTable[preSLBfreeTrack]); assert(N->SLBremainSectorTable[preSLBfreeTrack] <= NC->trackSizePage);
//					N->blocks[BlockNo].pages[PageNo].sector = preSLBfreeTrack*NC->trackSizePage + N->SLBremainSectorTable[preSLBfreeTrack] - 1;
//					assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < SLB->SLBpage_num);
//					assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preSLBfreeTrack);
//					SLB->writedPageNum++;
//					//更新SLBremainSectorTable資訊
//					assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//					N->SLBremainSectorTable[preSLBfreeTrack]--; assert(0 <= N->SLBremainSectorTable[preSLBfreeTrack] && N->SLBremainSectorTable[preSLBfreeTrack] < NC->trackSizePage);
//					if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){ N->SLBexhaustedTrack++; }
//				}
//				//這些GT寫入LB的統計在merge前做完
//				N->PM_GTsector = 0;
//			}
//		}//當前空間已寫滿
//		assert(NSLB->writedPageNum < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
//		//一般寫入流程
//		//新寫入sector的DB更新BPLRU時間
//		LBA = offsetSector / NC->blockSizeSector; assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
//		SectorTableNo = offsetSector / NC->pageSizeSector; assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
//		assert(N->BPLRU[LBA] <= N->cnt);
//		N->BPLRU[LBA] = N->cnt;
//		//確認該sector的L2P mapping table正確
//		block_no = getblock(simPtr, SectorTableNo);
//		page_no = getpage(simPtr, SectorTableNo);
//		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizePage);
//		assert(SectorTableNo / NC->blockSizeSector == block_no); assert(SectorTableNo % NC->blockSizeSector == page_no);
//		//新寫入或第二次以上寫入的情況分別處理
//		assert(0 <= N->blocks[block_no].pages[page_no].valid && N->blocks[block_no].pages[page_no].valid <= 3);
//		//|| N->blocks[block_no].pages[page_no].valid == 3
//		if (N->blocks[block_no].pages[page_no].valid == 2){//第二次以上寫入 //GT上的section有資料寫入
//			//NSLB舊page找尋最佳寫入位置
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
//			oldNSLBfreeTrack = N->blocks[block_no].pages[page_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
//
//			//把舊section寫到cur track(preNSLBfreeTrack)
//			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
//			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			//	}else{
//			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//			//		for (i = 0; i < N->NSLB_tracks; i++){
//			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
//			//				NSLBfreeTrack = i;
//			//				mini_TKdist = tmp_TKdist;
//			//			}
//			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
//			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//			//	//歸還舊NSLBremainSectorTable
//			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
//			//	//更新NSLB page位置
//			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
//			//	//更新NSLBremainSectorTable資訊
//			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
//			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//			//}
//
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
//			NSLB->write_count++;
//
//			N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//		}
//		else if (N->blocks[block_no].pages[page_no].valid == 1){//新寫入
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++; NSLB->writedPageNum++;
//			NSLB->write_count++;
//			//找尋可以寫入的NSLBremainSector
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
//			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
//			if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			}
//			else{
//				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//				for (i = 0; i < N->NSLB_tracks; i++){
//					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//					if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
//						NSLBfreeTrack = i;
//						mini_TKdist = tmp_TKdist;
//					}
//				}assert(NSLBfreeTrack != preNSLBfreeTrack);
//			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks); assert(N->NSLBremainSectorTable[NSLBfreeTrack]>0);
//			//在DB上標記為寫入NSLB的page
//			N->blocks[block_no].pages[page_no].valid = 2;
//			//把寫入的NSLB track記錄到原DB的sector上
//			assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack]); assert(N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);
//			N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[NSLBfreeTrack] - 1;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
//			assert(0 <= N->blocks[block_no].pages[page_no].sector && N->blocks[block_no].pages[page_no].sector < NSLB->NSLBpage_num);
//			assert(N->blocks[block_no].pages[page_no].sector / NC->trackSizePage == NSLBfreeTrack);
//			//更新NSLBremainSectorTable資訊
//			assert(N->NSLBremainSectorTable[NSLBfreeTrack]>0);
//			N->NSLBremainSectorTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] < NC->trackSizePage);
//			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//
//			N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//		}
//
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0){ break; }
//	}
//}
//
//void FASTwriteSectorNSLBPMRAM3_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	DWORD i = 0, j = 0;
//	//section mapping
//	DWORD start_section = -1;
//	DWORD Section_Update = 0;
//#ifdef Time_Overhead
//	DWORD WriteBuff_Size = 0;
//	DWORD *WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//#endif
//#ifdef NSLB_RAM_FastSectLRU_HIT
//	//sector LRU + hit
//	DWORD skip = 0;
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	DWORD LRU_band = -1;
//	DWORD old_NSLB_RAM_request = 0;
//	//DWORD start = 0;
//	if (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
//		while (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){
//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
//			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
//			for (i = 0; i < N->NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//					N->NSLB_RAM[i] = -1;//原RAM失效
//				}
//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//			//tmp_NSLB_RAM用section mapping寫入NSLB
//			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//			assert(tmp_NSLB_RAM[0] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
//			start_section = tmp_NSLB_RAM[0] / N->SubTrack;
//			/*Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[0] % N->SubTrack);*/
//#ifdef Time_Overhead
//			WriteBuff_Size = 0;
//#endif
//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
//				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//					//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//正式寫前面section的
//#ifdef Time_Overhead
//					SectorTableNo = start_section*N->SubTrack;
//					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); //N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
//					if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//					else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB   % (N->NSLB_DataTracks*NC->trackSizePage))
//					else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //     % (N->PM_Tracks*NC->trackSizePage))
//#endif
//					FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
//					//Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);
//				}
//				else{ /*Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);*/ }
//			}
//			//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//寫最後一個section的
//#ifdef Time_Overhead
//			SectorTableNo = start_section*N->SubTrack;
//			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); //N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
//			if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//			else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB    % (N->NSLB_DataTracks*NC->trackSizePage))
//			else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //  NC->LsizeSector + (N->NSLB_tracks - 1)*NC->trackSizePage
//#endif
//			FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
//#ifdef Time_Overhead
//			assert(0 < WriteBuff_Size); assert(WriteBuff_Size <= N->NSLB_RAM_SIZE);
//			qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
//			for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); }
//			assert(0 <= WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//			Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
//			for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//				if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){
//					//seek
//					assert(0 <= WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//					Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//				}
//			}
//#endif
//			//統計RAM Hit吸收的寫入
//			N->RAM_WriteOut += tmp_NSLB_RAM_request;
//
//			//更新NSLB_RAM_request資訊
//			old_NSLB_RAM_request = N->NSLB_RAM_request;
//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//			//整理NSLB_RAM，清掉-1的資料
//			j = 0;
//			for (i = 0; i < old_NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
//			}assert(j == N->NSLB_RAM_request);
//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
//			free(tmp_NSLB_RAM);
//		}assert(N->NSLB_RAM_request + lenSector <= N->NSLB_RAM_SIZE);
//	}
//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//	while (1) {
//		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		if (N->RAM_Valid[offsetSector] == 0){// skip == 0
//			N->RAM_Valid[offsetSector] = 1;
//			N->NSLB_RAM[N->NSLB_RAM_request] = offsetSector;
//			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//		}
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0) break;
//	}
//#endif//WC2
//#ifdef Time_Overhead
//	free(WriteBuff);
//#endif
//}
//
//void FASTwriteSectorNSLBPMRAM3_WriteAllOut(sim *simPtr)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	DWORD i = 0, j = 0;
//	DWORD start_section = 0, Section_Update = 0;
//	//sector LRU + hit
//	DWORD skip = 0;
//	DWORD *tmp_NSLB_RAM = NULL;
//	DWORD tmp_NSLB_RAM_request = 0;
//	DWORD LRU_band = -1;
//	DWORD old_NSLB_RAM_request = 0;
//	if (N->NSLB_RAM_request > 0){
//
//		/*qsort(N->NSLB_RAM, N->NSLB_RAM_request, sizeof(DWORD), compare);
//		assert(N->NSLB_RAM[0] != -1); assert(N->RAM_Valid[N->NSLB_RAM[0]] == 1); N->RAM_Valid[N->NSLB_RAM[0]] = 0;
//		start_section = N->NSLB_RAM[0] / N->SubTrack;
//		N->NSLB_RAM[0] = -1;
//		for (i = 1; i < N->NSLB_RAM_request; i++){
//		assert(N->NSLB_RAM[i] != -1); assert(N->RAM_Valid[N->NSLB_RAM[i]] == 1); N->RAM_Valid[N->NSLB_RAM[i]] = 0;
//		if (N->NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//		FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
//		start_section = N->NSLB_RAM[i] / N->SubTrack;
//		}
//		N->NSLB_RAM[i] = -1;
//		}
//		FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
//		N->NSLB_RAM_request = 0;*/
//
//#ifdef Time_Overhead
//		DWORD WriteBuff_Size = 0;
//		DWORD *WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//		DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
//#endif
//		while (N->NSLB_RAM_request > 0){
//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
//			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
//			for (i = 0; i < N->NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//					N->NSLB_RAM[i] = -1;//原RAM失效
//				}
//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
//			//tmp_NSLB_RAM用section mapping寫入NSLB
//			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//			assert(tmp_NSLB_RAM[0] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
//			start_section = tmp_NSLB_RAM[0] / N->SubTrack;
//			/*Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[0] % N->SubTrack);*/
//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
//				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//					//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//正式寫前面section的
//#ifdef Time_Overhead
//					SectorTableNo = start_section*N->SubTrack;
//					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); // N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
//					if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//					else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB   % (N->NSLB_DataTracks*NC->trackSizePage))
//					else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //   % (N->PM_Tracks*NC->trackSizePage))
//#endif
//					FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
//					//Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);
//				}
//				else{ /*Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);*/ }
//			}
//			//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//寫最後一個section的
//#ifdef Time_Overhead
//			SectorTableNo = start_section*N->SubTrack;
//			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); // N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
//			if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
//			else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB   % (N->NSLB_DataTracks*NC->trackSizePage))
//			else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //    % (N->PM_Tracks*NC->trackSizePage))
//#endif
//			FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
//			//統計RAM Hit吸收的寫入
//			N->RAM_WriteOut += tmp_NSLB_RAM_request;
//
//			//更新NSLB_RAM_request資訊
//			old_NSLB_RAM_request = N->NSLB_RAM_request;
//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
//			//整理NSLB_RAM，清掉-1的資料
//			j = 0;
//			for (i = 0; i < old_NSLB_RAM_request; i++){
//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
//			}assert(j == N->NSLB_RAM_request);
//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
//			free(tmp_NSLB_RAM);
//		}assert(N->NSLB_RAM_request == 0);
//#ifdef Time_Overhead
//		assert(0 < WriteBuff_Size);  assert(WriteBuff_Size <= N->NSLB_RAM_SIZE);
//		qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
//		for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); }
//		assert(0 <= WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//		Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
//		for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
//			if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){
//				//seek
//				assert(0 <= WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
//				Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//			}
//		}
//#endif
//	}
//}



//void FASTwriteSectorNSLBPM3(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
//	DWORD cur_8subband = -1, prev_8subband = -1;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	DWORD page_no = 0, block_no = 0;
//	DWORD NSLBfreeTrack = 0;
//	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
//	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;
//
//	DWORD go_OldSection_NSLBtrack = 0;
//	DWORD curSection = offsetSector / N->SubTrack;
//	DWORD cur_Section_Update = 0;
//
//	DWORD goRMW = 0;
//	DWORD oldSection = -1;
//	DWORD goDB = 0;
//
//	DWORD GT_StartIndex = 0, GT_EndIndex = 0;
//	DWORD preSLBfreeTrack = 0;
//	DWORD SLBfreeTrack = 0;
//	//找尋可以寫入的NSLBremainSector
//	/*NSLBfreeTrack = -1;
//	for (i = 0; i < N->NSLB_tracks; i++){ if (N->NSLBremainSectorTable[i]>0){ NSLBfreeTrack = i; break; } }
//	assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);*/
//	N->cnt++;
//	while (1) {
//		//確認merge條件
//		assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//		if (NSLB->writedPageNum == NSLB->NSLBpage_num){
//			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
//			//FASTmergeRWNSLBPM(simPtr); 
//
//			N->PM_GTsector = 0; N->PM_Tracks_Allocated2Use_Condition = 0;
//			FASTmergeRWNSLBPM3(simPtr);
//
//			//FASTmergeRWNSLBPM已標示那些GT寫入NSLB，清出GT需要的NSLB空間
//			while (NSLB->writedPageNum + N->PM_GTsector / 2 >= NSLB->NSLBpage_num){
//				FASTmergeNormalNSLB3(simPtr);
//			}assert(NSLB->writedPageNum + N->PM_GTsector / 2 < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
//			//assert(NSLB->writedPageNum < NSLB->NSLBpage_num);// assert(N->SLBexhaustedTrack < N->SLB_tracks);
//
//			//當NSLB轉SLB的時候，先寫fNSLB或SLB都沒差
//			if (N->PM_Sectors_Use < 2 * N->PM_Rate_SLBdefault * NSLB->NSLBpage_num / 100 - 1){//N->PM_Rate <= 5
//				if (N->PM_GTsector > 0 && N->PM_Tracks_Allocated2Use_Condition == 1){//有做PM //表示剛才有做NSLB轉SLB
//					//寫入新配的SLB
//					preSLBfreeTrack = N->preSLB_WriteSector / NC->trackSizePage; assert(0 <= preSLBfreeTrack && preSLBfreeTrack < N->SLB_tracks); //紀錄NSLB上之前寫到的track
//					for (i = 0; i < N->PM_GTsector / 2; i++){//直接送入預留的SLB 
//						if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){
//							SLBfreeTrack = -1; mini_TKdist = N->SLB_tracks; tmp_TKdist = N->SLB_tracks;//N->SLB_tracks
//							for (j = 0; j < N->SLB_tracks; j++){
//								tmp_TKdist = abs(j - preSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->SLB_tracks);
//								if (N->SLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//									SLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//								}
//							}assert(SLBfreeTrack != preSLBfreeTrack); assert(0 <= SLBfreeTrack && SLBfreeTrack < N->SLB_tracks);
//							preSLBfreeTrack = SLBfreeTrack; assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//						}
//						BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
//						PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
//						assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1);
//						assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
//						assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
//						N->blocks[BlockNo].pages[PageNo].valid = 0;
//						//GT放入SLB
//						assert(0 < N->SLBremainSectorTable[preSLBfreeTrack]); assert(N->SLBremainSectorTable[preSLBfreeTrack] <= NC->trackSizePage);
//						N->blocks[BlockNo].pages[PageNo].sector = preSLBfreeTrack*NC->trackSizePage + N->SLBremainSectorTable[preSLBfreeTrack] - 1;
//						assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < SLB->SLBpage_num);
//						assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preSLBfreeTrack);
//						SLB->writedPageNum++;
//						//更新SLBremainSectorTable資訊
//						assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//						N->SLBremainSectorTable[preSLBfreeTrack]--; assert(0 <= N->SLBremainSectorTable[preSLBfreeTrack] && N->SLBremainSectorTable[preSLBfreeTrack] < NC->trackSizePage);
//						if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){ N->SLBexhaustedTrack++; }
//					}
//					//這些GT寫入LB的統計在merge前做完
//					N->PM_GTsector = 0;
//				}
//			}
//			else{
//				if (N->PM_GTsector > 0 && N->PM_Tracks_Allocated2Use_Condition == 1){//有做PM //表示剛才有做NSLB轉SLB
//					//把NSLB被轉成SLB的紀錄起來，之後才能歸還
//					preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
//					mini_TKdist = 0; tmp_TKdist = 0;//N->NSLB_tracks
//					for (i = 0; i < N->NSLB_tracks; i++){//挑最遠的NSLBtrack轉成SLBtrack
//						tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//						if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist > mini_TKdist){
//							NSLBfreeTrack = i; mini_TKdist = tmp_TKdist;
//						}
//					}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//					preNSLBfreeTrack = NSLBfreeTrack;
//					for (i = 0; i < N->PM_GTsector / 2; i++){//1個NSLNtrack是2個SLBtrack，所以剛剛只清理N->PM_GTsector / 2空間
//						if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){//當之前挑到的NSLBtrack用完，再挑一條離最近的轉SLBtrack
//							NSLBfreeTrack = -1; mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;//N->NSLB_tracks
//							for (j = 0; j < N->NSLB_tracks; j++){
//								tmp_TKdist = abs(j - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//								if (N->NSLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//									NSLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//								}
//							}assert(NSLBfreeTrack != preNSLBfreeTrack); assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//							preNSLBfreeTrack = NSLBfreeTrack; assert(N->NSLBremainSectorTable[preNSLBfreeTrack]>0);
//						}
//						BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
//						PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
//						assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1);
//						assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
//						assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
//						N->blocks[BlockNo].pages[PageNo].valid = 3;
//						//GT放入NSLB
//						assert(0 < N->NSLBremainSectorTable[preNSLBfreeTrack]); assert(N->NSLBremainSectorTable[preNSLBfreeTrack] <= NC->trackSizePage);
//						N->blocks[BlockNo].pages[PageNo].sector = preNSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[preNSLBfreeTrack] - 1;
//						assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < NSLB->NSLBpage_num);
//						assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preNSLBfreeTrack);
//						NSLB->writedPageNum++;
//						//更新NSLBremainSectorTable資訊
//						assert(N->NSLBremainSectorTable[preNSLBfreeTrack]>0);
//						N->NSLBremainSectorTable[preNSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[preNSLBfreeTrack] && N->NSLBremainSectorTable[preNSLBfreeTrack] < NC->trackSizePage);
//						if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//					}
//				}
//			}
//
//			if (N->PM_GTsector > 0 && N->PM_Tracks_Allocated2Use_Condition == 1){//有做PM //表示剛才有做NSLB轉SLB
//				//寫入新配的SLB
//				preSLBfreeTrack = N->preSLB_WriteSector / NC->trackSizePage; assert(0 <= preSLBfreeTrack && preSLBfreeTrack < N->SLB_tracks); //紀錄NSLB上之前寫到的track
//
//				//紀錄PMGT寫到哪一個NSLB上
//				if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){
//					SLBfreeTrack = -1; mini_TKdist = N->SLB_tracks; tmp_TKdist = N->SLB_tracks;//N->SLB_tracks
//					for (j = 0; j < N->SLB_tracks; j++){
//						tmp_TKdist = abs(j - preSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->SLB_tracks);
//						if (N->SLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//							SLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//						}
//					}assert(SLBfreeTrack != preSLBfreeTrack); assert(0 <= SLBfreeTrack && SLBfreeTrack < N->SLB_tracks);
//					preSLBfreeTrack = SLBfreeTrack; assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//				}
//				N->PMGT_NSLBaddress = NC->LsizeSector + preSLBfreeTrack*NC->trackSizePage;
//
//				for (i = N->PM_GTsector / 2; i < N->PM_GTsector; i++){//另一半送入SLB // N->PM_GTsector / 2
//					if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){
//						SLBfreeTrack = -1; mini_TKdist = N->SLB_tracks; tmp_TKdist = N->SLB_tracks;//N->SLB_tracks
//						for (j = 0; j < N->SLB_tracks; j++){
//							tmp_TKdist = abs(j - preSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->SLB_tracks);
//							if (N->SLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//								SLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//							}
//						}assert(SLBfreeTrack != preSLBfreeTrack); assert(0 <= SLBfreeTrack && SLBfreeTrack < N->SLB_tracks);
//						preSLBfreeTrack = SLBfreeTrack; assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//					}
//					BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
//					PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
//					assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1);
//					assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
//					assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
//					N->blocks[BlockNo].pages[PageNo].valid = 0;
//					//GT放入SLB
//					assert(0 < N->SLBremainSectorTable[preSLBfreeTrack]); assert(N->SLBremainSectorTable[preSLBfreeTrack] <= NC->trackSizePage);
//					N->blocks[BlockNo].pages[PageNo].sector = preSLBfreeTrack*NC->trackSizePage + N->SLBremainSectorTable[preSLBfreeTrack] - 1;
//					assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < SLB->SLBpage_num);
//					assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preSLBfreeTrack);
//					SLB->writedPageNum++;
//					//更新SLBremainSectorTable資訊
//					assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//					N->SLBremainSectorTable[preSLBfreeTrack]--; assert(0 <= N->SLBremainSectorTable[preSLBfreeTrack] && N->SLBremainSectorTable[preSLBfreeTrack] < NC->trackSizePage);
//					if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){ N->SLBexhaustedTrack++; }
//				}
//
//				//這些GT寫入LB的統計在merge前做完
//				N->PM_GTsector = 0;
//			}
//
//		}//當前空間已寫滿
//		assert(NSLB->writedPageNum < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
//		//一般寫入流程
//		//新寫入sector的DB更新BPLRU時間
//		LBA = offsetSector / NC->blockSizeSector; assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
//		SectorTableNo = offsetSector / NC->pageSizeSector; assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
//		assert(N->BPLRU[LBA] <= N->cnt);
//		N->BPLRU[LBA] = N->cnt;
//		//確認該sector的L2P mapping table正確
//		block_no = getblock(simPtr, SectorTableNo);
//		page_no = getpage(simPtr, SectorTableNo);
//		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizePage);
//		assert(SectorTableNo / NC->blockSizeSector == block_no); assert(SectorTableNo % NC->blockSizeSector == page_no);
//		//新寫入或第二次以上寫入的情況分別處理
//		assert(0 <= N->blocks[block_no].pages[page_no].valid && N->blocks[block_no].pages[page_no].valid <= 3);
//		//|| N->blocks[block_no].pages[page_no].valid == 3
//		if (N->blocks[block_no].pages[page_no].valid == 2){//第二次以上寫入 //GT上的section有資料寫入
//			//NSLB舊page找尋最佳寫入位置
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
//			oldNSLBfreeTrack = N->blocks[block_no].pages[page_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
//
//			//把舊section寫到cur track(preNSLBfreeTrack)
//			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
//			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			//	}else{
//			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//			//		for (i = 0; i < N->NSLB_tracks; i++){
//			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
//			//				NSLBfreeTrack = i;
//			//				mini_TKdist = tmp_TKdist;
//			//			}
//			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
//			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//			//	//歸還舊NSLBremainSectorTable
//			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
//			//	//更新NSLB page位置
//			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
//			//	//更新NSLBremainSectorTable資訊
//			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
//			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//			//}
//
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
//			NSLB->write_count++;
//
//			N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//		}
//		else if (N->blocks[block_no].pages[page_no].valid == 1){//新寫入
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++; NSLB->writedPageNum++;
//			NSLB->write_count++;
//			//找尋可以寫入的NSLBremainSector
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
//			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
//			if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			}
//			else{
//				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//				for (i = 0; i < N->NSLB_tracks; i++){
//					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//					if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
//						NSLBfreeTrack = i;
//						mini_TKdist = tmp_TKdist;
//					}
//				}assert(NSLBfreeTrack != preNSLBfreeTrack);
//			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks); assert(N->NSLBremainSectorTable[NSLBfreeTrack]>0);
//			//在DB上標記為寫入NSLB的page
//			N->blocks[block_no].pages[page_no].valid = 2;
//			//把寫入的NSLB track記錄到原DB的sector上
//			assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack]); assert(N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);
//			N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[NSLBfreeTrack] - 1;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
//			assert(0 <= N->blocks[block_no].pages[page_no].sector && N->blocks[block_no].pages[page_no].sector < NSLB->NSLBpage_num);
//			assert(N->blocks[block_no].pages[page_no].sector / NC->trackSizePage == NSLBfreeTrack);
//			//更新NSLBremainSectorTable資訊
//			assert(N->NSLBremainSectorTable[NSLBfreeTrack]>0);
//			N->NSLBremainSectorTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] < NC->trackSizePage);
//			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//
//			N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//		}
//
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0){ break; }
//	}
//}

//void FASTwriteSectorNSLBPM3(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
//	DWORD cur_8subband = -1, prev_8subband = -1;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	DWORD page_no = 0, block_no = 0;
//	DWORD NSLBfreeTrack = 0;
//	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
//	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;
//
//	DWORD go_OldSection_NSLBtrack = 0;
//	DWORD curSection = offsetSector / N->SubTrack;
//	DWORD cur_Section_Update = 0;
//
//	DWORD goRMW = 0;
//	DWORD oldSection = -1;
//	DWORD goDB = 0;
//
//	DWORD GT_StartIndex = 0, GT_EndIndex = 0;
//	DWORD preSLBfreeTrack = 0;
//	DWORD SLBfreeTrack = 0;
//	//找尋可以寫入的NSLBremainSector
//	/*NSLBfreeTrack = -1;
//	for (i = 0; i < N->NSLB_tracks; i++){ if (N->NSLBremainSectorTable[i]>0){ NSLBfreeTrack = i; break; } }
//	assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);*/
//	N->cnt++;
//	while (1) {
//		//確認merge條件
//		assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//		if (NSLB->writedPageNum == NSLB->NSLBpage_num){
//			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
//			//FASTmergeRWNSLBPM(simPtr); 
//
//			N->PM_GTsector = 0; N->PM_Tracks_Allocated2Use_Condition = 0;
//			FASTmergeRWNSLBPM3(simPtr);
//
//			//FASTmergeRWNSLBPM已標示那些GT寫入NSLB，清出GT需要的NSLB空間
//			while (NSLB->writedPageNum + N->PM_GTsector / 2 >= NSLB->NSLBpage_num){
//				FASTmergeNormalNSLB3(simPtr);
//			}assert(NSLB->writedPageNum + N->PM_GTsector / 2 < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
//			//assert(NSLB->writedPageNum < NSLB->NSLBpage_num);// assert(N->SLBexhaustedTrack < N->SLB_tracks);
//
//			//當NSLB轉SLB的時候，先寫fNSLB或SLB都沒差
//			if (N->PM_Sectors_Use < 2 * N->PM_Rate_SLBdefault * NSLB->NSLBpage_num / 100 - 1 && N->PartialMerge_Threshold <= 32){//N->PM_Rate <= 5 1
//				if (N->PM_GTsector > 0 && N->PM_Tracks_Allocated2Use_Condition == 1){//有做PM //表示剛才有做NSLB轉SLB
//					//寫入新配的SLB
//					preSLBfreeTrack = N->preSLB_WriteSector / NC->trackSizePage; assert(0 <= preSLBfreeTrack && preSLBfreeTrack < N->SLB_tracks); //紀錄NSLB上之前寫到的track
//					for (i = 0; i < N->PM_GTsector/2; i++){//直接送入預留的SLB 
//						if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){
//							SLBfreeTrack = -1; mini_TKdist = N->SLB_tracks; tmp_TKdist = N->SLB_tracks;//N->SLB_tracks
//							for (j = 0; j < N->SLB_tracks; j++){
//								tmp_TKdist = abs(j - preSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->SLB_tracks);
//								if (N->SLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//									SLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//								}
//							}assert(SLBfreeTrack != preSLBfreeTrack); assert(0 <= SLBfreeTrack && SLBfreeTrack < N->SLB_tracks);
//							preSLBfreeTrack = SLBfreeTrack; assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//						}
//						BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
//						PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
//						assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1);
//						assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
//						assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
//						N->blocks[BlockNo].pages[PageNo].valid = 0;
//						//GT放入SLB
//						assert(0 < N->SLBremainSectorTable[preSLBfreeTrack]); assert(N->SLBremainSectorTable[preSLBfreeTrack] <= NC->trackSizePage);
//						N->blocks[BlockNo].pages[PageNo].sector = preSLBfreeTrack*NC->trackSizePage + N->SLBremainSectorTable[preSLBfreeTrack] - 1;
//						assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < SLB->SLBpage_num);
//						assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preSLBfreeTrack);
//						SLB->writedPageNum++;
//						//更新SLBremainSectorTable資訊
//						assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//						N->SLBremainSectorTable[preSLBfreeTrack]--; assert(0 <= N->SLBremainSectorTable[preSLBfreeTrack] && N->SLBremainSectorTable[preSLBfreeTrack] < NC->trackSizePage);
//						if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){ N->SLBexhaustedTrack++; }
//					}
//					//這些GT寫入LB的統計在merge前做完
//					N->PM_GTsector = 0;
//				}
//			}
//			else{
//				if (N->PM_GTsector > 0 && N->PM_Tracks_Allocated2Use_Condition == 1){//有做PM //表示剛才有做NSLB轉SLB
//					//把NSLB被轉成SLB的紀錄起來，之後才能歸還
//					preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
//					mini_TKdist = 0; tmp_TKdist = 0;//N->NSLB_tracks
//					for (i = 0; i < N->NSLB_tracks; i++){//挑最遠的NSLBtrack轉成SLBtrack
//						tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//						if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist > mini_TKdist){
//							NSLBfreeTrack = i; mini_TKdist = tmp_TKdist;
//						}
//					}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//					preNSLBfreeTrack = NSLBfreeTrack;
//					for (i = 0; i < N->PM_GTsector / 2; i++){//1個NSLNtrack是2個SLBtrack，所以剛剛只清理N->PM_GTsector / 2空間
//						if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){//當之前挑到的NSLBtrack用完，再挑一條離最近的轉SLBtrack
//							NSLBfreeTrack = -1; mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;//N->NSLB_tracks
//							for (j = 0; j < N->NSLB_tracks; j++){
//								tmp_TKdist = abs(j - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//								if (N->NSLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//									NSLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//								}
//							}assert(NSLBfreeTrack != preNSLBfreeTrack); assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//							preNSLBfreeTrack = NSLBfreeTrack; assert(N->NSLBremainSectorTable[preNSLBfreeTrack]>0);
//						}
//						BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
//						PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
//						assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1);
//						assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
//						assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
//						N->blocks[BlockNo].pages[PageNo].valid = 3;
//						//GT放入NSLB
//						assert(0 < N->NSLBremainSectorTable[preNSLBfreeTrack]); assert(N->NSLBremainSectorTable[preNSLBfreeTrack] <= NC->trackSizePage);
//						N->blocks[BlockNo].pages[PageNo].sector = preNSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[preNSLBfreeTrack] - 1;
//						assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < NSLB->NSLBpage_num);
//						assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preNSLBfreeTrack);
//						NSLB->writedPageNum++;
//						//更新NSLBremainSectorTable資訊
//						assert(N->NSLBremainSectorTable[preNSLBfreeTrack]>0);
//						N->NSLBremainSectorTable[preNSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[preNSLBfreeTrack] && N->NSLBremainSectorTable[preNSLBfreeTrack] < NC->trackSizePage);
//						if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//					}
//				}
//			}
//
//			if (N->PM_GTsector > 0 && N->PM_Tracks_Allocated2Use_Condition == 1){//有做PM //表示剛才有做NSLB轉SLB
//				//寫入新配的SLB
//				preSLBfreeTrack = N->preSLB_WriteSector / NC->trackSizePage; assert(0 <= preSLBfreeTrack && preSLBfreeTrack < N->SLB_tracks); //紀錄NSLB上之前寫到的track
//
//				//紀錄PMGT寫到哪一個NSLB上
//				if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){
//					SLBfreeTrack = -1; mini_TKdist = N->SLB_tracks; tmp_TKdist = N->SLB_tracks;//N->SLB_tracks
//					for (j = 0; j < N->SLB_tracks; j++){
//						tmp_TKdist = abs(j - preSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->SLB_tracks);
//						if (N->SLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//							SLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//						}
//					}assert(SLBfreeTrack != preSLBfreeTrack); assert(0 <= SLBfreeTrack && SLBfreeTrack < N->SLB_tracks);
//					preSLBfreeTrack = SLBfreeTrack; assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//				}
//				//計算寫入GT的seek
//				N->PMGT_NSLBaddress[N->PMGT_Signal] = NC->LsizeSector + preSLBfreeTrack*NC->trackSizePage;
//				N->PMGT_Signal++; assert(N->PMGT_Signal <= NC->trackSizePage);
//
//				for (i = N->PM_GTsector / 2; i < N->PM_GTsector; i++){//另一半送入SLB // N->PM_GTsector / 2
//					if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){
//						SLBfreeTrack = -1; mini_TKdist = N->SLB_tracks; tmp_TKdist = N->SLB_tracks;//N->SLB_tracks
//						for (j = 0; j < N->SLB_tracks; j++){
//							tmp_TKdist = abs(j - preSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->SLB_tracks);
//							if (N->SLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
//								SLBfreeTrack = j; mini_TKdist = tmp_TKdist;
//							}
//						}assert(SLBfreeTrack != preSLBfreeTrack); assert(0 <= SLBfreeTrack && SLBfreeTrack < N->SLB_tracks);
//						preSLBfreeTrack = SLBfreeTrack; assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//					}
//					BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
//					PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
//					assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1);
//					assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
//					assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
//					N->blocks[BlockNo].pages[PageNo].valid = 0;
//					//GT放入SLB
//					assert(0 < N->SLBremainSectorTable[preSLBfreeTrack]); assert(N->SLBremainSectorTable[preSLBfreeTrack] <= NC->trackSizePage);
//					N->blocks[BlockNo].pages[PageNo].sector = preSLBfreeTrack*NC->trackSizePage + N->SLBremainSectorTable[preSLBfreeTrack] - 1;
//					assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < SLB->SLBpage_num);
//					assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preSLBfreeTrack);
//					SLB->writedPageNum++;
//					//更新SLBremainSectorTable資訊
//					assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
//					N->SLBremainSectorTable[preSLBfreeTrack]--; assert(0 <= N->SLBremainSectorTable[preSLBfreeTrack] && N->SLBremainSectorTable[preSLBfreeTrack] < NC->trackSizePage);
//					if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){ N->SLBexhaustedTrack++; }
//				}
//
//				//這些GT寫入LB的統計在merge前做完
//				N->PM_GTsector = 0;
//			}
//
//		}//當前空間已寫滿
//		assert(NSLB->writedPageNum < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
//		//一般寫入流程
//		//新寫入sector的DB更新BPLRU時間
//		LBA = offsetSector / NC->blockSizeSector; assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
//		SectorTableNo = offsetSector / NC->pageSizeSector; assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
//		assert(N->BPLRU[LBA] <= N->cnt);
//		N->BPLRU[LBA] = N->cnt;
//		//確認該sector的L2P mapping table正確
//		block_no = getblock(simPtr, SectorTableNo);
//		page_no = getpage(simPtr, SectorTableNo);
//		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizePage);
//		assert(SectorTableNo / NC->blockSizeSector == block_no); assert(SectorTableNo % NC->blockSizeSector == page_no);
//		//新寫入或第二次以上寫入的情況分別處理
//		assert(0 <= N->blocks[block_no].pages[page_no].valid && N->blocks[block_no].pages[page_no].valid <= 3);
//		//|| N->blocks[block_no].pages[page_no].valid == 3
//		if (N->blocks[block_no].pages[page_no].valid == 2){//第二次以上寫入 //GT上的section有資料寫入
//			//NSLB舊page找尋最佳寫入位置
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
//			oldNSLBfreeTrack = N->blocks[block_no].pages[page_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
//
//			//把舊section寫到cur track(preNSLBfreeTrack)
//			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
//			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			//	}else{
//			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//			//		for (i = 0; i < N->NSLB_tracks; i++){
//			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
//			//				NSLBfreeTrack = i;
//			//				mini_TKdist = tmp_TKdist;
//			//			}
//			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
//			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//			//	//歸還舊NSLBremainSectorTable
//			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
//			//	//更新NSLB page位置
//			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
//			//	//更新NSLBremainSectorTable資訊
//			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
//			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//			//}
//
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
//			NSLB->write_count++;
//
//			N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//		}
//		else if (N->blocks[block_no].pages[page_no].valid == 1){//新寫入
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++; NSLB->writedPageNum++;
//			NSLB->write_count++;
//			//找尋可以寫入的NSLBremainSector
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
//			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
//			if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			}
//			else{
//				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//				for (i = 0; i < N->NSLB_tracks; i++){
//					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//					if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
//						NSLBfreeTrack = i;
//						mini_TKdist = tmp_TKdist;
//					}
//				}assert(NSLBfreeTrack != preNSLBfreeTrack);
//			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks); assert(N->NSLBremainSectorTable[NSLBfreeTrack]>0);
//			//在DB上標記為寫入NSLB的page
//			N->blocks[block_no].pages[page_no].valid = 2;
//			//把寫入的NSLB track記錄到原DB的sector上
//			assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack]); assert(N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);
//			N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[NSLBfreeTrack] - 1;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
//			assert(0 <= N->blocks[block_no].pages[page_no].sector && N->blocks[block_no].pages[page_no].sector < NSLB->NSLBpage_num);
//			assert(N->blocks[block_no].pages[page_no].sector / NC->trackSizePage == NSLBfreeTrack);
//			//更新NSLBremainSectorTable資訊
//			assert(N->NSLBremainSectorTable[NSLBfreeTrack]>0);
//			N->NSLBremainSectorTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] < NC->trackSizePage);
//			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//
//			N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//		}
//
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0){ break; }
//	}
//}






//seq2NSLB
void LongWriteRequest2LB(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	DWORD i = 0, j = 0;
	//section mapping
	DWORD start_section = -1;
	DWORD Section_Update = 0;
#ifdef Time_Overhead
	DWORD WriteBuff_Size = 0;
	DWORD *WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE + lenSector, sizeof(DWORD));
	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
#endif
	//sector LRU + hit
	DWORD skip = 0;
	DWORD *tmp_NSLB_RAM = NULL;
	DWORD tmp_NSLB_RAM_request = 0;
	DWORD LRU_band = -1;
	DWORD old_NSLB_RAM_request = 0;

	NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	DWORD tmp_NSLB_RAM_request2 = 0;
	DWORD len = lenSector;

	assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE + lenSector, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
	tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
	LRU_band = offsetSector / NC->blockSizeSector; assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
	for (i = 0; i < N->NSLB_RAM_request; i++){
		if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
			assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
			tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
			tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
			assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
			N->NSLB_RAM[i] = -1;//原RAM失效
		}
	}
	//if (tmp_NSLB_RAM_request == 0){ free(tmp_NSLB_RAM); return; } //
	assert(0 <= tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
	tmp_NSLB_RAM_request2 = tmp_NSLB_RAM_request;
	SectorTableNo = offsetSector;
	while (len != 0){
		tmp_NSLB_RAM[tmp_NSLB_RAM_request2] = SectorTableNo;
		N->RAM_Valid[SectorTableNo] = 1;
		//if (SectorTableNo == 547553576){ printf("%lu %lu, ", tmp_NSLB_RAM[tmp_NSLB_RAM_request2], N->RAM_Valid[SectorTableNo]); }
		tmp_NSLB_RAM_request2++; assert(tmp_NSLB_RAM_request2 <= N->NSLB_RAM_SIZE + lenSector);
		SectorTableNo++; len--;
	}
	//tmp_NSLB_RAM用section mapping寫入NSLB
	qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request2, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
	assert(tmp_NSLB_RAM[0] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
	start_section = tmp_NSLB_RAM[0] / N->SubTrack;
	/*Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[0] % N->SubTrack);*/
#ifdef Time_Overhead
	WriteBuff_Size = 0;
#endif
	for (i = 1; i < tmp_NSLB_RAM_request2; i++){
		assert(tmp_NSLB_RAM[i] != -1); //assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); 
		N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
		if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
			//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//正式寫前面section的
#ifdef Time_Overhead
			SectorTableNo = start_section*N->SubTrack;
			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); //N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
			if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE + lenSector); }//在DB
			else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE + lenSector); }//在NSLB   % (N->NSLB_DataTracks*NC->trackSizePage))
			else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE + lenSector); }//GT //     % (N->PM_Tracks*NC->trackSizePage))
#endif
			FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
			start_section = tmp_NSLB_RAM[i] / N->SubTrack;
			//Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);
		}
		else{ /*Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);*/ }
	}
	//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//寫最後一個section的
#ifdef Time_Overhead
	SectorTableNo = start_section*N->SubTrack;
	block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); //N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
	if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE + lenSector); }//在DB
	else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE + lenSector); }//在NSLB    % (N->NSLB_DataTracks*NC->trackSizePage))
	else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE + lenSector); }//GT //  NC->LsizeSector + (N->NSLB_tracks - 1)*NC->trackSizePage
#endif
	FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
#ifdef Time_Overhead
	assert(0 < WriteBuff_Size); assert(WriteBuff_Size <= N->NSLB_RAM_SIZE + lenSector);
	qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
	for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); }
	assert(0 <= WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
	Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
	for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
		if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){
			//seek
			assert(0 <= WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
			Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
		}
	}
#endif
	//統計RAM Hit吸收的寫入
	N->RAM_WriteOut += tmp_NSLB_RAM_request;
	//更新NSLB_RAM_request資訊
	old_NSLB_RAM_request = N->NSLB_RAM_request;
	N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	//整理NSLB_RAM，清掉-1的資料
	j = 0;
	for (i = 0; i < old_NSLB_RAM_request; i++){
		if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
	}assert(j == N->NSLB_RAM_request);
	for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
	free(tmp_NSLB_RAM);
#ifdef Time_Overhead
	free(WriteBuff);
#endif
}

void FASTwriteSectorNSLBPMRAM3_WriteAllOut(sim *simPtr)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	DWORD i = 0, j = 0;
	DWORD start_section = 0, Section_Update = 0;
	//sector LRU + hit
	DWORD skip = 0;
	DWORD *tmp_NSLB_RAM = NULL;
	DWORD tmp_NSLB_RAM_request = 0;
	DWORD LRU_band = -1;
	DWORD old_NSLB_RAM_request = 0;
	if (N->NSLB_RAM_request > 0){

		/*qsort(N->NSLB_RAM, N->NSLB_RAM_request, sizeof(DWORD), compare);
		assert(N->NSLB_RAM[0] != -1); assert(N->RAM_Valid[N->NSLB_RAM[0]] == 1); N->RAM_Valid[N->NSLB_RAM[0]] = 0;
		start_section = N->NSLB_RAM[0] / N->SubTrack;
		N->NSLB_RAM[0] = -1;
		for (i = 1; i < N->NSLB_RAM_request; i++){
		assert(N->NSLB_RAM[i] != -1); assert(N->RAM_Valid[N->NSLB_RAM[i]] == 1); N->RAM_Valid[N->NSLB_RAM[i]] = 0;
		if (N->NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
		FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
		start_section = N->NSLB_RAM[i] / N->SubTrack;
		}
		N->NSLB_RAM[i] = -1;
		}
		FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
		N->NSLB_RAM_request = 0;*/

#ifdef Time_Overhead
		DWORD WriteBuff_Size = 0;
		DWORD *WriteBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
		DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
#endif
		while (N->NSLB_RAM_request > 0){
			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
			for (i = 0; i < N->NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
					N->NSLB_RAM[i] = -1;//原RAM失效
				}
			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
			//tmp_NSLB_RAM用section mapping寫入NSLB
			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
			assert(tmp_NSLB_RAM[0] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
			start_section = tmp_NSLB_RAM[0] / N->SubTrack;
			/*Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[0] % N->SubTrack);*/
			for (i = 1; i < tmp_NSLB_RAM_request; i++){
				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
					//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//正式寫前面section的
#ifdef Time_Overhead
					SectorTableNo = start_section*N->SubTrack;
					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); // N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
					if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
					else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % ((N->NSLB_tracks - N->PM_Tracks) * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB   % (N->NSLB_DataTracks*NC->trackSizePage))
					else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % (N->PM_Tracks * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //   % (N->PM_Tracks*NC->trackSizePage))
#endif
					FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
					//Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);
				}
				else{ /*Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);*/ }
			}
			//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//寫最後一個section的
#ifdef Time_Overhead
			SectorTableNo = start_section*N->SubTrack;
			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); // N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
			if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
			else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % ((N->NSLB_tracks - N->PM_Tracks) * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB   % (N->NSLB_DataTracks*NC->trackSizePage))
			else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % (N->PM_Tracks * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //    % (N->PM_Tracks*NC->trackSizePage))
#endif
			FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
			//統計RAM Hit吸收的寫入
			N->RAM_WriteOut += tmp_NSLB_RAM_request;

			//更新NSLB_RAM_request資訊
			old_NSLB_RAM_request = N->NSLB_RAM_request;
			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
			//整理NSLB_RAM，清掉-1的資料
			j = 0;
			for (i = 0; i < old_NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
			}assert(j == N->NSLB_RAM_request);
			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
			free(tmp_NSLB_RAM);
		}assert(N->NSLB_RAM_request == 0);
#ifdef Time_Overhead
		assert(0 < WriteBuff_Size);  assert(WriteBuff_Size <= N->NSLB_RAM_SIZE);
		qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
		for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); }
		assert(0 <= WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
		Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
		for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
			if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){
				//seek
				assert(0 <= WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
				Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
			}
		}
#endif
	}
}


void FASTwriteSectorNSLBPM3(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD page_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;

	DWORD go_OldSection_NSLBtrack = 0;
	DWORD curSection = offsetSector / N->SubTrack;
	DWORD cur_Section_Update = 0;

	DWORD goRMW = 0;
	DWORD oldSection = -1;
	DWORD goDB = 0;

	DWORD GT_StartIndex = 0, GT_EndIndex = 0;
	DWORD preSLBfreeTrack = 0;
	DWORD SLBfreeTrack = 0;
	//找尋可以寫入的NSLBremainSector
	/*NSLBfreeTrack = -1;
	for (i = 0; i < N->NSLB_tracks; i++){ if (N->NSLBremainSectorTable[i]>0){ NSLBfreeTrack = i; break; } }
	assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);*/
	N->cnt++; N->PM_timer++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
		if (NSLB->writedPageNum == NSLB->NSLBpage_num){
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			//FASTmergeRWNSLBPM(simPtr); 

			N->MergeCondition++;
			N->PM_GTsector = 0; N->PM_Tracks_Allocated2Use_Condition = 0;
			FASTmergeRWNSLBPM3(simPtr);

			//FASTmergeNormalNSLB3(simPtr);//SE

			////FASTmergeRWNSLBPM已標示那些GT寫入NSLB，清出GT需要的NSLB空間
			//while (NSLB->writedPageNum + N->PM_GTsector / 2 >= NSLB->NSLBpage_num){
			//	//printf("warning\n"); system("pause");
			//	FASTmergeNormalNSLB3(simPtr);
			//}assert(NSLB->writedPageNum + N->PM_GTsector / 2 < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
			////assert(NSLB->writedPageNum < NSLB->NSLBpage_num);// assert(N->SLBexhaustedTrack < N->SLB_tracks);

			if (N->PM_GTsector > 0){
				FASTmergeNormalNSLB3(simPtr);
				assert(NSLB->writedPageNum + N->PM_GTsector / 2 < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
				assert(N->PM_GTsector % NC->trackSizePage == 0);
			}

			if (N->PM_GTsector > 0 && N->PM_Tracks_Allocated2Use_Condition == 1){//有做PM //表示剛才有做NSLB轉SLB
				//把NSLB被轉成SLB的紀錄起來，之後才能歸還
				preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
				mini_TKdist = 0; tmp_TKdist = 0;//N->NSLB_tracks
				for (i = 0; i < N->NSLB_tracks; i++){//挑最遠的NSLBtrack轉成SLBtrack
					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
					if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist > mini_TKdist){
						NSLBfreeTrack = i; mini_TKdist = tmp_TKdist;
					}
				}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
				preNSLBfreeTrack = NSLBfreeTrack;
				for (i = 0; i < N->PM_GTsector / 2; i++){//1個NSLBtrack是2個SLBtrack，所以剛剛只清理N->PM_GTsector / 2空間 
					if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){//當之前挑到的NSLBtrack用完，再挑一條離最近的轉SLBtrack
						NSLBfreeTrack = -1; mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;//N->NSLB_tracks
						for (j = 0; j < N->NSLB_tracks; j++){
							tmp_TKdist = abs(j - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
							if (N->NSLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
								NSLBfreeTrack = j; mini_TKdist = tmp_TKdist;
							}
						}assert(NSLBfreeTrack != preNSLBfreeTrack); assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
						preNSLBfreeTrack = NSLBfreeTrack; assert(N->NSLBremainSectorTable[preNSLBfreeTrack]>0);
					}
					BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
					PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
					assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1);
					assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
					assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
					N->blocks[BlockNo].pages[PageNo].valid = 3;
					//GT放入NSLB
					assert(0 < N->NSLBremainSectorTable[preNSLBfreeTrack]); assert(N->NSLBremainSectorTable[preNSLBfreeTrack] <= NC->trackSizePage);
					N->blocks[BlockNo].pages[PageNo].sector = preNSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[preNSLBfreeTrack] - 1;
					assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < NSLB->NSLBpage_num);
					assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preNSLBfreeTrack);
					NSLB->writedPageNum++;
					//更新NSLBremainSectorTable資訊
					assert(N->NSLBremainSectorTable[preNSLBfreeTrack]>0);
					N->NSLBremainSectorTable[preNSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[preNSLBfreeTrack] && N->NSLBremainSectorTable[preNSLBfreeTrack] < NC->trackSizePage);
					if (N->NSLBremainSectorTable[preNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
				}

				//寫入新配的SLB
				preSLBfreeTrack = N->preSLB_WriteSector / NC->trackSizePage; assert(0 <= preSLBfreeTrack && preSLBfreeTrack < N->SLB_tracks); //紀錄NSLB上之前寫到的track
				for (i = N->PM_GTsector / 2; i < N->PM_GTsector; i++){//直接送入預留的SLB   
					if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){
						SLBfreeTrack = -1; mini_TKdist = N->SLB_tracks; tmp_TKdist = N->SLB_tracks;//N->SLB_tracks
						for (j = 0; j < N->SLB_tracks; j++){
							tmp_TKdist = abs(j - preSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->SLB_tracks);
							if (N->SLBremainSectorTable[j] > 0 && tmp_TKdist < mini_TKdist){
								SLBfreeTrack = j; mini_TKdist = tmp_TKdist;
							}
						}assert(SLBfreeTrack != preSLBfreeTrack); assert(0 <= SLBfreeTrack && SLBfreeTrack < N->SLB_tracks);
						preSLBfreeTrack = SLBfreeTrack; assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
					}
					BlockNo = N->PMCandidate[i] / NC->blockSizeSector; assert(0 <= BlockNo && BlockNo < NC->LsizeBlock);
					PageNo = N->PMCandidate[i] % NC->blockSizeSector; assert(0 <= PageNo && PageNo < NC->blockSizePage);
					assert(N->blocks[BlockNo].GuardTrackIndex == PageNo / NC->trackSizePage); assert(N->blocks[BlockNo].Cut == 1);
					assert(N->blocks[BlockNo].pages[PageNo].valid == 1);
					assert(N->blocks[BlockNo].pages[PageNo].sector == BlockNo*NC->blockSizeSector + PageNo);
					N->blocks[BlockNo].pages[PageNo].valid = 0;
					//GT放入SLB
					assert(0 < N->SLBremainSectorTable[preSLBfreeTrack]); assert(N->SLBremainSectorTable[preSLBfreeTrack] <= NC->trackSizePage);
					N->blocks[BlockNo].pages[PageNo].sector = preSLBfreeTrack*NC->trackSizePage + N->SLBremainSectorTable[preSLBfreeTrack] - 1;
					assert(0 <= N->blocks[BlockNo].pages[PageNo].sector && N->blocks[BlockNo].pages[PageNo].sector < SLB->SLBpage_num);
					assert(N->blocks[BlockNo].pages[PageNo].sector / NC->trackSizePage == preSLBfreeTrack);
					SLB->writedPageNum++;
					//更新SLBremainSectorTable資訊
					assert(N->SLBremainSectorTable[preSLBfreeTrack]>0);
					N->SLBremainSectorTable[preSLBfreeTrack]--; assert(0 <= N->SLBremainSectorTable[preSLBfreeTrack] && N->SLBremainSectorTable[preSLBfreeTrack] < NC->trackSizePage);
					if (N->SLBremainSectorTable[preSLBfreeTrack] == 0){ N->SLBexhaustedTrack++; }
				}

				//這些GT寫入LB的統計在merge前做完
				N->PM_GTsector = 0;
			}

		}//當前空間已寫滿
		assert(NSLB->writedPageNum < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//一般寫入流程
		//新寫入sector的DB更新BPLRU時間
		LBA = offsetSector / NC->blockSizeSector; assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector / NC->pageSizeSector; assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;
		//確認該sector的L2P mapping table正確
		block_no = getblock(simPtr, SectorTableNo);
		page_no = getpage(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizePage);
		assert(SectorTableNo / NC->blockSizeSector == block_no); assert(SectorTableNo % NC->blockSizeSector == page_no);
		//新寫入或第二次以上寫入的情況分別處理
		assert(0 <= N->blocks[block_no].pages[page_no].valid && N->blocks[block_no].pages[page_no].valid <= 3);
		//assert(1 <= N->blocks[block_no].pages[page_no].valid && N->blocks[block_no].pages[page_no].valid <= 2);
		//|| N->blocks[block_no].pages[page_no].valid == 3
		if (N->blocks[block_no].pages[page_no].valid == 2){//第二次以上寫入 //GT上的section有資料寫入
			//NSLB舊page找尋最佳寫入位置
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
			oldNSLBfreeTrack = N->blocks[block_no].pages[page_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);

			//把舊section寫到cur track(preNSLBfreeTrack)
			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			//	}else{
			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
			//		for (i = 0; i < N->NSLB_tracks; i++){
			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
			//				NSLBfreeTrack = i;
			//				mini_TKdist = tmp_TKdist;
			//			}
			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//	//歸還舊NSLBremainSectorTable
			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
			//	//更新NSLB page位置
			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
			//	//更新NSLBremainSectorTable資訊
			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//}

			//寫入量統計
			NSLB->pageRead++; NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count++;
			//N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
		}
		else if (N->blocks[block_no].pages[page_no].valid == 1){//新寫入
			//寫入量統計
			NSLB->pageRead++; NSLB->pageWrite++; NSLB->writedPageNum++;
			NSLB->write_count++;
			//找尋可以寫入的NSLBremainSector
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			}
			else{
				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
				for (i = 0; i < N->NSLB_tracks; i++){
					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
					if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
						NSLBfreeTrack = i;
						mini_TKdist = tmp_TKdist;
					}
				}assert(NSLBfreeTrack != preNSLBfreeTrack);
#ifdef Time_Overhead
				//printf("%lu ", N->NSLBremainSectorTable[NSLBfreeTrack]);
				if (N->WriteBuff_Size > 0){
					NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
				}
				N->SLBtrackChange++;
				N->preNSLB_WriteSector = NSLBfreeTrack * NC->trackSizePage;
#endif
			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks); assert(N->NSLBremainSectorTable[NSLBfreeTrack]>0);
#ifdef Time_Overhead
			Calc_TimeOverhead2(simPtr, (NC->LsizeSector + NSLBfreeTrack * 2 * NC->trackSizePage % ((N->NSLB_tracks - N->PM_Tracks / 2)*NC->trackSizePage)) / NC->trackSizePage, 2); //  / NC->CylinderSizeTrack
#endif
			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].pages[page_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			assert(0 < N->NSLBremainSectorTable[NSLBfreeTrack]); assert(N->NSLBremainSectorTable[NSLBfreeTrack] <= NC->trackSizePage);
			N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[NSLBfreeTrack] - 1;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].pages[page_no].sector && N->blocks[block_no].pages[page_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].pages[page_no].sector / NC->trackSizePage == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			assert(N->NSLBremainSectorTable[NSLBfreeTrack]>0);
			N->NSLBremainSectorTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] < NC->trackSizePage);
			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
		}

		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0){ break; }
	}
}
void FASTwriteSectorNSLBPMRAM3_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	DWORD i = 0, j = 0;
	//section mapping
	DWORD Section_Update = 0;
	//#ifdef NSLB_RAM_FastSectLRU_HIT
	//	//sector LRU + hit
	//	DWORD skip = 0;
	//	DWORD *tmp_NSLB_RAM = NULL;
	//	DWORD tmp_NSLB_RAM_request = 0;
	//	DWORD LRU_band = -1;
	//	DWORD old_NSLB_RAM_request = 0;
	//	if (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
	//		while (N->NSLB_RAM_request + lenSector > N->NSLB_RAM_SIZE){
	//			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	//			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
	//			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
	//			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
	//			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
	//			for (i = 0; i < N->NSLB_RAM_request; i++){
	//				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
	//					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
	//					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
	//					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
	//					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
	//					N->NSLB_RAM[i] = -1;//原RAM失效
	//				}
	//			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
	//			//tmp_NSLB_RAM用section mapping寫入NSLB
	//			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
	//			assert(tmp_NSLB_RAM[0] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[0]] == 1); N->RAM_Valid[tmp_NSLB_RAM[0]] = 0;
	//			start_section = tmp_NSLB_RAM[0] / N->SubTrack;
	//
	//			//#ifdef Time_Overhead
	//			//			WriteBuff_Size = 0;
	//			//			N->PMGT_Signal = 0;//有PM的信號，用來處理PM的seek time
	//			//#endif
	//			//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
	//			//				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
	//			//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
	//			//#ifdef Time_Overhead
	//			//					SectorTableNo = start_section*N->SubTrack;
	//			//					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); //N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
	//			//					if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
	//			//					else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % ((N->NSLB_tracks - N->PM_Tracks) * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB   % (N->NSLB_DataTracks*NC->trackSizePage))
	//			//					else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % (N->PM_Tracks * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //     % (N->PM_Tracks*NC->trackSizePage))
	//			//#endif
	//			//					FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
	//			//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
	//			//				}
	//			//			}
	//			//#ifdef Time_Overhead
	//			//			SectorTableNo = start_section*N->SubTrack;
	//			//			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); //N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
	//			//			if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
	//			//			else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % ((N->NSLB_tracks - N->PM_Tracks) * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB    % (N->NSLB_DataTracks*NC->trackSizePage))
	//			//			else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % (N->PM_Tracks * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //  NC->LsizeSector + (N->NSLB_tracks - 1)*NC->trackSizePage
	//			//#endif
	//			//			FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
	//			//#ifdef Time_Overhead
	//			//			assert(0 < WriteBuff_Size); assert(WriteBuff_Size <= N->NSLB_RAM_SIZE);
	//			//			if (N->PMGT_Signal > 0){//GT寫入NSLB的seek，加入在RMW的行列之後
	//			//				for (i = 0; i < N->PMGT_Signal; i++){
	//			//					WriteBuff[WriteBuff_Size] = N->PMGT_NSLBaddress[i]; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE + NC->trackSizePage);
	//			//				}
	//			//				N->PMGT_Signal = 0;
	//			//			}
	//			//			qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
	//			//			for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); }
	//			//			assert(0 <= WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
	//			//			Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
	//			//			for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
	//			//				if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){ //seek
	//			//					assert(0 <= WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
	//			//					Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
	//			//				}
	//			//			}
	//			//#endif
	//			//統計RAM Hit吸收的寫入
	//
	//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
	//				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
	//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
	//#ifdef Time_Overhead
	//					if (N->WriteBuff_Size == NC->trackSizePage / N->SubTrack){
	//						NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
	//					}
	//					SectorTableNo = start_section*N->SubTrack; block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
	//					assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= N->blocks[block_no].pages[page_no].valid && N->blocks[block_no].pages[page_no].valid <= 3);
	//					assert(0 <= N->WriteBuff_Size); assert(N->WriteBuff_Size < NC->trackSizePage / N->SubTrack);
	//					if (N->blocks[block_no].pages[page_no].valid == 1){ N->WriteBuff[N->WriteBuff_Size] = SectorTableNo; N->RDB++; }//在DB
	//					else if (N->blocks[block_no].pages[page_no].valid == 2){ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector * 2; N->RMW++; }//在NSLB
	//					else{ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; N->RMW++; }//在PM
	//					N->WriteBuff_Size++;
	//#endif
	//					FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
	//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
	//				}
	//			}
	//#ifdef Time_Overhead
	//			if (N->WriteBuff_Size == NC->trackSizePage / N->SubTrack){
	//				NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
	//			}
	//			SectorTableNo = start_section*N->SubTrack; block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
	//			assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= N->blocks[block_no].pages[page_no].valid && N->blocks[block_no].pages[page_no].valid <= 3);
	//			assert(0 <= N->WriteBuff_Size); assert(N->WriteBuff_Size < NC->trackSizePage / N->SubTrack);
	//			if (N->blocks[block_no].pages[page_no].valid == 1){ N->WriteBuff[N->WriteBuff_Size] = SectorTableNo; N->RDB++; }//在DB
	//			else if (N->blocks[block_no].pages[page_no].valid == 2){ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector * 2; N->RMW++; }//在NSLB
	//			else{ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; N->RMW++; }//在PM
	//			N->WriteBuff_Size++;
	//#endif
	//			FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
	//
	//			N->RAM_WriteOut += tmp_NSLB_RAM_request;
	//
	//			//更新NSLB_RAM_request資訊
	//			old_NSLB_RAM_request = N->NSLB_RAM_request;
	//			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
	//			//整理NSLB_RAM，清掉-1的資料
	//			j = 0;
	//			for (i = 0; i < old_NSLB_RAM_request; i++){
	//				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
	//			}assert(j == N->NSLB_RAM_request);
	//			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
	//			free(tmp_NSLB_RAM);
	//		}assert(N->NSLB_RAM_request + lenSector <= N->NSLB_RAM_SIZE);
	//	}
	//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
	//	while (1) {
	//		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	//		if (N->RAM_Valid[offsetSector] == 0){// skip == 0
	//			N->RAM_Valid[offsetSector] = 1;
	//			N->NSLB_RAM[N->NSLB_RAM_request] = offsetSector;
	//			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	//		}
	//		offsetSector += NC->pageSizeSector;
	//		lenSector -= NC->pageSizeSector;
	//		if (lenSector <= 0) break;
	//	}
	//#endif//WC2

#ifdef NSLB_RAM_FastSectLRU_HIT

	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
	DWORD start_section = 0, end_section = 0;
	start_section = offsetSector / NC->Section; end_section = 0;
	if ((offsetSector + lenSector) % NC->Section == 0){ end_section = (offsetSector + lenSector - 1) / NC->Section; }
	else{ end_section = (offsetSector + lenSector - 1) / NC->Section + 1; }
	assert(start_section <= end_section);

	DWORD *tmp_NSLB_RAM = NULL;
	DWORD tmp_NSLB_RAM_request = 0;
	DWORD LRU_band = -1;
	DWORD old_NSLB_RAM_request = 0;
	DWORD free_len = (end_section - start_section + 1);


	assert(free_len <= N->NSLB_RAM_SIZE);
	//if (free_len > N->NSLB_RAM_SIZE){
	//	NSLBPMRAM3_NoRAM(simPtr, offsetSector, lenSector);
	//	return;
	//}

	if (N->NSLB_RAM_request + free_len > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
		while (N->NSLB_RAM_request + free_len > N->NSLB_RAM_SIZE){
			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
			for (i = 0; i < N->NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
					N->NSLB_RAM[i] = -1;//原RAM失效
				}
			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
			//tmp_NSLB_RAM用section mapping寫入NSLB
			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量

			//#ifdef Time_Overhead
			//			WriteBuff_Size = 0;
			//			N->PMGT_Signal = 0;//有PM的信號，用來處理PM的seek time
			//#endif
			//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
			//				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
			//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
			//#ifdef Time_Overhead
			//					SectorTableNo = start_section*N->SubTrack;
			//					block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); //N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
			//					if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
			//					else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % ((N->NSLB_tracks - N->PM_Tracks) * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB   % (N->NSLB_DataTracks*NC->trackSizePage))
			//					else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % (N->PM_Tracks * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //     % (N->PM_Tracks*NC->trackSizePage))
			//#endif
			//					FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
			//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
			//				}
			//			}
			//#ifdef Time_Overhead
			//			SectorTableNo = start_section*N->SubTrack;
			//			block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo); //N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3
			//			if (N->blocks[block_no].pages[page_no].valid == 1){ WriteBuff[WriteBuff_Size] = SectorTableNo; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在DB
			//			else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % ((N->NSLB_tracks - N->PM_Tracks) * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在NSLB    % (N->NSLB_DataTracks*NC->trackSizePage))
			//			else if (N->blocks[block_no].pages[page_no].valid == 0 || N->blocks[block_no].pages[page_no].valid == 3){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector % (N->PM_Tracks * NC->trackSizePage); WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//GT //  NC->LsizeSector + (N->NSLB_tracks - 1)*NC->trackSizePage
			//#endif
			//			FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack);
			//#ifdef Time_Overhead
			//			assert(0 < WriteBuff_Size); assert(WriteBuff_Size <= N->NSLB_RAM_SIZE);
			//			if (N->PMGT_Signal > 0){//GT寫入NSLB的seek，加入在RMW的行列之後
			//				for (i = 0; i < N->PMGT_Signal; i++){
			//					WriteBuff[WriteBuff_Size] = N->PMGT_NSLBaddress[i]; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE + NC->trackSizePage);
			//				}
			//				N->PMGT_Signal = 0;
			//			}
			//			qsort(WriteBuff, WriteBuff_Size, sizeof(DWORD), compare);
			//			for (i = 0; i < WriteBuff_Size - 1; i++){ assert(WriteBuff[i] <= WriteBuff[i + 1]); }
			//			assert(0 <= WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
			//			Calc_TimeOverhead(simPtr, WriteBuff[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//電梯法回SLB讀資料
			//			for (i = 1; i < WriteBuff_Size; i++){//電梯法回SLB讀資料 //讀SLB消耗的rotation
			//				if (WriteBuff[i - 1] / NC->trackSizePage != WriteBuff[i] / NC->trackSizePage){ //seek
			//					assert(0 <= WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack && WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack < N->Cylinders);
			//					Calc_TimeOverhead(simPtr, WriteBuff[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
			//				}
			//			}
			//#endif
			//統計RAM Hit吸收的寫入

			for (i = 0; i < tmp_NSLB_RAM_request; i++){
				assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;//
				assert(tmp_NSLB_RAM[i] != -1);
				start_section = tmp_NSLB_RAM[i] / N->SubTrack;//更新strat_sector
#ifdef Time_Overhead
				if (N->WriteBuff_Size == NC->trackSizePage / N->SubTrack){
					NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
				}
				SectorTableNo = start_section*N->SubTrack; block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
				assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= N->blocks[block_no].pages[page_no].valid && N->blocks[block_no].pages[page_no].valid <= 3);
				assert(0 <= N->WriteBuff_Size); assert(N->WriteBuff_Size < NC->trackSizePage / N->SubTrack);
				if (N->blocks[block_no].pages[page_no].valid == 1){ N->WriteBuff[N->WriteBuff_Size] = SectorTableNo; N->RDB++; }//在DB
				else if (N->blocks[block_no].pages[page_no].valid == 2){ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + (N->blocks[block_no].pages[page_no].sector * 2) % ((N->NSLB_tracks - N->PM_Tracks / 2)*NC->trackSizePage); N->RMW++; }//在NSLB
				else if (N->blocks[block_no].pages[page_no].valid == 0){ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + (N->blocks[block_no].pages[page_no].sector) % (N->PM_Tracks*NC->trackSizePage); N->RMW++; }//在PM
				N->WriteBuff_Size++;
#endif
				FASTwriteSectorNSLBPM3(simPtr, start_section*N->SubTrack, N->SubTrack); //FASTwriteSectorNSLBpart23_3_RMW(simPtr, start_section*N->SubTrack, N->SubTrack);
				N->RAM_WriteOut += N->SubTrack;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;

#ifdef Time_Overhead
				if (N->MergeCondition > 0){
					assert(N->MergeCondition == 1); N->MergeCondition = 0;
					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + N->preNSLB_WriteSector) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
				}
#endif
			}

			//更新NSLB_RAM_request資訊
			old_NSLB_RAM_request = N->NSLB_RAM_request;
			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
			//整理NSLB_RAM，清掉-1的資料
			j = 0;
			for (i = 0; i < old_NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
			}assert(j == N->NSLB_RAM_request);
			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
			free(tmp_NSLB_RAM);
		}assert(N->NSLB_RAM_request + free_len <= N->NSLB_RAM_SIZE);
	}
	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
	//while (1) {
	//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	//	if (N->RAM_Valid[offsetSector] == 0){// skip == 0
	//		N->RAM_Valid[offsetSector] = 1;
	//		N->NSLB_RAM[N->NSLB_RAM_request] = offsetSector;
	//		N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	//	}
	//	offsetSector += NC->pageSizeSector;
	//	lenSector -= NC->pageSizeSector;
	//	if (lenSector <= 0) break;
	//}

	//request切成section送進RAM
	start_section = offsetSector / NC->Section; end_section = 0;
	if ((offsetSector + lenSector) % NC->Section == 0){ end_section = (offsetSector + lenSector - 1) / NC->Section; }
	else{ end_section = (offsetSector + lenSector - 1) / NC->Section + 1; }
	assert(start_section <= end_section);
	while (start_section <= end_section) {
		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
		if (N->RAM_Valid[start_section*N->SubTrack] == 0){// skip == 0
			N->RAM_Valid[start_section*N->SubTrack] = 1;
			N->NSLB_RAM[N->NSLB_RAM_request] = start_section*N->SubTrack;
			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			N->RAM_Miss++;
		}
		else{ N->RAM_Hit++; }
		/*N->NSLB_RAM[N->NSLB_RAM_request] = start_section*N->SubTrack;
		N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);*/

		start_section++;
	}
#endif//WC2


}
void NSLBPMRAM3_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	DWORD start_section = offsetSector / NC->Section, end_section = 0;
	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;

	if ((offsetSector + lenSector) % NC->Section != 0){ end_section = (offsetSector + lenSector) / NC->Section + 1; }
	else{ end_section = (offsetSector + lenSector) / NC->Section; }
	assert(start_section <= end_section);

#ifdef Time_Overhead
	N->WriteBuff_Size = 0;
#endif
	while (start_section <= end_section){
		if (N->RAM_Valid[start_section*N->SubTrack] == 0){ N->RAM_Miss++; }
		else{ N->RAM_Hit++; }
#ifdef Time_Overhead
		if (N->WriteBuff_Size == NC->trackSizePage / NC->Section){
			NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
			Calc_TimeOverhead2(simPtr, (NC->LsizeSector + N->preNSLB_WriteSector * 2) / NC->trackSizePage, 2);
		}
		SectorTableNo = start_section*N->SubTrack; block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= N->WriteBuff_Size); assert(N->WriteBuff_Size < NC->trackSizePage / N->SubTrack);
		if (N->blocks[block_no].pages[page_no].valid == 1){ N->WriteBuff[N->WriteBuff_Size] = SectorTableNo; N->RDB++; }//在DB
		else if (N->blocks[block_no].pages[page_no].valid == 2 || N->blocks[block_no].pages[page_no].valid == 3){ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector * 2; N->RMW++; }//在PM
		else{ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; N->RMW++; }//在NSLB
		N->WriteBuff_Size++;
#endif
		FASTwriteSectorNSLBPM3(simPtr, start_section * NC->Section, NC->Section);
		N->RAM_WriteOut += N->SubTrack; assert(N->SubTrack == 32);
		start_section++;
	}
#ifdef Time_Overhead
	if (N->WriteBuff_Size > 0){
		NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
		Calc_TimeOverhead2(simPtr, (NC->LsizeSector + N->preNSLB_WriteSector * 2) / NC->trackSizePage, 2);
	}
#endif
}




void FASTwriteSectorNSLBRAM2_RMW_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	DWORD i = 0, j = 0;
	//section mapping
	DWORD Section_Update = 0;
#ifdef Time_Overhead
	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;
#endif
#ifdef NSLB_RAM_FastSectLRU_HIT
	DWORD start_section = 0, end_section = 0;
	start_section = offsetSector / NC->Section; end_section = 0;
	if ((offsetSector + lenSector) % NC->Section == 0){ end_section = (offsetSector + lenSector - 1) / NC->Section; }
	else{ end_section = (offsetSector + lenSector - 1) / NC->Section + 1; }
	assert(start_section <= end_section);

	DWORD *tmp_NSLB_RAM = NULL;
	DWORD tmp_NSLB_RAM_request = 0;
	DWORD LRU_band = -1;
	DWORD old_NSLB_RAM_request = 0;
	DWORD free_len = (end_section - start_section + 1);

	assert(free_len <= N->NSLB_RAM_SIZE);
	/*if (free_len > N->NSLB_RAM_SIZE){
	NSLB_NoRAM(simPtr, offsetSector, lenSector);
	return;
	}*/

	if (N->NSLB_RAM_request + free_len > N->NSLB_RAM_SIZE){//RAM裝不下，挑sector LRU - band unit寫入NSLB
		while (N->NSLB_RAM_request + free_len > N->NSLB_RAM_SIZE){
			assert(0 <= N->NSLB_RAM_request); assert(N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			tmp_NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD)); assert(tmp_NSLB_RAM != NULL);
			tmp_NSLB_RAM_request = 0; assert(N->NSLB_RAM[0] != -1);
			LRU_band = N->NSLB_RAM[0] / NC->blockSizeSector;
			assert(0 <= N->NSLB_RAM[0] && N->NSLB_RAM[0] < NC->LsizeSector); assert(0 <= LRU_band && LRU_band < NC->LsizeBlock);
			for (i = 0; i < N->NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] / NC->blockSizeSector == LRU_band){
					assert(0 <= tmp_NSLB_RAM_request && tmp_NSLB_RAM_request < N->NSLB_RAM_request);
					tmp_NSLB_RAM[tmp_NSLB_RAM_request] = N->NSLB_RAM[i]; assert(0 <= tmp_NSLB_RAM[tmp_NSLB_RAM_request] && tmp_NSLB_RAM[tmp_NSLB_RAM_request] < NC->LsizeSector);//RAM的資料給tmp_NSLB_RAM做寫入
					tmp_NSLB_RAM_request++;//統計LRU Band有幾個sector write
					assert(0 < tmp_NSLB_RAM_request && tmp_NSLB_RAM_request <= N->NSLB_RAM_request);
					N->NSLB_RAM[i] = -1;//原RAM失效
				}
			}assert(0< tmp_NSLB_RAM_request); assert(tmp_NSLB_RAM_request <= N->NSLB_RAM_request);

			//tmp_NSLB_RAM用section mapping寫入NSLB
			qsort(tmp_NSLB_RAM, tmp_NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
			for (i = 0; i < tmp_NSLB_RAM_request; i++){
				assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;//
				assert(tmp_NSLB_RAM[i] != -1);
				start_section = tmp_NSLB_RAM[i] / N->SubTrack;//更新strat_sector

#ifdef Time_Overhead
				if (N->WriteBuff_Size == NC->trackSizePage / NC->Section){
					NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
				}
				SectorTableNo = start_section*N->SubTrack; block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
				assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= N->WriteBuff_Size); assert(N->WriteBuff_Size < NC->trackSizePage / N->SubTrack);
				assert(N->blocks[block_no].pages[page_no].valid == 1 || N->blocks[block_no].pages[page_no].valid == 2);
				if (N->blocks[block_no].pages[page_no].valid == 1){ N->WriteBuff[N->WriteBuff_Size] = SectorTableNo; N->RDB++; }//在DB
				else{ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector * 2; N->RMW++; }//在NSLB
				N->WriteBuff_Size++;
#endif
				FASTwriteSectorNSLBpart23_3_RMW(simPtr, start_section*N->SubTrack, N->SubTrack);
				N->RAM_WriteOut += N->SubTrack;//統計RAM Hit吸收的寫入 //N->RAM_WriteOut += tmp_NSLB_RAM_request;
#ifdef Time_Overhead
				if (N->MergeCondition > 0){
					assert(N->MergeCondition == 1); N->MergeCondition = 0;
					Calc_TimeOverhead2(simPtr, (NC->LsizeSector + N->preNSLB_WriteSector) / NC->trackSizePage, 0);//    / NC->CylinderSizeTrack
				}
#endif
			}
			//更新NSLB_RAM_request資訊
			old_NSLB_RAM_request = N->NSLB_RAM_request;
			N->NSLB_RAM_request -= tmp_NSLB_RAM_request; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
			//整理NSLB_RAM，清掉-1的資料
			j = 0;
			for (i = 0; i < old_NSLB_RAM_request; i++){
				if (N->NSLB_RAM[i] != -1){ tmp_NSLB_RAM[j] = N->NSLB_RAM[i]; j++; }//把NSLB RAM剩的valid sector撈出來
			}assert(j == N->NSLB_RAM_request);
			for (i = 0; i < N->NSLB_RAM_request; i++){ N->NSLB_RAM[i] = tmp_NSLB_RAM[i]; assert(tmp_NSLB_RAM[i] != -1);  assert(N->NSLB_RAM[i] != -1); }//把NSLB RAM撈出來的放回NSLB RAM
			free(tmp_NSLB_RAM);
		}assert(N->NSLB_RAM_request + free_len <= N->NSLB_RAM_SIZE);
	}
	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request < N->NSLB_RAM_SIZE);
	//while (1) {
	//	assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	//	if (N->RAM_Valid[offsetSector] == 0){// skip == 0
	//		N->RAM_Valid[offsetSector] = 1;
	//		N->NSLB_RAM[N->NSLB_RAM_request] = offsetSector;
	//		N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
	//	}
	//	offsetSector += NC->pageSizeSector;
	//	lenSector -= NC->pageSizeSector;
	//	if (lenSector <= 0) break;
	//}

	//request切成section送進RAM
	start_section = offsetSector / NC->Section; end_section = 0;
	if ((offsetSector + lenSector) % NC->Section == 0){ end_section = (offsetSector + lenSector - 1) / NC->Section; }
	else{ end_section = (offsetSector + lenSector - 1) / NC->Section + 1; }
	assert(start_section <= end_section);
	while (start_section <= end_section) {
		assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
		if (N->RAM_Valid[start_section*N->SubTrack] == 0){// skip == 0
			N->RAM_Valid[start_section*N->SubTrack] = 1;
			N->NSLB_RAM[N->NSLB_RAM_request] = start_section*N->SubTrack;
			N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);
			N->RAM_Miss++;
		}
		else{ N->RAM_Hit++; }
		/*N->NSLB_RAM[N->NSLB_RAM_request] = start_section*N->SubTrack;
		N->NSLB_RAM_request++; assert(0 <= N->NSLB_RAM_request && N->NSLB_RAM_request <= N->NSLB_RAM_SIZE);*/

		start_section++;
	}

#endif//WC2
}
void FASTwriteSectorNSLBpart23_3_RMW(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
	DWORD cur_8subband = -1, prev_8subband = -1;
	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	DWORD page_no = 0, block_no = 0;
	DWORD NSLBfreeTrack = 0;
	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;

	DWORD go_OldSection_NSLBtrack = 0;
	DWORD curSection = offsetSector / N->SubTrack;
	DWORD cur_Section_Update = 0;

	DWORD goRMW = 0;
	DWORD oldSection = -1;
	DWORD goDB = 0;

	N->cnt++;
	while (1) {
		//確認merge條件
		assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
		if (NSLB->writedPageNum == NSLB->NSLBpage_num){
			N->MergeCondition++;
			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
			//FASTmergeRWNSLBpart23_3(simPtr);
			FASTmergeRWNSLBpart23_3_SE(simPtr); //SE merge
		}//當前空間已寫滿
		assert(NSLB->writedPageNum < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
		//一般寫入流程
		//新寫入sector的DB更新BPLRU時間
		LBA = offsetSector / NC->blockSizeSector; assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
		SectorTableNo = offsetSector / NC->pageSizeSector; assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
		assert(N->BPLRU[LBA] <= N->cnt);
		N->BPLRU[LBA] = N->cnt;
		//確認該sector的L2P mapping table正確
		block_no = getblock(simPtr, SectorTableNo);
		page_no = getpage(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizePage);
		assert(SectorTableNo / NC->blockSizeSector == block_no); assert(SectorTableNo % NC->blockSizeSector == page_no);
		//新寫入或第二次以上寫入的情況分別處理
		assert(N->blocks[block_no].pages[page_no].valid == 1 || N->blocks[block_no].pages[page_no].valid == 2);
		if (N->blocks[block_no].pages[page_no].valid == 2){//第二次以上寫入
			//NSLB舊page找尋最佳寫入位置
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
			oldNSLBfreeTrack = N->blocks[block_no].pages[page_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);

			//把舊section寫到cur track(preNSLBfreeTrack)
			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			//	}else{
			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
			//		for (i = 0; i < N->NSLB_tracks; i++){
			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
			//				NSLBfreeTrack = i;
			//				mini_TKdist = tmp_TKdist;
			//			}
			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
			//	//歸還舊NSLBremainSectorTable
			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
			//	//更新NSLB page位置
			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
			//	//更新NSLBremainSectorTable資訊
			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
			//}

			//寫入量統計
			NSLB->pageRead++; NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
			NSLB->write_count++;
#ifdef Time_Overhead
			//if (goRMW == 0){ 
			//	Calc_TimeOverhead2(simPtr, (NC->LsizeSector + oldNSLBfreeTrack * 2 * NC->trackSizePage) / NC->trackSizePage, 2); //  / NC->CylinderSizeTrack
			//	Calc_TimeOverhead2(simPtr, (NC->LsizeSector + preNSLBfreeTrack * 2 * NC->trackSizePage) / NC->trackSizePage, 2); //  / NC->CylinderSizeTrack
			//	goRMW = 1; assert(goDB!=1);
			//}
#endif
		}
		else if (N->blocks[block_no].pages[page_no].valid == 1){//新寫入
			//寫入量統計
			NSLB->pageRead++; NSLB->pageWrite++; NSLB->writedPageNum++;
			NSLB->write_count++;
			//找尋可以寫入的NSLBremainSector
			NSLBfreeTrack = -1;
			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
			if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
			}
			else{
				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
				for (i = 0; i < N->NSLB_tracks; i++){
					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
					if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist <= mini_TKdist){
						NSLBfreeTrack = i;
						mini_TKdist = tmp_TKdist;
					}
				}assert(NSLBfreeTrack != preNSLBfreeTrack);
#ifdef Time_Overhead
				//printf("%lu ", N->NSLBremainSectorTable[NSLBfreeTrack]);
				if (N->WriteBuff_Size > 0){
					NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
				}
				N->SLBtrackChange++;
				N->preNSLB_WriteSector = NSLBfreeTrack * NC->trackSizePage;
#endif
			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
#ifdef Time_Overhead
			Calc_TimeOverhead2(simPtr, (NC->LsizeSector + NSLBfreeTrack * 2 * NC->trackSizePage) / NC->trackSizePage, 2); //  / NC->CylinderSizeTrack
#endif
			//在DB上標記為寫入NSLB的page
			N->blocks[block_no].pages[page_no].valid = 2;
			//把寫入的NSLB track記錄到原DB的sector上
			N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[NSLBfreeTrack] - 1;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
			assert(0 <= N->blocks[block_no].pages[page_no].sector && N->blocks[block_no].pages[page_no].sector < NSLB->NSLBpage_num);
			assert(N->blocks[block_no].pages[page_no].sector / NC->trackSizePage == NSLBfreeTrack);
			//更新NSLBremainSectorTable資訊
			N->NSLBremainSectorTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] < NC->trackSizePage);
			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
#ifdef Time_Overhead
			//if (goDB == 0){//
			//	Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 2);//  / NC->CylinderSizeTrack
			//	Calc_TimeOverhead2(simPtr, (NC->LsizeSector + NSLBfreeTrack * 2 * NC->trackSizePage) / NC->trackSizePage, 2);//  / NC->CylinderSizeTrack
			//	goDB = 1; assert(goRMW != 1);
			//}
#endif
		}
		offsetSector += NC->pageSizeSector;
		lenSector -= NC->pageSizeSector;
		if (lenSector <= 0){ break; }
	}
}
void FASTmergeRWNSLBpart23_3_SE(sim *simPtr)
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

	DWORD StartTrack = 0;
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
		merge_ram_size = 0;
#endif
		k = 0;
		while (N->blocks[dataBlock].pages[k].valid == 1){ k++; }
		StartTrack = k / NC->trackSizePage; assert(0 <= k && k < NC->blockSizeSector);
		for (; k < NC->blockSizeSector; k++){//k = 0
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
				merge_ram[merge_ram_size] = NSLBdirtyTrack*NC->trackSizePage; merge_ram_size++; //未使用SLB故當作不存在，不加上:SLB->partial_page + 1
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
#ifdef Time_Overhead
		assert(merge_ram_size <= NC->blockSizeSector);
		qsort(merge_ram, merge_ram_size, sizeof(DWORD), compare);
		for (k = 0; k < merge_ram_size - 1; k++){ assert(merge_ram[k] <= merge_ram[k + 1]); }
		Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[0] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 3);//電梯法回SLB讀資料
		for (k = 1; k < merge_ram_size; k++){//電梯法回SLB讀資料
			if (merge_ram[k - 1] / NC->trackSizePage != merge_ram[k] / NC->trackSizePage){
				//seek
				Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + (merge_ram[k] / NC->trackSizePage) + 1) / NC->CylinderSizeTrack, 3);
			}
		}
		//DB seek
		for (k = StartTrack*NC->trackSizePage; k < NC->blockSizePage; k += NC->trackSizePage){
			SectorTableNo = dataBlock*NC->blockSizePage + k; assert(0 <= SectorTableNo && SectorTableNo < NC->LsizePage);
			Calc_TimeOverhead(simPtr, SectorTableNo / NC->trackSizePage / NC->CylinderSizeTrack, 3);//寫在DB上
		}
		//seek
		N->NSLBmerge = 1;//逼迫等一下回NSLB write一定要算seek
#endif
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






//可以跑
void NSLB2_TrackRMWtime(sim *simPtr, DWORD Max_Size){
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
#ifdef Time_Overhead
	assert(0 < N->WriteBuff_Size);  assert(N->WriteBuff_Size <= Max_Size);
	qsort(N->WriteBuff, N->WriteBuff_Size, sizeof(DWORD), compare);
	if (N->WriteBuff_Size > 1){ for (i = 0; i < N->WriteBuff_Size - 1; i++){ assert(N->WriteBuff[i] <= N->WriteBuff[i + 1]); } }
	Calc_TimeOverhead2(simPtr, N->WriteBuff[0] / NC->trackSizePage, 2);
	for (i = 1; i < N->WriteBuff_Size; i++){
		if (N->WriteBuff[i - 1] / NC->trackSizePage != N->WriteBuff[i] / NC->trackSizePage){ //seek
			Calc_TimeOverhead2(simPtr, N->WriteBuff[i] / NC->trackSizePage, 2);
		}
	}
	/*if (0 <= N->WriteBuff[N->WriteBuff_Size - 1] && N->WriteBuff[N->WriteBuff_Size - 1] < NC->LsizeSector){ N->preNSLB_WriteSector = 0; }
	else{ N->preNSLB_WriteSector = (N->WriteBuff[N->WriteBuff_Size - 1] - NC->LsizeSector)/2; }*/
#endif //電梯法回SLB讀資料  //讀SLB消耗的rotation
}
//void FASTwriteSectorNSLBpart23_3_RMW(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	DWORD LBA, LPA, i, j, k, flag = 0, newBlock, dataBlock, SectorTableNo, BlockNo, PageNo, os = offsetSector, ls = lenSector;
//	DWORD cur_8subband = -1, prev_8subband = -1;
//	simConfig *SC = &(simPtr->simConfigObj); simStat *SS = &(simPtr->simStatObj); NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	DWORD page_no = 0, block_no = 0;
//	DWORD NSLBfreeTrack = 0;
//	DWORD preNSLBfreeTrack = 0, oldNSLBfreeTrack = 0;
//	DWORD mini_TKdist = N->NSLB_tracks, tmp_TKdist = N->NSLB_tracks;
//
//	DWORD go_OldSection_NSLBtrack = 0;
//	DWORD curSection = offsetSector / N->SubTrack;
//	DWORD cur_Section_Update = 0;
//
//	DWORD goRMW = 0;
//	DWORD oldSection = -1;
//	DWORD goDB = 0;
//
//	N->cnt++;
//	while (1) {
//		//確認merge條件
//		assert(NSLB->writedPageNum <= NSLB->NSLBpage_num);
//		if (NSLB->writedPageNum == NSLB->NSLBpage_num){
//			assert(N->NSLBexhaustedTrack == N->NSLB_tracks);
//			FASTmergeRWNSLBpart23_3(simPtr);
//		}//當前空間已寫滿
//		assert(NSLB->writedPageNum < NSLB->NSLBpage_num); assert(N->NSLBexhaustedTrack < N->NSLB_tracks);
//		//一般寫入流程
//		//新寫入sector的DB更新BPLRU時間
//		LBA = offsetSector / NC->blockSizeSector; assert(0 <= offsetSector && offsetSector < NC->LsizeSector);
//		SectorTableNo = offsetSector / NC->pageSizeSector; assert(0 <= LBA && LBA <= NC->LsizeBlock - 1);
//		assert(N->BPLRU[LBA] <= N->cnt);
//		N->BPLRU[LBA] = N->cnt;
//		//確認該sector的L2P mapping table正確
//		block_no = getblock(simPtr, SectorTableNo);
//		page_no = getpage(simPtr, SectorTableNo);
//		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= page_no && page_no < NC->blockSizePage);
//		assert(SectorTableNo / NC->blockSizeSector == block_no); assert(SectorTableNo % NC->blockSizeSector == page_no);
//		//新寫入或第二次以上寫入的情況分別處理
//		assert(N->blocks[block_no].pages[page_no].valid == 1 || N->blocks[block_no].pages[page_no].valid == 2);
//		if (N->blocks[block_no].pages[page_no].valid == 2){//第二次以上寫入
//			//NSLB舊page找尋最佳寫入位置
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage; assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks); //紀錄NSLB上之前寫到的track
//			oldNSLBfreeTrack = N->blocks[block_no].pages[page_no].sector / NC->trackSizePage; assert(0 <= oldNSLBfreeTrack && oldNSLBfreeTrack < N->NSLB_tracks);
//
//			//把舊section寫到cur track(preNSLBfreeTrack)
//			//if (oldNSLBfreeTrack != preNSLBfreeTrack){
//			//	if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//			//		NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			//	}else{
//			//		mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//			//		for (i = 0; i < N->NSLB_tracks; i++){
//			//			tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//			//			if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist < mini_TKdist){
//			//				NSLBfreeTrack = i;
//			//				mini_TKdist = tmp_TKdist;
//			//			}
//			//		}assert(NSLBfreeTrack != preNSLBfreeTrack);
//			//	}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//			//	//歸還舊NSLBremainSectorTable
//			//	if (N->NSLBremainSectorTable[oldNSLBfreeTrack] == 0){ N->NSLBexhaustedTrack--; }
//			//	N->NSLBremainSectorTable[oldNSLBfreeTrack]++;
//			//	//更新NSLB page位置
//			//	N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack * NC->trackSizePage;
//			//	//更新NSLBremainSectorTable資訊
//			//	N->NSLBremainSectorTable[NSLBfreeTrack]--;
//			//	if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//			//}
//
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++;//重複寫入之前該NSLB track位置上的page，不用做sector值的更新
//			NSLB->write_count++;
//#ifdef Time_Overhead
//			//if (goRMW == 0){ 
//			//	Calc_TimeOverhead2(simPtr, (NC->LsizeSector + oldNSLBfreeTrack * 2 * NC->trackSizePage) / NC->trackSizePage, 2); //  / NC->CylinderSizeTrack
//			//	Calc_TimeOverhead2(simPtr, (NC->LsizeSector + preNSLBfreeTrack * 2 * NC->trackSizePage) / NC->trackSizePage, 2); //  / NC->CylinderSizeTrack
//			//	goRMW = 1; assert(goDB!=1);
//			//}
//#endif
//		}
//		else if (N->blocks[block_no].pages[page_no].valid == 1){//新寫入
//			//寫入量統計
//			NSLB->pageRead++; NSLB->pageWrite++; NSLB->writedPageNum++;
//			NSLB->write_count++;
//			//找尋可以寫入的NSLBremainSector
//			NSLBfreeTrack = -1;
//			preNSLBfreeTrack = N->preNSLB_WriteSector / NC->trackSizePage;//紀錄NSLB上之前寫到的track
//			assert(0 <= preNSLBfreeTrack && preNSLBfreeTrack < N->NSLB_tracks);
//			if (N->NSLBremainSectorTable[preNSLBfreeTrack]>0){
//				NSLBfreeTrack = preNSLBfreeTrack; //優先寫寫過的track，降低head move量
//			}
//			else{
//				mini_TKdist = N->NSLB_tracks; tmp_TKdist = N->NSLB_tracks;
//				for (i = 0; i < N->NSLB_tracks; i++){
//					tmp_TKdist = abs(i - preNSLBfreeTrack); assert(0 <= tmp_TKdist && tmp_TKdist < N->NSLB_tracks);
//					if (N->NSLBremainSectorTable[i] > 0 && tmp_TKdist <= mini_TKdist){
//						NSLBfreeTrack = i;
//						mini_TKdist = tmp_TKdist;
//					}
//				}assert(NSLBfreeTrack != preNSLBfreeTrack);
//#ifdef Time_Overhead
//				//printf("%lu ", N->NSLBremainSectorTable[NSLBfreeTrack]);
//				if (N->WriteBuff_Size > 0){
//					NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
//				}
//				N->SLBtrackChange++;
//				N->preNSLB_WriteSector = NSLBfreeTrack * NC->trackSizePage;
//#endif
//			}assert(0 <= NSLBfreeTrack && NSLBfreeTrack < N->NSLB_tracks);
//#ifdef Time_Overhead
//			Calc_TimeOverhead2(simPtr, (NC->LsizeSector + NSLBfreeTrack * 2 * NC->trackSizePage) / NC->trackSizePage, 2); //  / NC->CylinderSizeTrack
//#endif
//			//在DB上標記為寫入NSLB的page
//			N->blocks[block_no].pages[page_no].valid = 2;
//			//把寫入的NSLB track記錄到原DB的sector上
//			N->blocks[block_no].pages[page_no].sector = NSLBfreeTrack*NC->trackSizePage + N->NSLBremainSectorTable[NSLBfreeTrack] - 1;//配置NSLBfreeTrack上的第N->NSLBremainSectorTable[NSLBfreeTrack] - 1個page
//			assert(0 <= N->blocks[block_no].pages[page_no].sector && N->blocks[block_no].pages[page_no].sector < NSLB->NSLBpage_num);
//			assert(N->blocks[block_no].pages[page_no].sector / NC->trackSizePage == NSLBfreeTrack);
//			//更新NSLBremainSectorTable資訊
//			N->NSLBremainSectorTable[NSLBfreeTrack]--; assert(0 <= N->NSLBremainSectorTable[NSLBfreeTrack] && N->NSLBremainSectorTable[NSLBfreeTrack] < NC->trackSizePage);
//			if (N->NSLBremainSectorTable[NSLBfreeTrack] == 0){ N->NSLBexhaustedTrack++; }
//#ifdef Time_Overhead
//			//if (goDB == 0){//
//			//	Calc_TimeOverhead2(simPtr, SectorTableNo / NC->trackSizePage, 2);//  / NC->CylinderSizeTrack
//			//	Calc_TimeOverhead2(simPtr, (NC->LsizeSector + NSLBfreeTrack * 2 * NC->trackSizePage) / NC->trackSizePage, 2);//  / NC->CylinderSizeTrack
//			//	goDB = 1; assert(goRMW != 1);
//			//}
//#endif
//		}
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0){ break; }
//	}
//}
void NSLB_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
{
	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	DWORD i = 0, j = 0;
	DWORD start_section = offsetSector / NC->Section, end_section = 0;
	DWORD SectorTableNo = 0, block_no = 0, page_no = 0;

	if ((offsetSector + lenSector) % NC->Section != 0){ end_section = (offsetSector + lenSector) / NC->Section + 1; }
	else{ end_section = (offsetSector + lenSector) / NC->Section; }
	assert(start_section <= end_section);

#ifdef Time_Overhead
	N->WriteBuff_Size = 0;
#endif
	while (start_section <= end_section){
#ifdef Time_Overhead
		if (N->WriteBuff_Size == NC->trackSizePage / NC->Section){
			NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
			Calc_TimeOverhead2(simPtr, (NC->LsizeSector + N->preNSLB_WriteSector * 2) / NC->trackSizePage, 2);
		}
		SectorTableNo = start_section*N->SubTrack; block_no = getblock(simPtr, SectorTableNo); page_no = getpage(simPtr, SectorTableNo);
		assert(0 <= block_no && block_no < NC->LsizeBlock); assert(0 <= N->WriteBuff_Size); assert(N->WriteBuff_Size < NC->trackSizePage / N->SubTrack);
		assert(N->blocks[block_no].pages[page_no].valid == 1 || N->blocks[block_no].pages[page_no].valid == 2);
		if (N->blocks[block_no].pages[page_no].valid == 1){ N->WriteBuff[N->WriteBuff_Size] = SectorTableNo; N->RDB++; }//在DB
		else{ N->WriteBuff[N->WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector * 2; N->RMW++; }//在NSLB
		N->WriteBuff_Size++;
#endif
		FASTwriteSectorNSLBpart23_3_RMW(simPtr, start_section * NC->Section, NC->Section);
		start_section++;
	}
#ifdef Time_Overhead
	if (N->WriteBuff_Size > 0){
		NSLB2_TrackRMWtime(simPtr, NC->trackSizePage / N->SubTrack); N->WriteBuff_Size = 0;
		Calc_TimeOverhead2(simPtr, (NC->LsizeSector + N->preNSLB_WriteSector * 2) / NC->trackSizePage, 2);
	}
#endif
}
#endif	