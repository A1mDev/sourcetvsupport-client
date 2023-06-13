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
	char sSignatureAddress[256];
	size_t iSigSize = UTIL_StringToSignature(SIG_CBASEENTITY_UPDATEVISIBILITY, sSignatureAddress, sizeof(sSignatureAddress));

	C_BaseEntity::m_updateVisibilityFn = uintptr_t(g_MemUtils.FindPattern(clientdll, sSignatureAddress, iSigSize));
	if (!C_BaseEntity::m_updateVisibilityFn) {
		Error(VSP_LOG_PREFIX "Could not find signature for function 'C_BaseEntity::UpdateVisibility'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[C_BaseEntity::UpdateVisibility] Signature found: %x""\n", C_BaseEntity::m_updateVisibilityFn);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	iSigSize = UTIL_StringToSignature(SIG_CBASEPLAYER_GETVIEWMODEL, sSignatureAddress, sizeof(sSignatureAddress));

	C_BasePlayer::m_getViewModelFn = uintptr_t(g_MemUtils.FindPattern(clientdll, sSignatureAddress, iSigSize));
	if (!C_BasePlayer::m_getViewModelFn) {
		Error(VSP_LOG_PREFIX "Could not find signature for function 'C_BasePlayer::GetViewModel'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[C_BasePlayer::GetViewModel] Signature found: %x""\n", C_BasePlayer::m_getViewModelFn);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	iSigSize = UTIL_StringToSignature(SIG_CHLTVCAMERA_CALCCHASECAMVIEW, sSignatureAddress, sizeof(sSignatureAddress));

	C_HLTVCameraWrapper::m_calcChaseCamViewFn = uintptr_t(g_MemUtils.FindPattern(clientdll, sSignatureAddress, iSigSize));
	if (!C_HLTVCameraWrapper::m_calcChaseCamViewFn) {
		Error(VSP_LOG_PREFIX "Could not find signature for function 'C_HLTVCamera::CalcChaseCamView'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[C_HLTVCamera::CalcChaseCamView] Signature found: %x""\n", C_HLTVCameraWrapper::m_calcChaseCamViewFn);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	iSigSize = UTIL_StringToSignature(SIG_GAMERULES_PTR, sSignatureAddress, sizeof(sSignatureAddress));
	
	uintptr_t pGameRulesPattern = uintptr_t(g_MemUtils.FindPattern(clientdll, sSignatureAddress, iSigSize));
	
	g_ppGameRules = *(CTerrorGameRules***)(pGameRulesPattern + 2); // + 2 first bytes (8B 0D)
	if (!g_ppGameRules) {
		Error(VSP_LOG_PREFIX "Failed to get pointer to instance 'CTerrorGameRules'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[CTerrorGameRules] Received instance: %x""\n", g_ppGameRules);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	iSigSize = UTIL_StringToSignature(SIG_GLOBALS_PTR, sSignatureAddress, sizeof(sSignatureAddress));

	uintptr_t pGlobalsPattern = uintptr_t(g_MemUtils.FindPattern(clientdll, sSignatureAddress, iSigSize));

	CGlobalVars **ppGlobals = *(CGlobalVars***)(pGlobalsPattern + 2); // + 2 first bytes (8B 0D)
	if (!ppGlobals) {
		Error(VSP_LOG_PREFIX "Failed to get pointer to instance 'CGlobalVars'""\n");

		return false;
	}

	g_pGlobals = *ppGlobals;
	if (!g_pGlobals) {
		Msg("Failed to get pointer to class 'CGlobalVars': %x""\n", g_pGlobals);

		return NULL;
	}

	Msg(VSP_LOG_PREFIX "[CGlobalVars] Received instance: %x. MaxClients: %d ""\n", g_pGlobals, g_pGlobals->maxClients);

	return true;
}