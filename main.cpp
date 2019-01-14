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


int main(int argc, char* argv[]) {

	std::string string_expression = "id + id * (id * (id) + id)";    // OK

    // original grammar //
    /*
       S -> O
       O -> X or X
       O -> X
       X -> A xor A
       X -> A
       A -> R and R
       A -> R
       R -> L shr L
       R -> L
       L -> N shl N
       L -> N
       N -> not V
       N -> V
       V -> a
       V -> b
       V -> c
       V -> (S)
    */

//    Grammar grammar({
//                        "S -> O",

//                        "O -> X O'",
//                        "O' -> or O",
//                        "O' -> eps",

//                        "X -> A X'",
//                        "X' -> xor X",
//                        "X' -> eps",

//                        "A -> N A'",
//                        "A' -> and A",
//                        "A' -> eps",

//                        "R -> L R'",
//                        "R' -> shl R",
//                        "R' -> eps",

//                        "L -> N L'",
//                        "L' -> shr L",
//                        "L' -> eps",

//                        "N -> not V",
//                        "N -> V",

//                        "V -> a",
//                        "V -> b",
//                        "V -> c",
//                        "V -> ( S )"
//                    },
//                    {
//                        "S", "N", "A", "O", "X", "V", "R", "L",
//                        "S'", "A'", "O'", "X'", "R'", "L'"
//                    },
//                    {
//                        "(", ")", "not", "and", "or", "xor", "eps", "a", "b", "c", "shr", "shl"
//                    },
//                    "S");

	Grammar grammar({
						"X -> E",
						"E -> E + T",
						"E -> T",
						"T -> T * F",
						"T -> F",
						"F -> ( E )",
						"F -> id",
					},
					{
						"X", "E", "T", "F"
					},
					{
						"id", "(", ")", "eps", "+", "*"
					},
					"X"
					);

////	Reduce-Reduce Conflict
//	Grammar grammar({
//						"S' -> S",
//						"S 	-> 	e B b",
//						"S 	-> 	e A",
//						"A 	-> 	a",
//						"A 	-> 	eps",
//						"B 	-> 	a",
//						"B 	-> 	a D",
//						"D 	-> 	b",
//					},
//					{
//						"S'", "S", "B", "A", "D"
//					},
//					{
//						"e", "a", "b", "eps",
//					},
//					"S'");

//	Grammar grammar({
//						"S -> E",
//						"E -> L = R",
//						"E -> R",
//						"L -> id",
//						"L -> * R",
//						"R -> L"
//					},
//					{
//						"S", "E", "L", "R"
//					},
//					{
//						"id", "*", "=", "eps"
//					},
//					"S");

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


	try {
	auto xxxx = grammar.build_goto_table();

	cout << "Size = " << xxxx.size() << endl;

	for (auto const& entity : xxxx) {
		cout << entity.first << endl;
		for (auto const& rules : entity.second) {
			for (auto const& rhs_rule : rules.second) {
				cout << "       " + rules.first << " -> " <<
						rhs_rule << endl;
			}
		}
	}

	cout << string(33, '-') << endl << endl;

	for (auto const& entity : xxxx) {
		cout << entity.first << " : " << endl;
		for (std::string const& value : grammar.get_terminals()) {
			auto temp = grammar.get_action(entity.first, value);

			cout << "       ";
			cout << value << " ";
			cout << temp.first << " " << temp.second << endl;
		}

		for (std::string const& value : grammar.get_nonTerminals()) {
			auto temp = grammar.get_action(entity.first, value);

			cout << "       ";
			cout << value << " ";
			cout << temp.first << " " << temp.second << endl;
		}

		cout << "       " << "$ " << grammar.get_action(entity.first, "$").first << " " << grammar.get_action(entity.first, "$").second << endl;
	}

	} catch (std::exception& e) {

	}

	ofstream parser_out("parser_string.js");
	parser_out << "function parser_string() { document.getElementById('parser_string_place').innerHTML = '"s + string_expression + "';}\nparser_string();" << std::endl;

	Parser p(string_expression, grammar.get_terminals());;  // for provided grammar
//    Parser p(string_expression);                              // for pascal logic
	ofstream fjsout("tree.js");
	ofstream move_table_out("move_table.js");

	try {
		auto tree = p.parseSLR(grammar);    // for provided grammar
//        auto tree = p.parse();              // for pascal logic

		fjsout << build_json_tree(tree.data()) << std::endl;
		move_table_out << build_json_map(grammar.build_slr_table(), "move_table") << std::endl;

	} catch (parser_exception& e ) {
		cout << string_expression << ": ";
		cout << e.what() << endl;
	} catch (std::runtime_error& e) {
		cout << string_expression << ": ";
		cout << e.what() << endl;
	}

	ofstream follow_out("follow.js");
	follow_out << build_json_map(grammar.build_follow_set(), "follow") << std::endl;
	ofstream first_out("first.js");
	first_out << build_json_map(grammar.build_first_set(), "first") << std::endl;

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

    return 0;
}
