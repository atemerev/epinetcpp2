#pragma once

#include <random>
#include <queue>
#include <cmath>
#include <fstream>
#include <functional> // Required for std::function
#include "Common.h"
#include "util.h"

class Simulation {
private:
    config& cfg;
    std::ofstream output;

    std::vector<node> nodes = {}; // all nodes vector (the source of truth)
    std::priority_queue<event, std::vector<event>, std::greater<>> Q = {};
    std::map<int, int> cases_by_day;

    // Store the functional objects
    InfectivityProfile infectivity_profile_;
    std::function<double(double)> susceptibility_func_;
    std::function<double(double)> recovery_func_;

    void infect(event incoming_event);

public:
    // Modified constructor
    explicit Simulation(config& cfg, InfectivityProfile infect_profile,
        std::function<double(double)> susc_func,
        std::function<double(double)> recovery_func);

    ~Simulation();

    // These now use the stored functional objects
    [[nodiscard]] double infectiousness_function(double tau) const {
        return infectivity_profile_.infectivity_function(tau);
    }
    [[nodiscard]] double susceptibility_function(double tau) const {
        return susceptibility_func_(tau);
    }
    void simulate();

    void dump_state(int day, std::ostream& out);

    [[nodiscard]]
    std::vector<double> get_inf_times(double beta, double inf_length) const {
        std::vector<double> result;
        double t = 0;
        while (t < inf_length) {
            double u = epi::uniform();
            // 'beta' here is the overall rate for potential events from the config
            t = t - log(u) / beta;
            if (t < inf_length) {
                double rate_at_t = infectiousness_function(t); // Actual infectivity at time t from profile
                double s = epi::uniform();
                // Use max_function_value from the profile for thinning
                if (infectivity_profile_.max_function_value > 0 && // Avoid division by zero or issues with 0 max
                    s < rate_at_t / infectivity_profile_.max_function_value) {
                    result.push_back(t);
                } else if (infectivity_profile_.max_function_value == 0 && rate_at_t > 0) {
                    // If max is 0 but rate is positive, this is an edge case.
                    // Depending on model, might mean always accept or is an error.
                    // For now, let's assume if rate_at_t > 0 and max_value is 0, it's a valid event.
                    result.push_back(t);
                }
            }
        }
        return result;
    }

    static double get_inter_event_time_poisson(double rate) {
        const double u = epi::uniform();
        return - log(u) / rate;
    }

    [[nodiscard]]
    std::vector<double> get_spontaneous_infection_times(double lambda) const {
        std::vector<double> result;
        double t = 0;
        while (t < cfg.t_max) {
            double u = epi::uniform();
            t = t - log(u) / lambda;
            if (t < cfg.t_max) {
                result.push_back(t);
            }
        }
        return result;
    }

    [[nodiscard]]
    std::vector<double> get_deterministic_infection_times(double lambda) const {
        std::vector<double> result;
        double t = 0;
        while (t < cfg.t_max) {
            t = t + 1 / lambda;
            if (t < cfg.t_max) {
                result.push_back(t);
            }
        }
        return result;
    }

    node &select_contact();
};
