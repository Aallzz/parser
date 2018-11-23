#ifndef UTILITY_H
#define UTILITY_H

#include "lexer.h"
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <ostream>
#include <unistd.h>

std::string build_json_tree(std::vector<std::string> const& tree);

std::ostream& operator << (std::ostream& out, Token token);

std::string trim(std::string str);

std::pair<std::string, std::string> get_browser_arguments();

#endif // UTILITY_H
