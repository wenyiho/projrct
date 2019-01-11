#include <stdio.h>
#include <stdlib.h> 
#include <tgmath.h>
#include <time.h>
#include <complex.h>
#include <omp.h>
#include<sys/time.h>
#include<unistd.h>

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
	double complex tmp;

	for (l = 0; l < numofzpt; l++)
	{
		tmp = (double complex)x[l] * exp(-I*2.0*M_PI*(double)l / (double)numofzpt*20.0);

		value += tmp;
	}
	return value;
}

int i, j;
double err;

int seeding = 1;

int main() {

	omp_set_num_threads(omp_get_num_procs());
	struct  timeval start;
	struct  timeval end;
	gettimeofday(&start, NULL);
	FILE *fpout;
	fpout = fopen("out.txt", "w");
	FILE *fpoutt;
	fpoutt = fopen("outt.txt", "w");
	FILE *fpouttt;
	fpouttt = fopen("outtt.txt", "w");
	FILE *fpoudodo;
	fpoudodo = fopen("oudodo.txt", "w");
	FILE *fpoudododo;
	fpoudododo = fopen("oudododo.txt", "w");


	double complex expc[numofzpt];

	double lp[ne];															/*phase array*/
	double lpth[ne];
	double complex bf[turns];
	double zdis[numofzpt];
	double segpb = ((double)numofzpt) / nummb;
	int ii = 0;

	for (ii = 0; ii < numofzpt; ii++)
		expc[ii] = (double complex) 0;
	for (ii = 0; ii<turns; ii++) bf[ii] = (double complex) 0; //initialize array

	double complex local_value = (double complex) 0;
	
	ma = manonor / ebe;
	my_gamma = ebe / (0.511e6);
	qnep = es*sqrt(2.0*damprate);
	norr56 = 2.0*M_PI*r56 / modwl;
	dn = 1.0 / sqrt(M_PI*my_gamma / 137.0)*damprate / 2.0;
	lprange = 2.0*M_PI*nummb;
	numofzptdb = numofzpt;
	/*
	std::random_device rd;
	std::default_random_engine gen = std::default_random_engine(rd());
	std::uniform_real_distribution<double> dis(0- RAND_MAX, RAND_MAX);*/
	//srand(seeding);
	for (j = 0; j < ne; j++)
	{
		iniu = rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1e-10;
		iniv = rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1e-10;
		lp[j] = (rand_r(&seeding) / (double)(RAND_MAX + 1.0) + 1e-10)*lprange;
		lpth[j] = sqrt(-2.0*log(iniu))*cos(2.0*M_PI*iniv)*es;
	}
	for (j = 0; j < ne; j++)
	{
		fprintf(fpoudodo, "%f\n", lp[j]);
		fprintf(fpoudododo, "%f\n", lpth[j]);
	}
	#pragma omp parallel private(i,j,iniu,iniv,local_value,err,zposeg)
	{
		int id = omp_get_thread_num();
		int nthrds = omp_get_num_threads();
		for (i = 0; i < turns; i++)
		{
			local_value = 0;
			for (j = id; j < ne; j += nthrds)
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
				zposeg = (lp[j] / (lprange / (double)numofzpt));
				if (zposeg >= 0)
				{
					double complex tmp;
					if (expc[zposeg] == 0)
					{
						tmp = (double complex)exp(-I*2.0*M_PI*(double)zposeg / (double)numofzpt*20.0);
						expc[zposeg] = tmp;
					}
					else
						tmp = expc[zposeg];
					local_value += tmp;
				}
				if (j + nthrds >= ne)
					bf[i] += local_value;
			}

		}
	}
	
	/*
	for (i = 0; i < turns; i++)
	{
		for(ii=0;ii<4;ii++)
			local_value[ii] =(double complex) 0;
		#pragma omp parallel for schedule(guided) private(j,iniu,iniv,err,zposeg)
		for (j = 0; j < ne; j++)
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
			zposeg = (lp[j] / (lprange / (double)numofzpt));
			if (zposeg >= 0)
			{
				double complex tmp;
				if (expc[zposeg] == 0)
				{
					tmp = (double complex)exp(-I*2.0*M_PI*(double)zposeg / (double)numofzpt*20.0);
					expc[zposeg] = tmp;
				}
				else
					tmp = expc[zposeg];
				//#pragma omp critical 
				local_value[omp_get_thread_num()] += tmp;
			}

		}

		bf[i] = local_value[0] + local_value[1] + local_value[2] + local_value[3];

		//for(j = 0; j < numofzpt; j++)
		//{
		//printf("%f\n",zdis[j]);

		//}
	}
	*/

	//for(j = 0; j < numofzpt; j++)
	//{
	//printf("%f\n",zdis[j]);

	//}
	for (j = 0; j < ne; j++)
	{
		if (lp[j] < 0.0) lp[j] = lp[j] + lprange;
	}


	for (j = 0; j < turns; j++)
	{
		fprintf(fpout, "%f\n", cabs(bf[j] / (double)ne));
	}

	for (j = 0; j < ne; j++)
	{
		fprintf(fpoutt, "%f\n", lp[j]);
		fprintf(fpouttt, "%f\n", lpth[j]);
	}

	fclose(fpout);
	fclose(fpoutt);
	fclose(fpouttt);
	fclose(fpoudodo);
	fclose(fpoudododo);
	gettimeofday(&end, NULL);
	printf("%f\n", (double) (1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec)/1000000.0);
	return 0;
}