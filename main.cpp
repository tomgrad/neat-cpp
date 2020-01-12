// Based on "Evolving Neural Networks through Augmenting Topologies"
// by Kenneth O. Stanley and Risto Miikkulainen
// Evolutionary Computation 10(2): 99-127
// C++ implementation by tomasz.gradowski@pw.edu.pl

#include <iostream>

#include "pool.h"
#include <functional>
using namespace std;

int main(/* int argv, char *argc[] */)
{

    // auto XOR = [&](auto x, auto y) { return x + y; };

    const size_t inputs = 2, outputs = 1;

    auto pool = Pool(4, inputs, outputs);
    // auto fitness=[&]
    pool.info();
    pool.epoch();
    pool.info();

    pool.epoch();
    pool.info();

    // pool.mate(0, 1);
    // pool.mate(2, 3);

    // pool.info();

    // cout << p << endl;
    // Genotype G1(inputs, outputs);
    // Genotype G2 = G1;

    // G.mutate_weights(0.8, 0.9);
    // G1.mutate_add_node(0.5);
    // G2.mutate_add_node(0.5);

    // G.mutate_add_connection(0.5);
    // cout << G1 << endl;
    // cout << G2 << endl;

    // vector<double> in{0, 1};
    // auto out = G(in);

    // for (auto o : out)
    //     cout << o << ' ';
    cout << endl;
}
