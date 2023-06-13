#include "fixes.h"
#include "wrappers.h"

// A fix has been found by a 'A1m`'
// This code exists in `server_srv.so` so we can easily replicate it.
// Functions `const Vector& GetPlayerViewOffset(CTerrorPlayer* pPlayer, bool bDucked)` and `void CTerrorGameMovement::DecayPunchAngle()`

// When we watch a demo or broadcast, the code is called like this:
// `CViewRender::SetUpView()->C_HLTVCamera::CalcView(..)->C_HLTVCamera::CalcInEyeCamView(...)`

// ## Bug description:
// 1) Wrong first person view offset when the player is incapacitated, 
//		our view offset is at the same height when the player is standing

// `C_BasePlayer::m_Local.m_vecPunchAngle` - This prop is not available to us without modifying the server.
// Need to add code to function `SendProxy_SendLocalDataTable` for server.

// Ñode ignores crosshair position during player incapacitation :
// ## Game code to fix

/*
// Client code:
void C_HLTVCamera::CalcInEyeCamView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov )
{
	static ConVarRef survivor_incapacitated_eye_height( "survivor_incapacitated_eye_height" );

	C_BasePlayer *pPlayer = UTIL_PlayerByIndex( m_iTraget1 );

	if ( !pPlayer )
		return;

	if ( !pPlayer->IsAlive() )
	{
		// if dead, show from 3rd person
		CalcChaseCamView( eyeOrigin, eyeAngles, fov );
		return;
	}

	m_aCamAngle = pPlayer->EyeAngles();
	m_vCamOrigin = pPlayer->GetAbsOrigin();
	m_flFOV = pPlayer->GetFOV();

	+ // Apply punch angle
	+ VectorAdd( m_aCamAngle, pPlayer->GetPunchAngle(), m_aCamAngle );

	+ bool bSurvIsIncapacitated = ( pPlayer->GetTeamNumber() == TEAM_SURVIVOR && pPlayer->IsIncapacitated()
	+ 							&& !pPlayer->IsHangingFromLedge() && !pPlayer->IsHangingFromTongue() );

	+ if ( bSurvIsIncapacitated )
	+ {
	+ 	Vector vecView = VEC_DUCK_VIEW;

	+ 	float fIncapEyeHeight = survivor_incapacitated_eye_height.GetFloat();
	+ 	float fProgressBurDuration = pPlayer->GetProgressBarPercent();

	+ 	vecView.z = ( fIncapEyeHeight * ( 1.0 - fProgressBurDuration ) ) + ( vecView.z * fProgressBurDuration );

	+ 	m_vCamOrigin += vecView;
	+ }
	else if ( pPlayer->GetFlags() & FL_DUCKING )
	{
		m_vCamOrigin += VEC_DUCK_VIEW;
	}
	else
	{
		m_vCamOrigin += VEC_VIEW;
	}

	eyeOrigin = m_vCamOrigin;
	eyeAngles = m_aCamAngle;
	fov = m_flFOV;

	pPlayer->CalcViewModelView( eyeOrigin, eyeAngles );

	C_BaseViewModel *pViewModel = pPlayer->GetViewModel( 0 );

	if ( pViewModel )
	{
		Assert( pViewModel->GetOwner() == pPlayer );
		pViewModel->UpdateVisibility();
	}
}

virtual QAngle C_BasePlayer::GetPunchAngle()
{
	return m_Local.m_vecPunchAngle.Get();
}

// Server code:
extern IHLTVServer *g_pHLTVServer;

void* SendProxy_SendLocalDataTable( const SendProp *pProp, const void *pStruct, const void *pVarData, CSendProxyRecipients *pRecipients, int objectID )
{
	pRecipients->SetOnly( objectID - 1 );

	+ // include the SourceTV client in the recipients for local data
	+ int hltvSlotIndex = g_pHLTVServer->GetHLTVSlot();
	+ if ( hltvSlotIndex != 0 )
	+ {
	+ 	pRecipients->SetRecipient( hltvSlotIndex );
	+ }

	return ( void * )pVarData;
}
REGISTER_SEND_PROXY_NON_MODIFIED_POINTER( SendProxy_SendLocalDataTable );
*/

#define VEC_DUCK_VIEW_CUSTOM	GetGameRules()->GetViewVectors()->m_vDuckView
#define VEC_VIEW_CUSTOM			GetGameRules()->GetViewVectors()->m_vView

ConVar g_CvarHLTVCameraFix("l4d2_hltvcamera_fix", "1", FCVAR_CLIENTDLL, "Toggle fix HLTVCamera. 0 - Disable, 1 - Enable.");
//ConVar g_CvarHLTVCameraFixDebug("l4d2_hltvcamera_fix_debug", "0", FCVAR_CLIENTDLL, "Enable HLTVCamera fix debug. 0 - Disable, 1 - Enable.");

DETOUR_DECL_MEMBER3(C_HLTVCamera__CalcInEyeCamView, void, Vector&, eyeOrigin, QAngle&, eyeAngles, float&, fov)
{
	if (!g_CvarHLTVCameraFix.GetBool()) {
		DETOUR_MEMBER_CALL(C_HLTVCamera__CalcInEyeCamView)(eyeOrigin, eyeAngles, fov); // call real function

		return;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	//bool bDebug = g_CvarHLTVCameraFixDebug.GetBool();
	static ConVarRef survivor_incapacitated_eye_height("survivor_incapacitated_eye_height");

	C_HLTVCameraWrapper* pThis = (C_HLTVCameraWrapper*)this;

	int m_iTarget1 = pThis->GetCameraTarget1Member();

	QAngle& m_aCamAngle = pThis->GetCamAngleMemberRef();
	Vector& m_vCamOrigin = pThis->GetCamOriginMemberRef();
	float& m_flFOV = pThis->GetFOVMemberRef();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*if (bDebug) {
		Msg(VSP_LOG_PREFIX "[C_HLTVCamera::CalcInEyeCamView] eyeOrigin: %f %f %f, eyeAngles: %f %f %f, fov %f""\n", \
				eyeOrigin.x, eyeOrigin.y, eyeOrigin.z, eyeAngles[0], eyeAngles[1], eyeAngles[2], fov);
	}*/

	C_TerrorPlayer* pPlayer = (C_TerrorPlayer*)UTIL_PlayerByIndex(m_iTarget1);

	if (!pPlayer) {
		return;
	}

	if (!pPlayer->IsAlive()) {
		// if dead, show from 3rd person
		pThis->CalcChaseCamView(eyeOrigin, eyeAngles, fov);

		return;
	}

	m_aCamAngle = pPlayer->EyeAngles();
	m_vCamOrigin = pPlayer->GetAbsOrigin();
	m_flFOV = pPlayer->GetFOV();

	bool bSurvIsIncapacitated = (pPlayer->GetTeamNumber() == TEAM_SURVIVOR && pPlayer->IsIncapacitated()
								&& !pPlayer->IsHangingFromLedge() && !pPlayer->IsHangingFromTongue());

	// Apply punch angle (cs go code)
	
	// Function `C_BasePlayer::GetPunchAngle` allows us to apply camera rotation while the player is incapacitated, 
	// otherwise we will have to implement this code.

	// The rotation of the camera at this point is related to cvar `survivor_incapacitated_roll`.
	// Also if we apply this netprop `m_Local.m_vecPunchAngle` other effects related to camera rotation or camera shake will be visible.
	// These effects are not visible in normal viewing in the spectator team.
	// For example, the recoil of a weapon during shooting, if a tank rock hits you, if a teammate shoots at you.
	// `C_BasePlayer::m_Local.m_vecPunchAngle` - This prop is not available to us without modifying the server.
	// Need to add code to function `SendProxy_SendLocalDataTable` for server.
	VectorAdd(m_aCamAngle, CHLTVCameraFix::GetPunchAngle(pPlayer, bSurvIsIncapacitated), m_aCamAngle);
	
	// This code exists in `server_srv.so` so we can easily replicate it, 
	// function `const Vector& GetPlayerViewOffset(CTerrorPlayer * pPlayer, bool bDucked)`.

	// Previously this code did not take into account the view offset during player incapacitation. 
	// View offset related to cvar `survivor_incapacitated_eye_height`.
	//
	// When a player dies while playing as a tank, the code also adds this view offset, 
	// since the tank is also incapacitated, we need to remove this effect for the tank.
	//
	// We can do it differently, we can try netprop `m_vecViewOffset` here,
	// it will have the same effect, but how correct is this?

	//VectorAdd(m_vCamOrigin, pPlayer->GetViewOffset(), m_vCamOrigin);

	if (bSurvIsIncapacitated) {
		Vector vecView = VEC_VIEW_CUSTOM;

		float fIncapEyeHeight = survivor_incapacitated_eye_height.GetFloat();
		float fProgressBurDuration = pPlayer->GetProgressBarPercent();

		vecView.z = (fIncapEyeHeight * (1.0 - fProgressBurDuration)) + (vecView.z * fProgressBurDuration);

		m_vCamOrigin += vecView;
	} else if (pPlayer->GetFlags() & FL_DUCKING) {
		m_vCamOrigin += VEC_DUCK_VIEW_CUSTOM;
	} else {
		m_vCamOrigin += VEC_VIEW_CUSTOM;
	}

	eyeOrigin = m_vCamOrigin;
	eyeAngles = m_aCamAngle;
	fov = m_flFOV;

	pPlayer->CalcViewModelView(eyeOrigin, eyeAngles);

	C_BaseViewModel* pViewModel = pPlayer->GetViewModel(0);
	if (pViewModel) {
		pViewModel->UpdateVisibility();
	}

	//DETOUR_MEMBER_CALL(C_HLTVCamera__CalcInEyeCamView)(eyeOrigin, eyeAngles, fov); // skip real function
}

const QAngle& CHLTVCameraFix::GetPunchAngle(C_TerrorPlayer* pPlayer, bool bSurvIsIncapacitated)
{
	static QAngle vecIncapPunchAngleDef(0.0f, 0.0f, 19.968750f);
	//static Vector vecPunchAngle;
	//static ConVarRef survivor_incapacitated_roll("survivor_incapacitated_roll");
	//static ConVarRef punch_angle_decay_rate("punch_angle_decay_rate");

	// We need to know if the server sends this data from local tables
	// If not, try to reproduce the code during incap

	// This code exists in `server_srv.so`, function `void CTerrorGameMovement::DecayPunchAngle()`
	if (g_CvarSourceTVSendLocalTables.GetBool()) {
		/*Msg(VSP_LOG_PREFIX "Server cvar 'tv_send_local_data_tables' enabled, netprop `C_BasePlayer::m_Local.m_vecPunchAngle` applied: %f %f %f""\n", \
				//pPlayer->GetPunchAngle().x, pPlayer->GetPunchAngle().y, pPlayer->GetPunchAngle().z);*/

		return pPlayer->GetPunchAngle();
	}

	if (!bSurvIsIncapacitated) {
		return vec3_angle;
	}

	// Netprop value `C_BasePlayer::m_Local.m_vecPunchAngle` during player incapacitation: 0.000000 0.000000 19.968750
	// Netprop `C_BasePlayer::m_Local.m_vecPunchAngle` not available here so we don't use
	//Msg(VSP_LOG_PREFIX "Server cvar 'tv_send_local_data_tables' disabled attempt to reproduce code during player incapacitation""\n");

	/*float fRoll = survivor_incapacitated_roll.GetFloat();
	if (fRoll == 0.0f) {
		return vec3_angle;
	}

	//vec_t fPunchAngleZ = m_pPlayer->GetPunchAngle().z;
	//QAngle vecPunchAngle(m_pPlayer->GetPunchAngle().x, m_pPlayer->GetPunchAngle().y, 0.0f);
	vec_t fPunchAngleZ = 0.0f;
	vecPunchAngle = Vector(0.0f, 0.0f, 0.0f);

	float fScale = VectorNormalize(vecPunchAngle);

	fScale = fScale - (fScale * 0.5f + punch_angle_decay_rate.GetFloat()) * g_pGlobals->frametime;
	fScale = MAX(fScale, 0.0f);
	vecPunchAngle *= fScale;

	vecPunchAngle.z = ( ( ( fRoll - fPunchAngleZ ) * g_pGlobals->frametime ) * 10.0f ) + fPunchAngleZ;

	Msg(VSP_LOG_PREFIX "Applied vector during player incapacitation. vecPunchAngle: %f %f %f, frametime: %f, survivor_incapacitated_roll: %f, punch_angle_decay_rate: %f""\n", \
			vecPunchAngle.x, vecPunchAngle.y, vecPunchAngle.z, g_pGlobals->frametime, fRoll, punch_angle_decay_rate.GetFloat());
		
	return *(QAngle*)(&vecPunchAngle);*/

	// The above code returns wrong value, we return default value during player incapacitation
	return vecIncapPunchAngleDef;
}

bool CHLTVCameraFix::CreateDetour(HMODULE clientdll)
{
	char sSignatureAddress[256];
	size_t iSigSize = UTIL_StringToSignature(SIG_CHLTVCAMERA_CALCINEYECAMVIEW, sSignatureAddress, sizeof(sSignatureAddress));

	void* C_HLTVCamera__CalcInEyeCamView_pfn = g_MemUtils.FindPattern(clientdll, sSignatureAddress, iSigSize);
	if (!C_HLTVCamera__CalcInEyeCamView_pfn) {
		Error(VSP_LOG_PREFIX "Failed to find signature 'C_HLTVCamera::CalcInEyeCamView'. Please contact the author""\n");

		return false;
	}

	m_Detour_CalcInEyeCamView = DETOUR_CREATE_MEMBER(C_HLTVCamera__CalcInEyeCamView, C_HLTVCamera__CalcInEyeCamView_pfn);
	if (!m_Detour_CalcInEyeCamView) {
		Error(VSP_LOG_PREFIX "Could not obtain signature for 'C_HLTVCamera::CalcInEyeCamView'""\n");

		return false;
	}

	m_Detour_CalcInEyeCamView->EnableDetour();

	Msg(VSP_LOG_PREFIX "[C_HLTVCamera::CalcInEyeCamView] Enable detour. Detour ptr: %x, func ptr: %x, sig size: %d""\n", \
		m_Detour_CalcInEyeCamView, C_HLTVCamera__CalcInEyeCamView_pfn, iSigSize);
	
	return true;
}

void CHLTVCameraFix::DestroyDetour()
{
	if (m_Detour_CalcInEyeCamView) {
		m_Detour_CalcInEyeCamView->DisableDetour();
		m_Detour_CalcInEyeCamView = NULL;
	}
}