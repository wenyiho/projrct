#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"

#ifdef Simulation_HDD
void HDDfree(sim *simPtr) {
	/* NFTLstat	*NS = &(simPtr->NFTLobj.statObj);		NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);*/
	NFTL *N = &(simPtr->NFTLobj);
#ifdef SpecificTrack
	free(N->NSLB_RAM);
	free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	//free(N->WriteBuff);
	//free(N->NMR_RMWBuff);
#endif
}
#endif
#ifdef Simulation_SLB
void SLBfree(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat *NS = &(simPtr->NFTLobj.statObj); 
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	free(N->blocks[0].pages);
	free(N->blocks);
	free(N->st);
#ifdef SpecificTrack
	//free(N->NSLB_RAM);
	//free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
#endif
	//free(N->section_cumu);
	//free(N->SLB_section_cumu);
	//free(N->DB_section_cumu);
	free(N->SlotTable_Band);
}
void SLBfree_NoSt(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	free(N->blocks[0].pages);
	free(N->blocks);
	if (N->st != NULL){ free(N->st); }
#ifdef SpecificTrack
	//free(N->NSLB_RAM);
	//free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
#endif
	//free(N->section_cumu);
	//free(N->SLB_section_cumu);
	//free(N->DB_section_cumu);
	free(N->SlotTable_Band);
}
void SLBfree_NoStNoDB(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	//free(N->blocks[0].pages);
	for (i = 0; i < NC->PsizeBlock; i++){
		if (N->blocks[i].sections != NULL){ free(N->blocks[i].sections); }
	}
	free(N->blocks);
	if (N->st != NULL){ free(N->st); }
#ifdef SpecificTrack
	//free(N->NSLB_RAM);
	//free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
#endif
	//free(N->section_cumu);
	//free(N->SLB_section_cumu);
	//free(N->DB_section_cumu);
	free(N->SlotTable_Band);
	if (N->DBalloc != NULL){ free(N->DBalloc); }
	//統計Effect Merge會寫多少Data Cylinder
	if(N->EM_DataCylinder != NULL){ free(N->EM_DataCylinder);}
}
#endif
#ifdef Simulation_SLB_SHUFFLE
void SLBfree_NoStNoDB(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	//free(N->blocks[0].pages);
	for (i = 0; i < NC->PsizeBlock; i++){
		if (N->blocks[i].sections != NULL){ free(N->blocks[i].sections); }
	}
	free(N->blocks);
	if (N->st != NULL){ free(N->st); }
#ifdef SpecificTrack
	//free(N->NSLB_RAM);
	//free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
#endif
	//free(N->section_cumu);
	//free(N->SLB_section_cumu);
	//free(N->DB_section_cumu);
	free(N->SlotTable_Band);
	if (N->DBalloc != NULL){ free(N->DBalloc); }
#ifdef Simulation_SLB_SHUFFLE
	//TRANSFOR_LAYER
	for (i = 0; i < NC->LsizeBlock; i++){
		if (N->SHUFFLE_TL[i].L2P_Index != NULL){ assert(N->SHUFFLE_TL_Alloc[i]==1); free(N->SHUFFLE_TL[i].L2P_Index); }
		if (N->SHUFFLE_TL[i].P2L_Index != NULL){ free(N->SHUFFLE_TL[i].P2L_Index); }
		if (N->SHUFFLE_TL[i].WriteFreq != NULL){ free(N->SHUFFLE_TL[i].WriteFreq); }
		if (N->SHUFFLE_TL[i].DirtyUnit != NULL){ free(N->SHUFFLE_TL[i].DirtyUnit); }
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE//歸還MFMBO記憶體
		if (N->MFMBO_INFO[i].MergeFreq != NULL){ assert(N->SHUFFLE_TL_Alloc[i] == 1); free(N->MFMBO_INFO[i].MergeFreq); }
		if (N->MFMBO_INFO[i].MergeDS != NULL){ free(N->MFMBO_INFO[i].MergeDS); }
#endif
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE_TK//歸還MFMBO記憶體
		if (N->MFMBO_INFO[i].MergeFreq != NULL){ assert(N->SHUFFLE_TL_Alloc[i] == 1); free(N->MFMBO_INFO[i].MergeFreq); }
		if (N->MFMBO_INFO[i].MergeDS != NULL){ free(N->MFMBO_INFO[i].MergeDS); }
#endif
	}
	free(N->SHUFFLE_TL);
	//紀錄有沒有配過
	free(N->SHUFFLE_TL_Alloc);
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
	//限定只能Shuffle 1 次
	if (N->SHUFFLE_TL_OneTime != NULL){ free(N->SHUFFLE_TL_OneTime); }
	//統計整體情況
	if (N->SHUFFLE_DB_Info.WriteFreq != NULL){ free(N->SHUFFLE_DB_Info.WriteFreq); }
	if (N->SHUFFLE_DB_Info.DirtyUnit != NULL){ free(N->SHUFFLE_DB_Info.DirtyUnit); }
	for (i = 0; i < NC->LsizeBlock; i++){
		free(N->SHUFFLE_TL_OBSERVE[i].WriteFreq);
		free(N->SHUFFLE_TL_OBSERVE[i].DirtyUnit);
		assert(N->BAND_MPL[i].CUMU_TIME);
		assert(N->BAND_MPL[i].TEMP_TIME);
		assert(N->BAND_MPL[i].PERIOD);
		assert(N->BAND_MPL[i].PRE_DirtyUnit);
	}
	free(N->SHUFFLE_TL_OBSERVE);
	free(N->BAND_MPL);
#endif
#ifdef Simulation_SLB_DELAY
	free(N->DELAY_SECTORinBAND);
	free(N->VictimCylinder_BUFF);
#ifdef Simulation_SLB_DELAY_ROUND
	free(N->DRLB_CYLINDER);
#endif
	free(N->SHUFFLE_BOUND);
	free(N->BAND_DELAY_PERIOD);
#endif
#endif
}
#endif
#ifdef SACE
void SLBfree_NoStNoDB(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	//free(N->blocks[0].pages);
	for (i = 0; i < NC->PsizeBlock; i++){
		if (N->blocks[i].sections != NULL){ free(N->blocks[i].sections); }
#ifdef SACE
		if (N->blocks[i].SACE_TKDirtySector != NULL){ free(N->blocks[i].SACE_TKDirtySector); }
#endif
	}
	free(N->blocks);
	if (N->st != NULL){ free(N->st); }
#ifdef SpecificTrack
	//free(N->NSLB_RAM);
	//free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
#endif
	//free(N->section_cumu);
	//free(N->SLB_section_cumu);
	//free(N->DB_section_cumu);
	free(N->SlotTable_Band);
	if (N->DBalloc != NULL){ free(N->DBalloc); }
	//統計Effect Merge會寫多少Data Cylinder
	if(N->EM_DataCylinder != NULL){ free(N->EM_DataCylinder);}
}
#endif
#ifdef SLB_Mapping_Cache
#ifdef CFLRU
void Cachefree(sim *simPtr){
	NFTL *N = &(simPtr->NFTLobj);
	DWORD i = 0;
	BYTE k = 0;
	assert(N->SLB_Map_Cache != NULL);
	for (k = 0; k < 2; k++){
		for (i = 0; i <= N->SLB_Map_Cache[k].cache_used; i++){
			LRU_Map_Entry *tmp = NULL; 
			printf("K = %x, i = %lu  used: %lu\n", k, i, N->SLB_Map_Cache[k].cache_used);
			assert(N->SLB_Map_Cache[k].LRU_Head != NULL && N->SLB_Map_Cache[k].LRU_Head->next != NULL);
			tmp = N->SLB_Map_Cache[k].LRU_Head;
			N->SLB_Map_Cache[k].LRU_Head = N->SLB_Map_Cache[k].LRU_Head->next;
			assert(tmp->next == N->SLB_Map_Cache[k].LRU_Head);
			assert(tmp != NULL);
			free(tmp);
			tmp = NULL;
		}
	}
	
	assert(N->SLB_Map_Cache != NULL);
	free(N->SLB_Map_Cache);
	N->SLB_Map_Cache = NULL;
}
#else
void Cachefree(sim *simPtr){
	NFTL *N = &(simPtr->NFTLobj);
	DWORD i = 0;
	for (i = 0; i <= N->SLB_Map_Cache->cache_used; i++){
		LRU_Map_Entry *tmp = NULL;
		assert(N->SLB_Map_Cache->LRU_Head != NULL && N->SLB_Map_Cache->LRU_Head->next != NULL);
		tmp = N->SLB_Map_Cache->LRU_Head;
		N->SLB_Map_Cache->LRU_Head = N->SLB_Map_Cache->LRU_Head->next;
		assert(tmp != NULL);
		free(tmp);
		tmp = NULL;
	}
	assert(N->SLB_Map_Cache != NULL);
	free(N->SLB_Map_Cache);
}
#endif
#endif


#ifdef Simulation_SLBPM
void SLBfree_NoStNoDB(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	//free(N->blocks[0].pages);
	for (i = 0; i < NC->PsizeBlock; i++){
		if (N->blocks[i].sections != NULL){ free(N->blocks[i].sections); }
	}
	free(N->blocks);
	if (N->st != NULL){ free(N->st); }
#ifdef SpecificTrack
	//free(N->NSLB_RAM);
	//free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
#endif
	//free(N->section_cumu);
	//free(N->SLB_section_cumu);
	//free(N->DB_section_cumu);
	free(N->SlotTable_Band);
	if (N->DBalloc != NULL){ free(N->DBalloc); }
#ifdef Simulation_SLBPM
	if (N->WB2SMR_TASK){ free(N->WB2SMR_TASK); }
	if (N->PMG_Rank){ free(N->PMG_Rank); }
#endif
}
#endif
#ifdef Simulation_NSLB
void NSLBfree(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj);		NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	free(N->blocks[0].pages);
	free(N->blocks);
	//free(N->L2Ptable);
	if (N->st != NULL){ free(N->st); }
	if (N->BPLRU != NULL){ free(N->BPLRU); }
	/*NSLB專用*/
	if (N->NSLBremainSectionTable != NULL){ free(N->NSLBremainSectionTable); }

#ifdef SpecificTrack
	//free(N->NSLB_RAM);
	//free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
#endif
#ifdef NSLB_LocalUse
	if (N->DB2curNSLB_TK != NULL){ free(N->DB2curNSLB_TK); }
	if (N->ExcluNSLBTK != NULL){ free(N->ExcluNSLBTK); }
	if (N->NSLBTK_Share != NULL){ free(N->NSLBTK_Share); }
#endif
}
void NSLBfree_NoDB(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj);		NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	free(N->blocks[0].pages);
	free(N->blocks);
	//free(N->L2Ptable);
	if (N->st != NULL){ free(N->st); }
	if (N->BPLRU != NULL){ free(N->BPLRU); }
	/*NSLB專用*/
	//if (N->NSLBremainSectionTable != NULL){ free(N->NSLBremainSectionTable); }

#ifdef SpecificTrack
	//free(N->NSLB_RAM);
	//free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
#endif
#ifdef NSLB_LocalUse
	if (N->DB2curNSLB_TK != NULL){ free(N->DB2curNSLB_TK); }
	if (N->ExcluNSLBTK != NULL){ free(N->ExcluNSLBTK); }
	if (N->NSLBTK_Share != NULL){ free(N->NSLBTK_Share); }
#endif
}
#endif
#ifdef Simulation_HLB
void HLBfree(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
	NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	free(N->blocks[0].pages);
	free(N->blocks);
	free(N->st);
#ifdef SpecificTrack
	//free(N->NSLB_RAM);
	//free(N->RAM_Valid);
#endif
#ifdef Time_Overhead
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
#endif
	//free(N->section_cumu);
	//free(N->SLB_section_cumu);
	//free(N->DB_section_cumu);
	free(N->SlotTable_Band);

	if (N->BPLRU != NULL){ free(N->BPLRU); }
	if (N->NSLBremainSectionTable != NULL){ free(N->NSLBremainSectionTable); }
#ifdef Simulation_HLB
	if (N->N2S_LBsector != NULL){ free(N->N2S_LBsector); }
#endif
}
#endif
void FASTfree(sim *simPtr) {
	NFTL *N = &(simPtr->NFTLobj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj);		NFTLconfig	*NC = &(simPtr->NFTLobj.configObj);
	DWORD i = 0;
	free(N->blocks[0].pages);
	free(N->blocks);
	//free(N->L2Ptable);
	if (N->st != NULL){ free(N->st); }
	if (N->BPLRU != NULL){ free(N->BPLRU); }
	/*NSLB專用*/
	if (N->NSLBremainSectorTable != NULL){ free(N->NSLBremainSectorTable); }
#ifdef NEWGC64trackWRITE
	free(N->PMCandidate);
	if (N->SLBremainSectorTable != NULL){ free(N->SLBremainSectorTable); }
#endif

	
	//free(N->PMGT_NSLBaddress);
	free(N->WriteBuff);
	free(N->NMR_RMWBuff);
	
//#ifdef SpecificTrack
//	//試驗
//	//if (N->Band_NSLBTrackIndex != NULL){ free(N->Band_NSLBTrackIndex); }
//
//	//試驗
//	//if (N->Band_Tracks != NULL){ free(N->Band_Tracks); }
//	//if(N->Used_Sector != NULL){ free(N->Used_Sector); }
//
//	//試驗
//	if (N->NSLB_TrackIndex != NULL){
//		for (i = 0; i < NC->LsizeBlock; i++){
//			if (N->NSLB_TrackIndex[i].head != NULL){ free(N->NSLB_TrackIndex[i].head); }
//		}
//		free(N->NSLB_TrackIndex);
//	}
//	if(N->NSLB_Band != NULL){ free(N->NSLB_Band); }
//	if (N->NSLB_RAM != NULL){ free(N->NSLB_RAM); }
//
//	if(N->RAM_Valid != NULL){ free(N->RAM_Valid); }
//#ifdef NSLB_RAM_BandLRU_HIT
//	if (N->RAM_BPLRU != NULL){ free(N->RAM_BPLRU); }
//#endif
//#endif
}