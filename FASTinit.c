#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"
#include "memory.h"
//unsigned long = DWORD: 0..429 496 729 5

#ifdef Time_Overhead
void Init_Seagate_Cheetah15k5(sim *simPtr){//Init_Seagate_Cheetah15k5
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTL *N = &(simPtr->NFTLobj);
	//Seagate_Cheetah15k5
	NC->CylinderSizeTrack = 4;
	N->Disk.DiskSizeCylinder = 72172;//spec上寫: 72170
	// physical size: 140232MB ~ 140GB
	// zon bit recording紀錄的sector: 287194400 //spec上寫: 286749487
#ifdef Simulation_HDD
	N->Disk.DiskSizeMB = 140232;
	N->Disk.DiskSizeSector = 287194400;
	N->Disk.AvgTrackSize = 995;
	N->Disk.numZones = 14;
#endif
#if defined(Simulation_SLB) || defined(SACE) || defined(Simulation_NSLB)
	//加入ONVP空間
	N->Disk.DiskSizeMB = 140232;//158516 = 140232 + 
	N->Disk.DiskSizeSector = 287194400;//292938400 = 287194400 + 5744000
	N->Disk.AvgTrackSize = 995;
	N->Disk.numZones = 15;//資料從OD放到ID, 增設Zone[14]給SMR OVP
#endif
	//不使用: seek model boundary, 7261 = 300/2982*72171
	//72172 * 383 / 1962 = 14088
	N->SeekModelBoundary = 14088;//換14088再跑一次

	////Disk Zone-Bit-Record
	//N->Disk.Zone[0].FirstCylNo = 0;
	//N->Disk.Zone[0].LastCylNo = 7800;
	//N->Disk.Zone[0].ZoneSizeCyl = 7801;
	//N->Disk.Zone[0].TrackSizeSector = 1200;
	//N->Disk.Zone[0].ZoneSizeSector = 37444800;
	//N->Disk.Zone[0].deadspace = 0;
	////16356*7
	//N->Disk.Zone[1].FirstCylNo = 7801;
	//N->Disk.Zone[1].LastCylNo = 14200;
	//N->Disk.Zone[1].ZoneSizeCyl = 6400;
	//N->Disk.Zone[1].TrackSizeSector = 1160;
	//N->Disk.Zone[1].ZoneSizeSector = 29696000;
	//N->Disk.Zone[1].deadspace = 0;
	////19712*7
	//N->Disk.Zone[2].FirstCylNo = 14201;
	//N->Disk.Zone[2].LastCylNo = 20200;
	//N->Disk.Zone[2].ZoneSizeCyl = 6000;
	//N->Disk.Zone[2].TrackSizeSector = 1120;
	//N->Disk.Zone[2].ZoneSizeSector = 26880000;
	//N->Disk.Zone[2].deadspace = 0;
	////32760*7
	//N->Disk.Zone[3].FirstCylNo = 20201;
	//N->Disk.Zone[3].LastCylNo = 25900;
	//N->Disk.Zone[3].ZoneSizeCyl = 5700;
	//N->Disk.Zone[3].TrackSizeSector = 1104;
	//N->Disk.Zone[3].ZoneSizeSector = 25171200;
	//N->Disk.Zone[3].deadspace = 0;
	////32736*7
	//N->Disk.Zone[4].FirstCylNo = 25901;
	//N->Disk.Zone[4].LastCylNo = 31300;
	//N->Disk.Zone[4].ZoneSizeCyl = 5400;
	//N->Disk.Zone[4].TrackSizeSector = 1080;
	//N->Disk.Zone[4].ZoneSizeSector = 23328000;
	//N->Disk.Zone[4].deadspace = 0;
	////25432*7
	//N->Disk.Zone[5].FirstCylNo = 31301;
	//N->Disk.Zone[5].LastCylNo = 36400;
	//N->Disk.Zone[5].ZoneSizeCyl = 5100;
	//N->Disk.Zone[5].TrackSizeSector = 1020;
	//N->Disk.Zone[5].ZoneSizeSector = 20808000;
	//N->Disk.Zone[5].deadspace = 0;
	////28240*7
	//N->Disk.Zone[6].FirstCylNo = 36401;
	//N->Disk.Zone[6].LastCylNo = 41300;
	//N->Disk.Zone[6].ZoneSizeCyl = 4900;
	//N->Disk.Zone[6].TrackSizeSector = 994;
	//N->Disk.Zone[6].ZoneSizeSector = 19482400;
	//N->Disk.Zone[6].deadspace = 0;
	////26856*7
	//N->Disk.Zone[7].FirstCylNo = 41301;
	//N->Disk.Zone[7].LastCylNo = 46000;
	//N->Disk.Zone[7].ZoneSizeCyl = 4700;
	//N->Disk.Zone[7].TrackSizeSector = 960;
	//N->Disk.Zone[7].ZoneSizeSector = 18048000;
	//N->Disk.Zone[7].deadspace = 0;
	////
	//N->Disk.Zone[8].FirstCylNo = 46001;
	//N->Disk.Zone[8].LastCylNo = 50500;
	//N->Disk.Zone[8].ZoneSizeCyl = 4500;
	//N->Disk.Zone[8].TrackSizeSector = 920;
	//N->Disk.Zone[8].ZoneSizeSector = 16560000;
	//N->Disk.Zone[8].deadspace = 0;
	////
	//N->Disk.Zone[9].FirstCylNo = 50501;
	//N->Disk.Zone[9].LastCylNo = 54900;
	//N->Disk.Zone[9].ZoneSizeCyl = 4400;
	//N->Disk.Zone[9].TrackSizeSector = 880;
	//N->Disk.Zone[9].ZoneSizeSector = 15488000;
	//N->Disk.Zone[9].deadspace = 0;
	////
	//N->Disk.Zone[10].FirstCylNo = 54901;
	//N->Disk.Zone[10].LastCylNo = 59100;
	//N->Disk.Zone[10].ZoneSizeCyl = 4200;
	//N->Disk.Zone[10].TrackSizeSector = 840;
	//N->Disk.Zone[10].ZoneSizeSector = 14112000;
	//N->Disk.Zone[10].deadspace = 0;
	////
	//N->Disk.Zone[11].FirstCylNo = 59101;
	//N->Disk.Zone[11].LastCylNo = 63200;
	//N->Disk.Zone[11].ZoneSizeCyl = 4100;
	//N->Disk.Zone[11].TrackSizeSector = 816;
	//N->Disk.Zone[11].ZoneSizeSector = 13382400;
	//N->Disk.Zone[11].deadspace = 0;
	////
	//N->Disk.Zone[12].FirstCylNo = 63201;
	//N->Disk.Zone[12].LastCylNo = 67200;
	//N->Disk.Zone[12].ZoneSizeCyl = 4000;
	//N->Disk.Zone[12].TrackSizeSector = 780;
	//N->Disk.Zone[12].ZoneSizeSector = 12480000;
	//N->Disk.Zone[12].deadspace = 0;
	////
	//N->Disk.Zone[13].FirstCylNo = 67201;
	//N->Disk.Zone[13].LastCylNo = 72170;
	//N->Disk.Zone[13].ZoneSizeCyl = 4970;
	//N->Disk.Zone[13].TrackSizeSector = 720;
	//N->Disk.Zone[13].ZoneSizeSector = 14313600;
	//N->Disk.Zone[13].deadspace = 0;

	//Disk Zone-Bit-Record - reverse for logical address
	N->Disk.Zone[0].FirstCylNo = 0;
	N->Disk.Zone[0].LastCylNo = 4969;
	N->Disk.Zone[0].ZoneSizeCyl = 4970;
	N->Disk.Zone[0].TrackSizeSector = 720;
	N->Disk.Zone[0].ZoneSizeSector = 14313600;
	//
	N->Disk.Zone[1].FirstCylNo = 4970;
	N->Disk.Zone[1].LastCylNo = 8969;
	N->Disk.Zone[1].ZoneSizeCyl = 4000;
	N->Disk.Zone[1].TrackSizeSector = 780;
	N->Disk.Zone[1].ZoneSizeSector = 12480000;
	//
	N->Disk.Zone[2].FirstCylNo = 8970;
	N->Disk.Zone[2].LastCylNo = 13069;
	N->Disk.Zone[2].ZoneSizeCyl = 4100;
	N->Disk.Zone[2].TrackSizeSector = 816;
	N->Disk.Zone[2].ZoneSizeSector = 13382400;
	//
	N->Disk.Zone[3].FirstCylNo = 13070;
	N->Disk.Zone[3].LastCylNo = 17269;
	N->Disk.Zone[3].ZoneSizeCyl = 4200;
	N->Disk.Zone[3].TrackSizeSector = 840;
	N->Disk.Zone[3].ZoneSizeSector = 14112000;
	//
	N->Disk.Zone[4].FirstCylNo = 17270;
	N->Disk.Zone[4].LastCylNo = 21669;
	N->Disk.Zone[4].ZoneSizeCyl = 4400;
	N->Disk.Zone[4].TrackSizeSector = 880;
	N->Disk.Zone[4].ZoneSizeSector = 15488000;
	//
	N->Disk.Zone[5].FirstCylNo = 21670;
	N->Disk.Zone[5].LastCylNo = 26169;
	N->Disk.Zone[5].ZoneSizeCyl = 4500;
	N->Disk.Zone[5].TrackSizeSector = 920;
	N->Disk.Zone[5].ZoneSizeSector = 16560000;
	//
	N->Disk.Zone[6].FirstCylNo = 26170;
	N->Disk.Zone[6].LastCylNo = 30869;
	N->Disk.Zone[6].ZoneSizeCyl = 4700;
	N->Disk.Zone[6].TrackSizeSector = 960;
	N->Disk.Zone[6].ZoneSizeSector = 18048000;
	//
	N->Disk.Zone[7].FirstCylNo = 30870;
	N->Disk.Zone[7].LastCylNo = 35769;
	N->Disk.Zone[7].ZoneSizeCyl = 4900;
	N->Disk.Zone[7].TrackSizeSector = 994;
	N->Disk.Zone[7].ZoneSizeSector = 19482400;
	//
	N->Disk.Zone[8].FirstCylNo = 35770;
	N->Disk.Zone[8].LastCylNo = 40869;
	N->Disk.Zone[8].ZoneSizeCyl = 5100;
	N->Disk.Zone[8].TrackSizeSector = 1020;
	N->Disk.Zone[8].ZoneSizeSector = 20808000;
	//
	N->Disk.Zone[9].FirstCylNo = 40870;
	N->Disk.Zone[9].LastCylNo = 46269;
	N->Disk.Zone[9].ZoneSizeCyl = 5400;
	N->Disk.Zone[9].TrackSizeSector = 1080;
	N->Disk.Zone[9].ZoneSizeSector = 23328000;
	//
	N->Disk.Zone[10].FirstCylNo = 46270;
	N->Disk.Zone[10].LastCylNo = 51969;
	N->Disk.Zone[10].ZoneSizeCyl = 5700;
	N->Disk.Zone[10].TrackSizeSector = 1104;
	N->Disk.Zone[10].ZoneSizeSector = 25171200;
	//
	N->Disk.Zone[11].FirstCylNo = 51970;
	N->Disk.Zone[11].LastCylNo = 57969;
	N->Disk.Zone[11].ZoneSizeCyl = 6000;
	N->Disk.Zone[11].TrackSizeSector = 1120;
	N->Disk.Zone[11].ZoneSizeSector = 26880000;
	//
	N->Disk.Zone[12].FirstCylNo = 57970;
	N->Disk.Zone[12].LastCylNo = 64369;
	N->Disk.Zone[12].ZoneSizeCyl = 6400;
	N->Disk.Zone[12].TrackSizeSector = 1160;
	N->Disk.Zone[12].ZoneSizeSector = 29696000;
	//
	N->Disk.Zone[13].FirstCylNo = 64370;
	N->Disk.Zone[13].LastCylNo = 72170;
	N->Disk.Zone[13].ZoneSizeCyl = 7801;
	N->Disk.Zone[13].TrackSizeSector = 1200;
	N->Disk.Zone[13].ZoneSizeSector = 37444800;
	//SMR OVP
	N->Disk.Zone[14].FirstCylNo = 72171;
	N->Disk.Zone[14].LastCylNo = 73606;
	N->Disk.Zone[14].ZoneSizeCyl = 1436;
	N->Disk.Zone[14].TrackSizeSector = 1000;
	N->Disk.Zone[14].ZoneSizeSector = 5744000;

	// 0, 7800, 7801,1200,37444800
	//7801,14200,6400,1160,29696000
	//14201,20200,6000,1120,26880000
	//20201,25900,5700,1104,25171200
	//25901 31300 5400 1080 23328000
	//31301 36400 5100 1020 20808000
	//36401 41300 4900 994 19482400
	//41301 46000 4700 960 18048000
	/*DWORD FCN[15] = {    0,  7801, 14201, 20201, 25901, 31301, 36401, 41301 };
	DWORD LCN[15] = { 7800, 14200, 20200, 25900, 31300, 36400, 41300, 46000 };
	DWORD ZSC[15] = { 7801,	6400, 5700, 5400, 5100, 4900, 4700, 4700 };
	DWORD TSS[15] = { 1200, 1160, 1120, 1104, 1080, 1020, 994, 960 };
	DWORD ZSS[15] = { 37444800, 29696000, 26880000, 25171200, 23328000, 20808000, 19482400, 18048000};*/
}
void Init_HP_C3323A_validate(sim *simPtr){//N->SeekModelBoundary = 300;
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTL *N = &(simPtr->NFTLobj);
	//HP_C3323A_validate
	NC->CylinderSizeTrack = 7;
	N->Disk.DiskSizeMB = 1004;// 1031;
	N->Disk.numZones = 9;// 8;
	N->Disk.seekwritedelta = 0.07;
	//994*120 = 119280, 119280*7 = 834960
	N->Disk.Zone[0].FirstCylNo = 0;
	N->Disk.Zone[0].LastCylNo = 993;
	N->Disk.Zone[0].ZoneSizeCyl = 994;//625-0+1 // 993 - 0 + 1 = 994;
	N->Disk.Zone[0].TrackSizeSector = 120;// 120;
	N->Disk.Zone[0].ZoneSizeSector = 806760;
	N->Disk.Zone[0].deadspace = 1320;
	//16356*7
	N->Disk.Zone[1].FirstCylNo = 994;
	N->Disk.Zone[1].LastCylNo = 1134;
	N->Disk.Zone[1].ZoneSizeCyl = 141;//1134 - 994 + 1 = 141;
	N->Disk.Zone[1].TrackSizeSector = 116;
	N->Disk.Zone[1].ZoneSizeSector = 110432;
	N->Disk.Zone[1].deadspace = 0;
	//19712*7
	N->Disk.Zone[2].FirstCylNo = 1135;
	N->Disk.Zone[2].LastCylNo = 1310;
	N->Disk.Zone[2].ZoneSizeCyl = 176;// 1310 - 1135 + 1 = 176;
	N->Disk.Zone[2].TrackSizeSector = 112;
	N->Disk.Zone[2].ZoneSizeSector = 134848;
	N->Disk.Zone[2].deadspace = 0;
	//32760*7
	N->Disk.Zone[3].FirstCylNo = 1311;
	N->Disk.Zone[3].LastCylNo = 1625;
	N->Disk.Zone[3].ZoneSizeCyl = 315;// 1625 - 1311 + 1 = 315;
	N->Disk.Zone[3].TrackSizeSector = 104;
	N->Disk.Zone[3].ZoneSizeSector = 224224;
	N->Disk.Zone[3].deadspace = 0;
	//32736*7
	N->Disk.Zone[4].FirstCylNo = 1626;
	N->Disk.Zone[4].LastCylNo = 1966;
	N->Disk.Zone[4].ZoneSizeCyl = 341;// 1966 - 1626 + 1 = 341;
	N->Disk.Zone[4].TrackSizeSector = 96;
	N->Disk.Zone[4].ZoneSizeSector = 224448;
	N->Disk.Zone[4].deadspace = 0;
	//25432*7
	N->Disk.Zone[5].FirstCylNo = 1967;
	N->Disk.Zone[5].LastCylNo = 2255;
	N->Disk.Zone[5].ZoneSizeCyl = 289;// 2255 - 1967 + 1 = 289;
	N->Disk.Zone[5].TrackSizeSector = 88;
	N->Disk.Zone[5].ZoneSizeSector = 174944;
	N->Disk.Zone[5].deadspace = 0;
	//28240*7
	N->Disk.Zone[6].FirstCylNo = 2256;
	N->Disk.Zone[6].LastCylNo = 2608;
	N->Disk.Zone[6].ZoneSizeCyl = 353;// 2608 - 2256 + 1 = 353;
	N->Disk.Zone[6].TrackSizeSector = 80;
	N->Disk.Zone[6].ZoneSizeSector = 194880;
	N->Disk.Zone[6].deadspace = 0;
	//26856*7
	N->Disk.Zone[7].FirstCylNo = 2609;
	N->Disk.Zone[7].LastCylNo = 2981;
	N->Disk.Zone[7].ZoneSizeCyl = 373;// 2981 - 2609 + 1 = 373;
	N->Disk.Zone[7].TrackSizeSector = 72;// 72;
	N->Disk.Zone[7].ZoneSizeSector = 185472;
	N->Disk.Zone[7].deadspace = 0;
	//extra
	N->Disk.Zone[8].FirstCylNo = 2982;
	N->Disk.Zone[8].LastCylNo = 3354;
	N->Disk.Zone[8].ZoneSizeCyl = 373;// 2981 - 2609 + 1 = 373;
	N->Disk.Zone[8].TrackSizeSector = 72;// 72;
	N->Disk.Zone[8].ZoneSizeSector = 185472;
	N->Disk.Zone[8].deadspace = 0;

	//SLB time pos
	N->cur_index = 0; N->req_len = 0;

	////全部2109604 sector
	////Segate_ST41601N_validate
	//NC->CylinderSizeTrack = 17;
	//N->Disk.DiskSizeMB = 1308;// 1031;
	//N->Disk.numZones = 14;
	//N->Disk.seekwritedelta = 0.07;
	////994*120 = 119280, 119280*7 = 834960
	//N->Disk.Zone[0].FirstCylNo = 0;
	//N->Disk.Zone[0].LastCylNo = 625;//993
	//N->Disk.Zone[0].ZoneSizeCyl = 626;//625-0+1 // 993 - 0 + 1 = 994;
	//N->Disk.Zone[0].TrackSizeSector = 85;// 120;
	//N->Disk.Zone[0].ZoneSizeSector = 900814;
	//N->Disk.Zone[0].deadspace = 0;
	////16356*7
	//N->Disk.Zone[1].FirstCylNo = 626;// 994;
	//N->Disk.Zone[1].LastCylNo = 700;// 1134;
	//N->Disk.Zone[1].ZoneSizeCyl = 75;//1134 - 994 + 1 = 141;
	//N->Disk.Zone[1].TrackSizeSector = 83;// 116;
	//N->Disk.Zone[1].ZoneSizeSector = 105375;
	//N->Disk.Zone[1].deadspace = 0;
	////19712*7
	//N->Disk.Zone[2].FirstCylNo = 701;// 1135;
	//N->Disk.Zone[2].LastCylNo = 800;// 1310;
	//N->Disk.Zone[2].ZoneSizeCyl = 100;// 1310 - 1135 + 1 = 176;
	//N->Disk.Zone[2].TrackSizeSector = 82;// 112;
	//N->Disk.Zone[2].ZoneSizeSector = 138800;
	//N->Disk.Zone[2].deadspace = 0;
	////32760*7
	//N->Disk.Zone[3].FirstCylNo = 801;// 1311;
	//N->Disk.Zone[3].LastCylNo = 925;// 1625;
	//N->Disk.Zone[3].ZoneSizeCyl = 125;// 1625 - 1311 + 1 = 315;
	//N->Disk.Zone[3].TrackSizeSector = 79;// 104;
	//N->Disk.Zone[3].ZoneSizeSector = 167125;
	//N->Disk.Zone[3].deadspace = 0;
	////32736*7
	//N->Disk.Zone[4].FirstCylNo = 926;// 1626;
	//N->Disk.Zone[4].LastCylNo = 1050;// 1966;
	//N->Disk.Zone[4].ZoneSizeCyl = 125;// 1966 - 1626 + 1 = 341;
	//N->Disk.Zone[4].TrackSizeSector = 78;// 96;
	//N->Disk.Zone[4].ZoneSizeSector = 165000;
	//N->Disk.Zone[4].deadspace = 0;
	////25432*7
	//N->Disk.Zone[5].FirstCylNo = 1051;// 1967;
	//N->Disk.Zone[5].LastCylNo = 1175;// 2255;
	//N->Disk.Zone[5].ZoneSizeCyl = 125;// 2255 - 1967 + 1 = 289;
	//N->Disk.Zone[5].TrackSizeSector = 76;// 88;
	//N->Disk.Zone[5].ZoneSizeSector = 160750;
	//N->Disk.Zone[5].deadspace = 0;
	////28240*7
	//N->Disk.Zone[6].FirstCylNo = 1176;// 2256;
	//N->Disk.Zone[6].LastCylNo = 1300;// 2608;
	//N->Disk.Zone[6].ZoneSizeCyl = 125;// 2608 - 2256 + 1 = 353;
	//N->Disk.Zone[6].TrackSizeSector = 73;// 80;
	//N->Disk.Zone[6].ZoneSizeSector = 154375;
	//N->Disk.Zone[6].deadspace = 0;
	////26856*7
	//N->Disk.Zone[7].FirstCylNo = 1301;// 2609;
	//N->Disk.Zone[7].LastCylNo = 1400;// 2981;
	//N->Disk.Zone[7].ZoneSizeCyl = 100;// 2981 - 2609 + 1 = 373;
	//N->Disk.Zone[7].TrackSizeSector = 72;// 72;
	//N->Disk.Zone[7].ZoneSizeSector = 121800;
	//N->Disk.Zone[7].deadspace = 0;
	////
	//N->Disk.Zone[8].FirstCylNo = 1401;
	//N->Disk.Zone[8].LastCylNo = 1500;
	//N->Disk.Zone[8].ZoneSizeCyl = 100;
	//N->Disk.Zone[8].TrackSizeSector = 69;
	//N->Disk.Zone[8].ZoneSizeSector = 116700;
	//N->Disk.Zone[8].deadspace = 0;
	////
	//N->Disk.Zone[9].FirstCylNo = 1501;
	//N->Disk.Zone[9].LastCylNo = 1600;
	//N->Disk.Zone[9].ZoneSizeCyl = 100;
	//N->Disk.Zone[9].TrackSizeSector = 68;
	//N->Disk.Zone[9].ZoneSizeSector = 115000;
	//N->Disk.Zone[9].deadspace = 0;
	////
	//N->Disk.Zone[10].FirstCylNo = 1601;
	//N->Disk.Zone[10].LastCylNo = 1800;
	//N->Disk.Zone[10].ZoneSizeCyl = 200;
	//N->Disk.Zone[10].TrackSizeSector = 65;
	//N->Disk.Zone[10].ZoneSizeSector = 219800;
	//N->Disk.Zone[10].deadspace = 0;
	////
	//N->Disk.Zone[11].FirstCylNo = 1801;
	//N->Disk.Zone[11].LastCylNo = 1900;
	//N->Disk.Zone[11].ZoneSizeCyl = 100;
	//N->Disk.Zone[11].TrackSizeSector = 63;
	//N->Disk.Zone[11].ZoneSizeSector = 106500;
	//N->Disk.Zone[11].deadspace = 0;
	////
	//N->Disk.Zone[12].FirstCylNo = 1901;
	//N->Disk.Zone[12].LastCylNo = 2000;
	//N->Disk.Zone[12].ZoneSizeCyl = 100;
	//N->Disk.Zone[12].TrackSizeSector = 62;
	//N->Disk.Zone[12].ZoneSizeSector = 104800;
	//N->Disk.Zone[12].deadspace = 0;
	////
	//N->Disk.Zone[13].FirstCylNo = 2001;
	//N->Disk.Zone[13].LastCylNo = 2097;
	//N->Disk.Zone[13].ZoneSizeCyl = 98;
	//N->Disk.Zone[13].TrackSizeSector = 61;
	//N->Disk.Zone[13].ZoneSizeSector = 100007;
	//N->Disk.Zone[13].deadspace = 0;
	//--------------------------------------------

	//-------------------------------------------
	//NC->CylinderSizeTrack = 14;
	//N->Disk.DiskSizeMB = 1031;
	//N->Disk.numZones = 1;
	////994*120 = 119280, 119280*7 = 834960
	//N->Disk.Zone[0].FirstCylNo = 0;
	//N->Disk.Zone[0].LastCylNo = 2598;
	//N->Disk.Zone[0].ZoneSizeCyl = 2599;//625-0+1 // 993 - 0 + 1 = 994;
	//N->Disk.Zone[0].TrackSizeSector = 58;// 120;
	//N->Disk.Zone[0].ZoneSizeSector = 806760;
}

void Init_TimeOverhead(sim *simPtr){
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); NFTL *N = &(simPtr->NFTLobj);
	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
	//seek時間: 分別算write和merge的seek time
	N->Write_W_SeekTime = 0; N->Write_R_SeekTime = 0; N->Merge_W_SeekTime = 0; N->Merge_R_SeekTime = 0;
	N->Merge_RLB_SeekTime = 0;
	N->Write_RLB_SeekTime = 0; N->Write_RLB_SeekCount = 0;//
	N->Write_RDB_SeekTime = 0; N->Write_RDB_SeekCount = 0;//
	N->WB_SeekTime = 0; N->WB_SeekCount = 0;//
	N->SeekTime = 0;
	//seek次數
	N->Write_W_SeekCount = 0; N->Write_R_SeekCount = 0;
	N->Merge_RLB_SeekCount = 0;
	N->Merge_W_SeekCount = 0; N->Merge_R_SeekCount = 0;
	N->WB_SeekCount = 0;//
	//無效seek次數
	N->seek_0 = 0;
	N->seek_0_WW = 0; N->seek_0_WR = 0; N->seek_0_MW = 0; N->seek_0_MR = 0;
	N->seek_0_MRLB = 0;
	N->seek_0_WB = 0;//
	//seek距離
	N->seek_distance = 0;
	N->W_seek_distance = 0;
	N->MR_SLB_seek_distance = 0;
	N->MR_DB_seek_distance = 0;
	N->MWseek_distance = 0;
	N->WB_seek_distance = 0;//
	//未依比例縮放前seek距離
	N->rW_seek_distance = 0;
	N->rMR_SLB_seek_distance = 0;
	N->rMR_DB_seek_distance = 0;
	N->rMWseek_distance = 0;
	//rotation時間
	N->Previous_LTA = 0;//
	N->Write_W_RotTime = 0; N->Write_R_RotTime = 0; N->Merge_W_RotTime = 0; N->Merge_R_RotTime = 0;
	N->Merge_RLB_RotTime = 0;
	N->WB_RotTime = 0;
	N->Write_W_SkewRotTime = 0; N->Write_R_SkewRotTime = 0; N->Merge_W_SkewRotTime = 0; N->Merge_R_SkewRotTime = 0;
	N->Merge_RLB_SkewRotTime = 0;
	N->WB_SkewRotTime = 0;
	N->FirstTime = 0;//未使用 之後刪掉
	//xfer時間
	N->content_rotate = 0;
	N->WW_content_rotate = 0; N->WR_content_rotate = 0; N->MR_content_rotate = 0; N->MW_content_rotate = 0;
	N->WB_content_rotate = 0;//
#ifdef POWERFAIL
	N->seek_0_PF = 0;
	N->PF_SeekTime = 0; N->PF_SeekCount = 0; N->PF_seek_distance = 0;
	N->PF_RotTime = 0; N->PF_SkewRotTime = 0;
	N->PF_content_rotate = 0;
	//統計
	N->PF_pageWrite = 0;
#endif
	//scaling formula: default Archive HDD 8TB
	//8TB有15628053168個sector，8001563222016 Byte，635907個cylinder
	//這邊換設定可能造成seek的改變 //scale cylinder

	//HP c3323
	//各自初始化: 
	//NC->CylinderSizeTrack  17;// 7;// 8;
	//N->Cylinders = NC->PsizeSector / NC->trackSizePage / NC->CylinderSizeTrack;
	//N->SeekModelBoundary = 300;// 34829;// 15993;// 383;// 15000; //616; // 15000;

	//N->Scale_up = 81928; N->Scale_down = N->Cylinders;// 1962 //(83303, 635907):default Archive HDD 635907 cylinders
	
	N->d = 0;
	//Init_HP_C3323A_validate(simPtr);
	Init_Seagate_Cheetah15k5(simPtr);

	//disk head所在位置，用在每次計算兩個寫入的head位置差
	N->pre_HeadCylinder = 0; N->cur_HeadCylinder = 0;// (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack;
	N->pre_HeadTrack = 0; N->cur_HeadTrack = 0;
	N->pre_HeadSector = 0; N->cur_HeadSector = 0;// (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack;//假設disk head起始位置在LB的第0 track
	
	//電梯法找LB所需要的merge_ram
	//N->merge_ram_size、N->merge_ram[]: 改成merge func裡的local變數，因為global的變數會被遞迴的下一層破壞
	//紀錄NSLB上一個寫入的page位置、是否merge剛結束
	N->preNSLB_WriteSector = 0; N->NSLBmerge = 0;
	N->preNSLB_WriteTrack = 0;
	//統計rotation
	N->rotation_preSectorInTrack = 0;
	N->rotation_count = 0;

	//額外統計: reWrite_
	N->NSLBreWrite_SeekTime = 0;
	N->reWrite_pre_HeadLocatedCylinder = (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack;
	N->reWrite_cur_HeadLocatedCylinder = (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack;//假設disk head起始位置在LB的第0 track

	//以track為單位RMW或是做merge，1MB
	N->WriteBuff_SIZE = NC->RMWbufferSizeTrack * NC->trackSizeSection;
	N->WriteBuff = (DWORD*)calloc(N->WriteBuff_SIZE, sizeof(DWORD));
	N->WriteBuff_section = 0;

	//以track為單位RMW或是做merge，1MB
	N->NMR_RMWBuff_SIZE = NC->RMWbufferSizeTrack * NC->trackSizeSection;
	N->NMR_RMWBuff = (DWORD*)calloc(N->WriteBuff_SIZE, sizeof(DWORD));
	N->NMR_RMWBuff_section = 0;

	N->WriteBuff_writecount = 0;
	N->merge_ram_writecount = 0;
	N->Merge_Bandcount = 0;
	N->Merge_Trackcount = 0;
	N->Merge_LB_Trackcount = 0;
	N->WriteBuff_WriteOut = 0;
	N->NMR_Rotate = 0;
	N->NoMerge_Band = 0;

	printf("WriteBuff_SIZE %lu, NMR_RMWBuff_SIZE %lu\n", N->WriteBuff_SIZE, N->NMR_RMWBuff_SIZE);
	N->SLBws = 0; N->SLBms = 0; N->NSLBws = 0; N->NSLBms = 0;
	N->SLBtrackChange = 0; N->RDB = 0; N->RMW = 0; N->Merge = 0;

	N->RAM_WAIT = 0;
	N->Wait_SeekTime = 0;
	N->Wait_SeekCount = 0;
	N->Wait_RotTime = 0;
	N->Wait_content_rotate = 0;
	printf("Time Overhead Initial ok\n");
}
#endif

#ifdef READ_REQUEST
void FASTinitRead(sim *simPtr){
	NFTLconfig *NC = &(simPtr->NFTLobj.configObj); 
	NFTL *N = &(simPtr->NFTLobj);
	simStat *SS = &(simPtr->simStatObj);
	//讀取統計
	SS->reqSectorRead_max = 0;;
	SS->readReq = 0;
	SS->reqSectorRead = 0;
	N->ReqR_pageRead = 0;
	//seek 
	N->Read_SeekTime = 0;
	//seek次數
	N->R_SeekCount = 0;
	//無效seek次數
	N->seek_0_R = 0;
	//seek距離
	N->R_seek_distance = 0;
	//rotation時間
	N->Read_RotTime = 0;
	N->Read_SkewRotTime = 0;
	//xfer
	N->R_content_rotate = 0;
	//Response Time
	N->R_RespTIme = 0;
	N->R_WaitingTime = 0;
	N->R_RunTime = 0;
	//加入Read之後Response會混合RW, 因此這邊另外統計純Write的Resp
	N->W_RespTIme = 0;
	N->W_WaitingTime = 0;
	N->W_RunTime = 0;
}
#endif

#ifdef Simulation_HDD
void FASTinitHDD(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	/*new init*/
	//printf("FASTinit CLB ...\n");
	//NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	//NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	//NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	////NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	//NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	//NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	//NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	//NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	//NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	//NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	//NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	//NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	////NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");
	//assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	//assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	//assert(NC->LsizeByte % NC->blockSizeByte == 0);
	////track
	//NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	//NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	//NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	////block
	//NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	//NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	//NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	//NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	//NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	////logical
	//NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	//NC->LsizePage = NC->LsizeSector;//之後砍掉
	//NC->LsizeSection = NC->LsizePage / NC->Section;
	//NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	//NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	//NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	//NC->LsizeMB = NC->LsizeByte /1024 / 1024;
	////physical
	//NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	//NC->PsizePage = NC->PsizeSector;//之後砍掉
	//NC->PsizeSection = NC->PsizePage / NC->Section;;
	//NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	//NC->PsizeBlock = NC->LsizeBlock + 1;
	//NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	//NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//printf("NC variables\n");
	//printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	//printf("Section = %lu sector\n", NC->Section);
	//printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	//printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);
	//printf("Logical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	//printf("Pogical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	//printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);
	//printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	printf("NC->pageSizeByte %I64u, NC->sectorSizeByte %I64u\n", NC->pageSizeByte, NC->sectorSizeByte);
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->MAX_CUT = simGetDWORD(SC, "GEOMETRY", "MAXCUT");
	NC->FREE_CUT = simGetDWORD(SC, "GEOMETRY", "FREECUT");
	NC->WASTE_CUT = simGetDWORD(SC, "GEOMETRY", "WASTECUT");
	NC->cut_def = simGetDWORD(SC, "GEOMETRY", "CUTDEF");
	NC->delay_threshold = simGetDWORD(SC, "GEOMETRY", "DELAY_THRES");
	NC->cut_afterwrite = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERWRITE"); //write request period
	NC->cut_aftermerge = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERMERGE"); //merge log band period
	assert(NC->cut_afterwrite ^ NC->cut_aftermerge > 0); //只能選一個 0|x xor x|0
	//#define FreeList_byValid2
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "SectionSizeSector");//SubTrack
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");//RamSizeTrack
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");//
	NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");
	printf("NC->PsizeByte: %I64u byte\n", NC->PsizeByte); //要扣掉data和guard track 
	printf("NC->LsizeByte: %I64u byte\n", NC->LsizeByte);
	NC->PsizeByte += NC->trackSizeByte * NC->FREE_CUT;
	NC->PsizeByte -= NC->trackSizeByte * NC->WASTE_CUT;
	assert(NC->LsizeByte%NC->blockSizeByte == 0);
	NC->LsizeBlock = NC->LsizeByte / NC->blockSizeByte;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	//N->partial_band = NC->PsizeBlock - 1;
	//計算LB的partial page
	//N->partial_page = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	printf("NC->LsizeBlock: %lu, NC->PsizeBlock: %lu\n", NC->LsizeBlock, NC->PsizeBlock);
	//assert(NC->trackSizeByte - 1 <= N->partial_page);
	//printf("N->partial_band: %lu, N->partial_page:%lu\n", (DWORD)N->partial_band, N->partial_page);
	printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);
	assert(NC->trackSizeByte % NC->pageSizeByte == 0); assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->pageSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->pageSizeByte == 0);
	NC->trackSizePage = NC->trackSizeByte / NC->pageSizeByte;//外加
	NC->trackSizeSection = NC->trackSizePage / NC->Section;
	NC->pageSizeSector = NC->pageSizeByte / NC->sectorSizeByte;
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeByte / NC->pageSizeByte;
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeByte / NC->pageSizeByte;
	NC->LsizeSection = NC->LsizeSector / NC->Section;
	NC->PsizeSector = NC->PsizeByte / NC->pageSizeByte;  //(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeByte / NC->pageSizeByte; //(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	printf("NC variables\n"); printf("NC->PsizePage: %lu\n", NC->PsizePage);
	printf("NC->LsizePage: %lu\n", NC->LsizePage);
	printf("NC->trackSizePage: %lu\n", NC->trackSizePage);
	printf("NC->blockSizePage: %lu\n", NC->blockSizePage);
	NC->blockSizeTrack = (NC->blockSizePage / NC->trackSizePage);// assert(NC->blockSizeTrack == 64); //外加
	NC->blockSizeSection = NC->blockSizeSector / NC->Section; //init(simPtr); HDD不需要sector table
	NC->LsizeMB = NC->LsizeSector / 2048;//sector 512 Byte
	NC->PsizeMB = NC->PsizeSector / 2048;//sector 512 Byte


	N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
	N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
	N->writedPageNum = 0;
	N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	N->LBpage_num = 0;// N->partial_page + 1 - NC->trackSizePage;
	printf("FASTinit CLB OK\n");
#ifdef SpecificTrack
	//RAM結構
	N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	N->NSLB_RAM_request = 0;
	N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }

	//統計RAM Hit吸收的寫入
	N->RAM_WriteOut = 0;
	N->Host2RAM = 0;
	//試驗
	N->RAM_Hit = 0;
	N->RAM_Miss = 0;
	printf("SpecificTrack init OK\n");
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
	printf("Time_Overhead init OK\n");
#endif
	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;
}
#endif
#ifdef Simulation_SLB
void FASTinitSLB(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	//不用
	//NC->MAX_CUT = simGetDWORD(SC, "GEOMETRY", "MAXCUT");
	//NC->FREE_CUT = simGetDWORD(SC, "GEOMETRY", "FREECUT");
	//NC->WASTE_CUT = simGetDWORD(SC, "GEOMETRY", "WASTECUT");
	//NC->cut_def = simGetDWORD(SC, "GEOMETRY", "CUTDEF");
	//NC->delay_threshold = simGetDWORD(SC, "GEOMETRY", "DELAY_THRES");
	//NC->cut_afterwrite = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERWRITE"); //write request period
	//NC->cut_aftermerge = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERMERGE"); //merge log band period
	//assert(NC->cut_afterwrite ^ NC->cut_aftermerge > 0); //只能選一個 0|x xor x|0
	//NC->PsizeByte += NC->trackSizeByte * NC->FREE_CUT;
	//NC->PsizeByte -= NC->trackSizeByte * NC->WASTE_CUT;
	
	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizeSector / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte /1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizeSector / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	//printf("%lu %lu %lu %lu \n", NC->PsizeSector, NC->LsizeSector, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);

	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack)-1;//最後一個
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;//最後一個
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;//有幾個
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;//有幾個
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);
	
	printf("Logical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);
	
	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	//printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	//printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	//printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);

	/*配置block*/
	//N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	//N->blocks[0].pages = (flashPage *)calloc(NC->PsizePage, sizeof(flashPage)); assert(N->blocks[0].pages != NULL);//all phys pages(+guard band)
	//for (i = 0; i < NC->PsizeBlock; i++) {
	//	N->blocks[i].pages = N->blocks[0].pages + i*(NC->blockSizePage + NC->trackSizePage);//set of a data band and GT
	//}
	//init(simPtr);
	//for (i = 0; i < NC->LsizeBlock; i++) {
	//	//N->L2Ptable[i] = i;					// map LBA i to PBA i
	//	N->blocks[i].Merge = 0;
	//	for (j = 0; j < NC->blockSizePage; j++) {
	//		N->blocks[i].pages[j].sector = i*NC->blockSizeSector + j*NC->pageSizeSector;	// map the sector # to page		
	//		N->blocks[i].pages[j].valid = 1;
	//	}
	//}
	/*for (i = 0; i < NC->LsizeBlock; i++){
	for (j = 0; j < NC->blockSizePage; j += (NC->blockSizePage - 1)) {
	assert(0 <= N->blocks[i].pages[j].sector && N->blocks[i].pages[j].sector < NC->LsizePage);
	assert(N->blocks[i].pages[j].valid == 1);
	}
	}*/
	//for (j = 0; j <= N->partial_page; j++){
	//	//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
	//	N->blocks[NC->PsizeBlock - 1].pages[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
	//	N->blocks[NC->PsizeBlock - 1].pages[j].valid = 0;
	//	assert(N->blocks[NC->PsizeBlock - 1].pages[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].pages[j].valid == 0);
	//}
	//printf("	log buffer pages: %I64u\n", N->partial_page);
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	for (i = 0; i < NC->PsizeBlock; i++) {
		N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	}
	init(simPtr);//st table初始化
	for (i = 0; i < NC->LsizeBlock; i++) {
		//N->blocks[i].Merge = 0;//暫時清掉 要跑加回來
		for (j = 0; j < NC->blockSizeSection; j++) {
			N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
			N->blocks[i].sections[j].valid = 1;
			//N->blocks[i].sections[j].unique = 0;
		}
	}
	/*for (j = 0; j < NC->blockSizeSection; j++) {
	printf("%lu %lu, %lu %lu\n", N->blocks[0].sections[j].sector, N->blocks[0].sections[j].valid, N->blocks[1].sections[j].sector, N->blocks[1].sections[j].valid);
	}
	system("pause");*/
	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	for (i = 0; i < NC->LsizeBlock; i++){
		assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
		assert(N->blocks[i].sections[0].valid == 1);
	}
	for (j = 0; j <= N->Partial_Section; j++){
		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
		N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
		N->blocks[NC->PsizeBlock - 1].sections[j].valid = 0;
		assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
	}
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	//N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
	//N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
	//N->writedPageNum = 0;
	//N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	//printf("FASTinit CLB OK\n");
	//NS->mergecnt = 0;
	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->firstRWpage = 0;
	//SLB->lastRWpage = 0;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
	//SLB->writedPageNum = 0;
	//SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	//SLB->write_count = 0; SLB->merge_count = 0;
	//printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);


	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;

	printf("SLB Partial_Section %lu\n", SLB->Partial_Section);

	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;

	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	//printf("SpecificTrack init\n");
	//printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
	N->S_MRS = 0;
	N->S_CT = 0;
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;

	if (NC->Section == 1){ assert(NC->LsizeSection == NC->LsizeSector); }
	/*N->section_cumu = (DWORD*)calloc(NC->LsizeSection, sizeof(DWORD));
	for(i=0;i<NC->LsizeSection;i++){ 
		N->section_cumu[i]=0;
	}*/
	N->req_w_cumu = 0;

	N->Cumu_Dirty_Sectors = 0;
	N->Effect_Merge = 0;

	//計算同SLB cyl上平均寫多少不同DB
	N->b_time = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	N->b = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i<NC->LsizeBlock; i++){ assert(N->b_time[i] == 0); assert(N->b[i] == 0); }
	N->time = 0;
	N->min_cyc = 0;
	N->max_cyc = 0;

	
	N->SlotTable_Band = (SLOTTABLE_BAND*)calloc(NC->LsizeBlock, sizeof(SLOTTABLE_BAND));
	for (i = 0; i < NC->LsizeBlock; i++){ 
		N->SlotTable_Band[i].Act = 0;
		N->SlotTable_Band[i].InAct = 0;
	}
	N->SlotTable_Act = 0; N->SlotTable_InAct = 0;
	N->Merged_Band = 0;

	printf("%lu %lu\n", 23000 * (I64)SLB->SLBsection_num / 50331648, 34500 * (I64)SLB->SLBsection_num / 50331648);
	printf("%lu\n", 0xFFFFFFFF);

	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;
}

void FASTinitSLB_NoSt(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizeSector / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte /1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizeSector / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	//printf("%lu %lu %lu %lu \n", NC->PsizeSector, NC->LsizeSector, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);

	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack)-1;//最後一個
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;//最後一個
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;//有幾個
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;//有幾個
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	//printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	//printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	//printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);

	/*配置block*/
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	for (i = 0; i < NC->PsizeBlock; i++) {
		N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	}
	//init(simPtr);//st table初始化
	for (i = 0; i < NC->LsizeBlock; i++) {
		//N->blocks[i].Merge = 0;//暫時清掉 要跑加回來
		for (j = 0; j < NC->blockSizeSection; j++) {
			N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
			N->blocks[i].sections[j].valid = 1;
			//N->blocks[i].sections[j].unique = 0;
		}
	}
	
	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	for (i = 0; i < NC->LsizeBlock; i++){
		assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
		assert(N->blocks[i].sections[0].valid == 1);
	}
	for (j = 0; j <= N->Partial_Section; j++){
		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
		N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
		N->blocks[NC->PsizeBlock - 1].sections[j].valid = 0;
		assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
	}
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	//N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
	//N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
	//N->writedPageNum = 0;
	//N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	//printf("FASTinit CLB OK\n");
	//NS->mergecnt = 0;
	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->firstRWpage = 0;
	//SLB->lastRWpage = 0;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
	//SLB->writedPageNum = 0;
	//SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	//SLB->write_count = 0; SLB->merge_count = 0;
	//printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);

	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;

	printf("SLB Partial_Section %lu\n", SLB->Partial_Section);

	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;

	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	//printf("SpecificTrack init\n");
	//printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
	N->S_MRS = 0;
	N->S_CT = 0;
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;

	if (NC->Section == 1){ assert(NC->LsizeSection == NC->LsizeSector); }
	/*N->section_cumu = (DWORD*)calloc(NC->LsizeSection, sizeof(DWORD));
	for(i=0;i<NC->LsizeSection;i++){ 
	N->section_cumu[i]=0;
	}*/
	N->req_w_cumu = 0;

	N->Cumu_Dirty_Sectors = 0;
	N->Effect_Merge = 0;

	//計算同SLB cyl上平均寫多少不同DB
	N->b_time = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	N->b = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i<NC->LsizeBlock; i++){ assert(N->b_time[i] == 0); assert(N->b[i] == 0); }
	N->time = 0;
	N->min_cyc = 0;
	N->max_cyc = 0;

	N->SlotTable_Band = (SLOTTABLE_BAND*)calloc(NC->LsizeBlock, sizeof(SLOTTABLE_BAND));
	for (i = 0; i < NC->LsizeBlock; i++){ 
		N->SlotTable_Band[i].Act = 0;
		N->SlotTable_Band[i].InAct = 0;
	}
	N->SlotTable_Act = 0; N->SlotTable_InAct = 0;
	N->Merged_Band = 0;
	printf("%lu %lu\n", 23000 * (I64)SLB->SLBsection_num / 50331648, 34500 * (I64)SLB->SLBsection_num / 50331648);
	printf("%lu\n", 0xFFFFFFFF);
	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;
}

void FASTinitSLB_NoStNoDB(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//Cylinder
	NC->CylinderSizeSector = NC->CylinderSizeTrack*NC->trackSizeSector; assert(NC->CylinderSizeSector == 4096);
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizeSector / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte /1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizeSector / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	//printf("%lu %lu %lu %lu \n", NC->PsizeSector, NC->LsizeSector, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);

	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack)-1;//最後一個
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;//最後一個
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;//有幾個
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;//有幾個
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %I64u sector, %I64u section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %I64u sector, %I64u section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	printf("GT sector: %lu(%lu MB)", NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack / 1024 / 1024);
	//printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	//printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	//printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);

	///*配置block*/
	//N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	//N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	//for (i = 0; i < NC->PsizeBlock; i++) {
	//	N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	//}
	////init(simPtr);//st table初始化
	//for (i = 0; i < NC->LsizeBlock; i++) {
	//	N->blocks[i].Merge = 0;
	//	for (j = 0; j < NC->blockSizeSection; j++) {
	//		N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
	//		N->blocks[i].sections[j].valid = 1;
	//		//N->blocks[i].sections[j].unique = 0;
	//	}
	//}
	//for (i = 0; i < NC->LsizeBlock; i++){
	//	assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
	//	assert(N->blocks[i].sections[0].valid == 1);
	//}

	/*配置block*/
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	for (i = 0; i < NC->LsizeBlock; i++){
		N->blocks[i].sections = NULL;
	}
	N->blocks[NC->LsizeBlock].sections = (flashPage *)calloc(N->LB_LastSector + 1, sizeof(flashPage)); 
	assert(N->blocks[NC->LsizeBlock].sections != NULL);
	for (j = 0; j <= N->LB_LastSector; j++){
		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
		N->blocks[NC->LsizeBlock].sections[j].sector = 0xffffffffffff;// 0x3fffffff;//means the sector is not yet mapping to data band
		N->blocks[NC->LsizeBlock].sections[j].valid = 0;
		assert(N->blocks[NC->LsizeBlock].sections[j].sector == 0xffffffffffff); assert(N->blocks[NC->LsizeBlock].sections[j].valid == 0);
	}
	N->DBalloc = (BYTE *)calloc(NC->LsizeBlock, sizeof(BYTE)); assert(N->DBalloc != NULL);
	for (i = 0; i < NC->LsizeBlock; i++){ N->DBalloc[i] = 0; }
	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	//N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
	//N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
	//N->writedPageNum = 0;
	//N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	//printf("FASTinit CLB OK\n");
	//NS->mergecnt = 0;
	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->firstRWpage = 0;
	//SLB->lastRWpage = 0;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
	//SLB->writedPageNum = 0;
	//SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	//SLB->write_count = 0; SLB->merge_count = 0;
	//printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);

	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;

	printf("SLB Partial_Section %lu\n", SLB->Partial_Section);

	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;

	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	//printf("SpecificTrack init\n");
	//printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
	N->S_MRS = 0;
	N->S_CT = 0;
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;
	N->l2pmapOvd.max_AA = 0;
	if (NC->Section == 1){ assert(NC->LsizeSection == NC->LsizeSector); }
	/*N->section_cumu = (DWORD*)calloc(NC->LsizeSection, sizeof(DWORD));
	for(i=0;i<NC->LsizeSection;i++){ 
	N->section_cumu[i]=0;
	}*/
	N->req_w_cumu = 0;

	N->Cumu_Dirty_Sectors = 0;
	N->Effect_Merge = 0;
	//計算同SLB cyl上平均寫多少不同DB
	//N->b_time = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	//N->b = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	//for (i = 0; i<NC->LsizeBlock; i++){ assert(N->b_time[i] == 0); assert(N->b[i] == 0); }
	N->time = 0;
	N->min_cyc = 0;
	N->max_cyc = 0;
	N->SlotTable_Band = (SLOTTABLE_BAND*)calloc(NC->LsizeBlock, sizeof(SLOTTABLE_BAND));
	for (i = 0; i < NC->LsizeBlock; i++){ 
		N->SlotTable_Band[i].Act = 0;
		N->SlotTable_Band[i].InAct = 0;
	}
	N->SlotTable_Act = 0; N->SlotTable_InAct = 0;
	N->Merged_Band = 0;
	//
	//N->DirtySector = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSector; i++){ assert(N->DirtySector[i] == 0); }
	printf("%lu %lu\n", 23000 * (I64)SLB->SLBsection_num / 50331648, 34500 * (I64)SLB->SLBsection_num / 50331648);
	printf("%lu\n", 0xFFFFFFFF);
	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;
	N->BaseTime = 0;//Trace Replay需要把前一輪的Finish_Time累加進總時間
	//SLB統計PM會merge到那些Cylinder上
	N->LB_DirtySectors = 0;
	N->PMstatistic_DB = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->PMstatistic_DB[i] == 0); }
	N->Merge_UniqBand = 0; N->CheckMerge_Bandcount = 0;
	  //DB Merge概況
	N->PM_NoGuard_Cylinders = 0;
	N->Range_Cylinders = 0;
	N->DirtyInRange_Cylinders = 0;
	  //Clean Region數量和大小
	N->CleanRegion_Count = 0; N->CleanRegion_CumuSize = 0;
	N->CleanRegionMAx_Count = 0; N->CleanRegionMAx_CumuSize = 0;
	  //假設做PMG寫入有多少
	N->PMG_WriteCost_Sum = 0;

	//統計Effect Merge會寫多少Data Cylinder
	N->EM_DataCylinder_Len = 5;
	N->EM_DataCylinder = (DWORD*)calloc(N->EM_DataCylinder_Len, sizeof(DWORD));
	for (i = 0; i < N->EM_DataCylinder_Len; i++){ assert(N->EM_DataCylinder[i]==0); }
}
#endif
#ifdef Simulation_SLB_SHUFFLE
void FASTinitSLB_NoStNoDB(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
#ifdef Simulation_SLB_DELAY
	DWORD para_SC = simGetDWORD(SC, "GEOMETRY", "SC");
	DWORD para_DU = simGetDWORD(SC, "GEOMETRY", "DU");
	DWORD para_DS = simGetDWORD(SC, "GEOMETRY", "DS");
#endif
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//Cylinder
	NC->CylinderSizeSector = NC->CylinderSizeTrack*NC->trackSizeSector; assert(NC->CylinderSizeSector == 4096);
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizeSector / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte / 1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizeSector / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	//printf("%lu %lu %lu %lu \n", NC->PsizeSector, NC->LsizeSector, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);

	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack) - 1;//最後一個
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;//最後一個
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;//有幾個
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;//有幾個
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %I64u sector, %I64u section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %I64u sector, %I64u section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	printf("GT sector: %lu(%lu MB)", NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack / 1024 / 1024);
	//printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	//printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	//printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);

	///*配置block*/
	//N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	//N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	//for (i = 0; i < NC->PsizeBlock; i++) {
	//	N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	//}
	////init(simPtr);//st table初始化
	//for (i = 0; i < NC->LsizeBlock; i++) {
	//	N->blocks[i].Merge = 0;
	//	for (j = 0; j < NC->blockSizeSection; j++) {
	//		N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
	//		N->blocks[i].sections[j].valid = 1;
	//		//N->blocks[i].sections[j].unique = 0;
	//	}
	//}
	//for (i = 0; i < NC->LsizeBlock; i++){
	//	assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
	//	assert(N->blocks[i].sections[0].valid == 1);
	//}

	/*配置block*/
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	for (i = 0; i < NC->LsizeBlock; i++){
		N->blocks[i].sections = NULL;

	}
	N->blocks[NC->LsizeBlock].sections = (flashPage *)calloc(N->LB_LastSector + 1, sizeof(flashPage));
	assert(N->blocks[NC->LsizeBlock].sections != NULL);
	for (j = 0; j <= N->LB_LastSector; j++){
		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
		N->blocks[NC->LsizeBlock].sections[j].sector = 0xffffffffffff;// 0x3fffffff;//means the sector is not yet mapping to data band
		N->blocks[NC->LsizeBlock].sections[j].valid = 0;

		assert(N->blocks[NC->LsizeBlock].sections[j].sector == 0xffffffffffff); assert(N->blocks[NC->LsizeBlock].sections[j].valid == 0);
	}
	N->DBalloc = (BYTE *)calloc(NC->LsizeBlock, sizeof(BYTE)); assert(N->DBalloc != NULL);
	for (i = 0; i < NC->LsizeBlock; i++){ N->DBalloc[i] = 0; }
	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	//N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
	//N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
	//N->writedPageNum = 0;
	//N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	//printf("FASTinit CLB OK\n");
	//NS->mergecnt = 0;
	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->firstRWpage = 0;
	//SLB->lastRWpage = 0;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
	//SLB->writedPageNum = 0;
	//SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	//SLB->write_count = 0; SLB->merge_count = 0;
	//printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);

	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;
	printf("SLB Partial_Section %lu\n", SLB->Partial_Section);

	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;

	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	//printf("SpecificTrack init\n");
	//printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
	N->S_MRS = 0;
	N->S_CT = 0;
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;
	N->l2pmapOvd.max_AA = 0;
	if (NC->Section == 1){ assert(NC->LsizeSection == NC->LsizeSector); }
	/*N->section_cumu = (DWORD*)calloc(NC->LsizeSection, sizeof(DWORD));
	for(i=0;i<NC->LsizeSection;i++){
	N->section_cumu[i]=0;
	}*/
	N->req_w_cumu = 0;

	N->Cumu_Dirty_Sectors = 0;
	N->Effect_Merge = 0;

	//計算同SLB cyl上平均寫多少不同DB
	N->b_time = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	N->b = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i<NC->LsizeBlock; i++){ assert(N->b_time[i] == 0); assert(N->b[i] == 0); }
	N->time = 0;
	N->min_cyc = 0;
	N->max_cyc = 0;

	N->SlotTable_Band = (SLOTTABLE_BAND*)calloc(NC->LsizeBlock, sizeof(SLOTTABLE_BAND));
	for (i = 0; i < NC->LsizeBlock; i++){
		N->SlotTable_Band[i].Act = 0;
		N->SlotTable_Band[i].InAct = 0;
	}
	N->SlotTable_Act = 0; N->SlotTable_InAct = 0;
	N->Merged_Band = 0;
	
	/*N->DirtySector = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
	for (i = 0; i < NC->LsizeSector; i++){ assert(N->DirtySector[i] == 0); }*/
	printf("%lu %lu\n", 23000 * (I64)SLB->SLBsection_num / 50331648, 34500 * (I64)SLB->SLBsection_num / 50331648);
	printf("%lu\n", 0xFFFFFFFF);
	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;
	N->BaseTime = 0;
	//假做PMG的統計->統計結果顯示會有很多Clean Region,將這些Clean Region聚集往前放->發展出Shuffle Merge
	//SLB統計PM會merge到那些Cylinder上
	N->LB_DirtySectors = 0;
	N->PMstatistic_DB = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->PMstatistic_DB[i] == 0); }
	N->Merge_UniqBand = 0; N->CheckMerge_Bandcount = 0;
	//DB Merge概況
	N->PM_NoGuard_Cylinders = 0;
	N->Range_Cylinders = 0;
	N->DirtyInRange_Cylinders = 0;
	//Clean Region數量和大小
	N->CleanRegion_Count = 0; N->CleanRegion_CumuSize = 0;
	N->CleanRegionMAx_Count = 0; N->CleanRegionMAx_CumuSize = 0;
	//假設做PMG寫入有多少
	N->PMG_WriteCost_Sum = 0;
#ifdef Simulation_SLB_SHUFFLE
	//TRANSFOR_LAYER
	N->SHUFFLE_TL = (TRANSFOR_LAYER*)calloc(NC->LsizeBlock, sizeof(TRANSFOR_LAYER));
	for (i = 0; i < NC->LsizeBlock; i++){ 
		N->SHUFFLE_TL[i].L2P_Index = NULL; 
		N->SHUFFLE_TL[i].P2L_Index = NULL;
		N->SHUFFLE_TL[i].WriteFreq = NULL;
		N->SHUFFLE_TL[i].DirtyUnit = NULL;
	}
	//紀錄有沒有配過
	N->SHUFFLE_TL_Alloc = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->SHUFFLE_TL_Alloc[i] == 0); }
#ifdef Simulation_SLB_SHUFFLE_OBSERVE
	//限定只能Shuffle 1 次
	N->SHUFFLE_TL_OneTime = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->SHUFFLE_TL_OneTime[i] == 0); }
	//統計整體情況
	N->SHUFFLE_DB_Info.L2P_Index = NULL;
	N->SHUFFLE_DB_Info.P2L_Index = NULL;
	N->SHUFFLE_DB_Info.WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	N->SHUFFLE_DB_Info.DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
	for (i = 0; i < NC->blockSizeCylinder; i++){
		N->SHUFFLE_DB_Info.WriteFreq[i] = 0;
		N->SHUFFLE_DB_Info.DirtyUnit[i] = 0;
	}
	//統計各DB內部情況
	N->SHUFFLE_TL_OBSERVE = (TRANSFOR_LAYER*)calloc(NC->LsizeBlock, sizeof(TRANSFOR_LAYER));
	N->BAND_MPL = (MERGE_PERIOD_LOCALITY*)calloc(NC->LsizeBlock, sizeof(MERGE_PERIOD_LOCALITY));
	for (i = 0; i < NC->LsizeBlock; i++){
		N->SHUFFLE_TL_OBSERVE[i].L2P_Index = NULL;
		N->SHUFFLE_TL_OBSERVE[i].P2L_Index = NULL;
		N->SHUFFLE_TL_OBSERVE[i].WriteFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
		N->SHUFFLE_TL_OBSERVE[i].DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
		//N->SHUFFLE_TL_OBSERVE[i].MergeFreq = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));//
		N->BAND_MPL[i].CUMU_TIME = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
		N->BAND_MPL[i].TEMP_TIME = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
		N->BAND_MPL[i].PERIOD = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
		N->BAND_MPL[i].PRE_DirtyUnit = (DWORD*)calloc(NC->blockSizeCylinder, sizeof(DWORD));
		for (j = 0; j < NC->blockSizeCylinder; j++){
			N->SHUFFLE_TL_OBSERVE[i].WriteFreq[j] = 0;
			N->SHUFFLE_TL_OBSERVE[i].DirtyUnit[j] = 0;
			//N->SHUFFLE_TL_OBSERVE[i].MergeFreq[j] = 0;//
			N->BAND_MPL[i].CUMU_TIME[j] = 0;
			N->BAND_MPL[i].TEMP_TIME[j] = 0;
			N->BAND_MPL[i].PERIOD[j] = 0;
			N->BAND_MPL[i].PRE_DirtyUnit[j] = 0;
		}
	}
#endif
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE
	N->MFMBO_INFO = (MFMBO*)calloc(NC->LsizeBlock, sizeof(MFMBO));
	for (i = 0; i < NC->LsizeBlock; i++){
		N->MFMBO_INFO[i].MergeFreq = NULL;
		N->MFMBO_INFO[i].MergeDS = NULL;
	}
#endif
#ifdef MOST_FREQ_MERGE_BAND_OBSERVE_TK
	N->MFMBO_INFO = (MFMBO*)calloc(NC->LsizeBlock, sizeof(MFMBO));
	for (i = 0; i < NC->LsizeBlock; i++){
		N->MFMBO_INFO[i].MergeFreq = NULL;
		N->MFMBO_INFO[i].MergeDS = NULL;
	}
#endif
#ifdef Simulation_SLB_DELAY
	//Delay
	//SLB目前裝多少Delay Sector, 累積統計的值 
	N->SLB_DELAY_USE = 0; N->CumuSLB_DELAY_USE = 0;
	N->SLB_USE = 0; N->CumuSLB_USE = 0;
	//目前多少Band正在Delay, 累積統計的值 
	N->SLB_DELAY_BAND = 0; N->CumuSLB_DELAY_BAND = 0;
	N->DELAY_SECTORinBAND = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->DELAY_SECTORinBAND[i] == 0); }
	N->DELAY_cnt = 0; /**/N->DELAY_SAVE_CYLINDER = 0;
	N->MERGE_cnt = 0;
	N->VictimCylinder_BUFF = (DWORD*)calloc(NC->CylinderSizeSector, sizeof(DWORD));
	for (i = 0; i < NC->CylinderSizeSector; i++){ assert(N->VictimCylinder_BUFF[i] == 0); }
	N->VC_BUFF_Len = 0; N->cumuVC_BUFF_Len = 0;
#ifdef Simulation_SLB_DELAY_ROUND//DRLB, Delay Round Log Buffer
	N->DRLB_CYLINDER = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->DRLB_CYLINDER[i] == 0); }
#endif
	N->SHUFFLE_BOUND = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->DRLB_CYLINDER[i] == 0); }

	//Delay參數
	N->SaveCylinder_TD = para_SC;//4 8 16 24
	N->DirtyUnit_TD = para_DU;//128 1024 4096 8192
	N->SLB_DELAY_SPACE = para_DS * SLB->SLBsection_num / 100;//10 20 40 60
	N->DELAY_PERIOD = DWORD_MAX;// 1 5 10 20 40 DWORD_MAX

	N->BAND_DELAY_PERIOD = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->BAND_DELAY_PERIOD[i] == 0); }
#endif
#endif

#ifdef SLB_Mapping_Cache
#ifdef CFLRU  //L1:L2 = 1:4
	DWORD Total_Cache_size = SLB->SLBsection_num / 50; // 20%:/5
	N->SLB_Map_Cache[0].cache_size = Total_Cache_size /5 ;
	N->SLB_Map_Cache[1].cache_size = Total_Cache_size - N->SLB_Map_Cache[0].cache_size;
	printf("Cache Total Size: %lu\n", Total_Cache_size);
	printf("L1 size %lu\n", N->SLB_Map_Cache[0].cache_size);
	//assert((N->SLB_Map_Cache[0].cache_size + N->SLB_Map_Cache[1].cache_size) == (SLB->SLBsection_num / 5 * 2));
#else
	N->SLB_Map_Cache->cache_size = SLB->SLBsection_num / 5 ; //預設20%valid data 57MB
#endif
#endif

	/*N->blocks[NC->LsizeBlock].sections[0].valid = 0;
	printf("%lu\n", N->blocks[NC->LsizeBlock].sections[0].valid);
	N->blocks[NC->LsizeBlock].sections[0].valid = 1;
	printf("%lu\n", N->blocks[NC->LsizeBlock].sections[0].valid);
	N->blocks[NC->LsizeBlock].sections[0].valid = 3;
	printf("%lu\n", N->blocks[NC->LsizeBlock].sections[0].valid);
	N->blocks[NC->LsizeBlock].sections[0].valid = 0;*/
}
#endif
#ifdef SACE
void FASTinitSLB_NoStNoDB(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//Cylinder
	NC->CylinderSizeSector = NC->CylinderSizeTrack*NC->trackSizeSector; assert(NC->CylinderSizeSector == 4096);
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizeSector / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte /1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizeSector / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	//printf("%lu %lu %lu %lu \n", NC->PsizeSector, NC->LsizeSector, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);

	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack)-1;//最後一個
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;//最後一個
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;//有幾個
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;//有幾個
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %I64u sector, %I64u section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %I64u sector, %I64u section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	printf("GT sector: %lu(%lu MB)", NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack / 1024 / 1024);
	//printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	//printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	//printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);

	///*配置block*/
	//N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	//N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	//for (i = 0; i < NC->PsizeBlock; i++) {
	//	N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	//}
	////init(simPtr);//st table初始化
	//for (i = 0; i < NC->LsizeBlock; i++) {
	//	N->blocks[i].Merge = 0;
	//	for (j = 0; j < NC->blockSizeSection; j++) {
	//		N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
	//		N->blocks[i].sections[j].valid = 1;
	//		//N->blocks[i].sections[j].unique = 0;
	//	}
	//}
	//for (i = 0; i < NC->LsizeBlock; i++){
	//	assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
	//	assert(N->blocks[i].sections[0].valid == 1);
	//}

	/*配置block*/
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	for (i = 0; i < NC->LsizeBlock; i++){
		N->blocks[i].sections = NULL;
#ifdef SACE
		N->blocks[i].SACE_TKDirtySector = NULL;
#endif
	}
	N->blocks[NC->LsizeBlock].sections = (flashPage *)calloc(N->LB_LastSector + 1, sizeof(flashPage)); 
	assert(N->blocks[NC->LsizeBlock].sections != NULL);
	for (j = 0; j <= N->LB_LastSector; j++){
		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
		N->blocks[NC->LsizeBlock].sections[j].sector = 0xffffffffffff;// 0x3fffffff;//means the sector is not yet mapping to data band
		N->blocks[NC->LsizeBlock].sections[j].valid = 0;
		assert(N->blocks[NC->LsizeBlock].sections[j].sector == 0xffffffffffff); assert(N->blocks[NC->LsizeBlock].sections[j].valid == 0);
	}
	N->DBalloc = (BYTE *)calloc(NC->LsizeBlock, sizeof(BYTE)); assert(N->DBalloc != NULL);
	for (i = 0; i < NC->LsizeBlock; i++){ N->DBalloc[i] = 0; }
	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	//N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
	//N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
	//N->writedPageNum = 0;
	//N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	//printf("FASTinit CLB OK\n");
	//NS->mergecnt = 0;
	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->firstRWpage = 0;
	//SLB->lastRWpage = 0;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
	//SLB->writedPageNum = 0;
	//SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	//SLB->write_count = 0; SLB->merge_count = 0;
	//printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);

	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;

	printf("SLB Partial_Section %lu\n", SLB->Partial_Section);

	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;

	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	//printf("SpecificTrack init\n");
	//printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
	N->S_MRS = 0;
	N->S_CT = 0;
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;
	N->l2pmapOvd.max_AA = 0;
	if (NC->Section == 1){ assert(NC->LsizeSection == NC->LsizeSector); }
	/*N->section_cumu = (DWORD*)calloc(NC->LsizeSection, sizeof(DWORD));
	for(i=0;i<NC->LsizeSection;i++){ 
	N->section_cumu[i]=0;
	}*/
	N->req_w_cumu = 0;

	N->Cumu_Dirty_Sectors = 0;
	N->Effect_Merge = 0;
	//計算同SLB cyl上平均寫多少不同DB
	//N->b_time = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	//N->b = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	//for (i = 0; i<NC->LsizeBlock; i++){ assert(N->b_time[i] == 0); assert(N->b[i] == 0); }
	N->time = 0;
	N->min_cyc = 0;
	N->max_cyc = 0;
	N->SlotTable_Band = (SLOTTABLE_BAND*)calloc(NC->LsizeBlock, sizeof(SLOTTABLE_BAND));
	for (i = 0; i < NC->LsizeBlock; i++){ 
		N->SlotTable_Band[i].Act = 0;
		N->SlotTable_Band[i].InAct = 0;
	}
	N->SlotTable_Act = 0; N->SlotTable_InAct = 0;
	N->Merged_Band = 0;
	//
	//N->DirtySector = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSector; i++){ assert(N->DirtySector[i] == 0); }
	printf("%lu %lu\n", 23000 * (I64)SLB->SLBsection_num / 50331648, 34500 * (I64)SLB->SLBsection_num / 50331648);
	printf("%lu\n", 0xFFFFFFFF);
	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;
	N->BaseTime = 0;//Trace Replay需要把前一輪的Finish_Time累加進總時間
	//SLB統計PM會merge到那些Cylinder上
	N->LB_DirtySectors = 0;
	N->PMstatistic_DB = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->PMstatistic_DB[i] == 0); }
	N->Merge_UniqBand = 0; N->CheckMerge_Bandcount = 0;
	//DB Merge概況
	N->PM_NoGuard_Cylinders = 0;
	N->Range_Cylinders = 0;
	N->DirtyInRange_Cylinders = 0;
	//Clean Region數量和大小
	N->CleanRegion_Count = 0; N->CleanRegion_CumuSize = 0;
	N->CleanRegionMAx_Count = 0; N->CleanRegionMAx_CumuSize = 0;
	//假設做PMG寫入有多少
	N->PMG_WriteCost_Sum = 0;

	//統計Effect Merge會寫多少Data Cylinder
	N->EM_DataCylinder_Len = 5;
	N->EM_DataCylinder = (DWORD*)calloc(N->EM_DataCylinder_Len, sizeof(DWORD));
	for (i = 0; i < N->EM_DataCylinder_Len; i++){ assert(N->EM_DataCylinder[i]==0); }
#ifdef SACE
	N->WriteBandLC = 0;//寫在band LattCylinder
	N->WriteBandBGT = 0;//寫在band上BeforeGuardTrack
	N->WriteSLB = 0;
	N->MergeBGT = 0;
	N->Effect_MergeBGT = 0; N->HiddeInEffect_Merge = 0;
#endif
}
#endif

#ifdef SLB_Mapping_Cache
CacheInit(sim *simPtr)
{
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB);
	NFTL *N = &(simPtr->NFTLobj);
#ifdef CFLRU //2 level LRU
	N->SLB_Map_Cache = (LRU_CACHE *)calloc(2, sizeof(LRU_CACHE));
	BYTE k ;
	for (k = 0; k < 2; k++){
		N->SLB_Map_Cache[k].LRU_Head = (LRU_Map_Entry*)calloc(1, sizeof(LRU_Map_Entry));
		N->SLB_Map_Cache[k].LRU_Head->LA = 0;
		N->SLB_Map_Cache[k].LRU_Head->PA = 0;
		N->SLB_Map_Cache[k].LRU_Head->next = N->SLB_Map_Cache->LRU_Head;
		N->SLB_Map_Cache[k].LRU_Head->prev = N->SLB_Map_Cache->LRU_Head;
		N->SLB_Map_Cache[k].cache_used = 0;
	}
	N->Cache_Hit2 = 0;
	N->Cache_Alldelay2 = 0;
	N->Cache_Write2 = 0;
	N->Cache_Read2 = 0;
	N->Cache_L1toL2 = 0;
	N->Cache_Replace_withW = 0;
	N->Cache_Merge_withW = 0;
	N->Cache_Merge = 0;

#else
	N->SLB_Map_Cache = calloc(1, sizeof(LRU_CACHE));
	N->SLB_Map_Cache->cache_used = 0;
	N->SLB_Map_Cache->LRU_Head = (LRU_Map_Entry*)calloc(1, sizeof(LRU_Map_Entry));//empty head
	N->SLB_Map_Cache->LRU_Head->LA = 0;
	N->SLB_Map_Cache->LRU_Head->PA = 0;
	N->SLB_Map_Cache->LRU_Head->next = N->SLB_Map_Cache->LRU_Head;
	N->SLB_Map_Cache->LRU_Head->prev = N->SLB_Map_Cache->LRU_Head;
#endif
	//N->SLB_Map_Cache->cache_size = SLB->SLBsection_num ; //cache_size init in SHUFFLE

	N->Cache_Hit = 0;
	N->Cache_Read_Miss = 0;
	N->Cache_Write_Miss = 0;
	N->Cache_Alldelay = 0;
	N->Cache_Drop = 0;
	N->Cache_Read = 0;
	N->Cache_Replace = 0;
	N->Cache_Write = 0;
}
#endif


#ifdef Simulation_SLBPM
void FASTinitSLB_NoStNoDB(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
#ifdef Simulation_SLBPM
	NC->PMRatio = simGetDWORD(SC, "GEOMETRY", "PMRatio");
#endif
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//Cylinder
	NC->CylinderSizeSector = NC->CylinderSizeTrack*NC->trackSizeSector; assert(NC->CylinderSizeSector == 4096);
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizeSector / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte / 1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizeSector / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	//printf("%lu %lu %lu %lu \n", NC->PsizeSector, NC->LsizeSector, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);

	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack) - 1;//最後一個
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;//最後一個
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;//有幾個
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;//有幾個
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %I64u sector, %I64u section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %I64u sector, %I64u section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	printf("GT sector: %lu(%lu MB)", NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack / 1024 / 1024);
	//printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	//printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	//printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);

	///*配置block*/
	//N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	//N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	//for (i = 0; i < NC->PsizeBlock; i++) {
	//	N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	//}
	////init(simPtr);//st table初始化
	//for (i = 0; i < NC->LsizeBlock; i++) {
	//	N->blocks[i].Merge = 0;
	//	for (j = 0; j < NC->blockSizeSection; j++) {
	//		N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
	//		N->blocks[i].sections[j].valid = 1;
	//		//N->blocks[i].sections[j].unique = 0;
	//	}
	//}
	//for (i = 0; i < NC->LsizeBlock; i++){
	//	assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
	//	assert(N->blocks[i].sections[0].valid == 1);
	//}

	/*配置block*/
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	for (i = 0; i < NC->LsizeBlock; i++){
		N->blocks[i].sections = NULL;
		N->blocks[i].Cut = 0;//PM
		N->blocks[i].PM = 0;//PM
		N->blocks[i].GR = 0;//PM暫時還Guard
	}
	N->blocks[NC->LsizeBlock].sections = (flashPage *)calloc(N->LB_LastSector + 1, sizeof(flashPage));
	assert(N->blocks[NC->LsizeBlock].sections != NULL);
	for (j = 0; j <= N->LB_LastSector; j++){
		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
		N->blocks[NC->LsizeBlock].sections[j].sector = 0xffffffffffff;// 0x3fffffff;//means the sector is not yet mapping to data band
		N->blocks[NC->LsizeBlock].sections[j].valid = 0;
		assert(N->blocks[NC->LsizeBlock].sections[j].sector == 0xffffffffffff); assert(N->blocks[NC->LsizeBlock].sections[j].valid == 0);
	}
	N->DBalloc = (BYTE *)calloc(NC->LsizeBlock, sizeof(BYTE)); assert(N->DBalloc != NULL);
	for (i = 0; i < NC->LsizeBlock; i++){ N->DBalloc[i] = 0; }
	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	//N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
	//N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
	//N->writedPageNum = 0;
	//N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	//printf("FASTinit CLB OK\n");
	//NS->mergecnt = 0;
	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->firstRWpage = 0;
	//SLB->lastRWpage = 0;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
	//SLB->writedPageNum = 0;
	//SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	//SLB->write_count = 0; SLB->merge_count = 0;
	//printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);

	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;
	printf("SLB Partial_Section %lu\n", SLB->Partial_Section);
#ifdef Simulation_SLBPM
	SLB->SLBcylinder_num = SLB->SLBsection_num / NC->trackSizeSector / NC->CylinderSizeTrack;//為了做PM
	N->MAX_PM_CYLINDER = SLB->SLBcylinder_num * NC->PMRatio / 100;
	N->NUM_PM_CYLINDER = 0;
	N->NUM_PM_CYLINDER_Rate = 0;
	N->CUTPM_SIG = 0;
	N->WB2SMR_TASK = (DWORD*)calloc(NC->CylinderSizeSector, sizeof(DWORD));
	N->WB2SMR_TASK_LEN = 0;
	//統計
	N->ALLOW_PM = 0;
	N->PM_SUCCESS = 0;

	N->GET_FREE_LB_SECT_SUM = 0;
	N->GET_FREE_LB_SECT_cnt = 0;

	N->PMG_Rank = (PMG_RANK*)calloc(N->MAX_PM_CYLINDER, sizeof(PMG_RANK));
	//N->PMG_Rank[0].DB_No = (DWORD)11; N->PMG_Rank[0].PMG_WriteCyl = (DWORD)20;
	//printf("%lu %lu\n", N->PMG_Rank[0].DB_No, N->PMG_Rank[0].PMG_WriteCyl); system("pause");
	N->NUM_PM_CYLINDER_Rate = 0;
	N->GCyl_sector = 0; N->LB_GCyl_sector = 0; N->WB_LB_GCyl_sector = 0;
	N->CUTPM_SIG0_Rate = 0; N->CUTPM_SIG1_Rate = 0; N->CUTPM_SIG2_Rate = 0;
	
#endif
	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;

	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	//printf("SpecificTrack init\n");
	//printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
	N->S_MRS = 0;
	N->S_CT = 0;
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;
	N->l2pmapOvd.max_AA = 0;
	if (NC->Section == 1){ assert(NC->LsizeSection == NC->LsizeSector); }
	/*N->section_cumu = (DWORD*)calloc(NC->LsizeSection, sizeof(DWORD));
	for(i=0;i<NC->LsizeSection;i++){
	N->section_cumu[i]=0;
	}*/
	N->req_w_cumu = 0;

	N->Cumu_Dirty_Sectors = 0;
	N->Effect_Merge = 0;

	//計算同SLB cyl上平均寫多少不同DB
	N->b_time = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	N->b = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i<NC->LsizeBlock; i++){ assert(N->b_time[i] == 0); assert(N->b[i] == 0); }
	N->time = 0;
	N->min_cyc = 0;
	N->max_cyc = 0;

	N->SlotTable_Band = (SLOTTABLE_BAND*)calloc(NC->LsizeBlock, sizeof(SLOTTABLE_BAND));
	for (i = 0; i < NC->LsizeBlock; i++){
		N->SlotTable_Band[i].Act = 0;
		N->SlotTable_Band[i].InAct = 0;
	}
	N->SlotTable_Act = 0; N->SlotTable_InAct = 0;
	N->Merged_Band = 0;
	//
	N->DirtySector = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
	for (i = 0; i < NC->LsizeSector; i++){ assert(N->DirtySector[i] == 0); }

	printf("%lu %lu\n", 23000 * (I64)SLB->SLBsection_num / 50331648, 34500 * (I64)SLB->SLBsection_num / 50331648);
	printf("%lu\n", 0xFFFFFFFF);

	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;
	N->BaseTime = 0;
}
#endif
#ifdef Simulation_NSLB
//void FASTinitNSLB(sim *simPtr) { // 將 FAST FTL 初始化
//	DWORD	i, j;
//	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//
//	printf("FASTinit CLB ...\n");
//	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
//	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
//	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
//	NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
//	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
//	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
//	printf("NC->pageSizeByte %I64u, NC->sectorSizeByte %I64u\n", NC->pageSizeByte, NC->sectorSizeByte);
//
//	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
//	NC->MAX_CUT = simGetDWORD(SC, "GEOMETRY", "MAXCUT");
//	NC->FREE_CUT = simGetDWORD(SC, "GEOMETRY", "FREECUT");
//	NC->WASTE_CUT = simGetDWORD(SC, "GEOMETRY", "WASTECUT");
//	NC->cut_def = simGetDWORD(SC, "GEOMETRY", "CUTDEF");
//	NC->delay_threshold = simGetDWORD(SC, "GEOMETRY", "DELAY_THRES");
//	NC->cut_afterwrite = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERWRITE"); //write request period
//	NC->cut_aftermerge = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERMERGE"); //merge log band period
//	assert(NC->cut_afterwrite ^ NC->cut_aftermerge > 0); //只能選一個 0|x xor x|0
//
//	//#define FreeList_byValid2
//	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
//	NC->Section = simGetDWORD(SC, "GEOMETRY", "SectionSizeSector");//SubTrack
//	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");//RamSizeTrack
//	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");//
//	NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");
//
//	printf("NC->PsizeByte: %I64u byte\n", NC->PsizeByte); //要扣掉data和guard track 
//	printf("NC->LsizeByte: %I64u byte\n", NC->LsizeByte);
//	NC->PsizeByte += NC->trackSizeByte * NC->FREE_CUT;
//	NC->PsizeByte -= NC->trackSizeByte * NC->WASTE_CUT;
//	assert(NC->LsizeByte%NC->blockSizeByte == 0);
//
//	NC->LsizeBlock = NC->LsizeByte / NC->blockSizeByte;
//	NC->PsizeBlock = NC->LsizeBlock + 1;
//	N->partial_band = NC->PsizeBlock - 1;
//
//	//計算LB的partial page
//	//N->partial_page = (unsigned int)(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock - NC->trackSizeByte) / NC->pageSizeByte - 1;//CLB sub guard track directly, not move it in LB
//	N->partial_page = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
//	N->Partial_Section = (N->partial_page + 1) / NC->Section - 1;
//
//	printf("NC->LsizeBlock: %lu\n", NC->LsizeBlock);
//	printf("NC->PsizeBlock: %lu\n", NC->PsizeBlock);
//	assert(NC->trackSizeByte - 1 <= N->partial_page);
//	printf("N->partial_band: %lu, N->partial_page:%lu\n", (DWORD)N->partial_band, N->partial_page);
//	printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
//	printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
//	printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);
//	assert(NC->trackSizeByte % NC->pageSizeByte == 0);
//	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
//	assert(NC->blockSizeByte % NC->pageSizeByte == 0);
//	assert(NC->pageSizeByte % NC->sectorSizeByte == 0);
//
//	NC->pageSizeSector = NC->pageSizeByte / NC->sectorSizeByte;
//	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
//	NC->blockSizePage = NC->blockSizeByte / NC->pageSizeByte;
//	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
//
//	NC->trackSizePage = NC->trackSizeByte / NC->pageSizeByte;//外加
//	NC->trackSizeSection = NC->trackSizePage / NC->Section;
//	NC->blockSizeTrack = (NC->blockSizePage / NC->trackSizePage);
//
//	NC->LsizeSector = (DWORD)(NC->LsizeByte / NC->sectorSizeByte);
//	NC->LsizePage = (DWORD)(NC->LsizeByte / NC->pageSizeByte);
//	NC->LsizeSection = NC->LsizeSector / NC->Section;
//
//	NC->PsizeSector = NC->PsizeByte / NC->pageSizeByte;  //(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
//	NC->PsizePage = NC->PsizeByte / NC->pageSizeByte; //(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
//	NC->PsizeSection = NC->PsizeSector / NC->Section;
//
//	printf("NC variables\n");
//	printf("NC->PsizePage: %lu\n", NC->PsizePage);
//	printf("NC->LsizePage: %lu\n", NC->LsizePage);
//	printf("NC->trackSizePage: %lu\n", NC->trackSizePage);
//	printf("NC->blockSizePage: %lu\n", NC->blockSizePage);
//	printf("%lu pages in data band, %lu pages in LB\n", NC->blockSizeByte / NC->pageSizeByte, N->partial_page + 1);
//
//	/*配置block*/
//	//N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
//	//N->blocks[0].pages = (flashPage *)calloc(NC->PsizePage, sizeof(flashPage)); assert(N->blocks[0].pages != NULL);//all phys pages(+guard band)
//	//for (i = 0; i < NC->PsizeBlock; i++) {
//	//	N->blocks[i].pages = N->blocks[0].pages + i*(NC->blockSizePage + NC->trackSizePage);//set of a data band and GT
//	//}
//	//init(simPtr);
//	//for (i = 0; i < NC->LsizeBlock; i++) {
//	//	//N->L2Ptable[i] = i;					// map LBA i to PBA i
//	//	N->blocks[i].Merge = 0;
//	//	for (j = 0; j < NC->blockSizePage; j++) {
//	//		N->blocks[i].pages[j].sector = i*NC->blockSizeSector + j*NC->pageSizeSector;	// map the sector # to page		
//	//		N->blocks[i].pages[j].valid = 1;
//	//	}
//	//}
//	/*for (i = 0; i < NC->LsizeBlock; i++){
//	for (j = 0; j < NC->blockSizePage; j += (NC->blockSizePage - 1)) {
//	assert(0 <= N->blocks[i].pages[j].sector && N->blocks[i].pages[j].sector < NC->LsizePage);
//	assert(N->blocks[i].pages[j].valid == 1);
//	}
//	}*/
//	//for (j = 0; j <= N->partial_page; j++){
//	//	//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
//	//	N->blocks[NC->PsizeBlock - 1].pages[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
//	//	N->blocks[NC->PsizeBlock - 1].pages[j].valid = 0;
//	//	assert(N->blocks[NC->PsizeBlock - 1].pages[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].pages[j].valid == 0);
//	//}
//	//printf("	log buffer pages: %I64u\n", N->partial_page);
//	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
//	N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
//	for (i = 0; i < NC->PsizeBlock; i++) {
//		N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
//	}
//	init(simPtr);//st table初始化
//	for (i = 0; i < NC->LsizeBlock; i++) {
//		N->blocks[i].Merge = 0;
//		for (j = 0; j < NC->blockSizeSection; j++) {
//			N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
//			N->blocks[i].sections[j].valid = 1;
//		}
//	}
//	/*for (j = 0; j < NC->blockSizeSection; j++) {
//	printf("%lu %lu, %lu %lu\n", N->blocks[0].sections[j].sector, N->blocks[0].sections[j].valid, N->blocks[1].sections[j].sector, N->blocks[1].sections[j].valid);
//	}
//	system("pause");*/
//	N->Total_Merge = 0;
//	N->PM_Merge = 0;
//	N->PMBTD_Merge = 0;
//	for (i = 0; i < NC->LsizeBlock; i++){
//		assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
//		assert(N->blocks[i].sections[0].valid == 1);
//	}
//	for (j = 0; j <= N->Partial_Section; j++){
//		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
//		N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
//		N->blocks[NC->PsizeBlock - 1].sections[j].valid = 0;
//		assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
//	}
//	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
//	/*結束配置block*/
//
//	//N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
//	//N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
//	//N->writedPageNum = 0;
//	//N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
//	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
//	//printf("FASTinit CLB OK\n");
//	//NS->mergecnt = 0;
//	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
//	//SLB->firstRWpage = 0;
//	//SLB->lastRWpage = 0;
//	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
//	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
//	//SLB->writedPageNum = 0;
//	//SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
//	//SLB->write_count = 0; SLB->merge_count = 0;
//	//printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);
//
//	NC->LsizeMB = NC->LsizeSector / 2048;//sector 512 Byte
//	NC->PsizeMB = NC->PsizeSector / 2048;//sector 512 Byte
//
//	N->firstRWsection = 0;
//	N->lastRWsection = 0;
//	N->writedSectionNum = 0;
//	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
//	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;
//
//	N->NSLB_tracks = 0;
//	if (NC->NSLBpercent >= 5){
//		DWORD five_percent_tracks = (N->partial_page + 1) / (NC->trackSizePage) / 10 / 2;
//		N->NSLB_tracks = NC->NSLBpercent / 5 * five_percent_tracks;
//	}
//	else if (0 <= NC->NSLBpercent && NC->NSLBpercent <= 4){
//		DWORD one_percent_tracks = (N->partial_page + 1) / (NC->trackSizePage) / 100;
//		N->NSLB_tracks = NC->NSLBpercent * one_percent_tracks;
//	}
//	else{ printf("NSLB percent error"); system("pause"); }
//	//N->NSLB_tracks = NC->NSLBpercent * ((N->partial_page + 1) / NC->trackSizePage) / 100; //NSLB給多，可能可以做
//	printf("NC->NSLBpercent = %lu, NSLB_tracks = %lu\n", NC->NSLBpercent, N->NSLB_tracks);
//	//初始化NSLBtoSLB的TD、初始化section(SubTrack)的大小
//	N->NSLBGCtoDB_Threshold = 32 * NC->trackSizePage;//N->HD_WRITE_Len = 16;//不使用
//
//	NS->mergecnt = 0;
//	SLB->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
//	SLB->firstRWsection = 0;
//	SLB->lastRWsection = 0;
//	SLB->Partial_Section = N->Partial_Section - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page;// 
//	SLB->SLBsection_num = N->Partial_Section + 1 - NC->trackSizeSection - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
//	SLB->writedSectionNum = 0;
//	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
//	SLB->write_count = 0; SLB->merge_count = 0;
//
//	//NSLB_tracks = 50 * ((N->partial_page + 1) / (NC->trackSizePage) / 100);
//	NSLB->firstRWsection = SLB->Partial_Section + 1;
//	NSLB->lastRWsection = SLB->Partial_Section + 1;
//	NSLB->NSLBsection_num = N->NSLB_tracks*NC->trackSizeSection;
//	NSLB->NSLBpage_num = N->NSLB_tracks*NC->trackSizePage;
//	NSLB->Partial_Section = SLB->Partial_Section + N->NSLB_tracks*NC->trackSizeSection;// N->partial_page;//old log buffer partial page == NSLB's partial page
//	NSLB->writedSectionNum = 0;
//	NSLB->pageWrite = 0; NSLB->pageRead = 0; NSLB->mergecnt = 0;
//	NSLB->write_count = 0; NSLB->merge_count = 0;
//	assert(NSLB->Partial_Section - NSLB->firstRWsection + 1 == N->NSLB_tracks*NC->trackSizeSection);
//	for (i = NSLB->firstRWsection; i <= NSLB->Partial_Section; i++){ assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0); }
//
//	N->BPLRU = (I64*)calloc(NC->LsizeBlock, sizeof(I64));
//	for (i = 0; i < NC->LsizeBlock; i++){ N->BPLRU[i] = 0; }
//	N->cnt = 0;
//	N->NSLBremainSectionTable = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
//	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLBremainSectionTable[i] = NC->trackSizeSection; }
//	N->NSLBexhaustedTrack = 0;
//	printf("N->NSLBremainSectionTable[0] %lu, %lu\n", N->NSLBremainSectionTable[0], NC->trackSizeSection);
//	/*printf("NSLB->firstRWpage = %lu, NSLB->lastRWpage = %lu, NSLB->NSLBpage_num = %lu, NSLB->partial_page = %lu\n", NSLB->firstRWpage, NSLB->lastRWpage, NSLB->NSLBpage_num, NSLB->partial_page);
//	printf("NSLB->writedPageNum = %lu\n", NSLB->writedPageNum);*/
//
//
//	//試驗
//	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
//	N->BurstLen_Start = 0; N->BurstLen_End = 0;
//
//	//試驗
//	N->ValidIs2 = 0;
//	N->ValidIs0 = 0;
//	N->SourceTracks_count = 0;
//
//	//試驗
//	N->SLB_Hit = 0; N->SLB_Miss = 0;
//	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
//	N->cycle_reqSectorWrite = 0;
//	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
//	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;
//
//#ifdef SpecificTrack
//	//NSLB試驗
//	N->NSLBusedTrack = 0;
//	/*N->NSLB_TrackIndex = (NSLB_SpecificTrack*)calloc(NC->LsizeBlock, sizeof(NSLB_SpecificTrack));
//	for (i = 0; i < NC->LsizeBlock; i++){
//	N->NSLB_TrackIndex[i].track_num = 0;
//	N->NSLB_TrackIndex[i].use_index = 0;
//	N->NSLB_TrackIndex[i].head = NULL;
//	}*/
//	/*N->NSLB_Band = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
//	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLB_Band[i] = -1; assert(N->NSLB_Band[i] == -1); }*/
//
//	////RAM結構
//	N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
//	N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//	for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
//	N->NSLB_RAM_request = 0;
//	N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
//	for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
//
//	//sector ram
//	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
//	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
//	//N->NSLB_RAM_request = 0;
//	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
//	//for (i = 0; i < NC->LsizeSector; i++){ assert(N->RAM_Valid[i] == 0); }
//	
//	//統計RAM Hit吸收的寫入
//	N->RAM_WriteOut = 0;
//	N->Host2RAM = 0;
//	//試驗
//	N->RAM_Hit = 0;
//	N->RAM_Miss = 0;
//#ifdef NSLB_RAM_BandLRU_HIT
//	N->RAM_BPLRU = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
//	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->RAM_BPLRU[i] == 0); }
//	N->RAM_BPLRU_Time = 0;
//#endif
//#endif
//#ifdef Time_Overhead
//	Init_TimeOverhead(simPtr);
//#endif
//
//	printf("SeekTime init OK\n");
//#ifdef BPLRULlist_RunLen
//	N->BPLRUtotal_count = 0;
//	N->BPLRUtotal_len = 0;
//	N->BPLRUcurrent_len = 0;
//	N->BPLRUcurrent_max = 0;
//	N->BPLRUcurrent_min = 9223372036854775807;
//#endif
//	printf("NEWGC64trackWRITE init OK\n");
//}

void FASTinitNSLB(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizePage / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte / 1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizePage / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack) - 1;
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	//printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	//printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	//printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);

	/*配置block*/
	//N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	//N->blocks[0].pages = (flashPage *)calloc(NC->PsizePage, sizeof(flashPage)); assert(N->blocks[0].pages != NULL);//all phys pages(+guard band)
	//for (i = 0; i < NC->PsizeBlock; i++) {
	//	N->blocks[i].pages = N->blocks[0].pages + i*(NC->blockSizePage + NC->trackSizePage);//set of a data band and GT
	//}
	//init(simPtr);
	//for (i = 0; i < NC->LsizeBlock; i++) {
	//	//N->L2Ptable[i] = i;					// map LBA i to PBA i
	//	N->blocks[i].Merge = 0;
	//	for (j = 0; j < NC->blockSizePage; j++) {
	//		N->blocks[i].pages[j].sector = i*NC->blockSizeSector + j*NC->pageSizeSector;	// map the sector # to page		
	//		N->blocks[i].pages[j].valid = 1;
	//	}
	//}
	/*for (i = 0; i < NC->LsizeBlock; i++){
	for (j = 0; j < NC->blockSizePage; j += (NC->blockSizePage - 1)) {
	assert(0 <= N->blocks[i].pages[j].sector && N->blocks[i].pages[j].sector < NC->LsizePage);
	assert(N->blocks[i].pages[j].valid == 1);
	}
	}*/
	//for (j = 0; j <= N->partial_page; j++){
	//	//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
	//	N->blocks[NC->PsizeBlock - 1].pages[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
	//	N->blocks[NC->PsizeBlock - 1].pages[j].valid = 0;
	//	assert(N->blocks[NC->PsizeBlock - 1].pages[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].pages[j].valid == 0);
	//}
	//printf("	log buffer pages: %I64u\n", N->partial_page);
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	for (i = 0; i < NC->PsizeBlock; i++) {
		N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	}
	init(simPtr);//st table初始化
	for (i = 0; i < NC->LsizeBlock; i++) {
		N->blocks[i].Merge = 0;
		for (j = 0; j < NC->blockSizeSection; j++) {
			N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
			N->blocks[i].sections[j].valid = 1;
		}
	}
	
	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	for (i = 0; i < NC->LsizeBlock; i++){
		assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
		assert(N->blocks[i].sections[0].valid == 1);
	}
	for (j = 0; j <= N->Partial_Section; j++){
		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
		N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
		N->blocks[NC->PsizeBlock - 1].sections[j].valid = 0;
		assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
	}
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	NC->LsizeMB = NC->LsizeSector / 2048;//sector 512 Byte
	NC->PsizeMB = NC->PsizeSector / 2048;//sector 512 Byte

	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection; 

	assert(0 <= NC->NSLBpercent && NC->NSLBpercent <= 50);
	N->NSLB_tracks = NC->NSLBpercent * (N->partial_page + 1) / (NC->trackSizePage) / 100;
	printf("NC->NSLBpercent = %lu, NSLB_tracks = %lu\n", NC->NSLBpercent, N->NSLB_tracks);
	//初始化NSLBtoSLB的TD、初始化section(SubTrack)的大小
	N->NSLBGCtoDB_Threshold = 32 * NC->trackSizePage;//N->HD_WRITE_Len = 16;//不使用

	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
	
	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;

	NSLB->firstRWsection = SLB->Partial_Section + 1;
	NSLB->lastRWsection = SLB->Partial_Section + 1;
	NSLB->NSLBsection_num = N->NSLB_tracks*NC->trackSizeSection;
	NSLB->NSLBpage_num = N->NSLB_tracks*NC->trackSizePage;
	NSLB->Partial_Section = SLB->Partial_Section + N->NSLB_tracks*NC->trackSizeSection;// N->partial_page;//old log buffer partial page == NSLB's partial page
	NSLB->writedSectionNum = 0;
	NSLB->pageWrite = 0; NSLB->pageRead = 0; NSLB->mergecnt = 0;
	NSLB->write_count = 0; NSLB->merge_count = 0;
	assert(NSLB->Partial_Section - NSLB->firstRWsection + 1 == N->NSLB_tracks*NC->trackSizeSection);
	for (i = NSLB->firstRWsection; i <= NSLB->Partial_Section; i++){ assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0); }

	printf("NSLB->NSLBsection_num %lu\n", NSLB->NSLBsection_num); 

	N->BPLRU = (I64*)calloc(NC->LsizeBlock, sizeof(I64));
	for (i = 0; i < NC->LsizeBlock; i++){ N->BPLRU[i] = 0; }
	N->cnt = 0;
	N->NSLBremainSectionTable = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLBremainSectionTable[i] = NC->trackSizeSection; }
	N->NSLBexhaustedTrack = 0;
	printf("N->NSLBremainSectionTable[0] %lu, %lu\n", N->NSLBremainSectionTable[0], NC->trackSizeSection);
	/*printf("NSLB->firstRWpage = %lu, NSLB->lastRWpage = %lu, NSLB->NSLBpage_num = %lu, NSLB->partial_page = %lu\n", NSLB->firstRWpage, NSLB->lastRWpage, NSLB->NSLBpage_num, NSLB->partial_page);
	printf("NSLB->writedPageNum = %lu\n", NSLB->writedPageNum);*/

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;
	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;
	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	printf("SpecificTrack init\n");
	printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;

	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;

	N->DirtySector = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
	for (i = 0; i < NC->LsizeSector; i++){ assert(N->DirtySector[i] == 0); }
	N->NSLBwriteout_sector = 0;

	/*N->blocks[0].sections[0].valid = 0;
	printf("%lu ", N->blocks[0].sections[0].valid);
	N->blocks[0].sections[0].valid = 1;
	printf("%lu ", N->blocks[0].sections[0].valid);
	N->blocks[0].sections[0].valid = 2;
	printf("%lu\n", N->blocks[0].sections[0].valid);
	system("pause");*/
}
void FASTinitNSLB2(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizePage / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte / 1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizePage / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack) - 1;
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	
	/*配置block*/
	N->blocks = (flashBlock *)calloc(NC->LsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	N->blocks[0].sections = (flashPage *)calloc(NC->LsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	for (i = 0; i < NC->LsizeBlock; i++) {
		N->blocks[i].sections = N->blocks[0].sections + i*NC->blockSizeSection;// i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	}
	//init(simPtr);//st table初始化
	for (i = 0; i < NC->LsizeBlock; i++) {
		N->blocks[i].Merge = 0;
		for (j = 0; j < NC->blockSizeSection; j++) {
			N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
			N->blocks[i].sections[j].valid = 1;
		}
	}

	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	for (i = 0; i < NC->LsizeBlock; i++){
		assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
		assert(N->blocks[i].sections[0].valid == 1);
	}
	//for (j = 0; j <= N->Partial_Section; j++){
	//	//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
	//	N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
	//	N->blocks[NC->PsizeBlock - 1].sections[j].valid = 0;
	//	assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
	//}
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	NC->LsizeMB = NC->LsizeSector / 2048;//sector 512 Byte
	NC->PsizeMB = NC->PsizeSector / 2048;//sector 512 Byte

	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;

	assert(0 <= NC->NSLBpercent && NC->NSLBpercent <= 50);
	N->NSLB_tracks = NC->NSLBpercent * (N->partial_page + 1) / (NC->trackSizePage) / 100;
	printf("NC->NSLBpercent = %lu, NSLB_tracks = %lu\n", NC->NSLBpercent, N->NSLB_tracks);
	//初始化NSLBtoSLB的TD、初始化section(SubTrack)的大小
	N->NSLBGCtoDB_Threshold = 32 * NC->trackSizePage;//N->HD_WRITE_Len = 16;//不使用

	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;

	NSLB->firstRWsection = SLB->Partial_Section + 1;
	NSLB->lastRWsection = SLB->Partial_Section + 1;
	NSLB->NSLBsection_num = N->NSLB_tracks*NC->trackSizeSection;
	NSLB->NSLBpage_num = N->NSLB_tracks*NC->trackSizePage;
	NSLB->Partial_Section = SLB->Partial_Section + N->NSLB_tracks*NC->trackSizeSection;// N->partial_page;//old log buffer partial page == NSLB's partial page
	NSLB->writedSectionNum = 0;
	NSLB->pageWrite = 0; NSLB->pageRead = 0; NSLB->mergecnt = 0;
	NSLB->write_count = 0; NSLB->merge_count = 0;
	assert(NSLB->Partial_Section - NSLB->firstRWsection + 1 == N->NSLB_tracks*NC->trackSizeSection);
	//for (i = NSLB->firstRWsection; i <= NSLB->Partial_Section; i++){ assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0); }

	printf("NSLB->NSLBsection_num %lu\n", NSLB->NSLBsection_num);

	N->BPLRU = (I64*)calloc(NC->LsizeBlock, sizeof(I64));
	for (i = 0; i < NC->LsizeBlock; i++){ N->BPLRU[i] = 0; }
	N->cnt = 0;
	N->NSLBremainSectionTable = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLBremainSectionTable[i] = NC->trackSizeSection; }
	N->NSLBexhaustedTrack = 0;
	printf("N->NSLBremainSectionTable[0] %lu, %lu\n", N->NSLBremainSectionTable[0], NC->trackSizeSection);
	/*printf("NSLB->firstRWpage = %lu, NSLB->lastRWpage = %lu, NSLB->NSLBpage_num = %lu, NSLB->partial_page = %lu\n", NSLB->firstRWpage, NSLB->lastRWpage, NSLB->NSLBpage_num, NSLB->partial_page);
	printf("NSLB->writedPageNum = %lu\n", NSLB->writedPageNum);*/

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;
	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;
	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	printf("SpecificTrack init\n");
	printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;

	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;

	N->DirtySector = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
	for (i = 0; i < NC->LsizeSector; i++){ assert(N->DirtySector[i] == 0); }
	N->NSLBwriteout_sector = 0;
#ifdef NSLB_LocalUse
	N->DB2curNSLB_TK = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ N->DB2curNSLB_TK[i] = 2*N->NSLB_tracks; }
#endif
	printf("Init Ok\n");
}
void FASTinitNSLB3(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizePage / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte / 1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizePage / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack) - 1;
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);

	/*配置block*/
	N->blocks = (flashBlock *)calloc(NC->LsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	N->blocks[0].sections = (flashPage *)calloc(NC->LsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	for (i = 0; i < NC->LsizeBlock; i++) {
		N->blocks[i].sections = N->blocks[0].sections + i*NC->blockSizeSection;// i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	}
	//init(simPtr);//st table初始化
	for (i = 0; i < NC->LsizeBlock; i++) {
		N->blocks[i].Merge = 0;
		for (j = 0; j < NC->blockSizeSection; j++) {
			N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
			N->blocks[i].sections[j].valid = 1;
		}
	}

	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	for (i = 0; i < NC->LsizeBlock; i++){
		assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
		assert(N->blocks[i].sections[0].valid == 1);
	}
	//for (j = 0; j <= N->Partial_Section; j++){
	//	//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
	//	N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
	//	N->blocks[NC->PsizeBlock - 1].sections[j].valid = 0;
	//	assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
	//}
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	NC->LsizeMB = NC->LsizeSector / 2048;//sector 512 Byte
	NC->PsizeMB = NC->PsizeSector / 2048;//sector 512 Byte

	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;

	assert(0 <= NC->NSLBpercent && NC->NSLBpercent <= 50);
	N->NSLB_tracks = NC->NSLBpercent * (N->partial_page + 1) / (NC->trackSizePage) / 100;
	printf("NC->NSLBpercent = %lu, NSLB_tracks = %lu\n", NC->NSLBpercent, N->NSLB_tracks);
	//初始化NSLBtoSLB的TD、初始化section(SubTrack)的大小
	N->NSLBGCtoDB_Threshold = 32 * NC->trackSizePage;//N->HD_WRITE_Len = 16;//不使用

	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;

	NSLB->firstRWsection = SLB->Partial_Section + 1;
	NSLB->lastRWsection = SLB->Partial_Section + 1;
	NSLB->NSLBsection_num = N->NSLB_tracks*NC->trackSizeSection;
	NSLB->NSLBpage_num = N->NSLB_tracks*NC->trackSizePage;
	NSLB->Partial_Section = SLB->Partial_Section + N->NSLB_tracks*NC->trackSizeSection;// N->partial_page;//old log buffer partial page == NSLB's partial page
	NSLB->writedSectionNum = 0;
	NSLB->pageWrite = 0; NSLB->pageRead = 0; NSLB->mergecnt = 0;
	NSLB->write_count = 0; NSLB->merge_count = 0;
	assert(NSLB->Partial_Section - NSLB->firstRWsection + 1 == N->NSLB_tracks*NC->trackSizeSection);
	//for (i = NSLB->firstRWsection; i <= NSLB->Partial_Section; i++){ assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0); }

	printf("NSLB->NSLBsection_num %lu\n", NSLB->NSLBsection_num);

	N->BPLRU = (I64*)calloc(NC->LsizeBlock, sizeof(I64));
	for (i = 0; i < NC->LsizeBlock; i++){ N->BPLRU[i] = 0; }
	N->cnt = 0;
	N->NSLBremainSectionTable = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLBremainSectionTable[i] = NC->trackSizeSection; }
	N->NSLBexhaustedTrack = 0;
	printf("N->NSLBremainSectionTable[0] %lu, %lu\n", N->NSLBremainSectionTable[0], NC->trackSizeSection);
	/*printf("NSLB->firstRWpage = %lu, NSLB->lastRWpage = %lu, NSLB->NSLBpage_num = %lu, NSLB->partial_page = %lu\n", NSLB->firstRWpage, NSLB->lastRWpage, NSLB->NSLBpage_num, NSLB->partial_page);
	printf("NSLB->writedPageNum = %lu\n", NSLB->writedPageNum);*/

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;
	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;
	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	printf("SpecificTrack init\n");
	printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;

	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;

	N->DirtySector = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
	for (i = 0; i < NC->LsizeSector; i++){ assert(N->DirtySector[i] == 0); }
	N->NSLBwriteout_sector = 0;
#ifdef NSLB_LocalUse
	N->DB2curNSLB_TK = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ N->DB2curNSLB_TK[i] = 2 * N->NSLB_tracks; }
	//
	N->ExcluNSLBTK = (DWORD**)malloc(N->NSLB_tracks*sizeof(DWORD*));
	N->NSLBTK_Share = (DWORD**)malloc(N->NSLB_tracks*sizeof(DWORD*));
	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLBTK_Share[i] = 0; }
#endif
	printf("Init Ok\n");
}
//------

void FASTinitNSLB_NoDB(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizePage / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte / 1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizePage / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack) - 1;
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	//printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	//printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	//printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);

	/*配置block*/
	//N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	//N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	//for (i = 0; i < NC->PsizeBlock; i++) {
	//	N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	//}
	//init(simPtr);//st table初始化
	//for (i = 0; i < NC->LsizeBlock; i++) {
	//	N->blocks[i].Merge = 0;
	//	for (j = 0; j < NC->blockSizeSection; j++) {
	//		N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
	//		N->blocks[i].sections[j].valid = 1;
	//	}
	//}
	//N->Total_Merge = 0;
	//N->PM_Merge = 0;
	//N->PMBTD_Merge = 0;
	//for (i = 0; i < NC->LsizeBlock; i++){
	//	assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
	//	assert(N->blocks[i].sections[0].valid == 1);
	//}
	//for (j = 0; j <= N->Partial_Section; j++){
	//	//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
	//	N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
	//	N->blocks[NC->PsizeBlock - 1].sections[j].valid = 0;
	//	assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
	//}
	//printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	///*結束配置block*/

	/*配置block*/
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	for (i = 0; i < NC->LsizeBlock; i++){
		N->blocks[i].sections = NULL;
	}
	//N->blocks[NC->LsizeBlock].sections = (flashPage *)calloc(N->LB_LastSector + 1, sizeof(flashPage));
	//assert(N->blocks[NC->LsizeBlock].sections != NULL);
	//for (j = 0; j <= N->LB_LastSector; j++){
	//	//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
	//	N->blocks[NC->LsizeBlock].sections[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
	//	N->blocks[NC->LsizeBlock].sections[j].valid = 0;
	//	assert(N->blocks[NC->LsizeBlock].sections[j].sector == 0x3fffffff); assert(N->blocks[NC->LsizeBlock].sections[j].valid == 0);
	//}
	N->DBalloc = (BYTE *)calloc(NC->LsizeBlock, sizeof(BYTE)); assert(N->DBalloc != NULL);
	for (i = 0; i < NC->LsizeBlock; i++){ N->DBalloc[i] = 0; }
	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	NC->LsizeMB = NC->LsizeSector / 2048;//sector 512 Byte
	NC->PsizeMB = NC->PsizeSector / 2048;//sector 512 Byte

	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection; 

	assert(0 <= NC->NSLBpercent && NC->NSLBpercent <= 50);
	N->NSLB_tracks = NC->NSLBpercent * (N->partial_page + 1) / (NC->trackSizePage) / 100;
	printf("NC->NSLBpercent = %lu, NSLB_tracks = %lu\n", NC->NSLBpercent, N->NSLB_tracks);
	//初始化NSLBtoSLB的TD、初始化section(SubTrack)的大小
	N->NSLBGCtoDB_Threshold = 32 * NC->trackSizePage;//N->HD_WRITE_Len = 16;//不使用

	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection - 2 * N->NSLB_tracks*NC->trackSizeSection; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;

	NSLB->firstRWsection = SLB->Partial_Section + 1;
	NSLB->lastRWsection = SLB->Partial_Section + 1;
	NSLB->NSLBsection_num = N->NSLB_tracks*NC->trackSizeSection;
	NSLB->NSLBpage_num = N->NSLB_tracks*NC->trackSizePage;
	NSLB->Partial_Section = SLB->Partial_Section + N->NSLB_tracks*NC->trackSizeSection;// N->partial_page;//old log buffer partial page == NSLB's partial page
	NSLB->writedSectionNum = 0;
	NSLB->pageWrite = 0; NSLB->pageRead = 0; NSLB->mergecnt = 0;
	NSLB->write_count = 0; NSLB->merge_count = 0;
	assert(NSLB->Partial_Section - NSLB->firstRWsection + 1 == N->NSLB_tracks*NC->trackSizeSection);
	//for (i = NSLB->firstRWsection; i <= NSLB->Partial_Section; i++){ assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0); }

	printf("NSLB->NSLBsection_num %lu\n", NSLB->NSLBsection_num); 

	N->BPLRU = (I64*)calloc(NC->LsizeBlock, sizeof(I64));
	for (i = 0; i < NC->LsizeBlock; i++){ N->BPLRU[i] = 0; }
	N->cnt = 0;
	N->NSLBremainSectionTable = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLBremainSectionTable[i] = NC->trackSizeSection; }
	N->NSLBexhaustedTrack = 0;
	printf("N->NSLBremainSectionTable[0] %lu, %lu\n", N->NSLBremainSectionTable[0], NC->trackSizeSection);
	/*printf("NSLB->firstRWpage = %lu, NSLB->lastRWpage = %lu, NSLB->NSLBpage_num = %lu, NSLB->partial_page = %lu\n", NSLB->firstRWpage, NSLB->lastRWpage, NSLB->NSLBpage_num, NSLB->partial_page);
	printf("NSLB->writedPageNum = %lu\n", NSLB->writedPageNum);*/

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;
	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;
	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	printf("SpecificTrack init\n");
	printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;
	N->l2pmapOvd.max_AA = 0;

	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;
	N->BaseTime = 0;

	N->DirtySector = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
	for (i = 0; i < NC->LsizeSector; i++){ assert(N->DirtySector[i] == 0); }
	N->NSLBwriteout_sector = 0;

	/*N->blocks[0].sections = (flashPage *)calloc(NC->blockSizeSector, sizeof(flashPage));
	N->blocks[0].sections[0].valid = 0;
	printf("%lu ", N->blocks[0].sections[0].valid);
	N->blocks[0].sections[0].valid = 1;
	printf("%lu ", N->blocks[0].sections[0].valid);
	N->blocks[0].sections[0].valid = 2;
	printf("%lu ", N->blocks[0].sections[0].valid);*/
	printf("Init ok\n");
	//free(N->blocks[0].sections); system("pause");
}
#endif
#ifdef Simulation_HLB
void FASTinitHLB(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	//NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "Section");
	NC->CylinderSizeTrack = simGetDWORD(SC, "GEOMETRY", "CylinderSizeTrack");
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	//不用
	//NC->MAX_CUT = simGetDWORD(SC, "GEOMETRY", "MAXCUT");
	//NC->FREE_CUT = simGetDWORD(SC, "GEOMETRY", "FREECUT");
	//NC->WASTE_CUT = simGetDWORD(SC, "GEOMETRY", "WASTECUT");
	//NC->cut_def = simGetDWORD(SC, "GEOMETRY", "CUTDEF");
	//NC->delay_threshold = simGetDWORD(SC, "GEOMETRY", "DELAY_THRES");
	//NC->cut_afterwrite = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERWRITE"); //write request period
	//NC->cut_aftermerge = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERMERGE"); //merge log band period
	//assert(NC->cut_afterwrite ^ NC->cut_aftermerge > 0); //只能選一個 0|x xor x|0
	//NC->PsizeByte += NC->trackSizeByte * NC->FREE_CUT;
	//NC->PsizeByte -= NC->trackSizeByte * NC->WASTE_CUT;

	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->sectorSizeByte == 0);
	assert(NC->LsizeByte % NC->blockSizeByte == 0);
	//track
	NC->trackSizeSector = NC->trackSizeByte / NC->sectorSizeByte;
	NC->trackSizePage = NC->trackSizeSector;//之後砍掉
	NC->trackSizeSection = NC->trackSizeSector / NC->Section;
	//block
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeSector;//之後砍掉
	NC->blockSizeSection = NC->blockSizeSector / NC->Section;
	NC->blockSizeTrack = NC->blockSizeSection / NC->trackSizeSection;
	NC->blockSizeCylinder = NC->blockSizeTrack / NC->CylinderSizeTrack;
	//logical
	NC->LsizeSector = NC->LsizeByte / NC->sectorSizeByte;
	NC->LsizePage = NC->LsizeSector;//之後砍掉
	NC->LsizeSection = NC->LsizeSector / NC->Section;
	NC->LsizeTrack = NC->LsizeSection / NC->trackSizeSection;
	NC->LsizeBlock = NC->LsizeSection / NC->blockSizeSection;
	NC->LsizeCylinder = NC->LsizeTrack / NC->CylinderSizeTrack;
	NC->LsizeMB = NC->LsizeByte / 1024 / 1024;
	//physical
	NC->PsizeSector = NC->PsizeByte / NC->sectorSizeByte;//(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeSector;//之後砍掉
	NC->PsizeSection = NC->PsizeSector / NC->Section;;
	NC->PsizeTrack = NC->PsizeSection / NC->trackSizeSection;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	NC->PsizeCylinder = NC->PsizeTrack / NC->CylinderSizeTrack;;
	NC->PsizeMB = NC->PsizeByte / 1024 / 1024;
	//Log Buffer Initial
	N->LB_Block = NC->PsizeBlock - 1;
	//N->LB_LastSector = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	//N->LB_LastSection = (N->partial_page + 1) / NC->Section - 1;
	//printf("%lu %lu %lu %lu \n", NC->PsizeSector, NC->LsizeSector, NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack, NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);
	assert(NC->PsizeSector > NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack);

	N->LB_LastSector = NC->PsizeSector - (NC->LsizeSector + NC->LsizeBlock*NC->trackSizeSector*NC->CylinderSizeTrack) - 1;//最後一個
	N->LB_LastSection = (N->LB_LastSector + 1) / NC->Section - 1;//最後一個
	N->LB_Track = (N->LB_LastSection + 1) / NC->trackSizeSection;//有幾個
	N->LB_Cylinder = N->LB_Track / NC->CylinderSizeTrack;//有幾個
	//不用
	N->partial_band = N->LB_Block;//之後砍掉
	N->partial_page = N->LB_LastSector;//之後砍掉
	N->Partial_Section = N->LB_LastSection;//之後砍掉

	printf("NC variables\n");
	printf("blockSizeByte %lu, trackSizeByte %lu, sectorSizeByte %lu\n", NC->blockSizeByte, NC->trackSizeByte, NC->sectorSizeByte);
	printf("Section = %lu sector\n", NC->Section);
	printf("track: %lu sector, %lu section\n", NC->trackSizeSector, NC->trackSizeSection);
	printf("block: %lu sector, %lu section, %lu track, %lu cylinder\n", NC->blockSizeSector, NC->blockSizeSection, NC->blockSizeTrack, NC->blockSizeCylinder);

	printf("Logical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->LsizeByte, NC->LsizeSector, NC->LsizeSection, NC->LsizeTrack, NC->LsizeBlock, NC->LsizeCylinder, NC->LsizeMB);
	printf("Pogical: %I64u Byte, %lu sector, %lu section, %lu track, %lu block, %lu cylinder %lu MB\n", NC->PsizeByte, NC->PsizeSector, NC->PsizeSection, NC->PsizeTrack, NC->PsizeBlock, NC->PsizeCylinder, NC->PsizeMB);
	printf("CylinderSizeTrack = %lu\n", NC->CylinderSizeTrack);

	printf("LB: Block[%lu], %lu sector, %lu section, %lu track, %lu cylinder\n", N->LB_Block, N->LB_LastSector + 1, N->LB_LastSection + 1, N->LB_Track, N->LB_Cylinder);
	//printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	//printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	//printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);

	/*配置block*/
	//N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	//N->blocks[0].pages = (flashPage *)calloc(NC->PsizePage, sizeof(flashPage)); assert(N->blocks[0].pages != NULL);//all phys pages(+guard band)
	//for (i = 0; i < NC->PsizeBlock; i++) {
	//	N->blocks[i].pages = N->blocks[0].pages + i*(NC->blockSizePage + NC->trackSizePage);//set of a data band and GT
	//}
	//init(simPtr);
	//for (i = 0; i < NC->LsizeBlock; i++) {
	//	//N->L2Ptable[i] = i;					// map LBA i to PBA i
	//	N->blocks[i].Merge = 0;
	//	for (j = 0; j < NC->blockSizePage; j++) {
	//		N->blocks[i].pages[j].sector = i*NC->blockSizeSector + j*NC->pageSizeSector;	// map the sector # to page		
	//		N->blocks[i].pages[j].valid = 1;
	//	}
	//}
	/*for (i = 0; i < NC->LsizeBlock; i++){
	for (j = 0; j < NC->blockSizePage; j += (NC->blockSizePage - 1)) {
	assert(0 <= N->blocks[i].pages[j].sector && N->blocks[i].pages[j].sector < NC->LsizePage);
	assert(N->blocks[i].pages[j].valid == 1);
	}
	}*/
	//for (j = 0; j <= N->partial_page; j++){
	//	//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
	//	N->blocks[NC->PsizeBlock - 1].pages[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
	//	N->blocks[NC->PsizeBlock - 1].pages[j].valid = 0;
	//	assert(N->blocks[NC->PsizeBlock - 1].pages[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].pages[j].valid == 0);
	//}
	//printf("	log buffer pages: %I64u\n", N->partial_page);
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	N->blocks[0].sections = (flashPage *)calloc(NC->PsizeSection, sizeof(flashPage)); assert(N->blocks[0].sections != NULL);//all phys pages(+guard band)
	for (i = 0; i < NC->PsizeBlock; i++) {
		N->blocks[i].sections = N->blocks[0].sections + i*(NC->blockSizeSection + NC->trackSizeSection);//set of a data band and GT
	}
	init(simPtr);//st table初始化
	for (i = 0; i < NC->LsizeBlock; i++) {
		N->blocks[i].Merge = 0;
		for (j = 0; j < NC->blockSizeSection; j++) {
			N->blocks[i].sections[j].sector = i*NC->blockSizeSector + j*NC->Section;	// map the sector # to page		
			N->blocks[i].sections[j].valid = 1;
			//N->blocks[i].sections[j].unique = 0;
		}
	}
	/*for (j = 0; j < NC->blockSizeSection; j++) {
	printf("%lu %lu, %lu %lu\n", N->blocks[0].sections[j].sector, N->blocks[0].sections[j].valid, N->blocks[1].sections[j].sector, N->blocks[1].sections[j].valid);
	}
	system("pause");*/
	N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;
	for (i = 0; i < NC->LsizeBlock; i++){
		assert(0 <= N->blocks[i].sections[0].sector && N->blocks[i].sections[0].sector < NC->LsizePage);
		assert(N->blocks[i].sections[0].valid == 1);
	}
	for (j = 0; j <= N->Partial_Section; j++){
		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
		N->blocks[NC->PsizeBlock - 1].sections[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
		N->blocks[NC->PsizeBlock - 1].sections[j].valid = 0;
		assert(N->blocks[NC->PsizeBlock - 1].sections[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].sections[j].valid == 0);
	}
	printf("	log buffer pages: %lu, log buffer sections %lu\n", N->partial_page, N->Partial_Section);
	/*結束配置block*/

	//N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
	//N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
	//N->writedPageNum = 0;
	//N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	//printf("FASTinit CLB OK\n");
	//NS->mergecnt = 0;
	//SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	//SLB->firstRWpage = 0;
	//SLB->lastRWpage = 0;
	//SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
	//SLB->writedPageNum = 0;
	//SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	//SLB->write_count = 0; SLB->merge_count = 0;
	//printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);


	N->firstRWsection = 0;
	N->lastRWsection = 0;
	N->writedSectionNum = 0;
	N->guardTrack1stSection = N->firstRWsection + NC->trackSizeSection;
	N->LBsection_num = N->Partial_Section + 1 - NC->trackSizeSection;

	//
	assert(0 <= NC->NSLBpercent && NC->NSLBpercent <= 50);
	N->NSLB_tracks = NC->NSLBpercent * (N->partial_page + 1) / (NC->trackSizePage) / 100;
	if (N->NSLB_tracks % NC->CylinderSizeTrack != 0){ N->NSLB_tracks = N->NSLB_tracks / NC->CylinderSizeTrack * NC->CylinderSizeTrack + NC->CylinderSizeTrack; }
	printf("NC->NSLBpercent = %lu, NSLB_tracks = %lu\n", NC->NSLBpercent, N->NSLB_tracks);

	NS->mergecnt = 0;
	SLB->guardTrack1stSection = N->firstRWsection + NC->CylinderSizeTrack*NC->trackSizeSection; //N->firstRWsection + NC->trackSizeSection;
	SLB->firstRWsection = 0;
	SLB->lastRWsection = 0;
	SLB->Partial_Section = N->Partial_Section - 2*N->NSLB_tracks*NC->trackSizeSector; //N->partial_page;// 
	SLB->SLBsection_num = N->Partial_Section + 1 - NC->CylinderSizeTrack*NC->trackSizeSection - 2*N->NSLB_tracks*NC->trackSizeSector; //N->partial_page + 1 - NC->trackSizePage;//
	SLB->writedSectionNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;
	printf("SLB Partial_Section %lu\n", SLB->Partial_Section);

	NSLB->firstRWsection = SLB->Partial_Section + 1;
	NSLB->lastRWsection = SLB->Partial_Section + 1;
	NSLB->NSLBsection_num = N->NSLB_tracks*NC->trackSizeSection;
	NSLB->NSLBpage_num = N->NSLB_tracks*NC->trackSizePage;
	NSLB->Partial_Section = SLB->Partial_Section + N->NSLB_tracks*NC->trackSizeSection;// N->partial_page;//old log buffer partial page == NSLB's partial page
	NSLB->writedSectionNum = 0;
	NSLB->pageWrite = 0; NSLB->pageRead = 0; NSLB->mergecnt = 0;
	NSLB->write_count = 0; NSLB->merge_count = 0;
	assert(NSLB->Partial_Section - NSLB->firstRWsection + 1 == N->NSLB_tracks*NC->trackSizeSection);
	for (i = NSLB->firstRWsection; i <= NSLB->Partial_Section; i++){ assert(N->blocks[NC->PsizeBlock - 1].sections[i].valid == 0); }
	printf("NSLB->NSLBsection_num %lu\n", NSLB->NSLBsection_num);
	N->BPLRU = (I64*)calloc(NC->LsizeBlock, sizeof(I64));
	for (i = 0; i < NC->LsizeBlock; i++){ N->BPLRU[i] = 0; }
	N->cnt = 0;
	N->NSLBremainSectionTable = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLBremainSectionTable[i] = NC->trackSizeSection; }
	N->NSLBexhaustedTrack = 0;
	printf("N->NSLBremainSectionTable[0] %lu, %lu\n", N->NSLBremainSectionTable[0], NC->trackSizeSection);
	/*printf("NSLB->firstRWpage = %lu, NSLB->lastRWpage = %lu, NSLB->NSLBpage_num = %lu, NSLB->partial_page = %lu\n", NSLB->firstRWpage, NSLB->lastRWpage, NSLB->NSLBpage_num, NSLB->partial_page);
	printf("NSLB->writedPageNum = %lu\n", NSLB->writedPageNum);*/


	//試驗
	N->SLB_Hit = 0; N->SLB_Miss = 0;
	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
	N->cycle_reqSectorWrite = 0;
	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;

	//試驗
	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
	N->BurstLen_Start = 0; N->BurstLen_End = 0;

	//試驗
	N->ValidIs2 = 0;
	N->ValidIs0 = 0;
	N->SourceTracks_count = 0;

#ifdef SpecificTrack
	////NSLB試驗
	//N->NSLBusedTrack = 0;
	////RAM結構
	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	//N->NSLB_RAM_request = 0;
	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
	//for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
	////統計RAM Hit吸收的寫入
	//N->RAM_WriteOut = 0; N->Host2RAM = 0;
	////試驗
	//N->RAM_Hit = 0; N->RAM_Miss = 0;
	//printf("SpecificTrack init\n");
	//printf("NSLB_RAM_SIZE %lu\n", N->NSLB_RAM_SIZE);
#endif
#ifdef Time_Overhead
	Init_TimeOverhead(simPtr);
	N->S_MRS = 0;
	N->S_CT = 0;
#endif
	//N->l2pmapOvd.LB_Live_Rate = 0;
	//N->l2pmapOvd.round_LB_Live_Section = 0;

	N->l2pmapOvd.AA_Rate = 0;
	N->l2pmapOvd.AA = 0;

	if (NC->Section == 1){ assert(NC->LsizeSection == NC->LsizeSector); }
	/*N->section_cumu = (DWORD*)calloc(NC->LsizeSection, sizeof(DWORD));
	for(i=0;i<NC->LsizeSection;i++){
	N->section_cumu[i]=0;
	}*/
	N->req_w_cumu = 0;

	N->Cumu_Dirty_Sectors = 0;
	N->Effect_Merge = 0;

	//計算同SLB cyl上平均寫多少不同DB
	N->b_time = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	N->b = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i<NC->LsizeBlock; i++){ assert(N->b_time[i] == 0); assert(N->b[i] == 0); }
	N->time = 0;
	N->min_cyc = 0;
	N->max_cyc = 0;


	N->SlotTable_Band = (SLOTTABLE_BAND*)calloc(NC->LsizeBlock, sizeof(SLOTTABLE_BAND));
	for (i = 0; i < NC->LsizeBlock; i++){
		N->SlotTable_Band[i].Act = 0;
		N->SlotTable_Band[i].InAct = 0;
	}
	N->SlotTable_Act = 0; N->SlotTable_InAct = 0;
	N->Merged_Band = 0;

	printf("%lu %lu\n", 23000 * (I64)SLB->SLBsection_num / 50331648, 34500 * (I64)SLB->SLBsection_num / 50331648);
	printf("%lu\n", 0xFFFFFFFF);

	N->Resp_Start = 1;
	N->Arr_Time = 0;
	N->Finish_Time = 0;
	N->RespTIme = 0;
	N->WaitingTime = 0;
	N->RunTime = 0;
#ifdef Simulation_HLB
	N->N2S_LBsector = (DWORD*)calloc(NC->blockSizeSector, sizeof(DWORD));
	N->N2S_LBsector_len = 0;
#endif
}
#endif
#ifdef SLBNSLB_Tmerge
//void FASTinitSLBNSLB(sim *simPtr) { // 將 FAST FTL 初始化
//	DWORD	i, j;
//	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
//
//	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
//	//NFTL_SLB *SLB2 = &((simPtr->NFTLobj).SLB2);
//	//DWORD n = (DWORD)1216348159;// 1163919359;// 1048575999;// 1024 * 1024 * 64;
//	//printf("n = %lu=n", n); system("pause");
//	/*BYTE a = 0;
//	printf("%lu ", a);
//	a = 2;
//	printf("%lu ", a);
//	a = 255;
//	printf("%lu ", a);
//	a = 256;
//	printf("%lu ", a); 
//	a = 257;
//	printf("%lu ", a); 
//	a = 258;
//	printf("%lu ", a); system("pause");*/
//
//	printf("FASTinit CLB ...\n");
//	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
//	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
//	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
//	NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
//	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
//	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
//	printf("NC->pageSizeByte %I64u, NC->sectorSizeByte %I64u\n", NC->pageSizeByte, NC->sectorSizeByte);
//
//	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
//	NC->MAX_CUT = simGetDWORD(SC, "GEOMETRY", "MAXCUT");
//	NC->FREE_CUT = simGetDWORD(SC, "GEOMETRY", "FREECUT");
//	NC->WASTE_CUT = simGetDWORD(SC, "GEOMETRY", "WASTECUT");
//	NC->cut_def = simGetDWORD(SC, "GEOMETRY", "CUTDEF");
//	NC->delay_threshold = simGetDWORD(SC, "GEOMETRY", "DELAY_THRES");
//	NC->cut_afterwrite = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERWRITE"); //write request period
//	NC->cut_aftermerge = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERMERGE"); //merge log band period
//	assert(NC->cut_afterwrite ^ NC->cut_aftermerge > 0); //只能選一個 0|x xor x|0
//
//	//#define FreeList_byValid2
//	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
//	NC->Section = simGetDWORD(SC, "GEOMETRY", "SectionSizeSector");//SubTrack
//	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");//RamSizeTrack
//	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");//
//	NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");
//
//
//	printf("NC->PsizeByte: %I64u byte\n", NC->PsizeByte); //要扣掉data和guard track 
//	printf("NC->LsizeByte: %I64u byte\n", NC->LsizeByte);
//	NC->PsizeByte += NC->trackSizeByte * NC->FREE_CUT;
//	NC->PsizeByte -= NC->trackSizeByte * NC->WASTE_CUT;
//	assert(NC->LsizeByte%NC->blockSizeByte == 0);
//
//	NC->LsizeBlock = NC->LsizeByte / NC->blockSizeByte;
//	NC->PsizeBlock = NC->LsizeBlock + 1;
//	N->partial_band = NC->PsizeBlock - 1;
//
//	//計算LB的partial page
//	//N->partial_page = (unsigned int)(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock - NC->trackSizeByte) / NC->pageSizeByte - 1;//CLB sub guard track directly, not move it in LB
//	N->partial_page = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
//	
//	printf("NC->LsizeBlock: %lu\n", NC->LsizeBlock);
//	printf("NC->PsizeBlock: %lu\n", NC->PsizeBlock);
//	assert(NC->trackSizeByte - 1 <= N->partial_page);
//	printf("N->partial_band: %lu, N->partial_page:%lu\n", (DWORD)N->partial_band, N->partial_page);
//	printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
//	printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
//	printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);
//	assert(NC->trackSizeByte % NC->pageSizeByte == 0);
//	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
//	assert(NC->blockSizeByte % NC->pageSizeByte == 0);
//	assert(NC->pageSizeByte % NC->sectorSizeByte == 0);
//
//	NC->trackSizePage = NC->trackSizeByte / NC->pageSizeByte;//外加
//	NC->trackSizeSection = NC->trackSizePage / NC->Section;
//
//	NC->pageSizeSector = NC->pageSizeByte / NC->sectorSizeByte;
//	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
//	NC->blockSizePage = NC->blockSizeByte / NC->pageSizeByte;
//	NC->LsizeSector = (DWORD)(NC->LsizeByte / NC->sectorSizeByte);
//	NC->LsizePage = (DWORD)(NC->LsizeByte / NC->pageSizeByte);
//	NC->LsizeSection = NC->LsizeSector / NC->Section;
//
//
//	NC->PsizeSector = NC->PsizeByte / NC->pageSizeByte;  //(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
//	NC->PsizePage = NC->PsizeByte / NC->pageSizeByte; //(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
//	printf("NC variables\n");
//	printf("NC->PsizePage: %lu\n", NC->PsizePage);
//	printf("NC->LsizePage: %lu\n", NC->LsizePage);
//	printf("NC->trackSizePage: %lu\n", NC->trackSizePage);
//	printf("NC->blockSizePage: %lu\n", NC->blockSizePage);
//	printf("%lu pages in data band, %lu pages in LB\n", NC->blockSizeByte / NC->pageSizeByte, N->partial_page + 1);
//
//	NC->blockSizeTrack = (NC->blockSizePage / NC->trackSizePage);// assert(NC->blockSizeTrack == 64); //外加
//	
//	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
//	N->blocks[0].pages = (flashPage *)calloc(NC->PsizePage, sizeof(flashPage)); assert(N->blocks[0].pages != NULL);//all phys pages(+guard band) 
//
//	for (i = 0; i < NC->PsizeBlock; i++) {
//		N->blocks[i].pages = N->blocks[0].pages + i*(NC->blockSizePage + NC->trackSizePage);//set of a data band and GT
//	}
//
//	init(simPtr);
//	for (i = 0; i < NC->LsizeBlock; i++) {
//		//N->L2Ptable[i] = i;					// map LBA i to PBA i
//		N->blocks[i].Merge = 0;
//		for (j = 0; j < NC->blockSizePage; j++) {
//			N->blocks[i].pages[j].sector = i*NC->blockSizeSector + j*NC->pageSizeSector;	// map the sector # to page		
//			N->blocks[i].pages[j].valid = 1;
//		}
//	}
//	N->Total_Merge = 0;
//	N->PM_Merge = 0;
//	N->PMBTD_Merge = 0;
//
//	printf("	logical block, i = %d\n", i);
//	for (i = 0; i < NC->LsizeBlock; i++){
//		for (j = 0; j < NC->blockSizePage; j += (NC->blockSizePage - 1)) {
//			assert(0 <= N->blocks[i].pages[j].sector && N->blocks[i].pages[j].sector < NC->LsizePage);
//			assert(N->blocks[i].pages[j].valid == 1);
//		}
//	}
//
//	for (j = 0; j <= N->partial_page; j++){
//		//30bit sector:0x3fffffff, 31bit sector:0x7fffffff
//		N->blocks[NC->PsizeBlock - 1].pages[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
//		N->blocks[NC->PsizeBlock - 1].pages[j].valid = 0;
//		assert(N->blocks[NC->PsizeBlock - 1].pages[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].pages[j].valid == 0);
//	}
//	printf("	log buffer pages: %I64u\n", N->partial_page);
//
//	N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
//	N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
//	N->writedPageNum = 0;
//	N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
//	N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
//	printf("FASTinit CLB OK\n");
//
//	N->NSLB_tracks  = 0;
//	if (NC->NSLBpercent >= 5){
//		DWORD five_percent_tracks = (N->partial_page + 1) / (NC->trackSizePage) / 10 / 2;
//		N->NSLB_tracks = NC->NSLBpercent / 5 * five_percent_tracks;
//	}else if (0 <= NC->NSLBpercent && NC->NSLBpercent <=4){
//		DWORD one_percent_tracks = (N->partial_page + 1) / (NC->trackSizePage) / 100;
//		N->NSLB_tracks = NC->NSLBpercent * one_percent_tracks;
//	}else{ printf("NSLB percent error"); system("pause"); }
//	//N->NSLB_tracks = NC->NSLBpercent * ((N->partial_page + 1) / NC->trackSizePage) / 100; //NSLB給多，可能可以做
//	printf("NC->NSLBpercent = %lu, NSLB_tracks = %lu\n", NC->NSLBpercent, N->NSLB_tracks);
//	
//	//初始化NSLBtoSLB的TD、初始化section(SubTrack)的大小
//	//N->HD_WRITE_Len = 16;//不使用
//	N->NSLBGCtoDB_Threshold = 32 * NC->trackSizePage;
//	//N->SubTrack = NC->SubTrack;// 32;//SubTrack = section = 32 sector
//
//	NS->mergecnt = 0;
//	SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
//	SLB->firstRWpage = 0;
//	SLB->lastRWpage = 0;
//	SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
//	SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
//	SLB->writedPageNum = 0;
//	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
//	SLB->write_count = 0; SLB->merge_count = 0;
//	printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);
//
//	//NSLB_tracks = 50 * ((N->partial_page + 1) / (NC->trackSizePage) / 100);
//	NSLB->firstRWpage = SLB->partial_page + 1;
//	NSLB->lastRWpage = SLB->partial_page + 1;
//	NSLB->NSLBpage_num = N->NSLB_tracks*NC->trackSizePage;
//	NSLB->partial_page = SLB->partial_page + N->NSLB_tracks*NC->trackSizePage;// N->partial_page;//old log buffer partial page == NSLB's partial page
//	NSLB->writedPageNum = 0;
//	NSLB->pageWrite = 0; NSLB->pageRead = 0; NSLB->mergecnt = 0;
//	NSLB->write_count = 0; NSLB->merge_count = 0;
//	assert(NSLB->partial_page - NSLB->firstRWpage + 1 == N->NSLB_tracks*NC->trackSizePage);
//	for (i = NSLB->firstRWpage; i <= NSLB->partial_page; i++){ assert(N->blocks[NC->PsizeBlock - 1].pages[i].valid == 0); }
//
//	N->BPLRU = (I64*)calloc(NC->LsizeBlock, sizeof(I64));
//	for (i = 0; i < NC->LsizeBlock; i++){ N->BPLRU[i] = 0; }
//	N->cnt = 0;
//	N->NSLBremainSectorTable = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
//	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLBremainSectorTable[i] = NC->trackSizePage; }
//	N->NSLBexhaustedTrack = 0;
//
//	printf("NSLB->firstRWpage = %lu, NSLB->lastRWpage = %lu, NSLB->NSLBpage_num = %lu, NSLB->partial_page = %lu\n", NSLB->firstRWpage, NSLB->lastRWpage, NSLB->NSLBpage_num, NSLB->partial_page);
//	printf("NSLB->writedPageNum = %lu\n", NSLB->writedPageNum);
//
//	//試驗
//	N->SLB_Hit = 0; N->SLB_Miss = 0;
//	N->NSLB_Hit = 0; N->NSLB_Miss = 0;
//	N->cycle_reqSectorWrite = 0;
//	N->cycleSLB_Hit = 0; N->cycleSLB_Miss = 0;
//	N->cycleNSLB_Hit = 0; N->cycleNSLB_Miss = 0;
//	
//	//試驗
//	N->BurstLen = 0; N->BurstLen_Sum = 0; N->BurstLen_Max = 0;
//	N->BurstLen_Start = 0; N->BurstLen_End = 0;
//
//	//試驗
//	N->ValidIs2 = 0;
//	N->ValidIs0 = 0;
//	N->SourceTracks_count = 0;
//
//	//section分佈
//	for (i = 0; i < 27236; i++){
//		N->track_valid[i] = 0;
//		for (j = 0; j < 64; j++){
//			N->track_valid_section[i][j]=0;
//		}
//	}
//	N->track_valid_use=0;
//	N->req_write_section = 0;
//
//#ifdef SpecificTrack
//	//NSLB試驗
//	N->NSLBusedTrack = 0;
//	/*N->NSLB_TrackIndex = (NSLB_SpecificTrack*)calloc(NC->LsizeBlock, sizeof(NSLB_SpecificTrack));
//	for (i = 0; i < NC->LsizeBlock; i++){
//		N->NSLB_TrackIndex[i].track_num = 0;
//		N->NSLB_TrackIndex[i].use_index = 0;
//		N->NSLB_TrackIndex[i].head = NULL;
//	}*/
//	/*N->NSLB_Band = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
//	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLB_Band[i] = -1; assert(N->NSLB_Band[i] == -1); }*/
//
//	////RAM結構
//	N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage / NC->Section;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
//	N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//	for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
//	N->NSLB_RAM_request = 0;
//	N->RAM_Valid = (BYTE*)calloc(NC->LsizeSection, sizeof(BYTE));
//	for (i = 0; i < NC->LsizeSection; i++){ assert(N->RAM_Valid[i] == 0); }
//	
//	//sector ram
//	//N->NSLB_RAM_SIZE = NC->RamSizeTrack * NC->trackSizePage;/* assert(NC->SubTrack == 1); assert(N->NSLB_RAM_SIZE == 2 * NC->trackSizePage);*/// 
//	//N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
//	//for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
//	//N->NSLB_RAM_request = 0;
//	//N->RAM_Valid = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
//	//for (i = 0; i < NC->LsizeSector; i++){ assert(N->RAM_Valid[i] == 0); }
//
//
//
//	//統計RAM Hit吸收的寫入
//	N->RAM_WriteOut = 0;
//	N->Host2RAM = 0;
//	//試驗
//	N->RAM_Hit = 0;
//	N->RAM_Miss = 0;
//#ifdef NSLB_RAM_BandLRU_HIT
//	N->RAM_BPLRU = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
//	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->RAM_BPLRU[i] == 0); }
//	N->RAM_BPLRU_Time = 0;
//#endif
//#endif
//#ifdef Time_Overhead
//	//Write_W:0, Write_R:1, Merge_W:2, Merge_R:3
//	//分別算write和merge的seek time
//	N->Write_W_SeekTime = 0; N->Write_R_SeekTime = 0; N->Merge_W_SeekTime = 0; N->Merge_R_SeekTime = 0;
//	N->Write_W_RotTime = 0; N->Write_R_RotTime = 0; N->Merge_W_RotTime = 0; N->Merge_R_RotTime = 0;
//	N->content_rotate = 0;
//	N->FirstTime = 0;
//	//seek次數
//	N->Write_W_SeekCount = 0; N->Write_R_SeekCount = 0;
//	N->Merge_W_SeekCount = 0; N->Merge_R_SeekCount = 0;
//	N->seek_0 = 0;
//	N->seek_0_WW = 0; N->seek_0_WR = 0; N->seek_0_MW = 0; N->seek_0_MR = 0;
//
//	//scaling formula: default Archive HDD 8TB
//	//8TB有15628053168個sector，8001563222016 Byte，635907個cylinder
//	//這邊換設定可能造成seek的改變 //scale cylinder
//	/*if (NC->PsizeByte % 8001563222016 != 0){ N->Cylinders = NC->PsizeByte * 635907 / 8001563222016 + 1; }
//	else{ N->Cylinders = NC->PsizeByte * 635907 / 8001563222016; }*/
//	NC->CylinderSizeTrack = 8;
//	/*N->Cylinders = NC->PsizeSector / NC->trackSizePage / NC->CylinderSizeTrack;*/
//	N->Cylinders = NC->PsizeSector / NC->trackSizePage / NC->CylinderSizeTrack;
//
//	//N->Scale_up = 83303; N->Scale_down = N->Cylinders;//(83303, 635907):default Archive HDD 635907 cylinders
//	//N->SeekModelBoundary = 15000; 
//	//NC->CylinderSizeTrack = 12;
//	//N->dScale = 0;
//	N->Scale_up = 81928; N->Scale_down = N->Cylinders;// 1962
//	printf("N->Scale_up %lu, N->Scale_down(N->Cylinder) %lu, ", N->Scale_up, N->Scale_down); //system("pause");
//	N->Scale_down = N->Cylinders;//(83303, 635907):default Archive HDD 635907 cylinders
//	N->SeekModelBoundary = 34829;// 15993;// 383;// 15000; //616; // 15000;
//	NC->CylinderSizeTrack = 8;//head num 12;
//	N->d = 0;
//
//
//	//disk head所在位置，用在每次計算兩個寫入的head位置差
//	N->pre_HeadCylinder = (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack; 
//	N->cur_HeadCylinder = (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack;//假設disk head起始位置在LB的第0 track
//	N->pre_HeadSector = NC->LsizeSector; 
//	N->cur_HeadSector = NC->LsizeSector;
//	//電梯法找LB所需要的merge_ram
//	//N->merge_ram_size、N->merge_ram[]: 改成merge func裡的local變數，因為global的變數會被遞迴的下一層破壞
//	//紀錄NSLB上一個寫入的page位置、是否merge剛結束
//	N->preNSLB_WriteSector = 0; N->NSLBmerge = 0;
//	N->preNSLB_WriteTrack = 0;
//	//統計rotation
//	N->rotation_preSectorInTrack = 0;
//	N->rotation_count = 0;
//
//	//額外統計: reWrite_
//	N->NSLBreWrite_SeekTime = 0;
//	N->reWrite_pre_HeadLocatedCylinder = (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack;
//	N->reWrite_cur_HeadLocatedCylinder = (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack;//假設disk head起始位置在LB的第0 track
//
//	//以track為單位RMW或是做merge，1MB
//	N->WriteBuff_SIZE = NC->RMWbufferSizeTrack * NC->trackSizeSection;
//	N->WriteBuff = (DWORD*)calloc(N->WriteBuff_SIZE, sizeof(DWORD));
//	N->WriteBuff_section = 0;
//
//	//以track為單位RMW或是做merge，1MB
//	N->NMR_RMWBuff_SIZE = NC->RMWbufferSizeTrack * NC->trackSizeSection;
//	N->NMR_RMWBuff = (DWORD*)calloc(N->WriteBuff_SIZE, sizeof(DWORD));
//	N->NMR_RMWBuff_section = 0;
//
//	N->WriteBuff_writecount = 0;
//	N->merge_ram_writecount = 0;
//	N->Merge_Bandcount = 0;
//	N->Merge_Bandcount1 = 0;
//	N->Merge_Bandcount2_1 = 0;
//	N->Merge_Bandcount2_2 = 0;
//	N->Merge_Trackcount = 0;
//	N->Merge_LB_Trackcount = 0;
//	N->WriteBuff_WriteOut = 0;
//	N->NMR_Rotate = 0;
//	N->NoMerge_Band = 0;
//
//	printf("N->WriteBuff_SIZE %lu\n", N->WriteBuff_SIZE);// system("pause");
//
//	N->SLBws = 0;
//	N->SLBms = 0;
//	N->NSLBws = 0; 
//	N->NSLBms = 0;
//
//	N->SLBtrackChange = 0; 
//	N->RDB = 0;
//	N->RMW = 0;
//	N->Merge = 0;
//
//	/*N->SLBtrackChange_time = 0;
//	N->RDB_time = 0;
//	N->RMW_time = 0;
//	N->Merge_time = 0;*/
//
//	printf("Time Overhead Initial ok\n");
//#endif
//	printf("SeekTime init OK\n");
//#ifdef BPLRULlist_RunLen
//	N->BPLRUtotal_count = 0;
//	N->BPLRUtotal_len = 0;
//	N->BPLRUcurrent_len = 0;
//	N->BPLRUcurrent_max = 0;
//	N->BPLRUcurrent_min = 9223372036854775807;
//#endif
//	printf("NEWGC64trackWRITE init OK\n");
//}

#endif
#ifdef SLBNSLBPM_Tmerge
void FASTinitSLBNSLB(sim *simPtr) { // 將 FAST FTL 初始化
	DWORD	i, j;
	simConfig	*SC = &(simPtr->simConfigObj); NFTLconfig	*NC = &(simPtr->NFTLobj.configObj); NFTLstat	*NS = &(simPtr->NFTLobj.statObj); NFTL		*N = &(simPtr->NFTLobj);
	NFTL_SLB *SLB = &((simPtr->NFTLobj).SLB); NFTL_NSLB *NSLB = &((simPtr->NFTLobj).NSLB);
	
	/*BYTE sum = 0, tmp = 0;
	for (i = 0; i < 8; i++){
		printf("%lu\n", track_offset(i));
		sum += track_offset(i);
	}
	printf("sum=%lu\n", sum); 
	tmp |= track_offset(0);
	printf("tmp=%lu\n", tmp);
	tmp |= track_offset(2);
	printf("tmp=%lu\n", tmp);
	tmp |= track_offset(5);
	printf("tmp=%lu\n", tmp);
	tmp |= track_offset(5);
	tmp |= track_offset(0);
	printf("tmp=%lu\n", tmp);
	tmp |= track_offset(2);
	printf("tmp=%lu\n", tmp);
	printf("tmp=%lu\n", tmp);
	printf("&:%lu\n", tmp & track_offset(5));
	if (tmp & track_offset(5) == track_offset(5)){ printf("yes\n"); }
	system("pause");*/

	printf("FASTinit CLB ...\n");
	NC->PsizeByte = simGetI64(SC, "GEOMETRY", "PsizeByte");
	NC->LsizeByte = simGetI64(SC, "GEOMETRY", "LsizeByte");
	NC->blockSizeByte = simGetDWORD(SC, "GEOMETRY", "blockSizeByte");
	NC->pageSizeByte = simGetDWORD(SC, "GEOMETRY", "pageSizeByte");
	NC->sectorSizeByte = simGetDWORD(SC, "GEOMETRY", "sectorSizeByte");
	NC->trackSizeByte = simGetDWORD(SC, "GEOMETRY", "trackSizeByte");
	NC->trackSizePage = NC->trackSizeByte / NC->pageSizeByte;//我外加的
	printf("NC->pageSizeByte %I64u, NC->sectorSizeByte %I64u\n", NC->pageSizeByte, NC->sectorSizeByte);
	NC->writeinterference = simGetDWORD(SC, "GEOMETRY", "writeinterference");
	NC->MAX_CUT = simGetDWORD(SC, "GEOMETRY", "MAXCUT");
	NC->FREE_CUT = simGetDWORD(SC, "GEOMETRY", "FREECUT");
	NC->WASTE_CUT = simGetDWORD(SC, "GEOMETRY", "WASTECUT");
	NC->cut_def = simGetDWORD(SC, "GEOMETRY", "CUTDEF");
	NC->delay_threshold = simGetDWORD(SC, "GEOMETRY", "DELAY_THRES");
	NC->cut_afterwrite = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERWRITE"); //write request period
	NC->cut_aftermerge = simGetDWORD(SC, "GEOMETRY", "CUT_AFTERMERGE"); //merge log band period
	assert(NC->cut_afterwrite ^ NC->cut_aftermerge > 0); //只能選一個 0|x xor x|0
	//#define FreeList_byValid2
	NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	NC->Section = simGetDWORD(SC, "GEOMETRY", "SectionSizeSector");//SubTrack
	NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");//RamSizeTrack
	NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");//
	NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	////#define FreeList_byValid2
	//NC->NSLBpercent = simGetDWORD(SC, "GEOMETRY", "NSLBpercent");
	//NC->Section = simGetDWORD(SC, "GEOMETRY", "SectionSizeSector");//SubTrack
	//NC->RamSizeTrack = simGetDWORD(SC, "GEOMETRY", "RamSizeTrack");//RamSizeTrack
	//NC->RMWbufferSizeTrack = simGetDWORD(SC, "GEOMETRY", "RMWbufferSizeTrack");//
	//NC->Disk_trackSizePage = simGetDWORD(SC, "GEOMETRY", "Disk_trackSizePage");

	printf("NC->PsizeByte: %I64u byte\n", NC->PsizeByte); //要扣掉data和guard track 
	printf("NC->LsizeByte: %I64u byte\n", NC->LsizeByte);
	NC->PsizeByte += NC->trackSizeByte * NC->FREE_CUT;
	NC->PsizeByte -= NC->trackSizeByte * NC->WASTE_CUT;
	assert(NC->LsizeByte%NC->blockSizeByte == 0);

	NC->LsizeBlock = NC->LsizeByte / NC->blockSizeByte;
	NC->PsizeBlock = NC->LsizeBlock + 1;
	N->partial_band = NC->PsizeBlock - 1;

	//計算LB的partial page
	//N->partial_page = (unsigned int)(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock - NC->trackSizeByte) / NC->pageSizeByte - 1;//CLB sub guard track directly, not move it in LB
	N->partial_page = (DWORD)((NC->PsizeByte - NC->LsizeByte - (I64)NC->trackSizeByte*(I64)NC->LsizeBlock) / (I64)NC->pageSizeByte) - 1;
	printf("NC->LsizeBlock: %lu\n", NC->LsizeBlock);
	printf("NC->PsizeBlock: %lu\n", NC->PsizeBlock);
	assert(NC->trackSizeByte - 1 <= N->partial_page);
	printf("N->partial_band: %lu, N->partial_page:%lu\n", (DWORD)N->partial_band, N->partial_page);

	printf("physical ovp in byte(NC->PsizeByte - NC->LsizeByte): %I64u BYTE, %I64u page\n", NC->PsizeByte - NC->LsizeByte, (NC->PsizeByte - NC->LsizeByte) / (I64)NC->pageSizeByte);
	printf("可用來寫入的LB大小: %lu BYTE, %lu pages\n", (N->partial_page + 1)*NC->pageSizeByte, N->partial_page);  //(NC->PsizeByte - NC->LsizeByte - NC->trackSizeByte*NC->LsizeBlock) / NC->pageSizeByte
	printf("NC->MAX_CUT: 切%lu刀\n", NC->MAX_CUT);
	assert(NC->trackSizeByte % NC->pageSizeByte == 0);
	assert(NC->trackSizeByte % NC->sectorSizeByte == 0);
	assert(NC->blockSizeByte % NC->pageSizeByte == 0);
	assert(NC->pageSizeByte % NC->sectorSizeByte == 0);

	NC->pageSizeSector = NC->pageSizeByte / NC->sectorSizeByte;
	NC->blockSizeSector = NC->blockSizeByte / NC->sectorSizeByte;
	NC->blockSizePage = NC->blockSizeByte / NC->pageSizeByte;
	NC->LsizeSector = (DWORD)(NC->LsizeByte / NC->sectorSizeByte);
	NC->LsizePage = (DWORD)(NC->LsizeByte / NC->pageSizeByte);

	NC->PsizeSector = NC->PsizeByte / NC->pageSizeByte;  //(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	NC->PsizePage = NC->PsizeByte / NC->pageSizeByte; //(NC->PsizeByte - (NC->LsizeBlock + 1)*(NC->writeinterference - 1)*NC->trackSizeByte) / NC->pageSizeByte;
	printf("NC variables\n");
	printf("NC->PsizePage: %lu\n", NC->PsizePage);
	printf("NC->LsizePage: %lu\n", NC->LsizePage);
	printf("NC->trackSizePage: %lu\n", NC->trackSizePage);
	printf("NC->blockSizePage: %lu\n", NC->blockSizePage);
	printf("%lu pages in data band, %lu pages in LB\n", NC->blockSizeByte / NC->pageSizeByte, N->partial_page + 1);

	NC->blockSizeTrack = (NC->blockSizePage / NC->trackSizePage); //assert(NC->blockSizeTrack == 64); //外加
	N->blocks = (flashBlock *)calloc(NC->PsizeBlock, sizeof(flashBlock)); assert(N->blocks != NULL); //164個physical block
	N->blocks[0].pages = (flashPage *)calloc(NC->PsizePage, sizeof(flashPage)); assert(N->blocks[0].pages != NULL);//all phys pages(+guard band) 

	for (i = 0; i < NC->PsizeBlock; i++) {
		N->blocks[i].pages = N->blocks[0].pages + i*(NC->blockSizePage + NC->trackSizePage);//set of a data band and GT
	}

	N->L2Ptable = (unsigned int *)calloc(NC->LsizeBlock, sizeof(unsigned int)); assert(N->L2Ptable != 0);
	//N->repTable = (unsigned int *)calloc(NC->PsizeBlock, sizeof(unsigned int)); assert(N->repTable != 0);
	init(simPtr);
	for (i = 0; i < NC->LsizeBlock; i++) {
		N->L2Ptable[i] = i;					// map LBA i to PBA i
		//N->repTable[i] = MLC_LOGBLOCK_NULL;	// no log blocks	//好像沒用到，之後來看要不要刪	
		for (j = 0; j < NC->blockSizePage; j++) {
			N->blocks[i].pages[j].sector = i*NC->blockSizeSector + j*NC->pageSizeSector;	// map the sector # to page		
			N->blocks[i].pages[j].valid = 1;
		}
#ifdef NEWGC64trackWRITE
#ifdef Band_64MB_8Track
		for (j = 0; j < 8; j++) N->blocks[i].DirtyTrack[j] = 0;
#endif
#ifdef Band_128MB_16Track
		for (j = 0; j < 16; j++) N->blocks[i].DirtyTrack[j] = 0;
#endif
#ifdef Band_256MB_32Track
		for (j = 0; j < 32; j++) N->blocks[i].DirtyTrack[j] = 0;
#endif
		N->blocks[i].NewPM = 0;
		N->blocks[i].Cut = 0;
		N->blocks[i].GuardTrackIndex = 0;
#endif
	}

	//
	/*N->Total_Merge = 0;
	N->PM_Merge = 0;
	N->PMBTD_Merge = 0;*/

	printf("	logical block, i = %d\n", i);
	for (i = 0; i < NC->LsizeBlock; i++){
		for (j = 0; j < NC->blockSizePage; j += (NC->blockSizePage - 1)) {
			if (N->blocks[i].pages[j].sector >= NC->LsizePage){
				printf("error sector: block %I64u pages %I64u sector %I64u\n", i, j, N->blocks[i].pages[j].sector);
			}
			if (N->blocks[i].pages[j].valid == 0){
				printf("error valid: block %I64u pages %I64u sector %I64u\n", i, j, N->blocks[i].pages[j].sector);
			}
		}
	}

	for (j = 0; j <= N->partial_page; j++){
		N->blocks[NC->PsizeBlock - 1].pages[j].sector = 0x3fffffff;//means the sector is not yet mapping to data band
		N->blocks[NC->PsizeBlock - 1].pages[j].valid = 0;
		assert(N->blocks[NC->PsizeBlock - 1].pages[j].sector == 0x3fffffff); assert(N->blocks[NC->PsizeBlock - 1].pages[j].valid == 0);
	}
	//N->repTable[NC->PsizeBlock - 1] = NC->PsizeBlock - 1 - 1;
	printf("	log buffer pages: %I64u\n", N->partial_page);

	N->firstRWpage = 0;  //N->blocks[0].pages + (NC->PsizeBlock - 1)*(NC->blockSizePage + NC->trackSizeByte / NC->pageSizeByte);
	N->lastRWpage = 0;//  (N->partial_page + 1) / (NC->trackSizeByte / NC->pageSizeByte);
	N->writedPageNum = 0;
	N->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	N->LBpage_num = N->partial_page + 1 - NC->trackSizePage;
	printf("FASTinit CLB OK\n");

	//NSLB Initialize
	N->NSLB_tracks = 0;
	if (NC->NSLBpercent >= 5){
		DWORD five_percent_tracks = (N->partial_page + 1) / (NC->trackSizePage) / 10 / 2;
		N->NSLB_tracks = NC->NSLBpercent / 5 * five_percent_tracks;
	}
	else if (0 <= NC->NSLBpercent && NC->NSLBpercent <= 4){
		DWORD one_percent_tracks = (N->partial_page + 1) / (NC->trackSizePage) / 100;
		N->NSLB_tracks = NC->NSLBpercent * one_percent_tracks;
	}
	else{ printf("NSLB percent error"); system("pause"); }
	//N->NSLB_tracks = (N->partial_page + 1) / (NC->trackSizePage) / 2;
	printf("NC->NSLBpercent = %lu, NSLB_tracks = %lu\n", NC->NSLBpercent, N->NSLB_tracks);


	
	//初始化NSLBtoSLB的TD、初始化section(SubTrack)的大小
	//N->NSLBGCtoDB_Threshold = 32 * NC->trackSizePage;
	//N->SubTrack = 32;//SubTrack = section = 32 sector

	NS->mergecnt = 0;
	SLB->guardTrack1stPage = N->firstRWpage + NC->trackSizePage;
	SLB->firstRWpage = 0;
	SLB->lastRWpage = 0;
	SLB->partial_page = N->partial_page - 2 * N->NSLB_tracks*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = N->partial_page + 1 - NC->trackSizePage - 2 * N->NSLB_tracks*NC->trackSizePage;
	//SLB->writedPageNum = 0;
	SLB->pageWrite = 0; SLB->pageRead = 0; SLB->mergecnt = 0;
	SLB->write_count = 0; SLB->merge_count = 0;
	printf("SLB->guardTrack1stPage = %lu, SLB->firstRWpage = %lu, SLB->lastRWpage = %lu, SLB->partial_page = %lu, SLB->SLBpage_num = %lu, SLB->SLBtrack_num = %lu\n", SLB->guardTrack1stPage, SLB->firstRWpage, SLB->lastRWpage, SLB->partial_page, SLB->SLBpage_num, SLB->SLBpage_num / NC->trackSizePage);

	/*NEWGC64trackWRITE原先位置*/
//#ifdef NEWGC64trackWRITE
//	//觀察適合做Partial Merge的Band會不會一直來
//#ifdef Band_64MB_8Track
//	N->PartialMerge_Threshold = 32;//調整TD參數:寫32 track、25、20、15
//#endif
//#ifdef Band_128MB_16Track
//	N->PartialMerge_Threshold = 64;
//#endif
//#ifdef Band_256MB_32Track
//	N->PartialMerge_Threshold = 128;
//#endif
//
//	//Initial PM Space
//	assert(SLB->SLBpage_num % NC->trackSizePage == 0);
//	N->PM_Rate = 25;//沒用要設0
//	N->PM_Tracks = N->PM_Rate*(SLB->SLBpage_num / NC->trackSizePage) / 100;
//	printf("PM_Tracks %lu\n", N->PM_Tracks);
//	//更新SLB的partial_page、SLBpage_num
//	SLB->partial_page = SLB->partial_page - (N->PM_Tracks)*NC->trackSizePage; //N->partial_page;// 
//	SLB->SLBpage_num = SLB->SLBpage_num - (N->PM_Tracks)*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
//	//PM Queue Initialize
//	N->PMQ_Tnum = 0; N->PMQ_HEAD = NULL; N->PMQ_TAIL = NULL;
//	N->Free_Tnum = 0; N->FreeQ = NULL;
//	for (i = 0; i < N->PM_Tracks; i++){//PMQ初始化為NULL，所有PM_Track先放進FreeQ(ueue)
//		struct PM_NODE *PMNtmp = (struct PM_NODE*)calloc(1, sizeof(struct PM_NODE));
//		assert(PMNtmp->next == NULL);
//		assert(PMNtmp->Band_No == 0);
//		assert(PMNtmp->SLB_PageIndex == 0);
//		PMNtmp->next = N->FreeQ;
//		N->FreeQ = PMNtmp;
//		N->Free_Tnum++;
//	}
//	assert(N->Free_Tnum == N->PM_Tracks);
//	assert(N->PMQ_Tnum + N->Free_Tnum == N->PM_Tracks);
//	DWORD tmp_len0 = 0, tmp_len1 = 0;
//	struct PM_NODE *ct0 = N->FreeQ, *ct1 = N->PMQ_HEAD;
//	while (ct0 != NULL){ tmp_len0++; ct0 = ct0->next; }
//	while (ct1 != NULL){ tmp_len1++; ct1 = ct1->next; }
//	if (!(tmp_len0 + tmp_len1 == N->PM_Tracks)){ printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); system("pause"); }
//
//	N->PMSpace = (DWORD *)calloc(N->PM_Tracks*NC->trackSizePage, sizeof(DWORD)); assert(N->PMSpace != NULL);
//	N->PMlastBand = 0;
//	//統計成功做PM的比例
//	N->All_Merge = 0;
//	N->Allow_PM = 0;
//	N->DO_PM = 0;
//#ifdef Section_Mapping
//	N->SubTrack = 32;//跟SLB、NSLB、HLB的宣告位置不一樣，改在Section_Mapping裡
//#endif
//#endif

	//N->NSLB2SLBpercent = 40;
	//N->NSLB_tracks = (N->partial_page + 1) / (NC->trackSizePage) / 2;

	//NSLB Initialize
	NSLB->firstRWpage = SLB->partial_page + 1;
	NSLB->lastRWpage = SLB->partial_page + 1;
	NSLB->NSLBpage_num = N->NSLB_tracks*NC->trackSizePage;
	NSLB->partial_page = SLB->partial_page + N->NSLB_tracks*NC->trackSizePage;// N->partial_page;//old log buffer partial page == NSLB's partial page
	NSLB->writedPageNum = 0;
	NSLB->pageWrite = 0; NSLB->pageRead = 0; NSLB->mergecnt = 0;
	NSLB->write_count = 0; NSLB->merge_count = 0;
	assert(NSLB->partial_page - NSLB->firstRWpage + 1 == N->NSLB_tracks*NC->trackSizePage);
	//for (i = NSLB->firstRWpage; i <= NSLB->partial_page; i++){ assert(N->blocks[NC->PsizeBlock - 1].pages[i].valid == 0); }

	//NSLB BPLRU
	N->BPLRU = (I64*)calloc(NC->LsizeBlock, sizeof(I64));
	for (i = 0; i < NC->LsizeBlock; i++) assert( N->BPLRU[i] == 0);
	N->cnt = 0;
	N->NSLBremainSectorTable = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLBremainSectorTable[i] = NC->trackSizePage; }
	N->NSLBexhaustedTrack = 0;
	N->NSLBGCtoDB_Threshold = 32 * NC->trackSizePage;
	printf("NSLB->firstRWpage = %lu, NSLB->lastRWpage = %lu, NSLB->NSLBpage_num = %lu, NSLB->partial_page = %lu\n", NSLB->firstRWpage, NSLB->lastRWpage, NSLB->NSLBpage_num, NSLB->partial_page);
	printf("NSLB->writedPageNum = %lu\n", NSLB->writedPageNum);
	
	//Seek time Initialize
	NS->SLB_Seek = 0;
	NS->NSLB_Seek = 0;
	printf("SeekTime init OK\n");



#ifdef NEWGC64trackWRITE
	//觀察適合做Partial Merge的Band會不會一直來
#ifdef Band_64MB_8Track
	N->PartialMerge_Threshold = 32;//調整TD參數:寫32 track、25、20、15
#endif
#ifdef Band_128MB_16Track
	N->PartialMerge_Threshold = 64;
#endif
#ifdef Band_256MB_32Track
	N->PartialMerge_Threshold = 128;
#endif

	//Initial PM Space
	////N->PM_Tracks_Threshold = 20*(NSLB->NSLBpage_num / NC->trackSizePage) / 100;//NSLBPM
	//試驗
	assert(SLB->SLBpage_num % NC->trackSizePage == 0);
	N->PM_Rate = (50 - NC->NSLBpercent); assert(0 <= N->PM_Rate); assert(N->PM_Rate <= 50);//25//沒用要設0，1,3,5是prn0最大有效,15,25
	//N->PM_Tracks = N->PM_Rate*(SLB->SLBpage_num / NC->trackSizePage) / 100;//PM、HLB+
	assert(2 * N->PM_Rate*(NSLB->NSLBpage_num / NC->trackSizePage) / 100 >= 1);
	N->PM_Tracks = 2*N->PM_Rate*(NSLB->NSLBpage_num / NC->trackSizePage) / 100 - 1;//NSLB轉SLB提供PM空間
	//N->PM_Tracks = 25*(NSLB->NSLBpage_num / NC->trackSizePage) / 100;//NSLB轉SLB提供PM空間
	N->PM_Tracks_Use = 0;
	printf("N->PM_Tracks: %lu\n", N->PM_Tracks);

	//試驗
	N->PM_Rate_SLBdefault = 5;//預先設5%NSLB給SLB，需要更多必須坐NSLB轉SLB
	N->SLB_SeqTrackIndex = 4 * (NSLB->NSLBpage_num / NC->trackSizePage) / 100;//2%以上的SLB可放seq

	N->SLB_tracks = N->PM_Tracks;
	N->SLBremainSectorTable = (DWORD*)calloc(N->SLB_tracks, sizeof(DWORD));
	for (i = 0; i < N->SLB_tracks; i++){ N->SLBremainSectorTable[i] = NC->trackSizePage; }
	SLB->SLBpage_num = N->SLB_tracks*NC->trackSizePage;
	SLB->writedPageNum = 0;
	N->preSLB_WriteSector = 0;
	N->PM_Tracks_Allocated2Use_Condition = 0;
	N->PM_Sectors_Use = 0;




	//DWORD tmp_GTFirstPut_Base[6] = { 1, 2, 1, 2, 1, 2 }, tmp_GTPut_Ext[4] = { 4, 5, 8, 9 };
	//DWORD tmp_GTRePut_Base[6] = { 2, 3, 6, 7, 10, 11 };
	//for (i = 0; i < 6; i++){
	//	N->GTFirstPut_Base[i] = tmp_GTFirstPut_Base[i]; 
	//	N->GTRePut_Base[i] = tmp_GTRePut_Base[i];
	//}
	//for (i = 0; i < 4; i++){ N->GTPut_Ext[i] = tmp_GTPut_Ext[i]; }
	////25%SLB
	//N->LB_tracks50p = (NSLB->NSLBpage_num / NC->trackSizePage) / 2;// (N->partial_page + 1) / (NC->trackSizePage) / 4;
	////38%SLB
	//N->LB_tracks75p = (NSLB->NSLBpage_num / NC->trackSizePage) * 3 / 4;// (N->partial_page + 1) / (NC->trackSizePage) * 3 / 8;
	////42%SLB
	//N->LB_tracks83p = (NSLB->NSLBpage_num / NC->trackSizePage) * 5 / 6; //(N->partial_page + 1) / (NC->trackSizePage) * 5 / 12;

	//試驗
	//N->GTBuff_Size = 0;
	//N->GTBuff = (DWORD*)calloc(N->NSLB_RAM_SIZE * 3, sizeof(DWORD));
	
	/*N->PMGT_Signal = 0;
	N->PMGT_NSLBaddress = (DWORD*)calloc(NC->trackSizePage, sizeof(DWORD));*/

	printf("PM_Tracks %lu\n", N->PM_Tracks);

	//有使用FASTwriteSectorHLBplus_PM，SLB->partial_page預先扣掉N->PM_Tracks給PM Space。
	//更新SLB的partial_page、SLBpage_num
	//SLB->partial_page = SLB->partial_page - (N->PM_Tracks)*NC->trackSizePage; //N->partial_page;// 
	//SLB->SLBpage_num = SLB->SLBpage_num - (N->PM_Tracks)*NC->trackSizePage; //N->partial_page + 1 - NC->trackSizePage;//
	
	//新法法FASTwriteSectorHLBplus_PM是有PM發生才多做一次merge清出空間
	//FASTwriteSectorNSLBPM


	//PM Queue Initialize
	N->PMQ_Tnum = 0; N->PMQ_HEAD = NULL; N->PMQ_TAIL = NULL;
	N->Free_Tnum = 0; N->FreeQ = NULL;
	for (i = 0; i < N->PM_Tracks; i++){//PMQ初始化為NULL，所有PM_Track先放進FreeQ(ueue)
		struct PM_NODE *PMNtmp = (struct PM_NODE*)calloc(1, sizeof(struct PM_NODE));
		assert(PMNtmp->next == NULL);
		assert(PMNtmp->Band_No == 0);
		assert(PMNtmp->SLB_PageIndex == 0);
		PMNtmp->next = N->FreeQ;
		N->FreeQ = PMNtmp;
		N->Free_Tnum++;
	}
	assert(N->Free_Tnum == N->PM_Tracks);
	assert(N->PMQ_Tnum + N->Free_Tnum == N->PM_Tracks);
	DWORD tmp_len0 = 0, tmp_len1 = 0;
	struct PM_NODE *ct0 = N->FreeQ, *ct1 = N->PMQ_HEAD;
	while (ct0 != NULL){ tmp_len0++; ct0 = ct0->next; }
	while (ct1 != NULL){ tmp_len1++; ct1 = ct1->next; }
	if (!(tmp_len0 + tmp_len1 == N->PM_Tracks)){ printf("%lu %lu, %lu\n", tmp_len0, tmp_len1, N->PM_Tracks); system("pause"); }

	N->PMSpace = (DWORD *)calloc(N->PM_Tracks*NC->trackSizePage, sizeof(DWORD)); assert(N->PMSpace != NULL);
	N->PMlastBand = 0;
	//統計成功做PM的比例
	N->All_Merge = 0;
	N->Allow_PM = 0;
	N->DO_PM = 0;
	N->PM_len = 0;
	N->Rel_len = 0;
	//試驗
	N->pre_start = 0; N->pos_start = 0;
	N->PM_Round = 0; N->PM_GTsector = 0;
	N->PMCandidate = (DWORD*)calloc(N->PM_Tracks*NC->trackSizePage, sizeof(DWORD));
	for (i = 0; i < N->PM_Tracks; i++){ assert(N->PMCandidate[i] == 0); }
#ifdef Section_Mapping
	N->SubTrack = NC->Section;//跟SLB、NSLB、HLB的宣告位置不一樣，改在Section_Mapping裡
#endif
#endif
	printf("NEWGC64trackWRITE initial ok\n");
#ifdef SpecificTrack
	//NSLB試驗
	N->NSLBusedTrack = 0;
	N->NSLB_TrackIndex = (NSLB_SpecificTrack*)calloc(NC->LsizeBlock, sizeof(NSLB_SpecificTrack));
	for (i = 0; i < NC->LsizeBlock; i++){
		N->NSLB_TrackIndex[i].track_num = 0;
		N->NSLB_TrackIndex[i].use_index = 0;
		N->NSLB_TrackIndex[i].head = NULL;
	}
	N->NSLB_Band = (DWORD*)calloc(N->NSLB_tracks, sizeof(DWORD));
	for (i = 0; i < N->NSLB_tracks; i++){ N->NSLB_Band[i] = -1; assert(N->NSLB_Band[i] == -1); }

	N->NSLB_RAM_SIZE = 2 * NC->trackSizePage / NC->Section; //2 * NC->trackSizePage;
	N->NSLB_RAM = (DWORD*)calloc(N->NSLB_RAM_SIZE, sizeof(DWORD));
	for (i = 0; i < N->NSLB_RAM_SIZE; i++){ N->NSLB_RAM[i] = -1; assert(N->NSLB_RAM[i] == -1); }
	N->NSLB_RAM_request = 0;

	N->RAM_Valid = (BYTE*)calloc(NC->LsizeSector, sizeof(BYTE));
	for (i = 0; i < NC->LsizeSector; i++){ assert(N->RAM_Valid[i] == 0); }

	//統計RAM Hit吸收的寫入
	N->RAM_WriteOut = 0;
	//試驗
	N->RAM_Hit = 0;
	N->RAM_Miss = 0;
#ifdef NSLB_RAM_BandLRU_HIT
	N->RAM_BPLRU = (DWORD*)calloc(NC->LsizeBlock, sizeof(DWORD));
	for (i = 0; i < NC->LsizeBlock; i++){ assert(N->RAM_BPLRU[i] == 0); }
	N->RAM_BPLRU_Time = 0;
#endif
#endif
	printf("SpecificTrack initial ok\n");
#ifdef Time_Overhead
	//分別算write和merge的seek time
	N->SLBWrite_SeekTime = 0; N->SLBMerge_SeekTime = 0;
	N->NSLBWrite_SeekTime = 0; N->NSLBMerge_SeekTime = 0;


	//scaling formula: default Archive HDD 8TB
	//8TB有15628053168個sector，8001563222016 Byte，635907個cylinder
	//這邊換設定可能造成seek的改變 //scale cylinder
	//if (NC->PsizeByte % 8001563222016 != 0){ N->Cylinders = NC->PsizeByte * 635907 / 8001563222016 + 1; }
	//else{ N->Cylinders = NC->PsizeByte * 635907 / 8001563222016; }
	NC->CylinderSizeTrack = 8;
	N->Cylinders = NC->PsizeSector / NC->trackSizePage / NC->CylinderSizeTrack;

	//N->Scale_up = 83303; N->Scale_down = N->Cylinders;//(83303, 635907):default Archive HDD 635907 cylinders
	//N->SeekModelBoundary = 15000; 
	//NC->CylinderSizeTrack = 12;
	//N->dScale = 0;
	N->Scale_up = 1962; N->Scale_down = N->Cylinders;
	printf("N->Cylinders: %lu\n", N->Cylinders); //system("pause");
	N->Scale_down = N->Cylinders;//(83303, 635907):default Archive HDD 635907 cylinders
	N->SeekModelBoundary = 383;// 15000; //616; // 15000;
	NC->CylinderSizeTrack = 12;
	N->d = 0;




	//額外統計: reWrite_
	N->NSLBreWrite_SeekTime = 0;
	N->reWrite_pre_HeadLocatedCylinder = (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack;
	N->reWrite_cur_HeadLocatedCylinder = (NC->LsizeBlock*NC->blockSizeTrack + 1) / NC->CylinderSizeTrack;//假設disk head起始位置在LB的第0 track

	//試驗 以track為單位RMW
	N->WriteBuff_Size = 0;
	N->WriteBuff = (DWORD*)calloc(NC->trackSizePage / N->SubTrack, sizeof(DWORD));
	N->SLBws = 0;
	N->SLBms = 0;
	N->NSLBws = 0;
	N->NSLBms = 0;

	N->SLBtrackChange = 0;
	N->RDB = 0;
	N->RMW = 0;
	N->Merge = 0;

	N->PM_timer = 0;
	printf("Time Overhead Initial ok\n");
#endif
#ifdef BPLRULlist_RunLen
	N->BPLRUtotal_count = 0;
	N->BPLRUtotal_len = 0;
	N->BPLRUcurrent_len = 0;
	N->BPLRUcurrent_max = 0;
	N->BPLRUcurrent_min = 9223372036854775807;
	printf("BPLRULlist_RunLen Initial ok\n");
#endif
}

#endif


//緩衝區
/**/
/**/

