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


//  string_expression = "a (";   // WA for S'
//  string_expression = ")";
//  string_expression = "a or b or c not (a and (a not b))";
//  string_expression = "(a and b";
//  string_expression = "(a and f) or not (c xor (a or not b))";

//  string_expression = "((a and b) or not c) xor (a and not (b)) ";
//  string_expression = "not not not not a";

int main(int argc, char* argv[]) {

    std::string string_expression = "(a and c) or not (c xor (a or not b))";
//    string_expression = "a";
//      string_expression = "a (";   // WA for S'
//      string_expression = ")";
    //  string_expression = "a or b or c not (a and (a not b))";
//      string_expression = "(a and b";
    //  string_expression = "(a and f) or not (c xor (a or not b))";

    //  string_expression = "((a and b) or not c) xor (a and not (b)) ";
    //  string_expression = "not not not not a";

    Grammar grammar({
                        "S  -> ( S ) S'",
                        "S  -> not S S'",
                        "S  -> a S'",
                        "S  -> b S'",
                        "S  -> c S'",
                        "S' -> and S S'",
                        "S' -> or S S'",
                        "S' -> xor S S'",
                        "S' -> eps"
                    },
                    {
                        "S", "S'", "Var"
                    },
                    {
                        "(", ")", "not", "and", "or", "xor", "eps", "a", "b", "c"
                    },
                    "S");

    if (argc >= 2) {
        string_expression = string(argv[1]);
    }
    if (argc == 3) {
        std::string grammar_file_name(argv[2]);
        ifstream in_grammar(grammar_file_name);
        std::map<std::string, std::vector<std::string>> gr;

        std::string line;
        std::string section;
        std::string start;
        while (getline(in_grammar, line)) {
            if (line.empty()) continue;
            if (line[0] == '#') {
                section = line.substr(1);
            } else {
                gr[section].push_back(trim(line));
            }
        }

        if (gr["rules"].empty()) {
            cout << "No rules are proveded" << endl;
            return 0;
        }
        if (gr["terminals"].empty()) {
            cout << "No terminals are provided" << endl;
            return 0;
        }
        if (gr["non terminals"].empty()) {
            cout << "No non terminals are provided" << endl;
            return 0;
        }
        if (gr["start"].empty()) {
            cout << "No start is provided" << endl;
            return 0;
        }

        grammar = Grammar(gr["rules"],
                        std::set<std::string>(gr["non terminals"].begin(), gr["non terminals"].end()),
                        std::set<std::string>(gr["terminals"].begin(), gr["terminals"].end()),
                        gr["start"][0]);
    }
    if (argc > 3) {
        cout << "too many arguments for now" << endl;
        return 0;
    }

    ofstream parser_out("parser_string.js");
    parser_out << "function parser_string() { document.getElementById('parser_string_place').innerHTML = '"s + string_expression + "';}\nparser_string();" << std::endl;

    Parser p(string_expression);;
    try {
        auto tree = p.parseR(grammar);
        ofstream fjsout("tree.js");
        fjsout << build_json_tree(tree.data()) << std::endl;
        ofstream follow_out("follow.js");
        follow_out << build_json_map(grammar.build_follow_set(), "follow") << std::endl;
        ofstream first_out("first.js");
        first_out << build_json_map(grammar.build_first_set(), "first") << std::endl;
        ofstream move_table_out("move_table.js");
        move_table_out << build_json_map(grammar.build_ll1_table(), "move_table") << std::endl;

        pid_t pid = fork();

        if (pid < 0) {
            std::cerr << "Cannot fork process" << std::endl;
            return 0;
        }

        auto browser_args = get_browser_arguments();
        char* arguments[] = {browser_args.first.data(), browser_args.second.data(), nullptr};
        if (pid == 0) {
            if (execvp(arguments[0], arguments) < 0) {
                return -1;
            }
            return 0;
        } else {
            int status;
            while (wait(&status) != pid) {
            }
        }
    } catch (parser_exception& e) {
        cout << e.what() << endl;
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


//    Grammar grammar({
//                     "S  -> ( S ) S'",
//                     "S  -> not S S'",
//                     "S  -> Var S'",
//                     "S' -> and S S'",
//                     "S' -> or S S'",
//                     "S' -> xor S S'",
//                     "S' -> eps",
//                     "Var -> a",
//                     "Var -> b",
//                     "Var -> c"
//                    },
//                    {
//                     "S", "S'", "Var"
//                    },
//                    {
//                     "(", ")", "not", "and", "or", "xor", "eps", "a", "b", "c"
//                    },
//                    "S");

//        Grammar grammar({
//                            "E -> T E'",
//                            "E' -> + T E'",
//                            "E' -> eps",
//                            "T -> F T'",
//                            "T' -> * F T'",
//                            "T' -> eps",
//                            "F -> a",
//                            "F -> b",
//                            "F -> c",
//                            "F -> d",
//                            "F -> e",
//                            "F -> f",
//                            "F -> g",
//                            "F -> n",
//                            "F -> ( E )"
//                        },
//                        {
//                            "E", "E'", "T", "T'", "F"
//                        },
//                        {
//                            "(", ")", "n", "-", "+", "*", "+", "eps", "$", "a", "b", "c", "d", "e", "f", "g"
//                        },
//                        "E");

//        Grammar grammar ({
//                             "E -> T X",
//                             "X -> + T X",
//                             "X -> - T X",
//                             "X -> eps",
//                             "T -> F Y",
//                             "Y -> * F Y",
//                             "Y -> eps",
//                             "F -> - F",
//                             "F -> + F",
//                             "F -> n",
//                             "F -> ( E )"
//                         },
//                         {
//                            "E", "E'", "T", "T'", "F"
//                         },
//                         {
//                            "+", "-", "*", "n", "(", ")", "eps", "$"
//                         },
//                         "E"
//                         );
