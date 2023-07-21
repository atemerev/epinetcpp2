#include <iostream>
#include "Simulation.h"
#include "Common.h"
#include "util.h"

Simulation::Simulation(config &conf) : cfg(conf) {
    for (uint32_t i = 0; i < cfg.N; i++) {
        node n = {i, 0.0, 1.0, 0.0, 0};
        nodes.push_back(n);
    }
    this->cases_by_day = std::map<int, int>();
}

double Simulation::infectiousness_function(double tau) const {
    return logn(tau, cfg.inf_scale, cfg.inf_mean, cfg.inf_k);
}

// todo susceptibility function seems to be incorrect!
// todo check manually, e.g. fresh nodes have susceptiblity of 1.0, and with disease currently in progress - 0.0
// todo keep both last infected and last recovered times? so if t - last_infected < disease_length, time since
//  recovery is negative, and susceptibility is 1.0


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
    event first_infection = {0.0, 0, Infection};
    this->Q.push(first_infection);
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
    node& incoming_node = this->nodes.at(incoming_event.node_index);
    double tau = incoming_event.time - incoming_node.last_recovery_time;
    double susceptibility = incoming_node.last_recovery_time <= 0 ? 1.0 : this->susceptibility_function(tau);
    double rand_uni = uniform(mt());
    if (rand_uni > susceptibility) {
        return;
    }

    // recovery
    incoming_node.last_recovery_time = incoming_event.time + cfg.inf_length;

    // accounting for infection

    int day = (int) incoming_event.time;
    if (!this->cases_by_day.count(day)) {
        this->cases_by_day[day] = 1;
        // todo output count by previous day, or collect other statistics
        if (day > 0) {
            std::cout << day - 1 << "," << this->cases_by_day[day - 1] << std::endl;
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

void Simulation::dump_state() {
    for (auto & k : this->cases_by_day) {
        std::cout << k.first << "," << k.second << std::endl;
    }
}
