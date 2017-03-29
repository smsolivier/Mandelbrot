#include "../Mandelbrot.h"

using namespace std; 

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char *file, int line, bool abort=true)
{
   if (code != cudaSuccess) 
   {
      fprintf(stderr,"GPUassert: %s %s %d\n", cudaGetErrorString(code), file, line);
      if (abort) exit(code);
   }
}

__global__ void mandKernel(double minRe, double maxIm, 
							double dreal, double dim, 
							int maxIter, double escape, 
							int imageWidth, int imageHeight, 
							int *image, int *iter, 
							double *smooth) {
	int x = blockIdx.x * blockDim.x + threadIdx.x; 
	int y = blockIdx.y * blockDim.y + threadIdx.y; 

	if (x < imageWidth && y < imageHeight) {

		double cre, cim, zre, zim, temp; 

		cim = maxIm - y*(dim); 

		int index = y*imageWidth + x; 
		cre = minRe + x*dreal; 

		zim = 0; 
		zre = 0; 

		bool inSet = 1; 

		int k; 
		for (k=0; k<maxIter; k++) {
			temp = pow(zre,2) - pow(zim, 2) + cre; 
			zim = 2*zre*zim + cim; 
			zre = temp; 

			if (pow(zre,2) + pow(zim,2) > 4) {
				inSet = 0; 
				break; 
			}
		}
		if (inSet) image[index] = 1; 
		else if (!inSet) image[index] = 0; 
		else printf("major problem"); 

		iter[index] = k; 

		double mag = pow(pow(zre,2) + pow(zim,2),.5); 
		smooth[index] = k + 1 - log10f(log10f(mag))/log10f(2); 
	} 
}

class GPUMand : public Mandelbrot {
public:
	GPUMand(string name, int resolution, int printBool); 
private:
	void run(); 
	void end();  

	int ncore = 32; 

	int *gimage, *giter; 
	double *gsmooth; 

	cudaDeviceProp prop; 

	int progBarWidth = 50; 
}; 

GPUMand::GPUMand(string name, int resolution, int printBool) {
	res = resolution; 
	print = printBool; 
	deckname = name; 

	cudaGetDeviceProperties(&prop, 0); 

	cout << "Running GPUMandelbrot on " << prop.name << endl;
	cout << ncore << " threads per block" << endl;

	getDeck(); 

	printOpening(); 

	computeTime_start = clock();
	run(); 
	computeTime_stop = clock();

	if (print == 1) printData(); 

	printSet(); 

	end(); 
}

void GPUMand::run() {
	cout << "calculating set" << endl;
	gpuErrchk(cudaMalloc(&gimage, sizeof(int)*imageWidth*imageHeight)); 
	gpuErrchk(cudaMalloc(&giter, sizeof(int)*imageWidth*imageHeight)); 
	gpuErrchk(cudaMalloc(&gsmooth, sizeof(double)*imageWidth*imageHeight)); 

	size_t total; size_t avail; 
	cudaMemGetInfo(&avail, &total); 
	cout << "vram used = " << (total - avail)/1e9 << " GB" << endl;

	dim3 threads(ncore, ncore); 

	int bx = (imageWidth + threads.x - 1)/threads.x; 
	int by = (imageHeight + threads.y - 1)/threads.y; 

	dim3 blocks(bx, by); 
 
	mandKernel<<<blocks, threads>>>(minRe, maxIm, dreal, dim, maxIter, 
		escape, imageWidth, imageHeight, gimage, giter, gsmooth); 

	gpuErrchk(cudaMemcpy(image, gimage, 
		sizeof(int)*imageWidth*imageHeight, cudaMemcpyDeviceToHost)); 
	gpuErrchk(cudaMemcpy(iter, giter, 
		sizeof(int)*imageWidth*imageHeight, cudaMemcpyDeviceToHost)); 
	gpuErrchk(cudaMemcpy(smooth, gsmooth, 
		sizeof(double)*imageWidth*imageHeight, cudaMemcpyDeviceToHost));  

	cudaFree(gimage); cudaFree(giter); cudaFree(gsmooth);  

	// populate hist
	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			hist[iter[index]]++; 
		}
	}
}

void GPUMand::end() {
	wallTime_stop = clock(); 

	printTime((computeTime_stop - computeTime_start)/CLOCKS_PER_SEC, "Compute Time = "); 

	printTime((wallTime_stop - wallTime_start)/CLOCKS_PER_SEC, "Wall Time = "); 
}

int main(int argc, char *argv[]) {
	GPUMand g(argv[1], atoi(argv[2]), atoi(argv[3]));
	// GPUMand g("tendril", 1000, 1);  
}