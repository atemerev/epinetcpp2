#pragma once
#include "Common.h" // For config, InfectivityProfile
#include <functional> // For std::function

namespace epi::infect {

/// Creates an InfectivityProfile for a constant infectivity model.
InfectivityProfile create_const_infectivity_profile(double beta);

/// Creates an InfectivityProfile for a log-normal infectivity model.
InfectivityProfile create_lognormal_infectivity_profile(double scale, double mean, double k, double max_value);

/// Creates a susceptibility function based on a sigmoid-like curve.
std::function<double(double)> create_sigmoid_susceptibility_function(double k, double l, double x0);

std::function<double(double)> create_poisson_recovery_function(double recovery_length_expectation);

} // namespace epi::infect
