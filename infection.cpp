#include "infection.h"
#include "util.h" // For epi::logn and epi::uniform (though uniform not directly used here, logn is)
#include <cmath>   // For std::exp, HUGE_VAL

namespace epi::infect {

InfectivityProfile create_const_infectivity_profile(const config& cfg) {
    auto func = [beta = cfg.beta](double /*tau*/) { // tau is unused for constant infectivity
        return beta;
    };
    // For a constant function f(x) = C, the max value is C.
    return {cfg.beta, func};
}

InfectivityProfile create_lognormal_infectivity_profile(const config& cfg) {
    auto func = [inf_scale = cfg.inf_scale, inf_mean = cfg.inf_mean, inf_k = cfg.inf_k](double tau) {
        return epi::logn(tau, inf_scale, inf_mean, inf_k);
    };
    // The max value for this profile is provided by cfg.inf_max
    return {cfg.inf_max, func};
}

std::function<double(double)> create_sigmoid_susceptibility_function(const config& cfg) {
    return [susc_k = cfg.susc_k, susc_l = cfg.susc_l, susc_x0 = cfg.susc_x0](double tau) {
        if (tau < 0) {
            return 0.0;
        } else {
            double ex = std::exp(-susc_k * (tau - susc_x0));
            // Check for overflow before division
            if (ex == HUGE_VAL) return 1.0; // or some other appropriate limit
            return 1.0 - susc_l / (1.0 + ex);
        }
    };
}

} // namespace epi::infect
