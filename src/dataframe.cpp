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
bool DataFrame::read_csv(const std::string &filename, char sep, bool header, bool index) {
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
    int row_index = 0;
    while (getline(file, line)) {
        stringstream ss(Utils::rstrip(line));
        string cell;
        vector<double> row_data;
        int col_index = 0;
        while (getline(ss, cell, sep)) {
            if (row_index == 0 && header) {
                if (this->index_name == "index") { this->index_name = cell; }
                else { this->columns.push_back(cell); }
                this->max_column_length = max(this->max_column_length, static_cast<int>(cell.length()));
            } else if (col_index == 0 && index) {
                this->index.push_back(cell);
                this->max_index_length = max(this->max_index_length, static_cast<int>(cell.length()));
            } else {
                if (cell.empty()) {
                    row_data.push_back(std::numeric_limits<double>::quiet_NaN());
                } else {
                    row_data.push_back(stod(cell));
                }
            }
            col_index++;
        }
        header = false;
        if (line.back() == ',') {
            row_data.push_back(std::numeric_limits<double>::quiet_NaN());
        }
        if (row_data.size() > 0) {
            if (this->data.rows() == 0) {
                this->data.resize(1, row_data.size());
            } else {
                this->data.conservativeResize(this->data.rows() + 1, NoChange);
            }
            this->data.row(row_index) = Map<VectorXd>(row_data.data(), row_data.size());
            row_index++;
        }
    }
    this->max_index_length = max(static_cast<int>(this->index_name.length()), this->max_index_length);
    return true;
}