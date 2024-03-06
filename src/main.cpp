#include <map>
#include <iostream>

#include <CLI/CLI.hpp>
#include <CLI/Validators.hpp>

#include "utils.h"
#include "corrpairs.h"
#include "stablepairs.h"


std::map<std::string, Utils::Method> METHODS_MAP {
    {"pearson",   Utils::Method::PEARSON},
    {"spearman", Utils::Method::SPEARMAN},
    {"kendall",  Utils::Method::KENDALL}
};
std::map<std::string, Utils::Operation> OPERATIONS_MAP{
    {"add", Utils::Operation::ADD}, 
    {"subtract", Utils::Operation::SUBTRACT}, 
    {"multiply", Utils::Operation::MULTIPLY},
    {"divide", Utils::Operation::DIVIDE}
};

int main(int argc, char* argv[]) {
    if(argc <= 2){
        string helpCMD;
        if (argc == 1)
            helpCMD = string(argv[0]) + " -h";
        else
            helpCMD = string(argv[0]) + " " + string(argv[1]) + " -h";
        system(helpCMD.c_str());
        return 0;
    }
    CLI::App app("program:" + std::string(argv[0]) + "\n");
    app.get_formatter()->column_width(40);
    app.require_subcommand(1);   // 表示运行命令需要且仅需要一个子命令

    // stable
    StableOptions *stableopt = new StableOptions();
    CLI::App *stable_pairs = app.add_subcommand("stable", "stable gene pairs");
    stable_pairs->add_option("-i,--input", stableopt->expression, "gene expression filename")->check(CLI::ExistingFile)->required(true);
    stable_pairs->add_option("-t,--target", stableopt->target, "gene expression filename for calculate reverse gene pairs")->check(CLI::ExistingFile);
    stable_pairs->add_option("-o,--output", stableopt->output, "output filename");
    stable_pairs->add_option("-r,--ratio", stableopt->ratio, "The ratio of gene a > gene b in all samples.");
    stable_pairs->add_option("--revRatio", stableopt->revRatio, "The ratio of gene a < gene b in another samples.");
    // correlation
    CorrOptions *corropt = new CorrOptions();
    CLI::App *corr_pairs = app.add_subcommand("corr", "correlation gene pairs");
    corr_pairs->add_option("-i,--input", corropt->expression, "gene expression filename")->check(CLI::ExistingFile)->required(true);
    corr_pairs->add_option("-t,--target", corropt->target, "relevance target file")->check(CLI::ExistingFile)->required(true);
    corr_pairs->add_option("-o,--output", corropt->output, "output filename");
    corr_pairs->add_option("-m,--method", corropt->method, "correlation method")->transform(
        CLI::CheckedTransformer(METHODS_MAP, CLI::ignore_case));
    corr_pairs->add_option("-a,--operation", corropt->operation, "operations between gene expressions")->transform(
        CLI::CheckedTransformer(OPERATIONS_MAP, CLI::ignore_case));
    corr_pairs->add_option("-c,cutoff", corropt->threshold, "correlation coefficient threshold");
    stable_pairs->fallthrough(); // 当出现的参数子命令解析不了时,返回上一级尝试解析
    corr_pairs->fallthrough();

    CLI11_PARSE(app, argc, argv);
    // stable
    if (stable_pairs->parsed()) {
        StablePairs* sp = new StablePairs(stableopt);
        if (sp->getPairs()) {
            sp->writePairs();
        } else {
            std::cout << "Stable gene pair calculation error!\n";
        }
        delete stableopt;
    }
    // corr
    if (corr_pairs->parsed()) {
        CorrPairs* cp = new CorrPairs(corropt);
        if (cp->getPairs()) {
            cp->writePairs();
        } else {
            std::cout << "Correlation gene pair calculation error!\n";
        }
        delete corropt;
    }

    return 0;
}