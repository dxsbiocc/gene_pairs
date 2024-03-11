#include "corrpairs.h"

/**
 * @brief Construct a new Corr Pairs:: Corr Pairs object
 * 
 */
CorrPairs::CorrPairs() {
    options = new CorrOptions();
}

/**
 * @brief Construct a new Corr Pairs:: Corr Pairs object
 * 
 * @param opts 
 */
CorrPairs::CorrPairs(CorrOptions *opts) {
    options = opts;
    source = new DataFrame(options->expression);
    if (Utils::exists(options->target))
        target = new DataFrame(options->target);
    // correlation algorithm
    if (options->method == "pearson") {
        func = Algorithm::pearson;
    } else if (options->method == "spearman") {
        func = Algorithm::spearman;
    } else if (options->method == "kendall") {
        func = Algorithm::kendall;
    } else {
        throw std::invalid_argument("Invalid method." + options->method);
    }
    // block size for thread
    if (options->block < 10) {
        options->block = source->data.cols();
    }
    // output filename
    if (options->output.empty()) {
        options->output = Utils::dirname(options->expression) + "/output.txt";
    }

    threshold = static_cast<int>(options->threshold * 1000);
}

/**
 * @brief Destroy the Corr Pairs:: Corr Pairs object
 * 
 */
CorrPairs::~CorrPairs() {
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
 * @brief Calculate correlation between features of the same type
 * 
 * @return true 
 * @return false 
 */
bool CorrPairs::getCommonPairs() {
    if (source->data.rows() == 0) {
        std::cout << "[Common Pairs] - Expression file is empty." << std::endl;
        return false;
    }
    std::cout << "[Common Pairs] - Start identifying pairs of related features for the same data." << std::endl;
    int i, j;
    int corr;
    omp_set_num_threads(options->threads);
    #pragma omp parallel for collapse(2) private(i, j, corr) schedule(static, options->block)
    for (i = 0; i < source->data.cols(); ++i) {
        for (j = 0; j < source->data.cols(); ++j) {
            if (j <= i) continue;
            corr = static_cast<int>(std::round(func(source->data.col(i), source->data.col(j)) * 1000));
            if (abs(corr) > threshold) {
                #pragma omp critical
                pairs.push_back({i, j, corr});
            }
        }
    }
    std::cout << "[Common Pairs] - Successfully calculated all related features." << std::endl;
    return true;
}

/**
 * @brief Calculate correlations between different types of features (eg. corr(expr(a), drug(d)))
 * 
 * @return true 
 * @return false 
 */
bool CorrPairs::getCrossPairs() {
    std::cout << "[Cross Pairs] - Start identifying correlations between different types of features." << std::endl;
    if (source->data.rows() != target->data.rows()) {
        std::cout << "[Cross Pairs] - The number of data lines in the two files is inconsistent." << std::endl;
        return false;
    }
    int i, j;
    int corr;
    omp_set_num_threads(options->threads);
    #pragma omp parallel for collapse(2) private(i, j, corr) schedule(static, options->block)
    for (i = 0; i < source->data.cols(); ++i) {
        for (j = 0; j < target->data.cols(); ++j) {
            corr = static_cast<int>(std::round(func(source->data.col(i), target->data.col(j)) * 1000));
            if (abs(corr) > threshold) {
                #pragma omp critical
                pairs.push_back({i, j, corr});
            }
        }
    }
    std::cout << "[Cross Pairs] - Successfully calculated all related features." << std::endl;
    return true;
}

/**
 * @brief Calculate correlations between expression relationships between gene pairs and other features.
 *        such as, corr((expr(a) - expr(b)), drug(d))
 * 
 * @return true 
 * @return false 
 */
bool CorrPairs::getPairsCross() {
    std::cout << "[Pairs Cross] - Begin to recognize the correlation of homotypic feature pairs with another type of features." << std::endl;
    if (source->data.rows() != target->data.rows()) {
        return false;
    }
    int k, i, j;
    VectorXd res;
    int corr;
    omp_set_num_threads(options->threads);
    #pragma omp parallel for collapse(3) private(k, i, j, res, corr) schedule(static, options->block)
    for (k = 0; k < target->data.cols(); ++k) {
        for (i = 0; i < source->data.cols(); ++i) {
            for (j = 0; j < source->data.cols(); ++j) {
                if (j <= i) continue;
                if (i == 0 && j == 1) {
                    #pragma omp critical
                    std::cout << "[Pairs Cross] - Feature: " << target->columns[k] << std::endl;
                }
                res = Algorithm::column_operate(source->data, i, j, options->operation);
                corr = static_cast<int>(std::round(func(res, target->data.col(k)) * 1000));
                if (!isnan(corr) && abs(corr) > threshold)
                    #pragma omp critical
                    {
                        pairs.push_back({k, i, j, corr});
                    }
            }
        }
    }
    std::cout << "[Pairs Cross] - Successfully calculated all correlation gene pairs." << std::endl;
    return true;
}

/**
 * @brief External interface, task scheduling
 * 
 * @return true 
 * @return false 
 */
bool CorrPairs::getPairs() {
    bool success;
    Timer timer = Timer();
    if (options->analysis == "common") {
        success = getCommonPairs();
    } else if (options->analysis == "cross") {
        success = getCrossPairs();
    } else if (options->analysis == "pairs") {
        success = getPairsCross();
    } else {
        std::cout << "[Correlation Pairs] - This type of analysis is not supported: " << timer << std::endl;
        return false;
    }
    std::cout << "[Correlation Pairs] - The calculation time is: " << timer << std::endl;
    return success;
}

bool CorrPairs::writePairs() {
    std::ofstream outputFile(options->output);
    if (!outputFile.is_open()) {
        std::cerr << "[Correlation Pairs] - Failed to open file." << std::endl;
        return false;
    }
    std::cout << "[Correlation Pairs] - Total number of gene pairs: " << pairs.size() << std::endl;
    char outDelim = Utils::getDelim(options->output);
    std::cout << "[Correlation Pairs] - Start writing the results to " << options->output << std::endl;
    
    if (options->analysis == "common") {
        outputFile << "source" << outDelim << "target" << outDelim << "corr" << std::endl;
        for (const auto& pair : pairs)
            outputFile << source->columns[pair[0]] << outDelim << source->columns[pair[1]] << outDelim << 1.0 * pair[2] / 1000 << std::endl;
    } else if (options->analysis == "cross") {
        outputFile << "source" << outDelim << "target" << outDelim << "corr" << std::endl;
        for (const auto& pair : pairs)
            outputFile << source->columns[pair[0]] << outDelim << target->columns[pair[1]] << outDelim << 1.0 * pair[2] / 1000 << std::endl;
    } else if (options->analysis == "pairs") {
        outputFile << "feature" << outDelim << "source" << outDelim << "target" << outDelim << \
                    "corr(source" << Utils::getOperation(options->operation) << "target)" << std::endl;
        for (const auto& pair : pairs)
            outputFile << target->columns[pair[0]] << outDelim << source->columns[pair[1]] << outDelim << \
                source->columns[pair[2]] << outDelim << 1.0 * pair[3] / 1000 << std::endl;
    }

    // 关闭文件
    outputFile.close();
    std::cout << "[Correlation Pairs] - Writing is completed." << std::endl;
    return true;
}