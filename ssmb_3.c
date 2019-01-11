#include <stdio.h>
#include <stdlib.h> 
#include <tgmath.h>
#include <time.h>
#include <complex.h>
#include <pthread.h>
#include<sys/time.h>
#include<unistd.h>

int turns = 25000;														/*number of revolution*/
int ne = 10000;
pthread_mutex_t mutex;													/*number of electron*/
pthread_barrier_t barr;
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

double lp[10000];															/*phase array*/
double lpth[10000];
double bf[25000];
double zdis[300];
double complex expc[300];

double complex ft(double x[])
{
	double complex value = 0.0;
	int l;
	for (l = 0; l < numofzpt; l++)
	{
		value += x[l] * exp(-I*2.0*M_PI*(double)l / (double)numofzpt*20.0);
	}
	return value;
}
int seeding = 1;

void* Thread_sum(void* rank) 
{
	double iniu, iniv, err;
	int j,zposeg1;
	for (j = (int)rank; j < ne; j+=8)
	{
		iniu = rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1e-10;
		iniv = rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1e-10;
		lp[j] = (rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1e-10)*lprange;
		lpth[j] = sqrt(-2.0*log(iniu))*cos(2.0*M_PI*iniv)*es;
	}
	
	int i;
	for (i = 0; i < turns; i++)
	{
		double complex local_value = (double complex) 0;

		for (j = (int)rank; j < ne; j+=8)
		{
			lpth[j] = lpth[j] + ma*sin(lp[j]);

			iniu = rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1.0e-10;
			iniv = rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1.0e-10;
			err = -damprate*lpth[j] + qnep*sqrt(-2.0*log(iniu))*cos(2.0*M_PI*iniv);

			iniu = rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1.0e-10;
			iniv = rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1.0e-10;
			lp[j] = lp[j] + norr56*lpth[j] + (err + dn*sqrt(-2.0*log(iniu))*cos(2.0*M_PI*iniv))*norr56 / 2.0;
			lpth[j] = lpth[j] + err;

			//printf("%f\n",lp[j]);
			
			lp[j] = fmod(lp[j], lprange);
			
			//printf("%f\n",lp[j]);
			zposeg1 = (lp[j] / (lprange / (double)numofzpt));
			if (zposeg1 >= 0)
			{
				double complex tmp;
				if (expc[zposeg1] == 0)
				{
					tmp = (double complex)exp(-I*2.0*M_PI*(double)zposeg1 / (double)numofzpt*20.0);
					expc[zposeg1] = tmp;
				}
				else
					tmp = expc[zposeg1];
				local_value += tmp;
			}
			if (j + 8 >= ne)
			{
				bf[i] += local_value;
			}
			//printf("%d\n",zposeg);
		}
		
	}


}






int i,j;
double err;


int main() {

	struct  timeval start;
	struct  timeval end;
	gettimeofday(&start, NULL);
	pthread_barrier_init(&barr, NULL, (unsigned)4);
	FILE *fpout;
	fpout = fopen("out.txt","w");
	FILE *fpoutt;
	fpoutt = fopen("outt.txt","w");
	FILE *fpouttt;
	fpouttt = fopen("outtt.txt","w");
	
	for (int ii = 0; ii < numofzpt; ii++)
		expc[ii] = (double complex) 0;
	double segpb = ((double)numofzpt)/nummb;
	for(int ii=0;ii<turns;ii++) bf[ii] = 0; //initialize array
	
	ma = manonor/ebe;
	my_gamma = ebe/(0.511e6);
	qnep = es*sqrt(2.0*damprate);
	norr56 = 2.0*M_PI*r56/modwl;
	dn = 1.0/sqrt(M_PI*my_gamma/137.0)*damprate/2.0;
	lprange = 2.0*M_PI*nummb;
	numofzptdb = numofzpt;
	
	srand(seeding);
	
	int thread;
	int thread_count = 8;
	pthread_mutex_init(&mutex, NULL);
	pthread_t* thread_handles = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
	for (thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread], NULL, Thread_sum, (void*)thread);
	for (thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread], NULL);
	free(thread_handles);
	//for(j = 0; j < numofzpt; j++)
	//{
	//printf("%f\n",zdis[j]);

	//}
	
	for(j = 0; j < ne; j++)
	{
		if(lp[j] < 0.0) lp[j] = lp[j] + lprange;

	}
	
	
	for(j = 0; j < turns; j++)
	{
	fprintf(fpout,"%f\n", cabs(bf[j] / (double)ne));

	}
	
	for(j = 0; j < ne; j++)
	{
	fprintf(fpoutt,"%f\n",lp[j]);
	}
	for(j = 0; j < ne; j++)
	{
	fprintf(fpouttt,"%f\n",lpth[j]);
	}
	
	fclose(fpout);
	fclose(fpoutt);
	fclose(fpouttt);
	gettimeofday(&end, NULL);
	printf("%f\n", (double)(1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec) / 1000000.0);
	return 0;
}