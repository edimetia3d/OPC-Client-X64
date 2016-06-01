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

#if !defined(AFX_OPCITEM_H__B01EC96B_8666_4498_93C9_980AAFEABFB6__INCLUDED_)
#define AFX_OPCITEM_H__B01EC96B_8666_4498_93C9_980AAFEABFB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "OPCClientToolKitDLL.h"
#include "OPCClient.h"
#include "OPCProperties.h"
class COPCGroup;


/**
* Provides wrapper for operations that typically exist at the group level (e.g. reads) (it is at this level
* that OPC supports the operation) however, we provide the operation at this level for ease of use.
*/
class  COPCItem  
{
private:
	OPCHANDLE serversItemHandle;
    VARTYPE vtCanonicalDataType;
    DWORD dwAccessRights;

	COPCGroup & group;

	CString name;
	
protected:
	friend class COPCGroup;
	// used to set data for the OPC item AFTER it has been created in the server.
	void setOPCParams(OPCHANDLE handle, VARTYPE type, DWORD dwAccess);

	// items may only be created by group.
	COPCItem(CString &itemName, COPCGroup &g);
public:

	virtual ~COPCItem();

	void writeSync(VARIANT &data);


	void readSync(OPCItemData &data, OPCDATASOURCE source);


	/**
	* returned transaction object is owned
	*/
	CTransaction * readAsynch(ITransactionComplete *transactionCB = NULL);


	/**
	* returned transaction object is owned
	*/
	CTransaction * writeAsynch(VARIANT &data, ITransactionComplete *transactionCB = NULL);

	
	DWORD getAccessRights() const{
		return dwAccessRights;
	}

	OPCHANDLE getHandle() const{
		return serversItemHandle;
	}	

	const CString & getName() const{
		return name;
	} 

	void getSupportedProperties(CAtlArray<CPropertyDescription> &desc);

	
	/**
	* retreive the OPC item properties for the descriptors passed. Any data previously existing in propsRead will be destroyed.
	*/
	void getProperties(const CAtlArray<CPropertyDescription> &propsToRead, ATL::CAutoPtrArray<SPropertyValue> &propsRead);
};

#endif // !defined(AFX_OPCITEM_H__B01EC96B_8666_4498_93C9_980AAFEABFB6__INCLUDED_)
