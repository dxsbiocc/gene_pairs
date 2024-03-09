#include "stablepairs.h"

StablePairs::StablePairs() {
    options = new StableOptions();
}
StablePairs::StablePairs(StableOptions *opts) {
    options = opts;
    source = new DataFrame(options->expression);
    srows = source->data.rows();
    lowerBound = static_cast<int>(std::ceil(options->ratio * srows));

    if (options->block < 10) {
        options->block = source->data.cols();
    }

    if(!options->target.empty() and Utils::exists(options->target)) {
        target = new DataFrame(options->target);
        trows = target->data.rows();
        reverseBound = static_cast<int>(std::ceil(options->revRatio * trows));
    }

    if (options->output.empty()) {
        options->output = Utils::dirname(options->expression) + "/output.txt";
    }
}
StablePairs::~StablePairs() {
    if (options != nullptr) {
        delete options;
        options = nullptr;  
    } 
    if (source != nullptr) {
        delete source;
        source = nullptr;
    }
    if (target != nullptr) {
        delete target;
        target = nullptr;
    }
}

/**
 * @brief Only calculate stable gene pairs
 * 
 * @return true 
 * @return false 
 */
bool StablePairs::getPairsStable() {
    if (source->data.rows() == 0) {
        std::cout << "[Stable Pairs] - Expression file is empty." << std::endl;
        return false;
    }
    std::cout << "[Stable Pairs] - Begin the search for stable gene pairs." << std::endl;
    int i, j, count;
    omp_set_num_threads(options->threads);
    #pragma omp parallel for collapse(2) private(i, j, count) schedule(static, options->block)
    for (i = 0; i < source->data.cols(); ++i) {
        for (j = i + 1; j < source->data.cols(); ++j) {
            count = (source->data.col(i).array() > source->data.col(j).array()).count();
            if (count > lowerBound) {
                #pragma omp critical
                pairs.push_back({i, j, count});
            } else if (srows - count > lowerBound) {
                #pragma omp critical
                pairs.push_back({j, i, srows - count});
            }
        }
    }
    std::cout << "[Stable Pairs] - Successfully calculated all stable gene pairs." << std::endl;
    return true;
}

/**
 * @brief Calculate stable and reverse gene pairs
 * 
 * @return true 
 * @return false 
 */
bool StablePairs::getPairsReverse() {
    if (source->data.rows() == 0 || target->data.rows() == 0) {
        std::cout << "[Stable Pairs] - Expression file or target file is empty." << std::endl;
        return false;
    }

    std::cout << "[Stable Pairs] - Begin the search for stable and reversed gene pairs." << std::endl;
    omp_set_num_threads(options->threads);
    int i, j, percent, rev;
    #pragma omp parallel for collapse(2) private(i, j, percent, rev) schedule(static, options->block)
    for (i = 0; i < source->data.cols(); ++i) {
        for (j = i + 1; j < source->data.cols(); ++j) {
            percent = (source->data.col(i).array() > source->data.col(j).array()).count();
            rev = (target->data.col(i).array() < target->data.col(j).array()).count();
            if (percent > lowerBound && rev > reverseBound) {
                #pragma omp critical
                pairs.push_back({i, j, percent, rev});
            } else if (srows - percent > lowerBound && trows - rev > reverseBound) {
                #pragma omp critical
                pairs.push_back({j, i, srows - percent, trows - rev});
            }
        }
    }
    std::cout << "[Stable Pairs] - Successfully calculated all stable and reverse gene pairs." << std::endl;
    return true;
}

/**
 * @brief Find stable gene pairs
 * 
 * @return true 
 * @return false 
 */
bool StablePairs::getPairs() {
    bool success;
    Timer timer = Timer();
    if (target != nullptr) {
        success = getPairsReverse();
    } else {
        success = getPairsStable();
    }
    std::cout << "[Stable Pairs] - The calculation time is: " << timer << std::endl;
    return success;
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
        std::cerr << "[Stable Pairs] - Failed to open file." << std::endl;
        return false;
    }
    std::cout << "[Stable Pairs] - Total number of gene pairs: " << pairs.size() << std::endl;
    char outDelim = Utils::getDelim(options->output);
    std::cout << "[Stable Pairs] - Start writing the results to " << options->output << std::endl;
    outputFile << "source" << outDelim << "target" << outDelim << "ratio(source>target)" << outDelim << "reverse(source<target)\n";
    if (target != nullptr) {
        for (const auto& pair : pairs) {
            outputFile << source->columns[pair[0]] << outDelim << source->columns[pair[1]] << outDelim << 1.0 * pair[2] / srows \
                << outDelim << 1.0 * pair[3] / trows << "\n";
        }
    } else {
        for (const auto& pair : pairs) {
            outputFile << source->columns[pair[0]] << outDelim << source->columns[pair[1]] << outDelim << 1.0 * pair[2] / srows << outDelim << "0\n";
        }
    }
    // 关闭文件
    outputFile.close();
    std::cout << "[Stable Pairs] - Writing is completed." << std::endl;
    return true;
}