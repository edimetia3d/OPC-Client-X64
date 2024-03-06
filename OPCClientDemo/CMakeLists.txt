project(OPCClientDemo)

include_directories(../OPCClientToolKit)
set(SOURCE_H stdafx.h)
set(SOURCE_CPP OPCClientDemo.cpp stdafx.cpp)
set(SOURCE_EXE ${SOURCE_H} ${SOURCE_CPP})

add_executable(OPCClientDemo ${SOURCE_EXE})
target_link_libraries(OPCClientDemo OPCClientToolKit)
