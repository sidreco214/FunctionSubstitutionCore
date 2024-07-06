#pragma once

#ifdef FunctionSubstitutionCore_EXPORTS
#define FunctionSubstitutionCore_API __declspec(dllexport)
#else
#define FunctionSubstitutionCore_API __declspec(dllimport)
#endif

/// @brief Initializing Python Interpreter
/// @return If success, return 0. Otherwise, return 1.
extern "C" FunctionSubstitutionCore_API int init_python(const char* cwd, const char* python_path);
extern "C" FunctionSubstitutionCore_API void deinit_python();

/// @brief Processing Information Structure
extern "C" FunctionSubstitutionCore_API typedef struct _processing_info {
    const char* profile_path;
    const char* csv_path;
    const char* output_col_name;
    const char* output_file_name;
    const char* mapping_table; //form: variable1:column_index1;variable2:column_index2
} processing_info;

/// @brief Processing function substition
/// @param info structure of processing_info
/// @return If success, return 0. Otherwise, return 1.
extern "C" FunctionSubstitutionCore_API int process_data(processing_info* info);

/// @brief Making function image from python function string
/// @param funcStr python function string
/// @param fileName name of destnation file
/// @param fontsize size of font
/// @param dpi dot per inche
/// @return If success, return 0. Otherwise, return 1.
extern "C" FunctionSubstitutionCore_API int make_funcImg(const char *funcStr, const char *fileName, unsigned int fontsize, unsigned int dpi);