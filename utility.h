#ifndef UTILITY_H
#define UTILITY_H

#include "lexer.h"
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <ostream>

std::string build_json_tree(std::vector<std::string> const& tree);

std::ostream& operator << (std::ostream& out, Token token);

std::string trim(std::string str);

#endif // UTILITY_H
