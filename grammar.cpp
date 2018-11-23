#include "grammar.h"

Grammar::Grammar(std::vector<std::string> rules_list,
                 std::set<std::string> nTerminals,
                 std::set<std::string> trminals,
                 std::string const& strt)
    : start(strt) {

    for (auto const& nt : nTerminals) {
        nonTerminals.insert(index_by_rule(nt));
    }

    for (auto const& t : trminals) {
        terminals.insert(index_by_rule(t));
    }

    for (auto const& rule : rules_list) {
        size_t pos = rule.find("->");
        auto nonTerminal = trim(rule.substr(0, pos));
        auto rhs_string = trim(rule.substr(pos + 2));
        std::vector<std::string> rhs;
        std::istringstream iss(rhs_string);
        std::string current_term;
        while (std::getline(iss, current_term, ' ')) {
            rhs.emplace_back(std::move(current_term));
        }
        rules[index_by_rule(nonTerminal)].push_back(index_by_rule(rhs));
    }
}

size_t Grammar::index_by_rule(std::string const& entity) {
    return index_by_rule(std::vector{entity});
}

size_t Grammar::index_by_rule(std::vector<std::string> const& entity) {
    if (!rule_id.count(entity)) {
        rule_id[entity] = rule_id.size();
        rule_str[rule_id.size() - 1] = entity;
    }
    return rule_id[entity];
}

auto const& Grammar::data() const {
    return rules;
}

bool Grammar::unite(std::set<std::size_t>& st1,
                    std::set<std::size_t> const& st2) {
    auto len = st1.size();
    std::set<std::size_t> x;
    std::set_union(st1.begin(), st1.end(),
                   st2.begin(), st2.end(),
                   std::inserter(x, x.begin()));
    st1 = x;
    return len != st1.size();
}


std::vector<std::string> Grammar::rule_by_index(std::size_t id) {
    return rule_str[id];
}

std::map<std::size_t, std::set<std::size_t>> Grammar::build_first_set_raw() {

    std::map<std::size_t, std::set<std::size_t>> first;


    ///////////////////////////////// processing epsilon starts /////////////////////////////////

    for (auto const& nt_id : nonTerminals) {
        for (auto const& rul_id : rules[nt_id]) {
            std::vector<std::string> rule = rule_by_index(rul_id);
            if (terminals.count(index_by_rule(rule[0]))) {
                first[nt_id].insert(index_by_rule(rule[0]));
            }
            if (rule.size() == 1 && rule[0] == "eps") {
                first[nt_id].insert(index_by_rule("eps"));
            }
        }
    }

    while (true) {
        bool updated {false};

        for (auto const& rule : rules) {
            std::size_t lhs_id = rule.first;
            std::vector<size_t> const& rhs_id_list = rule.second;
            for (std::size_t rhs_id : rhs_id_list) {
                std::vector<std::string> rhs = rule_by_index(rhs_id);

                auto pos = std::find_if(rhs.begin(), rhs.end(), [this](auto const& entity) {
                    return terminals.count(index_by_rule(entity));
                });
                bool all_has_eps {true};
                std::for_each(rhs.begin(), pos, [&all_has_eps, this, &first, lhs_id](auto const& entity) {
                    all_has_eps &= first[index_by_rule(entity)].count(index_by_rule("eps")) != 0;
                    if (all_has_eps) {
                        std::size_t entity_id = index_by_rule(entity);
                        bool has = first[entity_id].count(index_by_rule("eps")) != 0;
                        if (has) first[entity_id].erase(index_by_rule("eps"));
                        unite(first[lhs_id], first[entity_id]);
                        if (has) first[entity_id].insert(index_by_rule("eps"));
                    }
                });
                if (all_has_eps) {
                    if (pos != rhs.end()) {
                        first[lhs_id].insert(index_by_rule(*pos));
                    } else {
                        first[lhs_id].insert(index_by_rule("eps"));
                    }
                }
            }
        }

        if (!updated) {
            break;
        }
    }

    ///////////////////////////////// processing epsilon end /////////////////////////////////


    for (size_t x : terminals) {
        first[x].insert(x);
    }

    while (true) {
        bool updated {false};
        for (auto const& nt_id : nonTerminals) {
            for (auto const& rul_id : rules[nt_id]) {
                std::vector<std::string> Mrule = rule_by_index(rul_id);

                for (auto it_left = Mrule.begin(); it_left != Mrule.end(); ++it_left) {
                    for (auto it_right = it_left; it_right != Mrule.end(); ++it_right) {
                        std::vector<std::string> rule(it_left, std::next(it_right));
                        if (terminals.count(index_by_rule(rule[0]))) {
                            std::set<std::size_t> st; st.insert(index_by_rule(rule[0]));
                            updated |= unite(first[index_by_rule(rule)], st);
                        } else {
                            if (!first[index_by_rule(rule[0])].count(index_by_rule("eps"))) {
                                updated |= unite(first[index_by_rule(rule)], first[index_by_rule(rule[0])]);
                            } else {
                                std::size_t rule0_id = index_by_rule(rule[0]);
                                bool has = first[rule0_id].count(index_by_rule("eps")) != 0;
                                if (has) first[rule0_id].erase(index_by_rule("eps"));
                                updated |= unite(first[index_by_rule(rule)], first[rule0_id]);
                                updated |= unite(first[index_by_rule(rule)], first[index_by_rule(std::vector<std::string>(std::next(rule.begin()), rule.end()))]);
                                if (has) first[rule0_id].insert(index_by_rule("eps"));
                            }
                        }
                    }
                }
            }
        }

        for (std::size_t nt_id : nonTerminals) {
            for (auto const& rul_id : rules[nt_id]) {
                updated |= unite(first[nt_id], first[rul_id]);
            }
        }

        if (!updated) {
            return first;
        }
    }
}

std::map<std::string, std::set<std::string>> Grammar::transform_index_by_rule(std::map<std::size_t, std::set<std::size_t>> mp) {
    std::map<std::string, std::set<std::string>> res;
    for (std::size_t nt_id : nonTerminals) {
        std::string nt = rule_by_index(nt_id).back();
        for (auto const& fst : mp[nt_id]) {
            res[nt].insert(rule_by_index(fst).back());
        }
    }
    return res;
}

std::map<std::size_t, std::set<std::size_t>> Grammar::build_follow_set_raw() {
    std::map<std::size_t, std::set<std::size_t>> follow;
    auto first = build_first_set_raw();
    follow[index_by_rule(start)].insert(index_by_rule("$"));

    for (std::size_t nt_id : nonTerminals) {
        for (auto const& rul_id : rules[nt_id]) {
            std::vector<std::string> rule = rule_by_index(rul_id);
            for (auto it = rule.begin(); std::next(it) != rule.end(); ++it) {
                if (nonTerminals.count(index_by_rule(*it)) &&
                        terminals.count(index_by_rule(*(std::next(it)))) &&
                            *(std::next(it)) != "eps") {
                    follow[index_by_rule(*it)].insert(index_by_rule(*(std::next(it))));
                }
            }
        }
    }

    while (true) {
        bool updated {false};

        for (std::size_t nt_id : nonTerminals) {
            for (auto const& rul_id : rules[nt_id]) {
                std::vector<std::string> rule = rule_by_index(rul_id);
                std::vector<std::string> suffix;
                for (auto it = rule.rbegin(); it != rule.rend(); ++it) {
                    if (nonTerminals.count(index_by_rule(*it))) {
                        bool has = first[index_by_rule(suffix)].count(index_by_rule("eps")) != 0;
                        if (has) first[index_by_rule(suffix)].erase(index_by_rule("eps"));
                        updated |= unite(follow[index_by_rule(*it)], first[index_by_rule(suffix)]);
                        if (has) first[index_by_rule(suffix)].insert(index_by_rule("eps"));
                        if (has || suffix.empty()) {
                            updated |= unite(follow[index_by_rule(*it)], follow[nt_id]);
                        }
                    }
                    suffix.insert(suffix.begin(), *it);
                }
            }
        }

        if (!updated) {
            return follow;
        }
    }
}

std::map<std::string, std::set<std::string>> Grammar::build_follow_set() {
    return transform_index_by_rule(build_follow_set_raw());
}

std::map<std::string, std::set<std::string>> Grammar::build_first_set() {
    return transform_index_by_rule(build_first_set_raw());
}






