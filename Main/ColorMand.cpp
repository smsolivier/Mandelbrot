#include <iostream> 
#include <fstream> 
#include <sstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <ctime>

#include "../Mandelbrot.h"

using namespace std; 

class ColorMand : public Mandelbrot {
public:
	ColorMand(int type); 
private:
	void getDeck(); 
	void printOpening(); 
	void buildArrays(); 
}; 

ColorMand::ColorMand(int type) {
	colorBool = type; 

	getDeck(); 
	printOpening(); 
	buildArrays(); 
	printSet(); 
}

void ColorMand::getDeck() {
	ifstream deck("Data/deck"); 
	read(imageWidth, deck); 
	read(imageHeight, deck); 
	read(maxIter, deck); 
	deck.close(); 

	ifstream colorDeck("colorDeck"); 
	read(cR, colorDeck); 
	read(cG, colorDeck); 
	read(cB, colorDeck); 
	// read(colorBool, colorDeck); 
	colorDeck.close(); 

	initializeArrays(); 
}

void ColorMand::printOpening() {
	cout << "image width = " << imageWidth << endl;
	cout << "image height = " << imageHeight << endl;
	cout << "max iter = " << maxIter << endl;
	cout << "RGB = " << cR << ", " << cG << ", " << cB << endl;

	printingType(); 
}

void readArray(int *array, int rows, int cols, string fname) {
	/*	Reads in an int array from file into a contiguous array in memory */
	ifstream in(fname); 
	if (in.is_open()) {
		for (int i=0; i<rows; i++) {
			for (int j=0; j<cols; j++) {
				int index = i*cols + j; 
				in >> array[index]; 
			}
		}
	} else {
		cout << "program terminated\ncould not open " << fname << endl;
		exit(0); 
	}
}

void readArray(double *array, int rows, int cols, string fname) {
	/*	Reads in a double array from file into a contiguous array in memory */
	ifstream in(fname); 
	if (in.is_open()) {
		for (int i=0; i<rows; i++) {
			for (int j=0; j<cols; j++) {
				int index = i*cols + j; 
				in >> array[index]; 
			}
		}
	} else {
		cout << "program terminated\ncould not open " << fname << endl;
		exit(0); 
	}
}

void ColorMand::buildArrays() {
	readArray(image, imageHeight, imageWidth, "Data/image"); 
	readArray(iter, imageHeight, imageWidth, "Data/iter"); 
	readArray(smooth, imageHeight, imageWidth, "Data/smooth");  
	readArray(hist, maxIter, 1, "Data/hist"); 
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		cout << "program terminated\nsupply command line arguments" << endl;
		exit(0); 
	}
	ColorMand c(atoi(argv[1])); 
}
