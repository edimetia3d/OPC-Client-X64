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

#if !defined(AFX_OPCCLIENT_H__1C1AA002_F7C5_4537_B569_8352FBA27544__INCLUDED_)
#define AFX_OPCCLIENT_H__1C1AA002_F7C5_4537_B569_8352FBA27544__INCLUDED_


#ifdef OPCCLIENTTOOLKITDLL_EXPORTS
#define OPCCLIENTTOOLKITDLL_API __declspec(dllexport)
#else
#define OPCCLIENTTOOLKITDLL_API __declspec(dllimport)
#endif 


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <atlbase.h>
#include <atlstr.h>
#include <atlexcept.h>
#include <atlcoll.h>
#include <objbase.h>
#include <COMCat.h>
#include <stdexcept>
#include "opcda.h"
#include "OPCItemData.h"


class COPCHost;
class COPCServer;
class COPCGroup;
class COPCItem;


/**
* Basic OPC expection
*/
class  OPCException:public ATL::CAtlException{
private:
	CString why;
public:
	OPCException(const CString& what,HRESULT code = 0):/*ATL::CAtlException(code),*/why(what){}

	const CString & reasonString() const{
		return why;
	}
};



/**
* Data received from the OnDataChange() method of the CAsynchDataCallback instance is delegated to an instance 
* of a child class implementing this interface. The Child class must obviously provide the desired behaviour
* in the overriden OnDataChange() method. This interface is active only when the corresponding group is active
* (achieved by the groups enableSynch() method.)
*/
class IAsynchDataCallback
{
public:
	virtual void OnDataChange(COPCGroup & group, CAtlMap<COPCItem *, OPCItemData *> & changes) = 0;
};












/**
* Starting point for 'everything'. Utility class that creates host objects and handles COM memory management. 
* Effectively a singleton.
*/
class COPCClient  
{
private:
	static ATL::CComPtr<IMalloc> iMalloc; 

public:

	static void init();
	
	static void stop();

	static void comFree(void *memory);

	static void comFreeVariant(VARIANT *memory, unsigned size);

	/**
	* make a host machine abstraction.
	* @param hostname - may be empty (in which case a local host is created).
	* @ returns host object (owned by caller).
	*/
	static COPCHost * makeHost(const CString &hostName);


	static const GUID CATID_OPCDAv10;

	static const GUID CATID_OPCDAv20;
};





#endif // !defined(AFX_OPCCLIENT_H__1C1AA002_F7C5_4537_B569_8352FBA27544__INCLUDED_)
