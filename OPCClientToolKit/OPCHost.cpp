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

#include "OPCHost.h"
#include "OPCServer.h"
#include "OpcEnum.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COPCHost::COPCHost()
{

}

COPCHost::~COPCHost()
{

}















CRemoteHost::CRemoteHost(const CString & hostName):host(hostName){
}
	


void CRemoteHost::makeRemoteObject(const IID requestedClass, const IID requestedInterface, void** interfacePtr){
	COAUTHINFO athn;
    ZeroMemory(&athn, sizeof(COAUTHINFO));
    // Set up the NULL security information
    athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_CONNECT;
	//athn.dwAuthnLevel = RPC_C_AUTHN_LEVEL_NONE;
    athn.dwAuthnSvc = RPC_C_AUTHN_WINNT;
    athn.dwAuthzSvc = RPC_C_AUTHZ_NONE;
    athn.dwCapabilities = EOAC_NONE;
    athn.dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE;
    athn.pAuthIdentityData = NULL;
    athn.pwszServerPrincName = NULL;


	COSERVERINFO remoteServerInfo;
	ZeroMemory(&remoteServerInfo, sizeof(COSERVERINFO));
	remoteServerInfo.pAuthInfo = &athn;
	USES_CONVERSION;
	remoteServerInfo.pwszName = T2OLE(host);
	//printf("%s\n", OLE2T(remoteServerInfo.pwszName));

	MULTI_QI reqInterface;
	reqInterface.pIID = &requestedInterface;
	reqInterface.pItf = NULL;
	reqInterface.hr = S_OK;

	HRESULT result = CoCreateInstanceEx(requestedClass,NULL, CLSCTX_REMOTE_SERVER, 
					   &remoteServerInfo, 1, &reqInterface);	
	
	if (FAILED(result)){
		printf("Error %x\n", result);
		throw OPCException("Failed to get remote interface");
	}
	
	*interfacePtr = reqInterface.pItf; // avoid ref counter getting incremented again
}




CLSID CRemoteHost::GetCLSIDFromRemoteRegistry(const CString & hostName, const CString &progID)
{
   CString keyName = "SOFTWARE\\Classes\\" + progID + "\\Clsid";
   HKEY remoteRegHandle;
   HKEY keyHandle;
   char classIdString[100];
   CLSID classId;
   HRESULT result = RegConnectRegistry(hostName, HKEY_LOCAL_MACHINE, &remoteRegHandle);
   if (SUCCEEDED(result)){
       result = RegOpenKeyEx(remoteRegHandle, keyName, 0, KEY_READ, &keyHandle);
	   if (SUCCEEDED(result)){
		   DWORD entryType;
	
		   unsigned bufferSize = 100;
           result = RegQueryValueEx(keyHandle, NULL, 0, &entryType, (LPBYTE)&classIdString, (LPDWORD)&bufferSize);
           if (FAILED(result)){
			printf("here");
		   }else{
				USES_CONVERSION;
				LPOLESTR sz = A2W(classIdString);
				if (CLSIDFromString(sz,&classId) != S_OK){
					printf("Failed");
				}
		   }
	   }       
	}	

	RegCloseKey(remoteRegHandle);
	RegCloseKey(keyHandle);
	return classId;
}




COPCServer * CRemoteHost::connectDAServer(const CString & serverProgID){
	CLSID clsid = GetCLSIDFromRemoteRegistry(host, serverProgID);
	return connectDAServer(clsid);
}


COPCServer * CRemoteHost::connectDAServer(const CLSID & serverClassID){
	ATL::CComPtr<IUnknown> iUnknown;
	makeRemoteObject(serverClassID, IID_IUnknown, (void **)&iUnknown);

	ATL::CComPtr<IOPCServer> iOpcServer;
	HRESULT result = iUnknown->QueryInterface(IID_IOPCServer, (void**)&iOpcServer);
	if (FAILED(result))
	{
		throw OPCException("Failed obtain IID_IOPCServer interface from server");
	}

	return new COPCServer(iOpcServer);
}



void CRemoteHost::getListOfDAServers(CATID cid, CAtlArray<CString> &listOfProgIDs){
	ATL::CComPtr<IOPCServerList> iCatInfo;

	 
	makeRemoteObject(CLSID_OpcServerList, IID_IOPCServerList, (void**)&iCatInfo);

	CATID Implist[1];
	Implist[0] = cid;

	ATL::CComPtr<IEnumCLSID> iEnum;
	HRESULT result = iCatInfo->EnumClassesOfCategories(1, Implist,0, NULL,&iEnum);
	if (FAILED(result)){
		throw OPCException("Failed to get enum for categeories");
	}

	GUID glist;
	ULONG actual;
	while((result = iEnum->Next(1, &glist, &actual)) == S_OK)
	{
		WCHAR *progID;
		WCHAR *userType;
		HRESULT res = iCatInfo->GetClassDetails(glist, &progID, &userType);/*ProgIDFromCLSID(glist, &progID)*/;
		if(FAILED(res)){
			throw OPCException("Failed to get ProgId from ClassId");
		}
		else {
			USES_CONVERSION;
			char * str = OLE2T(progID);
			char * str1 = OLE2T(userType);
			//printf("Adding %s\n", str);
			listOfProgIDs.Add(str);
			COPCClient::comFree(progID);
			COPCClient::comFree(userType);
		}
	}
}












CLocalHost::CLocalHost(){

}




COPCServer * CLocalHost::connectDAServer(const CString & serverProgID){
	USES_CONVERSION;
	WCHAR* wideName = T2OLE(serverProgID);

	CLSID clsid;
	HRESULT result = CLSIDFromProgID(wideName, &clsid);
	if(FAILED(result))
	{
		throw OPCException("Failed to convert progID to class ID");
	}


	ATL::CComPtr<IClassFactory> iClassFactory;
	result = CoGetClassObject(clsid, CLSCTX_LOCAL_SERVER, NULL, IID_IClassFactory, (void**)&iClassFactory);
	if (FAILED(result))
	{
		throw OPCException("Failed get Class factory");
	}

	ATL::CComPtr<IUnknown> iUnknown;
	result = iClassFactory->CreateInstance(NULL, IID_IUnknown, (void**)&iUnknown);
	if (FAILED(result))
	{
		throw OPCException("Failed get create OPC server ref");
	}

	ATL::CComPtr<IOPCServer> iOpcServer;
	result = iUnknown->QueryInterface(IID_IOPCServer, (void**)&iOpcServer);
	if (FAILED(result))
	{
		throw OPCException("Failed obtain IID_IOPCServer interface from server");
	}

	return new COPCServer(iOpcServer);
}



void CLocalHost::getListOfDAServers(CATID cid, CAtlArray<CString> &listOfProgIDs){
	CATID Implist[1];
	Implist[0] = cid;
	ATL::CComPtr<ICatInformation> iCatInfo;
	 

	HRESULT result = CoCreateInstance (CLSID_StdComponentCategoriesMgr, NULL,CLSCTX_INPROC_SERVER, IID_ICatInformation,(void **)&iCatInfo);
	if (FAILED(result)){
		throw OPCException("Failed to get IID_ICatInformation");
	}

	ATL::CComPtr<IEnumCLSID> iEnum;
	result = iCatInfo->EnumClassesOfCategories(1, Implist,0, NULL,&iEnum);
	if (FAILED(result)){
		throw OPCException("Failed to get enum for categeories");
	}

	GUID glist;
	ULONG actual;
	while((result = iEnum->Next(1, &glist, &actual)) == S_OK)
	{
		WCHAR *progID;
		HRESULT res = ProgIDFromCLSID(glist, &progID);
		if(FAILED(res)){
			throw OPCException("Failed to get ProgId from ClassId");
		}
		else {
			USES_CONVERSION;
			char * str = OLE2T(progID);
			listOfProgIDs.Add(str);
			COPCClient::comFree(progID);
		}
	}
}



