project(OPCClientToolKit)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(SOURCE_H OpcEnum.h opccomn.h opcda.h
    OPCClient.h OPCHost.h OPCItemData.h OPCServer.h
    OPCGroup.h OPCItem.h OPCProperties.h
    Transaction.h OPCClientToolKitDLL.h)
set(SOURCE_CCPP OpcEnum_i.c opccomn_i.c opcda_i.c
    OPCClient.cpp OPCHost.cpp OPCItemData.cpp OPCServer.cpp
    OPCGroup.cpp OPCItem.cpp OPCProperties.cpp
    Transaction.cpp)
set(SOURCE_LIB ${SOURCE_H} ${SOURCE_CCPP})

if (OPCDACLIENT_STATIC)
  add_definitions(-DNO_OPCDACLIENT_LIBRARY)
  add_library(OPCClientToolKit STATIC ${SOURCE_LIB})
else()
  add_definitions(-DOPCDACLIENT_LIBRARY)
  add_library(OPCClientToolKit SHARED ${SOURCE_LIB})
endif()

install(FILES ${SOURCE_H} DESTINATION include/OPCClientToolKit)
install(TARGETS OPCClientToolKit
    EXPORT opcclientTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
