#include "stablepairs.h"

StablePairs::StablePairs() {
    options = new StableOptions();
}
StablePairs::StablePairs(StableOptions *opts) {
    options = opts;
}
StablePairs::~StablePairs() {
    if (options)
        delete options;
}

/**
 * @brief Find stable gene pairs
 * 
 * @return true 
 * @return false 
 */
bool StablePairs::getPairs() {
    DataFrame df;
    char inDelim = Utils::getDelim(options->expression);
    bool readable = df.read_csv(options->expression, inDelim);
    if (!readable) {
        return false;
    }
    MatrixXd *mat = &df.data;
    for (int i = 0; i < mat->cols(); ++i) {
        for (int j = i + 1; j < mat->cols(); ++j) {
            double ratio = (mat->col(i).array() > mat->col(j).array()).cast<double>().mean();
            if (ratio > options->ratio) {
                pairs.push_back(std::make_tuple(df.columns[i], df.columns[j], ratio));
            } else if (1 - ratio > options->ratio) {
                pairs.push_back(std::make_tuple(df.columns[j], df.columns[i], 1 - ratio));
            }
        }
    }
    return true;
}

/**
 * @brief Write gene pairs to file
 * 
 * @return true 
 * @return false 
 */
bool StablePairs::writePairs() {
    std::ofstream outputFile(options->output);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return false;
    }
    char outDelim = Utils::getDelim(options->output);
    outputFile << "source" << outDelim << "target" << outDelim << "ratio(source>target)\n";
    for (const auto& pair : pairs) {
        outputFile << std::get<0>(pair) << outDelim << std::get<1>(pair) << outDelim << std::get<2>(pair) << "\n";
    }

    // 关闭文件
    outputFile.close();
    return true;
}