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

    const size_t inputs = 2, outputs = 1;

    auto pool = Pool(1024, inputs, outputs);

    for (auto i = 0; i < 10000; ++i)
    {
        auto error = pool.best().fitness;
        if (error > -0.0001)
            break;
        cout << "Epoch: " << i << " error: " << error << '\n';
        pool.epoch();
    }
    cout << pool.best() << "\n\n";
    cout << pool.best()({0, 0})[0] << '\n';
    cout << pool.best()({0, 1})[0] << '\n';
    cout << pool.best()({1, 0})[0] << '\n';
    cout << pool.best()({1, 1})[0] << '\n';

    cout << endl;
}
