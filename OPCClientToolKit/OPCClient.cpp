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

#include <process.h>

#include "OPCClient.h"
#include "OPCHost.h"
#include "OPCServer.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

const GUID COPCClient::CATID_OPCDAv10 = IID_CATID_OPCDAServer10; // {63D5F430-CFE4-11d1-B2C8-0060083BA1FB}

const GUID COPCClient::CATID_OPCDAv20 = IID_CATID_OPCDAServer20; // {63D5F432-CFE4-11d1-B2C8-0060083BA1FB}

ATL::CComPtr<IMalloc> COPCClient::iMalloc;

int COPCClient::ReleaseCount = 0;

bool COPCClient::init(OPCOLEInitMode mode)
{
    HRESULT result = -1;
    if (mode == APARTMENTTHREADED)
    {
        result = CoInitialize(nullptr);
    }
    if (mode == MULTITHREADED)
    {
        result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    }

    if (FAILED(result))
    {
        throw OPCException(L"COPCClient::init: CoInitialize failed");
    }

    CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr,
                         EOAC_NONE, nullptr);

    if (!iMalloc)
    {
        result = CoGetMalloc(MEMCTX_TASK, &iMalloc);
        if (FAILED(result))
        {
            throw OPCException(L"COPCClient::init: CoGetMalloc FAILED");
        }
    } // if

    ++ReleaseCount;
    return true;

} // COPCClient::init

void COPCClient::stop()
{
    if (--ReleaseCount <= 0)
    {
        iMalloc.Release();
    }

    CoUninitialize();

} // COPCClient::stop

void COPCClient::comFree(void *memory)
{
    iMalloc->Free(memory);

} // COPCClient::comFree

void COPCClient::comFreeVariant(VARIANT *memory, unsigned size)
{
    for (unsigned i = 0; i < size; ++i)
        VariantClear(&(memory[i]));

    iMalloc->Free(memory);

} // COPCClient::comFreeVariant

COPCHost *COPCClient::makeHost(const std::wstring &hostName)
{
    if (!hostName.size() || (hostName == L"localhost") || (hostName == L"127.0.0.1"))
    {
        return new CLocalHost();
    }

    return new CRemoteHost(hostName);

} // COPCClient::makeHost

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
