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

#include "OPCGroup.h"
#include "OPCItem.h"
#include "OPCServer.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

/**
 * Handles OPC (DCOM) callbacks at the group level. It deals with the receipt of data from asynchronous operations.
 * This is a fake COM object.
 */
class CAsyncDataCallback : public IOPCDataCallback
{
  private:
    DWORD ReferenceCount;

    /**
     * group this is a callback for
     */
    COPCGroup &CallbacksGroup;

  public:
    CAsyncDataCallback(COPCGroup &group) : CallbacksGroup(group), ReferenceCount(0)
    {
    } // CAsyncDataCallback

    virtual ~CAsyncDataCallback()
    {
    } // ~CAsyncDataCallback

    /**
     * Functions associated with IUNKNOWN
     */
    STDMETHODIMP QueryInterface(REFIID iid, LPVOID *ppInterface)
    {
        if (!ppInterface)
        {
            return E_INVALIDARG;
        }

        if (iid == IID_IUnknown)
        {
            *ppInterface = (IUnknown *)this;
        }
        else if (iid == IID_IOPCDataCallback)
        {
            *ppInterface = (IOPCDataCallback *)this;
        }
        else
        {
            *ppInterface = nullptr;
            return E_NOINTERFACE;
        } // else

        AddRef();
        return S_OK;

    } // QueryInterface

    STDMETHODIMP_(ULONG)

    AddRef()
    {
        return ++ReferenceCount;

    } // AddRef

    STDMETHODIMP_(ULONG)

    Release()
    {
        DWORD count = ReferenceCount ? --ReferenceCount : 0;

        if (!count)
        {
            delete this;
        }

        return count;

    } // Release

    /**
     * Functions associated with IOPCDataCallback, pls see also:
     * https://lhcb-online.web.cern.ch/ecs/opcevaluation/htmlspef/DA_Custom_IF/Custom_Client.html
     */
    STDMETHODIMP OnDataChange(DWORD transactionID, OPCHANDLE groupHandle, HRESULT masterQuality, HRESULT masterError,
                              DWORD count, OPCHANDLE *clientHandles, VARIANT *values, WORD *quality, FILETIME *time,
                              HRESULT *errors)
    {
        (void)groupHandle;
        (void)masterQuality;
        (void)masterError;

        IAsyncDataCallback *usrHandler = CallbacksGroup.getUsrAsyncHandler();

        if (transactionID)
        {
            // it is a result of a refresh (see p106 of spec)
            CTransaction *transaction = nullptr;
            if (CallbacksGroup.lookupTransaction(transactionID, transaction))
            {
                updateOPCData(transaction->getItemDataMap(), count, clientHandles, values, quality, time, errors);
                transaction->setCompleted();
                return S_OK;
            } // if

            else
            {
                throw OPCException(L"CAsyncDataCallback::OnDataChange: FAILED to lookup transaction ID in map");
            }
        } // if

        if (usrHandler)
        {
            COPCItemDataMap dataChanges;
            updateOPCData(dataChanges, count, clientHandles, values, quality, time, errors);
            usrHandler->OnDataChange(CallbacksGroup, dataChanges);
        } // if

        return S_OK;

    } // OnDataChange

    STDMETHODIMP OnReadComplete(DWORD transactionID, OPCHANDLE groupHandle, HRESULT masterQuality, HRESULT masterError,
                                DWORD count, OPCHANDLE *clientHandles, VARIANT *values, WORD *quality, FILETIME *time,
                                HRESULT *errors)
    {
        (void)groupHandle;
        (void)masterQuality;
        (void)masterError;

        // TODO this is bad  - server could corrupt address - need to use look up table
        CTransaction *transaction = nullptr;
        if (CallbacksGroup.lookupTransaction(transactionID, transaction))
        {
            updateOPCData(transaction->getItemDataMap(), count, clientHandles, values, quality, time, errors);
            transaction->setCompleted();
            return S_OK;
        } // if
        return S_FALSE;

    } // OnReadComplete

    STDMETHODIMP OnWriteComplete(DWORD transactionID, OPCHANDLE groupHandle, HRESULT masterError, DWORD count,
                                 OPCHANDLE *clientHandles, HRESULT *errors)
    {
        (void)groupHandle;
        (void)masterError;

        // TODO this is bad  - server could corrupt address - need to use look up table
        CTransaction *transaction = nullptr;
        if (CallbacksGroup.lookupTransaction(transactionID, transaction))
        {
            // see page 145 - number of items returned may be less than sent
            for (unsigned i = 0; i < count; ++i)
            {
                OPCItemData *data = nullptr;
                if (transaction->getItemDataMap().Lookup(clientHandles[i], data) && data)
                { // look up adjoining OPC data in map..
                    transaction->setItemError(data->item(), errors[i]);
                } // this records error state - may be good
            }     // for
            transaction->setCompleted();
        } // if
        return S_OK;

    } // OnWriteComplete

    STDMETHODIMP OnCancelComplete(DWORD transactionID, OPCHANDLE groupHandle)
    {
        printf("OnCancelComplete: transactionID=%ld groupHandle=%ld\n", transactionID, groupHandle);
        return S_OK;

    } // OnCancelComplete

    /**
     * make OPC item
     */
    static OPCItemData *makeOPCDataItem(VARIANT &value, WORD quality, FILETIME &time, HRESULT error,
                                        COPCItem *item = nullptr)
    {
        OPCItemData *data = nullptr;
        if (FAILED(error))
        {
            data = new OPCItemData(item, error);
        }
        else
        {
            data = new OPCItemData(item, value, quality, time, error);
        }
        return data;

    } // makeOPCDataItem

    /**
     * Enter the OPC items data that resulted from an operation
     */
    void updateOPCData(COPCItemDataMap &itemDataMap, DWORD count, OPCHANDLE *clientHandles, VARIANT *values,
                       WORD *quality, FILETIME *time, HRESULT *errors)
    {
        // see page 136 - returned arrays may be out of order
        for (unsigned i = 0; i < count; ++i)
        {
            COPCItemDataMap::CPair *pair = itemDataMap.Lookup(clientHandles[i]);

            if (!pair || !pair->m_value)
            {
                COPCItem *item = nullptr;
                CallbacksGroup.lookupOpcItem(clientHandles[i], item);
                OPCItemData *data = makeOPCDataItem(values[i], quality[i], time[i], errors[i],
                                                    item); // make data item..
                if (!pair)
                {
                    itemDataMap.SetAt(clientHandles[i],
                                      data); // create new item in OPC data map, but without valid OPC item pointer !
                }
                else
                {
                    itemDataMap.SetValueAt(pair, data);
                } // update existing item in OPC data map with new OPC data item..
            }     // if

            else
            {
                pair->m_value->set(values[i], quality[i], time[i], errors[i]);
            } // just set values of existing item..
        }     // for

    } // updateOPCData

}; // CAsyncDataCallback

COPCGroup::COPCGroup(const std::wstring &groupName, bool active, unsigned long reqUpdateRate_ms,
                     unsigned long &revisedUpdateRate_ms, float deadBand, COPCServer &server)
    : GroupName(groupName), OpcServer(server)
{
    HRESULT result = OpcServer.getServerInterface()->AddGroup(groupName.c_str(), active, reqUpdateRate_ms, 0, 0,
                                                              &deadBand, 0, &GroupHandle, &revisedUpdateRate_ms,
                                                              IID_IOPCGroupStateMgt, (LPUNKNOWN *)&iStateManagement);
    if (FAILED(result))
    {
        throw OPCException(L"COPCGroup::COPCGroup: FAILED to Add group");
    }

    result = iStateManagement->QueryInterface(IID_IOPCSyncIO, (void **)&iSyncIO);
    if (FAILED(result))
    {
        throw OPCException(L"COPCGroup::COPCGroup: FAILED to get IID_IOPCSyncIO");
    }

    result = iStateManagement->QueryInterface(IID_IOPCAsyncIO2, (void **)&iAsync2IO);
    if (FAILED(result))
    {
        throw OPCException(L"COPCGroup::COPCGroup: FAILED to get IID_IOPCAsyncIO2");
    }

    result = iStateManagement->QueryInterface(IID_IOPCItemMgt, (void **)&iItemManagement);
    if (FAILED(result))
    {
        throw OPCException(L"COPCGroup::COPCGroup: FAILED to get IID_IOPCItemMgt");
    }

} // COPCGroup::COPCGroup

COPCGroup::~COPCGroup()
{
    OpcServer.getServerInterface()->RemoveGroup(GroupHandle, false);

} // COPCGroup::~COPCGroup

OPCHANDLE *COPCGroup::buildServerHandleList(std::vector<COPCItem *> &items)
{
    OPCHANDLE *handles = new OPCHANDLE[items.size()];
    for (unsigned i = 0; i < items.size(); ++i)
    {
        if (!items[i])
        {
            delete[] handles;
            throw OPCException(L"COPCGroup::buildServerHandleList: item is nullptr");
        } // if

        handles[i] = items[i]->getHandle();
    } // for

    return handles;

} // COPCGroup::buildServerHandleList

void COPCGroup::readSync(std::vector<COPCItem *> &items, COPCItemDataMap &itemDataMap, OPCDATASOURCE source)
{
    OPCHANDLE *handles = buildServerHandleList(items);
    HRESULT *results = nullptr;
    OPCITEMSTATE *states = nullptr;
    DWORD nbrItems = static_cast<DWORD>(items.size());

    HRESULT result = iSyncIO->Read(source, nbrItems, handles, &states, &results);
    if (FAILED(result))
    {
        delete[] handles;
        throw OPCException(L"COPCGroup::readSync: sync read FAILED");
    } // if

    for (unsigned i = 0; i < nbrItems; ++i)
    {
        OPCHANDLE handle = states[i].hClient;
        OPCItemData *data = CAsyncDataCallback::makeOPCDataItem(states[i].vDataValue, states[i].wQuality,
                                                                states[i].ftTimeStamp, results[i], items[i]);
        COPCItemDataMap::CPair *pair = itemDataMap.Lookup(handle);
        if (!pair)
        {
            itemDataMap.SetAt(handle, data);
        }
        else
        {
            itemDataMap.SetValueAt(pair, data);
        }
        VariantClear(&(states[i].vDataValue));
    } // for

    delete[] handles;
    COPCClient::comFree(results);
    COPCClient::comFree(states);

} // COPCGroup::readSync

CTransaction *COPCGroup::readAsync(std::vector<COPCItem *> &items, ITransactionComplete *transactionCB)
{
    DWORD cancelID = 0;
    HRESULT *results = nullptr;
    OPCHANDLE *handles = buildServerHandleList(items);
    DWORD nbrItems = static_cast<DWORD>(items.size());
    CTransaction *transaction = new CTransaction(items, transactionCB);
    DWORD transactionID = addTransaction(transaction);

    HRESULT result = iAsync2IO->Read(nbrItems, handles, transactionID, &cancelID, &results);
    delete[] handles;
    if (FAILED(result))
    {
        deleteTransaction(transaction);
        throw OPCException(L"COPCGroup::readAsync: async read FAILED");
    } // if

    transaction->setCancelId(cancelID);
    unsigned failCount = 0;
    for (unsigned i = 0; i < nbrItems; ++i)
    {
        if (FAILED(results[i]))
        {
            transaction->setItemError(items[i], results[i]);
            ++failCount;
        }
    } // if

    if (failCount == items.size())
        transaction->setCompleted(); // if all items return error then no callback will occur. p 101

    COPCClient::comFree(results);
    return transaction;

} // COPCGroup::readAsync

CTransaction *COPCGroup::refresh(OPCDATASOURCE source, ITransactionComplete *transactionCB)
{
    DWORD cancelID = 0;
    CTransaction *transaction = new CTransaction(GroupItemDataMap, transactionCB);
    DWORD transactionID = addTransaction(transaction);

    HRESULT result = iAsync2IO->Refresh2(source, transactionID, &cancelID);
    if (FAILED(result))
    {
        deleteTransaction(transaction);
        throw OPCException(L"COPCGroup::refresh: refresh FAILED");
    } // if

    transaction->setCancelId(cancelID);
    return transaction;

} // COPCGroup::refresh

bool COPCGroup::cancelRefresh(CTransaction *&transaction)
{
    if (!transaction)
    {
        return false;
    }

    DWORD cancelID = transaction->getCancelId();
    if (!deleteTransaction(transaction))
    {
        return false;
    }

    HRESULT result = iAsync2IO->Cancel2(cancelID);
    if (FAILED(result) && (result != E_FAIL))
    { // 0x80004005L "Unspecified error", just return if this happened..
        throw OPCException(L"COPCGroup::cancelRefresh: cancel async refresh FAILED");
    }

    return !FAILED(result);

} // COPCGroup::cancelRefresh

COPCItem *COPCGroup::addItem(std::wstring &name, bool active)
{
    std::vector<COPCItem *> items;
    std::vector<HRESULT> errors;
    std::vector<std::wstring> names;
    names.push_back(name);
    if (addItems(names, items, errors, active) != 0)
    {
        throw OPCException(L"COPCGroup::addItem: FAILED to add item");
    }

    return items[0];

} // COPCGroup::addItem

int COPCGroup::addItems(std::vector<std::wstring> &names, std::vector<COPCItem *> &items, std::vector<HRESULT> &errors,
                        bool active)
{
    items.resize(names.size());
    errors.resize(names.size());
    OPCITEMDEF *itemDef = new OPCITEMDEF[names.size()];
    std::vector<CW2W *> nameVector;
    for (unsigned i = 0; i < names.size(); ++i)
    {
        items[i] = new COPCItem(names[i], *this);
        USES_CONVERSION;
        nameVector.push_back(new CW2W(names[i].c_str()));
        itemDef[i].szItemID = **(nameVector.end() - 1);
        itemDef[i].szAccessPath = nullptr; // wide name;
        itemDef[i].bActive = active;
        itemDef[i].hClient = getOpcHandle(items[i]);
        itemDef[i].dwBlobSize = 0;
        itemDef[i].pBlob = nullptr;
        itemDef[i].vtRequestedDataType = VT_EMPTY;
    } // for

    HRESULT *results = nullptr;
    OPCITEMRESULT *details = nullptr;
    DWORD nbrItems = static_cast<DWORD>(names.size());

    HRESULT result = getItemManagementInterface()->AddItems(nbrItems, itemDef, &details, &results);
    delete[] itemDef;
    for (unsigned i = 0; i < names.size(); ++i)
    {
        delete nameVector[i];
    }

    if (FAILED(result))
    {
        throw OPCException(L"COPCGroup::addItems: FAILED to add items");
    }

    int errorCount = 0;
    for (unsigned i = 0; i < nbrItems; ++i)
    {
        if (details[i].pBlob)
        {
            COPCClient::comFree(details[0].pBlob);
        }

        if (FAILED(results[i]))
        {
            delete items[i];
            items[i] = nullptr;
            errors[i] = results[i];
            ++errorCount;
        } // if
        else
        {
            addItemData(GroupItemDataMap, items[i]); // add new item to group's item data map..
            items[i]->setOPCParameters(details[i].hServer, details[i].vtCanonicalDataType, details[i].dwAccessRights);
            errors[i] = ERROR_SUCCESS;
        } // else
    }     // for

    COPCClient::comFree(details);
    COPCClient::comFree(results);
    return errorCount;

} // COPCGroup::addItems

OPCHANDLE COPCGroup::addItemData(COPCItemDataMap &opcItemDataMap, COPCItem *item, HRESULT error)
{
    OPCHANDLE handle = getOpcHandle(item);
    opcItemDataMap.SetAt(handle, new OPCItemData(item, error));
    return handle;

} // COPCGroup::addItemData

bool COPCGroup::lookupOpcItem(OPCHANDLE handle, COPCItem *&item)
{
    item = nullptr;
    bool result = false;
    OPCItemData *itemData = nullptr;
    if ((result = GroupItemDataMap.Lookup(handle, itemData)) && itemData)
    {
        item = itemData->item();
    }
    return result;

} // COPCGroup::lookupOpcItem

DWORD COPCGroup::addTransaction(CTransaction *transaction)
{
    DWORD transactionID = getTransactionID(transaction);
    if (transactionID)
    {
        TransactionMap.SetAt(transactionID, transaction);
    }

    return transactionID;

} // COPCGroup::addTransaction

bool COPCGroup::deleteTransaction(CTransaction *&transaction)
{
    bool result = false;
    DWORD transactionID = getTransactionID(transaction);
    result = TransactionMap.RemoveKey(transactionID);
    delete transaction;
    transaction = nullptr;
    return result;

} // COPCGroup::deleteTransaction

bool COPCGroup::lookupTransaction(DWORD transactionID, CTransaction *&transaction)
{
    return TransactionMap.Lookup(transactionID, transaction);

} // COPCGroup::lookupTransaction

bool COPCGroup::enableAsync(IAsyncDataCallback *handler)
{
    if (AsyncDataCallBackHandler)
    {
        throw OPCException(L"COPCGroup::enableAsync: async already enabled");
    }

    ATL::CComPtr<IConnectionPointContainer> iConnectionPointContainer = 0;
    HRESULT result =
        iStateManagement->QueryInterface(IID_IConnectionPointContainer, (void **)&iConnectionPointContainer);
    if (FAILED(result))
    {
        throw OPCException(L"COPCGroup::enableAsync: could not get IID_IConnectionPointContainer");
    }

    result = iConnectionPointContainer->FindConnectionPoint(IID_IOPCDataCallback, &iAsyncDataCallbackConnectionPoint);
    if (FAILED(result))
    {
        throw OPCException(L"COPCGroup::enableAsync: could not get IID_IOPCDataCallback");
    }

    AsyncDataCallBackHandler = new CAsyncDataCallback(*this);
    result = iAsyncDataCallbackConnectionPoint->Advise(AsyncDataCallBackHandler, &GroupCallbackHandle);
    if (FAILED(result))
    {
        iAsyncDataCallbackConnectionPoint = nullptr;
        AsyncDataCallBackHandler = nullptr;
        throw OPCException(L"COPCGroup::enableAsync: FAILED to set DataCallbackConnectionPoint");
    } // if

    UserAsyncCBHandler = handler;
    return true;

} // COPCGroup::enableAsync

void COPCGroup::setState(DWORD reqUpdateRate_ms, DWORD &returnedUpdateRate_ms, float deadBand, BOOL active)
{
    HRESULT result = iStateManagement->SetState(&reqUpdateRate_ms, &returnedUpdateRate_ms, &active, 0, &deadBand, 0, 0);
    if (FAILED(result))
        throw OPCException(L"COPCGroup::setState: FAILED to set group state");

} // COPCGroup::setState

bool COPCGroup::disableAsync()
{
    if (!AsyncDataCallBackHandler)
    {
        throw OPCException(L"COPCGroup::disableAsync: async is not enabled");
    }

    iAsyncDataCallbackConnectionPoint->Unadvise(GroupCallbackHandle);
    iAsyncDataCallbackConnectionPoint = nullptr;
    AsyncDataCallBackHandler = nullptr; // WE DO NOT DELETE callbackHandler, let the COM ref counting take care of that
    UserAsyncCBHandler = nullptr;
    return true;

} // COPCGroup::disableAsync

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
