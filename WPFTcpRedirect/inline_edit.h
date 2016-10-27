/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    Stream Edit Callout Driver Sample.
    
    This sample demonstrates inline stream inspection/editing 
    via the WFP stream API.

Environment:

    Kernel mode

--*/

#ifndef _INLINE_EDIT_H
#define _INLINE_EDIT_H

#if(NTDDI_VERSION >= NTDDI_WIN7)
void 
NTAPI
TcpConnectClassify(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN OUT void* layerData,
   IN const void* classifyContext,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut
   );
#else if(NTDDI_VERSION < NTDDI_WIN7)
void 
NTAPI
TcpConnectClassify(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN OUT void* layerData,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut
   );
#endif

#endif // _INLINE_EDIT_H

