#ifndef CORRPAIRS_H
#define CORRPAIRS_H

#include <string>

#include "utils.h"
#include "algorithm.h"
#include "dataframe.h"


struct CorrOptions
{
    std::string expression;
    std::string target = "";
    std::string output = "output.csv";
    Utils::Method method = Utils::Method::PEARSON;
    Utils::Operation operation = Utils::Operation::SUBTRACT;
    double threshold = 0.3;
};

class CorrPairs
{
private:
    /* data */
public:
    // feature, source, target, corr
    std::vector<std::tuple<std::string, std::string, std::string, double>> pairs;
    CorrOptions* options;

    CorrPairs();
    CorrPairs(CorrOptions *opts);
    ~CorrPairs();

    void getPairsOne(DataFrame* source, const VectorXd& vec, const string &vec_name);
    bool getPairs();
    bool writePairs();
};

#endif