#include <iostream>
#include <ctime>
#include <string>
#include <iomanip>

using namespace std; 

void progressBar(int j, double max, double width) {
	double percent = (double)j/max;

	cout << "["; 
	int pos = width * percent; 
	for (int i=0; i<width; i++) {
		if (i < pos) cout << "="; 
		else if (i == pos) cout << ">"; 
		else cout << " "; 
	}

	if (j == int(max) -1) cout << "] " << "100%" << endl;
	else cout << "] " << int(percent * 100) << "%\r";  
	cout.flush(); 
}

void progressBar(int j, double max, double start, double end, double width) {
	double percent = (double)j/max; 

	// cout << string((int)width+50, ' '); 
	// cout << "\r"; 

	if (j >= 2) {

		double elapsed = (end - start); 

		int est = elapsed/percent - elapsed; 
		string unit = " seconds"; 

		if (est >= 60) {
			est /= 60; 
			unit = " minutes"; 
		} else if (est >= 3600) {
			est /= 3600; 
			unit = " hours"; 
		}

		cout << "["; 
		int pos = width * percent; 
		for (int i=0; i<width; i++) {
			if (i < pos) cout << "="; 
			else if (i == pos) cout << ">"; 
			else cout << " "; 
		}

		if (j == int(max) -1) {
			if (elapsed >= 60) {
				elapsed /= 60; 
				unit = " minutes"; 
			} else if (elapsed >= 3600) {
				elapsed /= 3600; 
				unit = " hours"; 
			} else unit = " seconds"; 
			cout << "] " << "100% " << elapsed << unit << endl;
		}
		else cout << "] " << int(percent * 100) << "% " << est << unit << "\r";  
		cout.flush(); 
	} else {
		cout << "["; 
		int pos = width * percent; 
		for (int i=0; i<width; i++) {
			if (i < pos) cout << "="; 
			else if (i == pos) cout << ">"; 
			else cout << " "; 
		}

		if (j == int(max) -1) cout << "] " << "100%" << endl;
		else cout << "] " << int(percent * 100) << "%\r";  
		cout.flush(); 
	}
}