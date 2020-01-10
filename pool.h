#pragma once

#include "genotype.h"

class Pool
{
public:
    Pool(size_t population_size, size_t inputs, size_t outputs);
    void mate(size_t, size_t);

private:
    size_t population_size;
    size_t inputs;
    size_t outputs;
    std::vector<Genotype> population;
    std::vector<double> fitness;

    double mutation_prob = 0.8;
    double perturbation_prob = 0.9;
    double add_node_prob = 0.03;
    double add_con_prob = 0.05;
};
