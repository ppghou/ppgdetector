#include <string>
#include <vector>
#include <armadillo>
#include <iostream>
#include <math.h>
#include <random>

using namespace std;

#define PI 3.14159265
#define videofre 50
#define signallength 10

vector<int> findPeaks(vector<float> num, int count)
{
	vector<int> sign;
	for (int i = 1; i < count; i++)
	{
		float diff = num[i] - num[i - 1];
		sign.push_back(diff>0 ? 1 : (diff<0 ? -1 : 0));
	}
	vector<int> indMax;
	vector<int> indMin;

	for (int j = 1; j < sign.size(); j++)
	{
		if (sign[j] == 0) {
			if (sign[j - 1] > 0, sign[j + 1] < 0)
			{
				indMax.push_back(j);
				j++;
			}
			else if (sign[j - 1] < 0, sign[j + 1] > 0)
			{
				indMin.push_back(j);
				j++;
			}
			else
			{
				j++;
			}
			continue;
		}
		int diff = sign[j] - sign[j - 1];
		if (diff < 0)
			indMax.push_back(j);
		else if (diff > 0)
			indMin.push_back(j);
	}
	return indMax;
}

vector<float> signalGenerator(float fre)
{
	vector<float> signal;
	for (int i = 0; i < videofre*signallength; i++)
	{
		signal.push_back(sin(2 * PI * fre *i / videofre));
		//cout << signal[i] << endl;
	}
	return signal;
}

bool findPeaks_Test(vector<float> signal, int count, float fre)
{
	vector<int> indMax(findPeaks(signal, count));
	vector<int> signal_period;
	for (int i = 1; i < indMax.size(); i++)
	{
		signal_period.push_back(indMax[i] - indMax[i - 1]);
		cout << indMax[i] - indMax[i - 1] << endl;
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
	std::default_random_engine e;
	e.seed(time(0));
	uniform_real_distribution<float> u(1, 2);
	for (int i = 0; i < 1; i++)
	{
		fre = u(e);
		cout << fre << endl;
		vector<float> testSignal(signalGenerator(1));
		bool test = findPeaks_Test(testSignal, videofre*signallength, 1);
		std::cout << test << endl;
	}
	system("pause");
	return 0;
}
