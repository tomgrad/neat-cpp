#pragma once

#include <vector>
#include <random>

// sigmoidal transfer function
double f(const double x, const double slope);

enum class GeneType
{
    Input,
    Output,
    Hidden
};

struct NodeGene
{
    size_t id;
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

    // void randomize()
    // {
    //     std::uniform_real_distribution<double> real_dist(-1.0, 1.0);
    //     weight = real_dist(rng);
    // }
};

class Genotype
{

public:
    Genotype(const size_t inputs, const size_t outputs);

    void mutate_weight();
    void mutate_add_connection();
    void mutate_add_node();
    void input(std::vector<double> in);
    std::vector<double> output();
    void eval();

    std::vector<NodeGene> nodes;
    std::vector<ConnectGene> connections;

private:
    static std::mt19937 rng;
    size_t inputs;
    size_t outputs;
    unsigned next_innov_number = 0;
    double eval(size_t idx);
};
