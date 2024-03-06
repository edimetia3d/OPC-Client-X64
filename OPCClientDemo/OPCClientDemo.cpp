// OPCClientDemo.cpp : Defines the entry point for the console application.
//

/*
OPCClientToolKit
Copyright (C) 2005 Mark C. Beharrell

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.
*/

#include <stdio.h>
#include <sys\timeb.h>

#include "OPCClient.h"
#include "OPCGroup.h"
#include "OPCHost.h"
#include "OPCItem.h"
#include "OPCServer.h"
#include "opcda.h"

extern "C" {
#define MIDL_DEFINE_GUID(type, name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)                                        \
    const type name = {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}

    MIDL_DEFINE_GUID(IID, IID_CATID_OPCDAServer20, 0x63D5F432, 0xCFE4, 0x11d1, 0xB2, 0xC8, 0x00, 0x60, 0x08, 0x3B, 0xA1,
                     0xFB);

#undef MIDL_DEFINE_GUID
}

/**
 * Code demonstrates
 * 1) Browsing local servers.
 * 2) Connection to local server.
 * 3) Browsing local server namespace
 * 4) Creation of OPC item and group
 * 5) sync and async read of OPC item.
 * 6) sync and async write of OPC item.
 * 7) The receipt of changes to items within a group (subscribe)
 * 8) group refresh.
 * 9) Sync read of multiple OPC items.
 */

/**
 * Handle async data coming from changes in the OPC group
 */
class CMyCallback : public IAsyncDataCallback
{
  public:
    void OnDataChange(COPCGroup &group, COPCItemDataMap &changes)
    {
        printf("group '%ws', item(s) changed:\n", group.getName().c_str());
        POSITION pos = changes.GetStartPosition();
        while (pos)
        {
            OPCHANDLE handle = changes.GetKeyAt(pos);
            OPCItemData *data = changes.GetNextValue(pos);

            if (data)
            {
                const COPCItem *item = data->item(); // retrieve original item pointer from item data..

                if (item)
                {
                    printf("-----> '%ws', handle: %u, changed async read quality %d value %d\n",
                           item->getName().c_str(), handle, data->wQuality, data->vDataValue.iVal);
                }
            } // if
        }     // while

    } // OnDataChange

}; // CMyCallback

/**
 *	Handle completion of async operation
 */
class CTransComplete : public ITransactionComplete
{
  private:
    std::string CompletionMessage;

  public:
    CTransComplete()
    {
        CompletionMessage = "async operation completed";
    }

    void complete(CTransaction &)
    {
        printf("%s\n", CompletionMessage.c_str());
    }

    void setCompletionMessage(const std::string &message)
    {
        CompletionMessage = message;
    }

}; // CTransComplete

//---------------------------------------------------------
// main

#define MESSAGE_PUMP_UNTIL(x)                                                                                          \
    while (!x)                                                                                                         \
    {                                                                                                                  \
        MSG msg;                                                                                                       \
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))                                                            \
        {                                                                                                              \
            TranslateMessage(&msg);                                                                                    \
            DispatchMessage(&msg);                                                                                     \
        }                                                                                                              \
        Sleep(10);                                                                                                     \
    } // while

void main(void)
{
    COPCClient::init();

    printf("input hostname (warning: NOT an IP address, use such as: 'Jhon-PC' or 'localhost' / <ENTER> ):\n"); // See
    // readme
    // to find
    // reason
    char c_string[100] = {0};
    gets_s(c_string);
    if (!strlen(c_string))
    {
        strcpy_s(c_string, sizeof(c_string), "localhost");
    }
    COPCHost *host = COPCClient::makeHost(COPCHost::S2WS(c_string));

    // list servers
    std::vector<CLSID> localClassIdList;
    std::vector<std::wstring> localServerList;
    host->getListOfDAServers(IID_CATID_OPCDAServer20, localServerList, localClassIdList);

    printf("\nserver ID list:\n");
    int server_id = -1;
    for (unsigned i = 0; i < localServerList.size(); ++i)
    {
        printf("%d: '%ws'\n", i, localServerList[i].c_str());
        if (localServerList[i] == L"Matrikon.OPC.Simulation.1")
        {
            server_id = i;
        }
    } // for

    // connect to OPC
    printf("\nselect server ID or <ENTER> for Matrikon:\n");
    gets_s(c_string);
    if (strlen(c_string))
    {
        server_id = atol(c_string);
    }
    std::wstring serverName = localServerList[server_id];
    printf("server name: '%ws'\n", serverName.c_str());
    COPCServer *opcServer = host->connectDAServer(serverName);

    // check server status
    ServerStatus status = {0};
    const char *serverStateMsg[] = {"unknown", "running", "failed", "co-config", "suspended", "test", "comm-fault"};
    do
    {
        Sleep(100);
        opcServer->getStatus(status);
        printf("server state is %s, vendor is '%ws'\r", serverStateMsg[status.dwServerState],
               status.vendorInfo.c_str());
    } while (status.dwServerState != OPC_STATUS_RUNNING);

    // browse server
    std::vector<std::wstring> opcItemNames;
    opcServer->getItemNames(opcItemNames);
    printf("\n\ngot %d names:\n", static_cast<int>(opcItemNames.size()));
    for (unsigned i = 0; i < opcItemNames.size(); ++i)
    {
        printf("%3d: '%ws'\n", i + 1, opcItemNames[i].c_str());
    }

    // make demo group
    unsigned long refreshRate;
    COPCGroup *demoGroup = opcServer->makeGroup(L"DemoGroup", true, 1000, refreshRate, 0.0);

    // make a single item
    std::wstring itemName = opcItemNames[5];
    COPCItem *readWritableItem = demoGroup->addItem(itemName, true);

    // make several items
    std::vector<std::wstring> itemNames;
    std::vector<COPCItem *> itemsCreated;
    std::vector<HRESULT> errors;

    itemNames.push_back(opcItemNames[21]); // 15 -> Bucket Brigade.UInt2
    itemNames.push_back(opcItemNames[22]); // 16 -> Bucket Brigade.UInt4
    if (demoGroup->addItems(itemNames, itemsCreated, errors, true) != 0)
    {
        printf("add items to group FAILED\n");
    }

    // get properties
    std::vector<CPropertyDescription> propDesc;
    readWritableItem->getSupportedProperties(propDesc);
    printf("supported properties for '%ws'\n", readWritableItem->getName().c_str());
    for (unsigned i = 0; i < propDesc.size(); ++i)
    {
        printf("%3d: ID = %u, description = '%ws', type = %d\n", i, propDesc[i].id, propDesc[i].desc.c_str(),
               propDesc[i].type);
    }

    CAutoPtrArray<SPropertyValue> propVals;
    readWritableItem->getProperties(propDesc, propVals);
    for (unsigned i = 0; i < propDesc.size(); ++i)
    {
        if (!propVals[i])
        {
            printf("FAILED to get property %u\n", propDesc[i].id);
        }
        else
        {
            printf("Property %u=", propDesc[i].id);

            switch (propDesc[i].type)
            {
            case VT_R4:
                printf("%f\n", propVals[i]->value.fltVal);
                break;

            case VT_I4:
                printf("%d\n", propVals[i]->value.iVal);
                break;

            case VT_I2:
                printf("%d\n", propVals[i]->value.iVal);
                break;

            case VT_I1: {
                int v = propVals[i]->value.bVal;
                printf("%d\n", v);
            }
            break;

            default:
                printf("\n");
                break;
            } // switch
        }
    } // else

    // sync read of item
    OPCItemData data;
    readWritableItem->readSync(data, OPC_DS_DEVICE);
    printf("-----> '%ws', handle: %u, item sync read quality %u value %d\n", readWritableItem->getName().c_str(),
           readWritableItem->getHandle(), data.wQuality, data.vDataValue.iVal);

    // sync read on demo group
    COPCItemDataMap itemDataMap;
    demoGroup->readSync(itemsCreated, itemDataMap, OPC_DS_DEVICE);
    POSITION pos = itemDataMap.GetStartPosition();
    while (pos)
    {
        OPCHANDLE handle = itemDataMap.GetKeyAt(pos);
        OPCItemData *data = itemDataMap.GetNextValue(pos);
        if (data)
        {
            const COPCItem *item = data->item(); // retrieve original item pointer from item data..
            if (item)
                printf("-----> '%ws', handle: %u, group sync read quality %d value %d\n", item->getName().c_str(),
                       handle, data->wQuality, data->vDataValue.iVal);
        } // if
    }     // while

    // enable async - must be done before any async call will work
    CMyCallback usrCallBack;
    demoGroup->enableAsync(&usrCallBack);

    // async OPC item read
    CTransComplete complete;
    complete.setCompletionMessage("******* async read completion handler has been invoked (OPC item)");
    CTransaction *transaction = readWritableItem->readAsync(&complete);
    MESSAGE_PUMP_UNTIL(transaction->isCompleted())

    const OPCItemData *asyncData = transaction->getItemValue(readWritableItem); // not owned
    if (asyncData && !FAILED(asyncData->Error))
    {
        OPCHANDLE handle = COPCGroup::getOpcHandle(readWritableItem);
        printf("-----> '%ws', handle: %u, item async read quality %d value %d\n", readWritableItem->getName().c_str(),
               handle, asyncData->wQuality, asyncData->vDataValue.iVal);
    } // if
    demoGroup->deleteTransaction(transaction);

    // async read opc items from demo group
    complete.setCompletionMessage("******* async read completion handler has been invoked (OPC group)");
    transaction = demoGroup->readAsync(itemsCreated, &complete);
    MESSAGE_PUMP_UNTIL(transaction->isCompleted())

    for (unsigned i = 0; i < itemsCreated.size(); ++i)
    {
        const OPCItemData *asyncData = transaction->getItemValue(itemsCreated[i]); // not owned
        if (asyncData && !FAILED(asyncData->Error))
        {
            OPCHANDLE handle = COPCGroup::getOpcHandle(itemsCreated[i]);
            printf("-----> '%ws', handle: %u, group async read quality %d value %d\n",
                   itemsCreated[i]->getName().c_str(), handle, asyncData->wQuality, asyncData->vDataValue.iVal);
        } // if
    }     // for
    demoGroup->deleteTransaction(transaction);

    // sync write
    VARIANT var;
    var.vt = VT_I2;
    var.iVal = 99;
    readWritableItem->writeSync(var);

    // async write
    var.vt = VT_I2;
    var.iVal = 32;
    complete.setCompletionMessage("******* async write completion handler has been invoked");
    transaction = readWritableItem->writeAsync(var, &complete);
    MESSAGE_PUMP_UNTIL(transaction->isCompleted())

    asyncData = transaction->getItemValue(readWritableItem); // not owned
    if (asyncData && !FAILED(asyncData->Error))
        printf("async write completed OK\n");
    else
        printf("async write FAILED\n");
    demoGroup->deleteTransaction(transaction);

    // group refresh (async operation) - pass results to CMyCallback as well
    complete.setCompletionMessage("******* refresh completion handler has been invoked");
    transaction = demoGroup->refresh(OPC_DS_DEVICE, &complete);
    MESSAGE_PUMP_UNTIL(transaction->isCompleted())

    // readWritableItem is member of demo group - look for this and use it as a guide to see if operation succeeded.
    asyncData = transaction->getItemValue(readWritableItem);
    if (!FAILED(asyncData->Error))
        printf("refresh completed OK\n");
    else
        printf("refresh FAILED\n");
    demoGroup->deleteTransaction(transaction);

    // just loop - changes to items within demo group are picked up here

    MESSAGE_PUMP_UNTIL(false)

} // main
