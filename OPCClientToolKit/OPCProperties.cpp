#include ".\opcproperties.h"



SPropertyValue::SPropertyValue(const CPropertyDescription &desc, VARIANT &val):propDesc(desc){
	value.vt = VT_EMPTY;
	HRESULT result = VariantCopy( &value, &val);
	if (FAILED(result)){
		throw OPCException("VarCopy failed", result);
	}
}



SPropertyValue::~SPropertyValue(){
	VariantClear(&value);
}
