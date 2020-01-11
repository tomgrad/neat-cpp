#pragma once

#include "genotype.h"

class Pool
{
public:
    Pool(const size_t population_size, const size_t inputs, const size_t outputs);
    void mate(const size_t parent1, const size_t parent2);

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
