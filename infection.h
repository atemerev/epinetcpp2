#pragma once
#include "Common.h"

#include <functional>

namespace epi::infect {

std::vector<double>

static get_infection_times(double beta, double disease_length,
                    const InfectivityProfile &profile);



}