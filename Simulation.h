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
    std::function<double(double)> infectivity_func_;
    std::function<double(double)> susceptibility_func_;
    std::function<double(double)> recovery_func_;

    void infect(event incoming_event);
    void recover(event incoming_event);

    double precomputed_integral_;
    double precomputed_max_value_;

    void compute_integral_numerically(double inf_length) {
        const int n_steps = 1000;
        double dt = inf_length / n_steps;
        double integral = 0.0;
        double max_val = 0.0;

        // Trapezoidal rule + max finding
        double prev_val = infectivity_func_(0.0);
        max_val = prev_val;

        for (int i = 1; i <= n_steps; ++i) {
            double t = i * dt;
            double curr_val = infectivity_func_(t);

            integral += 0.5 * (prev_val + curr_val) * dt;
            max_val = std::max(max_val, curr_val);
            prev_val = curr_val;
        }

        precomputed_integral_ = integral;
        precomputed_max_value_ = max_val;
    }

public:
    explicit Simulation(config& cfg, std::function<double(double)> infectivity_func,
        std::function<double(double)> susc_func,
        std::function<double(double)> recovery_func);

    ~Simulation();

    void simulate();

    void dump_state(int day, std::ostream& out);

    [[nodiscard]]
    std::vector<double> get_inf_times(double beta, double inf_length) const {
        std::vector<double> result;

        // Handle edge cases
        if (beta <= 0 || precomputed_integral_ <= 0 || precomputed_max_value_ <= 0) {
            return result;
        }

        // Properly normalized rate for thinning
        double adjusted_rate = (beta * inf_length) * precomputed_max_value_ / precomputed_integral_;

        double t = 0;
        while (t < inf_length) {
            double u = epi::uniform();
            t = t - log(u) / adjusted_rate;

            if (t < inf_length) {
                double rate_at_t = infectivity_func_(t);
                double s = epi::uniform();

                if (s < rate_at_t / precomputed_max_value_) {
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
