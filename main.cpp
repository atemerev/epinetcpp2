#include "Simulation.h"
#include "include/CLI11.hpp"
#include "infection.h" // Include the header for factory functions

int main(int argc, char **argv) {
    int conf_N = 100000;
    double conf_t_max = 365 * 2;
    double conf_beta = 1.0;
    double conf_inf_length = 20.0;
    double conf_rec_length = 20; // recovery (expectation) length
    double conf_time_to_imm = 200; // time to immunity for exp immunity
    double conf_susc_k = -0.009776;
    double conf_susc_l = 1.0332;
    double conf_susc_x0 = 195.5736;
    double conf_inf_scale = 0.2577;
    double conf_inf_mean = 1.4915;
    double conf_inf_k = 0.293;
    double conf_sp_lambda = 0.00;
    double conf_susc_initial = 0.7;
    int conf_n_initial = 1;

    std::string conf_output_file = "out.csv";

    CLI::App app("EpiNet2 stochastic epidemic simulator");
    app.add_option("-N,--num-people", conf_N,
                   "Number of people in the population")
       ->required();
    app.add_option("-n,--n-initial", conf_n_initial,
                   "Number of initially infected people");
    app.add_option("-t,--time", conf_t_max, "Simulation time")->required();
    app.add_option("-b,--beta", conf_beta, "Beta: infectiousness modifier")
       ->required();
    app.add_option("-f,--output-file", conf_output_file, "Output file name");
    app.add_option("-i,--infection-length", conf_inf_length,
                   "Disease infection interval cutoff");
    app.add_option("-k,--k-susceptibility", conf_susc_k,
                   "Susceptibility: shape parameter");
    app.add_option("-l,--l-susceptibility", conf_susc_l,
                   "Susceptibility: scale parameter");
    app.add_option("-x,--x0-susceptibility", conf_susc_x0,
                   "Susceptibility: 50% point location");
    app.add_option("-s,--scale-infectiousness", conf_inf_scale,
                   "Infectiousness curve: scale parameter");
    app.add_option("-m,--mean-infectiousness", conf_inf_mean,
                   "Infectiousness curve: mean time");
    app.add_option("-K,--k-infectiousness", conf_inf_k,
                   "Infectiousness curve: shape parameter");
    app.add_option("-L,--lambda-spontaneous", conf_sp_lambda,
                   "Spontaneous infection rate");
    app.add_option("-S,--susc-initial", conf_susc_initial,
                   "Initial susceptibility (0.0 to 1.0)");

    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    // init config
    config config_obj = {.N = conf_N, // Renamed to avoid conflict with 'config' type
                     .t_max = conf_t_max,
                     .beta = conf_beta,
                     .inf_length = conf_inf_length,
                     .susc_k = conf_susc_k,
                     .susc_l = conf_susc_l,
                     .susc_x0 = conf_susc_x0,
                     .inf_scale = conf_inf_scale,
                     .inf_mean = conf_inf_mean,
                     .inf_k = conf_inf_k,
                     .sp_lambda = conf_sp_lambda,
                     .n_initial = conf_n_initial,
                     .susc_initial = conf_susc_initial,
                     .output_file = conf_output_file};

//    auto infectivity_func = epi::infect::create_const_infectivity_function(config_obj.beta);
    auto infectivity_func =
        epi::infect::create_lognormal_infectivity_function(config_obj.inf_scale, config_obj.inf_mean, config_obj.inf_k);
    
//    auto susc_func = epi::infect::create_exp_susceptibility_function(conf_time_to_imm);
    auto susc_func = epi::infect::create_sigmoid_susceptibility_function(config_obj.susc_k, config_obj.susc_l, config_obj.susc_x0);

//    auto recovery_func = epi::infect::create_poisson_recovery_function(conf_rec_length);
    auto recovery_func = epi::infect::create_const_recovery_function(config_obj.inf_length);

    Simulation simulation = Simulation(config_obj, infectivity_func, susc_func, recovery_func);

/*
     std::vector<double> sp_inf_times =
         simulation.get_inf_times(config_obj.beta, config_obj.inf_length);
     for (double t : sp_inf_times) {
         std::cout << t << std::endl;
     }


    const double N_TRIALS = 1000; // This was outside main, ensure it's defined if used

    double trials_sum = 0.0;

    for (int i = 0; i < N_TRIALS; i++) {
        // Note: get_inf_times is now a const member of Simulation.
        // It uses the configured infectivity_profile_ within the simulation instance.
        // The 'beta' parameter to get_inf_times is the base rate for event generation.
        std::vector<double> sp_inf_times =
            simulation.get_inf_times(config_obj.beta, config_obj.inf_length);
        trials_sum += double(sp_inf_times.size());
    }
    std::cout << "Average infections from one event: " << trials_sum / N_TRIALS << std::endl;
*/


    simulation.simulate();
    return 0;
}

