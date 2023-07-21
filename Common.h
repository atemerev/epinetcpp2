#pragma once
#include <cstdint>
#include <map>

enum Action {Infection, Recovery};

struct node {
    uint32_t index;
    double infectivity;
    double susceptibility;
    double last_recovery_time;
    int recovery_count;
};

struct event {
    double time;
    uint32_t node_index;
    Action action;

    bool operator >(const event& other) const {
        return (time > other.time);
    }
};


struct config {
    int N;
    double t_max;
    double beta;

    double inf_length;
    double inf_max;

    double susc_k;
    double susc_l;
    double susc_x0;

    double inf_scale;
    double inf_mean;
    double inf_k;
};