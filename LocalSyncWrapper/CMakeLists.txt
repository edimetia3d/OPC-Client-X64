project(LocalSyncWrapper)

include_directories(../OPCClientToolKit)
set(SOURCE_H LocalSyncOPCCLient.h)
set(SOURCE_CPP LocalSyncOPCCLient.cpp main.cpp)
set(SOURCE_EXE ${SOURCE_H} ${SOURCE_CPP})

if (OPCDACLIENT_STATIC)
  add_definitions(-DNO_OPCDACLIENT_LIBRARY)
endif()

add_executable(LocalSyncWrapper ${SOURCE_EXE})
target_link_libraries(LocalSyncWrapper OPCClientToolKit ws2_32)

install(TARGETS LocalSyncWrapper DESTINATION bin)
