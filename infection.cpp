#include "infection.h"
#include "util.h" // For epi::logn and epi::uniform (though uniform not directly used here, logn is)
#include <cmath>   // For std::exp, HUGE_VAL

namespace epi::infect {

std::function<double(double)> create_const_infectivity_function(double beta) {
    auto func = [beta](double /*tau*/) { // tau is unused for constant infectivity
        return beta;
    };
    return func;
}

std::function<double(double)> create_lognormal_infectivity_function(double scale, double mean, double k) {
    auto func = [scale, mean, k](double tau) {
        return epi::logn(tau, scale, mean, k);
    };
    return func;
}

std::function<double(double)> create_sigmoid_susceptibility_function(double k, double l, double x0) {
    return [k, l, x0](double tau) {
        if (tau < 0) {
            return 0.0;
        } else {
            double ex = std::exp(-k * (tau - x0));
            // Check for overflow before division
            if (ex == HUGE_VAL) return 1.0;
            return 1.0 - l / (1.0 + ex);
        }
    };
}

std::function<double(double)> create_poisson_recovery_function(double recovery_length_expectation) {
    return [recovery_length_expectation](double tau) {
        double u = epi::uniform();
        return -std::log(u) / (1 / recovery_length_expectation);
    };
}

std::function<double(double)> create_const_recovery_function(double recovery_length) {
    return [recovery_length](double /*tau*/) {
        return recovery_length;
    };
}

std::function<double(double)> create_exp_susceptibility_function(double time_to_immunity) {
    return [time_to_immunity](double tau) {
        return 1 - std::exp(-tau / time_to_immunity);
    };
};

} // namespace epi::infect
