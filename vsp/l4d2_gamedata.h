#ifndef _INCLUDE_GAMEDATA_VSP_H_
#define _INCLUDE_GAMEDATA_VSP_H_

// Signatures

#define SIG_CBASEENTITY_SETPARENT			"\x55\x8B\xEC\x83\xEC\x24\x56\x57\x8B\x7D\x08\x8B\xF1\x85\xFF\x74\x2A"
// void C_BaseEntity::SetParent(C_BaseEntity *pParentEntity, int iParentAttachment)
// 55 8B EC 83 EC 24 56 57 8B 7D 08 8B F1 85 FF 74 ?
// client.dll
// Can be found by line "General.BurningFlesh", 
// we will find function 'CC_EntityFlame *FireEffect(C_BaseAnimating *pTarget, C_BaseEntity *pServerFire, float *flScaleEnd, float *flTimeStart, float *flTimeEnd)',
// the function we need is inside, see the source code
// Offsets

#define OFF_CBASEENTITY_INDEX				88
// Offset 'C_BaseEntity::index'
// Can be found by line 'snd_soundmixer', 
// we will find function 'void C_BasePlayer::CheckForLocalPlayer( int nSplitScreenSlot )', 
// when searching we will find 4 functions, but only in this function there is 1 line 'snd_soundmixer', 
// there are no other lines, inside it there is the offset we need, look at the source code for the code
// Nearest dataprop 'C_BaseEntity::m_clrRender', Offset 100

#define OFF_CBASEENTITY_MOVEPARENT			516
// Offset 'C_BaseEntity::m_pMoveParent'
// Offset is inside function 'void C_BaseEntity::SetParent(C_BaseEntity *pParentEntity, int iParentAttachment)' which we have already found
// Nearest dataprop 'C_BaseEntity::m_flFadeScale', Offset 496

#define OFF_CBASEENTITY_NETWORK_MOVEPARENT	308
// Offset C_BaseEntity::m_hNetworkMoveParent (dataprop)
// Nearest dataprop 'C_BaseEntity::m_flFriction' - 304

#endif // _INCLUDE_GAMEDATA_VSP_H_