//
// Created by atemerev on 5/24/23.
//
#include <cmath>
#include <regex>
#include "ini.h"

double logn(double x, double s, double m, double k) {
    if (x == 0) {
        return 0;
    }
    double m1 = 1.0 / (x * s * sqrt(2 * M_PI));
    double t = -pow(log(x) - m, 2) / pow(2 * s, 2);
    return k * m1 * exp(t);
}

std::string extract_string(const mINI::INIStructure &ini, const std::string &section, const std::string &key) {
    const std::string& value_str = ini.get(section).get(key);
    if (value_str.empty()) {
        std::stringstream err;
        err << "Config error: no key '" << key << "' found in section '" << section << "'" << std::endl;
        throw std::runtime_error(err.str());
    }
    std::string trim = std::regex_replace(value_str, std::regex("^ +"), "");
    trim = std::regex_replace(value_str, std::regex(" +$"), "");
    return trim;
}

int extract_int(const mINI::INIStructure &ini, const std::string &section, const std::string &key) {
    const std::string& value_str = extract_string(ini, section, key);
    int result = std::stoi(value_str);
    return result;
}

double extract_double(const mINI::INIStructure &ini, const std::string &section, const std::string &key) {
    const std::string& value_str = extract_string(ini, section, key);
    double result = std::stod(value_str);
    return result;
}


