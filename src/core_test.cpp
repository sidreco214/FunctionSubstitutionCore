#include <iostream>
#include <conio.h>
#include "FunctionSubstitutionCore.h"

int main(int argc, char* argv[]) {
    std::cout << "init python" << std::endl;
    init_python(argv[0], "python");

    //create mapping table
    std::cout << "mapping table:" << "x:0;y:1;" << std::endl;
    std::cout << "processing data" << std::endl;
    processing_info info = {
        "sample.json",
        "test.csv",
        "result",
        "result.csv",
        "x:0;y:1;"
    };
    process_data(&info);

    std::cout << "\nerror test" << std::endl;
    processing_info info2 = {
        "sample.json",
        "result.csv",
        "result",
        "result2.csv",
        "x:0;y:1;"
    };
    process_data(&info2);

    std::cout << "==========================\nmake function image" << std::endl;
    make_funcImg("sin(a*x) + b*y + c", "result.png", 12, 130);

    std::cout << "deinit python" << std::endl;
    deinit_python();
    std::cout << "done" << std::endl;
    _getch();
    return 0;
}