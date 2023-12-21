#pragma once

#include <random>
#include <queue>
#include <cmath>
#include <fstream>
#include "Common.h"
#include "util.h"

class Simulation {
private:
    static std::mt19937 &mt() {
        thread_local static std::random_device srd;
        thread_local static std::mt19937 smt(srd());
        return smt;
    }

    config& cfg;
    std::ofstream output;

    std::vector<node> nodes = {}; // all nodes vector (the source of truth)
    std::priority_queue<event, std::vector<event>, std::greater<>> Q = {}; // event queue
    std::map<int, int> cases_by_day;

    void infect(event incoming_event);
//    void recover();

public:
    explicit Simulation(config& cfg);
    ~Simulation();

    [[nodiscard]] double infectiousness_function(double tau) const;
    [[nodiscard]] double susceptibility_function(double tau) const;
    void simulate();

    void dump_state(int day, std::ostream& out);

    [[nodiscard]]
    std::vector<double> get_inf_times(double beta, double inf_length) const {
        std::vector<double> result;

        double t = 0;
        while (t < inf_length) {
            double u = uniform(mt());
            t = t - log(u) / beta;
            if (t < inf_length) {
                double rate = infectiousness_function(t);
                double s = uniform(mt());
                if (s < rate / cfg.inf_max) {
                    result.push_back(t);
                }
            }
        }
        return result;
    }

    static double get_inter_event_time_poisson(double rate) {
        const double u = uniform(mt());
        return - log(u) / rate;
    }

    [[nodiscard]]
    std::vector<double> get_spontaneous_infection_times(double lambda) const {
        std::vector<double> result;
        double t = 0;
        while (t < cfg.t_max) {
            double u = uniform(mt());
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
