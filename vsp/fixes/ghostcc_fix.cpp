#include "fixes.h"
#include "gamedata/gamedata.h"
#include "util.h"
#include "vsp_client.h"

// Wrote fix code and found fix 'A1m`'.

// The problem is that the color correction never changes for the infected team 
// if you watch them through a SourceTV (demo or stream).
// Correction color always sepia, for spawned infected, 
// same for infected ghosts, although it should be blue for infected ghosts.
// This patch solves this problem.

// We remove function 'C_BasePlayer::IsLocalPlayer?' from the condition so that our code works not only for the local player (see below)

#if 0
// windows l4d2, l4d1
.text:1026F72E E8 1D 3E DF FF                                call    sub_10063550 // C_BasePlayer::IsLocalPlayer ?
.text:1026F733 83 C4 04                                      add     esp, 4
.text:1026F736 84 C0                                         test    al, al
.text:1026F738 74 16                                         jz      short loc_1026F750 // change bytes to nope 90 90

// linux l4d2
.text:0085C2C0 E8 CB 95 DC FF                                call    sub_625890 // C_BasePlayer::IsLocalPlayer ?
.text:0085C2C5 84 C0                                         test    al, al
.text:0085C2C7 0F 85 BB 01 00 00                             jnz     loc_85C488 // change bytes to 90 E9 ? ? ? ? 90 -> nop (remove extra byte) and jmp old address
#endif

patch_t g_GhostCCPatch_OriginalBytes;

bool CGhostCCFix::m_bPatchAddrChecked = false;

unsigned char g_cPatchBytes[] = GHOST_CC_PATCH_BYTES;

ConVar g_CvarGhostCCFix("l4d2_ghost_cc_fix", "1", FCVAR_CLIENTDLL, "Toggle fix ghost color correction. 0 - Disable, 1 - Enable.", CGhostCCFix::OnGhostPatchStateChanged);

bool CGhostCCFix::CreatePatch()
{
	size_t iSigSize = 0;
	m_patchedAddr = UTIL_SignatureToAddress(CLIENT_MODULE_NAME, SIG_GHOST_CC_FIX_PATCH_PLACE, &iSigSize);
	if (m_patchedAddr == PTR_NULL) {
		Error(VSP_LOG_PREFIX "Unable to find address for patch 'Ghost Collor Correction' in library '%s'. Please contact the author""\n", CLIENT_MODULE_NAME);

		return false;
	}

	byte cCheckByte = *(reinterpret_cast<byte*>(m_patchedAddr + SIG_GHOST_CC_FIX_PATCH_OFFSET));
	if (cCheckByte != GHOST_CC_PATCH_CHECK_BYTE) {
		Error(VSP_LOG_PREFIX "Wrong offset %d for patch. Byte %x expected, received byte %d. Please contact the author""\n", \
			SIG_GHOST_CC_FIX_PATCH_OFFSET, GHOST_CC_PATCH_CHECK_BYTE, cCheckByte);

		return false;
	}

	EnablePatch();

	CGhostCCFix::m_bPatchAddrChecked = true;

	Msg(VSP_LOG_PREFIX "[Ghost CC Patch] Enable patch. Sig ptr: %x, sig size: %d.""\n", m_patchedAddr, iSigSize);

	return true;
}

void CGhostCCFix::OnGhostPatchStateChanged(IConVar* var, const char* pOldValue, float flOldValue)
{
	if (!CGhostCCFix::m_bPatchAddrChecked) {
		Msg(VSP_LOG_PREFIX "The patch has not been checked yet, you cannot activate it now""\n");
		return;
	}

	if (g_CvarGhostCCFix.GetBool()) {
		g_GhostCCFix.EnablePatch();
		return;
	}

	g_GhostCCFix.DisablePatch();
}

bool CGhostCCFix::EnablePatch()
{
	if (m_bPatchEnabled) {
		Msg(VSP_LOG_PREFIX "[Ghost CC Patch] The patch is already activated""\n");
		return false;
	}

	patch_t ghostCCPatch;
	memcpy(ghostCCPatch.patch, g_cPatchBytes, sizeof(g_cPatchBytes));
	ghostCCPatch.bytes = sizeof(g_cPatchBytes);

	ApplyPatch((void*)m_patchedAddr, SIG_GHOST_CC_FIX_PATCH_OFFSET, &ghostCCPatch, &g_GhostCCPatch_OriginalBytes);
	ProtectMemory((void*)m_patchedAddr, ghostCCPatch.bytes, PAGE_EXECUTE_READ);

	m_bPatchEnabled = true;

	Msg(VSP_LOG_PREFIX "[Ghost CC Patch] Patch enabled""\n");

	return true;
}

bool CGhostCCFix::DisablePatch()
{
	if (!m_bPatchEnabled) {
		Msg(VSP_LOG_PREFIX "[Ghost CC Patch] The patch is not activated yet""\n");
		return false;
	}

	ApplyPatch((void*)m_patchedAddr, SIG_GHOST_CC_FIX_PATCH_OFFSET, &g_GhostCCPatch_OriginalBytes, NULL);
	ProtectMemory((void*)m_patchedAddr, g_GhostCCPatch_OriginalBytes.bytes, PAGE_EXECUTE_READ);

	m_bPatchEnabled = false;
	Msg(VSP_LOG_PREFIX "[Ghost CC Patch] Patch disabled""\n");

	return true;
}

void CGhostCCFix::DestroyPatch()
{
	DisablePatch();
}
