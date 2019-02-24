

//-------------------------------------------
//version control
//-------------------------------------------
//Block level merge
//Track level merge
//ClearCLB
//MergeAssociativity

//#define Block_level_merge (1)
//#define Track_level_merge (2)

//以下三個def和PM的def互斥
#define SLBNSLB_Tmerge (5)
////Statistics_64TrackWrite、SLB_64trackWRITE在跑bandsize 128MB和256MB時要關掉
////#define Statistics_64TrackWrite (22) //tracks cumulated dirty pages, one band 64 track
////#define SLB_64trackWRITE (23) //64bit mapping //用在 SLB，統計Clean Region

////#define SLBNSLB_DynCut (13) //有bug，不能用
////#define VirtualSLB (14)

////#define ClearCLB (3)
////#define ClearSLB (6)
////#define ClearNSLB (7)
#define MergeAssociativity (4)
#define MergeAssoc_SLBpart (9)
#define MergeAssoc_NSLBpart (10)
#define time_calculate (11)
#define MAX_MERGE_BAND 1

//#define NEWGC_4GROUP (18) //沒在用
//以下PM兩個def和fullSLB、fullNSLB的def互斥
//#define NEWGC64trackWRITE (24) //64bit mapping //用在 SLBNSLB_PM
//#define SLBNSLBPM_Tmerge (20)
//不同的Band Size，給NEWGC64trackWRITE用的。一次只開一個
#define Band_64MB_8Track (8)
//#define Band_128MB_16Track (16)
//#define Band_256MB_32Track (32)

#define Section_Mapping (21)
//SM下HLB+調POPS用的
#define POPS_2p (41)
////#define POPS_3pAbove (42) 


//用來計算NSLB、HLB、HLB+的BPLRU List在執行的時候有多少個band在裡面
#define BPLRULlist_RunLen (52)


#define SpecificTrack (53)
//以下擇一
//#define NSLB_RAM_FIFO_OLD (54) //不用//初版WC1
//#define NSLB_RAM_SectLRU_HIT (55) //不用
//NSLB
#define NSLB_RAM_FastSectLRU_HIT (56)//NSLB使用 WC2
//#define NSLB_RAM_BandLRU_HIT (57)//WC3 包含Fast版

//SLB
////#define SLB_RAM_OrgLRU_HIT (58)//不用
////#define SLB_RAM_OrgLRU_HIT2 (59)//不用
//#define SLB_RAM_LRUsector_Section (60)//SLB使用
//#define SLB_RAM_LRUsector_Track (61)
#define SLB_RAM_LRUsector_Group (62)

//#define SLB_RAM_MRUsector_Section (63)
//#define SLB_RAM_LRUsectorDifferentBand_Section (64)

#define NSLB_LocalUse (65)

//Control Menu
//Simulation分類
//#define Simulation_HDD (101)
#define Simulation_SLB (102)
//#define Simulation_SLB_SHUFFLE (110)
//#define Simulation_SLB_SHUFFLE_OBSERVE (111)
//#define MOST_FREQ_MERGE_BAND_OBSERVE (116) //用Simulation_SLB_SHUFFLE觀察 Cylinder版 
//#define MOST_FREQ_MERGE_BAND_OBSERVE_TK (117) //用Simulation_SLB_SHUFFLE觀察 Track版
//#define Simulation_SLB_DELAY (112)
//#define Simulation_SLB_DELAY_ROUND (113)
//Delay+VTLMA 控制Merge複雜度
#define Limited_MergeAssociativity 1
#define DWORD_MAX 4294967295
//ICCD 2017 Shingle Aware Cache Management
//#define SACE (200)

#define READ_REQUEST (114)
//#define SHUFFLE_CYLINDER (1101) //Shuffle選Cylinder或Track Unit
//#define SHUFFLE_TRACK (1102)

#define REQUEST_CHANGE (115)

//用來計算write和merge產生的Time Overhead
#define Time_Overhead (51)
#define POWERFAIL (120)

//mapping table with cache
//#define SLB_Mapping_Cache (2019)
//#define CFLRU (2020)


#define Band_64MB_128Track (301)
//#define Band_128MB_256Track (302)
//#define Band_256MB_512Track (303)
//#define Simulation_NSLB (103)
//#define Simulation_HLB (104)
//#define Simulation_SLBPM (105) //
//------------------------------------------
//end version control
//------------------------------------------

//SLB、NSLB、HLB
//SLBNSLB_Tmerge、MergeAssociativity、MergeAssoc_SLBpart、MergeAssoc_NSLBpart、time_calculate、Band_64MB_8Track、Section_Mapping、
//POPS_2p、Time_Overhead、BPLRULlist_RunLen、SpecificTrack、NSLB_RAM_FastSectLRU_HIT、SLB_RAM_LRUsector_Group
//PM、HLB+
//MergeAssociativity、MergeAssoc_SLBpart、MergeAssoc_NSLBpart、time_calculate、NEWGC64trackWRITE、SLBNSLBPM_Tmerge、Band_64MB_8Track
//Section_Mapping、POPS_2p、Time_Overhead、BPLRULlist_RunLen、SpecificTrack、NSLB_RAM_FastSectLRU_HIT、SLB_RAM_LRUsector_Group

//----------------------------------------
//共用
//MergeAssociativity
//MergeAssoc_SLBpart
//MergeAssoc_NSLBpart
//time_calculate
//Band_64MB_8Track
//Section_Mapping
//POPS_2p
//Time_Overhead
//BPLRULlist_RunLen
//SpecificTrack
//NSLB_RAM_FastSectLRU_HIT
//SLB_RAM_LRUsector_Group

//SLB、NSLB、HLB
//SLBNSLB_Tmerge

//PM、HLB+
//NEWGC64trackWRITE、SLBNSLBPM_Tmerge
//----------------------------------------

//DWORD = 2^32-1 =			 4294967295
//I64 = 2^64 - 1 = 18446744073709551615
//#define _20160609 //連hot都delay
#define notdelayable

//#define _20160623
//#define avoid_cutting_band_set //拒絕切某些band
//#define MLC_PAGE_FREE		0xffffffff
//#define MLC_LOGBLOCK_NULL	0xffffffff
#define MLC_LOGBLOCK_NULL	0xffff
//#define MLC_LPI_NULL		0xffffffff	

#ifdef SLB_Mapping_Cache
typedef struct LRU_Map_Entry{
	I64 LA;
	I64 PA;
	struct LRU_Map_Entry *prev;
	struct LRU_Map_Entry *next;
}LRU_Map_Entry;

typedef struct{
	LRU_Map_Entry *LRU_Head;
	DWORD cache_size;
	DWORD cache_used;
}LRU_CACHE; //circular buffer

#endif 

//old flashPage 偉嘉版本
typedef struct {
	I64	valid :3;
	//DWORD   unique : 0;
	I64	sector :50; //2,147,483,648-1 , limit < 1T, >512G
#ifdef SLB_Mapping_Cache
	LRU_Map_Entry *inCache;
#ifdef CFLRU
	BYTE cache_stat :1; //clean or dirty
	BYTE cache_lev : 1; //L1 or L2
#endif
#endif

//max : 2^31 - 1個sector = 4,294,967,296 - 1, 小於2T
}flashPage;

//給part5 6 9 11 13 15 19用的page，因為他們的page valid會到16，以辨認在SLB被重複寫入幾次
//typedef struct {
//	BYTE		valid;
//	DWORD		sector; //2,147,483,648-1 , limit < 1T, >512G
//	//max : 2^31 - 1個sector = 4,294,967,296 - 1, 小於2T
//							
//}flashPage;

/*
typedef struct {
	DWORD		valid;
	DWORD		sector; //max : 2^63 - 1個sector = 9,223,372,036,854,775,808-1 , 小於1T
}flashPage_500Gup;
*/
typedef struct { 
	flashPage *pages;
	flashPage *sections;
	DWORD PM : 2;//標示有做過PM
	DWORD Cut : 2;//DWORD NewPM : 1;
	DWORD GR : 2;
	DWORD GuardTrackIndex : 6;
	DWORD PMG_WriteCyl : 10;
	DWORD GuardCylIndex : 10;// DWORD PosIndex : 9; DWORD Merge : 1;
#ifdef NEWGC64trackWRITE	
#ifdef Band_64MB_8Track
	BYTE DirtyTrack[8];//bit mapping 64 track write. Every ever writted track sets to 0 after GC.
#endif
#ifdef Band_128MB_16Track
	BYTE DirtyTrack[16];
#endif
#ifdef Band_256MB_32Track
	BYTE DirtyTrack[32];
#endif
#endif
#ifdef SACE
	DWORD *SACE_TKDirtySector;//累積寫多少
#endif

}flashBlock;

flashBlock *victim_backup;
flashBlock *remain_backup;

#ifdef freedelay
flashBlock *fd;
DWORD *freefp;
DWORD *freelp;
#endif

typedef struct{ DWORD blockpageNo; }NFTLsectorElementxx;
typedef struct{ DWORD blockNo : 20; DWORD pageNo : 12;}NFTLsectorElement11;	//1MB band, 2^11個page //4096>個page, band=1M, 
typedef struct{ DWORD blockNo : 19; DWORD pageNo : 13;}NFTLsectorElement12;
typedef struct{ DWORD blockNo : 18; DWORD pageNo : 14;}NFTLsectorElement13;
typedef struct{ DWORD blockNo : 17; DWORD pageNo : 15;}NFTLsectorElement14;
typedef struct{ DWORD blockNo : 16; DWORD pageNo : 16;}NFTLsectorElement15;
typedef struct{ DWORD blockNo : 15; DWORD pageNo : 17;}NFTLsectorElement16;
typedef struct{ DWORD blockNo : 14; DWORD pageNo : 18;}NFTLsectorElement17;
typedef struct{ DWORD blockNo : 13; DWORD pageNo : 19;}NFTLsectorElement18;
typedef struct{ DWORD blockNo : 12; DWORD pageNo : 20;}NFTLsectorElement19;

//1024*1024>個page, band=256MB, 4096個>band, 2048個band
//< 256MB* 2047個band = 51xG //上限

typedef struct{ DWORD blockNo : 11; DWORD pageNo : 21;}NFTLsectorElement20;	//512MB band, 2^20個page

typedef struct{  //DWORD blockNo; 
	unsigned int blockNo;	//65536 - 1個band, 
							//ex. 400G bs=256MB, 1600+x個band 1600+x + 1 + 1 < 65535
							//ex. 400G bs=64MB, 6400+x個band 6400+x + 1 + 1 < 65535
							//ex. 1T bs=256MB, 4096+x
	DWORD sectionNo;// pageNo; //4, 294, 967, 296 - 1 個page //偉嘉版用DWORD，我因為LB的partial band的page太多只能用I64   
					//bs = 256MB, 524288 < 
					//<
					//remain
					//victim
}NFTLsectorElement_500Gup; //current

typedef struct { // --- common part: flash geometry
	//之後砍掉
	DWORD	pageSizeByte;
	DWORD	pageSizeSector;
	DWORD	MAX_CUT;
	DWORD	FREE_CUT;
	DWORD	WASTE_CUT;
	DWORD	cut_def;
	DWORD	delay_threshold;
	DWORD	cut_afterwrite;
	DWORD	cut_aftermerge;

	DWORD	Disk_trackSizePage;//?
	//---
	I64		PsizeByte;
	I64		LsizeByte;		//DWORD 4, 294, 967, 296 < < I64
	DWORD	blockSizeByte;
	DWORD	trackSizeByte;
	DWORD	sectorSizeByte; //4, 294, 967, 296

	DWORD	Section;//需要的Sector數
	
	DWORD	trackSizeSector;
	DWORD	trackSizePage;//之後砍掉
	DWORD	trackSizeSection;
	
	DWORD	blockSizeSector;
	DWORD	blockSizePage;//之後砍掉
	DWORD	blockSizeSection;
	DWORD	blockSizeTrack;
	DWORD	blockSizeCylinder;

	DWORD	CylinderSizeTrack;
	DWORD	CylinderSizeSector;

	I64	LsizeSector; // DWORD: 4, 294, 967, 296 ... 上限2048 G
	I64	LsizePage;//之後砍掉
	I64	LsizeSection;
	DWORD	LsizeTrack;
	DWORD	LsizeBlock;
	DWORD	LsizeCylinder;
	DWORD	LsizeMB;

	I64	PsizeSector;//DWORD
	I64	PsizePage;//之後砍掉
	I64	PsizeSection;
	DWORD	PsizeTrack;
	DWORD	PsizeBlock;
	DWORD	PsizeCylinder;
	DWORD	PsizeMB;

	DWORD	writeinterference;
	DWORD	RamSizeTrack;
	DWORD	RMWbufferSizeTrack;
	//NSLB
	DWORD	NSLBpercent;
	//PM Ratio
	DWORD	PMRatio;
}NFTLconfig;
// ============== statistics of the NFTL
typedef struct{
	I64	pageWrite;
	I64	pageRead;
	//I64	pageWrite_overhead;
	//I64	pageRead_overhead;
	I64 totalcut;
	I64 totalfill;
	I64 line;
	DWORD mergecnt;

	//total_seek = SLB_Seek + NSLB_Seek
	I64 SLB_Seek;
	I64 NSLB_Seek;
}NFTLstat;


// ================= The NFTL data structure
typedef struct
{
	DWORD track_num : 16;
	DWORD use_index : 16;
	DWORD *head;
}NSLB_SpecificTrack;

FILE *fPM_content;
FILE *fp_MergeMount;

#ifdef SLBNSLB_Tmerge
struct PM_NODE
{
	//flashPage Page;
	DWORD Band_No, SLB_PageIndex;
	struct VSLB_NODE *next;
};

typedef struct
{
	DWORD BandWriteCount[8];
}SUBBAND_8;

typedef struct
{
	DWORD BandWriteCount[64];
}SUBBAND_64;

//LinkList.c使用的資料結構
struct Free_List	*FLt;
struct BPLRU_List	*BPLRU;//舊
struct NSLB_LIST	*NSLBlist;//NSLB Second-Chance 在用

//queue<DWORD> BPLRUqueue;

FILE *ftime;
FILE *fp_SLB;
FILE *fp_NSLB;

FILE *fp_S64TW;
FILE *fp_CR;

FILE *fp_out;

typedef struct
{
	DWORD		SLBpage_num;
	DWORD		guardTrack1stPage;//DWORD
	DWORD		writedPageNum;//DWORD
	DWORD		partial_page;
	DWORD		firstRWpage; //DWORD
	DWORD		lastRWpage; //DWORD

	DWORD		SLBsection_num;
	DWORD		SLBcylinder_num;
	DWORD		guardTrack1stSection;
	DWORD		writedSectionNum;
	DWORD		Partial_Section;
	DWORD		firstRWsection; 
	DWORD		lastRWsection;

	//NFTLstat
	I64		pageWrite;
	I64		pageRead;
	I64		mergecnt;
	I64		write_count;
	I64		merge_count;
}NFTL_SLB;

typedef struct
{
	DWORD		NSLBpage_num;//DWORD
	DWORD		writedPageNum;//DWORD
	DWORD		partial_page;
	DWORD		firstRWpage;
	DWORD		lastRWpage;

	DWORD		NSLBsection_num;
	//DWORD		guardTrack1stSection;
	DWORD		writedSectionNum;
	DWORD		Partial_Section;
	DWORD		firstRWsection;
	DWORD		lastRWsection;

	//NFTLstat
	I64		pageWrite;
	I64		pageRead;
	I64		mergecnt;
	I64		write_count;
	I64		merge_count;
}NFTL_NSLB;





typedef struct
{
	DWORD		FirstCylNo;//DWORD
	DWORD		LastCylNo;//DWORD
	DWORD		ZoneSizeCyl;
	DWORD		ZoneSizeSector;
	DWORD		TrackSizeSector;
	DWORD		TrackSkew;
	DWORD		CylinderSkew;
	DWORD		deadspace;
}Disk_Zone;
typedef struct
{
	DWORD		DiskSizeMB;
	DWORD		DiskSizeSector;
	DWORD		AvgTrackSize;
	DWORD       numZones;
	double		seekwritedelta;
	double		DiskSizeCylinder;
	Disk_Zone	Zone[15];
}Disk_Structure;

typedef struct
{
	//double LB_Live_Rate;
	//DWORD  round_LB_Live_Section;
	double AA_Rate;
	DWORD  AA;//live
	DWORD max_AA;
}L2PMapOverhead;

//typedef struct
//{
//	DWORD track_num : 16;
//	DWORD use_index : 16;
//	DWORD *head;
//}NSLB_SpecificTrack;

typedef struct
{
	DWORD  Act, InAct;
}SLOTTABLE_BAND;

typedef struct
{
	DWORD validIs0, validIs1;
	//DWORD block_no, section_no;
	DWORD dataBlock;
#ifdef Time_Overhead
	DWORD merge_ram_size;
	DWORD *merge_ram;// = (DWORD*)calloc(NC->blockSizeSection, sizeof(DWORD));
	DWORD NoMerge_Band;
#endif
	//紀錄每次有效merge帶走多少band,多少dirty sectors,多少cumu dirty sectors
	DWORD Merge_Bands;
	I64 Dirty_Sectors, Cumu_Dirty_Sectors;
	DWORD tsp_start, tsp_end;
}SLB_MERGE_PARA;

typedef struct{
	DWORD DB_No : 20;
	DWORD PMG_SaveCyl : 12;
}PMG_RANK;
#ifdef Simulation_SLB_SHUFFLE
typedef struct{
	//LOGICAL_LAYER
	DWORD *L2P_Index;
	//PHYSICAL_LAYER
	DWORD *P2L_Index;
	DWORD *WriteFreq;
	DWORD *DirtyUnit;
}TRANSFOR_LAYER;
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
typedef struct{
	DWORD *CUMU_TIME;
	DWORD *TEMP_TIME;
	DWORD *PERIOD;
	DWORD *PRE_DirtyUnit;
}MERGE_PERIOD_LOCALITY;
#endif
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE
//MFMBO
typedef struct{
	DWORD *MergeFreq;
	DWORD *MergeDS;//Dirty Sector
}MFMBO;
#endif
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE_TK
//MFMBO
typedef struct{
	DWORD *MergeFreq;
	DWORD *MergeDS;//Dirty Sector
}MFMBO;
#endif
#endif
typedef struct{							//NFTL
	flashBlock	*blocks;				// the array of blocks,pages. mainly used to identify the logical addresses recorded in every spare of pages.
	unsigned int		*L2Ptable;				// the L2P table, in terms of blocks. Having LsizeBlock elements
	//unsigned int		*repTable;				// to chain the log blocks. Having PsizeBlock elements
	unsigned int		firstSpareBlock;		// the index of the first spare block. There might be many spare blocks. They are chained in the repTable
	unsigned int		lastSpareBlock;			// the index of the last spare block.	
	unsigned int		firstRWblock;			// oldest RWblock
	unsigned int		lastRWblock;			// writing RWblock
	

	NFTLsectorElement_500Gup *st;//沒用到
	NFTLconfig	configObj;
	NFTLstat	statObj;

	unsigned int	partial_band;//之後砍掉
	DWORD			partial_page;//之後砍掉
	DWORD			Partial_Section;//之後砍掉
	unsigned int	LB_Block; //<bs //沒用到?
	DWORD			LB_LastSector;
	DWORD			LB_LastSection;
	DWORD			LB_Track;
	DWORD			LB_Cylinder;

	DWORD		firstRWpage; //DWORD
	DWORD		lastRWpage;	//DWORD			// the index of first free page of writing RWblock
	DWORD		writedPageNum; //DWORD
	DWORD		guardTrack1stPage; //DWORD
	DWORD		LBpage_num;

	DWORD		firstRWsection; 
	DWORD		lastRWsection;
	DWORD		writedSectionNum;
	DWORD		guardTrack1stSection; //DWORD
	DWORD		LBsection_num;

	NFTL_SLB	SLB;
	NFTL_NSLB	NSLB;
	DWORD       HD_WRITE_Len;//?
	DWORD		NSLBGCtoDB_Threshold;//?
	I64		*BPLRU;//BPLRU_len = NC->LsizeBlock
	I64		 cnt;
	DWORD   NSLB_tracks;
	DWORD   *NSLBremainSectorTable;//不用
	DWORD   *NSLBremainSectionTable;
	DWORD   NSLBexhaustedTrack;

	I64 Total_Merge;//?
	I64 PM_Merge;//?
	I64 PMBTD_Merge;//?

	//DWORD SubTrack;
	//試驗 //換LB
	I64 SLB_Hit, SLB_Miss;
	I64 NSLB_Hit, NSLB_Miss;
	I64 cycle_reqSectorWrite;
	I64 cycleSLB_Hit, cycleSLB_Miss;
	I64 cycleNSLB_Hit, cycleNSLB_Miss;
	//試驗merge len
	I64 ValidIs0, ValidIs2;//SLB page、NSLB page
	I64 SourceTracks_count;
	//I64 ValidIs0_max, ValidIs2_max;
	//試驗//?
	DWORD BurstLen;
	I64 BurstLen_Sum;
	I64 BurstLen_Max;
	DWORD BurstLen_Start, BurstLen_End;

	//section寫入分佈//?
	DWORD track_valid[27236];
	BYTE track_valid_section[27236][64];
	DWORD track_valid_use;
	DWORD req_write_section;

#ifdef SpecificTrack
	//FASTwriteSectorNSLBpart_SpecificTrack
	//DWORD   *Band_NSLBTrackIndex;
	//DWORD   NSLBusedTrack;
	
	//試驗
	//DWORD   *Band_Tracks;
	//DWORD   *Used_Sector;

	//試驗
	DWORD   NSLBusedTrack;
	NSLB_SpecificTrack *NSLB_TrackIndex;
	DWORD   *NSLB_Band;

	DWORD NSLB_RAM_SIZE;
	DWORD *NSLB_RAM;
	DWORD NSLB_RAM_request;

	BYTE *RAM_Valid;
	//BYTE *Track_Valid;

	//統計RAM Hit吸收的寫入
	I64 RAM_WriteOut;
	I64 Host2RAM;
	DWORD RAM_Hit, RAM_Miss;
//#ifdef NSLB_RAM_BandLRU_HIT
//	DWORD *RAM_BPLRU;
//	DWORD RAM_BPLRU_Time;
//#endif
#endif
	//用來計算write和merge產生的Time Overhead
#ifdef Time_Overhead
	Disk_Structure Disk;
	DWORD RAM_WAIT;
	double Wait_SeekTime;
	DWORD Wait_SeekCount;
	DWORD Wait_RotTime;
	DWORD Wait_content_rotate;

	//分別算write和merge的seek time
	DWORD FirstTime;//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	unsigned long long Write_W_SeekTime, Write_R_SeekTime, Merge_W_SeekTime, Merge_R_SeekTime;
	unsigned long long Merge_RLB_SeekTime;
	unsigned long long Write_RLB_SeekTime, Write_RDB_SeekTime;//
	unsigned long long SeekTime;
	DWORD Previous_LTA, FirstSection;
	DWORD Write_W_RotTime, Write_R_RotTime, Merge_W_RotTime, Merge_R_RotTime;
	DWORD Merge_RLB_RotTime;
	DWORD Write_W_SkewRotTime, Write_R_SkewRotTime, Merge_W_SkewRotTime, Merge_R_SkewRotTime;
	DWORD Merge_RLB_SkewRotTime;
	unsigned long long content_rotate;
	unsigned long long WW_content_rotate, WR_content_rotate, MR_content_rotate, MW_content_rotate;
	//分別算write和merge的rotation time
	/*double SLBWrite_RotationTime, SLBMerge_RotationTime;
	double NSLBWrite_RotationTime, NSLBMerge_RotationTime;*/
	//seek次數
	DWORD Write_W_SeekCount, Write_R_SeekCount, Merge_W_SeekCount, Merge_R_SeekCount;
	DWORD Merge_RLB_SeekCount;
	DWORD Write_RLB_SeekCount, Write_RDB_SeekCount;//
	DWORD seek_0;
	DWORD seek_0_WW, seek_0_WR, seek_0_MW, seek_0_MR;
	DWORD seek_0_MRLB;

	I64 seek_distance;
	I64 W_seek_distance, MR_SLB_seek_distance, MR_DB_seek_distance, MWseek_distance;
	I64 rW_seek_distance, rMR_SLB_seek_distance, rMR_DB_seek_distance, rMWseek_distance;
	//
	unsigned long long WB_SeekTime;
	DWORD WB_SeekCount;
	DWORD seek_0_WB;
	I64 WB_seek_distance;
	DWORD WB_RotTime;
	DWORD WB_SkewRotTime;
	unsigned long long WB_content_rotate;

	//InterXferTime: 總量是pageWrite*InterXferTime
	//統計rotation
	I64 rotation_preSectorInTrack;
	I64 rotation_count;//算的是轉幾個page，最後要除track size page才知道幾圈
	//scaling formula
	double dScale;
	I64 Cylinders;
	double SeekModelBoundary;
	//disk head所在位置，用在每次計算兩個寫入的head位置差
	I64 pre_HeadCylinder, cur_HeadCylinder;
	I64 pre_HeadTrack, cur_HeadTrack;
	I64 pre_HeadSector, cur_HeadSector;
	I64 Scale_up, Scale_down;
#ifdef Simulation_NSLB
	DWORD Cylinder_Change;
#endif
	//試驗
	double d;
	//電梯法找LB所需要的merge_ram
	//N->merge_ram_size、N->merge_ram[]: 改成merge func裡的local變數，因為global的變數會被遞迴的下一層破壞
	//?
	DWORD preNSLB_WriteSector;
	DWORD preNSLB_WriteTrack;
	DWORD NSLBmerge;

	//試驗
	DWORD preNSLB_reWriteSector;
	double NSLBreWrite_SeekTime;
	I64 reWrite_pre_HeadLocatedCylinder, reWrite_cur_HeadLocatedCylinder;

	//試驗//?
	DWORD WriteBuff_SIZE;
	DWORD *WriteBuff;
	DWORD WriteBuff_section;
	
	//試驗//?
	DWORD NMR_RMWBuff_SIZE;
	DWORD *NMR_RMWBuff;
	DWORD NMR_RMWBuff_section;

	//試驗
	DWORD WriteBuff_writecount;//?
	DWORD merge_ram_writecount;//舊是Merge_Bandcount, 之後不用//?
	DWORD Merge_Bandcount;
	//DWORD Merge_Bandcount1;
	//DWORD Merge_Bandcount2_1;
	//DWORD Merge_Bandcount2_2;
	DWORD Merge_Trackcount;
	DWORD Merge_LB_Trackcount;
	DWORD WriteBuff_WriteOut;
	DWORD NMR_Rotate;
	DWORD NoMerge_Band;

	DWORD SLBws, SLBms, NSLBws, NSLBms;//?
	DWORD SLBtrackChange, RDB, RMW, Merge;//?

	//試驗 HP_C3323A_validate
	DWORD cur_index, req_len;//?
	I64 S_MRS, S_CT;//作用是?
#endif
#ifdef BPLRULlist_RunLen //用來計算NSLB、HLB、HLB+的BPLRU List在執行的時候有多少個band在裡面
	I64 BPLRUtotal_len, BPLRUtotal_count;
	I64 BPLRUcurrent_len;
	I64 BPLRUcurrent_max;
	I64 BPLRUcurrent_min;
#endif
#ifdef READ_REQUEST
	unsigned long long Read_SeekTime;
	DWORD R_SeekCount;
	DWORD seek_0_R;
	I64 R_seek_distance;
	DWORD Read_RotTime, Read_SkewRotTime;
	unsigned long long R_content_rotate;
	//Read Response使用Write的Arr_Time, Finish_Time, Req_RunTime算出
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	unsigned long long R_RespTIme, W_RespTIme;
	unsigned long long R_WaitingTime, W_WaitingTime;
	unsigned long long R_RunTime, W_RunTime;
	DWORD ReqR_pageRead;//
#endif
	L2PMapOverhead l2pmapOvd;
	DWORD *section_cumu;
	I64 req_w_cumu;
	I64 Cumu_Dirty_Sectors;
	DWORD Effect_Merge;
	//計算同SLB cyl上平均寫多少不同DB
	DWORD *b_time, *b;
	DWORD time;
	double min_cyc, max_cyc;
	//SlotTable_Band紀錄各band寫入request數
	DWORD SlotTable_Act, SlotTable_InAct;
	SLOTTABLE_BAND *SlotTable_Band;
	DWORD Merged_Band;
	//Resp(Finish_t - Arr_t)
	DWORD Resp_Start;
	unsigned long long Arr_Time, Finish_Time;//unsigned long long
	unsigned long long Req_RunTime;
	unsigned long long RespTIme;//unsigned long long
	unsigned long long WaitingTime;
	unsigned long long RunTime;
	unsigned long long BaseTime;
	unsigned long long PreArr_Time;//
#ifdef REQUEST_CHANGE
	DWORD RC_R_cnt, RC_W_cnt;
	unsigned long long RC_R_Finish_Time, RC_W_Finish_Time;
	unsigned long long RC_R_WaitingTime, RC_W_WaitingTime;
#endif
	BYTE *DirtySector;//作用是?
	DWORD NSLBwriteout_sector;
#ifdef NSLB_LocalUse
	//FASTinitNSLB2, FASTinitNSLB3
	DWORD *DB2curNSLB_TK;
	//FASTinitNSLB3
	DWORD **ExcluNSLBTK, *NSLBTK_Share;
#endif
#ifdef POWERFAIL
	unsigned long long PF_SeekTime;
	DWORD PF_SeekCount;
	DWORD seek_0_PF;
	I64 PF_seek_distance;
	DWORD PF_RotTime, PF_SkewRotTime;
	unsigned long long PF_content_rotate;
	DWORD PF_pageWrite;
#endif
#ifdef Simulation_HLB
	DWORD *N2S_LBsector;
	DWORD N2S_LBsector_len;
#endif
	BYTE *DBalloc;//動態配DB,省記憶體
	//SLB統計PM會merge到那些Cylinder上
	DWORD LB_DirtySectors;
	DWORD *PMstatistic_DB; DWORD Merge_UniqBand, CheckMerge_Bandcount;
	  //DB Merge概況
	DWORD PM_NoGuard_Cylinders;
	DWORD Range_Cylinders, DirtyInRange_Cylinders;
	  //Clean Region數量和大小
	DWORD CleanRegion_Count, CleanRegion_CumuSize;
	DWORD CleanRegionMAx_Count, CleanRegionMAx_CumuSize;
	  //假設做PMG寫入有多少
	DWORD PMG_WriteCost_Sum;
	//PMG:記錄當前merge DB的資訊, 用來傳入子涵式
	SLB_MERGE_PARA SLBMPa;
	//統計Effect Merge會寫多少Data Cylinder
	DWORD *EM_DataCylinder, EM_DataCylinder_Len;
#ifdef Simulation_SLBPM
	DWORD CUTPM_SIG;
	DWORD CUTPM_CANDIDATE_SIG;
	DWORD MAX_PM_CYLINDER, NUM_PM_CYLINDER;
	PMG_RANK *PMG_Rank;
	DWORD *WB2SMR_TASK;
	DWORD WB2SMR_TASK_LEN;
	DWORD GET_FREE_LB_SECT;
	//
	DWORD GET_FREE_LB_SECT_SUM, GET_FREE_LB_SECT_cnt;
	//統計
	DWORD ALLOW_PM, PM_SUCCESS;
	DWORD NUM_PM_CYLINDER_Rate;
	DWORD GCyl_sector, LB_GCyl_sector, WB_LB_GCyl_sector;
	DWORD CUTPM_SIG0_Rate, CUTPM_SIG1_Rate, CUTPM_SIG2_Rate;
#endif
#ifdef Simulation_SLB_SHUFFLE
	TRANSFOR_LAYER *SHUFFLE_TL;
	DWORD *SHUFFLE_TL_Alloc;
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
	//觀察Band間Merge過程的變化
	DWORD *SHUFFLE_TL_OneTime;
	TRANSFOR_LAYER SHUFFLE_DB_Info;
	TRANSFOR_LAYER *SHUFFLE_TL_OBSERVE;
	//觀察Band內段落性聚集寫入特性
	MERGE_PERIOD_LOCALITY *BAND_MPL;
#endif
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE
	//MFMBO
	MFMBO *MFMBO_INFO;//配合SHUFFLE_TL_Alloc配記憶體
#endif
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE_TK
	//MFMBO
	MFMBO *MFMBO_INFO;//配合SHUFFLE_TL_Alloc配記憶體
#endif
#ifdef Simulation_SLB_DELAY
	//Delay的變數
	DWORD *DELAY_SECTORinBAND;
	DWORD *VictimCylinder_BUFF, VC_BUFF_Len, WB_BUFF_Len;
	DWORD cumuVC_BUFF_Len;
	DWORD *SHUFFLE_BOUND;//P Address //CONT_DELAY_LEFT
	//統計
	DWORD SLB_DELAY_USE, CumuSLB_DELAY_USE;
	DWORD SLB_DELAY_BAND, CumuSLB_DELAY_BAND;
	DWORD DELAY_cnt, MERGE_cnt;//Merge代表Delay Fail
	/**/ DWORD DELAY_SAVE_CYLINDER;
	//整體統計
	DWORD SLB_USE, CumuSLB_USE;
#ifdef Simulation_SLB_DELAY_ROUND
	DWORD *DRLB_CYLINDER;
#endif
	//Delay參數
	DWORD SLB_DELAY_SPACE;
	DWORD DirtyUnit_TD, SaveCylinder_TD;
	DWORD DELAY_PERIOD;
	DWORD *BAND_DELAY_PERIOD;
#endif
#endif
#ifdef SACE
	DWORD WriteBandLC;//寫在band LattCylinder
	DWORD WriteBandBGT;//寫在band上BeforeGuardTrack
	DWORD WriteSLB;
	DWORD MergeBGT;
	DWORD Effect_MergeBGT, HiddeInEffect_Merge;
#endif

#ifdef SLB_Mapping_Cache
	I64 Cache_Hit, Cache_Write_Miss,Cache_Read_Miss;
	I64 Cache_Replace, Cache_Drop;
	I64 Cache_Alldelay, Cache_Write, Cache_Read;
	LRU_CACHE *SLB_Map_Cache;
	I64 Cache_Merge;
	I64 Cache_Replace_withW, Cache_Merge_withW;
	//CFLRU
	I64 Cache_Hit2;
	I64 Cache_Alldelay2, Cache_Write2, Cache_Read2;
	I64 Cache_L1toL2;

#endif

}NFTL;
#endif

#ifdef SLBNSLBPM_Tmerge
struct PM_NODE
{
	//flashPage Page;
	DWORD Band_No, SLB_PageIndex;
	struct VSLB_NODE *next;
};
//LinkList.c使用的資料結構
struct Free_List	*FLt;
struct BPLRU_List	*BPLRU;//舊
struct NSLB_LIST	*NSLBlist;//NSLB Second-Chance 在用

FILE *ftime;
FILE *fp_SLB;
FILE *fp_NSLB;
#ifdef NEWGC64trackWRITE
FILE *fp_64trackWRITE;
#endif

typedef struct
{
	DWORD		SLBpage_num;
	DWORD		writedPageNum;//DWORD
	DWORD		guardTrack1stPage;//DWORD
	DWORD		partial_page;
	DWORD		firstRWpage; //DWORD
	DWORD		lastRWpage; //DWORD

	//NFTLstat
	I64		pageWrite;
	I64		pageRead;
	I64		mergecnt;
	I64		write_count;
	I64		merge_count;
}NFTL_SLB;

typedef struct
{
	DWORD		NSLBpage_num;//DWORD
	DWORD		writedPageNum;//DWORD
	DWORD		partial_page;
	DWORD		firstRWpage;
	DWORD		lastRWpage;

	//NFTLstat
	I64		pageWrite;
	I64		pageRead;
	I64		mergecnt;
	I64		write_count;
	I64		merge_count;
}NFTL_NSLB;

typedef struct{
	flashBlock	*blocks;				// the array of blocks,pages. mainly used to identify the logical addresses recorded in every spare of pages.
	unsigned int		*L2Ptable;				// the L2P table, in terms of blocks. Having LsizeBlock elements
	//unsigned int		*repTable;				// to chain the log blocks. Having PsizeBlock elements
	unsigned int		firstSpareBlock;		// the index of the first spare block. There might be many spare blocks. They are chained in the repTable
	unsigned int		lastSpareBlock;			// the index of the last spare block.	
	unsigned int		firstRWblock;			// oldest RWblock
	unsigned int		lastRWblock;			// writing RWblock
	unsigned int		partial_band; //<bs

	NFTLsectorElement_500Gup *st;
	NFTLconfig	configObj;
	NFTLstat	statObj;

	DWORD		partial_page;
	DWORD		firstRWpage; //DWORD
	DWORD		lastRWpage;	//DWORD			// the index of first free page of writing RWblock
	DWORD		writedPageNum; //DWORD
	DWORD		guardTrack1stPage; //DWORD
	DWORD		LBpage_num;


	NFTL_SLB	SLB;
	NFTL_NSLB	NSLB;

	I64		*BPLRU;//BPLRU_len = NC->LsizeBlock
	I64		 cnt;
	//NSLB新的部分
	DWORD   NSLB_tracks;
	DWORD   *NSLBremainSectorTable;
	DWORD   NSLBexhaustedTrack;

	struct BPLRU_GROUP	*BPLRUGROUP;//NSLBPListGCSLB 在用
	DWORD		NSLBGCtoDB_Threshold;
#ifdef SpecificTrack
	//FASTwriteSectorNSLBpart_SpecificTrack
	//DWORD   *Band_NSLBTrackIndex;
	//DWORD   NSLBusedTrack;

	//試驗
	//DWORD   *Band_Tracks;
	//DWORD   *Used_Sector;

	//試驗
	DWORD   NSLBusedTrack;
	NSLB_SpecificTrack *NSLB_TrackIndex;
	DWORD   *NSLB_Band;

	DWORD NSLB_RAM_SIZE;
	DWORD *NSLB_RAM;
	DWORD NSLB_RAM_request;

	BYTE *RAM_Valid;
	
	//統計RAM Hit吸收的寫入
	I64 RAM_WriteOut;
	I64 Host2RAM;
	DWORD RAM_Hit, RAM_Miss;

	//#ifdef NSLB_RAM_BandLRU_HIT
	//	DWORD *RAM_BPLRU;
	//	DWORD RAM_BPLRU_Time;
	//#endif
	
	//試驗merge len
	I64 ValidIs0, ValidIs2;//SLB page、NSLB page
	I64 SourceTracks_count;

#endif
#ifdef Time_Overhead
	I64 Write_SeekCount, Merge_SeekCount;
	I64 Write_pageWrite, Merge_pageWrite;

	//算真的track move, distance
	I64 Write_SeekDist, Merge_SeekDist;
	DWORD SLB_SeekDist;
	//算真的track move, distance
	I64 Merge_SeekDist2;

	//算真的track move, distance: curved
	double Merge_SeekDist3;
#endif//不使用
#ifdef Time_Overhead
	//未用
	//分別算write和merge的seek time
	double SLBWrite_SeekTime, SLBMerge_SeekTime;
	double NSLBWrite_SeekTime, NSLBMerge_SeekTime;
	//分別算write和merge的rotation time
	double SLBWrite_RotationTime, SLBMerge_RotationTime;
	double NSLBWrite_RotationTime, NSLBMerge_RotationTime;
	//InterXferTime: 總量是pageWrite*InterXferTime
	//統計rotation
	I64 rotation_preSectorInTrack;
	I64 rotation_count;//算的是轉幾個page，最後要除track size page才知道幾圈


	//分別算write和merge的seek time
	DWORD FirstTime;//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	double Write_W_SeekTime, Write_R_SeekTime, Merge_W_SeekTime, Merge_R_SeekTime;
	double Write_W_RotTime, Write_R_RotTime, Merge_W_RotTime, Merge_R_RotTime;
	//分別算write和merge的rotation time
	/*double SLBWrite_RotationTime, SLBMerge_RotationTime;
	double NSLBWrite_RotationTime, NSLBMerge_RotationTime;*/
	//seek次數
	DWORD Write_W_SeekCount, Write_R_SeekCount, Merge_W_SeekCount, Merge_R_SeekCount;
	DWORD seek_0;
	//disk head所在位置，用在每次計算兩個寫入的head位置差
	I64 pre_HeadSector, cur_HeadSector;
	//試驗
	DWORD WriteBuff_writecount;
	DWORD merge_ram_writecount;
	DWORD Merge_Bandcount;
	DWORD Merge_Trackcount;
	DWORD Merge_LB_Trackcount;
	DWORD WriteBuff_WriteOut;
	DWORD NoMerge_Band;

	//scaling formula
	double dScale;
	I64 Cylinders;
	I64 Scale_up, Scale_down;
	I64 SeekModelBoundary;
	//disk head所在位置，用在每次計算兩個寫入的head位置差
	I64 pre_HeadLocatedCylinder, cur_HeadLocatedCylinder;
	//試驗
	double d;
	//電梯法找LB所需要的merge_ram
	//N->merge_ram_size、N->merge_ram[]: 改成merge func裡的local變數，因為global的變數會被遞迴的下一層破壞
	//
	DWORD preNSLB_WriteSector;
	DWORD preNSLB_WriteTrack;
	DWORD NSLBmerge;

	//試驗
	DWORD preNSLB_reWriteSector;
	double NSLBreWrite_SeekTime;
	I64 reWrite_pre_HeadLocatedCylinder, reWrite_cur_HeadLocatedCylinder;

	//試驗
	DWORD WriteBuff_Size;
	DWORD *WriteBuff;
	DWORD SLBws, SLBms, NSLBws, NSLBms;
	DWORD SLBtrackChange, RDB, RMW, Merge;

	//試驗
	DWORD MergeCondition;

	//試驗
	DWORD PM_timer;
#endif

#ifdef NEWGC64trackWRITE
	DWORD PartialMerge_Threshold;

	//#ifdef NEWGC64trackWRITE
	DWORD PM_Rate;
	DWORD PMQ_Tnum, Free_Tnum; //PM_Tracks = PMQ_Tnum + Free_Tnum
	struct PM_NODE	*PMQ_HEAD, *PMQ_TAIL;//PMQ FIFO Queue;
	struct PM_NODE	*FreeQ;//Freepool for PMQ
	
	DWORD *PMSpace;
	DWORD PMlastBand;

	//試驗
	DWORD PM_Rate_SLBdefault;
	DWORD SLB_SeqTrackIndex;
	//試驗
	DWORD PM_Tracks, PM_Tracks_Use;
	I64 PM_len;
	DWORD PM_Round;
	DWORD pre_start, pos_start;
	I64 Rel_len;
	I64 Miss_PM_len, Miss_Rel_len;
	//試驗
	DWORD PM_GTsector;
	DWORD *PMCandidate;
	DWORD PM_Tracks_Threshold;

	DWORD SLB_tracks;
	DWORD *SLBremainSectorTable;
	DWORD SLBexhaustedTrack;
	DWORD preSLB_WriteSector;
	BYTE PM_Tracks_Allocated2Use_Condition;
	DWORD PM_Sectors_Use;

	DWORD GTFirstPut_Base[6], GTPut_Ext[4];
	DWORD GTRePut_Base[6];

	DWORD LB_tracks50p, LB_tracks75p, LB_tracks83p;

	//試驗
	DWORD PMGT_Signal;
	DWORD *PMGT_NSLBaddress;

	//試驗
	/*DWORD GTBuff_Size;
	DWORD *GTBuff;*/
	//
	/*DWORD PM_Sectors_Use;
	DWORD OrgPM_Cost;*/

	//統計成功做PM的比例
	I64	All_Merge, Allow_PM, DO_PM;

	
#ifdef Section_Mapping
	DWORD SubTrack;
#endif
#endif
#ifdef BPLRULlist_RunLen //用來計算NSLB、HLB、HLB+的BPLRU List在執行的時候有多少個band在裡面
	I64 BPLRUtotal_len, BPLRUtotal_count;
	I64 BPLRUcurrent_len;
	I64 BPLRUcurrent_max;
	I64 BPLRUcurrent_min;
#endif
}NFTL;
#endif
//#define printf(...) do {} while (0)


