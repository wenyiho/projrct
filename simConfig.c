#include "basic.h"
#include "NFTL.h"
#include "sim.h"
#include "prototypes.h"
void simUpdateConfig(simConfig *C,BYTE *section,BYTE *part1,BYTE *part2,BYTE *part3);
//
// �N�]�w�ɸ̭����]�wŪ��]�w��̭��C�@���uŪ�@�չ���]�w�A�i�U���A�I�sŪ�U�@�աC
//
BYTE simGetConfig(simConfig *C) {
	BYTE section[100];
	BYTE strBuf[500],part1[100],part2[100],part3[100];

	// open the file if it has not been opened
	if(C->configFp == (FILE *)0)
		C->configFp = fopen(C->configFile,"rb");

	// �|���}�l���� config section
	section[0]=0;

	while(!feof(C->configFp))
	{	
		fgets(strBuf,500,C->configFp);

		// clear the buffers
		part1[0]=0;part2[0]=0;part3[0]=0;
		// get tokens
		sscanf(strBuf,"%s %s %s",part1,part2,part3);
		
		if(section[0]== 0 && strcmp(part1,"<SIMU>") != 0)
		{
			if(strcmp(part1,"<PAUSE>") == 0)
			{
				printf("\nPaused...press a key to continue.");
				getch();
			}
			continue;	// �S�J�� <SIMU> ���e���F�賣�S��
		}

		//
		// SIMU section �}�l�P����
		//
		if(section[0]== 0 && strcmp(part1,"<SIMU>")==0)
		{				
			strcpy(section,"SIMU");
			continue;
		}

		if(strcmp(section,"SIMU")==0 && strcmp(part1,"</SIMU>")==0)
		{
			section[0]= 0;
			return TRUE;
		}	

		//
		// BUFFER section �}�l�P����
		//
		if(strcmp(section,"SIMU")==0 && strcmp(part1,"<BUFFER>")==0)
		{	// BUFFER config section �}�l
			strcpy(section,"BUFFER");
			continue;
		}

		if(strcmp(section,"BUFFER")==0 && strcmp(part1,"</BUFFER>")==0)
		{	// BUFFER config section �����A�^�� SIMU section
			strcpy(section,"SIMU");
			continue;
		}

		//
		// NFTL section �}�l�P����
		//
		if(strcmp(section,"SIMU")==0 && strcmp(part1,"<NFTL>")==0)
		{	
			strcpy(section,"NFTL");
			continue;
		}

		if(strcmp(section,"NFTL")==0 && strcmp(part1,"</NFTL>")==0)
		{	
			strcpy(section,"SIMU");
			continue;
		}

		//
		// GEOMETRY section �}�l�P����
		//
		if(strcmp(section,"SIMU")==0 && strcmp(part1,"<GEOMETRY>")==0)
		{	
			strcpy(section,"GEOMETRY");
			continue;
		}

		if(strcmp(section,"GEOMETRY")==0 && strcmp(part1,"</GEOMETRY>")==0)
		{	
			strcpy(section,"SIMU");
			continue;
		}

		if(strcmp(part1,"")==0 || strcmp(part2,"")==0)
		{
			// ���������㪺 lines
			continue;
		}

		// �NŪ�쪺�]�w��s��]�w��̭�
		simUpdateConfig(C,section,part1,part2,part3);
	}

	// eof is encountered before </SIMU>
	return FALSE;
}


void simUpdateConfig(simConfig *C,BYTE *section,BYTE *part1,BYTE *part2,BYTE *part3)
{
	DWORD i;

	for(i=0;i<100;i++)
	{
		if(strcmp(section,C->configTable[i].section)==0 && strcmp(C->configTable[i].string,part1)==0)
		{
			// update if there is an existing entry
			strcpy(C->configTable[i].value,part2);
			// ���]�n���_��s,���p��줣�@�˪���
			strcpy(C->configTable[i].unit,part3);
			return;
		}
	}
	// falling here: no previously existing entry. insert a new one
	for(i=0;i<100;i++)
	{
		if(C->configTable[i].section[0] == 0)	// find a empty slot
		{
			strcpy(C->configTable[i].section,section);
			strcpy(C->configTable[i].string,part1);
			strcpy(C->configTable[i].value,part2);
			strcpy(C->configTable[i].unit,part3);
			return;
		}
	}

	assert(i!=100);	// there is no empty slot!
}

//
// �q�]�w����o�@�� DWORD �ƭ�
//

DWORD simGetDWORD(simConfig *C,BYTE *section,BYTE *string)
{
	DWORD ret,i;

	for(i=0;i<100;i++)
	{
		if(strcmp(C->configTable[i].section,section)==0 && strcmp(C->configTable[i].string,string)==0)
		{
			sscanf(C->configTable[i].value,"%lu",&ret);
			switch(C->configTable[i].unit[0])
			{
				case 'K': ret*=1024; break;
				case 'M': ret*=1024*1024; break;
				case 'G': ret*=1024*1024*1024; break;
			}
			return ret;
		}
	}
	assert(0);		// get a non-existing thing. this is surely an error!
	return TRUE;	// to make the compiler happy
}

//
// �q�]�w����o�@�� double �ƭ�
//

double simGetDouble(simConfig *C,BYTE *section,BYTE *string)
{
	DWORD i;
	double ret;

	for(i=0;i<100;i++)
	{
		if(strcmp(C->configTable[i].section,section)==0 && strcmp(C->configTable[i].string,string)==0)
		{
			sscanf(C->configTable[i].value,"%lf",&ret);
			switch(C->configTable[i].unit[0])
			{
				case 'K': ret*=1024.0; break;
				case 'M': ret*=1024.0*1024.0; break;
				case 'G': ret*=1024.0*1024.0*1024.0; break;
			}
			return ret;
		}
	}
	assert(0);		// get a non-existing thing. this is surely an error!
	return TRUE;	// to make the compiler happy
}

//
// �q�]�w����o�@�� I64 �ƭ�
//

I64 simGetI64(simConfig *C,BYTE *section,BYTE *string)
{
	DWORD i;
	I64 ret;

	for(i=0;i<100;i++)
	{
		if(strcmp(C->configTable[i].section,section)==0 && strcmp(C->configTable[i].string,string)==0)
		{
			sscanf(C->configTable[i].value,"%I64i",&ret);
			switch(C->configTable[i].unit[0])
			{
				case 'K': ret*=(I64)1024; break;
				case 'M': ret*=(I64)1024*(I64)1024; break;
				case 'G': ret*=(I64)1024*(I64)1024*(I64)1024; break;
			}
			return ret;
		}
	}
	assert(0);		// get a non-existing thing. this is surely an error!
	return TRUE;	// to make the compiler happy
}

//
// �q�]�w����o�@�� string
//

BYTE *simGetString(simConfig *C,BYTE *section,BYTE *string)
{
	DWORD i;

	for(i=0;i<100;i++)
	{
		if(strcmp(C->configTable[i].section,section)==0 && strcmp(C->configTable[i].string,string)==0)
		{
			return C->configTable[i].value;
		}
	}
	assert(0);		// get a non-existing thing. this is surely an error!
	return (BYTE *)0;	// to make the compiler happy
}
