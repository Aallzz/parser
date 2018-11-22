#include <iostream>
#include "lexer.h"
#include "grammar.h"
#include "parser.h"

using namespace std;

std::ostream& operator << (ostream& out, Token token) {
    out << token_names[static_cast<std::underlying_type<Token>::type>(token)];
    return out;
}



int main() {
    Lexer lexer("  ( a  and b)");
    Token x;
    while ((x = lexer.next_token()) != Token::None) {
        cout << x << " ";
    }

    cout << endl;
    Grammar grammar({
                     "S  -> ( S ) S'",
                     "S  -> not S S'",
                     "S  -> Var S'",
                     "S' -> and S S'",
                     "S' -> or S S'",
                     "S' -> xor S S'",
                     "S' -> eps",
                    },
                    {
                     "S", "S'"
                    },
                    {
                     "(", ")", "not", "Var", "and", "or", "xor", "eps"
                    },
                    "S");

    auto res = grammar.build_follow_set();
    for (auto const& e : res) {
        cout << e.first << ": ";
        for (auto const& r : e.second) {
            cout << r << ", ";
        }
        cout << endl;
    }
    Parser p("a and b");
    auto xx = p.parse();
    auto vxx = xx.data();
    for (auto value : vxx) {
        cout << value << endl;
    }

    return 0;
}


/* S  -> (S) S'
 * S  -> not S S'
 * S  -> Var S'
 * S' -> and S S'
 * S' -> or S S'
 * S' -> xor S S'
 * S' -> eps
 */
