#ifndef SHA1_H
#define SHA1_H
#include <string>

// Returns the 40 character hex SHA-1 of the given data
std::string sha1(const std::string &data);

#endif