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













COPCServer::COPCServer(ATL::CComPtr<IOPCServer> &opcServerInterface){
	iOpcServer = opcServerInterface;

	HRESULT res = opcServerInterface->QueryInterface(IID_IOPCBrowseServerAddressSpace, (void**)&iOpcNamespace);
	if (FAILED(res)){
		throw OPCException("Failed to obtain IID_IOPCBrowseServerAddressSpace interface",res);
	}

	res = opcServerInterface->QueryInterface(IID_IOPCItemProperties, (void**)&iOpcProperties);
	if (FAILED(res)){
		throw OPCException("Failed to obtain IID_IOPCItemProperties interface",res);
	}
}



COPCServer::~COPCServer()
{
}



COPCGroup *COPCServer::makeGroup(const CString & groupName, bool active, unsigned long reqUpdateRate_ms, unsigned long &revisedUpdateRate_ms, float deadBand){
	return new COPCGroup(groupName, active, reqUpdateRate_ms, revisedUpdateRate_ms, deadBand, *this);
}


void COPCServer::getItemNames(CAtlArray<CString> & opcItemNames){
	if (!iOpcNamespace) return;

	OPCNAMESPACETYPE nameSpaceType;
	HRESULT result = iOpcNamespace->QueryOrganization(&nameSpaceType);

	USES_CONVERSION;
	int v = 0;
	WCHAR emptyString[] = {0};
	//result = iOpcNamespace->ChangeBrowsePosition(OPC_BROWSE_TO,emptyString);

	ATL::CComPtr<IEnumString> iEnum;
	result = iOpcNamespace->BrowseOPCItemIDs(OPC_FLAT,emptyString,VT_EMPTY,0,(&iEnum));
	if (FAILED(result)){
		return;
	}


	 
    WCHAR * str;
    ULONG strSize;
	while((result = iEnum->Next(1, &str, &strSize)) == S_OK)
	{
		WCHAR * fullName;
		result = iOpcNamespace->GetItemID(str, &fullName);
		if (SUCCEEDED(result)){
			USES_CONVERSION;
			char * cStr = OLE2T(fullName);
			//char * cStr = OLE2T(str);
			//printf("Adding %s\n", cStr);
			opcItemNames.Add(cStr);
			COPCClient::comFree(fullName);
		}
		COPCClient::comFree(str);
	}
}


void COPCServer::getStatus(ServerStatus &status){
	OPCSERVERSTATUS *serverStatus;
	HRESULT result = iOpcServer->GetStatus(&serverStatus);
	if (FAILED(result)){
		throw OPCException("Failed to get status");
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
	if (serverStatus->szVendorInfo != NULL){
		USES_CONVERSION;
		status.vendorInfo = OLE2T(serverStatus->szVendorInfo);
		COPCClient::comFree(serverStatus->szVendorInfo);
	}
	COPCClient::comFree(serverStatus);
}