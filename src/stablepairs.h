#ifndef GENEPAIRS_H
#define GENEPAIRS_H

#include <string>
#include <vector>

#include "utils.h"
#include "dataframe.h"


struct StableOptions {
    std::string expression;
    std::string output = "output.csv";
    double ratio = 0.9;
};

class StablePairs
{
private:
public:
    
    std::vector<std::tuple<std::string, std::string, double>> pairs;
    StableOptions *options = NULL;

    StablePairs();
    StablePairs(StableOptions* opts);
    ~StablePairs();


    bool getPairs();
    bool writePairs();
};
#endif