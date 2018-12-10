#include <itpp/itbase.h>

using namespace itpp;
using namespace std;

int main() {
  vec a = linspace(0,1,11);
  ivec index_list = "3 5 2 2";
  bvec bin_list = "1 0 1 0 1 0 1 0 1 0 1";
  cout << "a = " << a << endl;
  cout << "a(index_list) = " << a(index_list) << endl;
  cout << "a.get(bin_list) = " << a.get(bin_list) << endl;
}