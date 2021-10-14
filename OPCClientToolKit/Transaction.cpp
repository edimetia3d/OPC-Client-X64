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

#include "Transaction.h"
#include "OPCGroup.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

CTransaction::CTransaction(ITransactionComplete *completeCB)
    : Completed(false), CancelID(0xffffffff), CompleteCallBack(completeCB)
{
} // CTransaction::CTransaction

CTransaction::CTransaction(std::vector<COPCItem *> &items, ITransactionComplete *completeCB)
    : Completed(false), CancelID(0xffffffff), CompleteCallBack(completeCB)
{
    for (unsigned i = 0; i < items.size(); ++i)
    {
        COPCGroup::addItemData(ItemDataMap, items[i]);
    }

} // CTransaction::CTransaction

CTransaction::CTransaction(COPCItemDataMap &itemDataMap, ITransactionComplete *completeCB)
    : Completed(false), CancelID(0xffffffff), CompleteCallBack(completeCB)
{
    ItemDataMap = itemDataMap;

} // CTransaction::CTransaction

void CTransaction::setItemError(COPCItem *item, HRESULT error)
{
    OPCHANDLE handle = COPCGroup::getOpcHandle(item);
    COPCItemDataMap::CPair *pair = ItemDataMap.Lookup(handle);
    if (!pair)
    {
        throw OPCException(L"CTransaction::setItemError: FAILED to find OPC item in OPC data map");
    }

    if (!pair->m_value)
    {
        ItemDataMap.SetValueAt(pair, new OPCItemData(item, error));
    }
    else
    {
        pair->m_value->set(error);
    } // just set error of existing item data..

} // CTransaction::setItemError

void CTransaction::setItemValue(COPCItem *item, FILETIME time, WORD quality, VARIANT &value, HRESULT error)
{
    OPCHANDLE handle = COPCGroup::getOpcHandle(item);
    COPCItemDataMap::CPair *pair = ItemDataMap.Lookup(handle);
    if (!pair)
    {
        throw OPCException(L"CTransaction::setItemValue: FAILED to find OPC item in OPC data map");
    }

    if (!pair->m_value)
    {
        ItemDataMap.SetValueAt(pair, new OPCItemData(item, value, quality, time, error)); // make new item data..
    }
    else
    {
        pair->m_value->set(value, quality, time, error);
    } // just set values of existing item data..

} // CTransaction::setItemValue

const OPCItemData *CTransaction::getItemValue(COPCItem *item) const
{
    OPCHANDLE handle = COPCGroup::getOpcHandle(item);
    const COPCItemDataMap::CPair *pair = ItemDataMap.Lookup(handle);
    if (!pair)
    {
        throw OPCException(L"CTransaction::getItemValue: FAILED to find OPC item in transaction OPC data map");
    }

    return pair->m_value;

} // CTransaction::getItemValue

void CTransaction::setCompleted()
{
    Completed = true;
    if (CompleteCallBack)
    {
        CompleteCallBack->complete(*this);
    }

} // CTransaction::setCompleted

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
