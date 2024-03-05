#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <map>
#include <tuple>
#include <vector>
#include <string>

#include <Eigen/Dense>

#include "utils.h"
#include "dataframe.h"
#include "threadpool.h"

using namespace std;
using namespace Eigen;

typedef tuple<string, string, double> GenePair;

namespace Algorithm {

    // functions
    double spearman(const VectorXd& x, const VectorXd& y);
    double pearson(const VectorXd& x, const VectorXd& y);
    double kendall(const VectorXd& x, const VectorXd& y);
    double correlation(const VectorXd& x, const VectorXd& y, Utils::Method method);

    VectorXd column_operate(const MatrixXd& matrix, int col1, int col2, Utils::Operation op);
}
#endif