#include "repository.h"
#include <iostream>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: minigit <init|add|commit|cat-file> [args]\n";
        return 1;
    }
    std::string cmd = argv[1];
    Repository repo;  # here make a repo which contain all the condition of commands 
    try {
        if (cmd == "init") repo.init();
        else if (cmd == "add" && argc > 2) repo.add(argv[2]);
        else if (cmd == "commit" && argc > 2) repo.commit(argv[2]);
        else if (cmd == "log") repo.log();  #this log command is recently added 
        else if (cmd == "cat-file" && argc > 2) repo.catFile(argv[2]);
        else { std::cout << "Unknown command or missing argument\n"; return 1; }
    } catch (const std::exception &e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
