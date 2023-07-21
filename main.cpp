#include "Simulation.h"
#include "include/CLI11.hpp"

int main(int argc, char** argv) {

    int conf_N = 100000;
    double conf_t_max = 365 * 2;
    double conf_beta = 1.5;
    double conf_inf_length = 20.0;
    double conf_inf_max = 0.10908;
    double conf_susc_k = -0.009776;
    double conf_susc_l = 1.0332;
    double conf_susc_x0 = 195.5736;
    double conf_inf_scale = 0.2577;
    double conf_inf_mean = 1.4915;
    double conf_inf_k = 0.293;

    CLI::App app("EpiNet2 stochastic epidemic simulator");
    app.add_option("-N,--num-people", conf_N, "Number of people in the population");
    app.add_option("-t,--time", conf_t_max, "Simulation time");
    app.add_option("-b,--beta", conf_beta, "Beta: infectiousness modifier");
    app.add_option("-i,--infection-length", conf_inf_length, "Disease infection interval cutoff");
    app.add_option("-M,--max_infection", conf_inf_max, "Max infectiousness level cutoff");
    app.add_option("-k,--k-susceptibility", conf_susc_k, "Susceptibility: shape parameter");
    app.add_option("-l,--l-susceptibility", conf_susc_l, "Susceptibility: scale parameter");
    app.add_option("-x,--x0-susceptibility", conf_susc_x0, "Susceptibility: 50% point location");
    app.add_option("-s,--scale-infectiousness", conf_inf_scale, "Infectiousness curve: scale parameter");
    app.add_option("-m,--mean-infectiousness", conf_inf_mean, "Infectiousness curve: mean time");
    app.add_option("-K,--k-infectiousness", conf_inf_k, "Infectiousness curve: shape parameter");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    // init config
    config config = {
        .N = conf_N,
        .t_max = conf_t_max,
        .beta = conf_beta,
        .inf_length = conf_inf_length,
        .inf_max = conf_inf_max,
        .susc_k = conf_susc_k,
        .susc_l = conf_susc_k,
        .susc_x0 = conf_susc_x0,
        .inf_scale = conf_inf_scale,
        .inf_mean = conf_inf_mean,
        .inf_k = conf_inf_k
    };

    // init simulation
    Simulation simulation = Simulation(config);
    // run with the single beta
    simulation.simulate();
//    simulation.dump_state();
//    std::cout << simulation.susceptibility_function(-50) << std::endl;
    return 0;
}

