using namespace std; 

void printTime(double elapsed, string s) {

	if (elapsed < 60) {
		cout << s << elapsed << " seconds" << endl;
	} else if (elapsed >= 60) {
		cout << s << elapsed/60 << " minutes" << endl;
	} else if (elapsed >= 3600) {
		cout << s << elapsed/3600 << " hours" << endl;
	}
}