#pragma once

#include <atlstr.h>
#include <atlenc.h>

CStringA EasyBase64Encode( LPCSTR pchSourcString );
CStringA EasyBase64Decode( LPCSTR pchEncodeString );