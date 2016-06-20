cmake_minimum_required(VERSION 3.5)
project(miniSQL)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp interpreter.h interpreter.cpp token.cpp token.h Lexer.cpp Lexer.h error.cpp error.h CatalogManager.cpp CatalogManager.h TableMeta.cpp TableMeta.h ConditionNode.cpp ConditionNode.h)
add_executable(miniSQL ${SOURCE_FILES})