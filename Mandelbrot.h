#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <ctime>
#include "hdf5.h"

#include "Functions/printTime.h"
#include "Functions/readDeck.c"
#include "Functions/progressBar.c"

class Mandelbrot {
private:
protected:
	void initializeArrays(); 
	void getDeck();
	void printingType(); 
	void printOpening();  
	void printArray(int *array, int rows, int cols, string fname);
	void printArray(double *array, int rows, int cols, string fname); 
	void printPlain();  
	double * makeColorFrac();
	double * makeSmooth(); 
	void printHistogram(); 
	void printSmooth(); 
	void printCombined();  
	void printCombined(string fname);
	void printIter(); 
	void printSet(); 
	void printData(); 
	void printHDF5Array(double * array, hid_t file_id, string fn);
	void printHDF5Array(int * array, hid_t file_id, string fn);
	void dumpHDF5(string fn); 

	double x, y, r; 

	int maxColor = 1000; 
	int progBarWidth = 50; 
	int res, maxIter, colorBool, cR, cG, cB, print, imageWidth, imageHeight; 

	double minRe, maxRe, minIm, maxIm, escape, resFactor, dreal, dim; 
	double computeTime_start, computeTime_stop, wallTime_start, wallTime_stop; 
	string deckname; 

	int *image, *iter, *hist; 
	double *smooth; 
};

void Mandelbrot::initializeArrays() {
	image = new int[imageWidth*imageHeight]; 
	iter = new int[imageWidth*imageHeight]; 
	smooth = new double[imageWidth*imageHeight]; 

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			image[index] = -1; 
			iter[index] = -1; 
			smooth[index] = -1; 
		}
	}

	hist = new int[maxIter]; 
	for (int i=0; i<maxIter; i++) {
		hist[i] = 0; 
	}
}

void Mandelbrot::getDeck() {
	wallTime_start = clock(); 

	ifstream deck(deckname); 
	read(x, deck); 
	read(y, deck); 
	read(r, deck); 
	read(resFactor, deck); 
	read(maxIter, deck); 
	read(escape, deck); 
	read(colorBool, deck); 
	read(cR, deck); 
	read(cG, deck); 
	read(cB, deck); 
	deck.close(); 

	cR *= maxColor/256; 
	cG *= maxColor/256; 
	cB *= maxColor/256; 

	minRe = x - r*resFactor; 
	maxRe = x + r*resFactor; 
	minIm = y - r; 
	maxIm = y + r; 

	imageWidth = res * resFactor; 
	imageHeight = res; 

	dreal = (maxRe - minRe)/(imageWidth-1); 
	dim = (maxIm - minIm)/(imageHeight-1);

	initializeArrays(); 
}

void Mandelbrot::printArray(int *array, int rows, int cols, string fname) {
	/* 	Prints int array to file
		Inputs:
			*array: pointer to array to be printed
			rows:	number of rows in array
			cols:	number of columns in array
			fname:	file name of save location 
	*/
	cout << "printing array " << fname << endl;
	ofstream out(fname); 
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			int index = i*cols + j; 
			out << array[index] << " "; 
		}
		out << "\n";
	}
	out.close(); 
}

void Mandelbrot::printArray(double *array, int rows, int cols, string fname) {
	/* 	Prints double array to file
		Inputs:
			*array: pointer to array to be printed
			rows:	number of rows in array
			cols:	number of columns in array
			fname:	file name of save location 
	*/
	cout << "printing array " << fname << endl;
	ofstream out(fname); 
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			int index = i*cols + j; 
			out << array[index] << " "; 
		}
		out << "\n";
	}
	out.close();
}

void Mandelbrot::printingType() {
	if (colorBool == 0) cout << "plain coloring" << endl;
	else if(colorBool == 1) cout << "histogram coloring" << endl;
	else if(colorBool == 2) cout << "smooth coloring" << endl;
	else if(colorBool == 3) cout << "histogram and smooth coloring" << endl;
	else {
		cout << "program terminated\ncolor must be 0, 1 or 2" << endl;
		exit(0); 
	}
}

void Mandelbrot::printOpening() {
	cout << "x = " << x << "\ny = " << y << "\nr = " << r << endl;
	printf("maxIter = %e", (double)maxIter); 
	cout << "\nescape = " << escape << "\nres = " << res << endl;

	cout << "plotting from " << minRe << " + i" << minIm << " to " 
			<< maxRe << " + i" << maxIm << endl;

	printingType(); 

	if (print == 1) cout << "printing = yes" << endl;
	else if (print == 0) cout << "printing = no" << endl;
	else {
		cout << "program terminated\nprint must be 0 or 1" << endl;
		exit(0); 
	}
}

string color(int r, int g, int b) {
	r = max(0, r); g = max(0, g); b = max(0, b); 
	string c = to_string(r) + " " + to_string(g) +  " " + to_string(b) + "\t";
	return c;
}

void Mandelbrot::printPlain() {
	ofstream map("map.ppm"); 
	map << "P3\n" << imageWidth << " " << imageHeight << "\n" << maxColor << "\n";
	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			if (image[index] == 1) map << color(0, 0, 0); 
			else if (image[index] == 0) map << color(maxColor, maxColor, maxColor); 
			else map << color(80, 0, 0); 
		}
		progressBar(i, imageHeight, progBarWidth); 
	}
	map.close();
}

double* Mandelbrot::makeColorFrac() {
	double *colorFrac = new double[maxIter]; 
	colorFrac[0] = hist[0]; 
	double max = hist[0]; 

	for (int i=1; i<maxIter; i++) {
		colorFrac[i] = colorFrac[i-1] + hist[i]; 
		if (colorFrac[i] > max) max = colorFrac[i]; 
	}

	for (int i=0; i<maxIter; i++) {
		colorFrac[i] /= max; 
	}

	return colorFrac; 
}

void Mandelbrot::printHistogram() {
	ofstream map("map.ppm"); 
	map << "P3\n" << imageWidth << " " << imageHeight << "\n" << maxColor << "\n"; 
	// double *colorFrac = new double[maxIter]; 
	// colorFrac[0] = hist[0]; 
	// double max = hist[0]; 

	// for (int i=1; i<maxIter; i++) {
	// 	colorFrac[i] = colorFrac[i-1] + hist[i]; 
	// 	if (colorFrac[i] > max) max = colorFrac[i]; 
	// }

	// for (int i=0; i<maxIter; i++) {
	// 	colorFrac[i] /= max; 
	// }

	// printArray(colorFrac, maxIter, 1, "colorFrac"); '

	double * colorFrac = makeColorFrac(); 

	// int inFact = 50; 

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			int k = iter[index]; 
			if (k > maxIter) cout << "k = " << k << endl;
			double cf = colorFrac[k]; 
			// double cf = exp(iter[index]/maxIter); 
			// if (image[index] == 1) map << color(max(cR-inFact, 0), max(cG-inFact, 0), 
			// 								max(cB-inFact, 0)); 
			// else if (image[index] == 0) {
			// 	map << color(cR*cf, cG*cf, cB*cf); 
			// } else {
			// 	cout << "image array corrupted\nending program" << endl;
			// 	exit(0); 
			// }
			// map << color(cR*cf, cG*cf, cB*cf); 
			// map << color(cR*k/maxIter, cG*k/maxIter, cB*k/maxIter); 
			map << color(cR*cf, cG*cf, cB*cf); 

		}
		progressBar(i, imageHeight, progBarWidth); 
	}
	map.close(); 
}

double * Mandelbrot::makeSmooth() {
	double * smoothFrac = new double[imageWidth*imageHeight]; 
	double max = smooth[0]; 

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			if (smooth[index] > max) max = smooth[index]; 
		}
	}

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			smoothFrac[index] = pow(smooth[index]/max, .4); 
		}
	}

	// printArray(smoothFrac, imageHeight, imageWidth, "smoothFrac"); 

	return smoothFrac; 
}

void Mandelbrot::printSmooth() {
	ofstream map("map.ppm"); 
	map << "P3\n" << imageWidth << " " << imageHeight << "\n" << maxColor << "\n"; 
	double smoothVal;  
	// for (int i=0; i<imageHeight; i++) {
	// 	for (int j=0; j<imageWidth; j++) {
	// 		int index = i*imageWidth + j; 
	// 		smoothVal = pow(smooth[index]/maxIter,.4); 
	// 		map << color(cR*smoothVal, cG*smoothVal, cB*smoothVal); 
	// 	}
	// 	progressBar(i, imageHeight, progBarWidth); 
	// }

	double * smoothFrac = makeSmooth(); 

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			smoothVal = smoothFrac[index]; 
			map << color(cR*smoothVal, cG*smoothVal, cB*smoothVal); 
		}
		progressBar(i, imageHeight, progBarWidth); 
	}
	map.close(); 
}

void Mandelbrot::printCombined() {
	ofstream map("map.ppm"); 
	map << "P3\n" << imageWidth << " " << imageHeight << "\n" << maxColor << "\n";  
	// double *colorFrac = new double[maxIter];  
	// colorFrac[0] = hist[0]; 

	// for (int i=1; i<maxIter; i++) {
	// 	colorFrac[i] = colorFrac[i-1] + hist[i]; 
	// }

	// for (int i=0; i<maxIter; i++) {
	// 	colorFrac[i] /= (imageWidth*imageHeight); 
	// }

	double * colorFrac = makeColorFrac(); 
	double * smoothFrac = makeSmooth(); 

	double smoothVal, histVal; 
	// for (int i=0; i<imageHeight; i++) {
	// 	for (int j=0; j<imageWidth; j++) {
	// 		int index = i*imageWidth + j; 
	// 		smoothVal = pow(smooth[index]/maxIter, .4); 
	// 		histVal = colorFrac[iter[index]]; 
	// 		map << color(cR*smoothVal, cG*histVal, cB*histVal); 
	// 	}
	// 	progressBar(i, imageHeight, 50); 
	// }

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			smoothVal = smoothFrac[index]; 
			histVal = colorFrac[iter[index]]; 

			map << color(cR*smoothVal, cG*histVal, cB*histVal); 
		}
		progressBar(i, imageHeight, progBarWidth); 
	}
	map.close(); 
}

void Mandelbrot::printCombined(string fname) {
	ofstream map(fname); 
	map << "P3\n" << imageWidth << " " << imageHeight << "\n" << maxColor << "\n";  
	// double *colorFrac = new double[maxIter];  
	// colorFrac[0] = hist[0]; 

	// for (int i=1; i<maxIter; i++) {
	// 	colorFrac[i] = colorFrac[i-1] + hist[i]; 
	// }

	// for (int i=0; i<maxIter; i++) {
	// 	colorFrac[i] /= (imageWidth*imageHeight); 
	// }

	double * colorFrac = makeColorFrac(); 
	double * smoothFrac = makeSmooth(); 

	double smoothVal, histVal; 
	// for (int i=0; i<imageHeight; i++) {
	// 	for (int j=0; j<imageWidth; j++) {
	// 		int index = i*imageWidth + j; 
	// 		smoothVal = pow(smooth[index]/maxIter, .4); 
	// 		histVal = colorFrac[iter[index]]; 
	// 		map << color(cR*smoothVal, cG*histVal, cB*histVal); 
	// 	}
	// 	progressBar(i, imageHeight, 50); 
	// }

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			smoothVal = smoothFrac[index]; 
			histVal = colorFrac[iter[index]]; 

			map << color(cR*smoothVal, cG*histVal, cB*histVal); 
		}
		progressBar(i, imageHeight, progBarWidth); 
	}
	map.close(); 
}

void Mandelbrot::printIter() {
	ofstream map("map.ppm"); 
	map << "P3\n" << imageWidth << " " << imageHeight << "\n" << maxColor << "\n"; 

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			double cf = iter[index]/maxIter; 

			map << color(cR*cf, cG, cB); 
		}
	}
}

void Mandelbrot::printSet() {
	if (colorBool == 0) {
		cout << "printing plain image to file" << endl;
		printPlain(); 
	}
	else if (colorBool == 1) {
		cout << "printing histogram image to file" << endl;
		printHistogram(); 
	}
	else if (colorBool == 2) {
		cout << "printing smooth image to file" << endl;
		printSmooth(); 
	}
	else if (colorBool == 3) {
		cout << "printing combined image to file" << endl;
		printCombined(); 
	}
}

void Mandelbrot::printData() {
	cout << "Printing data to file" << endl;

	printArray(image, imageHeight, imageWidth, "Data/image"); 
	printArray(iter, imageHeight, imageWidth, "Data/iter"); 
	printArray(smooth, imageHeight, imageWidth, "Data/smooth"); 
	printArray(hist, maxIter, 1, "Data/hist"); 

	ofstream out("Data/deck"); 
	out << "imageWidth\t" << imageWidth << endl;
	out << "imageHeight\t" << imageHeight << endl;
	out << "maxIter\t" << maxIter << endl;
	out.close(); 
}

void Mandelbrot::printHDF5Array(double * array, hid_t file_id, string fn) {
	hid_t dataset_id, dataspace_id; 
	hsize_t  dims[2];
	dims[0] = imageHeight; 
	dims[1] = imageWidth; 
	dataspace_id = H5Screate_simple (2, dims , NULL);

	dataset_id = H5Dcreate(file_id , fn.c_str(), H5T_IEEE_F64LE ,
		dataspace_id , H5P_DEFAULT ,
		H5P_DEFAULT , H5P_DEFAULT );
	H5Dwrite(dataset_id , H5T_NATIVE_DOUBLE ,
		H5S_ALL , H5S_ALL , H5P_DEFAULT , array);

	H5Dclose(dataset_id );
	H5Sclose(dataspace_id );
}

void Mandelbrot::printHDF5Array(int * array, hid_t file_id, string fn) {
	hid_t dataset_id, dataspace_id; 
	hsize_t  dims[2];
	dims[0] = imageHeight; 
	dims[1] = imageWidth; 
	dataspace_id = H5Screate_simple (2, dims , NULL);

	dataset_id = H5Dcreate(file_id , fn.c_str(), H5T_IEEE_F64LE ,
		dataspace_id , H5P_DEFAULT ,
		H5P_DEFAULT , H5P_DEFAULT );
	H5Dwrite(dataset_id , H5T_NATIVE_INT ,
		H5S_ALL , H5S_ALL , H5P_DEFAULT , array);

	H5Dclose(dataset_id );
	H5Sclose(dataspace_id );
}

void Mandelbrot::dumpHDF5(string fn) {
	const char* fname = fn.c_str(); 
	hid_t  file_id; 

	file_id = H5Fcreate(fname , H5F_ACC_TRUNC ,
		H5P_DEFAULT , H5P_DEFAULT );

	double * colorFrac = makeColorFrac(); 
	double * smoothFrac = makeSmooth(); 

	double * array = new double[imageWidth*imageHeight]; 

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			int index = i*imageWidth + j; 
			array[index] = colorFrac[iter[index]]; 
		}
	}

	printHDF5Array(array, file_id, "cf"); 
	printHDF5Array(smoothFrac, file_id, "smooth"); 
	printHDF5Array(iter, file_id, "iter"); 
	printHDF5Array(image, file_id, "image"); 
	H5Fclose(file_id );
}