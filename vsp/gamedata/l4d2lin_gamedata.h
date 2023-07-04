#ifndef _INCLUDE_L4D1_2_GAMEDATA_VSP_H_
#define _INCLUDE_L4D1_2_GAMEDATA_VSP_H_

////////////////////////////// Pointer signatures ////////////////////////////// 
// Unfixed bug, byte '/x00' is treated as a null terminator because of this, the signature cannot be found.

#define SIG_GAMERULES_PTR									"\xA1\x2A\x2A\x2A\x2A\x2A\x2A\x2A\xF3\x0F\x10\x45\x2A\xF3\x0F\x11\x43\x2A\xF6\x86\x2A\x2A\x2A\x2A\x2A"
// Pointer to class 'CTerrorGameRules'. Signature + 2 first bytes (A1 ?)
// A1 ? ? ? ? ? ? ? F3 0F 10 45 ? F3 0F 11 43 ? F6 86 ? ? ? ? ?
// Pointer to be inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.so

#define SIG_GLOBALS_PTR										"\x8B\x0D\x2A\x2A\x2A\x2A\x3B\x51\x2A\x7F\x2A"
// Pointer to class 'CGlobalVars'. Signature + 2 first bytes (8B 0D)
// 8B 0D ? ? ? ? 3B 51 ? 7F ?
// The pointer is in function 'UTIL_PlayerByIndex', which can also be found in function 'C_HLTVCamera::CalcInEyeCamView'
// client.so

////////////////////////////// Function signatures ////////////////////////////// 

#define SIG_CMODELRENDER_CFINDORCREATESTATICPROPCOLORDATA	"\x55\x89\xE5\x57\x56\x53\x81\xEC\x5C\x07\x2A\x2A"
// CColorMeshData* CModelRender::FindOrCreateStaticPropColorData(ModelInstanceHandle_t handle)
// 55 89 E5 57 56 53 81 EC 5C 07 ? ?
// engine.so
// Can be found by line "g_pMDLCache->GetHardwareData failed for %s\n"

#define SIG_CHLTVCAMERA_CALCCHASECAMVIEW					"\x55\x89\xE5\x57\x56\x53\x81\xEC\x3C\x01\x2A\x2A\x8B\x75\x08"
// void C_HLTVCamera::CalcChaseCamView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
// 55 89 E5 57 56 53 81 EC 3C 01 ? ? 8B 75 08
// client.so
// This function is in function 'C_HLTVCamera::CalcInEyeCamView'

#define SIG_CHLTVCAMERA_CALCINEYECAMVIEW					"\x55\x89\xE5\x57\x56\x53\x83\xEC\x2C\x8B\x45\x0C\x8B\x5D\x08\x8B\x7D\x10"
// void C_HLTVCamera::CalcInEyeCamView(Vector& eyeOrigin, QAngle& eyeAngles, float& fov)
// 55 89 E5 57 56 53 83 EC 2C 8B 45 0C 8B 5D 08 8B 7D 10
// client.so
// Can find function 'void CViewRender::SetUpView()' using cvar 'cl_demoviewoverride' (search as string), 
// inside this function there is function 'void C_HLTVCamera::CalcView(Vector& origin, QAngle& angles, float& fov)', 
// inside function 'void C_HLTVCamera::CalcView(Vector& origin, QAngle& angles, float& fov)' there is the function we need,
// see source code hl2sdk-l4d2.

#define SIG_CBASEENTITY_SETPARENT							"\x55\xB8\xFF\xFF\xFF\xFF\x89\xE5\x56\x53\x83\xEC\x40"
// void C_BaseEntity::SetParent(C_BaseEntity *pParentEntity, int iParentAttachment)
// 55 B8 FF FF FF FF 89 E5 56 53 83 EC 40
// client.so
// Can be found by line "General.BurningFlesh", 
// we will find function 'CC_EntityFlame *FireEffect(C_BaseAnimating *pTarget, C_BaseEntity *pServerFire, float *flScaleEnd, float *flTimeStart, float *flTimeEnd)',
// the function we need is inside, see the source code

#define SIG_CBASEENTITY_UPDATEVISIBILITY					"\x55\x89\xE5\x57\x56\x53\x83\xEC\x5C\x8B\x75\x08\x8B\x86\x50\x01"
// void C_BaseEntity::UpdateVisibility()
// 55 89 E5 57 56 53 83 EC 5C 8B 75 08 8B 86 50 01
// client.so
// This function is in function 'C_HLTVCamera::CalcInEyeCamView'

#define SIG_CBASEPLAYER_GETVIEWMODEL						"\x55\x89\xE5\x57\x56\x53\x83\xEC\x1C\x8B\x45\x0C\x8B\x5D\x08\x8D\x04\x85\xF0\x13"
// C_BaseViewModel* C_BasePlayer::GetViewModel(int viewmodelindex = 0)
// 55 89 E5 57 56 53 83 EC 1C 8B 45 0C 8B 5D 08 8D 04 85 F0 13
// client.so
// This function is in function 'C_HLTVCamera::CalcInEyeCamView'

////////////////////////////// Offsets //////////////////////////////

#define OFF_CBASEENTITY_INDEX								72
// Offset 'C_BaseEntity::index'
// Can be found by line 'snd_soundmixer', 
// we will find function 'void C_BasePlayer::CheckForLocalPlayer( int nSplitScreenSlot )', 
// when searching we will find 4 functions, but only in this function there is 1 line 'snd_soundmixer', 
// there are no other lines, inside it there is the offset we need, look at the source code for the code (the offset can be found inside the condition)
// Nearest dataprop 'C_BaseEntity::m_clrRender', offset 84
// client.so

#define OFF_CBASEENTITY_MOVEPARENT							500
// Offset 'C_BaseEntity::m_pMoveParent'
// Offset is inside function 'void C_BaseEntity::SetParent(C_BaseEntity *pParentEntity, int iParentAttachment)' which we have already found
// Nearest dataprop 'C_BaseEntity::m_iParentAttachment', offset 488
// client.so

//////////////////////////// Vtable Offsets ////////////////////////////

#define VTB_OFF_CBASEENTITY_ISPLAYER						191
// virtual bool IsPlayer() const 
// Vtable offset is inside function 'UTIL_PlayerByIndex', 
// which can also be found in function 'C_HLTVCamera::CalcInEyeCamView'
// client.so

#define VTB_OFF_IVENGINETOOL_GETCLIENTFACTORY				12
// void IEngineTool::GetClientFactory(CreateInterfaceFn& factory)
// Calculated with csgo source code and then checked
// 'hl2sdk-l4d2' has the wrong vtable table 'IEngineTool', vtable offset 10
// client.so

#define VTB_OFF_IBASECLIENTDLL_GETALLCLASSES				7
// ClientClass* IBaseClientDLL::GetAllClasses()
// 'hl2sdk-l4d2' has the wrong vtable table 'IBaseClientDLL', vtable offset 6
// Calculated closest value using 'hl2sdk-l4d2', and found exact value using testing
// client.so

#define VTB_OFF_CBASEPLAYER_CALCVIEWMODELVIEW				308
// void CBasePlayer::CalcViewModelView(const Vector& eyeOrigin, const QAngle& eyeAngles)
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.so

#define VTB_OFF_CBASEPLAYER_GETFOV							360		
// float CBasePlayer::GetFOV() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.so

#define VTB_OFF_CBASEENTITY_GETABSORIGIN					12
// const Vector& C_BaseEntity::GetAbsOrigin() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.so

#define VTB_OFF_CBASEPLAYER_EYEANGLES						205
// const QAngle& C_BaseEntity::EyeAngles() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.so

#define VTB_OFF_CTERRORGAMERULES_GETVIEWVECTORS				31
// const CViewVectors* CGameRules::GetViewVectors() const
// Vtable offset is inside function 'C_HLTVCamera::CalcInEyeCamView'
// client.so

#define VTB_OFF_CBASEPLAYER_CALCVIEW						307
// void CBasePlayer::CalcView(Vector& eyeOrigin, QAngle& eyeAngles, float& zNear, float& zFar, float& fov)
// The vtable can be found inside function 'C_HLTVCamera::CalcView', see 'hl2sdk-l4d2'. 
// Function ''C_HLTVCamera::CalcView' can be found using function 'C_HLTVCamera::CalcInEyeCamView',
// since it calls this function 'C_HLTVCamera::CalcInEyeCamView' internally. 
// Call order '..->CViewRender::SetUpView()->C_HLTVCamera::CalcView()->C_HLTVCamera::CalcInEyeCamView()'.
// client.so

#endif // _INCLUDE_L4D1_2_GAMEDATA_VSP_H_