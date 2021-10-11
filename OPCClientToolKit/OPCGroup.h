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

#pragma once

#pragma warning(disable : 4251) // can be ignored if deriving from a type in the Standard C++ Library..

#include "OPCClient.h"
#include "OPCClientToolKitDLL.h"
#include "Transaction.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

/**
 * Forward decl.
 */
class COPCItem;

/**
 * used internally to implement the async callback
 */
class CAsyncDataCallback;

/**
 * Client sided abstraction of an OPC group, wrapping the COM interfaces to the group within the OPC server.
 */
class OPCDACLIENT_API COPCGroup
{
  private:
    ATL::CComPtr<IOPCGroupStateMgt> iStateManagement;
    ATL::CComPtr<IOPCSyncIO> iSyncIO;
    ATL::CComPtr<IOPCAsyncIO2> iAsync2IO;
    ATL::CComPtr<IOPCItemMgt> iItemManagement;

    /**
     * used to keep track of the connection point for AsyncDataCallback
     */
    ATL::CComPtr<IConnectionPoint> iAsyncDataCallbackConnectionPoint;

    /**
     * handle given the group by the server
     */
    DWORD GroupHandle;

    /**
     * The server this group belongs to
     */
    COPCServer &OpcServer;

    /**
     * Callback for async data at the group level
     */
    ATL::CComPtr<CAsyncDataCallback> AsyncDataCallBackHandler;

    /**
     * map of OPC items associated with this goup. Not owned (at the moment!)
     */
    COPCItemDataMap GroupItemDataMap;

    CAtlMap<DWORD, CTransaction *> TransactionMap;

    /**
     * Name of the group
     */
    const std::wstring GroupName;

    /**
     * Handle given to callback by server.
     */
    DWORD GroupCallbackHandle;

    /**
     * Users hander to handle async data
     * NOT OWNED.
     */
    IAsyncDataCallback *UserAsyncCBHandler;
    CAsyncDataCallback *_CAsyncDataCallback;

    /**
     * Caller owns returned array
     */
    OPCHANDLE *buildServerHandleList(std::vector<COPCItem *> &items);

  public:
    COPCGroup(const std::wstring &groupName, bool active, unsigned long reqUpdateRate_ms,
              unsigned long &revisedUpdateRate_ms, float deadBand, COPCServer &server);

    virtual ~COPCGroup();

    COPCItem *addItem(std::wstring &name, bool active);

    /**
     * returns the number of failed item creates
     * itemsCreated[x] will be null if could not create and will contain error code in corresponding error entry
     */
    int addItems(std::vector<std::wstring> &names, std::vector<COPCItem *> &items, std::vector<HRESULT> &errors,
                 bool active);

    static OPCHANDLE getOpcHandle(void *ptr)
    {
        return static_cast<OPCHANDLE>(reinterpret_cast<uintptr_t>(ptr));
    }

    static OPCHANDLE addItemData(COPCItemDataMap &opcItemDataMap, COPCItem *item, HRESULT error = S_OK);

    bool lookupOpcItem(OPCHANDLE handle, COPCItem *&item);

    COPCItemDataMap &getItemDataMap()
    {
        return GroupItemDataMap;
    }

    static DWORD getTransactionID(void *ptr)
    {
        return static_cast<DWORD>(reinterpret_cast<uintptr_t>(ptr));
    }

    DWORD addTransaction(CTransaction *transaction);

    bool deleteTransaction(CTransaction *&transaction);

    bool lookupTransaction(DWORD transactionID, CTransaction *&transaction);

    /**
     * enable async I/O
     */
    bool enableAsync(IAsyncDataCallback *handler);

    /**
     * disable async I/O
     */
    bool disableAsync();

    /**
     * set the group state values.
     */
    void setState(DWORD reqUpdateRate_ms, DWORD &returnedUpdateRate_ms, float deadBand, BOOL active);

    /**
     * Read set of OPC items synchronously.
     */
    void readSync(std::vector<COPCItem *> &items, COPCItemDataMap &opcData, OPCDATASOURCE source);

    /**
     * Read a defined group of OPC item asynchronously
     */
    CTransaction *readAsync(std::vector<COPCItem *> &items, ITransactionComplete *transactionCB = nullptr);

    /**
     * Refresh is an async operation.
     * retrieves all active items in the group, which will be stored in the transaction object
     * Transaction object is owned by caller.
     * If group async is disabled then this call will not work
     */
    CTransaction *refresh(OPCDATASOURCE source, ITransactionComplete *transactionCB = nullptr);

    /**
     * Cancel the async group refresh again.
     */
    bool cancelRefresh(CTransaction *&transaction);

    ATL::CComPtr<IOPCSyncIO> &getSyncIOInterface()
    {
        return iSyncIO;
    }

    ATL::CComPtr<IOPCAsyncIO2> &getAsync2IOInterface()
    {
        return iAsync2IO;
    }

    ATL::CComPtr<IOPCItemMgt> &getItemManagementInterface()
    {
        return iItemManagement;
    }

    const std::wstring &getName() const
    {
        return GroupName;
    }

    IAsyncDataCallback *getUsrAsyncHandler()
    {
        return UserAsyncCBHandler;
    }

    /**
     * returns reference to the OPC server that this group belongs to.
     */
    COPCServer &getServer()
    {
        return OpcServer;
    }

}; // COPCGroup

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
