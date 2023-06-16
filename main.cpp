#include <iostream>
#include "ini.h"
#include "Simulation.h"

int main() {
    // init config
    config config = {
        .N = 10000,
        .t_max = 1200.0,
        .beta_from = 1.4,
        .beta_to = 1.5,
        .beta_step = 0.05,
        .inf_length = 20.0,
        .inf_max = 0.10908088834560385,
        .susc_k = -0.00977555285,
        .susc_l = 1.03320181,
        .susc_x0 = 195.573648,
        .inf_scale = 0.2577,
        .inf_mean = 1.4915,
        .inf_k = 0.293
    };
    // init simulation
    Simulation simulation = Simulation(config);
    // run with the single beta
    simulation.simulate();
    simulation.dump_state();
    return 0;
}

