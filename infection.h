#pragma once
#include "Common.h" // For config, InfectivityProfile
#include <functional> // For std::function

namespace epi::infect {

/// Creates an InfectivityProfile for a constant infectivity model.
/// The infectivity is cfg.beta, and the max_function_value is also cfg.beta.
InfectivityProfile create_const_infectivity_profile(const config& cfg);

/// Creates an InfectivityProfile for a log-normal infectivity model.
/// Uses cfg.inf_scale, cfg.inf_mean, cfg.inf_k for the logn function
/// and cfg.inf_max for the max_function_value.
InfectivityProfile create_lognormal_infectivity_profile(const config& cfg);

/// Creates a susceptibility function based on a sigmoid-like curve.
/// Uses cfg.susc_k, cfg.susc_l, cfg.susc_x0.
std::function<double(double)> create_sigmoid_susceptibility_function(const config& cfg);

} // namespace epi::infect
