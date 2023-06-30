#ifndef _INCLUDE_L4D1_2_GAMEDATA_VSP_H_
#define _INCLUDE_L4D1_2_GAMEDATA_VSP_H_

////////////////////////////// Pointer signatures ////////////////////////////// 
// Unfixed bug, byte '/x00' is treated as a null terminator because of this, the signature cannot be found.

#define SIG_GAMERULES_PTR									"\x8B\x0D\x2A\x2A\x2A\x2A\x8B\x11\x8B\x42\x2A\x74\x2A\xFF\xD0\x83\xC0\x2A"
// Pointer to class 'CTerrorGameRules'. Signature + 2 first bytes (8B 0D)
// 8B 0D ? ? ? ? 8B 11 8B 42 ? 74 ? FF D0 83 C0 ?
// Pointer to be inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define SIG_GLOBALS_PTR										"\x8B\x0D\x2A\x2A\x2A\x2A\x3B\x41\x2A\x7F\x2A\x8B\x0D\x2A\x2A\x2A\x2A"
// Pointer to class 'CGlobalVars'. Signature + 2 first bytes (8B 0D)
// 8B 0D ? ? ? ? 3B 41 ? 7F ? 8B 0D ? ? ? ?
// The pointer is in function 'UTIL_PlayerByIndex', which can also be found in function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

////////////////////////////// Function signatures ////////////////////////////// 

#define SIG_CMODELRENDER_CFINDORCREATESTATICPROPCOLORDATA	"\x66\x8B\x44\x24\x04\x81\xEC\x14\x05\x2A\x2A\x66\x3D\x2A\x2A"
// CColorMeshData* CModelRender::FindOrCreateStaticPropColorData(ModelInstanceHandle_t handle)
// 66 8B 44 24 04 81 EC 14 05 ? ? 66 3D ? ?
// engine.dll
// Can be found by line "g_pMDLCache->GetHardwareData failed for %s\n"

#define SIG_CHLTVCAMERA_CALCCHASECAMVIEW					"\x55\x8B\xEC\x83\xE4\xF0\x81\xEC\xA4\x2A\x2A\x2A\xA1\x2A\x2A\x2A\x2A"
// void C_HLTVCamera::CalcChaseCamView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
// 55 8B EC 83 E4 F0 81 EC A4 ? ? ? A1 ? ? ? ?
// client.dll
// This function is in function 'C_HLTVCamera::CalcInEyeCamView'

#define SIG_CHLTVCAMERA_CALCINEYECAMVIEW					"\x56\x8B\xF1\x8B\x46\x2C\x57\x50\xE8\x2A\x2A\x2A\x2A"
// void C_HLTVCamera::CalcInEyeCamView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
// 56 8B F1 8B 46 2C 57 50 E8 ? ? ? ?
// client.dll
// Can find function 'void CViewRender::SetUpView()' using cvar 'cl_demoviewoverride' (search as string), 
// inside this function there is function 'void C_HLTVCamera::CalcView(Vector& origin, QAngle& angles, float& fov)', 
// inside function 'void C_HLTVCamera::CalcView(Vector& origin, QAngle& angles, float& fov)' there is the function we need,
// see source code hl2sdk-l4d1.

#define SIG_CBASEENTITY_SETPARENT							"\x83\xEC\x24\x56\x57\x8B\x7C\x24\x30\x85\xFF\x8B\xF1\x74\x2A"
// void C_BaseEntity::SetParent(C_BaseEntity *pParentEntity, int iParentAttachment)
// 83 EC 24 56 57 8B 7C 24 30 85 FF 8B F1 74 ?
// client.dll
// Can be found by line "General.BurningFlesh", 
// we will find function 'CC_EntityFlame *FireEffect(C_BaseAnimating *pTarget, C_BaseEntity *pServerFire, float *flScaleEnd, float *flTimeStart, float *flTimeEnd)',
// the function we need is inside, see the source code

#define SIG_CBASEENTITY_UPDATEVISIBILITY					"\x83\xEC\x20\x55\x56\x57\x8B\xF9\x8D\x87\x38\x01\x2A\x2A"
// void C_BaseEntity::UpdateVisibility()
// 83 EC 20 55 56 57 8B F9 8D 87 38 01 ? ?
// client.dll
// This function is in function 'C_HLTVCamera::CalcInEyeCamView'

#define SIG_CBASEPLAYER_GETVIEWMODEL						"\x53\x55\x8B\x6C\x24\x0C\x57\x8B\xF9"
// C_BaseViewModel* C_BasePlayer::GetViewModel(int viewmodelindex = 0)
// 53 55 8B 6C 24 0C 57 8B F9
// client.dll
// This function is in function 'C_HLTVCamera::CalcInEyeCamView'

////////////////////////////// Offsets //////////////////////////////

#define OFF_CBASEENTITY_INDEX								80
// Offset 'C_BaseEntity::index'
// Can be found by line 'snd_soundmixer', 
// we will find function 'void C_BasePlayer::CheckForLocalPlayer( int nSplitScreenSlot )', 
// when searching we will find 4 functions, but only in this function there is 1 line 'snd_soundmixer', 
// there are no other lines, inside it there is the offset we need, look at the source code for the code (the offset can be found inside the condition)
// Nearest dataprop 'C_BaseEntity::m_clrRender', offset 92
// client.dll

#define OFF_CBASEENTITY_MOVEPARENT							476
// Offset 'C_BaseEntity::m_pMoveParent'
// Offset is inside function 'void C_BaseEntity::SetParent(C_BaseEntity *pParentEntity, int iParentAttachment)' which we have already found
// Nearest dataprop 'C_BaseEntity::m_iParentAttachment', offset 464
// client.dll

//////////////////////////// Vtable Offsets ////////////////////////////

#define VTB_OFF_CBASEENTITY_ISPLAYER						136
// virtual bool IsPlayer() const 
// Vtable offset is inside function 'UTIL_PlayerByIndex', 
// which can also be found in function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_IBASECLIENTDLL_GETALLCLASSES				7
// ClientClass* IBaseClientDLL::GetAllClasses()
// 'hl2sdk-l4d2' has the wrong vtable table 'IBaseClientDLL', vtable offset 6
// Calculated closest value using 'hl2sdk-l4d2', and found exact value using testing
// client.dll

#define VTB_OFF_CBASEPLAYER_CALCVIEWMODELVIEW				232
// void CBasePlayer::CalcViewModelView(const Vector& eyeOrigin, const QAngle& eyeAngles)
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_CBASEPLAYER_GETFOV							281			
// float CBasePlayer::GetFOV() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_CBASEENTITY_GETABSORIGIN					11
// const Vector& C_BaseEntity::GetAbsOrigin() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_CBASEPLAYER_EYEANGLES						146
// inline const QAngle& C_BaseEntity::EyeAngles() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_CTERRORGAMERULES_GETVIEWVECTORS				30
// const CViewVectors* CGameRules::GetViewVectors() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.dll

#define VTB_OFF_CBASEPLAYER_CALCVIEW						231
// void CBasePlayer::CalcView(Vector& eyeOrigin, QAngle& eyeAngles, float& zNear, float& zFar, float& fov)
// The vtable can be found inside function 'C_HLTVCamera::CalcView', see 'hl2sdk-l4d'. 
// Function ''C_HLTVCamera::CalcView' can be found using function 'C_HLTVCamera::CalcInEyeCamView',
// since it calls this function 'C_HLTVCamera::CalcInEyeCamView' internally. 
// Call order '..->CViewRender::SetUpView()->C_HLTVCamera::CalcView()->C_HLTVCamera::CalcInEyeCamView()'.
// client.dll

#endif // _INCLUDE_L4D1_2_GAMEDATA_VSP_H_