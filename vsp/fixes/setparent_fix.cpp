#include "fixes.h"
#include "vsp_client.h"
#include "wrappers.h"

// A fix has been found by a 'shqke', wrote fix code 'A1m`'.

// ## Bug description:
// 1) Missing arms models/blinking world model attachments during SourceTV footage playback
// 2) Various sounds and effects (for example, explosions, destruction of the tank rock, etc) appearing at zero coordinates on the map
// 3) Missing body parts from infected commons, these body parts appeared at zero coordinates on the map.

// ## Game code to fix
#if 0
//-----------------------------------------------------------------------------
// Unlinks from hierarchy
//-----------------------------------------------------------------------------
void C_BaseEntity::SetParent(C_BaseEntity* pParentEntity, int iParentAttachment)
{
	// NOTE: This version is meant to be called *outside* of PostDataUpdate
	// as it assumes the moveparent has a valid handle
	EHANDLE newParentHandle;
	newParentHandle.Set(pParentEntity);
	if (newParentHandle.ToInt() == m_pMoveParent.ToInt())
		return;

	// NOTE: Have to do this before the unlink to ensure local coords are valid
	Vector vecAbsOrigin = GetAbsOrigin();
	QAngle angAbsRotation = GetAbsAngles();
	Vector vecAbsVelocity = GetAbsVelocity();

	// First deal with unlinking
	if (m_pMoveParent.IsValid())
	{
		UnlinkChild(m_pMoveParent, this);
	}

	if (pParentEntity)
	{
		LinkChild(pParentEntity, this);
	}
	
	+ if (!IsServerEntity())
	+ {
	+ 	m_hNetworkMoveParent = pParentEntity;
	+ }

	m_iParentAttachment = iParentAttachment;

	m_vecAbsOrigin.Init(FLT_MAX, FLT_MAX, FLT_MAX);
	m_angAbsRotation.Init(FLT_MAX, FLT_MAX, FLT_MAX);
	m_vecAbsVelocity.Init(FLT_MAX, FLT_MAX, FLT_MAX);

	SetAbsOrigin(vecAbsOrigin);
	SetAbsAngles(angAbsRotation);
	SetAbsVelocity(vecAbsVelocity);
}
#endif

ConVar g_CvarSetParentFix("l4d2_setparent_fix", "1", FCVAR_CLIENTDLL, "Toggle fix SetParent. 0 - Disable, 1 - Enable.");
ConVar g_CvarSetParentFixDebug("l4d2_setparent_fix_debug", "0", FCVAR_CLIENTDLL, "Enable SetParent fix debug. 0 - Disable, 1 - Enable.");

DETOUR_DECL_MEMBER2(C_BaseEntity__SetParent, void, C_BaseEntity*, pParentEntity, int, iParentAttachment)
{
	C_BaseEntity* pThis = (C_BaseEntity*)this;
	bool bDebug = g_CvarSetParentFixDebug.GetBool();

	CL_EHANDLE newParentHandle;
	newParentHandle.Set(pParentEntity);
	CL_EHANDLE& m_pMoveParent = pThis->GetMoveParentMember();

	/*if (bDebug) {
		Msg(VSP_LOG_PREFIX "[C_BaseEntity__SetParent][1] This: %x, pParentEntity: %x, iParentAttachment: %d, index: %d""\n", \
											this, pParentEntity, iParentAttachment, pThis->GetIndex());
	}*/

	if (newParentHandle.ToInt() == m_pMoveParent.ToInt()) {
		/*if (bDebug) {
			Msg(VSP_LOG_PREFIX "[C_BaseEntity::SetParent][2] Var 'newParentHandle: %d' is equal to 'm_pMoveParent: %d'""\n", \
				newParentHandle.ToInt(), m_pMoveParent.ToInt());
		}*/
		
		DETOUR_MEMBER_CALL(C_BaseEntity__SetParent)(pParentEntity, iParentAttachment); // call real function
		return;
	}

	DETOUR_MEMBER_CALL(C_BaseEntity__SetParent)(pParentEntity, iParentAttachment); // call real function
	
	if (!g_CvarSetParentFix.GetBool()) {
		return;
	}

	if (bDebug) {
		Msg(VSP_LOG_PREFIX "[C_BaseEntity__SetParent][3] This: %x, pParentEntity: %x, iParentAttachment: %d, index: %d""\n", \
			this, pParentEntity, iParentAttachment, pThis->GetIndexMember());
		Msg(VSP_LOG_PREFIX "[C_BaseEntity::SetParent][4] Var 'newParentHandle: %d' is not equal to 'm_pMoveParent: %d'""\n", \
			newParentHandle.ToInt(), m_pMoveParent.ToInt());
	}

	CL_EHANDLE& m_hNetworkMoveParent = pThis->GetNetworkMoveParentMemberRef();

	if (!pThis->IsServerEntity()) {
		int iOldValue = m_hNetworkMoveParent.ToInt();
		m_hNetworkMoveParent = pParentEntity;

		if (bDebug) {
			Msg(VSP_LOG_PREFIX "[C_BaseEntity::SetParent][5] Set m_hNetworkMoveParent: %d, iOldValue: %d""\n", \
				m_hNetworkMoveParent.ToInt(), iOldValue);
		}
	}
}

bool CSetParentFix::CreateDetour(HMODULE clientdll)
{
	size_t iSigSize = 0;
	uintptr_t C_BaseEntity__SetParent_pfn = UTIL_SignatureToAddress(clientdll, SIG_CBASEENTITY_SETPARENT, &iSigSize);
	if (C_BaseEntity__SetParent_pfn == NULL) {
		Error(VSP_LOG_PREFIX "Failed to find signature 'C_BaseEntity::SetParent'. Please contact the author""\n");

		return false;
	}

	m_DetourSetParent = DETOUR_CREATE_MEMBER(C_BaseEntity__SetParent, (void*)C_BaseEntity__SetParent_pfn);
	if (m_DetourSetParent == NULL) {
		Error(VSP_LOG_PREFIX "Could not obtain signature for 'C_BaseEntity::SetParent'""\n");

		return false;
	}

	m_DetourSetParent->EnableDetour();

	Msg(VSP_LOG_PREFIX "[C_BaseEntity::SetParent] Enable detour. Detour ptr: %x, func ptr: %x, sig size: %d""\n", \
			m_DetourSetParent, C_BaseEntity__SetParent_pfn, iSigSize);

	return true;
}

void CSetParentFix::DestroyDetour()
{
	if (m_DetourSetParent != NULL) {
		m_DetourSetParent->DisableDetour();
		m_DetourSetParent = NULL;
	}
}