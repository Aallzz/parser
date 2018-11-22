#include "grammar.h"

Grammar::Grammar(std::vector<std::string> rules_list,
                 std::set<std::string> nTerminals,
                 std::set<std::string> trminals,
                 std::string const& strt)
    : nonTerminals(nTerminals),
      terminals(trminals),
      start(strt)
{
    for (auto const& rule : rules_list) {
        size_t pos = rule.find("->");
        auto nonTerminal = trim(rule.substr(0, pos));
        auto rhs = trim(rule.substr(pos + 2));
        rules[nonTerminal].push_back(rhs_rule_index(rhs));
    }
}

size_t Grammar::rhs_rule_index(std::string rhs) {
    if (!rhs_rule_id.count(rhs)) {
        rhs_rule_id[rhs] = rhs_rule_id.size();
        rhs_rule_str[rhs_rule_id.size() - 1] = rhs;
    }
    return rhs_rule_id[rhs];
}

auto const& Grammar::data() const {
    return rules;
}

std::map<std::string, std::set<std::string>> Grammar::build_first_set() {
    return build_first_epsilon_set().first;
}

std::set<std::string> Grammar::build_epsilon_set() {
    return build_first_epsilon_set().second;
}

bool Grammar::unite(std::set<std::string>& st1, std::set<std::string> const& st2) {
    auto len = st1.size();
    std::set<std::string> x;
    std::set_union(st1.begin(), st1.end(),
                   st2.begin(), st2.end(),
                   std::inserter(x, x.begin()));
    st1 = x;
    return len != st1.size();
}

std::pair<std::map<std::string, std::set<std::string>>,
          std::set<std::string>> Grammar::build_first_epsilon_set() {
    std::map<std::string, std::set<std::string>> first;
    for (auto const& x : terminals) {
        first[x].insert(x);
    }
    std::set<std::string> eps;

    while (true) {
        bool updated {false};
        for (auto const& rule : rules) {
            std::string const& lhs = rule.first;
            std::vector<size_t> const& rhs = rule.second;
            for (auto const& expression_id : rhs) {
                std::istringstream iss(rhs_rule_str[expression_id]);
                std::string cur;
                while (std::getline(iss, cur, ' ')) {
                    cur = trim(cur);
                    updated |= unite(first[lhs], first[cur]);
                    if (eps.count(cur)) {
                        updated |= unite(eps, std::initializer_list<std::string>{lhs});
                    } else {
                        break;
                    }
                }
            }
        }
        if (!updated) {
            return {first, eps};
        }
    }
}

std::map<std::string, std::set<std::string>> Grammar::build_follow_set() {
    std::map<std::string, std::set<std::string>> follow;
    auto first = build_first_epsilon_set().first;
    auto eps = build_first_epsilon_set().second;

    while (true) {
        bool updated {false};
        for (auto const& rule : rules) {
            std::string const& lhs = rule.first;
            std::vector<size_t> const& rhs = rule.second;
            for (auto const& expression_id : rhs) {
                std::string expression = rhs_rule_str[expression_id];
                std::reverse(expression.begin(), expression.end());
                std::istringstream iss(expression);
                std::string cur;
                std::set<std::string> aux = follow[lhs];
                while (std::getline(iss, cur, ' ')) {
                    std::reverse(cur.begin(), cur.end());
                    if (follow.count(cur)) {
                        updated |= unite(follow[cur], aux);
                    }
                    if (eps.count(cur)) {
                        unite(aux, first[cur]);
                    } else {
                        aux = first[cur];
                    }
                }

            }
        }
        if (!updated) {
            return follow;
        }
    }
}






