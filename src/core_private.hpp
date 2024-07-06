#ifndef __CORE_PRIVATE_HPP__
#define __CORE_PRIVATE_HPP__

//#define DEBUG

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <string>
#include <vector>

#include "boost/python.hpp"

#include "boost/property_tree/json_parser.hpp"

#include "exceptions.hpp"
#include "csv.hpp"

typedef struct _Profile {
    std::string name;
    std::vector<std::pair<std::string, double>> constants;
    std::vector<std::string> variables;
    boost::python::object function;
    boost::python::dict ns; //python namespace
} Profile;

void read_profile(std::filesystem::path path, Profile &p);

void func_substitute(Profile& p, csv::Table<double>& table, std::vector<double>& result, const char* maping_table);

#endif