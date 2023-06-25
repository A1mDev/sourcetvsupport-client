#include "wrappers.h"
#include "util.h"
#include "gamedata/gamedata.h"
#include "netproprs.h"
#include <string>

CGlobalVars* g_pGlobals = NULL;
CTerrorGameRules** g_ppGameRules = NULL;

uintptr_t C_BaseEntity::m_updateVisibilityFn = NULL;
uintptr_t C_BasePlayer::m_getViewModelFn = NULL;
uintptr_t C_HLTVCameraWrapper::m_calcChaseCamViewFn = NULL;

uint16_t C_BaseEntity::m_iSimulationTimeOffset = 0;
uint16_t C_BaseEntity::m_ifFlagsOffset = 0;
uint16_t C_BaseEntity::m_iLifeStateOffset = 0;
uint16_t C_BaseEntity::m_iVecViewOffsetOffset = 0;
uint16_t C_BaseEntity::m_iNetworkMoveParentOffset = 0;
uint16_t C_BaseEntity::m_iTeamNumOffset = 0;

uint16_t C_BasePlayer::m_iVecPunchAngleOffset = 0;

uint16_t C_CSPlayer::m_iProgressBarDurationOffset = 0;
uint16_t C_CSPlayer::m_iProgressBarStartTimeOffset = 0;

uint16_t C_TerrorPlayer::m_iHangingFromLedgeOffset = 0;
uint16_t C_TerrorPlayer::m_iHangingFromTongueOffset = 0;
uint16_t C_TerrorPlayer::m_iZombieClassOffset = 0;
uint16_t C_TerrorPlayer::m_iIncappedOffset = 0;
uint16_t C_TerrorPlayer::m_iJockeyAttackerOffset = 0;
uint16_t C_TerrorPlayer::m_iPounceVictimOffset = 0;

bool CheckPropOffsets()
{
	// Check if offsets that are not accessible to us(which are not netprops) have changed

	static const struct {
		const char* sPropName;
		uint16_t iPropOffset;
		const char* iNearestNetpropName;
		int16_t iNearestNetpropOffset;
	} s_offsets[] = {
		{ "CBaseEntity::index", OFF_CBASEENTITY_INDEX, "CBaseEntity::m_clrRender", -12 }, // 100 (CBaseEntity::m_clrRender) - 12 = 88 (CBaseEntity::index)
		{ "CBaseEntity::m_pMoveParent", OFF_CBASEENTITY_MOVEPARENT, "CBaseEntity::m_iParentAttachment", 12 }, // 504 (CBaseEntity::m_iParentAttachment) + 12 = 516 (CBaseEntity::m_pMoveParent)
	};

	for (auto&& el : s_offsets) {
		int iNetPropOffset = g_NetProps.GetOffset(el.iNearestNetpropName);

		if (iNetPropOffset == 0) {
			Error(VSP_LOG_PREFIX "Failed to check prop '%s' offset, nearest netprop '%s' not found""\n", \
						el.sPropName, el.iNearestNetpropName);

			return false;
		}

		if (iNetPropOffset + (int16_t)(el.iNearestNetpropOffset) != el.iPropOffset) {
			Error(VSP_LOG_PREFIX "Check the offset for prop '%s' it seems to have changed, the new possible offset is %d - %d - %d - %d""\n", \
				el.sPropName, iNetPropOffset + (int16_t)(el.iNearestNetpropOffset), iNetPropOffset, (int16_t)(el.iNearestNetpropOffset), el.iPropOffset);
		}
	}

	return true;
}

bool InitOffsets()
{
	if (!CheckPropOffsets()) {
		return false;
	}

	static const struct {
		std::string sPropName;
		uint16_t& iOffset;
	} s_offsets[] = {
		{ "CBaseEntity::m_flSimulationTime", C_BaseEntity::m_iSimulationTimeOffset },
		{ "CBasePlayer::m_fFlags", C_BaseEntity::m_ifFlagsOffset },
		{ "CBasePlayer::m_lifeState", C_BaseEntity::m_iLifeStateOffset },
		{ "CBasePlayer::m_vecViewOffset[0]", C_BaseEntity::m_iVecViewOffsetOffset },
		{ "CBaseEntity::moveparent", C_BaseEntity::m_iNetworkMoveParentOffset },
		{ "CBaseEntity::m_iTeamNum", C_BaseEntity::m_iTeamNumOffset },
		{ "CBasePlayer::m_vecPunchAngle", C_BasePlayer::m_iVecPunchAngleOffset },
#if SOURCE_ENGINE == SE_LEFT4DEAD2
		{ "CCSPlayer::m_flProgressBarDuration", C_CSPlayer::m_iProgressBarDurationOffset },
		{ "CTerrorPlayer::m_jockeyAttacker", C_TerrorPlayer::m_iJockeyAttackerOffset },
#else
		{ "CCSPlayer::m_iProgressBarDuration", C_CSPlayer::m_iProgressBarDurationOffset },
#endif
		{ "CCSPlayer::m_flProgressBarStartTime", C_CSPlayer::m_iProgressBarStartTimeOffset },
		{ "CTerrorPlayer::m_isHangingFromLedge", C_TerrorPlayer::m_iHangingFromLedgeOffset },
		{ "CTerrorPlayer::m_isHangingFromTongue", C_TerrorPlayer::m_iHangingFromTongueOffset },
		{ "CTerrorPlayer::m_zombieClass", C_TerrorPlayer::m_iZombieClassOffset },
		{ "CTerrorPlayer::m_isIncapacitated", C_TerrorPlayer::m_iIncappedOffset },
		{ "CTerrorPlayer::m_pounceVictim", C_TerrorPlayer::m_iPounceVictimOffset },
	};

	for (auto&& el : s_offsets) {
		el.iOffset = g_NetProps.GetOffset(el.sPropName);

		if (el.iOffset == 0) {
			Error(VSP_LOG_PREFIX "Cannot find specified offset '%s'""\n", el.sPropName);

			return false;
		}

		Msg(VSP_LOG_PREFIX "Received offset '%d' for netprop '%s'""\n", el.iOffset, el.sPropName);
	}

	return true;
}

bool InitFunctions(HMODULE clientdll)
{
	static const struct {
		const char* sFnName;
		const char* sSign;
		uintptr_t& pAddress;
	} s_sigs[] = {
		{ "C_BaseEntity::UpdateVisibility", SIG_CBASEENTITY_UPDATEVISIBILITY, C_BaseEntity::m_updateVisibilityFn },
		{ "C_BasePlayer::GetViewModel", SIG_CBASEPLAYER_GETVIEWMODEL, C_BasePlayer::m_getViewModelFn },
		{ "C_HLTVCamera::CalcChaseCamView", SIG_CHLTVCAMERA_CALCCHASECAMVIEW, C_HLTVCameraWrapper::m_calcChaseCamViewFn },
	};

	for (auto&& el : s_sigs) {
		el.pAddress = UTIL_SignatureToAddress(clientdll, el.sSign);

		if (el.pAddress == NULL) {
			Error(VSP_LOG_PREFIX "Could not find signature for function '%s'""\n", el.sFnName);

			return false;
		}

		Msg(VSP_LOG_PREFIX "[%s] Signature found: %x""\n", el.sFnName, el.pAddress);
	}

	return true;
}

bool InitGameRules(HMODULE clientdll)
{
	// We use a pointer to a pointer, because this class may not be available when changing the map
	
	g_ppGameRules = *(CTerrorGameRules***)(UTIL_SignatureToAddress(clientdll, SIG_GAMERULES_PTR) + 2); // + 2 first bytes (8B 0D)
	if (g_ppGameRules == NULL) {
		Error(VSP_LOG_PREFIX "Failed to get pointer to instance 'CTerrorGameRules'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[CTerrorGameRules] Received instance: %x""\n", g_ppGameRules);

	return true;
}

bool InitGlobals(HMODULE clientdll)
{
	CGlobalVars** ppGlobals = *(CGlobalVars***)(UTIL_SignatureToAddress(clientdll, SIG_GLOBALS_PTR) + 2); // + 2 first bytes (8B 0D) (dereference)
	if (ppGlobals == NULL) {
		Error(VSP_LOG_PREFIX "Failed to get pointer to instance 'CGlobalVars'""\n");

		return false;
	}

	g_pGlobals = *ppGlobals;
	if (g_pGlobals == NULL) {
		Error("Failed to get pointer to class 'CGlobalVars': %x""\n", g_pGlobals);

		return false;
	}

	Msg(VSP_LOG_PREFIX "[CGlobalVars] Received instance: %x. Realtime: %f ""\n", g_pGlobals, g_pGlobals->realtime);

	return true;
}

bool InitClassInstances(HMODULE clientdll)
{
	if (!InitGameRules(clientdll) || !InitGlobals(clientdll)) {
		return false;
	}

	return true;
}
