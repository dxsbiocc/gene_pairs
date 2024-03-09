#ifndef GENEPAIRS_H
#define GENEPAIRS_H

#include <string>
#include <vector>

#include <omp.h>

#include "timer.h"
#include "utils.h"
#include "dataframe.h"


struct StableOptions {
    std::string expression;
    std::string target;
    std::string output;
    double ratio = 0.9;
    double revRatio = 0.6;
    size_t block = 0;
    size_t threads = 2;
};

class StablePairs
{
private:
    bool getPairsStable();
    bool getPairsReverse();

    int srows;         // The number of samples in the source data
    int trows;         // The number of samples in the target data
    int lowerBound;    // Lower bound for stable pairs, ratio * srows
    int reverseBound;  // Lower bound for reverse pairs, revRatio * trows

public:

    DataFrame *source = nullptr;
    DataFrame *target = nullptr;    
    StableOptions *options = nullptr;
    std::vector<std::vector<int>> pairs;

    StablePairs();
    StablePairs(StableOptions* opts);
    ~StablePairs();


    bool getPairs();
    bool writePairs();
};
#endif