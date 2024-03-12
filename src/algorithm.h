#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <limits>

#include <Eigen/Dense>

#include "utils.h"
#include "dataframe.h"
#include "threadpool.h"

using namespace std;
using namespace Eigen;

typedef tuple<string, string, double> GenePair;

namespace Algorithm {

    // functions
    double calculatePearsonCorrelation(const VectorXd& x, const VectorXd& y);
    double calculatePearsonCorrelationWithNaN(const VectorXd& x, const VectorXd& y);
    double calculatePearsonCorrelationVectorized(const VectorXd& x, const VectorXd& y);
    double calculateSpearmanCorrelation(const VectorXd& x, const VectorXd& y);
    double calculateKendallCorrelation(const VectorXd& x, const VectorXd& y);

    VectorXd column_operate(const MatrixXd& matrix, int col1, int col2, std::string op);
}
#endif