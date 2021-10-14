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

#include "OPCServer.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

COPCServer::COPCServer(ATL::CComPtr<IOPCServer> &opcServerInterface)
{
    iOpcServer = opcServerInterface;

    HRESULT result = opcServerInterface->QueryInterface(IID_IOPCBrowseServerAddressSpace, (void **)&iOpcNameSpace);
    if (FAILED(result))
    {
        throw OPCException(L"COPCServer::COPCServer: FAILED to obtain IID_IOPCBrowseServerAddressSpace interface",
                           result);
    }

    result = opcServerInterface->QueryInterface(IID_IOPCItemProperties, (void **)&iOpcProperties);
    if (FAILED(result))
    {
        throw OPCException(L"COPCServer::COPCServer: FAILED to obtain IID_IOPCItemProperties interface", result);
    }

} // COPCServer::COPCServer

COPCServer::~COPCServer()
{
} // COPCServer::~COPCServer

COPCGroup *COPCServer::makeGroup(const std::wstring &groupName, bool active, unsigned long reqUpdateRate_ms,
                                 unsigned long &revisedUpdateRate_ms, float deadBand)
{
    return new COPCGroup(groupName, active, reqUpdateRate_ms, revisedUpdateRate_ms, deadBand, *this);

} // COPCServer::makeGroup

bool COPCServer::getItemNames(std::vector<std::wstring> &opcItemNames)
{
    if (!iOpcNameSpace)
    {
        return false;
    }

    OPCNAMESPACETYPE nameSpaceType = OPC_NS_FLAT;
    HRESULT result = iOpcNameSpace->QueryOrganization(&nameSpaceType);
    (void)result; // mutes clang complaints..

    USES_CONVERSION;
    WCHAR emptyString[] = {0};
    // result = iOpcNameSpace->ChangeBrowsePosition ( OPC_BROWSE_TO, emptyString );

    ATL::CComPtr<IEnumString> iEnum;
    result = iOpcNameSpace->BrowseOPCItemIDs(OPC_FLAT, emptyString, VT_EMPTY, 0, &iEnum);
    if (FAILED(result))
    {
        return false;
    }

    LPWSTR name = nullptr;
    ULONG nameSize = 0;
    while ((result = iEnum->Next(1, &name, &nameSize)) == S_OK)
    {
        (void)result; // mutes clang complaints..
        LPWSTR fullName = nullptr;
        result = iOpcNameSpace->GetItemID(name, &fullName);
        if (SUCCEEDED(result))
        {
            opcItemNames.push_back(fullName);
            COPCClient::comFree(fullName);
        } // if

        COPCClient::comFree(name);
    } // while

    return true;

} // COPCServer::getItemNames

bool COPCServer::getStatus(ServerStatus &status)
{
    OPCSERVERSTATUS *serverStatus = nullptr;
    HRESULT result = iOpcServer->GetStatus(&serverStatus);
    if (FAILED(result))
    {
        throw OPCException(L"COPCServer::getStatus: FAILED to get status");
    }

    status.ftStartTime = serverStatus->ftStartTime;
    status.ftCurrentTime = serverStatus->ftCurrentTime;
    status.ftLastUpdateTime = serverStatus->ftLastUpdateTime;
    status.dwServerState = serverStatus->dwServerState;
    status.dwGroupCount = serverStatus->dwGroupCount;
    status.dwBandWidth = serverStatus->dwBandWidth;
    status.wMajorVersion = serverStatus->wMajorVersion;
    status.wMinorVersion = serverStatus->wMinorVersion;
    status.wBuildNumber = serverStatus->wBuildNumber;

    if (serverStatus->szVendorInfo)
    {
        status.vendorInfo = serverStatus->szVendorInfo;
        COPCClient::comFree(serverStatus->szVendorInfo);
    } // if

    COPCClient::comFree(serverStatus);
    return true;

} // COPCServer::getStatus

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
