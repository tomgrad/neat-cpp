#include <algorithm>

#include "pool.h"

using namespace std;

Pool::Pool(const size_t population_size, const size_t inputs, const size_t outputs) : population_size(population_size), inputs(inputs), outputs(outputs)
{
    // wypełniamy kopiami, czyli krawędzie mają tę samą wartość innov
    population.resize(population_size, Genotype(inputs, outputs));

    for (auto &g : population)
        g.randomize();

    fitness.resize(population_size, 0);
}

// Matching up genomes for different network topologies using innovation
// numbers. (...) Matching genes are inherited randomly, whereas disjoint genes
// and excess genes are inherited from the more fit parent.
// The disabled genes may become enabled again in future generations :
// there’s a preset chance that an inherited gene is disabled
// if it is disabled in either parent.
void Pool::mate(size_t parent1, size_t parent2)
{
    uniform_int_distribution<int> rnd(0, 1);
    if (fitness[parent1] < fitness[parent2])
        swap(parent1, parent2);

    const auto &p1 = population[parent1];
    const auto &p2 = population[parent2];

    // kopiujemy lepiej dopasowanego
    auto offspring = p1;

    // szukamy wspólnych cech i losowo zamieniamy wagi
    for (auto &c : offspring.connections)
    {
        if (rnd(Genotype::rng)) // zamiana
        {
            // szukamy wspólnych genów
            const auto it = find_if(begin(p2.connections), end(p2.connections), [&](auto x) { return x.innov == c.innov; });
            if (it != end(p2.connections))
                c.weight = it->weight;
        }
    }

    // dziedziczone są nieprzekrywające się geny od obu rodziców z równym dopasowaniem
    if (fitness[parent1] == fitness[parent2])

        for (auto &c : p2.connections)
        {
            // szukamy wspólnych genów
            const auto it = find_if(begin(offspring.connections), end(offspring.connections), [&](auto x) { return x.innov != c.innov; });
            if (it != end(offspring.connections))
                offspring.connections.push_back(*it);
        }

    // uzupełniamy geny węzłów
    auto nodes = max(p1.nodes.size(), p2.nodes.size());
    for (size_t i = 0; i < nodes - offspring.nodes.size(); ++i)
        offspring.nodes.push_back(NodeGene{0});
}