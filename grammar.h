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
#include <iostream>
#include <cassert>
struct Grammar {

    Grammar(std::vector<std::string> rules_list,
            std::set<std::string> nonTerminals,
            std::set<std::string> terminals,
            std::string const& start);

    auto const& data() const;

    std::map<std::string, std::set<std::string>> build_follow_set();
    std::map<std::string, std::set<std::string>> build_first_set();
    std::map<std::string, std::map<std::string, std::vector<std::string>>> build_ll1_table();

    bool is_terminal(std::string const&);
    bool is_nonTerminal(std::string const&);

    std::string get_start();

private:

    std::set<std::string> get_first(std::vector<std::string> const& s);
    std::set<std::size_t> get_first_raw(std::vector<std::string> const& s);
    std::map<std::size_t, std::set<std::size_t>>  build_follow_set_raw();
    std::map<std::size_t, std::set<std::size_t>>  build_first_set_raw();

    std::set<std::string> transform_index_by_rule(std::set<std::size_t> st);
    std::map<std::string, std::set<std::string>> transform_index_by_rule(std::map<std::size_t, std::set<std::size_t>>);
    size_t index_by_rule(std::vector<std::string> const&);
    size_t index_by_rule(std::string const& entity);

    std::vector<std::string> rule_by_index(std::size_t id);

    bool unite(std::set<std::size_t>& st1,
               std::set<std::size_t> const& st2);

    std::map<std::vector<std::string>, std::size_t> rule_id;
    std::map<std::size_t, std::vector<std::string>> rule_str;
    std::map<std::size_t, std::vector<size_t>> rules;
    std::set<std::size_t> nonTerminals;
    std::set<std::size_t> terminals;
    std::string start;

    std::map<std::size_t, std::set<std::size_t>> first_set;
    std::map<std::size_t, std::set<std::size_t>> follow_set;

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
