project(OPCPerformance)

include_directories(../OPCClientToolKit)
set(SOURCE_H stdafx.h)
set(SOURCE_CPP OPCPerformance.cpp stdafx.cpp)
set(SOURCE_EXE ${SOURCE_H} ${SOURCE_CPP})

add_executable(OPCPerformance ${SOURCE_EXE})
target_link_libraries(OPCPerformance OPCClientToolKit)
