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

#include <codecvt>
#include <locale>
#include <vector>

#include "OPCClient.h"
#include "OPCClientToolKitDLL.h"

#ifdef OPCDA_CLIENT_NAMESPACE
namespace opcda_client
{
#endif

/**
 * Abstract class that represents a PC which may host one or more OPC servers. Provides means of getting a list
 * of OPC servers on the host and creating connections to OPC servers.
 */
class OPCDACLIENT_API COPCHost
{
    /**
     * TCHAR:    WCHAR if UNICODE is defined, a CHAR otherwise.
     * WCHAR:    16-bit Unicode character.
     * CHAR:     8-bit Windows (ANSI) character.
     * LPTSTR:   LPWSTR if UNICODE is defined, an LPSTR otherwise.
     * LPSTR:    pointer to a null-terminated string of 8-bit Windows (ANSI) characters.
     * LPWSTR:   pointer to a null-terminated string of 16-bit Unicode characters.
     * LPCTSTR:  LPCWSTR if UNICODE is defined, an LPCSTR otherwise.
     * LPCWSTR:  pointer to a constant null-terminated string of 16-bit Unicode characters.
     * LPCSTR:   pointer to a constant null-terminated string of 8-bit Windows (ANSI) characters.
     **/

  public:
    static std::string WS2S(const std::wstring &wstr)
    {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(wstr);
    }

    static std::wstring S2WS(const std::string &str)
    {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(str);
    }

    static std::basic_string<TCHAR> WS2T(const std::wstring &wstr)
    {
#ifdef UNICODE
        return wstr;
#else
        return WS2S(wstr);
#endif
    } // WS2T

    static std::basic_string<TCHAR> S2T(const std::string &str)
    {
#ifdef UNICODE
        return S2WS(str);
#else
        return str;
#endif
    } // S2T

    static LPCTSTR WS2LPCTSTR(const std::wstring &wstr)
    {
        return WS2T(wstr).c_str();
    }

    static LPCTSTR S2LPCTSTR(const std::string &str)
    {
        return S2T(str).c_str();
    }

    static std::wstring LPCSTR2WS(LPCSTR str)
    {
        return S2WS(std::string(str));
    }

  protected:
    /**
     * utility to create COM object instance
     */
    void makeCOMObjectEx(std::wstring hostName, tagCLSCTX serverLocation, const IID requestedClass,
                         const IID requestedInterface, void **interfacePtr);

    /**
     *  browse OPC servers on this host
     * @param cid the version of the OPC servers to browse
     * @param listOfProgIDs list of servers of version cid on this host
     */
    void getListOfDAServersEx(std::wstring hostName, tagCLSCTX serverLocation, CATID cid,
                              std::vector<std::wstring> &listOfProgIDs, std::vector<CLSID> &listOfClassIDs);

  public:
    COPCHost();

    virtual ~COPCHost();

    /**
     *  browse OPC servers on this host
     * @param cid the version of the OPC servers to browse
     * @param listOfProgIDs list of servers of version cid on this host
     */
    virtual void getListOfDAServers(CATID cid, std::vector<std::wstring> &listOfProgIDs,
                                    std::vector<CLSID> &listOfClassIDs) = 0;

    virtual CLSID getCLSID(const std::wstring &serverProgID) = 0;

    /**
     * Connect to OPC Data Access server on this host
     */
    virtual COPCServer *connectDAServer(const std::wstring &serverProgID) = 0;

    virtual COPCServer *connectDAServer(const CLSID &clsid) = 0;

}; // COPCHost

/**
 * Used for accessing OPC servers on a remote host. Make use of OpcEnum to browse servers.
 */
class OPCDACLIENT_API CRemoteHost : public COPCHost
{
  private:
    /**
     * name of the host we are to look for OPC servers on.
     */
    std::wstring HostName;

    CLSID GetCLSIDFromRemoteRegistry(const std::wstring &hostName, const std::wstring &progID);

  public:
    CRemoteHost(const std::wstring &hostName);

    /**
     *  browse OPC servers on this host
     * @param cid the version of the OPC servers to browse
     * @param listOfProgIDs list of servers of version cid on this host
     */
    void getListOfDAServers(CATID cid, std::vector<std::wstring> &listOfProgIDs, std::vector<CLSID> &listOfClassIDs);

    CLSID getCLSID(const std::wstring &serverProgID);

    /**
     * Make opc server from progID
     * @returns COPCServer owned by caller
     */
    COPCServer *connectDAServer(const std::wstring &serverProgID);

    /**
     * Make opc server from classID
     * @returns COPCServer owned by caller
     */
    COPCServer *connectDAServer(const CLSID &serverClassID);

}; // CRemoteHost

/**
 * Used for accessing OPC servers on a local (this) host. Uses the Component Categories manger to enumerate
 * the OPC servers on the local machine.
 */
class OPCDACLIENT_API CLocalHost : public COPCHost
{
  public:
    CLocalHost();

    /**
     *  browse OPC servers on this host
     * @param cid the version of the OPC servers to browse
     * @param listOfProgIDs list of servers of version cid on this host
     */
    void getListOfDAServers(CATID cid, std::vector<std::wstring> &listOfProgIDs, std::vector<CLSID> &listOfClassIDs);

    CLSID getCLSID(const std::wstring &serverProgID);

    /**
     * Make opc server from progID
     * @returns COPCServer owned by caller
     */
    COPCServer *connectDAServer(const std::wstring &serverProgID);

    /**
     * Make opc server from CLSID
     * @returns COPCServer owned by caller
     */
    COPCServer *connectDAServer(const CLSID &clsid);

}; // CLocalHost

#ifdef OPCDA_CLIENT_NAMESPACE
} // namespace opcda_client
#endif
