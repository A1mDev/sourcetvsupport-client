#ifndef _INCLUDE_FIXES_VSP_H_
#define _INCLUDE_FIXES_VSP_H_

#include "../includes/CDetour/detours.h"
#include "convar.h"

class C_TerrorPlayer;

class CSetParentFix
{
public:
	bool CreateDetour();

	void DestroyDetour();

public:
	CDetour* m_DetourSetParent = NULL;
};

class CHLTVCameraFix
{
public:
	bool CreateDetour();

	void DestroyDetour();

	static const QAngle& GetIncapPunchAngle(C_TerrorPlayer* pPlayer);

public:
	CDetour* m_Detour_CalcInEyeCamView = NULL;
};

class CModelCrashFix
{
public:
	bool CreateDetour();

	void DestroyDetour();

public:
	CDetour* m_DetourFindOrCreateStaticPropColorData = NULL;
};

extern ConVar g_CvarSourceTVSendLocalTables;

#endif // _INCLUDE_FIXES_VSP_H_