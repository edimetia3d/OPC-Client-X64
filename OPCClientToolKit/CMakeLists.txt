project(OPCClientToolKit)

set(SOURCE_H OpcEnum.h opccomn.h opcda.h
    OPCClient.h OPCHost.h OPCItemData.h OPCServer.h
    OPCGroup.h OPCItem.h OPCProperties.h
    Transaction.h OPCClientToolKitDLL.h)
set(SOURCE_CCPP OpcEnum_i.c opccomn_i.c opcda_i.c
    OPCClient.cpp OPCHost.cpp OPCItemData.cpp OPCServer.cpp
    OPCGroup.cpp OPCItem.cpp OPCProperties.cpp
    Transaction.cpp)
set(SOURCE_LIB ${SOURCE_H} ${SOURCE_CCPP})

add_definitions(-DOPCDACLIENT_LIBRARY -DWIN32_LEAN_AND_MEAN)
add_library(OPCClientToolKit SHARED ${SOURCE_LIB})

install(FILES ${SOURCE_H} DESTINATION include/OPCClientToolKit)
install(TARGETS OPCClientToolKit
    EXPORT opcclientTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
