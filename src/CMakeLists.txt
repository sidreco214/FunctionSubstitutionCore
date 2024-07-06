add_library(FunctionSubstitutionCore SHARED
	FunctionSubstitutionCore.h
	FunctionSubstitutionCore.cpp
	core_private.hpp
	core_private.cpp
	exceptions.hpp
	csv.hpp
)

target_include_directories(FunctionSubstitutionCore
	PRIVATE ${Python_INCLUDE_DIRS}
	PRIVATE ${Boost_INCLUDE_DIRS}
)

target_link_libraries(FunctionSubstitutionCore
	${Python_LIBRARIES}
	${Boost_LIBRARIES}
)

set_target_properties(FunctionSubstitutionCore PROPERTIES
	DEBUG_POSTFIX "-d"
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}
)

add_executable(core_test core_test.cpp)
target_link_libraries(core_test
	FunctionSubstitutionCore
)

set_target_properties(core_test PROPERTIES
	DEBUG_POSTFIX "-d"
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}
)