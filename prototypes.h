
//日志显示(文件名：行号}+日期  
#define LOG() printf("FIle:%s, Line:%d\n", __FILE__, __LINE__)
//错误信息位置（级别+文件名+函数名+行号+错误信息)  
#define ERROR() do{ fprintf(stderr, "[ERROR  at: %s] %s(Line %d):\n", __FILE__, __FUNCTION__, __LINE__); } while (0)
#define assert_printf(A, M, ...) if (!(A)) { printf("[ERROR]  FILE: %s, FUNC:%s, Line %d, Info:", __FILE__, __FUNCTION__, __LINE__); printf(M, __VA_ARGS__); fflush(stdout); system("pause"); }

// simulation driver
void simInit(sim *simPtr);
void simCheck(sim *simPtr);
void simFree(sim *simPtr);
void simWriteLog(sim *simPtr);

//FAST
#ifdef Block_level_merge
void FASTinit(sim *simPtr);
void FASTwriteSector(sim *simPtr, DWORD offsetSector, DWORD lenSector);
DWORD FASTgetSpareBlock(sim *simPtr);
void FASTmergeRW(sim *simPtr);
#endif

#ifdef Track_level_merge
void FASTinitCLB(sim *simPtr);
void FASTwriteSectorCLB(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTmergeRWCLB(sim *simPtr);
void FINALmergeRWtrack(sim *simPtr, DWORD tsp_start, DWORD tsp_end);//merge all data in log buffer in track unit after end of trace file
#endif

#ifdef Time_Overhead
DWORD compare(const DWORD *arg1, const DWORD *arg2);
void Calc_RotateOverhead2(sim *simPtr, I64 cur_HeadSector, DWORD Type);
void Calc_Rotate_SkewSMR(sim *simPtr, I64 cur_HeadSector, DWORD Type);


void Init_HP_C3323A_validate(sim *simPtr);

void Init_Seagate_Cheetah15k5(sim *simPtr);// Init_Seagate_Cheetah15k5
DWORD Seek_Seagate_C15k5(sim *simPtr, I64 cur_HeadSector, DWORD Type);
DWORD Seek_Seagate_C15k5_SMR(sim *simPtr, I64 cur_HeadSector, DWORD Type);
void Init_TimeOverhead(sim *simPtr);



void Calc_TimeOverhead2_old(sim *simPtr, I64 cur_HeadSector, DWORD Type);
void Calc_TimeOverhead2(sim *simPtr, I64 cur_HeadSector, DWORD Type);
DWORD Calc_TimeOverhead3(sim *simPtr, I64 cur_HeadSector, DWORD Type);//hpc3323
void Seek_End(sim *simPtr, I64 cur_HeadSector, DWORD Type);

void Calc_TimeOverhead(sim *simPtr, I64 cur_HeadSector, DWORD Type);
#ifdef POWERFAIL
void PF_DiskCacheWrite(sim *simPtr, DWORD start_Track);
#endif
#endif

DWORD Two_Expo(DWORD n);
#ifdef READ_REQUEST
void FASTinitRead(sim *simPtr);
void DM_SMRreadSector(sim *simPtr, I64 offsetSector, DWORD lenSector);
void DM_SMRreadSector2(sim *simPtr, I64 offsetSector, DWORD lenSector);
#endif
#ifdef Simulation_HDD
void FASTinitHDD(sim *simPtr);
void HDD(sim *simPtr, I64 offsetSector, DWORD lenSector);
void HDD_RAM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void HDD_RAM_WriteAllOut(sim *simPtr);
void HDDfree(sim *simPtr);
#endif
#ifdef Simulation_SLB
void FASTinitSLB(sim *simPtr);
void SLB2_TrackRMWtime(sim *simPtr, DWORD Max_Size, DWORD type);
void FASTwriteSectorSLBpart2(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void SLB_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTwriteSectorSLBRAM2_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTwriteSectorSLBRAM2_WriteAllOut(sim *simPtr);
void FASTmergeRWSLBpart2(sim *simPtr);
void SLBfree(sim *simPtr);

void FASTwriteSectorSLBpart2_BitMap(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTmergeRWSLBpart2_BitMap(sim *simPtr);

void FASTwriteSectorSLBpart2_Sr(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTmergeRWSLBpart2_Sr(sim *simPtr);

void DM_SMRwriteSector(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void DM_SMRmerge(sim *simPtr);
//------
void FASTinitSLB_NoSt(sim *simPtr);
void DM_SMRwriteSector_NoSt(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void SLB_NoRAM_NoSt(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTmergeRWSLBpart2_Sr_NoSt(sim *simPtr);
void SLBfree_NoSt(sim *simPtr);

void FASTinitSLB_NoStNoDB(sim *simPtr);
void DM_SMRwriteSector_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector);
void SLB_NoRAM_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector);
void FASTmergeRWSLBpart2_Sr_NoStNoDB(sim *simPtr);
void FASTmergeRWSLBpart2_Sr_NoStNoDB_PB(sim *simPtr);
void SLBfree_NoStNoDB(sim *simPtr);
#endif
#ifdef Simulation_SLB_SHUFFLE
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
void PRINT_Simulation_SLB_OBSERVE(sim *simPtr);
void FASTmergeRWSLB_Observe(sim *simPtr);
void FASTmergeRWSLB_Observe2(sim *simPtr);
#endif
void FASTinitSLB_NoStNoDB(sim *simPtr);
//Cylinder_Shuffle
void DM_SMR_SHUFFLEwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector);
void SLB_NoRAM_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector);
void FASTmergeRWSLB_SHUFFLE(sim *simPtr);
void FASTmergeRWSLB_SHUFFLE2(sim *simPtr);
//Track_Shuffle
void DM_SMR_SHUFFLE_TRACKwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector);
void FASTmergeRWSLB_SHUFFLE_TRACK(sim *simPtr);//來自FASTmergeRWSLB_SHUFFLE2
void SLBfree_NoStNoDB(sim *simPtr);
//ST: Shuffle Track,  VTLMA: Victim Track + Limited Merge Associativity
void DM_SMR_ST_VTLMAwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector);
void FASTmergeRWSLB_ST_VT(sim *simPtr);
void FASTmergeRWSLB_ST_VTLMA(sim *simPtr);


void DM_SMR_DELAY_SHUFFLEwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector);
void FASTmergeRWSLB_DELAY_SHUFFLE(sim *simPtr);
void FASTmergeRWSLB_DELAY_ROUND_SHUFFLE(sim *simPtr);
//Cylinder
void DM_SMR_DELAY_LEFT_SHUFFLEwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector);
void FASTmergeRWSLB_DELAY_LEFT_SHUFFLE(sim *simPtr);
void FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE(sim *simPtr);
//Track
void DM_SMR_DELAY_LEFT_SHUFFLE_TRACKwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector);
void FASTmergeRWSLB_CONT_DELAY_LEFT_SHUFFLE_TRACK(sim *simPtr);
//DLST: DELAY_LEFT_SHUFFLE_TRACK, VTLMA: Victim Track + Limited Merge Associativity
void DM_SMR_DLST_VTLMAwriteSector(sim *simPtr, I64 offsetSector, DWORD lenSector);
void FASTmergeRWSLB_CONT_DLST_VT(sim *simPtr);
void FASTmergeRWSLB_CONT_DLST_VTLMA(sim *simPtr);

void FASTmergeRWSLB_DELAY_tmp_SHUFFLE(sim *simPtr);//預估Delay Left最佳情況
#endif
#ifdef SACE
//ICCD 2017 Shingle Aware Cache Management
void FASTinitSLB_NoStNoDB(sim *simPtr);
void SACEwriteSector_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector);
void SACEmerge_NoStNoDB(sim *simPtr);
void SLBfree_NoStNoDB(sim *simPtr);
#endif



#ifdef Simulation_SLBPM
void FASTinitSLB_NoStNoDB(sim *simPtr);
void DM_SMRwriteSector_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector);
void SLB_NoRAM_NoStNoDB(sim *simPtr, I64 offsetSector, DWORD lenSector);
void FASTmergeRWSLBpart2_Sr_NoStNoDB(sim *simPtr);
void SLBfree_NoStNoDB(sim *simPtr);


void DM_SMRwriteSector_PM(sim *simPtr, I64 offsetSector, DWORD lenSector);
void GuardCyl2SMR(sim *simPtr);

void FREE_CUTPM_DB(sim *simPtr);
void PM_DB(sim *simPtr);
void CUTPM_DB_MergeRoutine_Time(sim *simPtr, DWORD start_track, DWORD end_track, DWORD TYPE);
void CUTPM_DB_MergeRoutine_W(sim *simPtr, DWORD start_sector, DWORD end_sector);
void PMG_Rank_SWAP(PMG_RANK *left, PMG_RANK *right);//排名工具
DWORD DBPMG_WriteCyl_RankIndex(sim *simPtr);//排名工具
void CUTPM_DB(sim *simPtr);
void GuardCyl_WB2SMR(sim *simPtr);
void FASTmergeRWSLBpart2_PM(sim *simPtr);
#endif
#ifdef Simulation_NSLB
void FASTinitNSLB(sim *simPtr);
void FASTwriteSectorNSLBpart23_3_RMW(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void NSLB_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTmergeRWNSLBpart3_3(sim *simPtr);
//void FASTmergeRWNSLBpart23_3(sim *simPtr);
void NSLBfree(sim *simPtr);


void DM_NSLBwriteSector(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void DM_NSLBmerge(sim *simPtr);

void FASTinitNSLB2(sim *simPtr);
void DM_NSLBwriteSector2(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void DM_NSLBmerge2(sim *simPtr);

void FASTinitNSLB3(sim *simPtr);
void DM_NSLBwriteSector3_ExcluNSLBTK(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void DM_NSLBmerge3_ExcluNSLBTK(sim *simPtr);
//------

void FASTinitNSLB_NoDB(sim *simPtr);
void DM_NSLBwriteSector_NoDB(sim *simPtr, I64 offsetSector, DWORD lenSector);
void DM_NSLBmerge_NoDB(sim *simPtr);
void NSLBfree_NoDB(sim *simPtr);
void DM_NSLBwriteSector_NoDB_WriteCurr(sim *simPtr, I64 offsetSector, DWORD lenSector);
#endif
#ifdef Simulation_HLB
void DM_SLBwriteSector(sim *simPtr, DWORD offsetSector, DWORD lenSector);
//void HLB_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTinitHLB(sim *simPtr);
void DM_N2SLBw(sim *simPtr);
void DM_SLBmerge(sim *simPtr);
void DM_NSLBwriteSector(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void DM_NSLBmerge(sim *simPtr);
void HLBfree(sim *simPtr);

void DM_SLBw2(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void DM_N2SLBw2(sim *simPtr);
void DM_NSLBw2(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void DM_ReqDependw2(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void DM_SLBmerge2(sim *simPtr);
void DM_NSLBmerge2(sim *simPtr);
#endif
#ifdef SLBNSLB_Tmerge
void FASTinitSLBNSLB(sim *simPtr);
#ifdef SLB_64trackWRITE
BYTE track_offset_SLB(DWORD n);
#endif

void FASTwriteSectorNSLBpart(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTmergeRWNSLBpart(sim *simPtr);
void FINALmergeRWSLB(sim *simPtr, DWORD tsp_start, DWORD tsp_end);
void FINALmergeRWNSLB(sim *simPtr, DWORD tsp_start, DWORD tsp_end);

void showNode(struct Node *ptr);
void FLaddNode(struct Free_List **FLt, struct Node **head, DWORD index);
DWORD FLdeletNode(struct Free_List **FLt, struct Node **head);
void FLdeletNodeIndex(struct Free_List **FLt, struct Node **head, DWORD index);

//BPLRU
void BPLRUshow();
void BPLRUadd(struct BPLRU_List **BPLRU, struct Node **MRU, struct Node **LRU, DWORD index);
void BPLRUdelete(struct BPLRU_List **BPLRU, struct Node **MRU, struct Node **LRU, DWORD index);
DWORD BPLRUdeleteLRU(struct BPLRU_List **BPLRU, struct Node **MRU, struct Node **LRU);
//-----------------------------------------------------------------------------------------------

#ifdef SpecificTrack

void FASTwriteSectorNSLBRAM2_RMW_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTwriteSectorNSLBRAM2_WriteAllOut_RMW(sim *simPtr);
//void NSLB2_TrackRMWtime(sim *simPtr, DWORD Max_Size);



//void FASTwriteSectorNSLBpart23_3_SE_RMW(sim *simPtr, DWORD offsetSector, DWORD lenSector);
//void FASTwriteSectorNSLBRAM2_SE_RMW_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
//void FASTwriteSectorNSLBRAM2_WriteAllOut_SE_RMW(sim *simPtr);
//void FASTmergeRWNSLBpart23_3_SE(sim *simPtr);

#endif



#endif
#ifdef SLBNSLBPM_Tmerge
#ifdef NEWGC64trackWRITE
BYTE track_offset(DWORD n);
#endif

void PMQ_Push(sim *simPtr, DWORD Band_No, DWORD SLB_PageIndex);
DWORD PMQ_Pop(sim *simPtr);
void PMQ_Delete(sim *simPtr, DWORD Band_No);
DWORD PMQ_DeletePush(sim *simPtr, DWORD Band_No);
DWORD WriteDB_SE(sim *simPtr, DWORD PMmerge, DWORD BandNo);
void WriteDB_Cut_SE(sim *simPtr, DWORD PMmerge, DWORD BandNo);
void MergeSLB_Statistics(sim *simPtr, DWORD dataBlock, DWORD PM_Condition);
void FASTmergeRWSLB_SLBNSLBPMe_SE_64PUT(sim *simPtr, int PM_Mode);//PM_Mode (1:0)? do PM;no PM
void FASTmergeRWNSLB_SLBNSLBPMe_SE_64PUT(sim *simPtr);





//NSLBPM
void FASTwriteSectorNSLBPM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTwriteSectorNSLBPMRAM_WriteAllOut(sim *simPtr);
void FASTwriteSectorNSLBPMRAM_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTmergeNormalNSLB(sim *simPtr);
void FASTmergeRWNSLBPM(sim *simPtr);

void FASTwriteSectorNSLBPM2(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTwriteSectorNSLBPMRAM2_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTwriteSectorNSLBPMRAM2_WriteAllOut(sim *simPtr);
DWORD WriteDB_SE2(sim *simPtr, DWORD PMmerge, DWORD BandNo);
void FASTmergeNormalNSLB2(sim *simPtr);
void FASTmergeRWNSLBPM2(sim *simPtr);


void NSLBPMRAM3_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void SLB2_TrackRMWtime(sim *simPtr, DWORD Max_Size);
void LongWriteRequest2LB(sim *simPtr, DWORD offsetSector, DWORD lenSector);

void FASTwriteSectorNSLBPM3(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTwriteSectorNSLBPMRAM3_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTwriteSectorNSLBPMRAM3_WriteAllOut(sim *simPtr);
void FASTmergeNormalNSLB3(sim *simPtr);
void FASTmergeRWNSLBPM3(sim *simPtr);


void NSLB2_TrackRMWtime(sim *simPtr, DWORD Max_Size);
void FASTwriteSectorNSLBpart23_3_RMW(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTmergeRWNSLBpart23_3_SE(sim *simPtr);
void FASTwriteSectorNSLBRAM2_RMW_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector);

void NSLB_NoRAM(sim *simPtr, DWORD offsetSector, DWORD lenSector);
void FASTmergeRWNSLBpart23_3(sim *simPtr);
#endif







void FASTfree(sim *simPtr);
void FASTconvertReq(sim *simPtr,DWORD *offsetSector,DWORD *lenSector);
void init(sim *simPtr);

//DWORD getpage(sim *simPtr, DWORD SectorTableNo);
DWORD getsection(sim *simPtr, DWORD SectorTableNo);
DWORD getblock(sim *simPtr, DWORD SectorTableNo);
DWORD getholepage(sim *simPtr, DWORD SectorTableNo);
DWORD getholeblock(sim *simPtr, DWORD SectorTableNo);

//void setpage(sim *simPtr, DWORD SectorTableNo, DWORD pageNo);
void setsection(sim *simPtr, DWORD SectorTableNo, DWORD sectionNo);
void setblock(sim *simPtr, DWORD SectorTableNo, DWORD blockNo);
void setholepage(sim *simPtr, DWORD SectorTableNo, DWORD pageNo);
void setholeblock(sim *simPtr, DWORD SectorTableNo, DWORD blockNo);
