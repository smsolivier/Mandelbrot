#include <mpi.h>

#include "../Mandelbrot.h"

using namespace std; 

class MPIMand : public Mandelbrot {
public:
	MPIMand(int rank, MPI_Status stat, int tasks, string name, int resolution, int printBool); 
private:
	int rank, numtasks; 
	MPI_Status Stat; 

	int *pimage, *piter; 
	double *psmooth; 

	int binStart, binEnd, binWidth; 
	void run(); 
	void buildArrays(); 
	void end(); 
};

MPIMand::MPIMand(int r, MPI_Status stat, int tasks, string name, int resolution, int printBool) {
	res = resolution; 
	print = printBool; 
	deckname = name; 
	rank = r; 
	Stat = stat; 
	numtasks = tasks; 

	getDeck(); 

	binWidth = imageWidth/numtasks; 

	binStart = rank * binWidth; 
	binEnd = binStart + binWidth; 

	pimage = new int[binWidth*imageHeight]; 
	piter = new int[binWidth*imageHeight]; 
	psmooth = new double[binWidth*imageHeight];  

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<binWidth; j++) {
			int index = i*binWidth + j; 
			pimage[index] = -1; 
			piter[index] = -1; 
			psmooth[index] = -1; 
		}
	}

	if (rank == 0) printOpening(); 

	MPI_Barrier(MPI_COMM_WORLD); 

	computeTime_start = clock(); 
	run(); 
	computeTime_stop = clock();

	MPI_Barrier(MPI_COMM_WORLD); 

	buildArrays(); 

	MPI_Barrier(MPI_COMM_WORLD); 

	if (rank == 0) {
		if (print == 1) printData(); 
		printSet(); 
	}

	end(); 
}

void MPIMand::run() {
	if (rank == 0) cout << "calculating set" << endl;
	double cre, cim, temp, zre, zim; 
	for (int i=0; i<imageHeight; i++) {
		cim = maxIm - i*dim; 
		for (int j=binStart; j<binEnd; j++) {
			int index = i*binWidth + j - binStart; 
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
			piter[index] = k; 
			if (inSet) pimage[index] = 1; 

			else if (!inSet) pimage[index] = 0; 

			double mag = pow(pow(zre,2) + pow(zim,2), .5); 
			psmooth[index] = k + 1 - log10(log10(mag))/log10(2); 
			if (k<=maxIter) hist[k] += 1; 
			else cout << "k = " << k << endl;

		}
		if (i%((int)imageHeight/6) == 0) {
			cout << string(rank*9,' '); 
			cout << fixed; 
			cout << setprecision(2);
			cout << rank << ":" << (double)i/imageHeight*100 << "\n";
			cout.flush(); 
		} 
	} 
}

void MPIMand::buildArrays() {
	if (rank != 0) {
		MPI_Send(pimage, binWidth*imageHeight, MPI_INT, 0, 1, MPI_COMM_WORLD); 
		MPI_Send(piter, binWidth*imageHeight, MPI_INT, 0, 2, MPI_COMM_WORLD); 
		MPI_Send(psmooth, binWidth*imageHeight, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD); 
		MPI_Send(hist, maxIter, MPI_INT, 0, 4, MPI_COMM_WORLD); 
	}
	else if (rank == 0) {
		cout << "building arrays" << endl;
		for (int i=0; i<imageHeight; i++) { // copy rank 0 to total array 
			for (int j=0; j<binWidth; j++) {
				int pindex = i*binWidth + j; 
				int index = i*imageWidth + j; 
				image[index] =  pimage[pindex]; 
				iter[index] = piter[pindex]; 
				smooth[index] = psmooth[pindex]; 
			}
		}
		int *thist = new int[maxIter]; 
		for (int k=1; k<numtasks; k++) { // copy rest to big array
			MPI_Recv(pimage, binWidth*imageHeight, MPI_INT, k, 1, MPI_COMM_WORLD, &Stat); 
			MPI_Recv(piter, binWidth*imageHeight, MPI_INT, k, 2, MPI_COMM_WORLD, &Stat); 
			MPI_Recv(psmooth, binWidth*imageHeight, MPI_DOUBLE, k, 3, MPI_COMM_WORLD, &Stat); 
			MPI_Recv(thist, maxIter, MPI_INT, k, 4, MPI_COMM_WORLD, &Stat); 
			for (int i=0; i<imageHeight; i++) {
				for (int j=0; j<binWidth; j++) {
					int pindex = i*binWidth + j; 
					int index = i*imageWidth + j + k*binWidth; 
					image[index] = pimage[pindex]; 
					iter[index] = piter[pindex]; 
					smooth[index] = psmooth[pindex]; 
				}
			}
			for (int i=0; i<maxIter; i++) {
				hist[i] += thist[i]; 
			}
			progressBar(k-1, numtasks-1, progBarWidth); 
		}
	}  
}

void MPIMand::end() {
	wallTime_stop = clock(); 

	double computeEl = (computeTime_stop - computeTime_start)/CLOCKS_PER_SEC; 
	double wallEl = (wallTime_stop - wallTime_start)/CLOCKS_PER_SEC; 

	if (rank != 0) {
		MPI_Send(&computeEl, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD); 
		MPI_Send(&wallEl, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD); 
	}

	else if (rank == 0) {
		double *compute = new double[numtasks]; 
		double *wall = new double[numtasks]; 

		compute[0] = computeEl; 
		wall[0] = wallEl; 

		for (int i=1; i<numtasks; i++) {
			MPI_Recv(&computeEl, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &Stat); 
			MPI_Recv(&wallEl, 1, MPI_DOUBLE, i, 2, MPI_COMM_WORLD, &Stat); 

			compute[i] = computeEl; 
			wall[i] = wallEl; 
		}

		cout << "Compute Times:  " << endl;  
		for (int i=0; i<numtasks; i++) {
			printTime(compute[i], "Task " + to_string(i) + " = "); 
		}

		cout << "Wall Times:  " << endl;
		for (int i=0; i<numtasks; i++) {
			printTime(wall[i], "Taks " + to_string(i) + " = "); 
		}

		// average, max time
		double cavg = 0; 
		double wavg = 0; 
		double cmax = -1; 
		double wmax = -1; 
		for (int i=0; i<numtasks; i++) {
			cavg += compute[i]; 
			wavg += wall[i]; 
			if (cmax < compute[i]) cmax = compute[i]; 
			if (wmax < wall[i]) wmax = wall[i]; 
		}

		printTime(cavg, "Average Compute Time = "); 
		printTime(wavg, "Average Wall Time = "); 
		printTime(cmax, "Max Compute Time = "); 
		printTime(wmax, "Max Wall Time = "); 
	}
}

int main(int argc, char *argv[]) {
	int numtasks, rank, len, rc; 
	char hostname[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Get_processor_name(hostname,&len);
	MPI_Status Stat;

	MPIMand m(rank, Stat, numtasks, argv[1], atoi(argv[2]), atoi(argv[3])); 

	MPI_Finalize(); 
	return 0; 
}
