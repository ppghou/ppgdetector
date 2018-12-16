#include "sigpack.h"
#include <string>
#include <vector>

using namespace std;
using namespace sp;

#define min(a,b) ((a)>(b) ? b:a)
#define max(a,b) ((a)<(b) ? b:a)
#define GNUPLOT 1

class Filter{
private:
    double fs;
    double tDelay;
    double tUpdate;
    
    FIR_filt<double, double, double> G1, G2;
    int groupDelay1, groupDelay2;
    int windowSize;
    vector<double> raw, window;
    vector<double> signal, peak;
    vector<int> peakPos;
    int findRange;

    #if GNUPLOT
    gplot gp0, gp1;
    #endif

    void calc(){
        arma::vec x(windowSize, arma::fill::zeros);  // Input sig
        arma::vec xCen(windowSize, arma::fill::zeros);  // intermediate sig
        arma::vec y(windowSize, arma::fill::zeros);  // Output sig
        arma::vec yFilt(windowSize, arma::fill::zeros);  // Filtered sig for segmentation

        for (int i = 0; i < windowSize; ++i)
            x[i] = window[i];

        double meanSig = mean(x);

        xCen = x - meanSig;
        y = G1.filter(xCen) + meanSig;
        yFilt = G2.filter(y);


        vector<int> peakPosFilt;
        peakPosFilt = findPeaks(yFilt, windowSize);  // Position of peaks in yFilt

        vector<double> peak;
        vector<int> peakPos;
        int pos = 0;
        for (int i = 0; i < peakPosFilt.size(); ++i){
            int posFilt = peakPosFilt[i] - groupDelay2;
            if (posFilt < 0)
                continue;
        
            double temp = -1000;
            for (int j = max(0, posFilt - findRange); 
                j < min(windowSize, posFilt + findRange); ++j)
                if (y[j] > temp){
                    temp = y[j];
                    pos = j;
                }

            pos = pos - groupDelay1;
            if (pos >= 0){
                peak.push_back(temp);
                peakPos.push_back(pos);
            }
        }

        update(y, peakPos, peak);

        #if GNUPLOT
        gp0.window("Plot", 10, 10, 500, 500);
        gp1.window("Plot", 510, 10, 500, 500);

        gp0.plot_add(x, "x");

        arma::vec ySpan(windowSize - groupDelay1, arma::fill::zeros);
        for (int i = 0; i < windowSize - groupDelay1; ++i){
            ySpan[i] = y[i + groupDelay1];
        }

        gp0.plot_add(ySpan, "y");

        arma::vec plotX(peakPos.size(), arma::fill::zeros);
        arma::vec plotY(peakPos.size(), arma::fill::zeros);
        for(int i=0; i<peakPos.size(); ++i){
            plotX[i] = peakPos[i];
            plotY[i] = peak[i];
        }

        gp0.plot_add(plotX, plotY, "peak");
        gp0.plot_show();

        arma::vec signal(this->signal.size(), arma::fill::zeros);
        arma::vec plotX2(this->peakPos.size(), arma::fill::zeros);
        arma::vec plotY2(this->peakPos.size(), arma::fill::zeros);
        for(int i=0; i<this->signal.size(); ++i){
            signal[i] = this->signal[i];
        }
        for(int i=0; i<this->peakPos.size(); ++i){
            plotX2[i] = this->peakPos[i];
            plotY2[i] = this->peak[i];
        }
        gp1.plot_add(signal, "signal");
        gp1.plot_add(plotX2, plotY2, "peak");
        gp1.plot_show();

        // int a;
        // cin >> a;
        #endif
    }

    vector<int> findPeaks(arma::vec num, int count){
        vector<int> sign;
        for (int i = 1; i < count; i++){
            float diff = num[i] - num[i - 1];
            sign.push_back(diff>0 ? 1 :(diff<0 ? -1 : 0));
        }
        vector<int> indMax;
        vector<int> indMin;

        for (int j = 1; j < sign.size(); j++)
        {
            int diff = sign[j] - sign[j - 1];
            if (diff < 0)
                indMax.push_back(j);
            else if (diff > 0)
                indMin.push_back(j);
        }

        return indMax;
    }

    void update(arma::vec y, vector<int> peakPosWin, vector<double> peakWin){ 
        // y - delayed by groupDelay1, peakPos - not delayed
        int start = signal.size() - (raw.size() - windowSize - groupDelay1);
        for (int i = start; i < windowSize; ++i){
            signal.push_back(y[i]);
        }

        pop_last(peakPos);
        pop_last(peak);
        for (int i = 0; i < peakPosWin.size(); ++i){
            int pos = peakPosWin[i] + (raw.size() - windowSize);
            if (peakPos.empty() || pos > peakPos.back()){
                peakPos.push_back(pos);
                peak.push_back(peakWin[i]);
            }
        }
    }

public:
    Filter(double samplingFrequency){
        initialize(samplingFrequency);
    }

    void initialize(double samplingFrequency){
        windowSize = 200;
        findRange = 15;
        tUpdate = 1.;
        tDelay = 5.;

        fs = samplingFrequency;

        int M = 20;      // filter order
        double f0 = 0.2; // low cutoff frequency
        double f1 = 5.0; // high cutoff frequency
        arma::vec b = fir1_bp(M, f0 / windowSize, f1 / windowSize);
        G1.set_coeffs(b);
        groupDelay1 = 10;

        M = 4;
        f0 = 0.04;
        b = fir1(M, f0 / windowSize);
        G2.set_coeffs(b);
        groupDelay2 = 0;

        clearVector(raw);
        clearVector(signal);
        clearVector(window);
        clearVector(peakPos);
    }

    void input(double data){
        raw.push_back(data);
        window.push_back(data);
        if (window.size() < windowSize)
            return;

        calc();
        window.erase(window.begin(), window.begin() + int(tUpdate * fs));
    }
};

int main(){
    vector<float> time;
    vector<float> data;
    loadCSV("../data/data_1031_0399_tRGB.csv", time, data);

    Filter filt(50);
    for (int i = 0; i < 2800; ++i){
        filt.input(data[i+10]);
    }

    return 0;
}
