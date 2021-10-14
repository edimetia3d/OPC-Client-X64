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

#include "OPCItem.h"
#include "OPCGroup.h"
#include "OPCServer.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

COPCItem::COPCItem(std::wstring &itemName, COPCGroup &itemGroup) : ItemName(itemName), ItemGroup(itemGroup)
{
} // COPCItem::COPCItem

COPCItem::~COPCItem()
{
    HRESULT *itemResult = nullptr;
    ItemGroup.getItemManagementInterface()->RemoveItems(1, &ServersItemHandle, &itemResult);
    COPCClient::comFree(itemResult);

} // COPCItem::~COPCItem

void COPCItem::setOPCParameters(OPCHANDLE handle, VARTYPE type, DWORD dwAccess)
{
    ServersItemHandle = handle;
    VtCanonicalDataType = type;
    DwAccessRights = dwAccess;

} // COPCItem::setOPCParameters

bool COPCItem::writeSync(VARIANT &data)
{
    HRESULT *itemWriteErrors;
    HRESULT result = ItemGroup.getSyncIOInterface()->Write(1, &ServersItemHandle, &data, &itemWriteErrors);
    if (FAILED(result))
    {
        throw OPCException(L"COPCItem::writeSync: synchronous write FAILED");
    }

    if (FAILED(itemWriteErrors[0]))
    {
        COPCClient::comFree(itemWriteErrors);
        throw OPCException(L"COPCItem::writeSync: synchronous write FAILED");
    } // if

    COPCClient::comFree(itemWriteErrors);
    return true;

} // COPCItem::writeSync

bool COPCItem::readSync(OPCItemData &data, OPCDATASOURCE source)
{
    std::vector<COPCItem *> items;
    OPCHANDLE handle = COPCGroup::getOpcHandle(this);
    items.push_back(this);
    COPCItemDataMap opcData;
    ItemGroup.readSync(items, opcData, source);

    COPCItemDataMap::CPair *pos = opcData.Lookup(handle);
    if (pos)
    {
        OPCItemData *readData = opcData.GetValueAt(pos);
        if (readData && !FAILED(readData->Error))
        {
            data = *readData;
            return true;
        } // if
    }     // if

    throw OPCException(L"COPCItem::readSync: synchronous read FAILED");
    return false;

} // COPCItem::readSync

CTransaction *COPCItem::readAsync(ITransactionComplete *transactionCB)
{
    std::vector<COPCItem *> items;
    items.push_back(this);
    return ItemGroup.readAsync(items, transactionCB);

} // COPCItem::readAsync

CTransaction *COPCItem::writeAsync(VARIANT &data, ITransactionComplete *transactionCB)
{
    DWORD cancelID = 0;
    HRESULT *individualResults = nullptr;
    std::vector<COPCItem *> items;
    items.push_back(this);
    CTransaction *transaction = new CTransaction(items, transactionCB);
    DWORD transactionID = ItemGroup.addTransaction(transaction);

    HRESULT result = ItemGroup.getAsync2IOInterface()->Write(1, &ServersItemHandle, &data, transactionID, &cancelID,
                                                             &individualResults);

    if (FAILED(result))
    {
        ItemGroup.deleteTransaction(transaction);
        throw OPCException(L"COPCItem::writeAsync: async write FAILED");
    } // if

    transaction->setCancelId(cancelID);
    if (FAILED(individualResults[0]))
    {
        transaction->setItemError(this, individualResults[0]);
        transaction->setCompleted(); // if all items return error then no callback will occur. p 104
    }                                // if

    COPCClient::comFree(individualResults);
    return transaction;

} // COPCItem::writeAsync

bool COPCItem::getSupportedProperties(std::vector<CPropertyDescription> &desc)
{
    DWORD nbrProperties = 0;
    DWORD *pPropertyIDs = nullptr;
    LPWSTR *pDescriptions = nullptr;
    VARTYPE *pvtDataTypes = nullptr;

    USES_CONVERSION;
    HRESULT result = ItemGroup.getServer().getPropertiesInterface()->QueryAvailableProperties(
        &ItemName[0], &nbrProperties, &pPropertyIDs, &pDescriptions, &pvtDataTypes);
    if (FAILED(result))
    {
        throw OPCException(L"COPCItem::getSupportedProperties: FAILED to retrieve properties", result);
    }

    for (unsigned i = 0; i < nbrProperties; ++i)
    {
        desc.push_back(CPropertyDescription(pPropertyIDs[i], pDescriptions[i], pvtDataTypes[i]));
    }

    COPCClient::comFree(pPropertyIDs);
    COPCClient::comFree(pDescriptions);
    COPCClient::comFree(pvtDataTypes);
    return true;

} // COPCItem::getSupportedProperties

bool COPCItem::getProperties(const std::vector<CPropertyDescription> &propsToRead,
                             ATL::CAutoPtrArray<SPropertyValue> &propsRead)
{
    DWORD nbrProperties = static_cast<DWORD>(propsToRead.size());
    VARIANT *pValues = nullptr;
    HRESULT *pErrors = nullptr;
    DWORD *pPropertyIDs = new DWORD[nbrProperties];
    for (unsigned i = 0; i < nbrProperties; ++i)
    {
        pPropertyIDs[i] = propsToRead[i].id;
    }

    propsRead.RemoveAll();
    propsRead.SetCount(nbrProperties);

    USES_CONVERSION;
    HRESULT result = ItemGroup.getServer().getPropertiesInterface()->GetItemProperties(
        &ItemName[0], nbrProperties, pPropertyIDs, &pValues, &pErrors);
    delete[] pPropertyIDs;
    if (FAILED(result))
    {
        throw OPCException(L"COPCItem::getProperties: FAILED to retrieve property values", result);
    }

    for (unsigned i = 0; i < nbrProperties; ++i)
    {
        CAutoPtr<SPropertyValue> value;
        if (!FAILED(pErrors[i]))
        {
            value.Attach(new SPropertyValue(propsToRead[i], pValues[i]));
        }
        propsRead[i] = value;
    } // for

    COPCClient::comFree(pErrors);
    COPCClient::comFreeVariant(pValues, nbrProperties);
    return true;

} // COPCItem::getProperties

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
