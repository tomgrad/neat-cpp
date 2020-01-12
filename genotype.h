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

    void mutate_weights(double mutation_prob, double perturbation_prob);
    void mutate_add_connection(double add_con_prob);
    void mutate_add_node(double add_node_prob);
    std::vector<ConnectGene> connections;
    std::vector<NodeGene> nodes;
    static std::mt19937 rng;
    static size_t num_nodes;

private:
    size_t inputs;
    size_t outputs;
    static unsigned next_innov_number;
    double eval(size_t idx);
};
