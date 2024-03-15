project(OPCClientToolKit)

set(SOURCE_H OpcEnum.h opccomn.h opcda.h
    OPCClient.h OPCHost.h OPCItemData.h OPCServer.h
    stdafx.h OPCGroup.h OPCItem.h OPCProperties.h
    Transaction.h OPCClientToolKitDLL.h)
set(SOURCE_CCPP OpcEnum_i.c opccomn_i.c opcda_i.c
    OPCClient.cpp OPCHost.cpp OPCItemData.cpp OPCServer.cpp
    stdafx.cpp OPCGroup.cpp OPCItem.cpp OPCProperties.cpp
    Transaction.cpp)
set(SOURCE_LIB ${SOURCE_H} ${SOURCE_CCPP})

add_definitions(-DOPCDACLIENT_LIBRARY)
add_library(OPCClientToolKit SHARED ${SOURCE_LIB})
