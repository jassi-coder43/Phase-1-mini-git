#ifndef OBJECTS_H
#define OBJECTS_H
#include <map>
#include <string>

// Base class: every Git object has a type, content and a SHA-1 id.
class GitObject {
public:
    virtual ~GitObject() {}
    virtual std::string type() const = 0;
    virtual std::string content() const = 0;

    // Same format as real Git: "<type> <size>\0<content>"
    std::string serialize() const;
    std::string hash() const;
};

// Blob = content of one file
#blob

class Blob : public GitObject {
    std::string data;
public:
    explicit Blob(const std::string &d) : data(d) {}
    std::string type() const override { return "blob"; }
    std::string content() const override { return data; }
};

// Tree = one folder. Maps name -> (type, hash)
class Tree : public GitObject {
public:
    struct Entry { std::string type; std::string hash; };
    void addEntry(const std::string &name, const std::string &type, const std::string &hash);
    std::string type() const override { return "tree"; }
    std::string content() const override;
private:
    std::map<std::string, Entry> entries; // map keeps names sorted
};

// Commit = snapshot (tree) + parent + message
class Commit : public GitObject {
    std::string treeHash, parentHash, author, message;
    long timestamp;
public:
    Commit(const std::string &tree, const std::string &parent,
           const std::string &author, const std::string &msg, long ts)
        : treeHash(tree), parentHash(parent), author(author), message(msg), timestamp(ts) {}
    std::string type() const override { return "commit"; }
    std::string content() const override;
};

#endif
