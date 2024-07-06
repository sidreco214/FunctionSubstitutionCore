#include "FunctionSubstitutionCore.h"

//#define DEBUG

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>

#include <string>
#include <format>

#include <array>

#include <windows.h>

#include "boost/python.hpp"
#include "core_private.hpp"

namespace fs = std::filesystem;
namespace bp = boost::python;

int init_python(const char *cwd, const char *python_path) {
    //API Reference https://docs.python.org/ko/3/c-api/init_config.html#python-path-configuration

    //get python absolute path
    fs::path pyPath = fs::canonical(fs::path(python_path));

    PyStatus status;
    PyConfig config;
    PyConfig_InitIsolatedConfig(&config);
    try {
        //set program name
        status = PyConfig_SetBytesString(&config, &config.program_name, cwd);
        if(PyStatus_Exception(status)) throw status;

        // Read all configuration at once to verify
        status = PyConfig_Read(&config);
        if(PyStatus_Exception(status)) throw status;

        //set python_home
        status = PyConfig_SetString(&config, &config.home, pyPath.wstring().c_str());
        if(PyStatus_Exception(status)) throw status;

        //set sys.path explictly
        //해당 폴더 없을 시 에러
        config.module_search_paths_set = 1;
        std::array<fs::path, 2> path_arr = {pyPath/"DLLs", pyPath/"Lib"};
        
        for(auto iter : path_arr) {
            status = PyWideStringList_Append(&config.module_search_paths, fs::canonical(iter).wstring().c_str());
            if(PyStatus_Exception(status)) throw status;
        }

        status = Py_InitializeFromConfig(&config);
        if(PyStatus_Exception(status)) throw status;
        PyConfig_Clear(&config);

        PyRun_SimpleString("from math import *");
        PyRun_SimpleString("defchk = lambda x : str(x) in globals()");

    }
    catch(PyStatus& status) {
        PyConfig_Clear(&config);
        std::cerr << "Initailizing Error: " << status.err_msg << std::endl;
        MessageBoxExA(NULL, status.err_msg, "Python Initalizing Error", MB_OK, 0);
        return 1;
    }
    return 0;
}

void deinit_python() {
    Py_Finalize();
}

int process_data(processing_info *info) {
    try{
        //info null check

        #ifdef DEBUG
        std::cout << "read profile" << std::endl;
        #endif
        Profile profile;
        read_profile(fs::canonical(fs::path(info->profile_path)), profile);

        #ifdef DEBUG
        std::cout << "read csv" << std::endl;
        #endif
        csv::Table<double> table;
        csv::read_csv(fs::path(info->csv_path), table);

        #ifdef DEBUG
        std::cout << "function substitution" << std::endl;
        #endif
        std::vector<double> result;
        func_substitute(profile, table, result, info->mapping_table);

        table.expand_col(info->output_col_name, result);

        #ifdef DEBUG
        std::cout << "write result csv file" << std::endl;
        #endif
        std::ofstream ofs(info->output_file_name);
        ofs << table << std::flush;

    }
    catch(bp::error_already_set&) {
        bp::object exc = bp::object(bp::handle(PyErr_GetRaisedException()));
        bp::object traceback = bp::import("traceback");
        bp::object format_exception = traceback.attr("format_exception");
        bp::list msgList = bp::list(format_exception(exc));
        
        std::stringstream ss;
        for(int i=0; i < len(msgList); ++i) ss << bp::extract<char*>(msgList[i]);

        std::cerr << ss.str() << std::endl;
        MessageBoxExA(NULL, ss.str().c_str(), "Python Error", MB_OK, 0);
        return 1;
    }
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
        MessageBoxExA(NULL, e.what(), "Error", MB_OK, 0);
        return 1;
    }
    return 0;
}

int make_funcImg(const char *funcStr, const char *fileName, unsigned int fontsize, unsigned int dpi) {
    try{
        #ifdef DEBUG
        std::cout << "called make_funcImg" << std::endl;
        #endif

        //inject __builtins__
        #ifdef DEBUG
        std::cout << "inject builtins" << std::endl;
        #endif
        bp::dict ns;
        ns["__builtins__"] = bp::import("builtins");

        #ifdef DEBUG
        std::cout << "import modules" << std::endl;
        #endif
        bp::exec("from sympy import *", ns);
        bp::exec("from sympy.abc import *", ns); //exports all latin and greek letters as Symbols
        bp::exec("import matplotlib.pyplot as plt", ns);
        
        
        //convert to latex string
        #ifdef DEBUG
        std::cout << "convert latex string" << std::endl;
        #endif
        std::string latex_str = bp::extract<std::string>(bp::eval(std::format("repr(latex({}))", funcStr).c_str(), ns));
        latex_str = latex_str.substr(1, latex_str.length()-2);

        #ifdef DEBUG
        std::cout << "original string: " << funcStr << std::endl;
        std::cout << "latex string: " << latex_str << std::endl;
        #endif

        //draw picture
        #ifdef DEBUG
        std::cout << "draw picture" << std::endl;
        #endif
        bp::exec("fig = plt.figure(figsize=(3, 0.5))", ns);
        bp::exec(std::format("fig.text(0, 0, '${}$', fontsize={})", latex_str, fontsize).c_str(), ns);
        bp::exec(std::format("plt.savefig(\"{}\", dpi={}, transparent=True, bbox_inches=\"tight\", pad_inches=0.0)", fileName, dpi).c_str(), ns);
        bp::exec("plt.close()", ns);
      
    }
    catch(bp::error_already_set&) {
        bp::object exc = bp::object(bp::handle(PyErr_GetRaisedException()));
        bp::object traceback = bp::import("traceback");
        bp::object format_exception = traceback.attr("format_exception");
        bp::list msgList = bp::list(format_exception(exc));
        
        std::stringstream ss;
        for(int i=0; i < len(msgList); ++i) ss << bp::extract<char*>(msgList[i]);

        std::cerr << ss.str() << std::endl;
        MessageBoxExA(NULL, ss.str().c_str(), "Python Error", MB_OK, 0);
        return 1;
    }
    catch(std::exception &e) {
        std::cerr << e.what() << std::endl;
        MessageBoxExA(NULL, e.what(), "Error", MB_OK, 0);
        return 1;
    }
    

    return 0;
}
