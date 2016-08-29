#include <hash_map>
using namespace std;
using namespace stdext;

typedef hash_map<DWORD,CStringA> NET_HASH_MAP,*PNET_HASH_MAP;
typedef NET_HASH_MAP::iterator NET_HASH_MAP_PTR;

typedef hash_map<char,NET_HASH_MAP> NET_HASH_MAP_ROOT,*PNET_HASH_MAP_ROOT;
typedef NET_HASH_MAP_ROOT::iterator NET_HASH_MAP_ROOT_PTR;


BOOL ParseListFile(LPCWSTR pszFilePath,PNET_HASH_MAP_ROOT pMap);
BOOL FastMatchRecord(LPCSTR pchDomain);
VOID StartParseDataFile( LPCWSTR pszFilePath );
