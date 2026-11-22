#include "objects.h"
#include "sha1.h"

std::string GitObject::serialize() const {
    std::string c = content();
    return type() + " " + std::to_string(c.size()) + '\0' + c;
}

std::string GitObject::hash() const { return sha1(serialize()); }

void Tree::addEntry(const std::string &name, const std::string &type, const std::string &hash) {
    entries[name] = {type, hash};
}

std::string Tree::content() const {
    std::string out;
    for (const auto &e : entries)
        out += e.second.type + " " + e.second.hash + " " + e.first + "\n";
    return out;
}

std::string Commit::content() const {
    std::string out = "tree " + treeHash + "\n";
    if (!parentHash.empty()) out += "parent " + parentHash + "\n";
    out += "author " + author + " " + std::to_string(timestamp) + "\n\n";
    out += message + "\n";
    return out;
}