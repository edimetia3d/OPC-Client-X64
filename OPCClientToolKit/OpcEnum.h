

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Mon May 23 14:10:22 2016
 */
/* Compiler settings for OpcEnum.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __OpcEnum_h__
#define __OpcEnum_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __OpcServerList_FWD_DEFINED__
#define __OpcServerList_FWD_DEFINED__

#ifdef __cplusplus
typedef class OpcServerList OpcServerList;
#else
typedef struct OpcServerList OpcServerList;
#endif /* __cplusplus */

#endif 	/* __OpcServerList_FWD_DEFINED__ */


/* header files for imported files */
#include "opccomn.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __OpcEnumLib_LIBRARY_DEFINED__
#define __OpcEnumLib_LIBRARY_DEFINED__

/* library OpcEnumLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_OpcEnumLib;

EXTERN_C const CLSID CLSID_OpcServerList;

#ifdef __cplusplus

class DECLSPEC_UUID("13486D51-4821-11D2-A494-3CB306C10000")
OpcServerList;
#endif
#endif /* __OpcEnumLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


