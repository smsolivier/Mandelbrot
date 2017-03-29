#include <iostream>
#include <fstream>

using namespace std; 

void writeHeader(int imageWidth, int imageHeight, int maxColor, ofstream &map) {
    map << "P3\n" << imageWidth << " " << imageHeight << "\n" << maxColor << "\n"; 
}

void color(int r, int g, int b, ofstream &map) {
	r = max(0, r); g = max(0, g); b = max(0, b); 
	string c = to_string(r) + " " + to_string(g) +  " " + to_string(b) + "\t";
	map << c; 
}
int main() {
	ofstream map("box.ppm");

	int imageHeight = 256; 
	int imageWidth = 256; 

	writeHeader(imageWidth, imageHeight, 256, map); 

	for (int i=0; i<imageHeight; i++) {
		for (int j=0; j<imageWidth; j++) {
			color(i, j, 0, map); 
		}
	}

	map.close();

	return 0; 
}