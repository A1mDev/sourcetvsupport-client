#ifndef _INCLUDE_FIXES_VSP_H_
#define _INCLUDE_FIXES_VSP_H_

#include "../CDetour/detours.h"
#include "convar.h"

class C_TerrorPlayer;

class CSetParentFix
{
public:
	bool CreateDetour(HMODULE clientdll);

	void DestroyDetour();

public:
	CDetour* m_DetourSetParent = NULL;
};

class CHLTVCameraFix
{
public:
	bool CreateDetour(HMODULE clientdll);

	void DestroyDetour();

	static const QAngle& GetPunchAngle(C_TerrorPlayer* pPlayer, bool bSurvIsIncapacitated);

public:
	CDetour* m_Detour_CalcInEyeCamView = NULL;
};

class CModelCrashFix
{
public:
	bool CreateDetour(HMODULE enginedll);

	void DestroyDetour();

public:
	CDetour* m_DetourFindOrCreateStaticPropColorData = NULL;
};

extern ConVar g_CvarSourceTVSendLocalTables;

#endif // _INCLUDE_FIXES_VSP_H_