#include <string>
#include <vector>
#include <armadillo>
#include <iostream>
#include <math.h>
#include <random>
#include "ppgFilter.h"

using namespace std;

#define PI 3.14159265
#define videofre 50
#define signallength 10

arma::vec signalGenerator(float fre)
{
	vector<float> signal;
	arma::vec transSignal(videofre*signallength, arma::fill::zeros);
	for (int i = 0; i < videofre*signallength; i++)
	{
		transSignal[i] = (sin(2 * PI * fre *i / videofre));
	}
	
	return transSignal;
}

bool findPeaks_Test(arma::vec signal, int count, float fre)
{
	Filter *testFilter = new Filter(videofre*signallength);

	vector<int> indMax(testFilter->findPeaks(signal, count));
	vector<int> signal_period;
	for (int i = 1; i < indMax.size(); i++)
	{
		signal_period.push_back(indMax[i] - indMax[i - 1]);
	}
	bool result = true;
	for (int j = 1; j < signal_period.size(); j++)
	{
		if (abs((videofre / fre) - signal_period[j]) > 1)
		{
			result = false;
			break;
		}
	}
	return result;
}

int main()
{
	float fre;
	int pass = 0;
	int fail = 0;
	int total = 1000;
	std::default_random_engine e;
	e.seed(time(0));
	uniform_real_distribution<float> u(1, 2);
	for (int i = 0; i < total; i++)
	{
		fre = u(e);

		arma::vec testSignal(signalGenerator(fre));
		bool test = findPeaks_Test(testSignal, videofre*signallength, fre);
		if (test == 1) {
			cout << "Test " << i+1 << " pass!" << endl;
			pass++;
		}
		else {
			cout << "Test " << i + 1 << " fail!" << endl;
			fail++;
		}
	}

	cout << endl;
	arma::vec specialSignal(videofre*signallength, arma::fill::ones);
	cout << "Special Test:";

	bool test = findPeaks_Test(specialSignal, videofre*signallength, 0);
	if (test == 1) {
		cout << " pass!" << endl;
		pass++;
	}
	else {
		cout << "Test fail!" << endl;
		fail++;
	}
	
	cout << "----------" << endl;
	cout << "Total: " << pass << "/" << total+1 << " passed!" << endl;
	
	system("pause");
	return 0;
}
