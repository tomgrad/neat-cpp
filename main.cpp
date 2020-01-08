// Based on "Evolving Neural Networks through Augmenting Topologies"
// by Kenneth O. Stanley and Risto Miikkulainen
// Evolutionary Computation 10(2): 99-127
// C++ implementation by tomasz.gradowski@pw.edu.pl

#include <iostream>
#include <array>
#include <cmath> // exp

#include "genotype.h"

using namespace std;

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
    auto out = G(in);

    for (auto o : out)
        cout << o << ' ';
    cout << endl;

    // auto rand_uni_weight = [&]() {
    //     std::uniform_real_distribution<double> distribution(-1.0, 1.0);
    //     return distribution(rng);
    // };

    // std::cout << rand_uni_weight() << std::endl;
}
