## Description
An object oriented OPC DA Client SDK/ToolKit written in C++, Both X86/X64 supported, implementing the OPC DA specification version 2.05A

## Known Issues
* the x64 version has some address corruption problem when you are using VS2012 or later, you can open "Project Property -> Linker -> CommandLine", then add `/HIGHENTROPYVA:NO` to avoid this issue, See [Issue#1](https://github.com/edimetia3d/OPC-Client-X64/issues/1) for more detail

## Detail & ChangeLog
* Date:2016-12-13
	* Thanks for [Tom Loya](https://github.com/tomloya)'s contribution, now the project can work in a multi-threaded environment. You should call `COPCClient::init()` and `COPCClient::stop()` in every thread.
* Date:2016-07-01
	* this std branch tries to move every ATL to STL.
	* the static .lib now works for MFC too.
* Date:2016-05-31
	* Modified from X86 Version [OPC Client 0.4a by beharrell](https://sourceforge.net/projects/opcclient/)
	* Add some Hint info
	* X64 Version uses includes files form `OPC Core Component 3.0.106`.
		* Install `OPC Core Component` is **NECESSARY**, I bundled the `3.0.105.1` version (seems more popular).
	* Tested with MatrikonOPC Simulation Server
		* You can get it free at [offical site](https://www.matrikonopc.com/products/opc-drivers/opc-simulation-server.aspx) after reg.
		* Start OPC Simulation Server, then build project. Run the demo, input `hostname`, then input `server ID`, it should work.
	* I advice you to use `hostname` instead of `IP address` for reasons below.
		* If you want to access OPC by IP, you have to enable RemoteRegistry service in `services.msc`
		* Also , for UAC problem after Vista, your program must run as admin to avoid some issue.
