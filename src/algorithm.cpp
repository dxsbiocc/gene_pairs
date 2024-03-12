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
double Algorithm::calculatePearsonCorrelation(const VectorXd& x, const VectorXd& y) {
    // The size of x and y must be the same.
    assert(x.size() == y.size());
    double mean_x = x.mean();
    double mean_y = y.mean();
    double cov_xy = ((x.array() - mean_x) * (y.array() - mean_y)).mean();
    double std_x = sqrt(((x.array() - mean_x) * (x.array() - mean_x)).mean());
    double std_y = sqrt(((y.array() - mean_y) * (y.array() - mean_y)).mean());
    double pearson_corr = cov_xy / (std_x * std_y);
    return pearson_corr;
}

double Algorithm::calculatePearsonCorrelationWithNaN(const VectorXd& x, const VectorXd& y) {
    assert(x.size() == y.size());

    double sumX = 0, sumY = 0;
    double sumXSq = 0, sumYSq = 0;
    double sumXY = 0;
    int count = 0;

    for (int i = 0; i < x.size(); ++i) {
        if (std::isnan(x[i]) || std::isnan(y[i])) continue;
        sumX += x[i];
        sumY += y[i];
        sumXSq += x[i] * x[i];
        sumYSq += y[i] * y[i];
        sumXY += x[i] * y[i];
        count++;
    }

    if (count < 2) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    double meanX = sumX / count;
    double meanY = sumY / count;
    double covariance = (sumXY / count - meanX * meanY);
    double varianceX = (sumXSq / count - meanX * meanX);
    double varianceY = (sumYSq / count - meanY * meanY);

    if (varianceX == 0 || varianceY == 0) {
        return std::numeric_limits<double>::quiet_NaN(); // Avoid division by zero
    }

    return covariance / (std::sqrt(varianceX) * std::sqrt(varianceY));
}

double Algorithm::calculatePearsonCorrelationVectorized(const Eigen::VectorXd& x, const Eigen::VectorXd& y) {
    assert(x.size() == y.size());

    // 计算有效元素的数量
    Eigen::Array<bool, Eigen::Dynamic, 1> mask = (x.array().isNaN() + y.array().isNaN()) == 0;
    int count = mask.count();
    if (count < 2) {
        return std::numeric_limits<double>::quiet_NaN();
    }

    Eigen::VectorXd new_x = mask.select(x, 0);
    Eigen::VectorXd new_y = mask.select(y, 0);
    // 使用mask进行过滤计算
    double sumX = new_x.array().sum();
    double sumY = new_y.array().sum();
    double sumXSq = new_x.array().square().sum();
    double sumYSq = new_y.array().square().sum();
    double sumXY = (new_x.array() * new_y.array()).sum();

    double meanX = sumX / count;
    double meanY = sumY / count;

    double covariance = (sumXY - sumX * meanY) / count;
    double varianceX = (sumXSq - sumX * meanX) / count;
    double varianceY = (sumYSq - sumY * meanY) / count;
    
    if (varianceX == 0 || varianceY == 0) {
        return std::numeric_limits<double>::quiet_NaN(); // Avoid division by zero
    }

    return covariance / (std::sqrt(varianceX) * std::sqrt(varianceY));
}
/**
 * @brief Calculate the Spearman correlation coefficient of two vectors
 * 
 * @param x
 * @param y 
 * @return correlation coefficient 
 */
double Algorithm::calculateSpearmanCorrelation(const VectorXd& x, const VectorXd& y) {
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
    return calculatePearsonCorrelation(rank_x, rank_y);
}

/**
 * @brief Calculate the Kendall correlation coefficient of two vectors
 * 
 * @param x vector
 * @param y vector
 * @return correlation coefficient 
*/
double Algorithm::calculateKendallCorrelation(const VectorXd& x, const VectorXd& y) {
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