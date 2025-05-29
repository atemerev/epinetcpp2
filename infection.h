#pragma once
#include "Common.h" // For config, InfectivityProfile
#include <functional> // For std::function

namespace epi::infect {

/// Creates an InfectivityProfile for a constant infectivity model.
std::function<double(double)> create_const_infectivity_function(double beta);

/// Creates an InfectivityProfile for a log-normal infectivity model.
std::function<double(double)> create_lognormal_infectivity_function(double scale, double mean, double k);

/// Creates a susceptibility function based on a sigmoid-like curve.
std::function<double(double)> create_sigmoid_susceptibility_function(double k, double l, double x0);

/// Create an exponential immunity decay susceptibility
std::function<double(double)> create_exp_susceptibility_function(double time_to_immunity);

std::function<double(double)> create_poisson_recovery_function(double recovery_length_expectation);

std::function<double(double)> create_const_recovery_function(double recovery_length);

} // namespace epi::infect
