#include "sigpack.h"

using namespace std;
using namespace sp;

int main()
{
    // Number of samples
    int N = 200;

    // Create a FIR filter and adaptive
    FIR_filt<double,double,double> G;
    FIR_filt<double,double,double> Ghat;

    // Filter coeffs.
    arma::vec b="-0.2 -0.1 0.1 0.3 0.7";
    G.set_coeffs(b);
    int M = b.size();

    // Setup adaptive filter
//  Ghat.setup_rls(M, 0.95, 50);
//  Ghat.setup_nlms(M,0.5,0.001);
    Ghat.setup_lms(M,0.05);

    // Signal vectors
    arma::vec x(N,arma::fill::randn);  // Input sig
    arma::vec y(N,arma::fill::zeros);  // Model sig
    arma::vec d(N,arma::fill::zeros);  // Output sig
    arma::vec z(N,arma::fill::randn);  // Measurement noise
    arma::vec e(N,arma::fill::zeros);  // Err sig

    // Log matrix
    arma::mat Wlog(M,N);

    // Apply G to input signal and add some measurement noise
    d = G.filter(x)+0.0001*z;

    // Filter - sample loop
    for(int n=0;n<N;n++)
    {
        // Apply adaptiv filter
        y(n) = Ghat(x(n));

        // Calc error
        e(n) = d(n)-y(n);

        // Update filter
//      Ghat.rls_adapt(e(n));
//      Ghat.nlms_adapt(e(n));
        Ghat.lms_adapt(e(n));

        // Save to log
        Wlog.col(n) = Ghat.get_coeffs();
    }

    // Display result
    cout << "Filter coeffs:    " << b.t() << endl;
    cout << "Estimated coeffs: " << Ghat.get_coeffs().t() << endl;
    gplot gp0,gp1;
    gp0.window("Plot", 10, 10, 500, 500);
//    gp0.set_output("Wlog.png");
    gp0.plot_add_mat(Wlog,"b");
    gp0.plot_show();

    gp1.window("Plot2", 600, 10, 500, 500);
//    gp1.set_output("LMS_err.png");
    arma::vec J = 10*log10(e%e);
    gp1.plot_add(J,"|Error|^2");
    gp1.plot_show();

    return 1;
}
