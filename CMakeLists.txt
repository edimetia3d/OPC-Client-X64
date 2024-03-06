cmake_minimum_required(VERSION 3.5)
set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)
project(OPCClient)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${dir}/bin")

add_subdirectory(OPCClientToolKit)
add_subdirectory(OPCClientDemo)
add_subdirectory(OPCPerformance)
add_subdirectory(LocalSyncWrapper)
