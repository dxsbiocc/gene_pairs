#ifndef GENEPAIRS_H
#define GENEPAIRS_H

#include <string>
#include <vector>

#include "utils.h"
#include "dataframe.h"


struct StableOptions {
    std::string expression;
    std::string target = "";
    std::string output = "output.csv";
    double ratio = 0.9;
    double revRatio = 0.6;
};

class StablePairs
{
private:
    bool getPairsStable();
    bool getPairsReverse();
public:
    
    std::vector<std::tuple<std::string, std::string, double, double>> pairs;
    StableOptions *options = NULL;
    bool reverse = false;

    StablePairs();
    StablePairs(StableOptions* opts);
    ~StablePairs();


    bool getPairs();
    bool writePairs();
};
#endif