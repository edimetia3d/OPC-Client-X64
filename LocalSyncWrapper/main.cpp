#include "LocalSyncOPCCLient.h"
#include <iostream>

void main()
{
	// connect local server
	LocalSyncOPCCLient* client = new LocalSyncOPCCLient;
	client->Init();
	if (client->Connect("Matrikon.OPC.Simulation.1"))
	{
		// sync write and sync read
		client->WriteUint16("Bucket Brigade.UInt2", 998);
		std::cout << client->ReadUint16("Bucket Brigade.UInt2");

		// disconnect and stop
		client->DisConnect();
		client->Stop();
		delete client;
	}

	getchar();
}
