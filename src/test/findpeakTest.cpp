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

	cout << endl << "Special Test1:";
	arma::vec specialSignal(videofre*signallength, arma::fill::ones);

	bool test1 = findPeaks_Test(specialSignal, videofre*signallength, 0);
	if (test1 == 1) {
		cout << " pass!" << endl;
		pass++;
	}
	else {
		cout << " fail!" << endl;
		fail++;
	}

	for (int i = videofre * signallength / 2; i < videofre*signallength; i++) {
		specialSignal[i] += i;
	}
	cout << "Special Test2:";
	bool test2 = findPeaks_Test(specialSignal, videofre*signallength, 0);
	if (test2 == 1) {
		cout << " pass!" << endl;
		pass++;
	}
	else {
		cout << " fail!" << endl;
		fail++;
	}

	for (int i = 0; i < videofre*signallength/2; i++) {
		specialSignal[i] += videofre*signallength/2-i;
	}
	cout << "Special Test3:";
	bool test3 = findPeaks_Test(specialSignal, videofre*signallength, 0);
	if (test3 == 1) {
		cout << " pass!" << endl;
		pass++;
	}
	else {
		cout << " fail!" << endl;
		fail++;
	}
	
	cout << "----------" << endl;
	cout << "Total: " << pass << "/" << total+3 << " passed!" << endl;
	
	system("pause");
	return 0;
}
