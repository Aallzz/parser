#include <iostream>
#include "lexer.h"
#include "grammar.h"
#include "parser.h"
#include "utility.h"
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

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

//        Grammar grammar({
//                            "E -> T E'",
//                            "E' -> + T E'",
//                            "E' -> eps",
//                            "T -> F T'",
//                            "T' -> * F T'",
//                            "T' -> eps",
//                            "F -> n",
//                            "F -> ( E )"
//                        },
//                        {
//                            "E", "E'", "T", "T'", "F"
//                        },
//                        {
//                            "(", ")", "n", "-", "+", "*", "+", "eps"
//                        },
//                        "E");

    auto res = grammar.build_follow_set();
    for (auto const& e : res) {
        cout << e.first << ": ";
        for (auto const& r : e.second) {
            cout << r << ", ";
        }
        cout << endl;
    }
    cout << "HEEEEE" << std::endl;

//    Parser p("(a and b) or not (c xor (a or not b))");
    Parser p("(a and b)");
    auto tree = p.parse();



    ofstream fjsout("tree.js");
    fjsout << build_json_tree(tree.data());

    pid_t pid = fork();

    if (pid < 0) {
        std::cout << "Cannot fork process" << std::endl;
        return 0;
    }


    auto browser_args = get_browser_arguments();
    char* arguments[] = {browser_args.first.data(), browser_args.second.data(), nullptr};
    if (pid == 0) {
        if (execvp(arguments[0], arguments) < 0) {
            cout << "hmm" << endl;
            cout << x << endl;
        }
        cout << "Fast" << endl;
        return 111;
    } else {
        int status;
        while (wait(&status) != pid) {
        }
    }

    cout << "DONE" << endl;
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
