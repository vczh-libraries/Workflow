cmake_minimum_required(VERSION 3.3.0)

project(CppMerge LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(SRCS
    ../../Import/Vlpp.cpp
    ../../Import/VlppOS.cpp
    ../../Import/VlppRegex.cpp
    ../../Source/Cpp/WfMergeCpp.cpp
    CppMerge/Main.cpp
)

if (WIN32)
    list(APPEND SRCS ../../Import/Vlpp.Windows.cpp ../../Import/VlppOS.Windows.cpp)
else()
    list(APPEND SRCS ../../Import/Vlpp.Linux.cpp ../../Import/VlppOS.Linux.cpp)
endif()

add_executable(CppMerge ${SRCS})

if(MSVC)
    target_compile_options(CppMerge PRIVATE /bigobj)
    target_compile_definitions(CppMerge PRIVATE UNICODE _UNICODE)
endif()

target_include_directories(CppMerge PRIVATE ../../Import)

install(TARGETS CppMerge RUNTIME DESTINATION "bin")
