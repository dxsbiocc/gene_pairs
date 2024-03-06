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

first, clone eigen and CLI11 library from github

```bash
git clone https://gitlab.com/libeigen/eigen.git
git clone https://github.com/CLIUtils/CLI11.git
```

second, modify [`Makefile`](./Makefile) line 5

```bash
- INCLUDE = -I /usr/local/include/eigen3 -I /usr/local/include
+ INCLUDE = -I ./eigen -I ./CLI11/include
```

## Usage
