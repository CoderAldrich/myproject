/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

    Stream Edit Callout Driver Sample.
    
    This sample demonstrates inline stream inspection/editing 
    via the WFP stream API.

Environment:

    Kernel mode

--*/

#include "ntddk.h"

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

#include "fwpsk.h"

#pragma warning(pop)

#include "fwpmk.h"

#include "inline_edit.h"
#include "tcp_connect_callout.h"

USHORT htons(USHORT hostshort)
{
	return ((hostshort << 8) | (hostshort >> 8));
}

USHORT ntohs(USHORT netshort)
{
	return ((netshort << 8) | (netshort >> 8));
}


#if (NTDDI_VERSION >= NTDDI_WIN7)
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
   )
#else if (NTDDI_VERSION < NTDDI_WIN7)
void 
NTAPI
TcpConnectClassify(
   IN const FWPS_INCOMING_VALUES* inFixedValues,
   IN const FWPS_INCOMING_METADATA_VALUES* inMetaValues,
   IN OUT void* layerData,
   IN const FWPS_FILTER* filter,
   IN UINT64 flowContext,
   OUT FWPS_CLASSIFY_OUT* classifyOut
   )
#endif
/* ++

   This is the ClassifyFn function registered by the inline stream edit callout.

   An inline stream modification callout performs editing from within the
   ClassifyFn call by permitting sections of the content and replacing other
   sections by removing them and injecting new content.

-- */
{
	if (classifyOut->rights & FWPS_RIGHT_ACTION_WRITE)
	{	
		//DbgPrint(" In StreamInlineEditClassify\n");

		if ( inFixedValues->layerId == FWPS_LAYER_ALE_CONNECT_REDIRECT_V4)
		{
			NTSTATUS status = STATUS_SUCCESS;
			HANDLE classifyHandle = NULL;
			PVOID pWritableLayerData = NULL;

			classifyOut->actionType = FWP_ACTION_PERMIT;

			//DbgPrint(" FWPS_LAYER_ALE_CONNECT_REDIRECT_V4 \n");

			status = FwpsAcquireClassifyHandle((void*)classifyContext,0,&classifyHandle);

			if(status == STATUS_SUCCESS)
			{
 				//DbgPrint("2222\n");

  				status = FwpsAcquireWritableLayerDataPointer(classifyHandle,
  					filter->filterId,
  					0,
  					&pWritableLayerData,
  					classifyOut);
  				if ( status == STATUS_SUCCESS && pWritableLayerData )

  				{
  					FWPS_CONNECT_REQUEST *pRequest = (FWPS_CONNECT_REQUEST *)pWritableLayerData;

					SOCKADDR_IN *pSockAddr = (SOCKADDR_IN *)&(pRequest->remoteAddressAndPort);

					DbgPrint(" %d.%d.%d.%d :%d",
						pSockAddr->sin_addr.S_un.S_un_b.s_b1,
						pSockAddr->sin_addr.S_un.S_un_b.s_b2,
						pSockAddr->sin_addr.S_un.S_un_b.s_b3,
						pSockAddr->sin_addr.S_un.S_un_b.s_b4,
						htons(pSockAddr->sin_port));

					pSockAddr->sin_addr.S_un.S_un_b.s_b1 = 192;
					pSockAddr->sin_addr.S_un.S_un_b.s_b2 = 168;
					pSockAddr->sin_addr.S_un.S_un_b.s_b3 = 0;
					pSockAddr->sin_addr.S_un.S_un_b.s_b4 = 21;
					
					FwpsApplyModifiedLayerData(classifyHandle,pWritableLayerData,FWPS_CLASSIFY_FLAG_REAUTHORIZE_IF_MODIFIED_BY_OTHERS);
 					//DbgPrint("1111\n"); 
  				}

				FwpsReleaseClassifyHandle(classifyHandle);
			}



		}

		
	}


   return;
}

