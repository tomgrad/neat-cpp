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
    GeneType type = GeneType::Hidden;
    double value = 0;
    double local_field = 0;
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
    unsigned innov; // innovation number

    ConnectGene(size_t in, size_t out, unsigned innov) : in(in), out(out), innov(innov) {}
    ConnectGene() {} // wymagane przez connections.resize()
};

class Genotype
{

public:
    Genotype(const size_t inputs, const size_t outputs);
    Genotype(){}; // wymagane przez population.resize() w selekcji
    std::vector<double> operator()(const std::vector<double> &in);

    void randomize();

    std::vector<ConnectGene> connections;
    std::vector<NodeGene> nodes;
    static std::mt19937 rng;
    static unsigned next_innov_number;
    double fitness = -1;

private:
    size_t inputs;
    size_t outputs;
};

std::ostream &operator<<(std::ostream &os, const Genotype &G);