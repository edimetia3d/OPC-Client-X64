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

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

/**
 * Holds desciption of a property for an OPC item.
 */
struct OPCDACLIENT_API CPropertyDescription
{
    /// properties identifier
    DWORD id;

    /// server supplied textual description
    std::wstring desc;

    /// data type of the property
    VARTYPE type;

    CPropertyDescription(DWORD i, std::wstring d, VARTYPE t) : id(i), desc(d), type(t)
    {
    } // CPropertyDescription

}; // CPropertyDescription

/**
 * holds value for an OPC item property
 */
struct SPropertyValue
{
    /// the property that this value describes.
    const CPropertyDescription &propDesc;

    /// properties value.
    VARIANT value;

    SPropertyValue(const CPropertyDescription &desc, VARIANT &val);

    virtual ~SPropertyValue()
    {
        VariantClear(&value);
    }

}; // SPropertyValue

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
