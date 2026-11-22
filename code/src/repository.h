#ifndef REPOSITORY_H
#define REPOSITORY_H
#include "objects.h"
#include <map>
#include <string>

class Repository {
    std::string root = ".minigit";
    std::map<std::string, std::string> index; // path -> blob hash

    std::string writeObject(const GitObject &obj);   // returns hash
    std::string readObject(const std::string &hash); // returns full serialized data
    void loadIndex();
    void saveIndex();
    void addPath(const std::string &path);
    std::string buildTree(const std::map<std::string, std::string> &files);
    std::string currentBranchFile();
    std::string headCommit();

public:
    void init();
    void add(const std::string &path);
    void commit(const std::string &message);
    void catFile(const std::string &hash);
    void log();
};

#endif