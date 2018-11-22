#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include "lexer.h"
#include <iostream>
#include <iomanip>

struct Tree {

    template<typename... Trees,
             typename = std::enable_if_t<(!std::is_lvalue_reference_v<Trees> && ...)>>
    Tree(std::string vl, Trees&&... cdren)
        : root (std::make_unique<node>(vl)) {
        (root->push_node(std::move(cdren.root)),...);
    }

    Tree(Tree&& other) {
        root = (std::move(other.root));
    }

    Tree(std::string value);

    std::vector<std::string> data();

private:

    struct node;

    void dfs(std::unique_ptr<node> const& cur, std::vector<std::string>& res);

    struct node {

        node(std::string const& value);

        void push_node(std::unique_ptr<node>&& pn);
        void emplace_node(std::string value, std::vector<std::unique_ptr<node>>&& children);

        void add_child(std::unique_ptr<node>&& pn);

        std::string value;
        std::vector<std::unique_ptr<node>> children;

    private:
    };


    std::unique_ptr<node> root {};


    std::vector<std::string> ws;
};

struct Parser {

    Tree parse();

    Parser(std::string str);

private:

    Tree parse_help(std::string operation, std::string ntroot);

    std::map<std::string, std::function<Tree(void)>> parse_;
    Lexer lexer;

};

#endif // PARSER_H
