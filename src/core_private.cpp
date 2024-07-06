#include "core_private.hpp"
namespace fs = std::filesystem;
namespace bp = boost::python;

void read_profile(fs::path path, Profile &p) {
    std::stringstream ss;
    std::ifstream in(path.string());
    if(!in.is_open()) throw ErrMsg("Error(read_profile): Could not found the profile");
    ss << in.rdbuf();
    #ifdef DEBUG
    std::cout << "read " << path.string() << std::endl << ss.str() << std::endl;
    #endif
    
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);
    boost::property_tree::ptree::assoc_iterator iter;

    //read string from json
    p.name = pt.get<std::string>("name", "None");
    if(p.name == "None") throw ErrMsg("Error(read_profile): Could not found name");

    iter = pt.find("constants");
    if(iter->first.empty()) throw ErrMsg("Error(read_profile): Could not found constans");
    for(auto item : iter->second) {
        auto element = std::pair<std::string, double>(item.first, std::stod(item.second.data()));
        p.constants.push_back(element);
    }

    iter = pt.find("variables");
    if(iter->first.empty()) throw ErrMsg("Error(read_profile): Could not found variables");
    for(auto item : iter->second) {
        p.variables.push_back(item.second.data());
    }

    std::string func_str = pt.get<std::string>("function", "None");
    if(func_str == "None") throw ErrMsg("Error(read_profile): Could not found function");

    #ifdef DEBUG
    std::cout << "\n===================\nresult of parsing json" << std::endl;
    std::cout << "name: " << p.name << std::endl;

    std::cout << "constants: ";
    for(auto item : p.constants) {
        std::cout << item.first << ": " << item.second << ", ";
    }
    std::cout << std::endl;

    std::cout << "variables: ";
    for(auto item : p.variables) {
        std::cout << item << ", ";
    }
    std::cout << std::endl;

    std::cout << "function:" << func_str << std::endl;
    #endif

    //inject __builtins__
    #ifdef DEBUG
    std::cout << "inject builtins" << std::endl;
    #endif
    p.ns["__builtins__"] = bp::import("builtins");
    bp::exec("from math import *", p.ns);

    //define constans and function in python
    #ifdef DEBUG
    std::cout << "define constants and function in python" << std::endl;
    #endif
    ss.clear();
    ss.str("");
    for(auto item : p.constants) {
        ss << item.first << " = " << item.second << std::endl;
        bp::exec(ss.str().c_str(), p.ns);
        ss.str("");
        ss.clear();
    }
    
    ss << "function = lambda ";
    auto element = p.variables.begin();
    ss << *element++;
    for(; element != p.variables.end(); ++element) {
        ss << ", " << *element;
    }
    ss << " : " << func_str;
    bp::exec(ss.str().c_str(), p.ns);

    ss.clear();
    ss.str("");
    ss << "dfunc = lambda dict : function(**dict)";
    bp::exec(ss.str().c_str(), p.ns);
    p.function = p.ns["dfunc"];
};

void func_substitute(Profile& p, csv::Table<double>& table, std::vector<double>& result, const char* mapping_table) {
    result.reserve(table.row_size());
    
    #ifdef DEBUG
    std::cout << "mapping table parsing" << std::endl;
    #endif
    //mapping table parsing
    //form: variable1:column_index1;variable2:column_index2
    //make a pair of variable name and variable vector
    std::vector<std::pair<std::string, std::vector<double>>> entries;
    std::string str = std::string(mapping_table);
    for(size_t pos=0; str.length() > 1;) {
        pos = str.find(':');
        if(pos == std::string::npos) throw ErrMsg("Parse Error(func_substitution): mapping table");
        std::string name = str.substr(0, pos);
        str.erase(0, pos+1);

        pos = str.find(";");
        if(pos == std::string::npos) throw ErrMsg("Parse Error(func_substitution): mapping table");
        int col_num = std::stoi(str.substr(0, pos));
        str.erase(0, pos+1);

        std::vector<double> v;
        table.get_colv(col_num, v);
        entries.push_back(std::pair(name, v));
    }

    #ifdef DEBUG
    std::cout << "call dfunc" << std::endl;
    #endif

    //call dfunc
    for(int i=0; i< table.row_size(); ++i) {
        bp::dict kargs;
        for(auto entry : entries) {
            kargs[entry.first] = entry.second.at(i);
        }
        result.push_back(bp::extract<double>(p.function(kargs)));
        
    }
};