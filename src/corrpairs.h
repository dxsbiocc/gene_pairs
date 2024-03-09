#ifndef CORRPAIRS_H
#define CORRPAIRS_H

#include <string>
#include <functional>

#include <omp.h>

#include "utils.h"
#include "timer.h"
#include "algorithm.h"
#include "dataframe.h"

struct CorrOptions
{
    std::string expression;
    std::string target;
    std::string output;
    std::string method;
    std::string operation;
    std::string analysis;
    double threshold = 0.3;
    size_t block = 0;
    size_t threads = 2;
};

class CorrPairs
{
private:
    /* data */
    bool getCrossPairs();
    bool getPairsCross();
    bool getCommonPairs();

    // function pointer
    std::function<double(const VectorXd&, const VectorXd&)> func;

    int threshold;

public:
    DataFrame *source = nullptr;
    DataFrame *target = nullptr;
    // feature, source, target, corr
    std::vector<std::vector<int> > pairs;
    CorrOptions *options;

    CorrPairs();
    CorrPairs(CorrOptions *opts);
    ~CorrPairs();

    bool getPairs();
    bool writePairs();
};

#endif