#pragma once

#include <random>

namespace epi {

static std::mt19937 &mt() {
    thread_local std::random_device srd;
    thread_local std::mt19937 smt(srd());
    return smt;
}

static double uniform() {
    static std::uniform_real_distribution<double> uniform(0, 1);
    return uniform(mt());
}

double logn(double x, double s, double m, double k);


} // namespace epi
