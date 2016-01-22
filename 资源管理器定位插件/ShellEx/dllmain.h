// dllmain.h : 模块类的声明。

class CShellExModule : public CAtlDllModuleT< CShellExModule >
{
public :
	DECLARE_LIBID(LIBID_ShellExLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SHELLEX, "{06619718-86DE-4FE5-B13F-54FBD2D2B3B9}")
};

extern class CShellExModule _AtlModule;
