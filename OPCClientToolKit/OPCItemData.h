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

#include "OPCClientToolKitDLL.h"
#include "opcda.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

class COPCItem;

/**
 *Wrapper for OPC data. Hides COM memory management.
 */
struct OPCDACLIENT_API OPCItemData
{
    FILETIME ftTimeStamp;
    WORD wQuality;
    VARIANT vDataValue;
    HRESULT Error;
    COPCItem *Item;

    OPCItemData();

    OPCItemData(COPCItem *item, HRESULT error = S_OK);

    OPCItemData(COPCItem *item, VARIANT &value, WORD quality, FILETIME time, HRESULT error = S_OK);

    OPCItemData(const OPCItemData &other);

    ~OPCItemData();

    OPCItemData &operator=(const OPCItemData &other);

    void set(OPCITEMSTATE &itemState);

    void set(HRESULT error = S_OK)
    {
        Error = error;
    }

    void set(VARIANT &value, WORD quality, FILETIME time, HRESULT error = S_OK);

    COPCItem *item()
    {
        return Item;
    }

}; // OPCItemData

/**
 * Can't find an ATL autoptr map - this class provides the functionality I want.
 */
class OPCDACLIENT_API COPCItemDataMap : public CAtlMap<OPCHANDLE, OPCItemData *>
{
  public:
    ~COPCItemDataMap();

    COPCItemDataMap &operator=(const COPCItemDataMap &other);

}; // COPCItemDataMap

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
