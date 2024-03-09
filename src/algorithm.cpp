#include <future>
#include <numeric>

#include "algorithm.h"

// 定义列之间的算术运算函数
VectorXd Algorithm::column_operate(const MatrixXd& matrix, int col1, int col2, std::string op) {
    if (op == "add") {
        return matrix.col(col1) + matrix.col(col2);
    } else if (op == "subtract") {
        return matrix.col(col1) - matrix.col(col2);
    } else if (op == "multiply") {
        return matrix.col(col1).array() * matrix.col(col2).array();
    } else if (op == "divide") {
        return matrix.col(col1).array() / matrix.col(col2).array();
    } else {
        throw std::invalid_argument("Invalid operation");
    }
}

/**
 * @brief Calculate the Pearson correlation coefficient of two vectors
 * 
 * @param x
 * @param y 
 * @return correlation coefficient 
 */
double Algorithm::pearson(const VectorXd& x, const VectorXd& y) {
    // "The size of x and y must be the same."
    assert(x.size() == y.size());
    double mean_x = x.mean();
    double mean_y = y.mean();
    double cov_xy = ((x.array() - mean_x) * (y.array() - mean_y)).mean();
    double std_x = sqrt(((x.array() - mean_x) * (x.array() - mean_x)).mean());
    double std_y = sqrt(((y.array() - mean_y) * (y.array() - mean_y)).mean());
    double pearson_corr = cov_xy / (std_x * std_y);
    return pearson_corr;
};

/**
 * @brief Calculate the Spearman correlation coefficient of two vectors
 * 
 * @param x
 * @param y 
 * @return correlation coefficient 
 */
double Algorithm::spearman(const VectorXd& x, const VectorXd& y) {
    VectorXd rank_x = x;
    VectorXd rank_y = y;
    // Calculate ranks
    std::vector<int> index(x.size());
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [&](int i, int j) { return x(i) < x(j); });
    for (int i = 0; i < x.size(); ++i)
        rank_x(index[i]) = i;
    std::iota(index.begin(), index.end(), 0);
    std::sort(index.begin(), index.end(), [&](int i, int j) { return y(i) < y(j); });
    for (int i = 0; i < y.size(); ++i)
        rank_y(index[i]) = i;
    return pearson(rank_x, rank_y);
}

/**
 * @brief Calculate the Kendall correlation coefficient of two vectors
 * 
 * @param x vector
 * @param y vector
 * @return correlation coefficient 
*/
double Algorithm::kendall(const VectorXd& x, const VectorXd& y) {
    int n = x.size();
    int P = 0, Q = 0;

    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double x_diff = x(i) - x(j);
            double y_diff = y(i) - y(j);

            if (x_diff * y_diff > 0) P++; // 同号，意味着一致对
            else if (x_diff * y_diff < 0) Q++; // 异号，意味着不一致对
        }
    }

    return static_cast<double>(P - Q) / static_cast<double>(n * (n - 1) / 2);
}