#ifndef _INCLUDE_FIXES_VSP_H_
#define _INCLUDE_FIXES_VSP_H_

#include "../CDetour/detours.h"

class CSetParentFix
{
public:
	bool CreateDetour(HMODULE clientdll);

	void DestroyDetour();

public:
	CDetour* m_DetourSetParent = NULL;
};

#endif // _INCLUDE_FIXES_VSP_H_