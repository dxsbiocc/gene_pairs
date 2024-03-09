#include "dataframe.h"


DataFrame::DataFrame() {}

DataFrame::DataFrame(const string& filename) {
    char delim = Utils::getDelim(filename);
    read_csv(filename, delim);
}
DataFrame::DataFrame(const MatrixXd& data, const vector<string>& index, const vector<string>& columns)
    : data(data), index(index), columns(columns) {
    max_index_length = 0;
    for (const auto& idx : index) {
        max_index_length = max(max_index_length, static_cast<int>(idx.length()));
    }

    max_column_length = 0;
    for (const auto& col : columns) {
        max_column_length = max(max_column_length, static_cast<int>(col.length()));
    }
}

/**
 * @brief load data from file
 * 
 * @param filename 
 * @param sep 
 * @param header 
 * @param index 
 * @return true 
 * @return false 
 */
bool DataFrame::read_csv(const std::string &filename, const char delimiter, bool header, bool index) {
    if (!getSize(filename, delimiter)) {
        return false;
    }
    // open file
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        // return MatrixType::Zero(0, 0);
        return false;
    }
    // read data from file
    std::string line;
    int row_offset = header ? 1 : 0;
    int col_offset = index ? 1 : 0;
    int row_index = -1;
    this->data.resize(nrows, ncols);
    this->data.setZero();
    std::cout << "Read data from file: " << filename << std::endl;
    while (getline(file, line)) {
        if (line.empty()) {
            continue; // 跳过此行
        }
        stringstream ss(Utils::rstrip(line));
        string cell;
        vector<double> row_data;
        int col_index = -1;
        while (getline(ss, cell, delimiter)) {
            if (row_index == -1 && header) {
                if (this->index_name == "index") { this->index_name = cell; }
                else { this->columns[col_index] = cell; }
                this->max_column_length = max(this->max_column_length, static_cast<int>(cell.length()));
            } else if (col_index == -1 && index) {
                this->index[row_index] = cell;
                this->max_index_length = max(this->max_index_length, static_cast<int>(cell.length()));
            } else {
                if (cell.empty()) {
                    this->data(row_index, col_index) = std::numeric_limits<double>::quiet_NaN();
                } else {
                    this->data(row_index, col_index) = stod(cell);
                }
            }
            col_index++;
        }
        header = false;
        row_index++;
    }
    file.close();
    std::cout << "File reading completed." << std::endl;
    this->max_index_length = max(static_cast<int>(this->index_name.length()), this->max_index_length);
    std::cout << "Data size: " << nrows << "x" << ncols << std::endl;
    return true;
}

//
ostream& operator<<(ostream& os, const DataFrame& df){
    os << "Matrix: " << df.index.size() << " X " << df.columns.size() << endl;
    os << setw(df.max_index_length) << setfill(df.fill_char) << df.index_name << "\t";
    for (const auto& col_name : df.columns) {
        os << setw(df.max_column_length) << setfill(df.fill_char) << col_name << "\t";
    }
    os << endl;
    int print_rows = min(static_cast<int>(df.data.rows()), 6); // 仅打印前六行
    for (int i = 0; i < print_rows; ++i) {
        os << setw(df.max_index_length) << setfill(df.fill_char) << df.index[i] << "\t";
        for (int j = 0; j < df.data.cols(); ++j) {
            os << setw(df.max_column_length) << setfill(df.fill_char) << df.data(i, j) << "\t";
        }
        os << endl;
    }
    return os;
}

// 加法运算符重载
DataFrame operator+(DataFrame& df, const DataFrame& other) {
    if ((df.data.rows() == other.data.rows()) &&
        (df.data.cols() == other.data.cols())) {
        return DataFrame(df.data + other.data, df.index, df.columns);
    } else {
        throw std::invalid_argument("The shapes of the two DataFrame objects are different.");
    }
}
// 减法运算符重载
DataFrame operator-(DataFrame& df, const DataFrame& other) {
    if ((df.data.rows() == other.data.rows()) &&
        (df.data.cols() == other.data.cols())) {
        return DataFrame(df.data - other.data, df.index, df.columns);
    } else {
        throw std::invalid_argument("The shapes of the two DataFrame objects are different.");
    }
}
// 乘法运算符重载
DataFrame operator*(DataFrame& df, const DataFrame& other) {
    if ((df.data.rows() == other.data.rows()) &&
        (df.data.cols() == other.data.cols())) {
        return DataFrame(df.data.cwiseProduct(other.data), df.index, df.columns);
    } else {
        throw std::invalid_argument("The shapes of the two DataFrame objects are different.");
    }
}
// 除法运算符重载
DataFrame operator/(DataFrame& df, const DataFrame& other) {
    if ((df.data.rows() == other.data.rows()) &&
        (df.data.cols() == other.data.cols())) {
        return DataFrame(df.data.array() / other.data.array(), df.index, df.columns);
    } else {
        throw std::invalid_argument("The shapes of the two DataFrame objects are different.");
    }
}

// private functions
bool DataFrame::getSize(const std::string& filename, const char delimiter) {
    std::ifstream file(filename); // 打开文件流

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        ++nrows;
        size_t temp_num_cols = 0;
        stringstream ss(Utils::rstrip(line));
        string cell;
        while (std::getline(ss, cell, delimiter)) {
            ++temp_num_cols;
        }
        ncols = std::max(ncols, temp_num_cols);
    }
    file.close();
    if (nrows * ncols > 0) {
        --nrows;
        --ncols;
        columns.resize(ncols);
        index.resize(nrows);
    } else {
        std::cout << "Empty file: " << filename << std::endl;
        return false;
    }
    return true;
}