#include <cmath>
namespace epi {

double logn(double x, double s, double m, double k) {
    if (x == 0) {
        return 0;
    }
    double m1 = 1.0 / (x * s * sqrt(2 * M_PI));
    double t = -pow(log(x) - m, 2) / pow(2 * s, 2);
    return k * m1 * exp(t);
}

}
