#pragma once
#include <cstdint>
#include <functional>
#include <string>
#include <map>

enum Action {Infection, Recovery};

struct InfectivityProfile {
    double max_function_value;
    std::function<double(double)> infectivity_function;
};

struct node {
    int index;
    double infectivity;
    double susceptibility;
    double last_recovery_time;
    int recovery_count;
    bool infected;
};

struct event {
    double time;
    int node_index;
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

    double susc_k;
    double susc_l;
    double susc_x0;

    double inf_scale;
    double inf_mean;
    double inf_k;

    double sp_lambda; // spontaneous infection rate
    int n_initial; // initial infected
    double susc_initial; // initial susceptibility
    std::string output_file;
};