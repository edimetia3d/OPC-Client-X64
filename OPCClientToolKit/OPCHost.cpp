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

#include <iostream>

#include "OPCHost.h"
#include "OPCServer.h"
#include "OpcEnum.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COPCHost::COPCHost()
{
} // COPCHost::COPCHost

COPCHost::~COPCHost()
{
} // COPCHost::~COPCHost

void COPCHost::makeCOMObjectEx(std::wstring hostName, tagCLSCTX serverLocation, const IID requestedClass,
                               const IID requestedInterface, void **interfacePtr)
{
    COAUTHINFO authn = {0};
    // Set up the NULL security information
    authn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT; // RPC_C_AUTHN_LEVEL_NONE
    authn.dwAuthnSvc = RPC_C_AUTHN_WINNT;
    authn.dwAuthzSvc = RPC_C_AUTHZ_NONE;
    authn.dwCapabilities = EOAC_NONE;
    authn.dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
    authn.pAuthIdentityData = nullptr;
    authn.pwszServerPrincName = nullptr;

    COSERVERINFO requestedServerInfo = {0};
    CW2W wstr(hostName.c_str());
    requestedServerInfo.pwszName = wstr;
    requestedServerInfo.pAuthInfo = &authn;
    printf("server name: '%ws'\n", requestedServerInfo.pwszName);

    MULTI_QI reqInterface;
    reqInterface.pIID = &requestedInterface;
    reqInterface.pItf = nullptr;
    reqInterface.hr = S_OK;

    HRESULT result =
        CoCreateInstanceEx(requestedClass, nullptr, serverLocation, &requestedServerInfo, 1, &reqInterface);
    if (FAILED(result))
    {
        printf("create instance error %x\n", result);
        throw OPCException(L"COPCHost::makeCOMObjectEx: FAILED to get remote interface");
    } // if

    *interfacePtr = reqInterface.pItf; // avoid ref counter getting incremented again

} // COPCHost::makeCOMObjectEx

void COPCHost::getListOfDAServersEx(std::wstring hostName, tagCLSCTX serverLocation, CATID cid,
                                    std::vector<std::wstring> &listOfProgIDs, std::vector<CLSID> &listOfClassIDs)
{
    CATID implist[1] = {cid};
    ATL::CComPtr<IEnumCLSID> iEnum;
    ATL::CComPtr<IOPCServerList> iCatInfo;
    makeCOMObjectEx(hostName, serverLocation, CLSID_OpcServerList, IID_IOPCServerList, (void **)&iCatInfo);

    HRESULT result = iCatInfo->EnumClassesOfCategories(1, implist, 0, nullptr, &iEnum);
    if (FAILED(result))
    {
        throw OPCException(L"COPCHost::getListOfDAServersEx: FAILED to get enum for categeories");
    }

    GUID classID = {0, 0, 0, {0}};
    ULONG actual = 0;
    while ((result = iEnum->Next(1, &classID, &actual)) == S_OK)
    {
        (void)result; // mutes clang complaints..
        LPOLESTR progID = nullptr;
        LPOLESTR userType = nullptr;
        result = iCatInfo->GetClassDetails(classID, &progID, &userType); // ProgIDFromCLSID ( classID, &progID )

        if (FAILED(result))
        {
            throw OPCException(L"COPCHost::getListOfDAServersEx: FAILED to get prog ID from class ID");
        }
        else
        {
            listOfClassIDs.push_back(classID);
            listOfProgIDs.push_back(progID);

            LPOLESTR classIDStr = nullptr;
            result = StringFromCLSID(classID, &classIDStr);
            if (FAILED(result))
            {
                throw OPCException(L"COPCHost::getListOfDAServersEx: FAILED to get class ID string from class ID");
            }

            printf("prog ID: '%ws' - class ID: %ws\n", progID, classIDStr);

            COPCClient::comFree(progID);
            COPCClient::comFree(userType);
        } // else
    }     // while

} // COPCHost::getListOfDAServersEx

CRemoteHost::CRemoteHost(const std::wstring &hostName) : HostName(hostName)
{
} // CRemoteHost::CRemoteHost

COPCServer *CRemoteHost::connectDAServer(const std::wstring &serverProgIDOrClsID)
{
    CLSID clsid = {0, 0, 0, {0}};

    if (serverProgIDOrClsID[0] == '{')
    {
        LPCOLESTR strClsId = serverProgIDOrClsID.c_str();
        HRESULT result = CLSIDFromString(strClsId, &clsid);
        if (FAILED(result))
        {
            throw OPCException(L"CRemoteHost::connectDAServer: invalid class ID string");
        }
    } // if

    else
    {
        clsid = GetCLSIDFromRemoteRegistry(HostName, serverProgIDOrClsID);
    }

    return connectDAServer(clsid);

} // CRemoteHost::connectDAServer

CLSID CRemoteHost::GetCLSIDFromRemoteRegistry(const std::wstring &hostName, const std::wstring &progID)
{
    std::wstring keyName;
    keyName.append(L"SOFTWARE\\Classes\\");
    keyName.append(progID);
    keyName.append(L"\\Guid"); // was L"\\Clsid"
    HKEY remoteRegHandle = 0;
    HKEY keyHandle = 0;
    char classIdStr[100] = {0};
    CLSID classId = {0, 0, 0, {0}};
    HRESULT result = RegConnectRegistry(WS2LPCTSTR(hostName), HKEY_LOCAL_MACHINE, &remoteRegHandle);
    if (SUCCEEDED(result))
    {
        result = RegOpenKeyEx(remoteRegHandle, WS2LPCTSTR(keyName), 0, KEY_READ, &keyHandle);
        if (SUCCEEDED(result))
        {
            DWORD entryType = 0;
            unsigned bufferSize = 100;
            result = RegQueryValueEx(keyHandle, nullptr, 0, &entryType, (LPBYTE)classIdStr, (LPDWORD)&bufferSize);
            if (FAILED(result))
            {
                throw OPCException(
                    L"CRemoteHost::GetCLSIDFromRemoteRegistry: FAILED to get class ID from remote registry");
            }
            else
            {
                USES_CONVERSION;
                LPOLESTR sz = A2W(classIdStr);
                if (CLSIDFromString(sz, &classId) != S_OK)
                {
                    printf("FAILED sz:(%ws) classIdStr(%s)\n", sz, classIdStr);
                }
            } // else
        }     // if
    }         // if

    RegCloseKey(remoteRegHandle);
    RegCloseKey(keyHandle);
    return classId;

} // CRemoteHost::GetCLSIDFromRemoteRegistry

COPCServer *CRemoteHost::connectDAServer(const CLSID &serverClassID)
{
    ATL::CComPtr<IUnknown> iUnknown;
    makeCOMObjectEx(HostName, CLSCTX_REMOTE_SERVER, serverClassID, IID_IUnknown, (void **)&iUnknown);
    ATL::CComPtr<IOPCServer> iOpcServer;

    HRESULT result = iUnknown->QueryInterface(IID_IOPCServer, (void **)&iOpcServer);
    if (FAILED(result))
    {
        throw OPCException(L"CRemoteHost::connectDAServer: FAILED to obtain IID_IOPCServer interface from server");
    }

    return new COPCServer(iOpcServer);

} // CRemoteHost::connectDAServer

void CRemoteHost::getListOfDAServers(CATID cid, std::vector<std::wstring> &listOfProgIDs,
                                     std::vector<CLSID> &listOfClassIDs)
{
    getListOfDAServersEx(HostName, CLSCTX_REMOTE_SERVER, cid, listOfProgIDs, listOfClassIDs);

} // CRemoteHost::getListOfDAServers

CLSID CRemoteHost::getCLSID(const std::wstring &serverProgID)
{
    CLSID clsId = {0, 0, 0, {0}};
    LPCOLESTR progId = serverProgID.c_str();
    ATL::CComPtr<IOPCServerList> iCatInfo;
    makeCOMObjectEx(HostName, CLSCTX_REMOTE_SERVER, CLSID_OpcServerList, IID_IOPCServerList, (void **)&iCatInfo);

    HRESULT result = iCatInfo->CLSIDFromProgID(progId, &clsId);
    if (FAILED(result))
    {
        throw OPCException(L"CRemoteHost::getCLSID: FAILED to get class ID");
    }

    return clsId;

} // CRemoteHost::getCLSID

CLocalHost::CLocalHost()
{
} // CLocalHost::CLocalHost

COPCServer *CLocalHost::connectDAServer(const std::wstring &serverProgID)
{
    CLSID clsid = {0, 0, 0, {0}};
    LPCOLESTR strClsId = serverProgID.c_str();
    HRESULT result = CLSIDFromProgID(strClsId, &clsid);
    if (FAILED(result))
    {
        throw OPCException(L"CLocalHost::connectDAServer: FAILED to convert prog ID to class ID");
    }

    ATL::CComPtr<IClassFactory> iClassFactory;
    result = CoGetClassObject(clsid, CLSCTX_LOCAL_SERVER, nullptr, IID_IClassFactory, (void **)&iClassFactory);
    if (FAILED(result))
    {
        throw OPCException(L"CLocalHost::connectDAServer: FAILED get class factory");
    }

    ATL::CComPtr<IUnknown> iUnknown;
    result = iClassFactory->CreateInstance(nullptr, IID_IUnknown, (void **)&iUnknown);
    if (FAILED(result))
    {
        throw OPCException(L"CLocalHost::connectDAServer: FAILED get create OPC server ref");
    }

    ATL::CComPtr<IOPCServer> iOpcServer;
    result = iUnknown->QueryInterface(IID_IOPCServer, (void **)&iOpcServer);
    if (FAILED(result))
    {
        throw OPCException(L"CLocalHost::connectDAServer: FAILED to obtain IID_IOPCServer interface from server");
    }

    return new COPCServer(iOpcServer);

} // CLocalHost::connectDAServer

COPCServer *CLocalHost::connectDAServer(const CLSID &clsid)
{
    ATL::CComPtr<IClassFactory> iClassFactory;
    HRESULT result = CoGetClassObject(clsid, CLSCTX_LOCAL_SERVER, nullptr, IID_IClassFactory, (void **)&iClassFactory);
    if (FAILED(result))
    {
        throw OPCException(L"CLocalHost::connectDAServer: FAILED get class factory");
    }

    ATL::CComPtr<IUnknown> iUnknown;
    result = iClassFactory->CreateInstance(nullptr, IID_IUnknown, (void **)&iUnknown);
    if (FAILED(result))
    {
        throw OPCException(L"CLocalHost::connectDAServer: FAILED get create OPC server ref");
    }

    ATL::CComPtr<IOPCServer> iOpcServer;
    result = iUnknown->QueryInterface(IID_IOPCServer, (void **)&iOpcServer);
    if (FAILED(result))
    {
        throw OPCException(L"CLocalHost::connectDAServer: FAILED obtain IID_IOPCServer interface from server");
    }

    return new COPCServer(iOpcServer);

} // CLocalHost::connectDAServer

void CLocalHost::getListOfDAServers(CATID cid, std::vector<std::wstring> &listOfProgIDs,
                                    std::vector<CLSID> &listOfClassIDs)
{
    getListOfDAServersEx(L"localhost", CLSCTX_LOCAL_SERVER, cid, listOfProgIDs, listOfClassIDs);

} // CLocalHost::getListOfDAServers

CLSID CLocalHost::getCLSID(const std::wstring &serverProgID)
{
    (void)serverProgID;
    return CLSID();

} // CLocalHost::getCLSID

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
