cmake_minimum_required(VERSION 3.6)
project(Preprocessor)

set(CMAKE_CXX_STANDARD 11)

set(PREPROCESSOR
        Preprocessor/Preprocessor.cpp
        Preprocessor/Preprocessor.h
        )

set(UTILITY
        Utility/StringBuilder.h
        )

set(SOURCE_FILES
        ${PREPROCESSOR}
        ${UTILITY}
        )

add_executable(Compiler main.cpp
        ${SOURCE_FILES}
        )

add_executable(TestPreprocessor Test/TestPreprocessor/main.cpp
        ${SOURCE_FILES}
        )