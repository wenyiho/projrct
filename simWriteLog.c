#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"


// the result table
//     
// +-------------+-------------+-------------+---------
// |  var 1 name | val for exp1| val for exp2|...     
// +-------------+-------------+-------------+---------
// |  var 2 name | val for exp1| val for exp2|...     
// +-------------+-------------+-------------+---------
// |  ......     |             |             |     
//
// column 0 20 chars, other columns 12 chars

// find the slot to put the result of the given name
void simFindResultSlot(simStat *SS,BYTE *name,DWORD *x,DWORD *y)
{
	DWORD	i,j;

	// first find if there is already an entry for this result
	for(j=0;j<100;j++)
	{
		if(strcmp(&(SS->resultTable[0][j][0]),name)==0)
			for(i=0;i<100;i++)
				if(SS->resultTable[i][j][0] == 0)
				{
					*x=i;*y=j;
					return;
				}
	}
	// falling here: this is a new result entry
	for(j=0;j<100;j++)
	{
		if(SS->resultTable[0][j][0] == 0)
		{
			assert(SS->resultTable[1][j][0] == 0);
			*x=1;*y=j;
			return;
		}
	}
	assert(0);	// must find a free row...
}

void simPutDWORD(simStat *SS,BYTE *name,DWORD value)
{
	DWORD x,y;

	simFindResultSlot(SS,name,&x,&y);
	sprintf(&(SS->resultTable[0][y][0]),"%-s",name);
	sprintf(&(SS->resultTable[x][y][0]),"%12lu",value);
}

void simPutI64(simStat *SS,BYTE *name,I64 value)
{
	DWORD x,y;

	simFindResultSlot(SS,name,&x,&y);
	sprintf(&(SS->resultTable[0][y][0]),"%-s",name);
	sprintf(&(SS->resultTable[x][y][0]),"%12I64i",value);
}


void simPutI64_1(simStat *SS, BYTE *name, I64 value)
{
	DWORD x, y;

	simFindResultSlot(SS, name, &x, &y);
	sprintf(&(SS->resultTable[0][y][0]), "%-s", name);
	sprintf(&(SS->resultTable[x][y][0]), "\t%I64", value);
}


void simPutDouble(simStat *SS,BYTE *name,double value)
{
	DWORD x,y;

	simFindResultSlot(SS,name,&x,&y);
	sprintf(&(SS->resultTable[0][y][0]),"%-s",name);
	sprintf(&(SS->resultTable[x][y][0]),"%12f",value);
}

void simPutString(simStat *SS,BYTE *name,BYTE *value)
{
	DWORD x,y;

	simFindResultSlot(SS,name,&x,&y);
	sprintf(&(SS->resultTable[0][y][0]),"%-s",name);
	strncpy(&(SS->resultTable[x][y][0]),value,12);
}

void simWriteLog(sim *simPtr)
{		
	simConfig		*SC = &(simPtr->simConfigObj);
	simStat			*SS = &(simPtr->simStatObj);
	NFTLconfig		*NC = &(simPtr->NFTLobj.configObj);
	NFTLstat		*NS = &(simPtr->NFTLobj.statObj);
	NFTL			*N	= &(simPtr->NFTLobj);

	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	I64 Output_pageWrite = 0;//Time_Overhead用的
	//DWORD Merge_Bandcount = N->Merge_Bandcount + N->Merge_Bandcount1 + N->Merge_Bandcount2_2;

	DWORD rot_time_cnt = N->Write_W_RotTime + N->Write_R_RotTime + N->Merge_W_RotTime + N->Merge_R_RotTime + N->WB_RotTime;// +N->Merge_RLB_RotTime;
	DWORD skew_rot_time_cnt = (N->Write_W_SkewRotTime + N->Write_R_SkewRotTime + N->Merge_W_SkewRotTime + N->Merge_R_SkewRotTime + N->WB_SkewRotTime);//Skew Rotate
	//Merge_R_RotTime有包含Merge_RLB_RotTime 所以不用再加Merge_RLB_RotTime!!
#ifdef READ_REQUEST
	rot_time_cnt += N->Read_RotTime;
	skew_rot_time_cnt += N->Read_SkewRotTime;
#endif
#ifdef POWERFAIL
	rot_time_cnt += N->PF_RotTime;
	skew_rot_time_cnt += N->PF_SkewRotTime;
#endif
	simPutString(SS,"---SIMU CONFIG---"," ");
	simPutDWORD(SS,"traceReplay",SC->traceReplay);
	//simPutI64(SS, "PsizeByte", NC->PsizeByte - NC->trackSizeByte * NC->FREE_CUT + NC->trackSizeByte * NC->WASTE_CUT);
	//simPutI64(SS,"LsizeByte",NC->LsizeByte);
	//simPutI64(SS, "PsizeGB", NC->PsizeByte/ 1024 / 1024 / 1024);
	//simPutI64(SS, "LsizeGB", NC->LsizeByte/1024/1024/1024);
	simPutDWORD(SS, "LB Sector ", SLB->SLBsection_num);
#ifdef Simulation_NSLB
	simPutDWORD(SS, "NSLB Sector ", NSLB->NSLBsection_num);
#endif
	simPutI64(SS, "PsizeMB", NC->PsizeByte / 1024 /  1024);
	simPutI64(SS, "LsizeMB", NC->LsizeByte / 1024 /  1024);

	simPutDWORD(SS,"blockSizeByte",NC->blockSizeByte);
	//simPutDWORD(SS,"pageSizeByte",NC->pageSizeByte);
	//simPutDWORD(SS,"sectorSizeByte",NC->sectorSizeByte);

	simPutI64(SS, "totalReq", SS->totalReq);
	simPutI64(SS, "writeReq", SS->writeReq);
	simPutI64(SS,"reqSectorWrite",SS->reqSectorWrite);

	//Output_pageWrite = NS->pageWrite + SLB->pageWrite + NSLB->pageWrite;
	simPutI64(SS, "pageWrite", NS->pageWrite + SLB->pageWrite + NSLB->pageWrite);
	simPutI64(SS, "SLB Write", SLB->write_count);
	simPutI64(SS, "SLB Merge", SLB->merge_count);//總共merge寫多少sector
	//simPutI64(SS, "NSLB Write", NSLB->write_count);
	//simPutI64(SS, "NSLB Merge", NSLB->merge_count);
	simPutI64(SS, "pageRead", NS->pageRead + SLB->pageRead + NSLB->pageRead);
	simPutDWORD(SS, "S_MRS   ", N->S_MRS);//merge read sector in SLB
#ifdef Time_Overhead
	simPutDWORD(SS, "WW_RotTime     ", N->Write_W_RotTime);
	simPutDWORD(SS, "WW_SkewRotTime ", N->Write_W_SkewRotTime);
	simPutDWORD(SS, "WR_RotTime     ", N->Write_R_RotTime);
	simPutDWORD(SS, "WR_SkewRotTime ", N->Write_R_SkewRotTime);
	simPutDWORD(SS, "MW_RotTime     ", N->Merge_W_RotTime);//N->NSLBreWrite_SeekTime
	simPutDWORD(SS, "MW_SkewRotTime ", N->Merge_W_SkewRotTime);
	simPutDWORD(SS, "MR_RotTime     ", N->Merge_R_RotTime);
	simPutDWORD(SS, "MR_SkewRotTime ", N->Merge_R_SkewRotTime);
	simPutDWORD(SS, "MRLB_RotTime     ", N->Merge_RLB_RotTime);
	simPutDWORD(SS, "MRLB_SkewRotTime ", N->Merge_RLB_SkewRotTime);
	simPutDWORD(SS, "WB_RotTime     ", N->WB_RotTime);//SLB, Shuffle必0
	simPutDWORD(SS, "WB_SkewRotTime ", N->WB_SkewRotTime);
#ifdef READ_REQUEST
	simPutDWORD(SS, "Read_RotTime     ", N->Read_RotTime);//SLB, Shuffle必0
	simPutDWORD(SS, "Read_SkewRotTime ", N->Read_SkewRotTime);
#endif
#ifdef POWERFAIL
	simPutDWORD(SS, "PF_RotTime      ", N->PF_RotTime);
	simPutDWORD(SS, "PF_SkewRotTime  ", N->PF_SkewRotTime);
#endif
	simPutDWORD(SS, "Sum_RotTime_cnt", rot_time_cnt); 
	simPutDWORD(SS, "Sum_Skew_RotTime_cnt", skew_rot_time_cnt);
	simPutDWORD(SS, "Sum_RotTime    ", rot_time_cnt * 5 + skew_rot_time_cnt * 2);

	simPutDouble(SS, "WW   ", N->WW_content_rotate);
	simPutDouble(SS, "WR   ", N->WR_content_rotate);
	simPutDouble(SS, "MR   ", N->MR_content_rotate);//N->NSLBreWrite_SeekTime
	simPutDouble(SS, "MW   ", N->MW_content_rotate);
	simPutDouble(SS, "WB   ", N->WB_content_rotate);//SLB, Shuffle必0
#ifdef READ_REQUEST
	simPutDouble(SS, "R    ", N->R_content_rotate);
#endif
#ifdef POWERFAIL
	simPutDouble(SS, "PF   ", N->PF_content_rotate);
#endif
	simPutDouble(SS, "content_rotate  ", N->content_rotate);
	simPutDouble(SS, "CR_time         ", (double)N->content_rotate * 10 / NC->trackSizeSector);

	//simPutDouble(SS, "Write_W_SeekTime    ", N->Write_W_SeekTime);
	//simPutDouble(SS, "Write_R_SeekTime    ", N->Write_R_SeekTime);
	//simPutDouble(SS, "Write_RDB_SeekTime  ", N->Write_RDB_SeekTime);//
	//simPutDouble(SS, "Write_RLB_SeekTime  ", N->Write_RLB_SeekTime);//
	//simPutDouble(SS, "Merge_W_SeekTime    ", N->Merge_W_SeekTime);//N->NSLBreWrite_SeekTime
	//simPutDouble(SS, "Merge_R_SeekTime    ", N->Merge_R_SeekTime);
	//simPutDouble(SS, "Merge_RLB_SeekTime  ", N->Merge_RLB_SeekTime);
	//simPutDouble(SS, "Sum_SeekTime        ", N->Write_W_SeekTime + N->Write_R_SeekTime + N->Merge_W_SeekTime + N->Merge_R_SeekTime);
	
	simPutI64(SS, "seek_dist        ", N->seek_distance);
	simPutI64(SS, "W_seek_dist      ", N->W_seek_distance);
	simPutI64(SS, "MR_SLB_seek_dist ", N->MR_SLB_seek_distance);
	simPutI64(SS, "MR_DB_seek_dist  ", N->MR_DB_seek_distance);
	simPutI64(SS, "MWseek_dist      ", N->MWseek_distance);
	simPutI64(SS, "WBseek_dist      ", N->WB_seek_distance);//
	simPutI64(SS, "rW_seek_dist     ", N->rW_seek_distance);//沒有做scale之前的seek_distance總和
	simPutI64(SS, "rMR_SLB_seek_dist", N->rMR_SLB_seek_distance);
	simPutI64(SS, "rMR_DB_seek_dist ", N->rMR_DB_seek_distance);
	simPutI64(SS, "rMWseek_dist     ", N->rMWseek_distance);

	simPutDWORD(SS, "Write_W_SeekCnt    ", N->Write_W_SeekCount);
	//simPutDWORD(SS, "Write_R_SeekCnt    ", N->Write_R_SeekCount);
	simPutDWORD(SS, "Write_RDB_SeekCnt  ", N->Write_RDB_SeekCount);//
	simPutDWORD(SS, "Write_RLB_SeekCnt  ", N->Write_RLB_SeekCount);//
	simPutDWORD(SS, "Write_SeekCnt      ", N->Write_W_SeekCount + N->Write_R_SeekCount);
	simPutDWORD(SS, "Merge_W_SeekCnt    ", N->Merge_W_SeekCount);
	simPutDWORD(SS, "Merge_R_SeekCnt    ", N->Merge_R_SeekCount);
	simPutDWORD(SS, "Merge_RLB_SeekCnt  ", N->Merge_RLB_SeekCount);
	simPutDWORD(SS, "WB_SeekCnt         ", N->WB_SeekCount);//
	simPutDWORD(SS, "Merge_SeekCnt      ", N->Merge_W_SeekCount + N->Merge_R_SeekCount + N->WB_SeekCount);
	simPutDWORD(SS, "Merge_Trackcnt     ", N->Merge_Trackcount);
	simPutDWORD(SS, "WB_SeekCnt         ", N->WB_SeekCount);
	//simPutDWORD(SS, "Merge_LB_Trackcnt  ", N->Merge_LB_Trackcount);
	//simPutI64(SS, "S_MRS   ", N->S_MRS); //simPutI64(SS, "S_CT    ", N->S_CT);
	simPutDWORD(SS, "seek_0_WW            ", N->seek_0_WW);
	simPutDWORD(SS, "seek_0_MRLB          ", N->seek_0_MRLB);
	simPutDWORD(SS, "seek_0_MR            ", N->seek_0_MR);
	simPutDWORD(SS, "seek_0_MW            ", N->seek_0_MW);
	simPutDWORD(SS, "seek_0               ", N->seek_0);
	simPutDWORD(SS, "SeekCount       ", N->Write_W_SeekCount + N->Write_R_SeekCount + N->Merge_W_SeekCount + N->Merge_R_SeekCount + N->WB_SeekCount);
	//simPutDWORD(SS, "merge_ram_writecount", N->merge_ram_writecount);

#endif
	//---------------------------------------------------------
	//NS->mergecnt = NS->mergecnt + SLB->mergecnt + NSLB->mergecnt
	simPutDWORD(SS, "mergeCount       ", NS->mergecnt + SLB->mergecnt + NSLB->mergecnt);
	simPutDWORD(SS, "SLB mergeCount   ", SLB->mergecnt);
	//simPutDWORD(SS, "NSLB mergeCount  ", NSLB->mergecnt);
	//simPutDWORD(SS, "Merge_Bandcount1 ", N->Merge_Bandcount1);
	simPutDWORD(SS, "Effect_Merge     ", N->Effect_Merge);
	//simPutDWORD(SS, "Merge_Bandcount21 ", N->Merge_Bandcount2_1);
	//simPutDWORD(SS, "Merge_Bandcount22 ", N->Merge_Bandcount2_2);
#ifdef Time_Overhead
	simPutDWORD(SS, "Merge_Bandcnt  ", N->Merge_Bandcount);
	//simPutDWORD(SS, "NoMerge_Band     ", N->NoMerge_Band);
#endif
#ifdef Simulation_SLBPM
	simPutDWORD(SS, "ALLOW_PM   ", N->ALLOW_PM);
	simPutDWORD(SS, "PM_SUCCESS ", N->PM_SUCCESS);
#endif
	//simPutDouble(SS, "LB_Live_Rate     ", N->l2pmapOvd.LB_Live_Rate);
	simPutDouble(SS, "LB_Live_RateAA   ", N->l2pmapOvd.AA_Rate);
	simPutDWORD(SS, "max_AA           ", N->l2pmapOvd.max_AA);
	simPutDWORD(SS, "final_AA         ", N->l2pmapOvd.AA);
	
	//----------------------------------------------------------

#ifdef SLBNSLB_Tmerge
	//試驗
	simPutI64(SS, "SLB_Hit   ", N->SLB_Hit);
	simPutI64(SS, "SLB_Miss  ", N->SLB_Miss);
	//simPutI64(SS, "NSLB_Hit  ", N->NSLB_Hit);
	//simPutI64(SS, "NSLB_Miss ", N->NSLB_Miss);
#endif

	simPutI64(SS, "ValidIs0           ", N->ValidIs0);
	simPutI64(SS, "Cumu_Dirty_Sectors ", N->Cumu_Dirty_Sectors);
	simPutI64(SS, "SourceTracks_count ", N->SourceTracks_count);

	{
		FILE *fp;
		DWORD x,y;
		
		fp=fopen("output.txt","w+t");
		for(y=0;y<100;y++)
		{				
			if(SS->resultTable[0][y][0]==0)
				break;

			fprintf(fp,"%-20s",&(SS->resultTable[0][y][0]));

			for(x=1;x<100;x++)
			{
				if(SS->resultTable[x][y][0]==0)
					break;

				fprintf(fp,"%-20s",&(SS->resultTable[x][y][0]));
			}
			fprintf(fp,"\n");
		}
		fclose(fp);
	}
}






//-----------暫時不用

//NSLBPM
//void FASTwriteSectorNSLBPM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
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
//	DWORD isPM_GT = 0;
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
//			//if (N->PM_Tracks_Use < N->PM_Tracks){
//			//	N->PM_Round = 0;
//			//	FASTmergeRWNSLBPM(simPtr);
//			//	//FASTmergeRWNSLBPM已標示那些GT寫入NSLB，清出GT需要的NSLB空間
//			//	while (NSLB->writedPageNum + N->PM_Round*NC->trackSizePage >= NSLB->NSLBpage_num){
//			//		FASTmergeNormalNSLB(simPtr);
//			//	}
//			//}
//			//else{
//			//	FASTmergeNormalNSLB(simPtr);
//			//}
//
//			N->PM_Round = 0;
//			FASTmergeRWNSLBPM(simPtr);
//			//FASTmergeRWNSLBPM已標示那些GT寫入NSLB，清出GT需要的NSLB空間
//			while (NSLB->writedPageNum + N->PM_Round*NC->trackSizePage >= NSLB->NSLBpage_num){
//				FASTmergeNormalNSLB(simPtr);
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
//		assert(1 <= N->blocks[block_no].pages[page_no].valid == 1 && N->blocks[block_no].pages[page_no].valid <= 3);
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
//#ifdef Time_Overhead//錯誤會被assert擋下，到這裡必算時間 //寫不同track才算seek
//			if (oldSection == -1){
//				oldSection = 1;
//			}//一次就複寫整個section。因此一個page之前寫過，則整個section必定之前也寫過
//			assert(oldSection == 1);
//			if (goRMW == 0){
//				Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + oldNSLBfreeTrack + 1) / NC->CylinderSizeTrack, 2);//回舊NSLB track原地更新
//				goRMW = 1;
//				N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//			}
//			N->NSLBmerge = 0;
//#endif
//		}
//		else if (N->blocks[block_no].pages[page_no].valid == 1 || N->blocks[block_no].pages[page_no].valid == 3){//新寫入
//			if (N->blocks[block_no].pages[page_no].valid == 3){ isPM_GT = 1; }
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
//#ifdef Time_Overhead
//			if (oldSection == -1){
//				oldSection = 0;
//			}//一個page沒寫過，則整個section必定沒寫過
//			assert(oldSection == 0);
//			//錯誤會被assert擋下，到這裡必算時間 //寫不同track才算seek
//			if (N->NSLBmerge == 1 || N->preNSLB_WriteSector / NC->trackSizePage != N->blocks[block_no].pages[page_no].sector / NC->trackSizePage){//剛剛merge完的第一次NSLB沒有preNSLB_WriteSector，用flag NSLBmerge進來
//				N->NSLBmerge = 0;
//				Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + N->blocks[block_no].pages[page_no].sector / NC->trackSizePage + 1) / NC->CylinderSizeTrack, 2);
//				//未使用SLB故當作不存在，不加上:(SLB->partial_page + 1) / NC->trackSizePage
//			}
//			N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//#endif
//			//#ifdef Time_Overhead//每個section的第一次write都回DB讀資料
//			//			if (oldSection == -1){
//			//				oldSection = 0;
//			//			}//一個page沒寫過，則整個section必定沒寫過
//			//			assert(oldSection == 0);
//			//			if (isPM_GT == 1){
//			//				if (goDB == 0){
//			//					Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + N->blocks[block_no].pages[page_no].sector / NC->trackSizePage + 1) / NC->CylinderSizeTrack, 2);//回舊NSLB track原地更新
//			//					goDB = 1;
//			//					N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//			//				}
//			//				N->NSLBmerge = 0;
//			//			}else{
//			//				//錯誤會被assert擋下，到這裡必算時間 //寫不同track才算seek
//			//				if (N->NSLBmerge == 1 || goDB == 0){//剛剛merge完的第一次NSLB沒有preNSLB_WriteSector，用flag NSLBmerge進來
//			//					N->NSLBmerge = 0; goDB = 1;
//			//					Calc_TimeOverhead(simPtr, SectorTableNo / NC->trackSizePage / NC->CylinderSizeTrack, 2);
//			//					Calc_TimeOverhead(simPtr, (NC->LsizeBlock*NC->blockSizeTrack - 1 + N->blocks[block_no].pages[page_no].sector / NC->trackSizePage + 1) / NC->CylinderSizeTrack, 2);
//			//					//未使用SLB故當作不存在，不加上:(SLB->partial_page + 1) / NC->trackSizePage
//			//					N->preNSLB_WriteSector = N->blocks[block_no].pages[page_no].sector;
//			//				}
//			//			}
//			//#endif
//		}
//
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0){ break; }
//	}
//}
//
//void FASTwriteSectorNSLBPMRAM_WriteAllOut(sim *simPtr)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	DWORD i = 0;
//	DWORD start_section = 0, Section_Update = 0;
//	if (N->NSLB_RAM_request > 0){
//		//tmp_NSLB_RAM用section mapping寫入NSLB
//		qsort(N->NSLB_RAM, N->NSLB_RAM_request, sizeof(DWORD), compare);//tmp_NSLB_RAM_request是victim sector write的數量
//		assert(N->NSLB_RAM[0] != -1); assert(N->RAM_Valid[N->NSLB_RAM[0]] == 1); N->RAM_Valid[N->NSLB_RAM[0]] = 0;
//		start_section = N->NSLB_RAM[0] / N->SubTrack;
//		//Section_Update = 0; Section_Update += Two_Expo(N->NSLB_RAM[0] % N->SubTrack);
//		N->NSLB_RAM[0] = -1;
//		for (i = 1; i < N->NSLB_RAM_request; i++){
//			assert(N->NSLB_RAM[i] != -1); assert(N->RAM_Valid[N->NSLB_RAM[i]] == 1); N->RAM_Valid[N->NSLB_RAM[i]] = 0;
//			if (N->NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//				//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//正式寫前面section的
//				FASTwriteSectorNSLBPM(simPtr, start_section*N->SubTrack, N->SubTrack);
//				start_section = N->NSLB_RAM[i] / N->SubTrack;
//				//Section_Update = 0; Section_Update += Two_Expo(N->NSLB_RAM[i] % N->SubTrack);
//			}
//			else{ /*Section_Update += Two_Expo(N->NSLB_RAM[i] % N->SubTrack);*/ }
//			N->NSLB_RAM[i] = -1;
//		}
//		//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//寫最後一個section的
//		FASTwriteSectorNSLBPM(simPtr, start_section*N->SubTrack, N->SubTrack);
//
//		N->NSLB_RAM_request = 0;
//	}
//}
//
//void FASTwriteSectorNSLBPMRAM_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
//{
//	NFTL *N = &(simPtr->NFTLobj); NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTLstat *NS = &(simPtr->NFTLobj.statObj);
//	DWORD i = 0, j = 0;
//	//section mapping
//	DWORD start_section = -1;
//	DWORD Section_Update = 0;
//#ifdef Time_Overhead
//	DWORD readDB_section_size = 0;
//	DWORD *readDB_section = (DWORD*)malloc(N->NSLB_RAM_SIZE*sizeof(DWORD));
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
//			//readDB_section_size = 0;
//#endif
//			for (i = 1; i < tmp_NSLB_RAM_request; i++){
//				assert(tmp_NSLB_RAM[i] != -1); assert(N->RAM_Valid[tmp_NSLB_RAM[i]] == 1); N->RAM_Valid[tmp_NSLB_RAM[i]] = 0;
//				if (tmp_NSLB_RAM[i] > start_section*N->SubTrack + N->SubTrack - 1){
//					//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//正式寫前面section的
//#ifdef Time_Overhead
//					if (N->blocks[start_section*N->SubTrack / NC->blockSizeSector].pages[start_section*N->SubTrack % NC->blockSizeSector].valid == 1){
//						readDB_section[readDB_section_size] = start_section*N->SubTrack; readDB_section_size++;
//					}
//#endif
//					FASTwriteSectorNSLBPM(simPtr, start_section*N->SubTrack, N->SubTrack);
//					start_section = tmp_NSLB_RAM[i] / N->SubTrack;
//					//Section_Update = 0; Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);
//				}
//				else{ /*Section_Update += Two_Expo(tmp_NSLB_RAM[i] % N->SubTrack);*/ }
//			}
//			//FASTwriteSectorNSLBpart_WriteToCurTrack_RMW(simPtr, start_section*N->SubTrack, N->SubTrack, Section_Update);//寫最後一個section的
//#ifdef Time_Overhead
//			if (N->blocks[start_section*N->SubTrack / NC->blockSizeSector].pages[start_section*N->SubTrack % NC->blockSizeSector].valid == 1){
//				readDB_section[readDB_section_size] = start_section*N->SubTrack; readDB_section_size++;
//			}
//#endif
//			FASTwriteSectorNSLBPM(simPtr, start_section*N->SubTrack, N->SubTrack);
//			//#ifdef Time_Overhead
//			//			qsort(readDB_section, readDB_section_size, sizeof(DWORD), compare);
//			//			if (readDB_section_size > 1){ for (i = 0; i < readDB_section_size - 1; i++){ assert(readDB_section[i] <= readDB_section[i + 1]); } }
//			//			if (readDB_section_size > 0){
//			//				Calc_TimeOverhead(simPtr, readDB_section[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//讀DB資料
//			//				for (i = 1; i < readDB_section_size; i++){
//			//					if (readDB_section[i - 1] / NC->trackSizePage != readDB_section[i] / NC->trackSizePage){
//			//						Calc_TimeOverhead(simPtr, readDB_section[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//			//					}
//			//				}
//			//			}
//			//#endif
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
//#ifdef Time_Overhead
//		qsort(readDB_section, readDB_section_size, sizeof(DWORD), compare);
//		if (readDB_section_size > 1){ for (i = 0; i < readDB_section_size - 1; i++){ assert(readDB_section[i] <= readDB_section[i + 1]); } }
//		if (readDB_section_size > 0){
//			Calc_TimeOverhead(simPtr, readDB_section[0] / NC->trackSizePage / NC->CylinderSizeTrack, 0);//讀DB資料
//			for (i = 1; i < readDB_section_size; i++){
//				if (readDB_section[i - 1] / NC->trackSizePage != readDB_section[i] / NC->trackSizePage){
//					Calc_TimeOverhead(simPtr, readDB_section[i] / NC->trackSizePage / NC->CylinderSizeTrack, 0);
//				}
//			}
//		}
//#endif
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
//	free(readDB_section);
//#endif
//}
//
//
////NSLBPM
//void FASTwriteSectorNSLBPM2(sim *simPtr, DWORD offsetSector, DWORD lenSector)
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
//	DWORD isPM_GT = 0;
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
//			//if (N->PM_Tracks_Use < N->PM_Tracks){
//			//	N->PM_Round = 0;
//			//	FASTmergeRWNSLBPM(simPtr);
//			//	//FASTmergeRWNSLBPM已標示那些GT寫入NSLB，清出GT需要的NSLB空間
//			//	while (NSLB->writedPageNum + N->PM_Round*NC->trackSizePage >= NSLB->NSLBpage_num){
//			//		FASTmergeNormalNSLB(simPtr);
//			//	}
//			//}
//			//else{
//			//	FASTmergeNormalNSLB(simPtr);
//			//}
//
//			N->PM_Round = 0;
//			FASTmergeRWNSLBPM2(simPtr);
//			//FASTmergeRWNSLBPM已標示那些GT寫入NSLB，清出GT需要的NSLB空間
//			while (NSLB->writedPageNum + N->PM_Round*NC->trackSizePage >= NSLB->NSLBpage_num){
//				FASTmergeNormalNSLB2(simPtr);
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
//		assert(1 <= N->blocks[block_no].pages[page_no].valid == 1 && N->blocks[block_no].pages[page_no].valid <= 3);
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
//		}
//		else if (N->blocks[block_no].pages[page_no].valid == 1 || N->blocks[block_no].pages[page_no].valid == 3){//新寫入
//			if (N->blocks[block_no].pages[page_no].valid == 3){ isPM_GT = 1; }
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
//		}
//
//		offsetSector += NC->pageSizeSector;
//		lenSector -= NC->pageSizeSector;
//		if (lenSector <= 0){ break; }
//	}
//}
//
//void FASTwriteSectorNSLBPMRAM2_STM(sim *simPtr, DWORD offsetSector, DWORD lenSector)
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
//					else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
//#endif
//					FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
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
//			else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
//#endif
//			FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
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
//void FASTwriteSectorNSLBPMRAM2_WriteAllOut(sim *simPtr)
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
//					else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
//#endif
//					FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
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
//			else if (N->blocks[block_no].pages[page_no].valid == 2){ WriteBuff[WriteBuff_Size] = NC->LsizeSector + N->blocks[block_no].pages[page_no].sector; WriteBuff_Size++; assert(WriteBuff_Size <= N->NSLB_RAM_SIZE); }//在SLB
//#endif
//			FASTwriteSectorNSLBPM2(simPtr, start_section*N->SubTrack, N->SubTrack);
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











