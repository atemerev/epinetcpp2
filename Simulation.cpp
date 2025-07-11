#include <iostream>
#include <ostream>
#include <utility> // For std::move
#include "Simulation.h"
#include "Common.h"
#include "util.h"

node tourist_node {
    .index = -1,
    .infectivity = 1.0,
    .susceptibility = 1.0,
    .last_recovery_time = 0,
    .recovery_count = 0,
    .infected = true
};

Simulation::Simulation(config &conf,
                       std::function<double(double)> infectivity_func,
                       std::function<double(double)> susc_func,
                       std::function<double(double)> recovery_func
                       )
    : cfg(conf),
      infectivity_func_(std::move(infectivity_func)),
      susceptibility_func_(std::move(susc_func)),
      recovery_func_(std::move(recovery_func)) {
    for (int i = 0; i < cfg.N; i++) {
        node n = {i, 0.0, conf.susc_initial, 0.0, 0, false};
        nodes.push_back(n);
    }
    this->cases_by_day = std::map<int, int>();
    this->output = std::ofstream(conf.output_file);
    compute_integral_numerically(conf.inf_length);
}

Simulation::~Simulation() {
    if (this->output.is_open()) { // Good practice to check before closing
        this->output.close();
    }
}

// Old infectiousness_function and susceptibility_function implementations are removed
// as they are now handled by the std::function members.

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
            this->recover(e);
                break;
        }
        Q.pop();
    }
}

void Simulation::infect(event incoming_event) {
    node& incoming_node = incoming_event.node_index >= 0 ? this->nodes.at(incoming_event.node_index) : tourist_node;
    // Determine susceptibility
    double tau = incoming_event.time - incoming_node.last_recovery_time;
    double susceptibility_value;
    if (incoming_event.node_index == -1) { // Tourist node always susceptible (or use its own property)
        susceptibility_value = tourist_node.susceptibility;
    } else if (incoming_node.last_recovery_time <= 0 && incoming_node.recovery_count == 0) { // Never infected before
        susceptibility_value = cfg.susc_initial;
    } else { // Previously infected, calculate based on time since last recovery
        susceptibility_value = tau > 0 ? this->susceptibility_func_(tau) : 0; // tau can be negative if recovery time is in the future
    }

    double rand_uni = epi::uniform();
    if (rand_uni > susceptibility_value) {
        // not infected
        return;
    }

    // infected

    incoming_node.infected = true;

    // setting up recovery for the currently infected node
    double recovery_length = this->recovery_func_(incoming_event.time);

    if (incoming_event.node_index >=0) { // Regular nodes update their state
        incoming_node.last_recovery_time = incoming_event.time + recovery_length;
        incoming_node.recovery_count++;

        // push recovery event
        event new_recovery_event = {incoming_node.last_recovery_time, incoming_node.index, Recovery};
        Q.push(new_recovery_event);
    }
    // Tourist node state doesn't need to be tracked in the same way for recovery

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
    // The 'cfg.beta' passed to get_inf_times is used as the base rate for Poisson generation of potential contact times.

    std::vector<double> inf_times = this->get_inf_times(cfg.beta, recovery_length);

    for (double t_inf : inf_times) {
        double t_actual_infection = incoming_event.time + t_inf;
        if (t_actual_infection > cfg.t_max) continue; // Don't schedule events past t_max

        node& target = this->select_contact();
        // infection event for the target
        event new_infection_event = {t_actual_infection, target.index, Infection};
        Q.push(new_infection_event);
        
    }
}

void Simulation::recover(event incoming_event) {
    node& incoming_node = this->nodes.at(incoming_event.node_index);
    incoming_node.infected = false;
}

node& Simulation::select_contact() {
    std::uniform_int_distribution<> idist(0, (int) this->nodes.size() - 1);
    int i = idist(epi::mt());
    return this->nodes.at(i);
}

void Simulation::dump_state(int day, std::ostream& out) {
    int infected_count = 0; // Count of currently infectious individuals
    double total_susceptibility = 0;
    double current_time_for_stats = static_cast<double>(day-1); // Stats for the completed day

    for (const node& n : this->nodes) {
        // A node is considered infectious if its last_recovery_time is in the future
        // relative to current_time_for_stats, but not further than inf_length ago from that future recovery time.
        // Essentially, current_time_for_stats < n.last_recovery_time AND n.last_recovery_time - current_time_for_stats <= cfg.inf_length

        // if (n.last_recovery_time > current_time_for_stats && (n.last_recovery_time - current_time_for_stats) <= this->cfg.inf_length) {
        //     infected_count++;
        // }

        if (n.infected) {
            infected_count++;
        }

        double susceptibility_value;
        if (n.recovery_count == 0) { // Never infected
             susceptibility_value = this->cfg.susc_initial;
        } else {
            // Time since the last recovery event completed.
            // If n.last_recovery_time is in the future, the node is still infected or immune from that event.
            // The susceptibility function should ideally take time from *end* of the infectious period.
            double time_since_immunity_waning_started = current_time_for_stats - n.last_recovery_time;
            susceptibility_value = this->susceptibility_func_(time_since_immunity_waning_started);
        }
        total_susceptibility += susceptibility_value;
    }
    double avg_susceptibility = this->nodes.empty() ? 0 : total_susceptibility / (double) this->nodes.size();
    
    out << day - 1 << ","
        << (this->cases_by_day.count(day - 1) ? this->cases_by_day.at(day - 1) : 0) << ","
        << infected_count << ","
        << avg_susceptibility << std::endl;
    out.flush();
}
