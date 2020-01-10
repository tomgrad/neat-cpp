#include "pool.h"

Pool::Pool(size_t population_size, size_t inputs, size_t outputs) : population_size(population_size), inputs(inputs), outputs(outputs)
{
    population.reserve(population_size);
    for (size_t i = 0; i < population_size; ++i)
        population.push_back(Genotype(inputs, outputs));

    fitness.resize(population_size, 0);
}