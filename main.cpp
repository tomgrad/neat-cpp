// Based on "Evolving Neural Networks through Augmenting Topologies"
// by Kenneth O. Stanley and Risto Miikkulainen
// Evolutionary Computation 10(2): 99-127
// C++ implementation by tomasz.gradowski@pw.edu.pl

#include <iostream>
#include <vector>
#include <array>
#include <cmath> // exp
#include <random>

using namespace std;

// TODO: lokalny rng (thread-local) przekazywany przez referencję do obiektów.
std::mt19937 rng;

// sigmoidal transfer function
double f(const double x, const double slope = 4.9)
{
    return 1 / (1 + exp(-slope * x));
}

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

    ConnectGene(size_t in, size_t out, unsigned innov) : in(in), out(out)
    {
        randomize();
    }

    void randomize()
    {
        std::uniform_real_distribution<double> real_dist(-1.0, 1.0);
        weight = real_dist(rng);
    }
};

class Genotype
{

public:
    Genotype(const size_t inputs, const size_t outputs);

    void mutate_weight();
    void mutate_add_connection();
    void mutate_add_node();
    void input(vector<double> in);
    vector<double> output();
    void eval();

    std::vector<NodeGene> nodes;
    std::vector<ConnectGene> connections;

private:
    size_t inputs;
    size_t outputs;
    unsigned next_innov_number = 0;
    double eval(size_t idx);
};

Genotype::Genotype(const size_t inputs, const size_t outputs) : inputs(inputs), outputs(outputs)
{
    for (size_t i = 0; i < inputs + 1; ++i) // inputs + bias
        nodes.push_back(NodeGene{i, GeneType::Input});
    for (size_t i = 0; i < outputs; ++i)
        nodes.push_back(NodeGene{inputs + i, GeneType::Output});

    unsigned innov = 0;
    for (size_t i = 0; i < inputs + 1; ++i) // inputs + bias
        for (size_t j = 0; j < outputs; ++j)
            connections.push_back(ConnectGene{i, j, innov++});
}

void Genotype::input(vector<double> in)
{
    for (size_t i = 0; i < inputs; ++i)
        nodes[i].value = in[i];
    nodes[inputs].value = 1; // bias
}

vector<double> Genotype::output()
{
    vector<double> out(outputs);
    for (size_t i = 0; i < outputs; ++i)
        out[i] = nodes[i + inputs + 1].value;
    return out;
}
void Genotype::eval()
{
    // TODO: zamienic na operator() i scalić z input(*in)
    for (auto &n : nodes)
        n.cached = false;
}

double Genotype::eval(size_t idx)
{
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
        if (link.out == idx)
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

class Pool
{
public:
    Pool(size_t inputs, size_t outputs);
    std::vector<Genotype> population;
    void mate(size_t, size_t);
};

Pool::Pool(size_t inputs, size_t outputs)
{
}

int main(/* int argv, char *argc[] */)
{
    const size_t inputs = 2, outputs = 1;
    Genotype G(inputs, outputs);
    vector<double> in{0, 1};
    G.input(in);
    G.eval();
    auto out = G.output();

    for (auto o : out)
        cout << o << ' ';
    cout << endl;

    // auto rand_uni_weight = [&]() {
    //     std::uniform_real_distribution<double> distribution(-1.0, 1.0);
    //     return distribution(rng);
    // };

    // std::cout << rand_uni_weight() << std::endl;
}
