#ifndef _INCLUDE_WRAPPERS_VSP_H_
#define _INCLUDE_WRAPPERS_VSP_H_

#include "vsp_client.h"
#include "gamedata/gamedata.h"
#include "l4d_sdk/constants.h"
#include "ehandle.h"
#include "icliententitylist.h"
#include "iclientunknown.h"
#include "iclientnetworkable.h"
#include "const.h" // LIFE_ALIVE

#include "igameevents.h" // for include hltvcamera.h
#include "l4d_sdk/GameEventListener.h" // for include hltvcamera.h, and l4d1 fix
#include "usercmd.h" // for include hltvcamera.h
extern IGameEventManager2* gameeventmanager; // Need for hltvcamera.h
#include "hltvcamera.h"

#include "shareddefs.h"
//#include "itoolentity.h"
#include "l4d_sdk/model.h"
#include "netproprs.h"
#include "ienginetool.h"

class C_BaseEntity;
class C_BaseViewModel;
class CTerrorGameRules;

extern IClientEntityList* g_pClientEntityList;
extern CGlobalVars* g_pGlobals;
extern CTerrorGameRules** g_ppGameRules;
//extern IClientTools* g_pClientTools;

typedef CHandle<C_BaseEntity> EHANDLE; // The client's version of EHANDLE.

template <typename fn>
__forceinline fn vfunc(void* classbase, int func_index)
{
	return (*reinterpret_cast<fn**>(classbase))[func_index];
}

inline IClientEntityList* ClientEntityList();

class C_BaseEntity
{
public:
	inline EHANDLE GetMoveParentMember()
	{
		// C_BaseEntity::m_pMoveParent
		return *(EHANDLE*)((byte*)(this) + OFF_CBASEENTITY_MOVEPARENT);
	}

	inline EHANDLE &GetNetworkMoveParentMemberRef()
	{
		// C_BaseEntity::m_hNetworkMoveParent
		return *(EHANDLE*)((byte*)(this) + m_iNetworkMoveParentOffset);
	}

	inline bool IsServerEntity() const
	{
		return (GetIndexMember() != -1);
	}

	inline int GetIndexMember() const
	{
		// C_BaseEntity::index

		//Msg("C_BaseEntity::index: %d""\n", *(int*)((byte*)(this) + OFF_CBASEENTITY_INDEX));
		return *(int*)((byte*)(this) + OFF_CBASEENTITY_INDEX);
	}

	inline bool IsPlayer() /*const*/
	{
		return vfunc<bool(__thiscall*)(C_BaseEntity*)>(this, VTB_OFF_CBASEENTITY_ISPLAYER)(this);
	}

	inline bool IsAlive() const
	{
		// C_BaseEntity::m_lifeState
		char m_lifeState = *(char*)((byte*)(this) + m_iLifeStateOffset);
		//Msg("C_BaseEntity::m_lifeState: %d, offset: %d""\n", m_lifeState, m_iLifeStateOffset);
		return m_lifeState == LIFE_ALIVE;
	}

	inline int GetFlags() const
	{
		// C_BaseEntity::m_fFlags
		//Msg("C_BaseEntity::m_fFlags: %d, offset: %d""\n", *(int*)((byte*)(this) + m_ifFlagsOffset), m_ifFlagsOffset);
		return *(int*)((byte*)(this) + m_ifFlagsOffset);
	}

	inline void UpdateVisibility()
	{
		((void(__thiscall*)(C_BaseEntity*))(m_updateVisibilityFn))(this);
	}

	inline const Vector& GetAbsOrigin() /*const*/
	{
		return vfunc<const Vector& (__thiscall*)(C_BaseEntity*)>(this, VTB_OFF_CBASEENTITY_GETABSORIGIN)(this);
	}

	inline float GetSimulationTime() const
	{
		// C_BaseEntity::m_flSimulationTime
		//Msg("C_BaseEntity::GetSimulationTime: %f, offset: %d""\n", *(float*)((byte*)(this) + m_iSimulationTimeOffset), m_iSimulationTimeOffset);
		return *(float*)((byte*)(this) + m_iSimulationTimeOffset);
	}

	int GetTeamNumber() const
	{
		// C_BaseEntity::m_iTeamNum
		//Msg("C_BaseEntity::m_iTeamNum: %d""\n", *(int*)((byte*)(this) + m_iTeamNumOffset));
		return *(int*)((byte*)(this) + m_iTeamNumOffset);
	}

	const Vector& GetViewOffset() const
	{
		// C_BaseEntity::m_vecViewOffset
		//Vector m_vecViewOffset = *(Vector*)((byte*)(this) + m_iVecViewOffsetOffset);
		//Msg("C_BaseEntity::m_vecViewOffset: %f %f %f""\n", m_vecViewOffset.x, m_vecViewOffset.y, m_vecViewOffset.z);
		return *(Vector*)((byte*)(this) + m_iVecViewOffsetOffset);
	}

	/*char const* GetClassname()
	{
		int iEntIndex = g_pClientTools->GetEntIndex(this);
		Msg("[C_BaseEntity::GetClassname] EntIndex: %d, maxEntities: %d""\n", iEntIndex, g_pGlobals->maxEntities);

		if (iEntIndex < 0 || g_pGlobals->maxEntities) {
			return "Unknown";
		}

		HTOOLHANDLE curHandle = g_pClientTools->GetToolHandleForEntityByIndex(iEntIndex);
		if (curHandle != 0) {
			return "Unknown";
		}

		return g_pClientTools->GetClassname(curHandle);
	}*/

public:
	static uintptr_t m_updateVisibilityFn;

	static uint16_t m_iSimulationTimeOffset;
	static uint16_t m_ifFlagsOffset;
	static uint16_t m_iLifeStateOffset;
	static uint16_t m_iNetworkMoveParentOffset;
	static uint16_t m_iVecViewOffsetOffset;
	static uint16_t m_iTeamNumOffset;
};

class C_BasePlayer :
	public C_BaseEntity
{
public:
	inline C_BaseViewModel* GetViewModel(int iViewModelIndex = 0) const
	{
		return ((C_BaseViewModel* (__thiscall*)(const C_BasePlayer*, int))(m_getViewModelFn))(this, iViewModelIndex);
	}

	inline void CalcViewModelView(const Vector& eyeOrigin, const QAngle& eyeAngles)
	{
		vfunc<void(__thiscall*)(C_BasePlayer*, const Vector&, const QAngle&)>(this, VTB_OFF_CBASEPLAYER_CALCVIEWMODELVIEW)(this, eyeOrigin, eyeAngles);
	}

	inline float GetFOV() /*const*/
	{
		return vfunc<float(__thiscall*)(C_BasePlayer*)>(this, VTB_OFF_CBASEPLAYER_GETFOV)(this);
	}

	inline const QAngle& EyeAngles() /*const*/
	{
		return vfunc<const QAngle& (__thiscall*)(C_BasePlayer*)>(this, VTB_OFF_CBASEPLAYER_EYEANGLES)(this);
	}
	
	inline const QAngle& GetPunchAngle()
	{
		// C_BasePlayer::m_Local.m_vecPunchAngle
		// m_Local.m_vecPunchAngle.Get();

		//QAngle m_vecPunchAngle = *(QAngle*)((byte*)(this) + m_iVecPunchAngleOffset);
		//Msg("C_BasePlayer::m_Local.m_vecPunchAngle: %f %f %f""\n", m_vecPunchAngle.x, m_vecPunchAngle.y, m_vecPunchAngle.z);
		return *(QAngle*)((byte*)(this) + m_iVecPunchAngleOffset);
	}

public:
	static uintptr_t m_getViewModelFn;

	static uint16_t m_iVecPunchAngleOffset;
};

class C_CSPlayer :
	public C_BasePlayer
{
public:
	inline float GetProgressBarStartTime()
	{
		// C_CSPlayer::m_flProgressBarStartTime
		//Msg("C_CSPlayer::m_flProgressBarStartTime: %f""\n", *(float*)((byte*)(this) + m_iProgressBarStartTimeOffset));

		return *(float*)((byte*)(this) + m_iProgressBarStartTimeOffset);
	}

	inline float GetProgressBarDuration()
	{
		// l4d2 C_CSPlayer::m_flProgressBarDuration
		// l4d1 C_CSPlayer:: m_iProgressBarDuration
#if SOURCE_ENGINE == SE_LEFT4DEAD2
		//Msg("C_CSPlayer::m_flProgressBarDuration: %f""\n", *(float*)((byte*)(this) + m_iProgressBarDurationOffset));
		return *(float*)((byte*)(this) + m_iProgressBarDurationOffset);
#else
		//Msg("C_CSPlayer::m_iProgressBarDuration: %f""\n", float(*(int*)((byte*)(this) + m_iProgressBarDurationOffset)));
		return float(*(int*)((byte*)(this) + m_iProgressBarDurationOffset));
#endif
	}

	static uint16_t m_iProgressBarDurationOffset;
	static uint16_t m_iProgressBarStartTimeOffset;
};

class C_TerrorPlayer :
	public C_CSPlayer
{
public:
	inline C_TerrorPlayer* GetPounceVictim()
	{
		// C_TerrorPlayer::m_pounceVictim

		EHANDLE m_pounceVictim = *(EHANDLE*)((byte*)(this) + m_iPounceVictimOffset);
		return (C_TerrorPlayer*)ClientEntityList()->GetClientEntityFromHandle(m_pounceVictim);
	}

	inline C_TerrorPlayer* GetJockeyAttacker()
	{
		// C_TerrorPlayer::m_jockeyAttacker

		EHANDLE m_jockeyAttacker = *(EHANDLE*)((byte*)(this) + m_iJockeyAttackerOffset);
		return (C_TerrorPlayer*)ClientEntityList()->GetClientEntityFromHandle(m_jockeyAttacker);
	}

	inline bool IsIncapacitated()
	{
		if (IsAlive()) {
			// C_TerrorPlayer::m_isIncapacitated
			//Msg("C_TerrorPlayer::m_isIncapacitated: %d""\n", *(bool*)((byte*)(this) + m_iIncappedOffset));

			return *(bool*)((byte*)(this) + m_iIncappedOffset);
		}

		return false;
	}

	// This code exists in 'server_srv.so' so we can easily replicate it.
	float GetProgressBarPercent()
	{
		if (!IsProgressBarActive()) {
			return 0.0f;
		}

		if (GetProgressBarDuration() + GetProgressBarStartTime() < GetSimulationTime()) {
			return 1.0f;
		}

		return (GetSimulationTime() - GetProgressBarStartTime()) / GetProgressBarDuration();
	}

	// This code exists in 'server_srv.so' so we can easily replicate it.
	bool IsProgressBarActive()
	{
		if (GetProgressBarDuration() < 0.0f) {
			return false;
		}

		//Msg("g_pGlobals->curtime: %f""\n", g_pGlobals->curtime);
		return (IsA(zClass_Hunter) || (GetProgressBarDuration() + GetProgressBarStartTime() >= g_pGlobals->curtime));
	}

	inline ZombieClassType GetClass()
	{
		// C_TerrorPlayer::m_zombieClass
		//Msg("C_TerrorPlayer::GetClass: %d""\n", *(ZombieClassType*)((byte*)(this) + m_iZombieClassOffset));

		return *(ZombieClassType*)((byte*)(this) + m_iZombieClassOffset);
	}

	inline bool IsA(ZombieClassType zClass)
	{
		return (GetTeamNumber() == TEAM_ZOMBIE && GetClass() == zClass);
	}

	inline bool IsHangingFromLedge()
	{
		// C_TerrorPlayer::m_isHangingFromLedge
		//Msg("C_TerrorPlayer::IsHangingFromLedge: %d""\n", *(bool*)((byte*)(this) + m_iHangingFromLedgeOffset));

		return *(bool*)((byte*)(this) + m_iHangingFromLedgeOffset);
	}

	inline bool IsHangingFromTongue()
	{
		// C_TerrorPlayer::m_isHangingFromTongue
		//Msg("C_TerrorPlayer::IsHangingFromTongue: %d""\n", *(bool*)((byte*)(this) + m_iHangingFromTongueOffset));

		return *(bool*)((byte*)(this) + m_iHangingFromTongueOffset);
	}

public:
	static uint16_t m_iPounceVictimOffset;
	static uint16_t m_iJockeyAttackerOffset;

	static uint16_t m_iIncappedOffset;
	static uint16_t m_iZombieClassOffset;
	static uint16_t m_iHangingFromLedgeOffset;
	static uint16_t m_iHangingFromTongueOffset;
};

class C_BaseViewModel :
	public C_BaseEntity
{
public:
	// 
};

class C_HLTVCameraWrapper :
	public C_HLTVCamera
{
public:
	inline void CalcChaseCamView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
	{
		((void(__thiscall*)(C_HLTVCamera*, Vector&, QAngle&, float&))(m_calcChaseCamViewFn))(this, eyeOrigin, eyeAngles, fov);
	}

	inline int GetCameraTarget1Member()
	{
		return this->m_iTraget1;
	}

	inline QAngle& GetCamAngleMemberRef()
	{
		return this->m_aCamAngle;
	}

	inline Vector& GetCamOriginMemberRef()
	{
		return this->m_vCamOrigin;
	}

	inline float& GetFOVMemberRef()
	{
		return this->m_flFOV;
	}

public:
	static uintptr_t m_calcChaseCamViewFn;
};

class IBaseClientDLLWrapper
{
public:
	// 'hl2sdk-l4d2' and 'hl2sdk-l4d1' has the wrong vtable table 'IBaseClientDLL' =(
	inline ClientClass* GetAllClasses()
	{
		return vfunc<ClientClass* (__thiscall*)(IBaseClientDLLWrapper*)>(this, VTB_OFF_IBASECLIENTDLL_GETALLCLASSES)(this);
	}
};

class IEngineToolWrapper :
	public IEngineTool
{
public:
	// 'hl2sdk-l4d2' has the wrong vtable table 'IEngineTool' =(
	inline void GetClientFactory(CreateInterfaceFn& factory)
	{
#if SOURCE_ENGINE == SE_LEFT4DEAD2
		vfunc<void(__thiscall*)(IEngineToolWrapper*, CreateInterfaceFn&)>(this, VTB_OFF_IVENGINETOOL_GETCLIENTFACTORY)(this, factory);
#else 
		((IEngineTool*)this)->GetClientFactory(factory);
#endif
	}
};

class CColorMeshData
{
public:
	inline CColorMeshData* GetData()
	{
		return this;
	}
};

class CModelRenderWrapper :
	public CModelRender
{
public:
	// Is inside the inherited class 'IDataCacheClient'
	inline CColorMeshData* CacheGet(DataCacheHandle_t handle, bool bFrameLock = true)
	{
		//return vfunc<CColorMeshData*(__thiscall*)(IDataCacheSection*, DataCacheHandle_t, bool)>(m_pCache, VTB_OFF_CMODELRENDER_CACHEGET)(m_pCache, handle, bFrameLock);
		return (CColorMeshData*)(((CColorMeshData*)m_pCache->Get(handle, bFrameLock))->GetData());
	}
};

class CTerrorGameRules
{
public:
	const CViewVectors* GetViewVectors() /*const*/
	{
		return vfunc<const CViewVectors*(__thiscall*)(CTerrorGameRules*)>(this, VTB_OFF_CTERRORGAMERULES_GETVIEWVECTORS)(this);
	}
};

//-----------------------------------------------------------------------------
// Inline methods
//-----------------------------------------------------------------------------

inline CTerrorGameRules* GetGameRules(bool bMsg = true)
{
	CTerrorGameRules* pGameRules = *g_ppGameRules;  // (dereference)

	// Map not loaded yet this class is not available.
	if (pGameRules == NULL) {
		if (bMsg) {
			Msg("Failed to get pointer to class 'CTerrorGameRules': %x""\n", pGameRules);
		}

		return NULL;
	}

	return pGameRules;
}

inline IClientEntityList* ClientEntityList()
{
	return g_pClientEntityList;
}

inline C_BasePlayer* ToBasePlayer(C_BaseEntity* pEntity)
{
	if (pEntity == NULL || !pEntity->IsPlayer()) {
		return NULL;
	}

	return (C_BasePlayer*)(pEntity);
}

inline C_BasePlayer* UTIL_PlayerByIndex(int iEntIndex)
{
	if (iEntIndex < 1 || iEntIndex > g_pGlobals->maxClients) {
		return NULL;
	}

	IClientNetworkable* pNet = ClientEntityList()->GetClientNetworkable(iEntIndex);
	if (pNet == NULL) {
		return NULL;
	}

	IClientUnknown* pUnk = pNet->GetIClientUnknown();
	if (pUnk == NULL) {
		return NULL;
	}

	return ToBasePlayer(pUnk->GetBaseEntity());
}

/*inline C_BaseEntity* UTIL_EntityByIndex(int iEntIndex)
{
	if (iEntIndex < 0 || iEntIndex >= MAX_EDICTS) {
		return NULL;
	}

	IClientNetworkable* pNet = ClientEntityList()->GetClientNetworkable(iEntIndex);
	if (pNet  == NULL) {
		return NULL;
	}

	IClientUnknown* pUnk = pNet->GetIClientUnknown();
	if (pUnk  == NULL) {
		return NULL;
	}

	return pUnk->GetBaseEntity();
}*/

#endif // _INCLUDE_WRAPPERS_VSP_H_