#include "parser.h"

parser_exception::parser_exception(const char* message) : msg_(message) {}

parser_exception::parser_exception(const std::string& message) : msg_(message) {}

parser_exception::~parser_exception() {}

const char* parser_exception::what() const noexcept{
   return msg_.c_str();
}

Tree::Tree(Tree&& other) {
    root = (std::move(other.root));
}

Tree::Tree(std::string value) : root (std::make_unique<node>(std::move(value))) {}

Tree::node::node(std::string const& vl) : value(vl) {}

void Tree::node::push_node(std::unique_ptr<node>&& pn) {
    children.emplace_back(std::move(pn));
}

void Tree::node::emplace_node(std::string vl, std::vector<std::unique_ptr<node>>&& ch) {
    children.emplace_back(std::make_unique<node>(vl));
    std::move(ch.begin(), ch.end(), children.begin());
}

void Tree::node::add_child(std::unique_ptr<node>&& pn) {
    children.emplace_back(std::move(pn));
}

std::vector<std::string> Tree::data() {
    std::vector<std::string> res;
    dfs(root, res);
    return res;
}

void Tree::dfs(std::unique_ptr<node> const& cur, std::vector<std::string>& res) {
    static std::string w = "";
    if (!cur) return ;
    res.push_back(w + cur->value);
    for (auto const& to : cur->children) {
        w += "  ";
        dfs(to, res);
        w.pop_back();
        w.pop_back();
    }
}

Parser::Parser(std::string str) : lexer(str) {

    parse_["S"] = [this]() mutable {
        switch (lexer.current_token()) {
            case Token::LB: {
                lexer.next_token();
                auto tree_1 = std::invoke(parse_["S"]);
                if (lexer.current_token() != Token::RB) {
                    throw parser_exception("Expected \')\', but found:" + lexer.current_string_token());
                }
                lexer.next_token();
                auto tree_2 = std::invoke(parse_["S'"]);
                return Tree("S", Tree("("), std::move(tree_1), Tree(")"), std::move(tree_2));
            }
            case Token::Not: {
                return parse_help("not", "S");
            }
            case Token::Var: {
                std::string cur = lexer.current_string_token();
                lexer.next_token();
                Tree tree_2 = std::invoke(parse_["S'"]);
                return Tree("S", Tree(cur), std::move(tree_2));
            }
            case Token::RB:
            case Token::And:
            case Token::Or:
            case Token::Xor:
            case Token::End:
                throw parser_exception("Unexpected symbol: " + lexer.current_string_token());
            default:
                throw parser_exception(lexer.current_string_token() + " is not a token");
        }
    };

    parse_["S'"] = [this]() mutable {
        switch (lexer.current_token()) {
            case Token::And:
                return parse_help("and", "S'");
            case Token::Or:
                return parse_help("or", "S'");
            case Token::Xor:
                return parse_help("xor", "S'");
            case Token::RB:
            case Token::End:
                return Tree("S'", Tree("eps"));
            case Token::LB:
            case Token::Var:
            case Token::Not:
                throw parser_exception("Unexpected symbol: " + lexer.current_string_token());
            default:
                throw parser_exception(lexer.current_string_token() + " is not a token");
        }
    };
}

Tree Parser::parse_help(std::string operation, std::string ntroot) {
    lexer.next_token();
    Tree tree_1 = std::invoke(parse_["S"]);
    Tree tree_2 = std::invoke(parse_["S'"]);
    return Tree(ntroot, Tree(operation), std::move(tree_1), std::move(tree_2));
}

Tree Parser::parse() {
    lexer.next_token();
    return std::invoke(parse_["S"]);
}

Tree Parser::parse(Grammar& grammar) {
    /// reversed rule for action
    std::vector<std::string> action;
    action.push_back(grammar.get_start());
    Tree tree (grammar.get_start());
    lexer.next_token();
    auto ll1_table = grammar.build_ll1_table();

    while (!action.empty()) {
        std::string cur = action.back();
        action.pop_back();
        if (grammar.is_nonTerminal(cur)) {
            if (!ll1_table.count(cur)) {
                throw parser_exception("Cannot find ");
            }
            if (!ll1_table[cur].count(lexer.current_string_token())) {
                throw parser_exception("No rule found ");
            }
            std::vector<std::string> rl = ll1_table[cur][lexer.current_string_token()];
            std::reverse(rl.begin(), rl.end());
            for (auto const& e : rl) {
                action.push_back(e);
            }
        } else if (grammar.is_terminal(action.back())) {
            if (lexer.token_by_string(action.back()) == lexer.current_token()) {
                lexer.next_token();
            } else {
                throw std::exception();
            }
        } else {
            throw std::exception();
        }
    }
    return tree;
}

Tree Parser::parseR(Grammar& grammar) {
    grammar.build_first_set();
    for (auto const& nt : grammar.get_nonTerminals()) {
        parse_[nt] = [this, &grammar, nt]() mutable {
            bool has_eps {false};
            Tree res(nt);
            for (auto const& tl : grammar.get_rules_for(nt)) {
                has_eps |= grammar.get_first(tl).count("eps");
                if (grammar.get_first(tl).count(lexer.current_string_token())) {
                    for (auto const& v : tl) {
                        if (grammar.is_nonTerminal(v)) {
                            res.add_children(parse_[v]());
                        } else if (grammar.is_terminal(v)) {
                            // check?
                            if (lexer.current_string_token() != v) {
                                throw parser_exception("Expected \')\', but found:" + lexer.current_string_token());
                            }
                            lexer.next_token();
                            res.add_children(Tree(v));
                        } else {
                            throw parser_exception(lexer.current_string_token() + " is not a token");
                        }
                    }
                    return res;
                }
            }
            if (has_eps) {
                if (grammar.get_follow(nt).count(lexer.current_string_token())) {
                    res.add_children(Tree("eps"));
                } else {
                    throw parser_exception("nexpected symbol: " + lexer.current_string_token());
                }
                return  res;
            } else {
                throw parser_exception("Unexpected symbol: " + lexer.current_string_token());
            }
        };
    }
    lexer.next_token();
    return parse_[grammar.get_start()]();
}

//http://fperucic.github.io/treant-js/
/* S  -> (S) S'
 * S  -> not S S'
 * S  -> Var S'
 * S' -> and S S'
 * S' -> or S S'
 * S' -> xor S S'
 * S' -> eps
 */
