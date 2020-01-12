#include "pool.h"
#include <algorithm>
#include <iostream>

using namespace std;

Pool::Pool(const size_t population_size, const size_t inputs, const size_t outputs) : population_size(population_size), inputs(inputs), outputs(outputs)
{
    // wypełniamy kopiami, czyli krawędzie mają tę samą wartość innov
    population.resize(population_size, Genotype(inputs, outputs));

    for (auto &g : population)
        g.randomize();

    fitness.resize(population_size, 0);
    Genotype::num_nodes = inputs + outputs + 1;
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
        for (const auto &c : p2.connections)
        {

            // szukamy wspólnych genów
            if (!any_of(
                    begin(offspring.connections),
                    end(offspring.connections),
                    [&](auto x) { return x.innov == c.innov; }))
                offspring.connections.push_back(c);

            // const auto it = find_if(begin(offspring.connections), end(offspring.connections), [&](auto x) { return x.innov != c.innov; });
            // if (it == end(offspring.connections))
        }

    // uzupełniamy geny węzłów
    auto nodes = max(p1.nodes.size(), p2.nodes.size());
    for (size_t i = 0; i < nodes - offspring.nodes.size(); ++i)
        offspring.nodes.push_back(NodeGene{0});

    population.emplace_back(offspring);
}

void Pool::info()
{

    for (auto &G : population)
        cout << G << endl;
    cout << endl;
}

void Pool::epoch()
{
    for (size_t i = 0; i < population.size(); ++i)
        mutate_add_node(i);
}

// There was an 80% chance of a genome having its connection weights mutated,
// in which case each weight had a 90% chance of being uniformly perturbed
// and a 10% chance of being assigned a new random value.
// brak informacji w oryginalnej pracy, jak duże jest zabużenie
// można to sprawdzić w oryginalnej implementacji (tu założenie: +-0.1)
void Pool::mutate_weights(size_t n)
{
    auto &G = population[n];
    std::uniform_real_distribution<double> rnd_weight(-1.0, 1.0);
    std::uniform_real_distribution<double> prob(0, 1.0);

    for (auto &c : G.connections)
    {
        if (c.enabled)
        {
            if (mutation_prob > prob(rng)) // zachodzi mutacja
            {
                if (perturbation_prob > prob(rng)) // tylko perturbacja

                    c.weight += 0.1 * rnd_weight(rng);
                else // nowa losowa waga z pstwem (1-perturbation_prob)
                    c.weight = rnd_weight(rng);
            }
        }
    }
}

// In the add node mutation, an existing connection is split and the new node
// placed where the old connection used to be.The old connection is disabled
// and two new connections are added to the genome.The new connection leading
// into the new node receives a weight of 1, and the new connection leading
// out receives the same weight as the old connection.
// (...) the probability of adding a new node was 0.03
void Pool::mutate_add_node(size_t n)
{
    auto &G = population[n];
    std::uniform_real_distribution<double> prob(0, 1.0);
    if (add_node_prob < prob(rng)) // brak mutacji z pstwem (1-add_node_prob)
        return;

    std::uniform_int_distribution<size_t> rnd_link(0, G.connections.size() - 1);

    auto new_id = G.num_nodes++;
    G.nodes.size();
    G.nodes.push_back(NodeGene{new_id});
    ConnectGene *old_edge;
    do
        old_edge = &G.connections[rnd_link(rng)];
    while (old_edge->enabled == false);

    old_edge->enabled = false;

    auto edge1 = ConnectGene(old_edge->in, new_id, G.next_innov_number++);
    auto edge2 = ConnectGene(new_id, old_edge->out, G.next_innov_number++);

    edge1.weight = old_edge->weight;
    edge2.weight = 1;

    G.connections.emplace_back(edge1);
    G.connections.emplace_back(edge2);
}

// In the add connection mutation, a single new connection gene with a random
// weight is added connecting two previously unconnected nodes.
// In smaller populations (...) the probability of a new link mutation was
// 0.05. In the larger population, the probability of adding a new link was
// 0.3, because a larger population can tolerate a larger number
// of prospective species and greater topological diversity.
void Pool::mutate_add_connection(size_t n)
{
    auto &G = population[n];
    std::uniform_real_distribution<double> prob(0, 1.0);
    if (add_con_prob < prob(rng)) // brak mutacji z pstwem (1prob)
        return;

    // find 2 unconnected nodes
    // sprawdzamy wszystkie pary w losowej kolejności
    std::vector<size_t> idx(G.nodes.size());
    iota(begin(idx), end(idx), 0);
    auto jdx = idx;
    shuffle(begin(idx), end(idx), rng);
    shuffle(begin(jdx), end(jdx), rng);

    for (auto i : idx)
        for (auto j : jdx)
        {
            auto it = find_if(begin(G.connections), end(G.connections), [&](auto c) { return (c.in == i && c.out == j); });
            if (it != end(G.connections)) // znaleziono
            {
                auto new_edge = ConnectGene(i, j, G.next_innov_number++);
                std::uniform_real_distribution<double> rnd_weight(-1.0, 1.0);
                new_edge.weight = rnd_weight(rng);
                G.connections.emplace_back(new_edge);
                return;
            }
        }
}
// TODO: sprawdzic, czy już istnieją w populacji krawędzie łączące te same węzły i skopiować innov
// albo dodajemy kolejne innov, a potem duplikaton nadajemy ten sam nr innov
// uwaga na numery węzłów

void Pool::check_integrity() {}