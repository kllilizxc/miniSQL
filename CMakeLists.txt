cmake_minimum_required(VERSION 3.5)
project(miniSQL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp interpreter.h interpreter.cpp token.cpp token.h Lexer.cpp Lexer.h error.cpp error.h CatalogManager.cpp CatalogManager.h TableMeta.cpp TableMeta.h ConditionNode.cpp ConditionNode.h table.cpp table.h recordmanager.h recordmanager.cpp buffermanager.h buffermanager.cpp comparevalue.h comparevalue.cpp indexmanager.cpp translator_test_func.cpp translator_test_generator.cpp typeconvert.h typeconvert.cpp)
add_executable(miniSQL ${SOURCE_FILES})