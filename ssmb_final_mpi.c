#include <stdio.h>
#include <stdlib.h> 
#include <tgmath.h>
#include <sys/time.h>
#include <complex.h>
#include"mpi.h"
#include<time.h>
int turns = 25000;														/*number of revolution*/
int ne = 10000; 														/*number of electron*/

double c = 2.99792458e8;												/*c-m/s*/
double r56 = 0.0512e-3; 												/*dispersion-m*/
double es = 0.44e-3;													/*energy spread*/
double damprate = 1.45e-4;    											/*damping rate*/		
double ebe = 0.629e9;   												/*beam energy-GeV*/
double manonor = 0.3e6;   												/*modulation amplitude-GeV*/
double ma;   															/*normalized modulation amplitude*/
double my_gamma;   										/*Lorentz factor*/

double nummb = 20.0;													/*number of microbunch*/

														/*energy array*/
double iniu;															/*for Gaussian randim number*/
double iniv;															/*for Gaussian randim number*/

double qnep;															/*for quantum excitation*/

double modwl = 1.0e-6;  						 						/*modulation wavelength-m*/
double norr56;															/*normalized dispersion*/
	
double dn; 

double lprange;															/*phase range in longitudinal phase space*/

int numofzpt = 300;														/*number of segmentation of phase*/
double numofzptdb;

double zposegdb;
int zposeg;



double complex ft(double x[])												
{
	double complex value = 0.0;
	int l;
	for(l = 0; l < numofzpt; l++)
	{
		value += x[l]*exp(-I*2.0*M_PI*(double)l/(double)numofzpt*20.0);
	}
	return value;
}

double err;

unsigned int seeding;

int main() {
	//int start = time(NULL);
	struct timeval start1, end, diff;
	gettimeofday(&start1, NULL);
	FILE *fpout;
	fpout = fopen("out.txt","w");
	FILE *fpoutt;
	fpoutt = fopen("outt.txt","w");
	FILE *fpouttt;
	fpouttt = fopen("outtt.txt","w");
	
	double lp[ne];															/*phase array*/
	double lpth[ne];
	double bf[turns];
	double zdis[numofzpt];
	double segpb = ((double)numofzpt)/nummb;
	for(int ii=0;ii<turns;ii++) bf[ii] = 0; //initialize array
	
	ma = manonor/ebe;
	my_gamma = ebe/(0.511e6);
	qnep = es*sqrt(2.0*damprate);
	norr56 = 2.0*M_PI*r56/modwl;
	dn = 1.0/sqrt(M_PI*my_gamma/137.0)*damprate/2.0;
	lprange = 2.0*M_PI*nummb;
	numofzptdb = numofzpt;
	
	int p_num, p_no;
	int source = 0;
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &p_num);
	MPI_Comm_rank(MPI_COMM_WORLD, &p_no);
	unsigned int local_size = ne / p_num;
	unsigned int local_start = p_no * local_size;
	if(p_no == p_num-1) local_size += ne % p_num;
	
	double *local_lp, *local_lpth, *local_zdis;
	local_lp = (double*) malloc(local_size*sizeof(double));
	local_lpth = (double *) malloc(local_size * sizeof(double));
	local_zdis = (double *) malloc(numofzpt * sizeof(double));
	double local_iniu, local_iniv;

	seeding = (time(NULL)+p_no);
	int j;	
	for(j = 0; j < local_size; j++)
	{
		local_iniu = rand_r(&seeding)/(double)(RAND_MAX+1.0)+1e-10;
		local_iniv = rand_r(&seeding)/(double)(RAND_MAX+1.0)+1e-10;
		local_lp[j] = (rand_r(&seeding)/(double)(RAND_MAX+1.0)+1e-10)*lprange;
		local_lpth[j] = sqrt(-2.0*log(local_iniu))*cos(2.0*M_PI*local_iniv)*es;

	}
	double local_err;
	int local_zposeg;
	int i;
	for(i = 0; i < turns; i++)
	{
		for(int ii=0;ii<numofzpt;ii++) 
		{
			local_zdis[ii] = 0;
			zdis[ii] = 0; //initialize array
		}
		
		for(j = 0; j < local_size; j++)
		{
			local_lpth[j] = local_lpth[j] + ma*sin(local_lp[j]);

			local_iniu = rand_r(&seeding)/(double)(RAND_MAX+1.0)+1.0e-10;
			local_iniv = rand_r(&seeding)/(double)(RAND_MAX+1.0)+1.0e-10;
			local_err = -damprate*local_lpth[j] + qnep*sqrt(-2.0*log(local_iniu))*cos(2.0*M_PI*local_iniv); 
			
 			local_iniu = rand_r(&seeding)/(double)(RAND_MAX+1.0)+1.0e-10;
			local_iniv = rand_r(&seeding)/(double)(RAND_MAX+1.0)+1.0e-10;   
			local_lp[j] = local_lp[j] + norr56*local_lpth[j]+ (local_err + dn*sqrt(-2.0*log(local_iniu))*cos(2.0*M_PI*local_iniv))*norr56/2.0;  	
			local_lpth[j] = local_lpth[j] + local_err;
			
			local_lp[j] = fmod(local_lp[j],lprange);

			local_zposeg = (local_lp[j]/(lprange/(double)numofzpt));
			if(local_zposeg >= 0)
				local_zdis[local_zposeg] += 1.0;
		}
		/*MPI_Status status;
		if(p_no == 0)
		{
			for(int ii = 0; ii < numofzpt; ii ++) zdis[ii] += local_zdis[ii];
			for(int des = 0; des<p_num; des++)
				MPI_Recv(local_zdis, numofzpt, MPI_DOUBLE, des, 0, MPI_COMM_WORLD, &status);
			for(int ii = 0; ii < numofzpt; ii ++) zdis[ii] += local_zdis[ii];
		}
		else
		{
			MPI_Send(local_zdis, numofzpt, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		}*/
		MPI_Reduce(local_zdis, zdis, numofzpt, MPI_DOUBLE, MPI_SUM, source, MPI_COMM_WORLD);
		if(p_no == 0) bf[i] = cabs(ft(zdis)/(double)ne);
	}
	
	int *gather, *offsets;
	gather = (int*)malloc(p_num*sizeof(int));
	offsets = (int*)malloc(p_num*sizeof(int));
	for(int ii=0; ii <p_num; ii++)
	{
		gather[ii] = ne / p_num;
		offsets[ii] = ii * (ne/p_num);
		if(ii == p_num-1) gather[ii] += ne % p_num;
	//	printf("p_no: %d, size: %d, offsets: %d\n",ii, gather[ii], offsets[ii]);
	}

	for(j = 0; j <local_size; j++)
	{
		if(local_lp[j] < 0.0) local_lp[j] = local_lp[j] + lprange;
	}


	MPI_Gatherv(local_lp, local_size, MPI_DOUBLE, lp, gather, offsets, MPI_DOUBLE, source, MPI_COMM_WORLD);
	MPI_Gatherv(local_lpth, local_size, MPI_DOUBLE, lpth, gather, offsets, MPI_DOUBLE, source, MPI_COMM_WORLD);

	MPI_Finalize();
	if(p_no == 0)
	{
		for(j = 0; j < turns; j ++) {fprintf(fpout,"%f\n",bf[j]);}
		for(j = 0; j < ne; j ++) {fprintf(fpoutt,"%f\n",lp[j]);} 
		for(j = 0; j < ne; j ++) {fprintf(fpouttt,"%f\n",lpth[j]);}
	}
	gettimeofday(&end, NULL);
	timersub(&end, &start1, &diff);	
	double time_used = diff.tv_sec + (double) diff.tv_usec / 1000000.0;
	if(p_no == 0) printf("total:%lf(s)\n",time_used);
	//if(p_no == 0) printf("total: %d(s)\n", time(NULL)-start);
	fclose(fpout);
	fclose(fpoutt);
	fclose(fpouttt);
	
	return 0;
}
