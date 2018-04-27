/**
 * Copyright (c) 2018 by Marek Wydmuch
 * All rights reserved.
 */

#include "args.h"
#include "types.h"
#include "base.h"
#include "pltree.h"
#include "utils.h"

void test(Args &args) {
    SRMatrix<Label> labels;
    SRMatrix<Feature> features;
    args.load(args.model + "/args.bin");
    args.printArgs();
    args.readData(labels, features);

    PLTree tree;
    tree.load(args.model + "/tree.bin");

    std::cerr << "Loading base classifiers ...\n";
    std::vector<Base*> bases;
    std::ifstream in(args.model + "/weights.bin");
    for(int i = 0; i < tree.nodes(); ++i) {
        printProgress(i, tree.nodes());
        bases.emplace_back(new Base());
        bases.back()->load(in, args);
    }
    in.close();

    tree.test(labels, features, bases, args);

    for(auto base : bases) delete base;
}

void train(Args &args) {
    args.printArgs();

    SRMatrix<Label> labels;
    SRMatrix<Feature> features;
    args.readData(labels, features);

    PLTree tree;
    tree.train(labels, features, args);
}

void shrink(Args &args) {
    args.printArgs();

    PLTree tree;
    tree.load(args.input + "/tree.bin");

    std::ifstream in(args.input + "/weights.bin");
    std::ofstream out(args.model + "/weights.bin");
    for (int i = 0; i < tree.nodes(); ++i){
        Base base;
        base.load(in, args);
        base.threshold(args.threshold);
        base.save(out, args);
    }
    in.close();
    out.close();
}

int main(int argc, char** argv) {
    std::vector<std::string> arg(argv, argv + argc);
    Args args = Args();
    args.parseArgs(arg);

    if(args.command == "train")
        train(args);
    else if(args.command == "test")
        test(args);
    else if(args.command == "shrink")
        shrink(args);
    else
        args.printHelp();

    return 0;
}
