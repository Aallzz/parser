#include "parser.h"

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
                    throw std::exception();
                }
                lexer.next_token();
                auto tree_2 = std::invoke(parse_["S'"]);
                return Tree("S", Tree("("), std::move(tree_1), Tree(")"), std::move(tree_2));
            }
            case Token::Not: {
                return parse_help("not", "S");
            }
            case Token::Var: {
                lexer.next_token();
                Tree tree_2 = std::invoke(parse_["S'"]);
                return Tree("S", Tree("var"), std::move(tree_2));
            }
            default:
                throw std::exception();
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
            default:
                return Tree("S'", Tree("eps"));
        }
    };
}

Tree Parser::parse_help(std::string operation, std::string ntroot) {
    lexer.next_token();
    Tree tree_1 = std::invoke(parse_["S"]);
    lexer.next_token();
    Tree tree_2 = std::invoke(parse_["S'"]);
    return Tree(ntroot, Tree(operation), std::move(tree_1), std::move(tree_2));
}

Tree Parser::parse() {
    lexer.next_token();
    return std::invoke(parse_["S"]);
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
