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
#include "OPCProperties.h"
#include "Transaction.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

class COPCGroup;

/**
 * Provides wrapper for operations that typically exist at the group level (e.g. reads) (it is at this level
 * that OPC supports the operation) however, we provide the operation at this level for ease of use.
 */
class OPCDACLIENT_API COPCItem
{
  private:
    OPCHANDLE ServersItemHandle;
    VARTYPE VtCanonicalDataType;
    DWORD DwAccessRights;

    COPCGroup &ItemGroup;

    std::wstring ItemName;

  protected:
    friend class COPCGroup;

    // items may only be created by group.
    COPCItem(std::wstring &itemName, COPCGroup &itemGroup);

  public:
    virtual ~COPCItem();

    /// used to set data for the OPC item AFTER it has been created in the server.
    void setOPCParameters(OPCHANDLE handle, VARTYPE type, DWORD dwAccess);

    bool writeSync(VARIANT &data);

    bool readSync(OPCItemData &data, OPCDATASOURCE source);

    /**
     * returned transaction object is owned
     */
    CTransaction *readAsync(ITransactionComplete *transactionCB = nullptr);

    /**
     * returned transaction object is owned
     */
    CTransaction *writeAsync(VARIANT &data, ITransactionComplete *transactionCB = nullptr);

    DWORD getAccessRights() const
    {
        return DwAccessRights;
    }

    VARTYPE getDataType() const
    {
        return VtCanonicalDataType;
    }

    OPCHANDLE getHandle() const
    {
        return ServersItemHandle;
    }

    COPCGroup &getGroup() const
    {
        return ItemGroup;
    }

    const std::wstring &getName() const
    {
        return ItemName;
    }

    bool getSupportedProperties(std::vector<CPropertyDescription> &desc);

    /**
     * retrieve the OPC item properties for the descriptors passed. Any data previously existing in propsRead will be
     * destroyed.
     */
    bool getProperties(const std::vector<CPropertyDescription> &propsToRead,
                       ATL::CAutoPtrArray<SPropertyValue> &propsRead);

}; // COPCItem

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
