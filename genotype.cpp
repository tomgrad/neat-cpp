#include "genotype.h"
#include <algorithm> // shuffle
#include <iostream>

using namespace std;

// sigmoidal transfer function
double f(const double x, const double slope = 4.9)
{
    return 1 / (1 + exp(-slope * x));
}

mt19937 Genotype::rng = mt19937();
unsigned Genotype::next_innov_number = 0;

Genotype::Genotype(const size_t inputs, const size_t outputs) : inputs(inputs),
                                                                outputs(outputs)
{
    for (size_t i = 0; i < inputs + 1; ++i) // inputs + bias
        nodes.push_back(NodeGene{GeneType::Input});
    for (size_t i = 0; i < outputs; ++i)
        nodes.push_back(NodeGene{GeneType::Output});

    for (size_t i = 0; i < inputs + 1; ++i) // inputs + bias
        for (size_t j = 0; j < outputs; ++j)
            connections.push_back(ConnectGene(i, j + inputs + 1, next_innov_number++));
}

void Genotype::randomize()
{
    // TODO: static distrib.
    std::uniform_real_distribution<double> real_dist(-1, 1);
    for (auto &c : connections)
        c.weight = real_dist(rng);
}

vector<double> Genotype::operator()(const vector<double> &in)
{
    // // copy inputs
    for (size_t i = 0; i < inputs; ++i)
        nodes[i].value = in[i];
    nodes[inputs].value = 1; // bias

    for (auto &n : nodes)
        n.local_field = 0;

    for (auto &c : connections)
        if (c.enabled)
            nodes[c.out].local_field += c.weight * nodes[c.in].value;

    for (auto &n : nodes)
        n.value = f(n.local_field);

    // return outputs
    vector<double> out(outputs);
    for (size_t i = 0; i < outputs; ++i)
        out[i] = nodes[i + inputs + 1].value;

    return out;
}

std::ostream &operator<<(std::ostream &os, const Genotype &G)
{
    auto flag = [&](bool enabled) {
        return enabled ? " " : "d";
    };
    os << G.fitness << " ";
    for (auto &c : G.connections)
        os << c.innov << flag(c.enabled) << ": " << c.in << "->" << c.out << " | ";
    return os;
}