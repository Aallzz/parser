#include "utility.h"
#include <iostream>

std::string build_json_tree(std::vector<std::string> const& tree) {
//    using std::string_literals::operator""s;
    using namespace std; // to avoid annoying ""s warning
    std::string res;
    std::map<size_t, std::string> lst;
    int node_id {};
    std::string indent("            ");
    res += "var config = {\n"s;
    res += indent + "container: \""s + "#my_tree"s + "\",\n"s;
    res += indent + "connectors: { type: 'step'},\n"s;
    res += indent + "node: { HTMLclass: 'tree-style'},\n"s;
    res += "        },\n";

    for (auto const& branch : tree) {
        size_t w = branch.find_first_not_of(" ");
        std::string current_node = "node" + std::to_string(node_id);
        node_id++;
        res += "        "s + current_node + " = {\n"s;
        if (lst.count(w - 2)) {
            res += indent + "parent: " + lst[w - 2] + ",\n";
        }
        res += indent + "text: {name: \""s + branch.substr(w) + "\"}\n        },\n"s;
        lst[w] = current_node;
    }

    res += "        chart_config = [\n";
    res += indent + "config,\n";
    for (int i = 0; i < node_id; ++i) {
        res += indent + "node"s + std::to_string(i) + ",\n";
    }
    res += "        ];\n";
    return res;
}

std::string build_json_map(std::map<std::string, std::set<std::string>> const& mp, std::string where) {
//    using std::string_literals::operator""s;
    using namespace std; // to avoid annoying ""s warning
    std::string res;
    res += "function generate_" + where + "_table() {\n"s;
    res += "    var div = document.getElementById('" + where + "_place');\n"s;
    res += "    var tbl = document.createElement('" + where + "_table');\n"s;
    res += "    var tblBody = document.createElement('tbody');\n"s;
    res += "    var row = document.createElement('tr');\n"s;
    res += "    var cell_caption = document.createElement('td');\n"s;
    res += "    var cell_caption_text = document.createTextNode('"s + where + "_set');\n"s;
    res += "    cell_caption.appendChild(cell_caption_text);\n"s;
    res += "    row.appendChild(cell_caption);\n"s;
    res += "        tblBody.appendChild(row);\n"s;
    res += "    var nonTerms = [\n";
    for (auto const& assoc : mp) {
        res += "            \""s +assoc.first + "\",\n"s;
    }
    res += "    ];\n"s;
    res += "    var term_sets = [\n";
    for (auto const& assoc : mp) {

        std::string term_set = "'{";
        bool comma = {false};
        for_each(assoc.second.begin(), assoc.second.end(), [&comma, &term_set](auto const& term) {
            if (comma) {
                term_set += ", ";
            }
            comma = true;
            term_set += term;
        });
        term_set += "}'";

        res += "            "s +term_set + ",\n"s;
    }
    res += "    ];\n";

    res += "    for (var i = 0; i < "s + to_string(mp.size()) + "; i++){\n"s;
    res += "        var row = document.createElement('tr');\n"s;
    res += "        var cell_nonTerm = document.createElement('td');\n"s;
    res += "        var cell_term_set = document.createElement('td');\n"s;
    res += "        var cell_nonTerm_text = document.createTextNode(nonTerms[i] + \": \");\n"s;
    res += "        var cell_term_set_text = document.createTextNode(term_sets[i]);\n"s;
    res += "        cell_nonTerm.appendChild(cell_nonTerm_text);\n"s;
    res += "        row.appendChild(cell_nonTerm);\n"s;
    res += "        cell_term_set.appendChild(cell_term_set_text);\n"s;
    res += "        row.appendChild(cell_term_set);\n"s;
    res += "        tblBody.appendChild(row);\n"s;
    res += "    };\n";

    res += "    tbl.appendChild(tblBody);\n";
    res += "    div.appendChild(tbl);\n";
    res += "    tbl.setAttribute('border', '2');\n";
    res += "};\n";
    res += "generate_" + where + "_table();\n";

    return res;
}

std::ostream& operator << (std::ostream& out, Token token) {
    out << token_names[static_cast<std::underlying_type<Token>::type>(token)];
    return out;
}

std::string trim(std::string str) {
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}

std::pair<std::string, std::string> get_browser_arguments() {
    char cCurrentPath[FILENAME_MAX];
    if (!getcwd(cCurrentPath, sizeof(cCurrentPath))) {
        throw std::exception();
    }
    std::string f(cCurrentPath);
    f += "/index.html";
    std::string ff("firefox");
    return {ff, f};
}

