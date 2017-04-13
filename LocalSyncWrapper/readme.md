# Local OPC Sync Client Wrapper
This is a c++ convenience class which should help you connect local OPC server much more easier, it is based on the OPC-Client-X64.

## Usage
```cpp
	// connect local server
	LocalSyncOPCCLient* client = new LocalSyncOPCCLient;
	client->Init();
	if (client->Connect("Matrikon.OPC.Simulation.1"))
	{
		// sync write and sync read
		client->WriteUint16("Bucket Brigade.UInt2", 998);
		std:std::cout << client->ReadUint16("Bucket Brigade.UInt2");

		// disconnect and stop
		client->DisConnect();
		client->Stop();
		delete client;
	}

```

## Tips
* The latest .lib and include files could be found in [OPC-Client-X64](https://github.com/edimetia3d/OPC-Client-X64)
* You could rewrite or override the member function `IsOPCConnectedPLC()` to make connection more safety
* You could rewrite or override the member function `ItemNameFilter(std::string)` to avoid adding useless items
* Since there are too many [Variant type](https://msdn.microsoft.com/en-us/library/windows/desktop/ms221627(v=vs.85).aspx), I only add three basic I/O functions as a guidence, it should help you to add what you need