#ifndef __FILTER__
#define __FILTER__

#include "sigpack.h"
#include "utils.h"
#include <string>
#include <vector>

//using namespace std;
using namespace sp;

#define min(a,b) ((a)>(b) ? b:a)
#define max(a,b) ((a)<(b) ? b:a)
#define GNUPLOT 0

class Filter
{
private:
    // Sampling frequency
    double fs;

    // Time interval between updates
    double tUpdate;
    
    // Filter and parameters
    FIR_filt<double, double, double> G1, G2;
    int groupDelay1, groupDelay2;

    // Signals
    std::vector<double> raw, window;
    std::vector<double> peak, signal, signalTmp;
    std::vector<int> peakPos, peakPosTmp;
    std::vector<double> frequency, frequencyTmp;

    int windowSize;
    int findRange;

    // Output parameters
    bool outputFlag;
    int rankSigOut;
    int rankPeakPosOut;
    int frequencyPosOut;

    #if GNUPLOT
    gplot gp0, gp1;
    #endif

    void calc();

    void update(arma::vec y, std::vector<int> peakPosWin, std::vector<double> peakWin);

public:
    std::vector<int> findPeaks(arma::vec num, int count);

    Filter(double samplingFrequency);

    void initialize(double samplingFrequency);

    void filterInput(double data);

    bool getFlag();

    std::vector<double> getSignal();
    
    std::vector<int> getPeakPos();

    std::vector<double> getFrequency();

    void clearTmpData();
};

#endif
