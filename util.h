#pragma once

static inline std::uniform_real_distribution<double> uniform(0, 1);

double logn(double x, double s, double m, double k);

std::string extract_string(const mINI::INIStructure &ini, const std::string &section, const std::string &key);


int extract_int(const mINI::INIStructure &ini, const std::string &section, const std::string &key);

double extract_double(const mINI::INIStructure &ini, const std::string &section, const std::string &key);


