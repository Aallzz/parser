#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <exception>
#include <cctype>
#include <string_view>

enum class Token {
    None = 0,
    And,
    Or,
    Not,
    Xor,
    LB,
    RB,
    Var,
    Eps,
};

static constexpr std::string_view token_names[] {"END", "And", "Or", "Not", "Xor", "LB", "RB", "Var"};

struct Lexer {

    Lexer(std::string const& str);

    Token next_token();
    Token current_token() const;

    Token token_by_string(std::string const&);
private:

    template<typename T>
    struct node {

        T key {};
        std::map<int, unsigned long> to;
    };

    struct trie {

        trie();
        void add_string(std::string const& s, Token key);

        void reset_state();
        bool move_state(char c);

        bool has_next(char c) const;
        bool has_key() const;
        Token get_key() const;

    private:

        std::vector<node<Token>> t;
        unsigned long state;
    } tokens_trie;

    std::istringstream iss;
    Token _current_token;
};

#endif // LEXER_H
