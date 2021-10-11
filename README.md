## Description

* An object oriented OPC DA Client SDK/ToolKit written in C++, Both X86/X64 supported, implementing the OPC DA
  specification version 2.05A
* New Features (Compare with the original version)
    * Build with x64 project.
    * Build with MFC.
    * Multithread supported.
    * Connect to server with CLSID.(Hasn't tested)

## Notes

Current master branch contains a huge bug fixes commit from [PR#14](https://github.com/edimetia3d/OPC-Client-X64/pull/14), which might introduce some new bugs. If the master branch went
wrong, you may check release 0.4.1 as a backup.

## Basic Guide

* Install `OPC Core Component x64` is **NECESSARY** if you want to build x64 project, I bundled the `3.0.105.1`
  version (seems more popular).
* You can test the Toolkit with MatrikonOPC Simulation Server.
    * You can get it free at [offical site](https://www.matrikonopc.com/products/opc-drivers/opc-simulation-server.aspx)
      after reg.
    * Start OPC Simulation Server, then build project. Run the demo, input `hostname`, then input `server ID`, it should
      work.
    * If you were to commit a issue, you should give a code sample with the Simulation Server. Since this is the only
      env we can reproduce the error.
* I advice you to use `hostname` instead of `IP address` for reasons below.
    * If you want to access OPC by IP, you have to enable RemoteRegistry service in `services.msc`
    * Also , for UAC problem after Vista, your program must run as admin to avoid some issue.
* There is a convenience wrapper class that may help you connect local server easier.

## Advanced Guide

* Multi - Thread Programing Guide
    * Call `COPCCLint::init`and `COPCClinet::stop` in every thread. There are two method to init, in one application,
      different thread could use different method.
        * You can call `COPCClient::init(MULTITHREADED)` to init thread OLE, then the `COPCxxx` created in the thread
          could be accessed from other thread that is also inited with `MULTITHREADED`.
        * You can call `COPCClient::init()` to init thread OLE, but all `COPCxxx` created in the thread can't be
          accessed from different thread.
    * Attention: If you are using anything related to **MFC GUI**, you MUST use `COPCClient::init()` in main thread (the
      UI thread)
      . ([See MSDN](https://support.microsoft.com/en-us/help/828643/mfc-application-stops-responding-when-you-initialize-the-application-a))
      .

## Detail & ChangeLog
* Date: 2021-10-10
    * Thanks for wolfgang.griech (wolfgang.griech@gmail.com), who contributed a huge bug fixes [PR#14](https://github.com/edimetia3d/OPC-Client-X64/pull/14) that may fix many issues in this project.
* Date: 2017-07-07
    * Thanks for [mzillgith](https://github.com/mzillgith/OPC-Client-X64), now the project can connect to remote server
      with CLSID.(It hasn't been tested)

* Date: 2017-04-13
    * Fix a string conversion bug which might lead stack overflow
    * Add a convenience class to connect local server in sync I/O mode
  ```cpp
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

  ```
    * You could rewrite or override the member function `IsOPCConnectedPLC()` to make connection more safety
    * You could rewrite or override the member function `ItemNameFilter(std::string)` to avoid adding useless items
    * Since there are too
      many [Variant type](https://msdn.microsoft.com/en-us/library/windows/desktop/ms221627(v=vs.85).aspx), I only add
      three basic I/O functions as a guidence, it should help you to add what you need

* Date:2016-12-13
    * Thanks for [Tom Loya](https://github.com/tomloya)'s contribution, now the project can work in a multi-threaded
      environment. You should call `COPCClient::init()` and `COPCClient::stop()` in every thread.
* Date:2016-07-01
    * this std branch tries to move every ATL to STL.
    * the static .lib now works for MFC too.
* Date:2016-05-31
    * Modified from X86 Version [OPC Client 0.4a by beharrell](https://sourceforge.net/projects/opcclient/)
    * Add some Hint info
    * X64 Version uses includes files form `OPC Core Component 3.0.106`.
