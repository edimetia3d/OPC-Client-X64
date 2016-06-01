#include ".\transaction.h"


CTransaction::CTransaction(ITransactionComplete * completeCB)
:completed(FALSE), cancelID(0xffffffff), completeCallBack(completeCB){
}



CTransaction::CTransaction(CAtlArray<COPCItem *>&items, ITransactionComplete * completeCB)
:completed(FALSE), cancelID(0xffffffff), completeCallBack(completeCB){
	for (unsigned i = 0; i < items.GetCount(); i++){
		opcData.SetAt(items[i],NULL);
	}
}


void CTransaction::setItemError(COPCItem *item, HRESULT error){
	CAtlMap<COPCItem *, OPCItemData *>::CPair* pair = opcData.Lookup(item);
	opcData.SetValueAt(pair,new OPCItemData(error));
}



void CTransaction::setItemValue(COPCItem *item, FILETIME time, WORD qual, VARIANT & val, HRESULT err){
	CAtlMap<COPCItem *, OPCItemData *>::CPair* pair = opcData.Lookup(item);
	opcData.SetValueAt(pair,new OPCItemData(time, qual, val, err));
}


const OPCItemData * CTransaction::getItemValue(COPCItem *item) const{
	const CAtlMap<COPCItem *, OPCItemData *>::CPair* pair = opcData.Lookup(item);
	if (!pair) return NULL; // abigious - we do'nt know if the key does not exist or there is no value - TODO throw exception

	return pair->m_value;
}

void CTransaction::setCompleted(){
	completed = TRUE;
	if (completeCallBack){
		completeCallBack->complete(*this);
	}
}
