#define M_PI 3.14159265358979323846

#include <cstdio>
#include <cstdlib>
#include <ctgmath>
#include <ctime>
//#include <complex>

// For the CUDA runtime routines (prefixed with "cuda_")
//#include <cuda.h>
#include <curand_kernel.h>
#include <curand.h>
#include <cuComplex.h>
//#include <cuda_runtime.h>
//#include <device_launch_parameters.h>

#define NUM_OF_THREADS 10000
#define THREADS_PER_BLOCK 256

#define TURNS   25000										        	/*number of revolution*/
#define NE      10000 										        	/*number of electron*/

#define NUMOFZPT 300								                	/*number of segmentation of phase*/
//double numofzptdb = numofzpt;

//#define _C_m_s		(2.99792458e8)										/*c-m/s*/
//#define R56			(0.0512e-3) 										/*dispersion-m*/
#define ES			(0.44e-3)											/*energy spread*/
#define DAMPRATE	(1.45e-4)    										/*damping rate*/
//#define EBE			(0.629e9)				   							/*beam energy-GeV*/
//#define MANONOR 	(0.3e6)												/*modulation amplitude-GeV*/

#define MA			(0.00047694753577106518) 							/*normalized modulation amplitude*/
//double my_gamma = EBE/(0.511e6);				      					/*Lorentz factor*/

#define NUMMB		(20.0)    								    		/*number of microbunch*/

#define QNEP		(7.4929300010076163e-006)  							/*for quantum excitation*/

#define MODWL		(1.0e-6)  						 		    		/*modulation wavelength-m*/
#define NORR56		(321.69908772759482)				    			/*normalized dispersion*/

#define DN			(1.3646097851959425e-005)

#define LPRANGE		(125.66370614359172)								/*phase range in longitudinal phase space*/

//double zposegdb;

__constant__ int seeding = 1;

__device__ double atomicDoubleAdd(double* address, double val){
    unsigned long long int* address_as_ull =
                             (unsigned long long int*)address;
    unsigned long long int old = *address_as_ull, assumed;
    do {
        assumed = old;
old = atomicCAS(address_as_ull, assumed,
                        __double_as_longlong(val +
                               __longlong_as_double(assumed)));
    } while (assumed != old);
    return __longlong_as_double(old);
}

__device__ cuDoubleComplex ft(double *x){
	cuDoubleComplex value = make_cuDoubleComplex(0.0, 0.0);//, I = make_cuDoubleComplex(0.0, 1.0);
	int l;
	double y = -2.0*M_PI/(double)NUMOFZPT*20.0;
	cuDoubleComplex tmp;
	for(l = 0; l < NUMOFZPT; l++){
		sincos(y*(double)l, &tmp.x, &tmp.y);
		
		value.x += x[l]*tmp.x;
		value.y += x[l]*tmp.y;
		//printf("%f\n",x[l]);
		x[l] = 0;
	}
	return value;
}

// Kernel
__global__ void CalKernel(double *lp, double *lpth, double *bf, curandStateXORWOW_t *states){

	int tid = threadIdx.x, id = tid + blockIdx.x * THREADS_PER_BLOCK;
	curandStateXORWOW_t localState = states[id];
	__shared__ double zdis[NUMOFZPT];
	
	double l_lp, l_lpth;
	cuDoubleComplex l_bf;
	
	int zposeg;
                                                                            /*energy array*/
    double iniu;															/*for Gaussian random number*/
    double iniv;															/*for Gaussian random number*/
	double err;
	if(id < NE){
		iniu = curand_uniform_double(&localState);
		iniv = curand_uniform_double(&localState);
		l_lp = curand_uniform_double(&localState)*LPRANGE;
		l_lpth = sqrt(-2.0*log(iniu))*cos(2.0*M_PI*iniv)*ES;
		//if(id == 0) printf("\nlp[id] = %f, lpth[id] = %f\n",lp[id],lpth[id]);
		int i;

		if(id == 0){
			for(i = 0; i < NUMOFZPT; i++){
				zdis[i] = 0;
			}
		}
		for(i = 0; i < TURNS; i++){
			__syncthreads();
			//if(id == 0 && i < 10) printf("\n**TURN: %d**\n",i);
			l_lp += MA*sin(l_lp);

			iniu = curand_uniform_double(&localState);
			iniv = curand_uniform_double(&localState);
			err = -DAMPRATE*l_lpth + QNEP*sqrt(-2.0*log(iniu))*cos(2.0*M_PI*iniv);
			//if(id == 0 && i < 10) printf("err = %f\n",err);
			iniu = curand_uniform_double(&localState);
			iniv = curand_uniform_double(&localState);
			l_lp += NORR56*l_lpth+ (err + DN*sqrt(-2.0*log(iniu))*cos(2.0*M_PI*iniv))*NORR56/2.0;
			l_lpth = l_lpth + err;
			//if(id == 0 && i < 10) printf("lp[id] = %f, lpth[id] = %f\n",lp[id],lpth[id]);

			l_lp = fmod(l_lp,LPRANGE);
			zposeg = (l_lp/(LPRANGE/(double)NUMOFZPT));
			
			if(zposeg >= 0) {atomicDoubleAdd(&zdis[zposeg], 1.0);}
			
			__syncthreads();
			if(threadIdx.x == 0){
				l_bf = ft(zdis);
				atomicDoubleAdd(&bf[i], l_bf.x);
				atomicDoubleAdd(&bf[i+TURNS], l_bf.y);
			}
		}
		if(l_lp < 0.0) l_lp = l_lp + LPRANGE;
		
		lp[id] = l_lp;
		lpth[id] = l_lpth;
	}
}

__global__ void SetupKernel(curandStateXORWOW_t *states){
    int id = threadIdx.x + blockIdx.x * THREADS_PER_BLOCK;
	if(id < NE){
		curand_init(seeding, id, 0, &states[id]);
	}
}

void Calculate(double *lp, double *lpth, double *bf, int blocksPerGrid, curandStateXORWOW_t *states){
    cudaError_t error;
	double *d_lp, *d_lpth, *d_bf, bf_tmp[TURNS*2];
	
	//cudaMemcpyToSymbol(blockcounter, &counter, sizeof(int));

	// Allocate memory for result on Device
    cudaMalloc(&d_lp, sizeof(double)*NE);
    cudaMalloc(&d_lpth, sizeof(double)*NE);
    cudaMalloc(&d_bf, sizeof(double)*TURNS*2);
	cudaMemset(d_bf, 0, sizeof(double)*TURNS*2);

	// Launch Kernel
    CalKernel<<<blocksPerGrid, THREADS_PER_BLOCK>>>(d_lp, d_lpth, d_bf, states);
  // check for error
  error = cudaGetLastError();
  if(error != cudaSuccess)
  {
    // print the CUDA error message and exit
    printf("CUDA error: %s\n", cudaGetErrorString(error));
    exit(-1);
  }

	// Copy result to Host
    error = cudaMemcpy(lp, d_lp, sizeof(double)*NE, cudaMemcpyDeviceToHost);
	//printf("Error NO:%d\n", error);
	printf("CUDA error: %s\n", cudaGetErrorString(error));
    cudaMemcpy(lpth, d_lpth, sizeof(double)*NE, cudaMemcpyDeviceToHost);
    cudaMemcpy(bf_tmp, d_bf, sizeof(double)*TURNS*2, cudaMemcpyDeviceToHost);
	
	int i;
	for(i = 0; i < TURNS; i++){
		//if(i < 10) printf("(%f, %f i)\n",bf_tmp[i],bf_tmp[i+TURNS]);
		bf[i] = (bf_tmp[i]/NE)*(bf_tmp[i]/NE) + (bf_tmp[i+TURNS]/NE)*(bf_tmp[i+TURNS]/NE);
		bf[i] = sqrt(bf[i]);
	}

	// Free Memory
    cudaFree(d_lp);
    cudaFree(d_lpth);
    cudaFree(d_bf);
}

/*void SetupConstant(){
	// Calculate constant value
	double l_ma = MANONOR/EBE;
	double l_qnep = ES*sqrt(2.0*DAMPRATE);
	double l_norr56 = 2.0*M_PI*R56/MODWL;
	double l_dn = 1.0/sqrt(M_PI*my_gamma/137.0)*DAMPRATE/2.0;
	double l_lprange = 2.0*M_PI*NUMMB;
	
	printf("\nma = %f\nqnep = %f\nnorr56 = %f\ndn = %f\nlprange = %f\n",l_ma,l_qnep,l_norr56,l_dn,l_lprange);
	
	// Copy constant value to device
	cudaMemcpyToSymbol(ma, &l_ma, sizeof(double));
	cudaMemcpyToSymbol(qnep, &l_qnep, sizeof(double));
	cudaMemcpyToSymbol(norr56, &l_norr56, sizeof(double));
	cudaMemcpyToSymbol(dn, &l_dn, sizeof(double));
	cudaMemcpyToSymbol(lprange, &l_lprange, sizeof(double));
}*/

void CalOnDevice(double *lp, double *lpth, double *bf){
    int blocksPerGrid = (NUM_OF_THREADS + THREADS_PER_BLOCK - 1) / THREADS_PER_BLOCK;
    curandStateXORWOW_t *states;

	// Allocate memory for Random Generator State
    cudaMalloc((void **)&states, THREADS_PER_BLOCK * blocksPerGrid * sizeof(curandStateXORWOW_t));

	// Setup Constant
	/*printf("Setup Constant...");
	SetupConstant();
	printf("Complete.\n");*/
	
	// Setup Random Generator State
	printf("Setup Random Generator State...");
    SetupKernel<<<blocksPerGrid, THREADS_PER_BLOCK>>>(states);
	printf("Complete.\n");
	
	// Start Calculation
	printf("Start Calculation...");
    Calculate(lp, lpth, bf, blocksPerGrid, states);
	printf("Complete.\n");
	
    cudaFree(states);
}

int main() {
	FILE *fpout;
	fpout = fopen("out.txt","w");
	FILE *fpoutt;
	fpoutt = fopen("outt.txt","w");
	FILE *fpouttt;
	fpouttt = fopen("outtt.txt","w");

	double lp[NE];															/*phase array*/
	double lpth[NE];
	double bf[TURNS];
	//double segpb = ((double)numofzpt)/nummb;

	printf("Execute calculation on the device.\n");
	CalOnDevice(lp, lpth, bf);

	int j;

	printf("Output: out.txt\n");
	for(j = 0; j < TURNS; j++){
        fprintf(fpout,"%f\n",sqrt(bf[j]));
	}
	printf("Output: outt.txt\n");
	for(j = 0; j < NE; j++){
        fprintf(fpoutt,"%f\n",lp[j]);
	}
	printf("Output: outtt.txt\n");
	for(j = 0; j < NE; j++){
        fprintf(fpouttt,"%f\n",lpth[j]);
	}

	fclose(fpout);
	fclose(fpoutt);
	fclose(fpouttt);

	return 0;
}
