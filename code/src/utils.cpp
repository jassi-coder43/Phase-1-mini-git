#include "utils.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <stdexcept>
#include <vector>
#include <zlib.h>

std::string readFile(const std::string &path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("cannot open file: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

void writeFile(const std::string &path, const std::string &data) {
    std::ofstream f(path, std::ios::binary);
    if (!f) throw std::runtime_error("cannot write file: " + path);
    f.write(data.data(), data.size());
}

bool fileExists(const std::string &path) {
    struct stat st;
    return stat(path.c_str(), &st) == 0;
}

std::string zCompress(const std::string &data) {
    uLongf size = compressBound(data.size());
    std::vector<Bytef> buf(size);
    if (compress(buf.data(), &size, (const Bytef *)data.data(), data.size()) != Z_OK)
        throw std::runtime_error("zlib compress failed");
    return std::string((char *)buf.data(), size);
}

std::string zDecompress(const std::string &data) {
    z_stream zs{};
    if (inflateInit(&zs) != Z_OK) throw std::runtime_error("zlib init failed");
    zs.next_in = (Bytef *)data.data();
    zs.avail_in = data.size();
    std::string out;
    char buf[4096];
    int ret;
    do {
        zs.next_out = (Bytef *)buf;
        zs.avail_out = sizeof buf;
        ret = inflate(&zs, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END) {
            inflateEnd(&zs);
            throw std::runtime_error("zlib decompress failed");
        }
        out.append(buf, sizeof buf - zs.avail_out);
    } while (ret != Z_STREAM_END);
    inflateEnd(&zs);
    return out;
}