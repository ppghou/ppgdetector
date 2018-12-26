#include <iostream>
#include "sigpack.h"
#include <vector>

using namespace std;
using namespace sp;

inline int loadCSV(const char* fileName, vector<float> &time, vector<float> &data);
void findPeaks(arma::vec num, int count, vector<int> &max);
int test(int &detailsCounts, vector<int> &max, vector<int> &T, int &tempT);

int main() {
	vector<float> time;
	vector<float> data;
	loadCSV("data_1031_0399_tRGB.csv", time, data);
	int N = 500; // Number of samples
	int Fs = 50; // sample frequency
	vector<int> max;
	vector<int> T;
	int tempT, maxCount;
	int detailsCount = 0;
	// Signal vectors
	arma::vec x(N, arma::fill::zeros);  // Input sig
	arma::vec y(N, arma::fill::zeros);  // Output sig

	for (int i = 0; i < N; i++)
		x[i] = data[i + 10] - 115;

	// Create a FIR filter
	FIR_filt<double, double, double> G;

	// Filter coeffs.
	int M = 20;      // filter order
	double f0 = 0.2; // low cutoff frequency
	double f1 = 5.0; // high cutoff frequency
	arma::vec b = fir1_bp(M, .01 / N, 100./N);
	G.set_coeffs(b);

	// Apply G to input signal
	y = G.filter(x);
	findPeaks(y, y.size(), max);
	
	test(detailsCount, max, T, tempT);


	return 0;
}

inline int loadCSV(const char* fileName, vector<float> &time, vector<float> &data) {
	float t, R, G, B;
	string line; char c;
	ifstream file(fileName);
	while (getline(file, line)) {
		stringstream strm(line);
		strm >> t >> c >> R >> c >> G >> c >> B;
		time.push_back(t);
		data.push_back(G);
	}
	file.close();
	return 0;
}

void findPeaks(arma::vec num, int count, vector<int> &max) {
	vector<int> sign;
	for (int i = 1; i < count; i++) {
		float diff = num[i] - num[i - 1];
		sign.push_back(diff > 0 ? 1 : (diff < 0 ? -1 : 0));
	}
	vector<int> indMax;
	vector<int> indMin;

	for (int j = 1; j < sign.size(); j++)
	{
		int diff = sign[j] - sign[j - 1];
		if (diff < 0)
			max.push_back(j);
		else if (diff > 0)
			indMin.push_back(j);
	}
}

int test(int &detailsCount, vector<int> &max, vector<int> &T, int &tempT) {
	for (int i = 0; i < max.size()-1; i++) {
		tempT = max[i + 1] - max[i];
		T.push_back(tempT);
	}
	cout << detailsCount << endl;
	for (int i = 0; i < T.size(); i++) {

		cout << T[i] << endl;
		if (T[i] < 20) {
			detailsCount++;
		}
		else {
			continue;
		}
	}
	cout<<detailsCount<<endl;
	cout<<T.size()<<endl;

	if (detailsCount > 0.1 * T.size()) {
		cout<<"With details!"<<endl;
		system("pause");
		return 1; // With details
	}
	else {
		cout<<"Without details!"<<endl;
		system("pause");
		return 0; // Without details
	}
}