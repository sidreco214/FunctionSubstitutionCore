#ifndef _CSV_HPP_
#define _CSV_HPP_

#include <iostream>
#include <sstream>

#include <filesystem>
#include <vector>
#include <algorithm>

#include "exceptions.hpp"

namespace csv {

template<typename T>
class Table {
    protected:
        std::vector<std::vector<T>> data;
        std::vector<std::string> col_name;
    
    public:
    Table() = default;
    template<typename _T>
    friend void read_csv(std::filesystem::path path, Table<_T>& table);
    template<typename _T>
    friend std::ostream& operator<<(std::ostream& ost, Table<_T>& table);

    size_t row_size() {return data.size();};
    size_t col_size() {return col_name.size();};

    void get_colv(size_t index, std::vector<T>& colv);
    void get_colv(std::string name, std::vector<T>& colv);

    void expand_row(std::vector<T>& rowv);
    void expand_col(std::string name, std::vector<T>& colv);
};

template<typename _T>
std::ostream &operator<<(std::ostream &ost, Table<_T> &table) {
    std::vector<std::string>::iterator name = table.col_name.begin();
    ost << *name++;
    for(; name != table.col_name.end(); ++name) {
        ost << ',' << *name;
    }
    ost << std::endl;

    for(auto row : table.data) {
        auto item = row.begin();
        ost << *item++;
        for(; item != row.end(); ++item) {
            ost << ',' << *item;
        }
        ost << std::endl;
    }
    return ost;
}

template<typename T>
void read_csv(std::filesystem::path path, Table<T>& table) {
    table.col_name = {};
    table.data = {};

    std::stringstream ss;
    std::ifstream in(path.string());
    if(!in.is_open()) throw ErrMsg("Error(read_csv): Could not open file");
    ss << in.rdbuf();

    #ifdef DEBUG
    std::cout << "read from file\n" << ss.str() << std::endl;
    #endif

    std::string line;
    size_t pos = 0;
    getline(ss, line);
    while((pos = line.find(',')) != std::string::npos) {
        table.col_name.push_back(line.substr(0, pos));
        line.erase(0, pos+1);
    }
    table.col_name.push_back(line);

    while(std::getline(ss, line)) {
        std::vector<T> v;
        pos = 0;
        while((pos = line.find(',')) != std::string::npos) {
            v.push_back(std::stod(line.substr(0, pos)));
            line.erase(0, pos + 1);
        }
        v.push_back(std::stod(line));
        table.data.push_back(v);
    }
}

template <typename T>
inline void Table<T>::get_colv(size_t index, std::vector<T>& colv) {
    colv.reserve(this->col_size());
    for(auto row : this->data) {
        colv.push_back(row.at(index));
    }
}

template <typename T>
inline void Table<T>::get_colv(std::string name, std::vector<T>& colv) {
    size_t index = 0;
    std::vector<std::string>::iterator iter = std::find(this->col_name.begin(), this->col_name.end(), name);
    if(iter == this->col_name.end()) throw ErrMsg("Error(csv): Could not found the column");
    index = iter - this->col_name.begin();
    this->get_colv(index, colv);
}

template <typename T>
inline void Table<T>::expand_row(std::vector<T> &rowv) {
    if(this->col_size() != rowv.size()) throw ErrMsg("Error(csv): Could not expand to different size row");
    this->data.push_back(rowv);
}

template <typename T>
inline void Table<T>::expand_col(std::string name, std::vector<T> &colv) {
    if(this->row_size() != colv.size()) throw ErrMsg("Error(csv): Could not expand to different size column");

    //중복 검사
    std::vector<std::string>::iterator iter = std::find(this->col_name.begin(), this->col_name.end(), name);
    if(iter != this->col_name.end()) throw ErrMsg("Error(csv): Duplicated name of column is not allowed");

    #ifdef DEBUG
    for(auto item : this->col_name) {
        std::cout << item << ", ";
    }
    std::cout << std::endl;
    std::cout << "add " << name << std::endl;
    #endif
    
    this->col_name.push_back(name);
    for(int i=0; i<this->row_size(); ++i) {
        data.at(i).push_back(colv.at(i));
    }
}

}
#endif