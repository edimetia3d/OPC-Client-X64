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

#include <vector>

#include "OPCClient.h"
#include "OPCClientToolKitDLL.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

class CTransaction;

/**
 * Interface which provides means by which the client can be notified when an asynchronous operation
 * is completed. The implementer must implement this interface overriding the complete method to provide
 * the desired behaviour. An instance of the implementation can be passed as a parameter to an asynchronous
 * operation and used as a means of completion notification.
 */
class ITransactionComplete
{
  public:
    virtual void complete(CTransaction &transaction) = 0;

}; // ITransactionComplete

/**
 * Used to indicate completion of an asynchronous operation.
 * Will contain the results of that operation.
 */
class CTransaction
{
  private:
    /**
     * Optional transation complete callback - not owned
     */
    ITransactionComplete *CompleteCallBack;

    // true when the transaction has completed
    bool Completed;

    DWORD CancelID;

    /**
     * keyed on OPCitem address (not owned)
     * OPCitem data is owned by the transaction - may be nullptr
     */
    COPCItemDataMap ItemDataMap;

  public:
    CTransaction(ITransactionComplete *completeCB = nullptr);

    /**
     * Used where the transaction completion will result in data being received.
     */
    CTransaction(std::vector<COPCItem *> &items, ITransactionComplete *completeCB);

    CTransaction(COPCItemDataMap &itemDataMap, ITransactionComplete *completeCB);

    COPCItemDataMap &getItemDataMap()
    {
        return ItemDataMap;
    }

    void setItemError(COPCItem *item, HRESULT error = S_OK);

    void setItemValue(COPCItem *item, FILETIME time, WORD quality, VARIANT &value, HRESULT error = S_OK);

    /**
     * return Value stored for a given opc item.
     */
    const OPCItemData *getItemValue(COPCItem *item) const;

    /**
     * trigger completion of the transaction.
     */
    void setCompleted();

    bool isCompleted() const
    {
        return Completed;
    }

    void setCancelId(DWORD cancelID)
    {
        CancelID = cancelID;
    }

    DWORD getCancelId() const
    {
        return CancelID;
    }

}; // CTransaction

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
