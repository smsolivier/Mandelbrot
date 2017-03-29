#include "ompMand.h"

int main(int argc, char* argv[]) {
	string deckName = argv[1]; 
	int resolution = atoi(argv[2]);
	int printBool = atoi(argv[3]); 

	OMPMand m(deckName, resolution, printBool); 
	m.start(); 
}