#include "genotype.h"

using namespace std;

// sigmoidal transfer function
double f(const double x, const double slope = 4.9)
{
    return 1 / (1 + exp(-slope * x));
}

mt19937 Genotype::rng = mt19937();

Genotype::Genotype(const size_t inputs, const size_t outputs) : inputs(inputs),
                                                                outputs(outputs)
{
    for (size_t i = 0; i < inputs + 1; ++i) // inputs + bias
        nodes.push_back(NodeGene{i, GeneType::Input});
    for (size_t i = 0; i < outputs; ++i)
        nodes.push_back(NodeGene{inputs + i, GeneType::Output});

    // unsigned innov = 0;
    for (size_t i = 0; i < inputs + 1; ++i) // inputs + bias
        for (size_t j = 0; j < outputs; ++j)
            connections.push_back(ConnectGene(i, j + inputs + 1, next_innov_number++));

    // TODO: static distrib.
    std::uniform_real_distribution<double> real_dist(-1.0, 1.0);
    for (auto &c : connections)
        c.weight = real_dist(rng);
}

vector<double> Genotype::operator()(vector<double> &in)
{
    // copy inputs
    for (size_t i = 0; i < inputs; ++i)
        nodes[i].value = in[i];
    nodes[inputs].value = 1; // bias

    // forward propagation
    for (auto &n : nodes)
        n.cached = false;
    for (size_t i = inputs + 1; i < inputs + 1 + outputs; ++i)
        eval(i);

    // return outputs
    vector<double> out(outputs);
    for (size_t i = 0; i < outputs; ++i)
        out[i] = nodes[i + inputs + 1].value;
    return out;
}

double Genotype::eval(size_t idx)
{
    // recursive function
    if (nodes[idx].cached)
        return nodes[idx].value;

    if (idx <= inputs) // wezly wejsciowe (sensory) + bias
    {
        nodes[idx].cached = true;
        return nodes[idx].value;
    }

    double local_field = 0;
    for (auto link : connections)
    {
        if (link.out == idx && link.enabled)
            local_field += eval(link.in); // rekurencja
    }
    nodes[idx].value = f(local_field);
    nodes[idx].cached = true;
    return nodes[idx].value;
}

// There was an 80% chance of a genome having its connection weights mutated,
// in which case each weight had a 90% chance of being uniformly perturbed
// and a 10% chance of being assigned a new random value.
// brak informacji w oryginalnej pracy, jak duże jest zabużenie
// można to sprawdzić w oryginalnej implementacji (tu założenie: +-0.1)
void Genotype::mutate_weights(double mutation_prob, double perturbation_prob)
{
    std::uniform_real_distribution<double> rnd_weight(-1.0, 1.0);
    std::uniform_real_distribution<double> prob(0, 1.0);

    for (auto &c : connections)
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
void Genotype::mutate_add_node(double add_prob)
{
    std::uniform_real_distribution<double> prob(0, 1.0);
    if (add_prob < prob(rng)) // brak mutacji z pstwem (1-add_prob)
        return;

    std::uniform_int_distribution<size_t> rnd_link(0, connections.size());

    auto new_id = nodes.size();
    nodes.push_back(NodeGene{new_id});
    ConnectGene *old_edge;
    do
        old_edge = &connections[rnd_link(rng)];
    while (!old_edge->enabled);

    old_edge->enabled = false;

    auto edge1 = ConnectGene(old_edge->in, new_id, next_innov_number++);
    auto edge2 = ConnectGene(new_id, old_edge->in, next_innov_number++);

    edge1.weight = old_edge->weight;
    edge2.weight = 1;

    connections.emplace_back(edge1);
    connections.emplace_back(edge2);
}

// In the add connection mutation, a single new connection gene with a random
// weight is added connecting two previously unconnected nodes.
// In smaller populations (...) the probability of a new link mutation was
// 0.05. In the larger population, the probability of adding a new link was
// 0.3, because a larger population can tolerate a larger number
// of prospective species and greater topological diversity.
void Genotype::mutate_add_connection()
{
}