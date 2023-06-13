#ifndef _INCLUDE_L4D2_GAMEDATA_VSP_H_
#define _INCLUDE_L4D2_GAMEDATA_VSP_H_

////////////////////////////// Signatures ////////////////////////////// 
// Unfixed bug, byte '/x00' is treated as a null terminator because of this, the signature cannot be found.

#define SIG_GAMERULES_PTR						"\x8B\x0D\x2A\x2A\x2A\x2A\x8B\x11\x8B\x42\x2A\x74\x2A\xFF\xD0\xF3\x0F\x10\x40\x2A"
// Pointer to class 'CTerrorGameRules'. Signature + 2 first bytes (8B 0D)
// 8B 0D ? ? ? ? 8B 11 8B 42 ? 74 ? FF D0 F3 0F 10 40 ?
// Pointer to be inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define SIG_GLOBALS_PTR							"\x8B\x0D\x2A\x2A\x2A\x2A\x3B\x41\x2A\x7F\x2A\x8B\x0D\x2A\x2A\x2A\x2A"
// Pointer to class 'CGlobalVars'. Signature + 2 first bytes (8B 0D)
// 8B 0D ? ? ? ? 3B 41 ? 7F ? 8B 0D ? ? ? ?
// The pointer is in function 'UTIL_PlayerByIndex', which can also be found in function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define SIG_CHLTVCAMERA_CALCCHASECAMVIEW		"\x53\x8B\xDC\x83\xEC\x08\x83\xE4\xF0\x83\xC4\x04\x55\x8B\x6B\x04\x89\x6C\x24\x04\x8B\xEC\x81\xEC\xC8\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A\x33\xC5\x89\x45\xFC\x8B\x15\x2A\x2A\x2A\x2A"
// void C_HLTVCamera::CalcChaseCamView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
// 53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC C8 ? ? ? A1 ? ? ? ? 33 C5 89 45 FC 8B 15 ? ? ? ?
// client.dll
// This function is in function 'C_HLTVCamera::CalcInEyeCamView'

#define SIG_CHLTVCAMERA_CALCINEYECAMVIEW		"\x55\x8B\xEC\x56\x8B\xF1\x8B\x46\x2C"
// void C_HLTVCamera::CalcInEyeCamView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
// 55 8B EC 56 8B F1 8B 46 2C
// client.dll
// Can find function 'void CViewRender::SetUpView()' using cvar 'cl_demoviewoverride' (search as string), 
// inside this function there is function 'void C_HLTVCamera::CalcView(Vector& origin, QAngle& angles, float& fov)', 
// inside function 'void C_HLTVCamera::CalcView(Vector& origin, QAngle& angles, float& fov)' there is the function we need,
// see source code hl2sdk-l4d2.

#define SIG_CBASEENTITY_SETPARENT				"\x55\x8B\xEC\x83\xEC\x24\x56\x57\x8B\x7D\x08\x8B\xF1\x85\xFF\x74\x2A"
// void C_BaseEntity::SetParent(C_BaseEntity *pParentEntity, int iParentAttachment)
// 55 8B EC 83 EC 24 56 57 8B 7D 08 8B F1 85 FF 74 ?
// client.dll
// Can be found by line "General.BurningFlesh", 
// we will find function 'CC_EntityFlame *FireEffect(C_BaseAnimating *pTarget, C_BaseEntity *pServerFire, float *flScaleEnd, float *flTimeStart, float *flTimeEnd)',
// the function we need is inside, see the source code

#define SIG_CBASEENTITY_UPDATEVISIBILITY		"\x55\x8B\xEC\x83\xEC\x2C\xA1\x2A\x2A\x2A\x2A\x33\xC5\x89\x45\xFC\x57\x8B\xF9\x8B\x8F\x60\x01\x2A\x2A"
// void C_BaseEntity::UpdateVisibility()
// 55 8B EC 83 EC 2C A1 ? ? ? ? 33 C5 89 45 FC 57 8B F9 8B 8F 60 01 ? ?
// client.dll
// This function is in function 'C_HLTVCamera::CalcInEyeCamView'

#define SIG_CBASEPLAYER_GETVIEWMODEL			"\x55\x8B\xEC\x8B\x45\x08\x53\x57\x8B\xF9"
// C_BaseViewModel* C_BasePlayer::GetViewModel(int viewmodelindex = 0)
// 55 8B EC 8B 45 08 53 57 8B F9
// client.dll
// This function is in function 'C_HLTVCamera::CalcInEyeCamView'

////////////////////////////// Offsets //////////////////////////////

#define OFF_CBASEENTITY_INDEX					88
// Offset 'C_BaseEntity::index'
// Can be found by line 'snd_soundmixer', 
// we will find function 'void C_BasePlayer::CheckForLocalPlayer( int nSplitScreenSlot )', 
// when searching we will find 4 functions, but only in this function there is 1 line 'snd_soundmixer', 
// there are no other lines, inside it there is the offset we need, look at the source code for the code
// Nearest dataprop 'C_BaseEntity::m_clrRender', Offset 100
// client.dll

#define OFF_CBASEENTITY_MOVEPARENT				516
// Offset 'C_BaseEntity::m_pMoveParent'
// Offset is inside function 'void C_BaseEntity::SetParent(C_BaseEntity *pParentEntity, int iParentAttachment)' which we have already found
// Nearest dataprop 'C_BaseEntity::m_flFadeScale', Offset 496
// client.dll

#define OFF_CBASEENTITY_NETWORK_MOVEPARENT		308
// Offset C_BaseEntity::m_hNetworkMoveParent (dataprop)
// Nearest dataprop 'C_BaseEntity::m_flFriction' - 304
// client.dll

#define OFF_CBASEENTITY_LIFESTATE				327
// Offset 'C_BaseEntity::m_lifeState' (netprop)
// client.dll

#define OFF_CBASEENTITY_FFLAGS					240
// Offset 'C_BaseEntity::m_fFlags' (netprop)
// client.dll

#define OFF_CTERRORPLAYER_ISINCAPACITATED		7849
// Offset 'C_TerrorPlayer::m_isIncapacitated' (netprop)
// client.dll

#define OFF_CBASEENTITY_SIMULATIONTIME			336
// Offset 'C_BaseEntity::m_flSimulationTime' (netprop)
// client.dll

#define OFF_CTERRORPLAYER_ZOMBIECLASS			7312
// Offset 'C_TerrorPlayer::m_zombieClass' (netprop)
// client.dll

#define OFF_CBASEENTITY_TEAMNUM					228
// Offset 'C_BaseEntity::m_iTeamNum' (netprop)
// client.dll

#define OFF_CCSPLAYER_PROGRESSBARSTARTTIME		6300
// Offset 'C_CSPlayer::m_flProgressBarStartTime' (netprop)
// client.dll

#define OFF_CCSPLAYER_PROGRESSBARDURATION		6296
// Offset 'C_CSPlayer::m_flProgressBarDuration' (netprop)
// client.dll

#define OFF_CTERRORPLAYER_ISHANGINGFROMTONGUE	8068
// Offset 'C_TerrorPlayer::m_isHangingFromTongue' (netprop)
// client.dll

#define OFF_CTERRORPLAYER_ISHANGINGFROMLEDGE	9708
// Offset 'C_TerrorPlayer::m_isHangingFromLedge' (netprop)
// client.dll

#define OFF_CBASEENTITY_VIEWOFFSET				244
// Offset 'C_BaseEntity::m_vecViewOffset' (netprop)
// client.dll

#define OFF_CBASEPLAYER_PUNCHANGLE				4612
// Offset 'C_BasePlayer::m_Local.m_vecPunchAngle' (netprop)
// client.dll

#define OFF_CTERRORPLAYER_POUNCEVICTIM			10024
// Offset 'C_TerrorPlayer::m_pounceVictim' (netprop)
// client.dll

#define OFF_CTERRORPLAYER_JOCKEYATTACKER		10060
// Offset 'C_TerrorPlayer::m_jockeyAttacker' (netprop)
// client.dll

//////////////////////////// Vtable Offsets ////////////////////////////

#define VTB_OFF_CBASEENTITY_ISPLAYER			143
// virtual bool IsPlayer( void ) const 
// Vtable offset is inside function 'UTIL_PlayerByIndex', 
// which can also be found in function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_IVENGINETOOL_GETCLIENTFACTORY	11
// void IEngineTool::GetClientFactory(CreateInterfaceFn& factory)
// Calculated with csgo source code and then checked
// client.dll

#define VTB_OFF_CBASEPLAYER_CALCVIEWMODELVIEW	249
// void CBasePlayer::CalcViewModelView(const Vector& eyeOrigin, const QAngle& eyeAngles)
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_CBASEPLAYER_GETFOV				301			
// float CBasePlayer::GetFOV() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_CBASEENTITY_GETABSORIGIN		11
// const Vector& C_BaseEntity::GetAbsOrigin() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_CBASEPLAYER_EYEANGLES			156
// inline const QAngle& C_BaseEntity::EyeAngles(void) const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_CTERRORGAMERULES_GETVIEWVECTORS	30
// const CViewVectors* CGameRules::GetViewVectors() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#endif // _INCLUDE_L4D2_GAMEDATA_VSP_H_