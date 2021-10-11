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
#include "OPCGroup.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

/**
 * Holds status information about the server
 */
struct ServerStatus
{
    FILETIME ftStartTime;
    FILETIME ftCurrentTime;
    FILETIME ftLastUpdateTime;
    OPCSERVERSTATE dwServerState;
    DWORD dwGroupCount;
    DWORD dwBandWidth;
    WORD wMajorVersion;
    WORD wMinorVersion;
    WORD wBuildNumber;
    std::wstring vendorInfo;

}; // ServerStatus

/**
 * Local representation of a local or remote OPC server. Wrapper for the COM interfaces to the server.
 */
class OPCDACLIENT_API COPCServer
{
  private:
    /**
     * IUnknown interface to the OPC server
     */
    ATL::CComPtr<IOPCServer> iOpcServer;

    /**
     * Interface to the OPC server namespace
     */
    ATL::CComPtr<IOPCBrowseServerAddressSpace> iOpcNameSpace;

    /**
     * interface to the properties maintained for each item in the server namespace
     */
    ATL::CComPtr<IOPCItemProperties> iOpcProperties;

    /**
     * Used by group object.
     */
    friend class COPCGroup;

    ATL::CComPtr<IOPCServer> &getServerInterface()
    {
        return iOpcServer;
    }

    friend class COPCItem;

    ATL::CComPtr<IOPCItemProperties> &getPropertiesInterface()
    {
        return iOpcProperties;
    }

  public:
    /**
     * Make OPC server.
     * @param opcServerInterface passed form the OPCHost
     */
    COPCServer(ATL::CComPtr<IOPCServer> &opcServerInterface);

    virtual ~COPCServer();

    /**
     * Browse the OPC server's namespace.
     * This is currently done FLAT mode
     * TODO implement browsing of structured namespace
     */
    bool getItemNames(std::vector<std::wstring> &opcItemNames);

    /**
     * Get an OPC group. Caller owns
     */
    COPCGroup *makeGroup(const std::wstring &groupName, bool active, unsigned long reqUpdateRate_ms,
                         unsigned long &revisedUpdateRate_ms, float deadBand);

    /**
     * get the current status of the server.
     */
    bool getStatus(ServerStatus &status);

}; // COPCServer

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
