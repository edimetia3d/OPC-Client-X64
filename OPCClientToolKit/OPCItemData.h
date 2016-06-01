#pragma once

#include "OPCClientToolKitDLL.h"
#include "opcda.h"

class COPCItem;


/**
*Wrapper for OPC data. Hides COM memory management.
*/
struct  OPCItemData{
	FILETIME ftTimeStamp;
	WORD wQuality;
	VARIANT vDataValue;
	HRESULT error;


	OPCItemData(HRESULT err);



	OPCItemData(FILETIME time, WORD qual, VARIANT & val, HRESULT err);


	OPCItemData();



	~OPCItemData();


	void set(OPCITEMSTATE &itemState);


	void set(FILETIME time, WORD qual, VARIANT & val);

	OPCItemData & operator=(OPCItemData &itemData);
};



/**
* Can't find an ATL autoptr map - this class provides the functionality I want.
*/
class  COPCItem_DataMap:public CAtlMap<COPCItem *, OPCItemData *>{
public:
	~COPCItem_DataMap();
};



