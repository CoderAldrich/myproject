#pragma once
#include <atlstr.h>
#include <string>
using namespace std;

CStringA Base64_Encode(const unsigned char* Data,int DataByte);
string Base64_Decode(const char* Data,int DataByte,int& OutByte);