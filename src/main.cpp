#include <map>
#include <iostream>

#include <CLI/CLI.hpp>
#include <CLI/Validators.hpp>

#include "utils.h"
#include "corrpairs.h"
#include "stablepairs.h"


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
    CLI::App *stable_pairs = app.add_subcommand("stable", "Find feature pairs that have a stable relationship in one type of sample and a reversed relationship in another type of sample.");
    stable_pairs->add_option("-i,--input", stableopt->expression, "Feature data file.")->check(CLI::ExistingFile)->required(true);
    stable_pairs->add_option("-t,--target", stableopt->target, "Other features data file.")->check(CLI::ExistingFile);
    stable_pairs->add_option("-o,--output", stableopt->output, "Output filename.");
    stable_pairs->add_option("--ratio", stableopt->ratio, "The ratio of feature a > feature b in all samples.")->default_val(0.9);
    stable_pairs->add_option("--revRatio", stableopt->revRatio, "The ratio of feature a < feature b in another samples.")->default_val(0.7);
    stable_pairs->add_option("--threads", stableopt->threads, "Number of threads used.")->default_val(2);
    stable_pairs->add_option("--block", stableopt->block, "Data blocks processed by each thread, defaults to the size of the column.");
    // 当出现的参数子命令解析不了时,返回上一级尝试解析
    stable_pairs->fallthrough();
    // correlation
    CorrOptions *corropt = new CorrOptions();
    CLI::App *corr_pairs = app.add_subcommand("corr", "Find feature pairs whose expression relationships (addition, subtraction, multiplication, division) are highly correlated with other features.");
    corr_pairs->add_option("-i,--input", corropt->expression, "Feature data file.")->check(CLI::ExistingFile)->required(true);
    corr_pairs->add_option("-t,--target", corropt->target, "Other features data file.")->check(CLI::ExistingFile);
    corr_pairs->add_option("-o,--output", corropt->output, "Output filename.");
    corr_pairs->add_option("-m,--method", corropt->method, "Correlation method, pearson/spearman/kendall.")->default_val("pearson");
    corr_pairs->add_option("-a,--operation", corropt->operation, "Operations(add/subtract/multiply/divide) between features.")->default_val("subtract");
    corr_pairs->add_option("--type", corropt->analysis, "Analysis type, common/cross/pairs.")->default_val("common");
    corr_pairs->add_option("--cutoff", corropt->threshold, "Correlation coefficient threshold.")->default_val(0.3);
    corr_pairs->add_option("--threads", corropt->threads, "Number of threads used.")->default_val(2);
    corr_pairs->add_option("--block", corropt->block, "Data blocks processed by each thread, defaults to the size of the column.");
    corr_pairs->fallthrough();

    CLI11_PARSE(app, argc, argv);
    // stable
    if (stable_pairs->parsed()) {
        std::cout << "[Stable Pairs] - Begin at: " << Utils::currentTime() << std::endl;
        StablePairs* sp = new StablePairs(stableopt);
        if (sp->getPairs()) {
            sp->writePairs();
        } else {
            std::cout << "Stable Pairs] - Stable gene pair calculation error!\n";
        }
        std::cout << "[Stable Pairs] - End at: " << Utils::currentTime() << std::endl;
        delete sp;
    }
    // corr
    if (corr_pairs->parsed()) {
        std::cout << "[Correlation Pairs] - Begin at: " << Utils::currentTime() << std::endl;
        CorrPairs* cp = new CorrPairs(corropt);
        if (cp->getPairs()) {
            cp->writePairs();
        } else {
            std::cout << "[[Correlation Pairs] - Correlation gene pair calculation error!\n";
        }
        std::cout << "[Correlation Pairs] - End at: " << Utils::currentTime() << std::endl;
        delete cp;
    }
}