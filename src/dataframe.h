#ifndef DATAFRAME_H
#define DATAFRAME_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <Eigen/Dense>

#include "utils.h"

using namespace Eigen;
using namespace std;

// 定义包含行名和列名的矩阵结构体
class DataFrame {
public:
    MatrixXd data; // 数据矩阵
    vector<string> index; // 行名
    vector<string> columns; // 列名
    string index_name = "index";
private:
        int max_column_length = -1;
        int max_index_length = this->index_name.length();
        char fill_char = ' ';

public:
    DataFrame();
    DataFrame(const string& filename);
    DataFrame(const MatrixXd& data, const vector<string>& index, const vector<string>& columns);
    // 从文件中读取数据
    bool read_csv(const string& filename, char sep=',', bool header=true, bool index=true);
    // 运算符重载
    friend ostream& operator<<(ostream& os, const DataFrame& df) {
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
    DataFrame operator+(const DataFrame& other) const {
        return DataFrame(data + other.data, index, columns);
    }

    // 减法运算符重载
    DataFrame operator-(const DataFrame& other) const {
        return DataFrame(data - other.data, index, columns);
    }

    // 乘法运算符重载
    DataFrame operator*(const DataFrame& other) const {
        return DataFrame(data.cwiseProduct(other.data), index, columns);
    }

    // 除法运算符重载
    DataFrame operator/(const DataFrame& other) const {
        return DataFrame(data.array() / other.data.array(), index, columns);
    }
};
#endif