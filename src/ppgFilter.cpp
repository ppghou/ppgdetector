#include "ppgFilter.h"
#include "utils.h"

void Filter::calc(){
    arma::vec x(windowSize, arma::fill::zeros);  // Input sig
    arma::vec y(windowSize, arma::fill::zeros);  // Output sig
    arma::vec xCen(windowSize, arma::fill::zeros);  // intermediate sig
    arma::vec yFilt(windowSize, arma::fill::zeros);  // Filtered sig for segmentation

    for (int i = 0; i < windowSize; ++i)
        x[i] = window[i];

    // Filtering
    double meanSig = mean(x);
    xCen = x - meanSig;
    y = G1.filter(xCen) + meanSig;
    yFilt = G2.filter(y);

    // Find peaks in yFilt
    std::vector<int> peakPosFilt;
    peakPosFilt = findPeaks(yFilt, windowSize);

    // Find peaks in y (spanned)
    std::vector<double> peak;
    std::vector<int> peakPos;

    int pos = 0;
    for (int i = 0; i < peakPosFilt.size(); ++i) {
        int posFilt = peakPosFilt[i] - groupDelay2;
        if (posFilt < 0)
            continue;

        double temp = -1000;
        for (int j = max(0, posFilt - findRange);
            j < min(windowSize, posFilt + findRange); ++j)
            if (y[j] > temp) {
                temp = y[j];
                pos = j;
            }

        pos = pos - groupDelay1;
        if (pos >= 0) {
            peak.push_back(temp);
            peakPos.push_back(pos);
        }
    }

    // Update signal, peak and peakPos
    update(y, peakPos, peak);

#if GNUPLOT
    gp0.window("Plot", 10, 10, 500, 500);
    gp1.window("Plot", 510, 10, 500, 500);

    gp0.plot_add(x, "x");

    arma::vec ySpan(windowSize - groupDelay1, arma::fill::zeros);
    for (int i = 0; i < windowSize - groupDelay1; ++i) {
        ySpan[i] = y[i + groupDelay1];
    }

    gp0.plot_add(ySpan, "y");

    arma::vec plotX(peakPos.size(), arma::fill::zeros);
    arma::vec plotY(peakPos.size(), arma::fill::zeros);
    for (int i = 0; i < peakPos.size(); ++i) {
        plotX[i] = peakPos[i];
        plotY[i] = peak[i];
    }

    gp0.plot_add(plotX, plotY, "peak");
    gp0.plot_show();

    arma::vec signal(this->signal.size(), arma::fill::zeros);
    arma::vec plotX2(this->peakPos.size(), arma::fill::zeros);
    arma::vec plotY2(this->peakPos.size(), arma::fill::zeros);
    for (int i = 0; i < this->signal.size(); ++i) {
        signal[i] = this->signal[i];
    }
    for (int i = 0; i < this->peakPos.size(); ++i) {
        plotX2[i] = this->peakPos[i];
        plotY2[i] = this->peak[i];
    }
    gp1.plot_add(signal, "signal");
    gp1.plot_add(plotX2, plotY2, "peak");
    gp1.plot_show();
#endif
}

std::vector<int> Filter::findPeaks(arma::vec num, int count){
    std::vector<int> sign;
    for (int i = 1; i < count; i++) {
        double diff = num[i] - num[i - 1];
        sign.push_back(diff > 0 ? 1 : (diff < 0 ? -1 : 0));
    }
    std::vector<int> indMax;
    std::vector<int> indMin;

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

void Filter::update(arma::vec y, std::vector<int> peakPosWin, std::vector<double> peakWin){
    // y - delayed by groupDelay1, peakPos - not delayed

    // Update all 
    int start = signal.size() - (raw.size() - windowSize - groupDelay1);
    for (int i = start; i < windowSize; ++i) {
        signal.push_back(y[i]);
    }

    // Discard the last peakPos,
    // Update all peaks after the second last one
    pop_last(peakPos);
    pop_last(peak);
    pop_last(frequency);

    for (int i = 0; i < peakPosWin.size(); ++i){
        int pos = peakPosWin[i] + (raw.size() - windowSize);
        if (peakPos.empty() || pos > peakPos.back()) {
            double fre;
            if(peakPos.empty()){
                peakPos.push_back(pos);
                peak.push_back(peakWin[i]);
            }else{
                fre = 60 * fs / (pos - peakPos.back());
                if(fre > 120) continue;
                frequency.push_back(fre);
                peakPos.push_back(pos);
                peak.push_back(peakWin[i]);
            }
        }
    }

    // Save signal and peakpos for output
    for (int i = rankSigOut; i < peakPos[peakPos.size() - 1]; ++i)
        signalTmp.push_back(signal[i]);
    rankSigOut = peakPos[peakPos.size() - 1];

    for (int i = rankPeakPosOut; i < peakPos.size() - 1; ++i)
        peakPosTmp.push_back(peakPos[i]);
    rankPeakPosOut = peakPos.size() - 1;

    for (int i = frequencyPosOut; i < frequency.size() - 1; ++i)
        frequencyTmp.push_back(frequency[i]);
    frequencyPosOut = frequency.size() - 1;

    outputFlag = true;
}

Filter::Filter(double samplingFrequency){
    initialize(samplingFrequency);
}

void Filter::initialize(double samplingFrequency){
    windowSize = samplingFrequency*5;
    findRange = 15;
    tUpdate = 0.1;
    
    outputFlag = false;
    rankSigOut = 0;
    rankPeakPosOut = 0;
    frequencyPosOut = 0;

    fs = samplingFrequency;

    int M = 20;      // filter order
    double f0 = 0.2; // low cutoff frequency
    double f1 = 5.0; // high cutoff frequency
    arma::vec b = fir1_bp(M, f0 / samplingFrequency, f1 / samplingFrequency);
    G1.set_coeffs(b);
    groupDelay1 = 10;

    M = 4;
    f0 = 0.04;
    b = fir1(M, f0 / samplingFrequency);
    G2.set_coeffs(b);
    groupDelay2 = 0;

    clearVector(raw);
    clearVector(signal);
    clearVector(signalTmp);
    clearVector(window);
    clearVector(peak);
    clearVector(peakPos);
    clearVector(peakPosTmp);
}

void Filter::filterInput(double data){
    raw.push_back(data);
    window.push_back(data);
    if (window.size() < windowSize)
        return;

    calc();
    window.erase(window.begin(), window.begin() + int(tUpdate * fs));
}

bool Filter::getFlag() {
    return outputFlag;
}

std::vector<double> Filter::getSignal(){
    return signalTmp;
}

std::vector<int> Filter::getPeakPos(){
    return peakPosTmp;
}

std::vector<double> Filter::getFrequency(){
    return frequencyTmp;
}

void Filter::clearTmpData(){
    clearVector(signalTmp);
    clearVector(peakPosTmp);
    clearVector(frequencyTmp);
    outputFlag = false;
    return;
}

// int main(){
//  vector<float> time;
//  vector<float> data;
//  loadCSV("data_1031_0399_tRGB.csv", time, data);
// 
//  Filter filt(50);
//  for (int i = 0; i < 2800; ++i){
//      filt.filterInput(data[i + 10]);
//  }
//  
//  std::vector<double> signal = filt.getSignal();
//  freopen("output_signal.txt", "w", stdout);
//  for (int i = 0; i < signal.size(); ++i)
//      cout << signal[i] << endl;
// 
//  freopen("output_peakPos.txt", "w", stdout);
//  std::vector<int> peakPos = filt.getPeakPos();
//  for (int i = 0; i < peakPos.size(); ++i)
//      cout << peakPos[i] << endl;
//  
//  fclose(stdout);
//  return 0;
// }
