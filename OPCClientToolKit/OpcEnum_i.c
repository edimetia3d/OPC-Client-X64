

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */

/* File created by MIDL compiler version 6.00.0366 */
/* at Thu Feb 05 03:04:00 2009
 */
/* Compiler settings for .\OpcEnum.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data
    VC __declspec() decoration level:
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning(disable : 4049) /* more than 64k source lines */

#ifdef __cplusplus
extern "C"
{
#endif

#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type, name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)                                        \
    DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type, name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)                                        \
    const type name = {l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}

#endif !_MIDL_USE_GUIDDEF_

    MIDL_DEFINE_GUID(IID, LIBID_OpcEnumLib, 0x13486D43, 0x4821, 0x11D2, 0xA4, 0x94, 0x3C, 0xB3, 0x06, 0xC1, 0x00, 0x00);

    MIDL_DEFINE_GUID(CLSID, CLSID_OpcServerList, 0x13486D51, 0x4821, 0x11D2, 0xA4, 0x94, 0x3C, 0xB3, 0x06, 0xC1, 0x00,
                     0x00);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif
