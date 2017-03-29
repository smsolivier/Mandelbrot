#include <omp.h>

#include "../Mandelbrot.h"

using namespace std;

#define OMP 1

class OMPMand : public Mandelbrot {
public:
	OMPMand(string dn, int resolution, int pb); 
	void start(); 
	void run(); 
private:
	void end(); 

};

OMPMand::OMPMand(string dn, int resolution, int pb) {
	res = resolution; 
	deckname = dn; 
	print = pb;  

	getDeck(); 
}

void OMPMand::start() {
	cout << "Running OMPMandelbrot" << endl;

	printOpening(); 

	computeTime_start = omp_get_wtime(); 
	run(); 
	computeTime_stop = omp_get_wtime(); 

	if (print == 1) printData(); 

	printSet(); 

	dumpHDF5("mand.h5"); 

	end(); 
}

void OMPMand::run() {
	cout << "calculating set" << endl;

	double cre, cim, temp, zre, zim; 
	int j, tid; 

	for (int i=0; i<imageHeight; i++) {
		cim = maxIm - i*dim; 

		// #if OMP == 1
		#pragma omp parallel private(j, tid)
		// #endif
		{
			// #if OMP == 1
			#pragma omp for schedule(dynamic, 1) nowait
			// #endif 
			for (j=0; j<imageWidth; j++) {
				int index = i*imageWidth + j; 
				cre = minRe + j*dreal; 
				zim = 0; 
				zre = 0; 

				bool converged = 1; 
				bool inSet = 1; 

				int k;
				for (k=0; k<maxIter; k++) {
					temp = pow(zre,2) - pow(zim, 2) + cre; 
					zim = 2*zre*zim + cim; 
					zre = temp; 

					converged = pow(zre,2) + pow(zim,2) < 4; 

					if (converged == 0) {
						inSet = 0; 
						break; 
					}
				}
				iter[index] = k; 
				if (inSet) image[index] = 1; 

				else if (!inSet) image[index] = 0; 

				double mag = pow(pow(zre,2) + pow(zim,2), .5); 
				smooth[index] = k + 1 - log10(log10(mag))/log10(2); 
				if (k<=maxIter) hist[k] += 1; 
				else cout << "k = " << k << endl;
			}
		}
		progressBar(i, imageHeight, computeTime_start, omp_get_wtime(), 50); 
	} 

}

void OMPMand::end() {
	wallTime_stop = omp_get_wtime(); 

	printTime(computeTime_stop - computeTime_start, "Compute Time = "); 
	// printTime(wallTime_stop - wallTime_start, "Wall Time = "); 
}

