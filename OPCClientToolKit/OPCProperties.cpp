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

#include "OPCProperties.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

SPropertyValue::SPropertyValue(const CPropertyDescription &desc, VARIANT &val) : propDesc(desc)
{
    value.vt = VT_EMPTY;
    HRESULT result = VariantCopy(&value, &val);
    if (FAILED(result))
    {
        throw OPCException(L"SPropertyValue::SPropertyValue: VariantCopy() FAILED", result);
    }

} // SPropertyValue::SPropertyValue

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
