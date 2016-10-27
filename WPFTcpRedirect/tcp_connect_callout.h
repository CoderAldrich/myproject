/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

   Stream Edit Callout Driver Sample.

   This sample demonstrates finding and replacing a string pattern from a
   live TCP stream via the WFP stream API.

--*/

#ifndef _STREAM_CALLOUT_H
#define _STREAM_CALLOUT_H

// 
// Configurable parameters
//

extern USHORT  configInspectionPort;

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union
#pragma warning(pop)

extern
NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteN(
    __out_bcount_part(MaxBytesInMultiByteString, *BytesInMultiByteString) PCHAR MultiByteString,
    __in ULONG MaxBytesInMultiByteString,
    __out_opt PULONG BytesInMultiByteString,
    __in_bcount(BytesInUnicodeString) PWCH UnicodeString,
    __in ULONG BytesInUnicodeString
    );

#endif // _STREAM_CALLOUT_H

