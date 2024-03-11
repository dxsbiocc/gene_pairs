# Gene Pairs Analysis

## Table of Contents

- [Gene Pairs](#gene-pairs-analysis)
  - [Introduction](#introduction)
  - [Install](#install)
  - [Uasge](#usage)

## Introduction

Gene pairs with a stable relationship in the sample, which can be gene expression data, or correlations between gene expression and drug sensitivity (or other data).

## Install

### Eigen

install eigen3

```bash
git clone https://gitlab.com/libeigen/eigen.git
cd eigen
mkdir build
cd build
cmake ..
make
make install
```

### CLI11

install CLI11

```bash
git clone https://github.com/CLIUtils/CLI11.git
cd CLI11
mkdir build
cd build
cmake ..
make
make install
```

### Local

There is no need to install CLI11 and eigen header files into the system library, use local header files directly.

Clone eigen and CLI11 library from github

```bash
git clone https://gitlab.com/libeigen/eigen.git
git clone https://github.com/CLIUtils/CLI11.git
```

### Makefile

For MacOS, you should install `libomp` and `llvm`

```bash
brew install llvm
brew install libomp
```

Check if the library is installed successfully

```bash
brew info llvm
brew info libomp
```

modify [`Makefile`](./Makefile), Replace with the path to the corresponding library on your computer

```makefile
INCLUDE = -I /usr/local/include/eigen3 -I /usr/local/include -I /usr/local/opt/libomp/include
LIBRARY = -L /usr/local/opt/llvm/lib
CXXFLAGS = -std=c++17 -Xclang -fopenmp -lomp -O3 $(LIBRARY) $(INCLUDE)
```

For Linux

modify [`Makefile`](./Makefile), If the system supports openmp, modify it directly

```makefile
INCLUDE = -I ./eigen -I ./CLI11/include
CXXFLAGS = -std=c++17 -fopenmp -O3 $(INCLUDE)
```

Finally, compile the program using `make`

## Usage

The program contains two subcommands, which are used to calculate feature pairs with stable size relationships and feature pairs with correlated relationships.

```bash
./gene_pairs                                                                                                                                     
program:./gene_pairs

Usage: ./gene_pairs [OPTIONS] SUBCOMMAND

Options:
  -h,--help                             Print this help message and exit

Subcommands:
  stable                                Find feature pairs that have a stable relationship in one type of sample and a reversed relationship in another type of sample.
  corr                                  Find feature pairs whose expression relationships (addition, subtraction, multiplication, division) are highly correlated with other features.
```

For identifying feature pairs with stable size relationships

```bash
./gene_pairs stable                                                                                                                              
Find feature pairs that have a stable relationship in one type of sample and a reversed relationship in another type of sample.
Usage: ./gene_pairs stable [OPTIONS]

Options:
  -h,--help                             Print this help message and exit
  -i,--input TEXT:FILE REQUIRED         Feature data file.
  -t,--target TEXT:FILE                 Other features data file.
  -o,--output TEXT                      Output filename.
  --ratio FLOAT [0.9]                   The ratio of feature a > feature b in all samples.
  --revRatio FLOAT [0.7]                The ratio of feature a < feature b in another samples.
  --threads UINT [2]                    Number of threads used.
  --block UINT                          Data blocks processed by each thread, defaults to the size of the column.
```

For identifying relevant feature pairs

```bash
./gene_pairs corr                                                                                                                                
Find feature pairs whose expression relationships (addition, subtraction, multiplication, division) are highly correlated with other features.
Usage: ./gene_pairs corr [OPTIONS]

Options:
  -h,--help                             Print this help message and exit
  -i,--input TEXT:FILE REQUIRED         Feature data file.
  -t,--target TEXT:FILE                 Other features data file.
  -o,--output TEXT                      Output filename.
  -m,--method TEXT [pearson]            Correlation method, pearson/spearman/kendall.
  -a,--operation TEXT [subtract]        Operations(add/subtract/multiply/divide) between features.
  --type TEXT [common]                  Analysis type, common/cross/pairs.
  --cutoff FLOAT [0.3]                  Correlation coefficient threshold.
  --threads UINT [2]                    Number of threads used.
  --block UINT                          Data blocks processed by each thread, defaults to the size of the column.
```
