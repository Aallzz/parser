#include "lexer.h"


Lexer::Lexer(std::string const& str) : iss(str) {
    tokens_trie.add_string("and", Token::And);
    tokens_trie.add_string("or", Token::Or);
    tokens_trie.add_string("xor", Token::Xor);
    tokens_trie.add_string("not", Token::Not);
    tokens_trie.add_string("(", Token::LB);
    tokens_trie.add_string(")", Token::RB);
    for (char i = 'a'; i <= 'z'; ++i) {
        tokens_trie.add_string(std::string(1, i), Token::Var);
    }
}

Token Lexer::next_token() {
    char c;
    while (isspace(iss.peek())) {
        iss.get();
    }
    tokens_trie.reset_state();
    while (!iss.eof() && (c = static_cast<char>(iss.get()))) {
        if (tokens_trie.move_state(c)) {
            if (tokens_trie.has_key()
                    && !tokens_trie.has_next(static_cast<char>(iss.peek()))) {
                return _current_token = tokens_trie.get_key();
            }
        } else {
            throw std::exception();
        }
    }
    return _current_token = Token::None;
}

Token Lexer::current_token() const {
    return _current_token;
}

Lexer::trie::trie() {
    t.emplace_back();
}

void Lexer::trie::add_string(std::string const& s, Token key) {
    unsigned long cur = 0;
    for (char c : s) {
        if (!t[cur].to.count(c)) {
            t[cur].to[c] = t.size();
            t.emplace_back();
        }
        cur = t[cur].to[c];
    }
    t[cur].key = key;
}

bool Lexer::trie::move_state(char c) {
    if (t[state].to.count(c)) {
        state = t[state].to[c];
        return true;
    } else {
        state = 0;
        return false;
    }
}

void Lexer::trie::reset_state() {
    state = 0;
}

bool Lexer::trie::has_key() const {
    return t[state].key != Token::None;
}

bool Lexer::trie::has_next(char c) const {
    return t[state].to.count(c);
}

Token Lexer::trie::get_key() const {
    return t[state].key;
}
