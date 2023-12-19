#include <iostream>
#include <ostream>
#include "Simulation.h"
#include "Common.h"
#include "util.h"

node tourist_node {
    .index = -1,
    .infectivity = 1.0,
    .susceptibility = 1.0,
    .last_recovery_time = 0,
    .recovery_count = 0
};

Simulation::Simulation(config &conf) : cfg(conf) {
    for (int i = 0; i < cfg.N; i++) {
        node n = {i, 0.0, 1.0, 0.0, 0};
        nodes.push_back(n);
    }
    this->cases_by_day = std::map<int, int>();
    this->output = std::ofstream(conf.output_file);
}

Simulation::~Simulation() {
    this->output.close();
}

double Simulation::infectiousness_function(double tau) const {
    return logn(tau, cfg.inf_scale, cfg.inf_mean, cfg.inf_k);
}

double Simulation::susceptibility_function(double tau) const {
    if (tau < 0) {
        return 0;
    } else {
        double ex = exp(-cfg.susc_k * (tau - cfg.susc_x0));
        return ex == HUGE_VAL ? 1 : 1 - cfg.susc_l / (1 + ex);
    }
}

void Simulation::simulate() {

    // Assuming a single initial infected (first in the index)
    for (int i = 0; i < this->cfg.n_initial; i++) {
        event initial_infection = {0.0, i, Infection};
        this->Q.push(initial_infection);
    }

    // Pushing spontaneous infection events

    // stochastic

    /*
    std::vector<double> sp_inf_times = this->get_spontaneous_infection_times(cfg.sp_lambda);
    for (double t : sp_inf_times) {
        event sp_infection = {t, -1, Infection};
        this->Q.push(sp_infection);
    }
    */

    // deterministic

    std::vector<double> sp_inf_times = this->get_deterministic_infection_times(cfg.sp_lambda);
    for (double t : sp_inf_times) {
        event sp_infection = {t, -1, Infection};
        this->Q.push(sp_infection);
    }

    while (!Q.empty()) {
        event e = Q.top();
        if (e.time > cfg.t_max) {
            break;
        }
        switch (e.action) {
            case Infection:
//                std::cout << ".";
                this->infect(e);
                break;
            case Recovery:
                break;
        }
        Q.pop();
    }
}

void Simulation::infect(event incoming_event) {
    node& incoming_node = incoming_event.node_index >= 0 ? this->nodes.at(incoming_event.node_index) : tourist_node;
    // todo what if already infected and not recovered yet?
    // ok, recovery time might be negative
    double tau = incoming_event.time - incoming_node.last_recovery_time;
    double susceptibility = incoming_node.last_recovery_time <= 0 ? 1.0 : this->susceptibility_function(tau);
    double rand_uni = uniform(mt());
    if (rand_uni > susceptibility) {
        // not infected
        return;
    }

    // infected

    // setting up recovery
    incoming_node.last_recovery_time = incoming_event.time + cfg.inf_length;

    int day = (int) incoming_event.time;
    if (!this->cases_by_day.count(day)) {
        this->cases_by_day[day] = 1;
        // todo output count by previous day, or collect other statistics
        if (day > 0) {
            this->dump_state(day, std::cout);
            this->dump_state(day, this->output);
        }
    } else {
        this->cases_by_day[day] = this->cases_by_day[day] + 1;
    }

    // spreading infection to other nodes
    std::vector<double> inf_times = this->get_inf_times(cfg.beta); // todo loop over

    for (double t_inf : inf_times) {
        double t = incoming_event.time + t_inf;
        node& target = this->select_contact();
        // infection event
        event infect = {t, target.index, Infection};
        Q.push(infect);
        // recovery event (for stats purposes)
        event recovery = {t + cfg.inf_length, target.index, Recovery};
        Q.push(recovery);
    }
}

node& Simulation::select_contact() {
    std::uniform_int_distribution<> idist(0, (int) this->nodes.size() - 1);
    int i = idist(mt());
    return this->nodes.at(i);
}

void Simulation::dump_state(int day, std::ostream& out) {
    int infected_count = 0;
    double avg_susceptibility = 0;
    for (node n : this->nodes) {
        int delta_t = (day - 1) - (int) n.last_recovery_time;
        if (delta_t < 0 && delta_t > -this->cfg.inf_length) {
            infected_count++;
        }
        double susceptibility = n.last_recovery_time <= 0 ? 1.0 : susceptibility_function(delta_t);
        avg_susceptibility += susceptibility;
    }
    avg_susceptibility = avg_susceptibility / (double) this->nodes.size();
    out << day - 1 << ","
        << this->cases_by_day[day - 1] << ","
        << infected_count << ","
        << avg_susceptibility << std::endl;
    out.flush();
}
