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
    double conf_sp_lambda = 0;
    double conf_susc_initial = 1.0;
    int conf_n_initial = 1;

    std::string conf_output_file = "out.csv";

    CLI::App app("EpiNet2 stochastic epidemic simulator");
    app.add_option("-N,--num-people", conf_N, "Number of people in the population")->required();
    app.add_option("-n,--n-initial", conf_n_initial, "Number of initially infected people");
    app.add_option("-t,--time", conf_t_max, "Simulation time")->required();
    app.add_option("-b,--beta", conf_beta, "Beta: infectiousness modifier")->required();
    app.add_option("-f,--output-file", conf_output_file, "Output file name");
    app.add_option("-i,--infection-length", conf_inf_length, "Disease infection interval cutoff");
    app.add_option("-M,--max_infection", conf_inf_max, "Max infectiousness level cutoff");
    app.add_option("-k,--k-susceptibility", conf_susc_k, "Susceptibility: shape parameter");
    app.add_option("-l,--l-susceptibility", conf_susc_l, "Susceptibility: scale parameter");
    app.add_option("-x,--x0-susceptibility", conf_susc_x0, "Susceptibility: 50% point location");
    app.add_option("-s,--scale-infectiousness", conf_inf_scale, "Infectiousness curve: scale parameter");
    app.add_option("-m,--mean-infectiousness", conf_inf_mean, "Infectiousness curve: mean time");
    app.add_option("-K,--k-infectiousness", conf_inf_k, "Infectiousness curve: shape parameter");
    app.add_option("-L,--lambda-spontaneous", conf_sp_lambda, "Spontaneous infection rate");
    app.add_option("-S,--susc-initial", conf_susc_initial, "Initial susceptibility (0.0 to 1.0)");

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
        .susc_l = conf_susc_l,
        .susc_x0 = conf_susc_x0,
        .inf_scale = conf_inf_scale,
        .inf_mean = conf_inf_mean,
        .inf_k = conf_inf_k,
        .sp_lambda = conf_sp_lambda,
        .n_initial = conf_n_initial,
        .susc_initial = conf_susc_initial,
        .output_file = conf_output_file
    };

    // init simulation
    Simulation simulation = Simulation(config);
//    std::cout << simulation.infectiousness_function(3.0) << std::endl;
//    std::cout << simulation.susceptibility_function(108) << std::endl;
//    std::vector<double> sp_inf_times = simulation.get_spontaneous_infection_times(conf_sp_lambda);
//    for (double t : sp_inf_times) {
//        std::cout << t << std::endl;
//    }
    simulation.simulate();
    return 0;
}

