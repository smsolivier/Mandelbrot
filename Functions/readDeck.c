#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

void read(int &x, ifstream &deck) {
	/* 	Reads in values from a deck. Format is variable_name variable_value
		Inputs:
			x:	storage variable for the read value
			deck:	name of ifstream 
		Effects:
			x is overwritten with the value from the deck line
	*/
	string line; 
	getline(deck, line);
	istringstream iss(line);
	string a;
	iss >> a >> x; 
}

void read(string &x, ifstream &deck) {
	/* 	Reads in values from a deck. Format is variable_name variable_value
		Inputs:
			x:	storage variable for the read value
			deck:	name of ifstream 
		Effects:
			x is overwritten with the value from the deck line
	*/
	string line; 
	getline(deck, line);
	istringstream iss(line);
	string a;
	iss >> a >> x; 
}

void read(double &x, ifstream &deck) {
	/* 	Reads in values from a deck. Format is variable_name variable_value
		Inputs:
			x:	storage variable for the read value
			deck:	name of ifstream 
		Effects:
			x is overwritten with the value from the deck line
	*/
	string line; 
	getline(deck, line);
	istringstream iss(line);
	string a;
	iss >> a >> x; 
}