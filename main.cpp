// Based on "Evolving Neural Networks through Augmenting Topologies"
// by Kenneth O. Stanley and Risto Miikkulainen
// Evolutionary Computation 10(2): 99-127
// C++ implementation by tomasz.gradowski@pw.edu.pl

#include <iostream>

#include "genotype.h"

using namespace std;

int main(/* int argv, char *argc[] */)
{
    const size_t inputs = 2, outputs = 1;
    Genotype G(inputs, outputs);
    G.mutate_weights(0.8, 0.9);
    vector<double> in{0, 1};
    auto out = G(in);

    for (auto o : out)
        cout << o << ' ';
    cout << endl;
}
