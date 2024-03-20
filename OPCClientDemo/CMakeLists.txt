project(OPCClientDemo)

include_directories(../OPCClientToolKit)
set(SOURCE_CPP OPCClientDemo.cpp)
set(SOURCE_EXE ${SOURCE_H} ${SOURCE_CPP})

if (OPCDACLIENT_STATIC)
  add_definitions(-DNO_OPCDACLIENT_LIBRARY)
endif()

add_executable(OPCClientDemo ${SOURCE_EXE})
target_link_libraries(OPCClientDemo OPCClientToolKit)

install(TARGETS OPCClientDemo DESTINATION bin)
