#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"

#include <time.h>


/*char str[20] = ".txt", file_name[20] = "Band_Flow", tmp[20] = "";
sprintf(tmp, "%lu", i); strcat(file_name, tmp); strcat(file_name, str);
DWORD SUM_MergeFreq = 0, SUM_Dirty_Cylinder = 0;
for (j = 0; j < NC->blockSizeCylinder; j++){
assert(N->SHUFFLE_TL_OBSERVE[i].MergeFreq[j] >= 0);
if (N->SHUFFLE_TL_OBSERVE[i].MergeFreq[j] > 0){
SUM_MergeFreq += N->SHUFFLE_TL_OBSERVE[i].MergeFreq[j]; SUM_Dirty_Cylinder++;
assert(N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j] > 0); assert(N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j] > 0);
}
if (N->SHUFFLE_TL_OBSERVE[i].MergeFreq[j] > 0) fprintf(fp_Band_Flow, "%.3lf ", (double)N->SHUFFLE_TL_OBSERVE[i].MergeFreq[j] / N->PMstatistic_DB[i]);
else fprintf(fp_Band_Flow, "0 ");
}assert(SUM_Dirty_Cylinder <= 32);
fprintf(fp_Band_Flow, "%.3lf %lu\n", (double)SUM_MergeFreq / N->PMstatistic_DB[i], SUM_Dirty_Cylinder);*/
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
void PRINT_Simulation_SLB_OBSERVE(sim *simPtr){
	NFTL *N = &(simPtr->NFTLobj);
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj);
	DWORD i, j, k;
	DWORD DB_WriteFreq_SUM = 0, DB_DirtyUnit_SUM = 0;
	FILE *fp_SLB_OBSERVE = fopen("SLB_OBSERVE.txt", "w");
	FILE *fp_Band_Flow = fopen("Band_Flow.txt", "w");
	
	DWORD Dirty_Cylinder = 0;
	DWORD Delay_Band16 = 0, Sequential_Band16 = 0;
	DWORD Merge_Bandcount = 0, Merge_Delay_Band16count = 0;//N->PMstatistic_DB[dataBlock]
	              //統計整體情況//Delay_Band情況       //Sequential_Band情況       //Delay_Band2情況       //Sequential_Band情況
	TRANSFOR_LAYER SLB_DB_Info, SLB_Delay_Band16_Info, SLB_Sequential_Band16_Info;
	//統計整體情況
	SLB_DB_Info.WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	SLB_DB_Info.DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	//Delay_Band情況
	SLB_Delay_Band16_Info.WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	SLB_Delay_Band16_Info.DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	//Sequential_Band情況
	SLB_Sequential_Band16_Info.WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	SLB_Sequential_Band16_Info.DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	for (i = 0; i < NC->blockSizeCylinder; i++){ 
		SLB_DB_Info.WriteFreq[i] = 0; SLB_DB_Info.DirtyUnit[i] = 0;//整體
		SLB_Delay_Band16_Info.WriteFreq[i] = 0; SLB_Delay_Band16_Info.DirtyUnit[i] = 0;//分類
		SLB_Sequential_Band16_Info.WriteFreq[i] = 0; SLB_Sequential_Band16_Info.DirtyUnit[i] = 0;
	}

	//觀察Band內段落性聚集寫入特性
	DWORD dbDelay_Band16 = 0, dbSequential_Band16 = 0;
	DWORD dbDirty_Cylinder = 0;
	MERGE_PERIOD_LOCALITY SLB_LOCALITY_Info, SL_DELA16, SL_SEQ16;
	//統計整體情況
	SLB_LOCALITY_Info.CUMU_TIME = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD)); SLB_LOCALITY_Info.PERIOD = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	SL_DELA16.CUMU_TIME = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD)); SL_DELA16.PERIOD = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	SL_SEQ16.CUMU_TIME = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD)); SL_SEQ16.PERIOD = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	for (i = 0; i < NC->blockSizeCylinder; i++){
		SLB_LOCALITY_Info.CUMU_TIME[i] = 0; SLB_LOCALITY_Info.PERIOD[i] = 0;//整體
		SL_DELA16.CUMU_TIME[i] = 0; SL_DELA16.PERIOD[i] = 0; //分類
		SL_SEQ16.CUMU_TIME[i] = 0; SL_SEQ16.PERIOD[i] = 0;
	}

	for (i = 0; i < NC->LsizeBlock; i++){
		if (N->PMstatistic_DB[i] > 0){
			j = 0;//找出DB上第一條有髒資料的P address Cylinder
			while (N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j] == 0){ j++; }//SHUFFLE_TL[dataBlock].DirtyUnit找出第一條髒cylinder
			assert(j <= NC->blockSizeCylinder - 1); assert(N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j] > 0);
			//初始化start_Sector, start_Track, start_Cylinder, 提供之後算時間
			const DWORD start_Cylinder = j; //找出第一條髒cylinder上的第一條髒track和髒sector
			//觀察Band內段落性聚集寫入特性
			for (j = 0; j < NC->blockSizeCylinder; j++){ //先把BAND_MPL未完成的統計做玩再參加排序
				if (N->BAND_MPL[i].TEMP_TIME[j] > 0){//把留在裡面的TEMP_TIME也算進來才完整
					N->BAND_MPL[i].CUMU_TIME[j] += N->BAND_MPL[i].TEMP_TIME[j];
					N->BAND_MPL[i].PERIOD[j]++;
					N->BAND_MPL[i].TEMP_TIME[j] = 0;
				}
			}
			//剛剛已排除DB前段CR(clean region), 開始Shuffle整個DB, Cylinder依照優先權排順序
			for (j = NC->blockSizeCylinder - 1; j > start_Cylinder; j--){//j, k 都是P裡的位置
				for (k = 0; k < j; k++){//不可以讓j, k < 0, DWORD沒有負數
					DWORD SHUFFLE_TL_SWAP_SIG = 0;//初始化, 1表示要交換Cylinder 
					assert(0 <= j); assert(j < NC->blockSizeCylinder); assert(0 <= k); assert(k < NC->blockSizeCylinder);
					if (N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j] < N->SHUFFLE_TL_OBSERVE[i].WriteFreq[k]){ SHUFFLE_TL_SWAP_SIG = 1; }
					else if ((N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j] == N->SHUFFLE_TL_OBSERVE[i].WriteFreq[k]) && (N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j] < N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[k])){ SHUFFLE_TL_SWAP_SIG = 1; }
					//用L_Address排序一次, 大的自然在後面 ??

					if (SHUFFLE_TL_SWAP_SIG == 1){
						//初始化:Lj, Pj -> 裝資料A. Lk, Pk -> 裝資料B
						DWORD Pj = j, Pk = k;//只是要看統計, 這裡用L_Address重新排一次, 沒有跟著改L2P mapping會失去正確的mapping
						//備份K位置上的資料
						DWORD tmp_WriteFreq = N->SHUFFLE_TL_OBSERVE[i].WriteFreq[k]; DWORD tmp_DirtyUnit = N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[k];
						DWORD tmpCUMU_TIME = N->BAND_MPL[i].CUMU_TIME[k], tmpTEMP_TIME = N->BAND_MPL[i].TEMP_TIME[k], tmpPERIOD = N->BAND_MPL[i].PERIOD[k];
						DWORD tmpPRE_DirtyUnit = N->BAND_MPL[i].PRE_DirtyUnit[k];
						//觀察Band間Merge過程的變化 k<-j
						N->SHUFFLE_TL_OBSERVE[i].WriteFreq[k] = N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j];
						N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[k] = N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j];
						//觀察Band內段落性聚集寫入特性
						N->BAND_MPL[i].CUMU_TIME[k] = N->BAND_MPL[i].CUMU_TIME[j];
						N->BAND_MPL[i].TEMP_TIME[k] = N->BAND_MPL[i].TEMP_TIME[j];
						N->BAND_MPL[i].PERIOD[k] = N->BAND_MPL[i].PERIOD[j];
						N->BAND_MPL[i].PRE_DirtyUnit[k] = N->BAND_MPL[i].PRE_DirtyUnit[j];
						//觀察Band間Merge過程的變化 j<-tmp
						N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j] = tmp_WriteFreq;
						N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j] = tmp_DirtyUnit;
						//觀察Band內段落性聚集寫入特性
						N->BAND_MPL[i].CUMU_TIME[j] = tmpCUMU_TIME;
						N->BAND_MPL[i].TEMP_TIME[j] = tmpTEMP_TIME;
						N->BAND_MPL[i].PERIOD[j] = tmpPERIOD;
						N->BAND_MPL[i].PRE_DirtyUnit[j] = tmpPRE_DirtyUnit;
					}
				}
				if (j == 0){ break; }//j,k是DWORD, 沒有負數
			}
			//檢查用的變數
			DB_WriteFreq_SUM = 0; DB_DirtyUnit_SUM = 0;
			fprintf(fp_SLB_OBSERVE, "%lu:\n", i);
			//判斷是不是Sequential Band
			Dirty_Cylinder = 0;
			for (j = 0; j < NC->blockSizeCylinder; j++){ if (N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j] > 0){ Dirty_Cylinder++; } }
			if (Dirty_Cylinder <= NC->blockSizeCylinder / 2){ Delay_Band16++; Merge_Delay_Band16count += N->PMstatistic_DB[i]; }//分出兩種Band
			else{ Sequential_Band16++; }
			//用PMstatistic_DB加總各Band做幾次merge
			Merge_Bandcount += N->PMstatistic_DB[i];
			for (j = 0; j < NC->blockSizeCylinder; j++){
				if (N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j] > 0){ assert(N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j] > 0); }//安全檢查
				SLB_DB_Info.WriteFreq[j] += N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j];//整體
				DB_WriteFreq_SUM += N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j];//統計單一DB
				//分類
				if (Dirty_Cylinder <= NC->blockSizeCylinder / 2){ SLB_Delay_Band16_Info.WriteFreq[j] += N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j]; }
				else{ SLB_Sequential_Band16_Info.WriteFreq[j] += N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j]; }
				fprintf(fp_SLB_OBSERVE, "%lu ", N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j]);
			} fprintf(fp_SLB_OBSERVE, "= %lu\n", DB_WriteFreq_SUM);
			for (j = 0; j < NC->blockSizeCylinder; j++){
				if (N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j] > 0){ assert(N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j] > 0); }//安全檢查
				SLB_DB_Info.DirtyUnit[j] += N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j];//整體
				DB_DirtyUnit_SUM += N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j];//統計單一DB
				//分類
				if (Dirty_Cylinder <= NC->blockSizeCylinder / 2){ SLB_Delay_Band16_Info.DirtyUnit[j] += N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j]; }
				else{ SLB_Sequential_Band16_Info.DirtyUnit[j] += N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j]; }
				fprintf(fp_SLB_OBSERVE, "%lu ", N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j]);
			} fprintf(fp_SLB_OBSERVE, "= %lu\n", DB_DirtyUnit_SUM);

			//觀察Band內段落性聚集寫入特性
			dbDirty_Cylinder = 0; 
			for (j = 0; j < NC->blockSizeCylinder; j++){ if (N->BAND_MPL[i].PERIOD[j] > 0){ dbDirty_Cylinder++; } }
			if (dbDirty_Cylinder <= NC->blockSizeCylinder / 2){ dbDelay_Band16++; }//分類
			else{ dbSequential_Band16++; }
			for (j = 0; j < NC->blockSizeCylinder; j++){
				SLB_LOCALITY_Info.CUMU_TIME[j] += N->BAND_MPL[i].CUMU_TIME[j];
				SLB_LOCALITY_Info.PERIOD[j] += N->BAND_MPL[i].PERIOD[j];
				//SL_DELA16, SL_SEQ16
				if (dbDirty_Cylinder <= NC->blockSizeCylinder / 2){
					SL_DELA16.CUMU_TIME[j] += N->BAND_MPL[i].CUMU_TIME[j];
					SL_DELA16.PERIOD[j] += N->BAND_MPL[i].PERIOD[j];
				}else{
					SL_SEQ16.CUMU_TIME[j] += N->BAND_MPL[i].CUMU_TIME[j];
					SL_SEQ16.PERIOD[j] += N->BAND_MPL[i].PERIOD[j];
				}
			} fprintf(fp_SLB_OBSERVE, "= %lu\n", DB_WriteFreq_SUM);
			/*FILE *fp_flow = fopen("flow.txt", "a");
			fprintf(fp_flow, "%lu \n", i);
			for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_flow, "%lu ", N->BAND_MPL[i].CUMU_TIME[j]); } fprintf(fp_flow, "\n");
			for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_flow, "%lu ", N->BAND_MPL[i].PERIOD[j]); } fprintf(fp_flow, "\n");
			fclose(fp_flow);*/
		}
	}
	//觀察Band間Merge過程的變化
	fprintf(fp_SLB_OBSERVE, "SHUFFLE_OBSERVE Info: Band AVG( merge cnt )\n");
	fprintf(fp_SLB_OBSERVE, "Merge_Bandcount: %lu\n", Merge_Bandcount);
	fprintf(fp_SLB_OBSERVE, "WriteFreq: ");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%.3lf ", (double)SLB_DB_Info.WriteFreq[i] / N->Merge_Bandcount); } fprintf(fp_SLB_OBSERVE, "\n");
	fprintf(fp_SLB_OBSERVE, "DirtyUnit: ");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%.3lf ", (double)SLB_DB_Info.DirtyUnit[i] / N->Merge_Bandcount); } fprintf(fp_SLB_OBSERVE, "\n");
	//分類
	fprintf(fp_SLB_OBSERVE, "Merge_Delay_Bandcount: %lu\n", Merge_Delay_Band16count);
	fprintf(fp_SLB_OBSERVE, "WriteFreq: ");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%.3lf ", (double)SLB_Delay_Band16_Info.WriteFreq[i] / Merge_Delay_Band16count); } fprintf(fp_SLB_OBSERVE, "\n");
	fprintf(fp_SLB_OBSERVE, "DirtyUnit: ");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%.3lf ", (double)SLB_Delay_Band16_Info.DirtyUnit[i] / Merge_Delay_Band16count); } fprintf(fp_SLB_OBSERVE, "\n");
	fprintf(fp_SLB_OBSERVE, "Merge_Seq_Bandcount: %lu\n", N->Merge_Bandcount - Merge_Delay_Band16count);
	fprintf(fp_SLB_OBSERVE, "WriteFreq: ");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%.3lf ", (double)SLB_Sequential_Band16_Info.WriteFreq[i] / (N->Merge_Bandcount - Merge_Delay_Band16count)); } fprintf(fp_SLB_OBSERVE, "\n");
	fprintf(fp_SLB_OBSERVE, "DirtyUnit: ");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%.3lf ", (double)SLB_Sequential_Band16_Info.DirtyUnit[i] / (N->Merge_Bandcount - Merge_Delay_Band16count)); } fprintf(fp_SLB_OBSERVE, "\n");
	
	//觀察Band內段落性聚集寫入特性
	fprintf(fp_SLB_OBSERVE, "Delay_Band16: %lu, Sequential_Band16: %lu\n", dbDelay_Band16, dbSequential_Band16);
	fprintf(fp_SLB_OBSERVE, "MERGE_PERIOD_LOCALITY in Band: \n");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%lu ", SLB_LOCALITY_Info.CUMU_TIME[i]); } fprintf(fp_SLB_OBSERVE, "\n");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%lu ", SLB_LOCALITY_Info.PERIOD[i]); } fprintf(fp_SLB_OBSERVE, "\n");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%.3lf ", (double)SLB_LOCALITY_Info.CUMU_TIME[i] / SLB_LOCALITY_Info.PERIOD[i]); } fprintf(fp_SLB_OBSERVE, "\n");
	//分類
	fprintf(fp_SLB_OBSERVE, "SL_DELA16:\n");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%lu ", SL_DELA16.CUMU_TIME[i]); } fprintf(fp_SLB_OBSERVE, "\n");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%lu ", SL_DELA16.PERIOD[i]); } fprintf(fp_SLB_OBSERVE, "\n");
	for (i = 0; i < 16; i++) fprintf(fp_SLB_OBSERVE, "0 ");
	for (i = 16; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%.3lf ", (double)SL_DELA16.CUMU_TIME[i] / SL_DELA16.PERIOD[i]); } fprintf(fp_SLB_OBSERVE, "\n");
	fprintf(fp_SLB_OBSERVE, "SL_SEQ16:\n");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%lu ", SL_SEQ16.CUMU_TIME[i]); } fprintf(fp_SLB_OBSERVE, "\n");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%lu ", SL_SEQ16.PERIOD[i]); } fprintf(fp_SLB_OBSERVE, "\n");
	for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_SLB_OBSERVE, "%.3lf ", (double)SL_SEQ16.CUMU_TIME[i] / SL_SEQ16.PERIOD[i]); } fprintf(fp_SLB_OBSERVE, "\n");
	
	fclose(fp_SLB_OBSERVE);
	fclose(fp_Band_Flow);
	//觀察Band間Merge過程的變化
	free(SLB_DB_Info.WriteFreq); free(SLB_DB_Info.DirtyUnit);
	free(SLB_Delay_Band16_Info.WriteFreq); free(SLB_Delay_Band16_Info.DirtyUnit);
	free(SLB_Sequential_Band16_Info.WriteFreq); free(SLB_Sequential_Band16_Info.DirtyUnit);
	//觀察Band內段落性聚集寫入特性
	free(SLB_LOCALITY_Info.CUMU_TIME); free(SLB_LOCALITY_Info.PERIOD);
	free(SL_DELA16.CUMU_TIME); free(SL_DELA16.PERIOD);
	free(SL_SEQ16.CUMU_TIME); free(SL_SEQ16.PERIOD);
}
#endif

sim simObj; // the one and only global variable!
main(int argc, char *argv[]) {
	BYTE lineBuffer[1024], op[100], time[100], ampm[100];
	DWORD /*lsn,*/ len, i = 0, j = 0, temp = 0, total = 0, count = 0;	// lsn stands for logical sector number
	I64 lsn;
	//DWORD lsn;
	sim	*S = &(simObj); simConfig *SC = &(simObj.simConfigObj); simStat *SS = &(simObj.simStatObj); NFTL *N = &(simObj.NFTLobj); NFTLconfig *NC = &(simObj.NFTLobj.configObj); NFTLstat *NS = &(simObj.NFTLobj.statObj);
	NFTL_SLB *SLB = &((S->NFTLobj).SLB); NFTL_NSLB *NSLB = &((S->NFTLobj).NSLB);
	int xx = 0, flushedPages = 0, flushedBlocks = 0, prevMiss = 0;
	DWORD k = 0, l = 0;//Statistics_64TrackWrite、SLB_64trackWRITE 在用
	unsigned long long arrtime = 0;
	FILE *fp_BurstLen = fopen("BurstLen.txt", "w");
	FILE *fp_trace = fopen("trace.txt", "w");
	//FILE IO
	fp_MergeMount = fopen("MergeMount.txt", "w");
//#ifdef SLBNSLB_Tmerge
//	NFTL_SLB *SLB = &((S->NFTLobj).SLB); NFTL_NSLB *NSLB = &((S->NFTLobj).NSLB);
//#endif
//#ifdef SLBNSLBPM_Tmerge
//	NFTL_SLB *SLB = &((S->NFTLobj).SLB); NFTL_NSLB *NSLB = &((S->NFTLobj).NSLB);
//#endif

	fp_out = fopen("SectionLocate_num.txt", "w");
#ifdef time_calculate
	//calculate time
	ftime = fopen("time.txt", "w");
	fPM_content = fopen("LRU_Cache_LB.txt", "w");
	clock_t start_time, end_time;
	float total_time = 0;
	start_time = clock(); /* mircosecond */
#endif
#ifdef MergeAssoc_SLBpart
	fp_SLB = fopen("MA_SLB.txt", "w");
	if (fp_SLB == NULL) return;
#endif
#ifdef MergeAssoc_NSLBpart
	fp_NSLB = fopen("MA_NSLB.txt", "w");
	if (fp_NSLB == NULL) return;
#endif
	strcpy(SC->configFile, "_exp.txt");
	memset(SC->configTable, 0, 100 * sizeof(configSlot));
	simInit(S);
	printf("simInit ... \n");

	while (simGetConfig(SC) != FALSE) {
		SC->traceReplay = simGetDWORD(SC, "SIMU", "traceReplay");	// # of trace replay		
		SC->replayReq = simGetDWORD(SC, "SIMU", "replayReq"); // # of reqs to replay
		strcpy(SC->traceFile, simGetString(SC, "SIMU", "traceFile"));	// get the file name of the trace file
		strcpy(SC->Buf_algo, simGetString(SC, "SIMU", "BUFFER"));		// get buffer algo
		strcpy(SC->NFTL_algo, simGetString(SC, "SIMU", "NFTL"));		// get NFTL algo
		S->simStatObj.totalReq = 0;
		S->simStatObj.reqSectorWrite = 0;
		S->simStatObj.writeReq = 0;

		if (strcmp(SC->NFTL_algo, "FAST") == 0)
		{
#ifdef SLB_Mapping_Cache
			printf("Cache init....");
			CacheInit(S);
			printf("Cache init done!\n");
#endif
#ifdef Simulation_HDD
			FASTinitHDD(S);
#endif
#ifdef Simulation_SLB
			//FASTinitSLB(S);
			//FASTinitSLB_NoSt(S);
			FASTinitSLB_NoStNoDB(S);
#endif
#ifdef Simulation_SLB_SHUFFLE
			FASTinitSLB_NoStNoDB(S);
#endif
#ifdef SACE
			FASTinitSLB_NoStNoDB(S);
#endif
#ifdef Simulation_SLBPM
			FASTinitSLB_NoStNoDB(S);
#endif
#ifdef Simulation_NSLB
			//FASTinitNSLB(S);
			//FASTinitNSLB2(S);
			//FASTinitNSLB3(S);
			FASTinitNSLB_NoDB(S);
#endif
#ifdef Simulation_HLB
			FASTinitHLB(S);
#endif
#ifdef SLBNSLB_Tmerge
			//FASTinitSLBNSLB(S);
#endif
#ifdef SLBNSLBPM_Tmerge
			FASTinitSLBNSLB(S);
#endif
#ifdef READ_REQUEST //SLB_SHUFFLE_DELAY共用Read
			FASTinitRead(S);
#endif
#ifdef REQUEST_CHANGE
			//RC_REQ_TYPE;
			N->RC_R_cnt = 0; N->RC_W_cnt = 0;
			N->RC_R_Finish_Time = 0; N->RC_W_Finish_Time = 0;
			N->RC_R_WaitingTime = 0; N->RC_W_WaitingTime = 0;
#endif
		}
		else printf("Warning:Not found the NFTL algo, Maybe typing error \n",0);

		if (strcmp(SC->Buf_algo, "CLOSE") == 0) {} // close buffer
		else printf("Warning:Not found the  Buffer algo, Maybe typing error \n",0);

		printf("tp=%lu \n", SC->traceReplay); //getch();
		for (i = 0; i<SC->traceReplay; i++){// traceReplay must be a signed integer
			NS->line=0;

			printf("replay x %lu\n", i);
			SC->traceFp = fopen(SC->traceFile, "rt"); assert(SC->traceFp != NULL);
			while (!feof(SC->traceFp)) {
				NS->line++;
				//if (NS->line % 1000000 == 0) printf("line%I64u,mc=%lu cut, N write %I64u, SLB write %I64u, NSLB write %I64u\n",NS->line,NS->mergecnt, NS->pageWrite, SLB->pageWrite, NSLB->pageWrite);

				if (SC->replayReq != 0)	if (SS->totalReq > SC->replayReq)	break;
				fgets(lineBuffer, 1024, SC->traceFp);
				sscanf(lineBuffer, "%s %llu %s %I64u %lu", ampm, &arrtime, op, &lsn, &len);
				//sscanf(lineBuffer, "%s %s %s %d %d", ampm, time, op, &lsn, &len);
				//sscanf(lineBuffer, "%s %s %s %I64u %d", ampm, time, op, &lsn, &len);
				//printf("%s %llu %s %lu %lu\n", ampm, arrtime, op, lsn, len); system("pause");
				
				fflush(stdout);

				if (lsn + (I64)len - 1 >= NC->LsizeSector) { continue; } // out of range!
				//if ((len > 2048) || (len == 0))	{ continue; }
				SS->totalReq++;
				
				if (op[0] == 'R') {
#ifdef READ_REQUEST
					if (SS->reqSectorRead_max < len){ SS->reqSectorRead_max = len; }
					SS->readReq++;
					SS->reqSectorRead += len;

					arrtime += N->BaseTime;
					N->Arr_Time = arrtime;
					
					if (N->Resp_Start == 1 || arrtime > N->Finish_Time){ N->Resp_Start = 0; N->Finish_Time = arrtime; }//Response Time
					N->Req_RunTime = 0;
					//DM_SMRreadSector(S, lsn, len);
					DM_SMRreadSector2(S, lsn, len);
#ifdef REQUEST_CHANGE //RC_REQ_TYPE: 0 read, 1 write
					//W造成R等待
					if (arrtime < N->RC_W_Finish_Time){
						N->RC_R_WaitingTime += (N->RC_W_Finish_Time - arrtime);
						N->RC_R_cnt++;
					}//更新R的最新結尾時間
					N->RC_R_Finish_Time = N->Finish_Time;
#endif
#endif
				} else if (op[0] == 'W') {
					if (strcmp(SC->Buf_algo, "CLOSE") == 0) {	//沒buffer
						if (strcmp(SC->NFTL_algo, "FAST") == 0) {
#ifdef Simulation_HDD
							//fprintf(fp_trace, "%llu %lu %lu, ", arrtime, lsn, len);
							N->Arr_Time = arrtime;
							if (N->Resp_Start == 1 || arrtime > N->Finish_Time){ N->Resp_Start = 0; N->Finish_Time = arrtime; }
							N->Req_RunTime = 0;

							HDD(S, lsn, len);
							//fprintf(fp_trace, "%lf - %lf - %lf = %lf %lf\n", N->Finish_Time, N->Arr_Time, N->Req_RunTime, N->Finish_Time - N->Arr_Time - N->Req_RunTime, N->RespTIme);
							//HDD_RAM(S, lsn, len);//on-board request queue
#endif
#ifdef Simulation_SLB
							arrtime += N->BaseTime;
							N->Arr_Time = arrtime;
							if (N->Resp_Start == 1 || arrtime > N->Finish_Time){ N->Resp_Start = 0; N->Finish_Time = arrtime; }
							N->Req_RunTime = 0;

							SLB_NoRAM_NoStNoDB(S, lsn, len);
							//for (j = lsn; j < lsn + len; j++){ N->DirtySector[j] = 1; }
#endif
#ifdef Simulation_SLB_SHUFFLE
							arrtime += N->BaseTime;
							assert(N->PreArr_Time <= arrtime);
							N->Arr_Time = arrtime;
							N->PreArr_Time = arrtime;//
							if (N->Resp_Start == 1 || arrtime > N->Finish_Time){ N->Resp_Start = 0; N->Finish_Time = arrtime; }
							N->Req_RunTime = 0;

							//SLB_NoRAM_NoStNoDB(S, lsn, len);//幫Read開 SHUFFLE_CYLINDER //開MOST_FREQ_MERGE_BAND_OBSERVE, 觀察做SHuffle的潛力
							//DM_SMR_SHUFFLE_TRACKwriteSector(S, lsn, len);//幫Read開 SHUFFLE_TRACK //開MOST_FREQ_MERGE_BAND_OBSERVE_TK,但不要開POWERFAIL 觀察做SHuffle_TK的潛力
							//DM_SMR_ST_VTLMAwriteSector(S, lsn, len);
#ifdef Simulation_SLB_DELAY
							//DM_SMR_DELAY_SHUFFLEwriteSector(S, lsn, len);
							//DM_SMR_DELAY_LEFT_SHUFFLEwriteSector(S, lsn, len);//幫Read開 SHUFFLE_CYLINDER
							//DM_SMR_DELAY_LEFT_SHUFFLE_TRACKwriteSector(S, lsn, len);//幫Read開 SHUFFLE_TRACK
							DM_SMR_DLST_VTLMAwriteSector(S, lsn, len);//Victim Track + Limited Merge Associativity
#endif
							//for (j = lsn; j < lsn + len; j++){ N->DirtySector[j] = 1; }
#endif
#ifdef SACE
							arrtime += N->BaseTime;
							N->Arr_Time = arrtime;
							if (N->Resp_Start == 1 || arrtime > N->Finish_Time){ N->Resp_Start = 0; N->Finish_Time = arrtime; }
							N->Req_RunTime = 0;
							//ICCD 2017 Shingle Aware Cache Management
							SACEwriteSector_NoStNoDB(S, lsn, len);
#endif
#ifdef REQUEST_CHANGE
							//R造成W等待
							if (arrtime < N->RC_R_Finish_Time){
								N->RC_W_WaitingTime += (N->RC_R_Finish_Time - arrtime);
								N->RC_W_cnt++;
							}//更新W的最新結尾時間
							N->RC_W_Finish_Time = N->Finish_Time;
#endif


#ifdef Simulation_SLBPM
							arrtime += N->BaseTime;
							N->Arr_Time = arrtime;
							if (N->Resp_Start == 1 || arrtime > N->Finish_Time){ N->Resp_Start = 0; N->Finish_Time = arrtime; }
							N->Req_RunTime = 0;
							SLB_NoRAM_NoStNoDB(S, lsn, len);
							for (j = lsn; j < lsn + len; j++){ N->DirtySector[j] = 1; }
#endif
#ifdef Simulation_NSLB
							//printf("%llu %s %lu %lu\n", arrtime, op, lsn, len); system("pause");
							arrtime += N->BaseTime;
							N->Arr_Time = arrtime;
							if (N->Resp_Start == 1 || arrtime > N->Finish_Time){ N->Resp_Start = 0; N->Finish_Time = arrtime; }
							N->Req_RunTime = 0;
							
							//NSLB_NoRAM(S, lsn, len);
							//FASTwriteSectorNSLBpart23_3_RMW(S, lsn, len);
							//DM_NSLBwriteSector(S, lsn, len);
							//DM_NSLBwriteSector2(S, lsn, len);
							//DM_NSLBwriteSector3_ExcluNSLBTK(S, lsn, len);
							DM_NSLBwriteSector_NoDB(S, lsn, len);
							//DM_NSLBwriteSector_NoDB_WriteCurr(S, lsn, len);
							for (j = lsn; j < lsn + len; j++){ N->DirtySector[j] = 1; }

							//if (len >= 512 && (1 < N->PM_Rate && N->PartialMerge_Threshold <= 32)){
							//	LongWriteRequest2LB(S, lsn, len);//特殊處理sequential write，送NSLB
							//}else{
							//	FASTwriteSectorNSLBPMRAM3_STM(S, lsn, len);//有做RMW
							//}

							//FASTwriteSectorNSLBRAM2_RMW_STM(S, lsn, len);
#endif
#ifdef Simulation_HLB
							N->Arr_Time = arrtime;
							if (N->Resp_Start == 1 || arrtime > N->Finish_Time){ N->Resp_Start = 0; N->Finish_Time = arrtime; }
							N->Req_RunTime = 0;
							//先寫NSLB
							//DM_NSLBwriteSector(S, lsn, len);
							//依照req性質寫入SLB或NSLB
							DM_ReqDependw2(S, lsn, len);
							//fprintf(fp_trace, "%lf - %lf - %lf = %lf %lf\n", N->Finish_Time, N->Arr_Time, N->Req_RunTime, N->Finish_Time - N->Arr_Time - N->Req_RunTime, N->RespTIme);

#endif
#ifdef SLBNSLB_Tmerge			
#ifdef SpecificTrack
								//使用
								//NSLB_NoRAM(S, lsn, len);
								//FASTwriteSectorNSLBRAM2_RMW_STM(S, lsn, len);//有做RMW
								//FASTwriteSectorNSLBRAM2_SE_RMW_STM(S, lsn, len);//有做RMW

								//不用
								//FASTwriteSectorHLBRAM_RMW_STM(S, lsn, len, 0);//使用

								//hit rate
								/*if (SS->writeReq % 200000 == 0){
									fprintf(ftime, "%I64u %I64u %I64u %I64u\n", N->cycleSLB_Hit, N->cycleSLB_Miss, N->cycleNSLB_Hit, N->cycleNSLB_Miss);
									N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
									N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;
								}*/

								//burst
								/*if (SS->writeReq % 270000 == 0){
									fprintf(fp_BurstLen, "%I64u %I64u\n", N->BurstLen_Sum, N->BurstLen_Max);
									N->BurstLen_Sum = 0;
									N->BurstLen_Max = 0;
									N->BurstLen = 0;
									N->BurstLen_Start = 0;
									N->BurstLen_End = 0;
								}
								if (N->BurstLen_End + 1 == lsn){
									N->BurstLen += len;
									N->BurstLen_Start = lsn;
									N->BurstLen_End = lsn + len - 1;
								}else{
									N->BurstLen_Sum += (I64)N->BurstLen;
									if (N->BurstLen > N->BurstLen_Max){ N->BurstLen_Max = N->BurstLen; }
									N->BurstLen = len;
									N->BurstLen_Start = lsn;
									N->BurstLen_End = lsn + len - 1;
								}*/

								//不使用
								/*if (SS->writeReq % 120000 == 0){
								if (N->ValidIs0 > 0 || N->ValidIs2 > 0){
								fprintf(fp_MergeMount, "%I64u %I64u %I64u %I64u\n", N->ValidIs0, N->ValidIs2, N->ValidIs0_max, N->ValidIs2_max);
								}
								N->ValidIs0 = 0; N->ValidIs2 = 0;
								N->ValidIs0_max = 0; N->ValidIs2_max = 0;
								}*/

#endif
								////PM
								//FASTwriteSectorSLBpart_PM_64PUT_SM(S, lsn, len);//only PM method
								////HLB+
								////FASTwriteSectorSLBNSLB_SLBNSLBPM_SE_64PUT_SM(S, lsn, len);//HLB+PM method
#endif
#ifdef SLBNSLBPM_Tmerge
							NSLBPMRAM3_NoRAM(S, lsn, len);
#ifdef Section_Mapping
								//新
								//FASTwriteSectorNSLBPMRAM_STM(S, lsn, len);//有做
								//FASTwriteSectorNSLBPMRAM2_STM(S, lsn, len);//有做RMW
								//FASTwriteSectorNSLBPMRAM3_STM(S, lsn, len);//有做RMW
								NSLBPMRAM3_NoRAM(S, lsn, len);
								//NSLB_NoRAM(S, lsn, len);

								//if (len >= 512 && (1 < N->PM_Rate && N->PartialMerge_Threshold <= 32)){
								//	LongWriteRequest2LB(S, lsn, len);//特殊處理sequential write，送NSLB
								//}else{
								//	FASTwriteSectorNSLBPMRAM3_STM(S, lsn, len);//有做RMW
								//}

								//FASTwriteSectorNSLBRAM2_RMW_STM(S, lsn, len);
#endif
#endif	
						}
					} else {}									//有buffer
					SS->writeReq++;
					SS->reqSectorWrite += (I64)len;
				}
			}
			fprintf(fp_trace, "replay x %lu\n", i);
			fprintf(fp_trace, "N->RespTIme:        %llu\n", N->RespTIme);
			fprintf(fp_trace, "N->WaitingTime:     %llu\n", N->WaitingTime);
			fprintf(fp_trace, "N->RunTime:         %llu\n", N->RunTime);
			//統計Effect Merge會寫多少Data Cylinder
			fprintf(fp_trace, "EffectMerge_DataCylinder\n");
			for(j = 0;j < N->EM_DataCylinder_Len;j++){ fprintf(fp_trace, "%lu ", N->EM_DataCylinder[j]); }
			fprintf(fp_trace, "\n");
#ifdef Simulation_SLBPM
			fprintf(fp_trace, "N->GET_FREE_LB_SECT_SUM: %llu\n", N->GET_FREE_LB_SECT_SUM);
			fprintf(fp_trace, "N->GET_FREE_LB_SECT_cnt: %llu\n", N->GET_FREE_LB_SECT_cnt);
			fprintf(fp_trace, "SLB->SLBcylinder_num:    %llu\n", SLB->SLBcylinder_num);
			fprintf(fp_trace, "N->MAX_PM_CYLINDER:      %llu\n", N->MAX_PM_CYLINDER);
			fprintf(fp_trace, "N->NUM_PM_CYLINDER:      %llu\n", N->NUM_PM_CYLINDER);
			fprintf(fp_trace, "N->NUM_PM_CYLINDER_Rate: %llu\n", N->NUM_PM_CYLINDER_Rate);
			DWORD live_sector = 0;
			for (j = 0; j <= SLB->Partial_Section; j++){
				if (N->blocks[NC->PsizeBlock - 1].sections[j].valid == 1) live_sector++;
			}
			fprintf(fp_trace, "sector:live sector %lu %lu\n", SLB->Partial_Section, live_sector);
			fprintf(fp_trace, "GCyl_sector:%lu LB_GCyl_sector:%lu WB_LB_GCyl_sector:%lu\n", N->GCyl_sector, N->LB_GCyl_sector, N->WB_LB_GCyl_sector);
			fprintf(fp_trace, "CUTPM_SIG0_Rate:%lu 1:%lu 2:%lu\n", N->CUTPM_SIG0_Rate, N->CUTPM_SIG1_Rate, N->CUTPM_SIG2_Rate);
			
#endif
			fprintf(fp_trace, "---\n");
#ifdef time_calculate
			unsigned long long SeekTime = N->Write_W_SeekTime + N->Write_R_SeekTime + N->Merge_W_SeekTime + N->Merge_R_SeekTime + N->WB_SeekTime;
			fprintf(ftime, "replay x %lu\n", i);
			fprintf(ftime, "Write_W_SeekTime:   %llu\n", N->Write_W_SeekTime);
			fprintf(ftime, "Write_R_SeekTime:   %llu\n", N->Write_R_SeekTime);
			fprintf(ftime, "Write_RDB_SeekTime: %llu\n", N->Write_RDB_SeekTime);
			fprintf(ftime, "Write_RLB_SeekTime: %llu\n", N->Write_RLB_SeekTime);
			fprintf(ftime, "Merge_W_SeekTime:   %llu\n", N->Merge_W_SeekTime);
			fprintf(ftime, "Merge_R_SeekTime:   %llu\n", N->Merge_R_SeekTime);
			fprintf(ftime, "Merge_RLB_SeekTime: %llu\n", N->Merge_RLB_SeekTime);
			fprintf(ftime, "WB_SeekTime:        %llu\n", N->WB_SeekTime);//
#ifdef POWERFAIL
			SeekTime += N->PF_SeekTime;//
			fprintf(ftime, "PF_SeekTime:        %llu\n", N->PF_SeekTime);//
#endif
#ifdef READ_REQUEST
			SeekTime += N->Read_SeekTime;
			fprintf(ftime, "Read_SeekTime:      %llu\n", N->Read_SeekTime);//
#endif
			fprintf(ftime, "Sum_SeekTime:       %llu\n", SeekTime);
			fprintf(ftime, "---\n");
#endif
			//Trace Replay需要把前一輪的Finish_Time累加進總時間
			N->BaseTime += N->Finish_Time;
			fclose(SC->traceFp);
			//write S everytime. total read = NFTL read + SLB read + NSLB read. update the pageWrite and pageRead from BLB or CLB or SLBNSLB
			
			//DWORD write_section=0, sum = 0;
			//for (i = 0; i<N->track_valid_use; i++) {
			//	fprintf(fp_out, "%8lu: ", N->track_valid[i]);
			//	for (j = 0; j<64; j++) {
			//		if (N->track_valid_section[i][j]>0) {
			//			write_section++;
			//		}
			//		fprintf(fp_out,"%d ", N->track_valid_section[i][j]);
			//		/*if (N->track_valid_section[i][j]>0){ fprintf(fp_out, "1 "); }
			//		else{ fprintf(fp_out, "0 "); }*/
			//	}
			//	sum++;
			//	fprintf(fp_out, "\n");
			//}
			//fclose(fp_out);
			//printf("sum %lu, write_section %lu, req_write_section %lu\n", sum, write_section, N->req_write_section);
			//system("pause");

			//HDD_RAM_WriteAllOut(S);
			//FASTwriteSectorSLBRAM2_WriteAllOut(S);//有RAM有write buff
			//FASTwriteSectorNSLBRAM2_WriteAllOut_RMW(S);//有做RMW //把RAM剩餘資料寫入NSLB專用

			N->min_cyc = -1; N->max_cyc = -1;
			/*for (i = 0; i < NC->LsizeBlock; i++){//HDD不能用
				if (N->b_time[i] > 0){ 
					assert(N->b[i] > 0);
					double tmp_cyc = (double)N->b_time[i] / N->b[i];
					if (N->min_cyc == -1 || N->min_cyc > tmp_cyc){ N->min_cyc = tmp_cyc; }
					if (N->max_cyc == -1 || N->max_cyc < tmp_cyc){ N->max_cyc = tmp_cyc; }
					assert(N->min_cyc <= N->max_cyc);
				}
			}*/

			simWriteLog(S);
			printf("\nwr=%I64u, tr=%I64u\n", SS->writeReq, SS->totalReq);
			printf("replay %lu, wr=%I64u, tr=%I64u\n", i, SS->writeReq, SS->totalReq);
			printf("read:%I64u\twrite:%I64u\n", NS->pageRead, NS->pageWrite);
#ifdef time_calculate
			end_time = clock();
			/* CLOCKS_PER_SEC is defined at time.h */
			total_time = (float)(end_time - start_time) / CLOCKS_PER_SEC;
			printf("Time : %f sec \n", total_time);
			fprintf(ftime, "Time : %f sec , NS->mergecnt:%I64u, SLBws %lu, SLBms %lu, NSLBws %lu, NSLBms %lu\n", total_time, NS->mergecnt, N->SLBws, N->SLBms, N->NSLBws, N->NSLBms);
			fprintf(ftime, "%lu %lu %lu %lu\n", N->SLBtrackChange,N->RDB,N->RMW,N->Merge);
			
			//fprintf(ftime, "N->req_w_cumu %I64u\n", N->req_w_cumu);
			//for (j = 0; j < NC->LsizeSection; j++){
			//	if (N->section_cumu[j] > 0){
			//		//assert(N->section_cumu[i] = N->SLB_section_cumu[i] + N->DB_section_cumu[i]);
			//		N->req_w_cumu += (I64)N->section_cumu[j];
			//	}
			//}
			//fprintf(ftime, "All N->req_w_cumu %I64u\n", N->req_w_cumu);
			//DWORD SLB_restValid = 0;
			//DWORD dataBlock = 0, section = 0;
			//for (j = 0; j <= SLB->Partial_Section; j++){
			//	if (N->blocks[NC->PsizeBlock - 1].sections[j].valid == 1){
			//		assert(N->section_cumu[N->blocks[NC->PsizeBlock - 1].sections[j].sector / NC->Section] > 0);
			//		SLB_restValid++; 
			//	}
			//}
			//fprintf(ftime, "SLB_restValid %lu\n", SLB_restValid);
			/*I64 cumu_sum = 0;
			for (j = 0; j<NC->LsizeSection; j++){
				cumu_sum += N->section_cumu[j];
			}
			fprintf(ftime, "cumu_sum %I64u\n", cumu_sum);*/
			//assert((N->Cache_Hit + N->Cache_Miss) != 0);
#ifdef SLB_Mapping_Cache
			//fprintf(fPM_content, "Total cache size: %f\n", N->SLB_Map_Cache[0].cache_size+N->SLB_Map_Cache[1].cache_size);
			//fprintf(fPM_content, "L1 size: %f\n", N->SLB_Map_Cache[0].cache_size);
			//fprintf(fPM_content, "L2 size: %f\n", N->SLB_Map_Cache[1].cache_size);
#ifdef CFLRU
			double hit_ratio = (double)(N->Cache_Hit + N->Cache_Hit2)/ (double)(N->Cache_Hit + N->Cache_Hit2 + N->Cache_Read_Miss + N->Cache_Write_Miss);
#else
			double hit_ratio = (double)(N->Cache_Hit) / (double)(N->Cache_Hit + N->Cache_Read_Miss + N->Cache_Write_Miss);
#endif
			fprintf(fPM_content, "Cache hit ratio: %f\n", hit_ratio);
			fprintf(fPM_content, "Cache write miss : %lu\n", N->Cache_Write_Miss);
			fprintf(fPM_content, "Cache read miss : %lu\n", N->Cache_Read_Miss);
			fprintf(fPM_content, "Cache Write L1 : %lu\n", N->Cache_Write);
			fprintf(fPM_content, "Cache Read L1 : %lu\n", N->Cache_Read);
			fprintf(fPM_content, "Cache Merge : %lu\n", N->Cache_Merge);
			fprintf(fPM_content, "Cache delay(delay+LMA) : %lu\n", N->Cache_Alldelay);
			fprintf(fPM_content, "Cache Replace : %lu\n", N->Cache_Replace);
			fprintf(fPM_content, "=========CFLRU==============\n");
			fprintf(fPM_content, "Cache Merge with W : %lu\n", N->Cache_Merge_withW);
			fprintf(fPM_content, "Cache Replace with W : %lu\n", N->Cache_Replace_withW);
			fprintf(fPM_content, "Cache hit L2 : %lu\n", N->Cache_Hit2);
			fprintf(fPM_content, "Cache hit L1 : %lu, (%f)% of all hit\n", N->Cache_Hit, (double)N->Cache_Hit /(double)(N->Cache_Hit + N->Cache_Hit2));
			fprintf(fPM_content, "Cache Write L2 : %lu\n", N->Cache_Write2);
			fprintf(fPM_content, "Cache Read L2 : %lu\n", N->Cache_Read2);
			fprintf(fPM_content, "Cache delay L2 : %lu\n", N->Cache_Alldelay2);
			fprintf(fPM_content, "L1 fall to L2: %lu\n", N->Cache_L1toL2);

			
			Cachefree(S);
			//assert((N->Cache_Hit+N->Cache_Miss) == N->ReqR_pageRead);
#endif
			fprintf(fp_MergeMount, "replay 1 time---------------------\n");
#ifdef SLBNSLBPM_Tmerge
			//fprintf(ftime, "%I64u %I64u %I64u, %I64u\n", N->All_Merge, N->Allow_PM, N->DO_PM, N->PM_len);
			//fprintf(ftime, "%I64u %I64u %I64u\n", N->Rel_len, N->Miss_PM_len, N->Miss_Rel_len);
#endif
#endif
		}//end trace replay round
		//FASTwriteSectorSLBRAM2_WriteAllOut(S);//有做RMW
#ifdef SpecificTrack
		//FASTwriteSectorNSLBRAM2_WriteAllOut_RMW(S);//有做RMW //把RAM剩餘資料寫入NSLB專用
		//FASTwriteSectorNSLBRAM2_WriteAllOut_SE_RMW(S);//有做RMW //把RAM剩餘資料寫入NSLB專用
#endif
#ifdef SLBNSLBPM_Tmerge
		//FASTwriteSectorPM_RAM_WriteAllOut(S);
		//FASTwriteSectorHLBPlus_RAM_WriteAllOut(S);
		//FASTwriteSectorNSLBPMRAM_WriteAllOut(S);//有做RMW //把RAM剩餘資料寫入NSLB
		//FASTwriteSectorNSLBPMRAM2_WriteAllOut(S);//有做RMW //把RAM剩餘資料寫入NSLB專用
		//FASTwriteSectorNSLBPMRAM3_WriteAllOut(S);//有做RMW //把RAM剩餘資料寫入NSLB專用
#endif
		//simWriteLog(S);
		printf("wr=%I64u, tr=%I64u\n",SS->writeReq,SS->totalReq);
		printf("read:%I64u\twrite:%I64u\n", NS->pageRead + SLB->pageRead + NSLB->pageRead, NS->pageWrite + SLB->pageWrite + NSLB->pageWrite);//NS->pageRead ,NS->pageWrite
		//printf("read:%I64u\twrite:%I64u\n", NS->pageRead_overhead, NS->pageWrite_overhead);
		
#ifdef Simulation_HDD
		HDDfree(S);
#endif
#ifdef Simulation_SLB
		//SLBfree(S);
		//SLBfree_NoSt(S);

		//SLB統計PM會merge到那些Cylinder上
		fprintf(fp_trace, "LB_DirtySectors          %lu\n", N->LB_DirtySectors);
		for (i = 0; i < NC->LsizeBlock; i++){ 
			if (N->PMstatistic_DB[i] > 0){
				N->CheckMerge_Bandcount += N->PMstatistic_DB[i];
				N->Merge_UniqBand++;
			} 
		}
		fprintf(fp_trace, "CheckMerge_Bandcount     %lu\n", N->CheckMerge_Bandcount);
		fprintf(fp_trace, "Merge_UniqBand           %lu\n", N->Merge_UniqBand);
		  //DB Merge概況
		fprintf(fp_trace, "PM_NoGuard_Cylinders     %lu\n", N->PM_NoGuard_Cylinders);
		fprintf(fp_trace, "Range_Cylinders          %lu\n", N->Range_Cylinders);
		fprintf(fp_trace, "DirtyInRange_Cylinders   %lu\n", N->DirtyInRange_Cylinders);
		  //Clean Region數量和大小
		fprintf(fp_trace, "CleanRegion_CumuSize     %lu\n", N->CleanRegion_CumuSize);
		fprintf(fp_trace, "CleanRegion_Count        %lu\n", N->CleanRegion_Count);
		fprintf(fp_trace, "CleanRegionMAx_CumuSize  %lu\n", N->CleanRegionMAx_CumuSize);
		fprintf(fp_trace, "CleanRegionMAx_Count     %lu\n", N->CleanRegionMAx_Count);
		  //假設做PMG寫入有多少
		fprintf(fp_trace, "PMG_WriteCost_Sum        %lu\n", N->PMG_WriteCost_Sum);
		SLBfree_NoStNoDB(S);
#endif
#ifdef Simulation_SLB_SHUFFLE
		//SLB統計PM會merge到那些Cylinder上
		fprintf(fp_trace, "LB_DirtySectors          %lu\n", N->LB_DirtySectors);
		for (i = 0; i < NC->LsizeBlock; i++){ 
			if (N->PMstatistic_DB[i] > 0){
				N->CheckMerge_Bandcount += N->PMstatistic_DB[i];
				N->Merge_UniqBand++;
			}
		}
		fprintf(fp_trace, "CheckMerge_Bandcount     %lu\n", N->CheckMerge_Bandcount);
		fprintf(fp_trace, "Merge_UniqBand           %lu\n", N->Merge_UniqBand);
		//DB Merge概況
		fprintf(fp_trace, "PM_NoGuard_Cylinders     %lu\n", N->PM_NoGuard_Cylinders);
		fprintf(fp_trace, "Range_Cylinders          %lu\n", N->Range_Cylinders);
		fprintf(fp_trace, "DirtyInRange_Cylinders   %lu\n", N->DirtyInRange_Cylinders);
		//Clean Region數量和大小
		fprintf(fp_trace, "CleanRegion_CumuSize     %lu\n", N->CleanRegion_CumuSize);
		fprintf(fp_trace, "CleanRegion_Count        %lu\n", N->CleanRegion_Count);
		fprintf(fp_trace, "CleanRegionMAx_CumuSize  %lu\n", N->CleanRegionMAx_CumuSize);
		fprintf(fp_trace, "CleanRegionMAx_Count     %lu\n", N->CleanRegionMAx_Count);
		//假設做PMG寫入有多少
		fprintf(fp_trace, "PMG_WriteCost_Sum        %lu\n", N->PMG_WriteCost_Sum);
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
		fprintf(fp_trace, "SHUFFLE_OBSERVE Info 所有Band做平均\n");
		fprintf(fp_trace, "WriteFreq: ");
		for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_trace, "%.3lf ", (double)N->SHUFFLE_DB_Info.WriteFreq[i] / N->Merge_Bandcount); } fprintf(fp_trace, "\n");
		fprintf(fp_trace, "DirtyUnit: ");
		for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_trace, "%.3lf ", (double)N->SHUFFLE_DB_Info.DirtyUnit[i] / N->Merge_Bandcount); } fprintf(fp_trace, "\n");
		
		PRINT_Simulation_SLB_OBSERVE(S);
#endif
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE
		//印出來
		DWORD MFMBand = 0, MFMerge = 1;//避開沒Merge過的 
		DWORD SWAP = 0, tmpMergeFreq, tmpMergeDS;
		DWORD MFMBO_CleanCylinder = 0, tmpCleanCylinder = 0;
		for(i = 0;i < NC->LsizeBlock; i++){
			tmpCleanCylinder = 0;//計算空Cylinder
			if (N->PMstatistic_DB[i] > 0){ assert(N->SHUFFLE_TL_Alloc[i] == 1); 
				for (j = 0; j < NC->blockSizeCylinder; j++){ if (N->MFMBO_INFO[i].MergeFreq[j] == 0){ tmpCleanCylinder++; } }
			}
			if (N->PMstatistic_DB[i] > MFMerge || (N->PMstatistic_DB[i] == MFMerge && tmpCleanCylinder > MFMBO_CleanCylinder)){
				MFMBand = i; MFMerge = N->PMstatistic_DB[i]; MFMBO_CleanCylinder = tmpCleanCylinder;
			}
		}assert(MFMBand < NC->LsizeBlock);
		
		FILE *fp_MFMBO = fopen("MFMBO.txt", "w");
		fprintf(fp_trace, "MFMBand:   %lu\n", MFMBand); fprintf(fp_trace, "PMstatistic_DB:   %lu\n", N->PMstatistic_DB[MFMBand]);
		fprintf(fp_trace, "MergeFreq\n"); for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_trace, "%lu ", N->MFMBO_INFO[MFMBand].MergeFreq[i]); } fprintf(fp_trace, "\n");
		fprintf(fp_trace, "MergeDS\n"); for (i = 0; i < NC->blockSizeCylinder; i++){ fprintf(fp_trace, "%lu ", N->MFMBO_INFO[MFMBand].MergeDS[i]); } fprintf(fp_trace, "\n");

		for (i = 0; i < NC->LsizeBlock; i++){
			if (N->PMstatistic_DB[i]>0){
				for (j = NC->blockSizeCylinder - 1; j >= 1; j--){
					for (k = 0; k < j; k++){
						SWAP = 0;
						if (N->MFMBO_INFO[i].MergeFreq[k] > N->MFMBO_INFO[i].MergeFreq[j]){ SWAP = 1; }
						else if (N->MFMBO_INFO[i].MergeFreq[k] == N->MFMBO_INFO[i].MergeFreq[j] && N->MFMBO_INFO[i].MergeDS[k] > N->MFMBO_INFO[i].MergeDS[j]){ SWAP = 1; }
						if (SWAP == 1){
							tmpMergeFreq = N->MFMBO_INFO[i].MergeFreq[j];
							tmpMergeDS = N->MFMBO_INFO[i].MergeDS[j];
							N->MFMBO_INFO[i].MergeFreq[j] = N->MFMBO_INFO[i].MergeFreq[k];
							N->MFMBO_INFO[i].MergeDS[j] = N->MFMBO_INFO[i].MergeDS[k];
							N->MFMBO_INFO[i].MergeFreq[k] = tmpMergeFreq;
							N->MFMBO_INFO[i].MergeDS[k] = tmpMergeDS;
						}
					}
				}
				fprintf(fp_MFMBO, "%lu MergeFreq, MergeDS\n", i);
				for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_MFMBO, "%lu ", N->MFMBO_INFO[i].MergeFreq[j]); } fprintf(fp_MFMBO, "\n");
				for (j = 0; j < NC->blockSizeCylinder; j++){ fprintf(fp_MFMBO, "%lu ", N->MFMBO_INFO[i].MergeDS[j]); } fprintf(fp_MFMBO, "\n");
			}
		}
		fclose(fp_MFMBO);
#endif
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE_TK
		//印出來
		DWORD MFMBand = 0, MFMerge = 1;//避開沒Merge過的 
		DWORD SWAP = 0, tmpMergeFreq, tmpMergeDS;
		DWORD MFMBO_CleanCylinder = 0, tmpCleanCylinder = 0;
		for(i = 0;i < NC->LsizeBlock; i++){
			tmpCleanCylinder = 0;//計算空Cylinder
			if (N->PMstatistic_DB[i] > 0){ assert(N->SHUFFLE_TL_Alloc[i] == 1); 
			for (j = 0; j < NC->blockSizeTrack; j++){ if (N->MFMBO_INFO[i].MergeFreq[j] == 0){ tmpCleanCylinder++; } }
			}
			if (N->PMstatistic_DB[i] > MFMerge || (N->PMstatistic_DB[i] == MFMerge && tmpCleanCylinder > MFMBO_CleanCylinder)){
				MFMBand = i; MFMerge = N->PMstatistic_DB[i]; MFMBO_CleanCylinder = tmpCleanCylinder;
			}
		}assert(MFMBand < NC->LsizeBlock);

		FILE *fp_MFMBO = fopen("MFMBO_TK.txt", "w");
		fprintf(fp_trace, "MFMBand:   %lu\n", MFMBand); fprintf(fp_trace, "PMstatistic_DB:   %lu\n", N->PMstatistic_DB[MFMBand]);
		fprintf(fp_trace, "MergeFreq\n"); for (i = 0; i < NC->blockSizeTrack; i++){ fprintf(fp_trace, "%lu ", N->MFMBO_INFO[MFMBand].MergeFreq[i]); } fprintf(fp_trace, "\n");
		fprintf(fp_trace, "MergeDS\n"); for (i = 0; i < NC->blockSizeTrack; i++){ fprintf(fp_trace, "%lu ", N->MFMBO_INFO[MFMBand].MergeDS[i]); } fprintf(fp_trace, "\n");

		for (i = 0; i < NC->LsizeBlock; i++){
			if (N->PMstatistic_DB[i]>0){
				for (j = NC->blockSizeTrack - 1; j >= 1; j--){
					for (k = 0; k < j; k++){
						SWAP = 0;
						if (N->MFMBO_INFO[i].MergeFreq[k] > N->MFMBO_INFO[i].MergeFreq[j]){ SWAP = 1; }
						else if (N->MFMBO_INFO[i].MergeFreq[k] == N->MFMBO_INFO[i].MergeFreq[j] && N->MFMBO_INFO[i].MergeDS[k] > N->MFMBO_INFO[i].MergeDS[j]){ SWAP = 1; }
						if (SWAP == 1){
							tmpMergeFreq = N->MFMBO_INFO[i].MergeFreq[j];
							tmpMergeDS = N->MFMBO_INFO[i].MergeDS[j];
							N->MFMBO_INFO[i].MergeFreq[j] = N->MFMBO_INFO[i].MergeFreq[k];
							N->MFMBO_INFO[i].MergeDS[j] = N->MFMBO_INFO[i].MergeDS[k];
							N->MFMBO_INFO[i].MergeFreq[k] = tmpMergeFreq;
							N->MFMBO_INFO[i].MergeDS[k] = tmpMergeDS;
						}
					}
				}
				fprintf(fp_MFMBO, "%lu MergeFreq, MergeDS\n", i);
				for (j = 0; j < NC->blockSizeTrack; j++){ fprintf(fp_MFMBO, "%lu ", N->MFMBO_INFO[i].MergeFreq[j]); } fprintf(fp_MFMBO, "\n");
				for (j = 0; j < NC->blockSizeTrack; j++){ fprintf(fp_MFMBO, "%lu ", N->MFMBO_INFO[i].MergeDS[j]); } fprintf(fp_MFMBO, "\n");
			}
		}
		fclose(fp_MFMBO);
#endif
#ifdef Simulation_SLB_DELAY //Delay資訊
		fprintf(fp_trace, "CumuSLB_DELAY_USE        %lu\n", N->CumuSLB_DELAY_USE);
		fprintf(fp_trace, "CumuSLB_USE              %lu\n", N->CumuSLB_USE);
		fprintf(fp_trace, "CumuSLB_DELAY_BAND       %lu\n", N->CumuSLB_DELAY_BAND);
		fprintf(fp_trace, "cumuVC_BUFF_Len          %lu\n", N->cumuVC_BUFF_Len);
		fprintf(fp_trace, "DELAY_SAVE_CYLINDER      %lu\n", N->DELAY_SAVE_CYLINDER);/**/
		fprintf(fp_trace, "DELAY_cnt                %lu\n", N->DELAY_cnt);
		fprintf(fp_trace, "MERGE_cnt                %lu\n", N->MERGE_cnt);
		//三個參數
		fprintf(fp_trace, "SaveCylinder_TD          %lu\n", N->SaveCylinder_TD);
		fprintf(fp_trace, "DirtyUnit_TD             %lu\n", N->DirtyUnit_TD);
		fprintf(fp_trace, "SLB_DELAY_SPACE          %lu\n", N->SLB_DELAY_SPACE);
#endif
#ifdef Limited_MergeAssociativity
		fprintf(fp_trace, "LMA                      %lu\n", Limited_MergeAssociativity);
#endif
#ifdef SLB_Mapping_Cache
		
#endif
		SLBfree_NoStNoDB(S);//FREE 記憶體
#endif
#ifdef SACE
		//統計PM會merge到那些Cylinder上
		fprintf(fp_trace, "LB_DirtySectors          %lu\n", N->LB_DirtySectors);
		for (i = 0; i < NC->LsizeBlock; i++){ 
			if (N->PMstatistic_DB[i] > 0){
				N->CheckMerge_Bandcount += N->PMstatistic_DB[i];
				N->Merge_UniqBand++;
			}
		}
		fprintf(fp_trace, "CheckMerge_Bandcount     %lu\n", N->CheckMerge_Bandcount);
		fprintf(fp_trace, "Merge_UniqBand           %lu\n", N->Merge_UniqBand);
		//DB Merge概況
		fprintf(fp_trace, "PM_NoGuard_Cylinders     %lu\n", N->PM_NoGuard_Cylinders);
		fprintf(fp_trace, "Range_Cylinders          %lu\n", N->Range_Cylinders);
		fprintf(fp_trace, "DirtyInRange_Cylinders   %lu\n", N->DirtyInRange_Cylinders);
		//Clean Region數量和大小
		fprintf(fp_trace, "CleanRegion_CumuSize     %lu\n", N->CleanRegion_CumuSize);
		fprintf(fp_trace, "CleanRegion_Count        %lu\n", N->CleanRegion_Count);
		fprintf(fp_trace, "CleanRegionMAx_CumuSize  %lu\n", N->CleanRegionMAx_CumuSize);
		fprintf(fp_trace, "CleanRegionMAx_Count     %lu\n", N->CleanRegionMAx_Count);
		//假設做PMG寫入有多少
		fprintf(fp_trace, "PMG_WriteCost_Sum        %lu\n", N->PMG_WriteCost_Sum);
		//寫入分類
		fprintf(fp_trace, "WriteBandLC     %lu\n", N->WriteBandLC);//寫在band LattCylinder
		fprintf(fp_trace, "WriteBandBGT    %lu\n", N->WriteBandBGT);//寫在band上BeforeGuardTrack
		fprintf(fp_trace, "WriteSLB        %lu\n", N->WriteSLB);
		fprintf(fp_trace, "MergeBGT        %lu\n", N->MergeBGT);
		fprintf(fp_trace, "Effect_MergeBGT %lu\n", N->Effect_MergeBGT);
		fprintf(fp_trace, "HiddeIn_EMerge  %lu\n", N->HiddeInEffect_Merge);
		SLBfree_NoStNoDB(S); 
#endif



#ifdef Simulation_SLBPM
		DWORD PM_DB = 0;
		for (i = 0; i < NC->LsizeBlock; i++){
			if (N->blocks[i].PM == 1){ PM_DB++; }
		}
		fprintf(fp_trace, "PM_DB:                  %lu\n", PM_DB);
		SLBfree_NoStNoDB(S);
#endif
#ifdef Simulation_NSLB
		//NSLBfree(S);
		NSLBfree_NoDB(S);
#endif
#ifdef Simulation_HLB
		HLBfree(S);
#endif
		//simFree(S);
		//NSLBfree(S);
	}
#ifdef MergeAssoc_SLBpart
	fclose(fp_SLB);
#endif
#ifdef time_calculate
	fclose(ftime);
#endif
	fclose(fPM_content);
	fclose(fp_MergeMount);
	
	DWORD DS = 0;
	//for (i = 0; i < NC->LsizeSector; i++){ if (N->DirtySector[i] == 1){ DS++; } }
	fprintf(fp_trace, "DS:                     %lu\n", DS);
	//free(N->DirtySector);
#ifdef Simulation_NSLB
	fprintf(fp_trace, "N->LBTracks:            %lu\n", N->NSLB_tracks);
	fprintf(fp_trace, "N->NSLBwriteout_sector: %lu\n", N->NSLBwriteout_sector);
	fprintf(fp_trace, "NSLB->NSLBsection_num:  %lu\n", NSLB->NSLBsection_num);
	for (i = 0; i <N->NSLB_tracks; i++){ fprintf(fp_trace, "%lu ", N->NSLBremainSectionTable[i]); }
	free(N->NSLBremainSectionTable);
#endif
#ifdef READ_REQUEST
	fprintf(fp_trace, "Read Request\n");
	fprintf(fp_trace, "max_sector_len:   %lu\n", SS->reqSectorRead_max);
	fprintf(fp_trace, "readReq:          %I64u\n", SS->readReq);
	fprintf(fp_trace, "reqSectorRead:    %I64u\n", SS->reqSectorRead);
	fprintf(fp_trace, "ReqR_pageRead:    %lu\n", N->ReqR_pageRead);
	//fprintf(fp_trace, "R_content_rotate: %llu\n", N->R_content_rotate);
	fprintf(fp_trace, "R_seek_dist:      %llu\n", N->R_seek_distance);//
	fprintf(fp_trace, "Read_SeekCnt:      %lu\n", N->R_SeekCount);//
	fprintf(fp_trace, "seek_0_R:          %lu\n", N->seek_0_R);//
	fprintf(fp_trace, "R_RespTIme:      %llu\n", N->R_RespTIme);
	fprintf(fp_trace, "R_WaitingTime:   %llu\n", N->R_WaitingTime);
	fprintf(fp_trace, "R_RunTime:       %llu\n", N->R_RunTime);
	fprintf(fp_trace, "W_RespTIme:      %llu\n", N->W_RespTIme);
	fprintf(fp_trace, "W_WaitingTime:   %llu\n", N->W_WaitingTime);
	fprintf(fp_trace, "W_RunTime:       %llu\n", N->W_RunTime);
#endif
#ifdef REQUEST_CHANGE
	//RC_REQ_TYPE;
	fprintf(fp_trace, "RC_cnt:         %lu\n", N->RC_W_cnt + N->RC_R_cnt);
	fprintf(fp_trace, "W_cnt:          %lu\n", N->RC_W_cnt);
	fprintf(fp_trace, "R_cnt:          %lu\n", N->RC_R_cnt);
	//fprintf(fp_trace, "RC_Finish_Time: %llu\n", N->RC_W_Finish_Time + N->RC_R_Finish_Time);
	fprintf(fp_trace, "W_Finish:       %llu\n", N->RC_W_Finish_Time);
	fprintf(fp_trace, "R_Finish:       %llu\n", N->RC_R_Finish_Time);
	fprintf(fp_trace, "RC_WaitingTime: %llu\n", N->RC_W_WaitingTime + N->RC_R_WaitingTime);
	fprintf(fp_trace, "W_Waiting:      %llu\n", N->RC_W_WaitingTime);
	fprintf(fp_trace, "R_Waiting:      %llu\n", N->RC_R_WaitingTime);
#endif
#ifdef POWERFAIL
	fprintf(fp_trace, "PF_pageWrite:     %lu\n", N->PF_pageWrite);
	fprintf(fp_trace, "seek_0_PF:        %lu\n", N->seek_0_PF);
	fprintf(fp_trace, "PF_SeekCount:     %lu\n", N->PF_SeekCount); 
	fprintf(fp_trace, "PF_seek_distance: %I64u\n", N->PF_seek_distance);
#endif
	fclose(fp_trace);
#ifdef MergeAssoc_NSLBpart
	fclose(fp_NSLB);
#endif
}

