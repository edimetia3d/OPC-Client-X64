#include <atlstr.h>
#include <atlcoll.h>
#include "OPCItemData.h"
#include "OPCClient.h"


OPCItemData::OPCItemData(HRESULT err):error(err){
	vDataValue.vt = VT_EMPTY;
}



OPCItemData::OPCItemData(FILETIME time, WORD qual, VARIANT & val, HRESULT err){
	vDataValue.vt = VT_EMPTY;
	HRESULT result = VariantCopy( &vDataValue, &val);
	if (FAILED(result)){
		throw OPCException("VarCopy failed");
	}

	ftTimeStamp = time;
	wQuality = qual;
	error = err;
}


OPCItemData::OPCItemData(){
	vDataValue.vt = VT_EMPTY;
}



OPCItemData::~OPCItemData(){
	VariantClear(&vDataValue);
}


void OPCItemData::set(OPCITEMSTATE &itemState){
	HRESULT result = VariantCopy( &vDataValue, &itemState.vDataValue);
	if (FAILED(result)){
		throw OPCException("VarCopy failed");
	}

	ftTimeStamp = itemState.ftTimeStamp;
	wQuality = itemState.wQuality;
}


void OPCItemData::set(FILETIME time, WORD qual, VARIANT & val){
	HRESULT result = VariantCopy( &vDataValue, &val);
	if (FAILED(result)){
		throw OPCException("VarCopy failed");
	}

	ftTimeStamp = time;
	wQuality = qual;
}

OPCItemData & OPCItemData::operator=(OPCItemData &itemData){
	HRESULT result = VariantCopy( &vDataValue, &(itemData.vDataValue));
	if (FAILED(result)){
		throw OPCException("VarCopy failed");
	}

	ftTimeStamp = itemData.ftTimeStamp;
	wQuality = itemData.wQuality;

	return *this;
}





COPCItem_DataMap::~COPCItem_DataMap(){
	POSITION pos = GetStartPosition();
	while (pos != NULL){
		OPCItemData * data = GetNextValue(pos);
		if (data){
			delete data;
		}
	}
	RemoveAll();
}





