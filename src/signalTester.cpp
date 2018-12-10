#include "sigpack.h"
#include <string>

using namespace std;
using namespace sp;

inline int loadCSV(const char* fileName, vector<float> &time, vector<float> &data);

int main()
{
    vector<float> time;
    vector<float> data;
    loadCSV("../data/data_1031_0399_tRGB.csv", time, data);

    // Number of samples
    int N = data.size();

    // Create a FIR filter and adaptive
    FIR_filt<double,double,double> G;
    FIR_filt<double,double,double> Ghat;

    // Filter coeffs.
    arma::vec b="-0.2 -0.1 0.1 0.3 0.7";
    G.set_coeffs(b);
    int M = b.size();

    // Setup adaptive filter
    Ghat.setup_lms(M, 0.95);

    // Signal vectors
    arma::vec x(N, arma::fill::zeros);  // Input sig
    arma::vec y(N, arma::fill::zeros);  // Model sig
    arma::vec d(N, arma::fill::zeros);  // Output sig
    // arma::vec z(N, arma::fill::randn);  // Measurement noise
    arma::vec e(N, arma::fill::zeros);  // Err sig

    for(int i=0; i<N; i++) x[i] = data[i];

    // Log matrix
    arma::mat Wlog(M,N);

    // Apply G to input signal and add some measurement noise
    d = G.filter(x);

    // Display result
    cout << "Filter coeffs:    " << b.t() << endl;
    cout << "Estimated coeffs: " << Ghat.get_coeffs().t() << endl;
    gplot gp0;
    gp0.window("Plot", 10, 10, 500, 500);
    gp0.plot_add(d, "b");
    gp0.plot_show();
    return 0;
}

inline int loadCSV(const char* fileName, vector<float> &time, vector<float> &data){
    float t, R, G, B;
    string line; char c;
    ifstream file(fileName);
    while(getline(file, line)){
        stringstream strm(line);
        strm >> t >> c >> R >> c >> G >> c >> B;
        time.push_back(t);
        data.push_back(G);
    }
    file.close();
}