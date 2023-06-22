#include "MemoryUtils/MemoryUtils.h"
#include "l4d2_gamedata.h"
#include "util.h"
#include "wrappers.h"

uintptr_t C_BaseEntity::m_updateVisibilityFn = NULL;
uintptr_t C_BasePlayer::m_getViewModelFn = NULL;
uintptr_t C_HLTVCameraWrapper::m_calcChaseCamViewFn = NULL;

CGlobalVars* g_pGlobals = NULL;
CTerrorGameRules** g_ppGameRules = NULL;

bool LoadSignatures(HMODULE clientdll)
{
	C_BaseEntity::m_updateVisibilityFn = UTIL_SignatureToAddress(clientdll, SIG_CBASEENTITY_UPDATEVISIBILITY);
	if (C_BaseEntity::m_updateVisibilityFn == NULL) {
		Error(VSP_LOG_PREFIX "Could not find signature for function 'C_BaseEntity::UpdateVisibility'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[C_BaseEntity::UpdateVisibility] Signature found: %x""\n", C_BaseEntity::m_updateVisibilityFn);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	C_BasePlayer::m_getViewModelFn = UTIL_SignatureToAddress(clientdll, SIG_CBASEPLAYER_GETVIEWMODEL);
	if (C_BasePlayer::m_getViewModelFn == NULL) {
		Error(VSP_LOG_PREFIX "Could not find signature for function 'C_BasePlayer::GetViewModel'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[C_BasePlayer::GetViewModel] Signature found: %x""\n", C_BasePlayer::m_getViewModelFn);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	C_HLTVCameraWrapper::m_calcChaseCamViewFn = UTIL_SignatureToAddress(clientdll, SIG_CHLTVCAMERA_CALCCHASECAMVIEW);
	if (C_HLTVCameraWrapper::m_calcChaseCamViewFn == NULL) {
		Error(VSP_LOG_PREFIX "Could not find signature for function 'C_HLTVCamera::CalcChaseCamView'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[C_HLTVCamera::CalcChaseCamView] Signature found: %x""\n", C_HLTVCameraWrapper::m_calcChaseCamViewFn);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// We use a pointer to a pointer, because this class may not be available when changing the map
	g_ppGameRules = *(CTerrorGameRules***)(UTIL_SignatureToAddress(clientdll, SIG_GAMERULES_PTR) + 2); // + 2 first bytes (8B 0D)
	if (g_ppGameRules == NULL) {
		Error(VSP_LOG_PREFIX "Failed to get pointer to instance 'CTerrorGameRules'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[CTerrorGameRules] Received instance: %x""\n", g_ppGameRules);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	CGlobalVars **ppGlobals = *(CGlobalVars***)(UTIL_SignatureToAddress(clientdll, SIG_GLOBALS_PTR) + 2); // + 2 first bytes (8B 0D) (dereference)
	if (ppGlobals == NULL) {
		Error(VSP_LOG_PREFIX "Failed to get pointer to instance 'CGlobalVars'""\n");

		return false;
	}

	g_pGlobals = *ppGlobals;
	if (g_pGlobals == NULL) {
		Msg("Failed to get pointer to class 'CGlobalVars': %x""\n", g_pGlobals);

		return false;
	}

	Msg(VSP_LOG_PREFIX "[CGlobalVars] Received instance: %x. MaxClients: %d ""\n", g_pGlobals, g_pGlobals->maxClients);

	return true;
}