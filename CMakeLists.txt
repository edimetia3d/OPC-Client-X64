cmake_minimum_required(VERSION 3.5)
set(CMAKE_POLICY_DEFAULT_CMP0079 NEW)
project(opcclient)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_subdirectory(OPCClientToolKit)
add_subdirectory(OPCClientDemo)
add_subdirectory(OPCPerformance)
add_subdirectory(LocalSyncWrapper)

set(CPACK_PACKAGE_VENDOR ${project})
set(CPACK_PACKAGE_NAME ${project})
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 5)
set(CPACK_PACKAGE_VERSION_PATCH 1)
include(CPack)
