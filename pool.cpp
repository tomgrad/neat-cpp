#include "pool.h"
#include <algorithm>
#include <iostream>

using namespace std;

Pool::Pool(const size_t population_size, const size_t inputs, const size_t outputs) : population_size(population_size), inputs(inputs), outputs(outputs)
{
    // wypełniamy kopiami, czyli krawędzie mają tę samą wartość innov
    population.resize(population_size, Genotype(inputs, outputs));
    random_device rd;
    rng.seed(rd());
    rnd_w = uniform_real_distribution<double>(-1, 1);
    rnd_p = uniform_real_distribution<double>(0, 1);

    for (auto &g : population)
        g.randomize();
}

// There was an 80% chance of a genome having its connection weights mutated,
// in which case each weight had a 90% chance of being uniformly perturbed
// and a 10% chance of being assigned a new random value.
// brak informacji w oryginalnej pracy, jak duże jest zabużenie
// można to sprawdzić w oryginalnej implementacji (tu założenie: +-0.1)
void Pool::mutate_weights(size_t n)
{
    if (mutation_prob < rnd_p(rng)) // nie zachodzi mutacja
        return;
    auto &G = population[n];

    for (auto &c : G.connections)
    {
        if (perturbation_prob > rnd_p(rng)) // tylko perturbacja
            c.weight += 0.05 * rnd_w(rng);
        else // nowa losowa waga z pstwem (1-perturbation_prob)
            c.weight = rnd_w(rng);
        // if (toggle_enable_prob > rnd_p(rng))
        //     c.enabled = !c.enabled;
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
    if (add_node_prob < rnd_p(rng)) // brak mutacji z pstwem (1-add_node_prob)
        return;
    auto &G = population[n];

    std::uniform_int_distribution<size_t> rnd_link(0, G.connections.size() - 1);

    auto new_id = G.nodes.size();
    G.nodes.push_back(NodeGene{});
    ConnectGene *old_edge;
    do
        old_edge = &G.connections[rnd_link(rng)];
    while (!old_edge->enabled);

    old_edge->enabled = false;

    // sprawdzamy, czy ta cecha już pojawiła się w aktualnym pokoleniu, by ew. skopiować innov
    auto get_innov = [&](auto i, auto j) {
        const auto it = find_if(begin(curr_gen), end(curr_gen), [&](auto x) { return x.in == i && x.out == j; });
        if (it != end(curr_gen)) // jest już taka cecha, więc kopiujemy innov
            return it->innov;
        else
            return G.next_innov_number++;
    };

    auto edge1 = ConnectGene(old_edge->in, new_id, get_innov(old_edge->in, new_id));
    auto edge2 = ConnectGene(new_id, old_edge->out, get_innov(new_id, old_edge->out));

    edge1.weight = old_edge->weight;
    edge2.weight = 1;

    G.connections.emplace_back(edge1);
    G.connections.emplace_back(edge2);
    curr_gen.push_back(edge1);
    curr_gen.push_back(edge2);
}

// In the add connection mutation, a single new connection gene with a random
// weight is added connecting two previously unconnected nodes.
// In smaller populations (...) the probability of a new link mutation was
// 0.05. In the larger population, the probability of adding a new link was
// 0.3, because a larger population can tolerate a larger number
// of prospective species and greater topological diversity.
void Pool::mutate_add_connection(size_t n)
{
    if (add_con_prob < rnd_p(rng)) // brak mutacji z pstwem (1prob)
        return;

    auto &G = population[n];

    // find 2 unconnected nodes
    // sprawdzamy wszystkie pary w losowej kolejności
    std::vector<size_t> idx(G.nodes.size());
    iota(begin(idx), end(idx), 0);
    auto jdx = idx;
    shuffle(begin(idx), end(idx), rng);
    shuffle(begin(jdx), end(jdx), rng);
    unsigned innov = 0;
    for (auto i : idx)
        for (auto j : jdx)
        {
            auto it = find_if(begin(G.connections), end(G.connections), [&](auto c) { return (c.in == i && c.out == j); });
            if (it != end(G.connections)) // znaleziono
            {
                // sprawdzamy, czy ta cecha już pojawiła się w aktualnym pokoleniu, by ew. skopiować innov
                const auto it = find_if(begin(curr_gen), end(curr_gen), [&](auto x) { return x.in == i && x.out == j; });
                if (it != end(curr_gen)) // jest już taka cecha, więc kopiujemy innov
                    innov = it->innov;
                else
                    innov = G.next_innov_number++;

                auto new_edge = ConnectGene(i, j, innov);
                new_edge.weight = rnd_w(rng);
                G.connections.emplace_back(new_edge);
                curr_gen.push_back(new_edge); // śledzimy zmiany w aktualnej generacji
                return;                       // tylko jedno nowe połączenie na epokę
            }
        }
}

// Matching up genomes for different network topologies using innovation
// numbers. (...) Matching genes are inherited randomly, whereas disjoint genes
// and excess genes are inherited from the more fit parent.
// The disabled genes may become enabled again in future generations :
// there’s a preset chance that an inherited gene is disabled
// if it is disabled in either parent.
void Pool::mate(size_t parent1, size_t parent2)
{
    if (population[parent1].fitness < population[parent2].fitness)
        swap(parent1, parent2);

    const auto &p1 = population[parent1];
    const auto &p2 = population[parent2];

    // kopiujemy lepiej dopasowanego
    auto offspring = p1;

    // szukamy wspólnych cech i losowo zamieniamy wagi
    for (auto &c : offspring.connections)
        if (rnd_p(rng) > 0.5) // zamiana
        {
            // szukamy wspólnych genów
            const auto it = find_if(
                begin(p2.connections),
                end(p2.connections),
                [&](auto x) { return x.innov == c.innov; });
            if (it != end(p2.connections))
            {
                c.weight = it->weight;
                c.enabled = it->enabled;
            }
        }

    // dziedziczone są nieprzekrywające się geny od obu rodziców z równym dopasowaniem
    // można też losowo
    if (population[parent1].fitness == population[parent2].fitness)
        for (const auto &c : p2.connections)
            // szukamy wspólnych genów
            if (!any_of(
                    begin(offspring.connections),
                    end(offspring.connections),
                    [&](auto x) { return x.innov == c.innov; }))
                offspring.connections.push_back(c);

    // ponowne aktywowanie genów
    for (auto &c : offspring.connections)
        if (!c.enabled && reenable_prob > rnd_p(rng))
            c.enabled = true;

    // uzupełniamy geny węzłów
    auto num_nodes = max(p1.nodes.size(), p2.nodes.size());
    offspring.nodes.resize(num_nodes);
    population.emplace_back(offspring);
}

// fix, actually...
void Pool::check_integrity()
{
    for (auto &G : population)
    {
        partition(
            begin(G.connections),
            end(G.connections),
            [&](auto x) { return x.enabled; });
        sort(
            begin(G.connections),
            end(G.connections),
            [&](auto x, auto y) { return x.innov < y.innov; });
        sort(
            begin(G.connections),
            end(G.connections),
            [&](auto x, auto y) { return x.out < y.out; });

        sort(
            begin(G.connections),
            end(G.connections),
            [&](auto x, auto y) { return x.in < y.in; });

        // szukamy duplikatów krawędzi u osobnika
        auto cut = unique(
            begin(G.connections),
            end(G.connections),
            [&](auto x, auto y) { return ((x.in == y.in) && (x.out == y.out)); });

        // unique tylko przesuwa duplikaty na koniec
        G.connections.erase(cut, end(G.connections));

        // uzupełniamy liczbę węzłów po krzyżowaniu
        auto num_nodes = max_element(
            begin(G.connections),
            end(G.connections),
            [&](auto x, auto y) { return x.out < y.out; });
        G.nodes.resize(num_nodes->out + 1);
    }
}

void Pool::selection()
{
    // evaluate
    calc_fitness();
    // sort
    sort(
        begin(population),
        end(population),
        [&](auto x, auto y) { return x.fitness > y.fitness; });

    // remove loosers
    population.resize(population_size);
}

void Pool::calc_fitness()
{
    // hardcoded XOR

    vector<double> ins[4] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    vector<double> outs = {0, 1, 1, 0};

    for (auto &G : population)
    {
        G.fitness = 0;
        for (int i = 0; i < 4; ++i)
            G.fitness -= (G(ins[i])[0] - outs[i]) * (G(ins[i])[0] - outs[i]);
    }
}

void Pool::epoch()
{
    curr_gen.clear();

    for (size_t i = 0; i < population.size(); ++i)
    {
        mutate_add_node(i);
        mutate_add_connection(i);
        mutate_weights(i);
    }

    std::vector<size_t> idx(population_size);
    iota(begin(idx), end(idx), 0);
    shuffle(begin(idx), end(idx), rng);

    for (size_t i = 0; i < population_size; i += 2)
        mate(idx[i], idx[i + 1]);

    check_integrity();
    selection();
    // check_integrity();
}

Genotype &Pool::best()
{
    return population[0];
}

void Pool::info()
{
    for (auto &G : population)
        cout << G << endl;
    cout << endl;
}