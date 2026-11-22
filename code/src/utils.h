#ifndef UTILS_H
#define UTILS_H
#include <string>

std::string readFile(const std::string &path);
void writeFile(const std::string &path, const std::string &data);
bool fileExists(const std::string &path);
std::string zCompress(const std::string &data);
std::string zDecompress(const std::string &data);

#endif