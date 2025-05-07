#include "infection.h"

#include "util.h"

#include <cmath>

namespace epi::infect {

static std::function<double(double)> const_infectivity(const double beta) {
    return [beta](double t) { return beta; };
}

double const_infectivity(config cfg, double tau) { return cfg.beta; }

double log_susceptibility(double tau) const {
    if (tau < 0) {
        return 0;
    } else {
        double ex = exp(-cfg.susc_k * (tau - cfg.susc_x0));
        return ex == HUGE_VAL ? 1 : 1 - cfg.susc_l / (1 + ex);
    }
}


static std::vector<double> get_infection_times(const double beta,
                                               const double disease_length,
                                               const InfectivityProfile &profile) {
    std::vector<double> result;

    double t = 0;
    while (t < disease_length) {
        double u = uniform();
        t = t - log(u) / beta;
        if (t < disease_length) {
            double rate = profile.infectivity_function(t);
            double s = uniform();
            if (s < rate / profile.max_function_value) {
                result.push_back(t);
            }
        }
    }
    return result;
}



} // namespace epi::infect