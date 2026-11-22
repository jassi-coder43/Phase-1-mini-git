#include "repository.h"
#include "utils.h"
#include <cstdlib>
#include <ctime>
#include <dirent.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <cstdlib>
static void makeDir(const std::string &p) { mkdir(p.c_str(), 0755); }

static bool isDir(const std::string &p) {
    struct stat st;
    return stat(p.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

// Saves object as .minigit/objects/ab/cdef... (zlib compressed)
std::string Repository::writeObject(const GitObject &obj) {
    std::string h = obj.hash();
    std::string dir = root + "/objects/" + h.substr(0, 2);
    std::string path = dir + "/" + h.substr(2);
    if (!fileExists(path)) { // same content = same hash = stored only once
        makeDir(dir);
        writeFile(path, zCompress(obj.serialize()));
    }
    return h;
}

std::string Repository::readObject(const std::string &hash) {
    std::string path = root + "/objects/" + hash.substr(0, 2) + "/" + hash.substr(2);
    if (hash.size() != 40 || !fileExists(path))
        throw std::runtime_error("object not found: " + hash);
    return zDecompress(readFile(path));
}

void Repository::loadIndex() {
    index.clear();
    std::string path = root + "/index";
    if (!fileExists(path)) return;
    std::istringstream in(readFile(path));
    std::string hash, file;
    while (in >> hash && std::getline(in >> std::ws, file)) index[file] = hash;
}

void Repository::saveIndex() {
    std::string out;
    for (const auto &e : index) out += e.second + " " + e.first + "\n";
    writeFile(root + "/index", out);
}

void Repository::init() {
    if (isDir(root)) { std::cout << "Repository already exists\n"; return; }
    makeDir(root);
    makeDir(root + "/objects");
    makeDir(root + "/refs");
    makeDir(root + "/refs/heads");
    writeFile(root + "/HEAD", "ref: refs/heads/master\n");
    writeFile(root + "/index", "");
    std::cout << "Initialized empty MiniGit repository in " << root << "/\n";
}

void Repository::addPath(const std::string &path) {
    if (isDir(path)) {
        DIR *d = opendir(path.c_str());
        if (!d) return;
        while (dirent *e = readdir(d)) {
            std::string name = e->d_name;
            if (name == "." || name == ".." || name == ".minigit" || name == ".git") continue;
            addPath(path == "." ? name : path + "/" + name);
        }
        closedir(d);
    } else {
        Blob b(readFile(path));
        index[path] = writeObject(b);
        std::cout << "Added " << path << "\n";
    }
}

void Repository::add(const std::string &path) {
    if (!isDir(root)) throw std::runtime_error("not a minigit repository (run init first)");
    if (!fileExists(path)) throw std::runtime_error("path not found: " + path);
    loadIndex();
    addPath(path);
    saveIndex();
}

// Turns flat paths (src/a.c, b.c) into nested Tree objects. Returns root tree hash.
std::string Repository::buildTree(const std::map<std::string, std::string> &files) {
    Tree tree;
    std::map<std::string, std::map<std::string, std::string>> subdirs;
    for (const auto &f : files) {
        size_t slash = f.first.find('/');
        if (slash == std::string::npos)
            tree.addEntry(f.first, "blob", f.second);
        else
            subdirs[f.first.substr(0, slash)][f.first.substr(slash + 1)] = f.second;
    }
    for (const auto &s : subdirs)
        tree.addEntry(s.first, "tree", buildTree(s.second));
    return writeObject(tree);
}

std::string Repository::currentBranchFile() {
    std::string head = readFile(root + "/HEAD"); // "ref: refs/heads/master\n"
    std::string ref = head.substr(5);
    while (!ref.empty() && (ref.back() == '\n' || ref.back() == ' ')) ref.pop_back();
    return root + "/" + ref;
}

std::string Repository::headCommit() {
    std::string f = currentBranchFile();
    if (!fileExists(f)) return "";
    std::string h = readFile(f);
    while (!h.empty() && (h.back() == '\n' || h.back() == ' ')) h.pop_back();
    return h;
}

void Repository::commit(const std::string &message) {
    if (!isDir(root)) throw std::runtime_error("not a minigit repository (run init first)");
    loadIndex();
    if (index.empty()) throw std::runtime_error("nothing to commit (use add first)");

    std::string tree = buildTree(index);
    std::string parent = headCommit();
    Commit c(tree, parent, "MiniGit User", message, (long)time(nullptr));
    std::string h = writeObject(c);
    writeFile(currentBranchFile(), h + "\n");
    std::cout << "[" << h.substr(0, 7) << "] " << message << "\n";
}

void Repository::catFile(const std::string &hash) {
    std::string data = readObject(hash);
    size_t nul = data.find('\0');
    std::cout << data.substr(nul + 1);
}

// Walks from HEAD back through parent commits and prints each one
void Repository::log() {
    if (!isDir(root)) throw std::runtime_error("not a minigit repository (run init first)");
    std::string h = headCommit();
    if (h.empty()) { std::cout << "No commits yet\n"; return; }

    while (!h.empty()) {
        std::string data = readObject(h);
        std::string body = data.substr(data.find('\0') + 1);

        std::string parent, author, message;
        long ts = 0;
        std::istringstream in(body);
        std::string line;
        while (std::getline(in, line) && !line.empty()) {
            if (line.compare(0, 7, "parent ") == 0) parent = line.substr(7);
            else if (line.compare(0, 7, "author ") == 0) {
                size_t sp = line.rfind(' ');
                author = line.substr(7, sp - 7);
                ts = atol(line.substr(sp + 1).c_str());
            }
        }
        std::getline(in, message); // first line of the message

        time_t t = (time_t)ts;
        std::string when = ctime(&t);
        when.pop_back(); // remove trailing newline

        std::cout << "commit " << h << "\n";
        std::cout << "Author: " << author << "\n";
        std::cout << "Date:   " << when << "\n\n";
        std::cout << "    " << message << "\n\n";
        h = parent;
    }
}

