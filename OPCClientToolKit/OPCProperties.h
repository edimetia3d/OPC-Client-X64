#pragma once
#include "OPCClient.h"

/**
* Holds desciption of a property for an OPC item.
*/
struct CPropertyDescription{
	/// properties identifier
	DWORD id;

	/// server supplied textual description
	CString desc;

	/// data type of the property
	VARTYPE type;

	CPropertyDescription(DWORD i, CString d, VARTYPE t):id(i),desc(d), type(t){};
};




/**
* holds value for an OPC item property
*/
struct SPropertyValue{
	/// the property that this value describes.
	const CPropertyDescription & propDesc;

	/// properties value.
	VARIANT value;

	SPropertyValue(const CPropertyDescription &desc, VARIANT &val);

	~SPropertyValue();
};
