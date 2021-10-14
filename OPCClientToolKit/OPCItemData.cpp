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

#include <atlcoll.h>
#include <atlstr.h>

#include "OPCClient.h"
#include "OPCItemData.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

OPCItemData::OPCItemData() : Item(nullptr), wQuality(0), Error(0)
{
    ftTimeStamp.dwLowDateTime = 0;
    ftTimeStamp.dwHighDateTime = 0;
    VariantInit(&vDataValue); // initialize variant..

} // OPCItemData::OPCItemData

OPCItemData::OPCItemData(COPCItem *item, HRESULT error) : Item(item), wQuality(0), Error(error)
{
    ftTimeStamp.dwLowDateTime = 0;
    ftTimeStamp.dwHighDateTime = 0;
    VariantInit(&vDataValue); // initialize variant..

} // OPCItemData::OPCItemData

OPCItemData::OPCItemData(COPCItem *item, VARIANT &value, WORD quality, FILETIME time, HRESULT error)
    : Item(item), wQuality(quality), ftTimeStamp(time), Error(error)
{
    VariantInit(&vDataValue); // initialize variant..
    HRESULT result = VariantCopy(&vDataValue, &value);
    if (FAILED(result))
    {
        throw OPCException(L"OPCItemData::OPCItemData: VarCopy FAILED");
    }

} // OPCItemData::OPCItemData

OPCItemData::OPCItemData(const OPCItemData &other)
{
    VariantInit(&vDataValue); // initialize variant..
    HRESULT result = VariantCopy(&vDataValue, &other.vDataValue);
    if (FAILED(result))
    {
        throw OPCException(L"OPCItemData::OPCItemData: VariantCopy() FAILED");
    }

    Item = other.Item;
    wQuality = other.wQuality;
    ftTimeStamp = other.ftTimeStamp;
    Error = other.Error;

} // OPCItemData::OPCItemData

OPCItemData::~OPCItemData()
{
    VariantClear(&vDataValue);

} // OPCItemData::~OPCItemData

OPCItemData &OPCItemData::operator=(const OPCItemData &other)
{
    HRESULT result = VariantCopy(&vDataValue, &other.vDataValue);
    if (FAILED(result))
    {
        throw OPCException(L"OPCItemData::operator=: VariantCopy() FAILED");
    }

    Item = other.Item;
    wQuality = other.wQuality;
    ftTimeStamp = other.ftTimeStamp;
    Error = other.Error;
    return *this;

} // OPCItemData::operator=

void OPCItemData::set(OPCITEMSTATE &itemState)
{
    HRESULT result = VariantCopy(&vDataValue, &itemState.vDataValue);
    if (FAILED(result))
    {
        throw OPCException(L"OPCItemData::set: VariantCopy() FAILED");
    }

    wQuality = itemState.wQuality;
    ftTimeStamp = itemState.ftTimeStamp;

} // OPCItemData::set

void OPCItemData::set(VARIANT &value, WORD quality, FILETIME time, HRESULT error)
{
    HRESULT result = VariantCopy(&vDataValue, &value);
    if (FAILED(result))
        throw OPCException(L"OPCItemData::set: VariantCopy() FAILED");

    wQuality = quality;
    ftTimeStamp = time;
    Error = error;

} // OPCItemData::set

COPCItemDataMap::~COPCItemDataMap()
{
    POSITION pos = GetStartPosition();
    while (pos)
    {
        OPCItemData *data = GetNextValue(pos);
        if (data)
        {
            delete data;
        }
    } // while

    RemoveAll();

} // COPCItemDataMap::~COPCItemDataMap

COPCItemDataMap &COPCItemDataMap::operator=(const COPCItemDataMap &other)
{
    POSITION pos = other.GetStartPosition();

    while (pos)
    {
        OPCHANDLE handle = other.GetKeyAt(pos);
        OPCItemData *itemData = new OPCItemData(*other.GetNextValue(pos));
        SetAt(handle, itemData);
    } // while

    return *this;

} // COPCItemDataMap::operator

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
