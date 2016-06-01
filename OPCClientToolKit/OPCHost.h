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

#if !defined(AFX_OPCHOST_H__D8F307D8_4412_4FE7_93AE_E101F5366817__INCLUDED_)
#define AFX_OPCHOST_H__D8F307D8_4412_4FE7_93AE_E101F5366817__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "OPCClient.h"

/**
* Abstract class that represents a PC which may host one or more OPC servers. Provides means of getting a list
* of OPC servers on the host and creating connections to OPC servers.
*/
class COPCHost  
{
public:
	COPCHost();
	virtual ~COPCHost();


	/**
	*  browse OPC servers on this host
	* @param cid the version of the OPC servers to browse
	* @param listOfProgIDs list of servers of version cid on this host
	*/
	virtual void getListOfDAServers(CATID cid, CAtlArray<CString> &listOfProgIDs) = 0;


	/**
	* Connect to OPC Data Access server on this host
	*/
	virtual COPCServer * connectDAServer(const CString & serverProgID) = 0;
};



/**
* Used for accessing OPC servers on a remote host. Make use of OPCenum to browse servers.
*/
class CRemoteHost:public COPCHost{
private:
	/**
	* name of the host we are to look for OPC servers on.
	*/
	CString host;


	/**
	* util object to create remote COM object
	*/
	void makeRemoteObject(const IID requestedClass, const IID requestedInterface, void** interfacePtr);


	CLSID GetCLSIDFromRemoteRegistry(const CString & hostName, const CString &progID);

public:
	CRemoteHost(const CString & hostName);


	/**
	*  browse OPC servers on this host
	* @param cid the version of the OPC servers to browse
	* @param listOfProgIDs list of servers of version cid on this host
	*/
	void getListOfDAServers(CATID cid, CAtlArray<CString> &listOfProgIDs);


	/**
	* Make opc server from progID
	* @returns COPCServer owned by caller
	*/
	COPCServer * connectDAServer(const CString & serverProgID);

	/**
	* Make opc server from classID
	* @returns COPCServer owned by caller
	*/
	COPCServer * connectDAServer(const CLSID & serverClassID);
};


/**
* Used for accessing OPC servers on a local (this) host. Uses the Component Categories manger to enumerate 
* the OPC servers on the local machine.
*/
class CLocalHost:public COPCHost{
public:
	CLocalHost();

	/**
	*  browse OPC servers on this host
	* @param cid the version of the OPC servers to browse
	* @param listOfProgIDs list of servers of version cid on this host
	*/
	void getListOfDAServers(CATID cid, CAtlArray<CString> &listOfProgIDs);


	/**
	* Make opc server from progID
	* @returns COPCServer owned by caller
	*/
	COPCServer * connectDAServer(const CString & serverProgID);
};

#endif // !defined(AFX_OPCHOST_H__D8F307D8_4412_4FE7_93AE_E101F5366817__INCLUDED_)
