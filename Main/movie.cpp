#include "ompMand.h"

class MovieMand : public OMPMand {
public:
	MovieMand(string dn, int resolution, int pb);
private:
	int numzooms = 25; 
	double dzoom; 

	double final; 
};

MovieMand::MovieMand(string dn, int resolution, int pb) 
	: OMPMand(dn, resolution, pb) {

	double start = omp_get_wtime(); 

	final = r; 
	r = 1.7; 

	double rstart = r; 

	dzoom = exp(1/double(numzooms) * log(rstart/final)); 

	printf("%21.14e\n", dzoom); 

	for (int i=0; i<numzooms; i++) {
		cout << double(i)/numzooms << endl;
		minRe = x - r*resFactor; 
		maxRe = x + r*resFactor; 
		minIm = y - r; 
		maxIm = y + r; 

		dreal = (maxRe - minRe)/(imageWidth-1); 
		dim = (maxIm - minIm)/(imageHeight-1);

		computeTime_start = omp_get_wtime(); 
		run(); 

		dumpHDF5("Movie/visit_"+to_string(i)+".h5"); 

		printCombined("Movie/ppm_"+to_string(i)+".ppm"); 

		r = rstart/pow(dzoom, i); 
	}

	double end = omp_get_wtime(); 

	printTime(end - start, "Wall Time = "); 
}

int main(int argc, char* argv[]) {
	string deckName = argv[1]; 
	int resolution = atoi(argv[2]);
	int printBool = atoi(argv[3]); 

	MovieMand m(deckName, resolution, printBool); 
}