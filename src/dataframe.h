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
    size_t nrows = 0;
    size_t ncols = 0;

    int max_column_length = -1;
    int max_index_length = this->index_name.length();
    char fill_char = ' ';

    bool getSize(const string& filename, const char delimiter);

public:
    DataFrame();
    DataFrame(const string& filename);
    DataFrame(const MatrixXd& data, const vector<string>& index, const vector<string>& columns);
    // 从文件中读取数据
    bool read_csv(const string& filename, const char delimiter=',', bool header=true, bool index=true);
    // 运算符重载
    friend ostream& operator<<(ostream& os, const DataFrame& df);
    friend DataFrame operator+(DataFrame& df, const DataFrame& other);
    friend DataFrame operator-(DataFrame& df, const DataFrame& other);
    friend DataFrame operator*(DataFrame& df, const DataFrame& other);
    friend DataFrame operator/(DataFrame& df, const DataFrame& other);
};
#endif