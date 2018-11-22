#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <map>
#include <vector>
#include <cstdlib>
#include <set>
#include <algorithm>
#include <sstream>
#include "utility.h"

struct Grammar {

    Grammar(std::vector<std::string> rules_list,
            std::set<std::string> nonTerminals,
            std::set<std::string> terminals,
            std::string const& start);

    auto const& data() const;

    std::map<std::string, std::set<std::string>> build_follow_set();
    std::map<std::string, std::set<std::string>> build_first_set();
    std::set<std::string> build_epsilon_set();

private:

    std::pair<std::map<std::string, std::set<std::string>>,
              std::set<std::string>> build_first_epsilon_set();
    size_t rhs_rule_index(std::string);
    bool unite(std::set<std::string>& st1, std::set<std::string> const& st2);

    std::map<std::string, std::size_t> rhs_rule_id;
    std::map<std::size_t, std::string> rhs_rule_str;
    std::map<std::string, std::vector<size_t>> rules;
    std::set<std::string> nonTerminals;
    std::set<std::string> terminals;
    std::string start;

};

#endif // GRAMMAR_H

/* S -> S and S
 * S -> S or S
 * S -> (S)
 * S -> not S
 * S -> Var
 */

/* S -> (S)A
 * S -> (S)
 * S -> not S A
 * S -> not S
 * S -> Var A
 * S -> Var
 * A -> and S A
 * A -> and S
 * A -> or S A
 * A -> or S
*/


/* S  -> (S) S'
 * S  -> not S S'
 * S  -> Var S'
 * A  -> and S S'
 * A  -> or S S'
 * S' -> A
 * S' -> eps
 */
