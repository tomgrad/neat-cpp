#pragma once

#include "genotype.h"

class Pool
{
public:
    Pool(size_t inputs, size_t outputs);
    std::vector<Genotype> population;
    void mate(size_t, size_t);
};
