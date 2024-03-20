project(OPCPerformance)

include_directories(../OPCClientToolKit)
set(SOURCE_CPP OPCPerformance.cpp)
set(SOURCE_EXE ${SOURCE_H} ${SOURCE_CPP})

if (OPCDACLIENT_STATIC)
  add_definitions(-DNO_OPCDACLIENT_LIBRARY)
endif()

add_executable(OPCPerformance ${SOURCE_EXE})
target_link_libraries(OPCPerformance OPCClientToolKit)

install(TARGETS OPCPerformance DESTINATION bin)
