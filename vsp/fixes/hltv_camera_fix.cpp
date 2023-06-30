#include "fixes.h"
#include "wrappers.h"

// Wrote fix code and found fix 'A1m`'.

// In the game l4d1 and l4d2 the same problems with the camera when watching a demo or stream from sourceTV. 
// The problem exists with both camera offset and rotation. The problem is that the code inside class 'C_HLTVCamera' never adapted to games l4d1 and l4d2
// and many cases of camera offset or rotation are ignored.
// A few examples (maybe some other cases):
// 1) When you are a jockey, hunter or smoker and you attack a survivor (same for survivor).
// 2) When you are a survivor and use a first aid kit.
// 3) When you play as a tankand pick up a rock.
// 4) When you are a tank and the death animation is playing.
// 5) Wrong first person view offset when the player is incapacitated, our view offset is at the same height when the player is standing.
//
// At this point, the first person view changes to a 3rd person view with different camera offsets.
// We see the wrong first person animation because the view should not be in first person 
// and in these cases the animation was never adapted to first person (not in all cases).

// We will call a function that handles all or part of these cases and play the code with camera rotation when the player is incapacitated, 
// if the server does not send us data from the local table. I don't know what other cases of camera rotation are so noticeable other than this.

// Part of the code is on the server in library 'server_srv.so' l4d2 and library 'server.so' l4d1, we can reproduce this code.
// The code in game l4d2 and in game l4d1 is the same in these functions.
// Functions `const Vector& GetPlayerViewOffset(CTerrorPlayer* pPlayer, bool bDucked)` and `void CTerrorGameMovement::DecayPunchAngle()`

// When we watch a demo or broadcast, the code is called like this:
// `CViewRender::SetUpView()->C_HLTVCamera::CalcView(..)->C_HLTVCamera::CalcInEyeCamView(...)`

// `C_BasePlayer::m_Local.m_vecPunchAngle` - This prop is not available to us without modifying the server.
// Need to add code to function `SendProxy_SendLocalDataTable` for server.

// ## Game code to fix:
#if 0
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

	- if ( pPlayer->GetFlags() & FL_DUCKING )
	- {
	- 	m_vCamOrigin += VEC_DUCK_VIEW;
	- }
	- else
	- {
	- 	m_vCamOrigin += VEC_VIEW;
	- }

	eyeOrigin = m_vCamOrigin;
	eyeAngles = m_aCamAngle;
	fov = m_flFOV;

	+ float fZNear, fZFar;
	+ pPlayer->CalcView(eyeOrigin, eyeAngles, fZNear, fZFar, fov);

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
#endif

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

	if (pPlayer == NULL) {
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

	// We do not need this code, apparently it is already inside function 'C_BasePlayer::CalcView', 
	// if we use it, then the camera offset will be incorrect.

#if 0
	if (bSurvIsIncapacitated) {
		Vector vecView = VEC_DUCK_VIEW_CUSTOM;
		float fIncapEyeHeight = survivor_incapacitated_eye_height.GetFloat();
		float fProgressBarPercent = pPlayer->GetProgressBarPercent();

		vecView.z = (fIncapEyeHeight * (1.0f - fProgressBarPercent)) + (vecView.z * fProgressBarPercent);
		
		m_vCamOrigin += vecView;
	} else if (pPlayer->GetFlags() & FL_DUCKING) {
		m_vCamOrigin += VEC_DUCK_VIEW_CUSTOM;
	} else {
		m_vCamOrigin += VEC_VIEW_CUSTOM;
	}
#endif
	
	eyeOrigin = m_vCamOrigin;
	eyeAngles = m_aCamAngle;
	fov = m_flFOV;

	// Function 'C_HLTVCamera::CalcView' makes this function call in the same way (C_BasePlayer::CalcView);
	float fZNear, fZFar;
	pPlayer->CalcView(eyeOrigin, eyeAngles, fZNear, fZFar, fov);

	pPlayer->CalcViewModelView(eyeOrigin, eyeAngles);
	
	C_BaseViewModel* pViewModel = pPlayer->GetViewModel(0);
	if (pViewModel != NULL) {
		pViewModel->UpdateVisibility();
	}
}

const QAngle& CHLTVCameraFix::GetPunchAngle(C_TerrorPlayer* pPlayer, bool bSurvIsIncapacitated)
{
	// Netprop value `C_BasePlayer::m_Local.m_vecPunchAngle` during player incapacitation: 0.000000 0.000000 19.968750 (l4d1 and l4d2)
	// This code has an accumulative effect, the `C_BasePlayer::m_Local.m_vecPunchAngle` - z accumulates over time (never exceeds 20 on the z-axis).

	// We need to know if the server sends this data from local tables
	// If not, try to reproduce the code during incap
	// This code exists in `server_srv.so`, function `void CTerrorGameMovement::DecayPunchAngle()`

	if (g_CvarSourceTVSendLocalTables.GetBool()) {
		/*Msg(VSP_LOG_PREFIX "Server cvar 'tv_send_local_data_tables' enabled, netprop `C_BasePlayer::m_Local.m_vecPunchAngle` applied: %f %f %f""\n", \
				pPlayer->GetPunchAngle().x, pPlayer->GetPunchAngle().y, pPlayer->GetPunchAngle().z);*/

		return pPlayer->GetPunchAngle();
	}

	// We can't use this code because the vector has a cumulative effect and we need to save it somewhere, clean it up, and so on,
	// and netprop `C_BasePlayer::m_Local.m_vecPunchAngle` is cleared every frame for no clear reason, 
	// even if the server does not send this data (example at the bottom).
#if 0
	static ConVarRef survivor_incapacitated_roll("survivor_incapacitated_roll");
	static ConVarRef punch_angle_decay_rate("punch_angle_decay_rate");

	QAngle &vecPunchAngleMember = pPlayer->GetPunchAngleRef();

	// Clear the vector if the player is no longer incapacitated
	if (!bSurvIsIncapacitated) {
		vecPunchAngleMember = vec3_angle;
		return vec3_angle;
	}

	// Netprop `C_BasePlayer::m_Local.m_vecPunchAngle` not available here so try to reproduce the code
	//Msg(VSP_LOG_PREFIX "Server cvar 'tv_send_local_data_tables' disabled attempt to reproduce code during player incapacitation""\n");

	float fRoll = survivor_incapacitated_roll.GetFloat();
	if (fRoll == 0.0f) {
		vecPunchAngleMember = vec3_angle;
		return vec3_angle;
	}

	Vector vecPunchAngleResult = Vector(pPlayer->GetPunchAngle().x, pPlayer->GetPunchAngle().y, 0.0f);

	float fScale = VectorNormalize(vecPunchAngleResult);
	fScale = fScale - (fScale * 0.5f + punch_angle_decay_rate.GetFloat()) * g_pGlobals->frametime;
	fScale = MAX(fScale, 0.0f);
	vecPunchAngleResult *= fScale;

	vecPunchAngleResult.z = (fRoll - pPlayer->GetPunchAngle().z) * (g_pGlobals->frametime * 10.0f) + pPlayer->GetPunchAngle().z;
	
	/*Msg(VSP_LOG_PREFIX "Applied vector during player incapacitation. vecPunchAngle: %f %f %f, frametime: %f, survivor_incapacitated_roll: %f, punch_angle_decay_rate: %f""\n", \
		vecPunchAngleResult.x, vecPunchAngleResult.y, vecPunchAngleResult.z, g_pGlobals->frametime, fRoll, punch_angle_decay_rate.GetFloat());*/
	
	vecPunchAngleMember = QAngle(vecPunchAngleResult.x, vecPunchAngleResult.y, vecPunchAngleResult.z);
	return vecPunchAngleMember;
#else
	static QAngle vecIncapPunchAngleDef(0.0f, 0.0f, 19.968750f);

	return vecIncapPunchAngleDef;
#endif
}

bool CHLTVCameraFix::CreateDetour(HMODULE clientdll)
{
	size_t iSigSize = 0;
	uintptr_t C_HLTVCamera__CalcInEyeCamView_pfn = UTIL_SignatureToAddress(clientdll, SIG_CHLTVCAMERA_CALCINEYECAMVIEW, &iSigSize);
	if (C_HLTVCamera__CalcInEyeCamView_pfn == NULL) {
		Error(VSP_LOG_PREFIX "Failed to find signature 'C_HLTVCamera::CalcInEyeCamView'. Please contact the author""\n");

		return false;
	}

	m_Detour_CalcInEyeCamView = DETOUR_CREATE_MEMBER(C_HLTVCamera__CalcInEyeCamView, (void*)C_HLTVCamera__CalcInEyeCamView_pfn);
	if (m_Detour_CalcInEyeCamView == NULL) {
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
	if (m_Detour_CalcInEyeCamView != NULL) {
		m_Detour_CalcInEyeCamView->DisableDetour();
		m_Detour_CalcInEyeCamView = NULL;
	}
}

/*
Server gpGlobals->frametime 0.009999
Client gpGlobals->frametime 0.005606

// Server
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle: -1.840965 0.000000 0.000000
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : -1.631760 0.000000 1.999999
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : -1.423601 0.000000 3.799999
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : -1.216483 0.000000 5.419999
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : -1.010400 0.000000 6.877999
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : -0.805348 0.000000 8.190198
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : -0.601321 0.000000 9.371178
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : -0.398315 0.000000 10.434061
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : -0.196323 0.000000 11.390654
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 12.251588
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 13.026430
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 13.723787
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 14.351408
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 14.916268
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 15.424641
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 15.882177
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 16.293958
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 16.664562
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 16.998106
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 17.298295
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 17.568466
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 17.811618
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 18.030456
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 18.227411
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 18.404670
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 18.564203
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 18.707782
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 18.837003
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 18.953302
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.057971
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.152173
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.236955
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.313259
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.381933
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.443738
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.499364
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.549428
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.594486
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.635038
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.671533
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.704380
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.733942
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.760547
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.784492
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.806043
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.825439
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.842895
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.858606
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.872745
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.885471
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.896924
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.907232
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.916509
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.924858
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.932373
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.939136
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.945222
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.950700
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.955631
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.960067
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.964061
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.967655
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.970890
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.973800
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.976421
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.978778
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.980901
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.982810
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.984529
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.986076
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.987468
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.988721
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.989849
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.990863
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.991777
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.992599
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.993339
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.994005
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.994604
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.995143
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.995630
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.996067
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.996459
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.996814
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.997133
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.997419
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.997676
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.997909
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.998119
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.998308
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.998477
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.998630
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.998767
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.998891
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999002
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999101
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999191
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999271
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999343
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999408
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999467
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999521
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999568
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999612
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999650
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999685
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999715
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999744
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999769
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999792
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999813
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999832
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999849
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999864
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999877
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999889
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999900
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999910
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999919
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999927
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999935
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999940
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999946
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999952
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999956
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999959
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999963
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999967
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999971
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999973
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999975
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999977
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999979
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999980
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999982
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999984
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999986
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999988
[CTerrorGameMovement::DecayPunchAngle Pre] Coach incapacitated, m_vecPunchAngle : 0.000000 0.000000 19.999990
*/