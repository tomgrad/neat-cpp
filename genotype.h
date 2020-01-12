#pragma once

#include <random>
#include <vector>

// transfer function
double f(const double x, const double slope);

enum class GeneType
{
    Input,
    Output,
    Hidden
};

struct NodeGene
{
    size_t id; // TODO: usunąć
    GeneType type = GeneType::Hidden;
    double value = 0;
    bool cached = false;
};

struct ConnectGene
{
    size_t in;
    size_t out;
    double weight;
    bool enabled = true;
    // Whenever a new gene appears (through structural mutation), a global
    // innovation number is incremented and assigned to that gene.
    // The innovation numbers thus represent a chronology of the appearance
    // of every gene in the system. (...) In the future, whenever these
    // genomes mate, the offspring will inherit the same innovation numbers
    // on each gene; innovation numbers are never changed.
    int innov; // innovation number

    ConnectGene(size_t in, size_t out, unsigned innov) : in(in), out(out), innov(innov)
    {
    }
};

class Genotype
{

public:
    Genotype(const size_t inputs, const size_t outputs);
    std::vector<double> operator()(std::vector<double> &in);

    void randomize();

    std::vector<ConnectGene> connections;
    std::vector<NodeGene> nodes;
    static std::mt19937 rng;
    static size_t num_nodes;
    static unsigned next_innov_number;

private:
    size_t inputs;
    size_t outputs;
    double eval(size_t idx);
};

std::ostream &operator<<(std::ostream &os, const Genotype &G);