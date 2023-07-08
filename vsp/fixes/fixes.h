#ifndef _INCLUDE_FIXES_VSP_H_
#define _INCLUDE_FIXES_VSP_H_

#include "../includes/CDetour/detours.h"
#include "convar.h"
#include "util.h"

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

class CGhostCCFix
{
public:
	bool CreatePatch();
	void DestroyPatch();

	bool EnablePatch();
	bool DisablePatch();

	static void OnGhostPatchStateChanged(IConVar* var, const char* pOldValue, float flOldValue);

public:
	uintptr_t m_patchedAddr = PTR_NULL;

	bool m_bPatchEnabled = false;

	static bool m_bPatchAddrChecked;
};

extern ConVar g_CvarSourceTVSendLocalTables;
extern CGhostCCFix g_GhostCCFix;

#endif // _INCLUDE_FIXES_VSP_H_