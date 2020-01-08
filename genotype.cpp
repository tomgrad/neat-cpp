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

    unsigned innov = 0;
    for (size_t i = 0; i < inputs + 1; ++i) // inputs + bias
        for (size_t j = 0; j < outputs; ++j)
            connections.push_back(ConnectGene{i, j, innov++});

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

void Genotype::mutate_weight()
{
}

// In the add node mutation, an existing connection is split and the new node
// placed where the old connection used to be.The old connection is disabled
// and two new connections are added to the genome.The new connection leading
// into the new node receives a weight of 1, and the new connection leading
// out receives the same weight as the old connection.
void Genotype::mutate_add_node()
{
}

// In the add connection mutation, a single new connection gene with a random
// weight is added connecting two previously unconnected nodes.
void Genotype::mutate_add_connection()
{
}