#pragma once


BOOL WINAPI InitShieldResource();
BOOL WINAPI UpdateShieldType( LPCWSTR *pszArrayTypes,int nTypesCount );
BOOL WINAPI SetShieldResource(BOOL bSwitchOn);