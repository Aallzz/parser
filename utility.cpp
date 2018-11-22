#include "utility.h"

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
    res += indent + "node: { HTMLclass: 'nodeExample1'},\n"s;
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

std::ostream& operator << (std::ostream& out, Token token) {
    out << token_names[static_cast<std::underlying_type<Token>::type>(token)];
    return out;
}

std::string trim(std::string str) {
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}
