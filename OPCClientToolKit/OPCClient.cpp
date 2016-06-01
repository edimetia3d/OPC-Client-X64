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
#include "OPCServer.h"
#include "OPCHost.h"

const GUID COPCClient::CATID_OPCDAv10 = 
{ 0x63d5f430, 0xcfe4, 0x11d1, { 0xb2, 0xc8, 0x0, 0x60, 0x8, 0x3b, 0xa1, 0xfb } };
// {63D5F430-CFE4-11d1-B2C8-0060083BA1FB}

const GUID COPCClient::CATID_OPCDAv20 = 
{ 0x63d5f432, 0xcfe4, 0x11d1, { 0xb2, 0xc8, 0x0, 0x60, 0x8, 0x3b, 0xa1, 0xfb } };
//{63D5F432-CFE4-11d1-B2C8-0060083BA1FB}


ATL::CComPtr<IMalloc> COPCClient::iMalloc;





void COPCClient::init()
{	
	HRESULT	result = CoInitialize(NULL);
	if (FAILED(result))
	{
		throw OPCException("CoInitialize failed");
	}

	CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);

	result = CoGetMalloc(MEMCTX_TASK, &iMalloc);
	if (FAILED(result))
	{
		throw OPCException("CoGetMalloc failed");
	}
}




void COPCClient::stop()
{
	iMalloc = NULL;
	CoUninitialize();
}





void COPCClient::comFree(void *memory){
	iMalloc->Free(memory);
}


void COPCClient::comFreeVariant(VARIANT *memory, unsigned size){
	for (unsigned i = 0; i < size; i++){
		VariantClear(&(memory[i]));	
	}
	iMalloc->Free(memory);
}



COPCHost * COPCClient::makeHost(const CString &hostName){
	if (CString::StringLength(hostName)== 0){
		return new CLocalHost();
	}
	
	return new CRemoteHost(hostName);
}

