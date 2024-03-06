#include "stablepairs.h"

StablePairs::StablePairs() {
    options = new StableOptions();
}
StablePairs::StablePairs(StableOptions *opts) {
    options = opts;
    if (Utils::exists(options->target)) {
        reverse = true;
    }
}
StablePairs::~StablePairs() {
    if (options)
        delete options;
}

/**
 * @brief Only calculate stable gene pairs
 * 
 * @return true 
 * @return false 
 */
bool StablePairs::getPairsStable() {
    DataFrame* source = new DataFrame(options->expression);

    if (source->data.rows() == 0) {
        std::cout << "Expression file is empty." << std::endl;
        return false;
    }

    for (int i = 0; i < source->data.cols(); ++i) {
        for (int j = i + 1; j < source->data.cols(); ++j) {
            double ratio = (source->data.col(i).array() > source->data.col(j).array()).cast<double>().mean();
            if (ratio > options->ratio) {
                pairs.push_back(std::make_tuple(source->columns[i], source->columns[j], ratio, 0.0));
            } else if (1 - ratio > options->ratio) {
                pairs.push_back(std::make_tuple(source->columns[j], source->columns[i], 1 - ratio, 0.0));
            }
        }
    }
    std::cout << "Successfully calculated all stable gene pairs." << std::endl;
    return true;
}

/**
 * @brief Calculate stable and reverse gene pairs
 * 
 * @return true 
 * @return false 
 */
bool StablePairs::getPairsReverse() {
    DataFrame* source = new DataFrame(options->expression);

    assert(Utils::exists(options->target));
    DataFrame* target = new DataFrame(options->target);

    if (source->data.rows() == 0 || target->data.rows() == 0) {
        std::cout << "Expression file or target file is empty." << std::endl;
        return false;
    }

    for (int i = 0; i < source->data.cols(); ++i) {
        for (int j = i + 1; j < source->data.cols(); ++j) {
            double ratio = (source->data.col(i).array() > source->data.col(j).array()).cast<double>().mean();
            double rev = (target->data.col(i).array() < target->data.col(j).array()).cast<double>().mean();
            if (ratio > options->ratio && rev > options->revRatio) {
                pairs.push_back(std::make_tuple(source->columns[i], source->columns[j], ratio, rev));
            } else if (1 - ratio > options->ratio && 1 - rev > options->revRatio) {
                pairs.push_back(std::make_tuple(source->columns[j], source->columns[i], 1 - ratio, 1 - rev));
            }
        }
    }
    std::cout << "Successfully calculated all stable and reverse gene pairs." << std::endl;
    return true;
}
/**
 * @brief Find stable gene pairs
 * 
 * @return true 
 * @return false 
 */
bool StablePairs::getPairs() {
    if (reverse) {
        return getPairsReverse();
    } else {
        return getPairsStable();
    }
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
    outputFile << "source" << outDelim << "target" << outDelim << "ratio(source>target)" << outDelim << "reverse(source<target)\n";
    for (const auto& pair : pairs) {
        outputFile << std::get<0>(pair) << outDelim << std::get<1>(pair) << outDelim << std::get<2>(pair) << "\n";
    }

    // 关闭文件
    outputFile.close();
    return true;
}