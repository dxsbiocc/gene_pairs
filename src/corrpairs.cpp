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
}

/**
 * @brief Destroy the Corr Pairs:: Corr Pairs object
 * 
 */
CorrPairs::~CorrPairs() {
    if (options)
        delete options;
}
/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool CorrPairs::getPairs() {
    DataFrame* source = new DataFrame(options->expression);
    DataFrame* target = new DataFrame(options->target);
    if (source->data.rows() != target->data.rows()) {
        return false;
    }

    for (int i = 0; i < target->data.cols(); ++i) {
        std::cout << "Calulate feature: " << target->columns[i] << std::endl;
        getPairsOne(source, target->data.col(i), target->columns[i]);
    }
    return true;
}

/**
 * @brief 
 * 
 * @param source 
 * @param vec 
 * @param vec_name 
 */
void CorrPairs::getPairsOne(DataFrame* source, const VectorXd& vec, const string &vec_name) {
    for (int i = 0; i < source->data.cols(); ++i) {
        for (int j = i + 1; j < source->data.cols(); ++j) {
            VectorXd res = Algorithm::column_operate(source->data, i, j, options->operation);
            double corr = Algorithm::correlation(res, vec, options->method);
            if (isnan(corr) || corr < options->threshold) continue;
            pairs.push_back(std::make_tuple(vec_name, source->columns[i], source->columns[j], corr));
        }
    }
}

bool CorrPairs::writePairs() {
    std::ofstream outputFile(options->output);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return false;
    }
    std::cout << "Total number of gene pairs: " << pairs.size() << std::endl;
    char outDelim = Utils::getDelim(options->output);
    std::cout << "Start writing the results to " << options->output << std::endl;
    outputFile << "feature" << outDelim << "source" << outDelim << "target" << outDelim << \
                "corr(source" << Utils::getOperation(options->operation) << "target)" << std::endl;
    for (const auto& pair : pairs) {
        outputFile << std::get<0>(pair) << outDelim << std::get<1>(pair) << outDelim << std::get<2>(pair) << outDelim << std::get<3>(pair)<< std::endl;
    }

    // 关闭文件
    outputFile.close();
    std::cout << "Writing is completed." << std::endl;
    return true;
}