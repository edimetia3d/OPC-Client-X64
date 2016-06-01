#pragma once

#include "OPCClient.h"
class CTransaction;



/**
* Interface which provides means by which the client can be notified when an asynchronous operation
* is completed. The implementer must implement this interface overriding the complete method to provide
* the desired behaviour. An instance of the implementation can be passed as a parameter to an asynchronous 
* operation and used as a means of completion notification.
*/
class ITransactionComplete{
public:
	virtual void complete(CTransaction &transaction) = 0;
};





/**
* Used to indicate completion of an asynchronous operation. 
* Will contain the results of that operation.
*/
class CTransaction{

	/**
	* Optional transation complete callback - not owned
	*/
	ITransactionComplete * completeCallBack;

	// true when the transaction has completed
	bool completed;
	

	DWORD cancelID;


public:
	/**
	* keyed on OPCitem address (not owned)
	* OPCitem data is owned by the transaction - may be NULL
	*/
	COPCItem_DataMap opcData;


	CTransaction(ITransactionComplete * completeCB = NULL);

	/**
	* Used where the transaction completion will result in data being received.
	*/
	CTransaction(CAtlArray<COPCItem *>&items, ITransactionComplete * completeCB);


	
	void setItemError(COPCItem *item, HRESULT error);


	void setItemValue(COPCItem *item, FILETIME time, WORD qual, VARIANT & val, HRESULT err);


	/**
	* return Value stored for a given opc item.
	*/
	const OPCItemData * getItemValue(COPCItem *item) const;
			

	/**
	* trigger completion of the transaction.
	*/
	void setCompleted();

	bool isCompeleted() const{
		return completed;
	}

	void setCancelId(DWORD id){
		cancelID = id;
	}

	DWORD getCancelId() const{
		return cancelID;
	}
};
