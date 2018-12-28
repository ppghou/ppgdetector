#ifndef __FILTER__
#define __FILTER__

#include "sigpack.h"
#include "utils.h"
#include <string>
#include <vector>
<<<<<<< HEAD
#include <QObject>
=======
//#include <QObject>
>>>>>>> 8d65bf9dd02952114a8c4a1e39aed0ab305d258a

//using namespace std;
using namespace sp;

#define min(a,b) ((a)>(b) ? b:a)
#define max(a,b) ((a)<(b) ? b:a)
#define GNUPLOT 0

class Filter
{
private:
<<<<<<< HEAD
    // Sampling frequency
    double fs;

    // Time interval between updates
    double tUpdate;
    
    // Filter and parameters
    FIR_filt<double, double, double> G1, G2;
    int groupDelay1, groupDelay2;

    // Signals
=======
	// Sampling frequency
    double fs;

	// Time interval between updates
    double tUpdate;
    
	// Filter and parameters
    FIR_filt<double, double, double> G1, G2;
    int groupDelay1, groupDelay2;

	// Signals
>>>>>>> 8d65bf9dd02952114a8c4a1e39aed0ab305d258a
    std::vector<double> raw, window;
    std::vector<double> peak, signal, signalTmp;
    std::vector<int> peakPos, peakPosTmp;
    std::vector<double> frequency, frequencyTmp;

<<<<<<< HEAD
    int windowSize;
    int findRange;

    // Output parameters
    bool outputFlag;
    int rankSigOut;
    int rankPeakPosOut;
=======
	int windowSize;
    int findRange;

	// Output parameters
	bool outputFlag;
	int rankSigOut;
	int rankPeakPosOut;
>>>>>>> 8d65bf9dd02952114a8c4a1e39aed0ab305d258a
    int frequencyPosOut;

    #if GNUPLOT
    gplot gp0, gp1;
    #endif

<<<<<<< HEAD
    void calc();

    std::vector<int> findPeaks(arma::vec num, int count);

    void update(arma::vec y, std::vector<int> peakPosWin, std::vector<double> peakWin);

public:
    Filter(double samplingFrequency);

    void initialize(double samplingFrequency);

    void filterInput(double data);

    bool getFlag();

    std::vector<double> getSignal();
    
    std::vector<int> getPeakPos();

    std::vector<double> getFrequency();

    void clearTmpData();
};

=======
	void calc();
	
	void update(arma::vec y, std::vector<int> peakPosWin, std::vector<double> peakWin);

public:
	Filter(double samplingFrequency);

	void initialize(double samplingFrequency);

	void filterInput(double data);

	bool getFlag();

	std::vector<double> getSignal();

	std::vector<int> findPeaks(arma::vec num, int count);
	
	std::vector<int> getPeakPos();

    std::vector<double> getFrequency();

	void clearTmpData();
};
>>>>>>> 8d65bf9dd02952114a8c4a1e39aed0ab305d258a
#endif
