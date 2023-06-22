#include "fixes.h"
#include "wrappers.h"
#include "ivmodelrender.h"

#define MODEL_CRASH_DEBUG 0

#if MODEL_CRASH_DEBUG
#include <map>
#include <string>

static std::map<void*, std::string> g_mapModels;
#endif

// A fix has been found by a 'A1m`'

// I don't think this is a full fix, but this code exists in game cs go and game tf2, but in game l4d2 this code does not exist, maybe in l4d1
// This code also exists on the server in library 'engine_srv.so'.
// Client crash occurs with different models, maybe the models have not been deleted somewhere (some kind of memory is not cleared)?
// Command 'record' for recording demos affects this.

enum
{
	eDebugPrintAll = 1,
	eFixDebugPrint
};

ConVar g_CvarModelCrashFix("l4d2_model_crash_fix", "1", FCVAR_CLIENTDLL, "Toggle fix ModelCrash. 0 - Disable, 1 - Enable.");
ConVar g_CvarModelCrashFixDebug("l4d2_model_crash_fix_debug", "2", FCVAR_CLIENTDLL, "Enable ModelCrash fix debug. 1 - print all debug messages, 2 - only when fix is applied.");

//
//-----------------------------------------------------------------------------
// Allocates the static prop color data meshes
//-----------------------------------------------------------------------------
// FIXME? : Move this to StudioRender?
/*CColorMeshData* CModelRender::FindOrCreateStaticPropColorData(ModelInstanceHandle_t handle)
{
	if (handle == MODEL_INSTANCE_INVALID)
	{
		// the card can't support it
		return NULL;
	}

	ModelInstance_t& instance = m_ModelInstances[handle];
	CColorMeshData* pColorMeshData = CacheGet(instance.m_ColorMeshHandle);
	if (pColorMeshData)
	{
		// found in cache
		return pColorMeshData;
	}

	+ Assert(instance.m_pModel);
	+ if (!instance.m_pModel)
	+ {
	+	// Avoid crash in mat_reloadallmaterials
	+	return NULL;
	+ }

	Assert(modelloader->IsLoaded(instance.m_pModel) && (instance.m_pModel->type == mod_studio));
	studiohwdata_t* pStudioHWData = g_pMDLCache->GetHardwareData(instance.m_pModel->studio);
	Assert(pStudioHWData);
	if (!pStudioHWData)
	{
		char fn[MAX_PATH];
		g_pFullFileSystem->String(instance.m_pModel->fnHandle, fn, sizeof(fn));
		Sys_Error("g_pMDLCache->GetHardwareData failed for %s\n", fn);
		return NULL;
	}

	colormeshparams_t params;
	InitColormeshParams(instance, pStudioHWData, &params);
	if (params.m_nMeshes <= 0)
	{
		// nothing to create
		return NULL;
	}

	// create the meshes
	params.m_fnHandle = instance.m_pModel->fnHandle;
	instance.m_ColorMeshHandle = CacheCreate(params);
	ProtectColorDataIfQueued(instance.m_ColorMeshHandle);
	pColorMeshData = CacheGet(instance.m_ColorMeshHandle);

	return pColorMeshData;
}*/

DETOUR_DECL_MEMBER1(CModelRender__FindOrCreateStaticPropColorData, CColorMeshData*, ModelInstanceHandle_t, handle)
{
	// Game L4D2 (engine.dll client, engine_srv.so server)
	// ModelInstance_t - size 260
	// The size of this structure is very important to us. 
	// Class 'CUtlLinkedList' adds the full size (260) of the structure + 4 (something in class 'CUtlLinkedList') bytes internally using operator [].
	// Otherwise we can get into the wrong memory area.
	// 
	// Windows offsets:
	// CModelRender::m_ModelInstances = 12
	// CModelRender::ModelInstance_t::m_ColorMeshHandle = 256
	// CModelRender::ModelInstance_t::m_pModel = 4
	// CModelRender::CManagedDataCacheClient::m_pCache = 8
	// 
	// CUtlLinkedList::operator[] = 264 (added to variable 'CModelRender::m_ModelInstances', seen in program 'ida'). 
	// size ModelInstance_t (260) + 4 bytes (something in class 'CUtlLinkedList').
	// 
	// Windows vtable offsets
	// CModelRender::IDataCacheClient::CacheGet() = 12 - (IDataCacheClient *m_pCache->Get()). Calculated in hl2sdk-l4d2 vtable offset is correct.
	// This code also exists on the server in library 'engine_srv.so'.
	
	if (!g_CvarModelCrashFix.GetBool()) {
		return DETOUR_MEMBER_CALL(CModelRender__FindOrCreateStaticPropColorData)(handle); // call real function
	}

	int iDebug = g_CvarModelCrashFixDebug.GetInt();

	if (iDebug == eDebugPrintAll) {
		Msg(VSP_LOG_PREFIX "[CModelRender::FindOrCreateStaticPropColorData][1] This: %x, handle: %d, ModelInstance_t sizeof: %d!""\n",
				this, handle, sizeof(ModelInstance_t));
	}

	if (handle == MODEL_INSTANCE_INVALID) {
		if (iDebug == eDebugPrintAll) {
			Msg(VSP_LOG_PREFIX "[CModelRender::FindOrCreateStaticPropColorData][2] handle == MODEL_INSTANCE_INVALID!""\n");
		}

		return DETOUR_MEMBER_CALL(CModelRender__FindOrCreateStaticPropColorData)(handle); // call real function
	}

	CModelRenderWrapper* pThis = (CModelRenderWrapper*)this;

	ModelInstance_t& instance = pThis->m_ModelInstances[handle];
	CColorMeshData* pColorMeshData = pThis->CacheGet(instance.m_ColorMeshHandle);

	if (pColorMeshData != NULL) {
		if (iDebug == eDebugPrintAll) {
			Msg(VSP_LOG_PREFIX "[CModelRender::FindOrCreateStaticPropColorData][3] ModelInstance_t: %x, pColorMeshData: %x!""\n", \
					&instance, pColorMeshData);
		}

		return DETOUR_MEMBER_CALL(CModelRender__FindOrCreateStaticPropColorData)(handle); // call real function
	}

	// Fix crash
	if (instance.m_pModel == NULL) {
		if (iDebug == eFixDebugPrint) {
#if MODEL_CRASH_DEBUG
			std::string sCachedModelPath = "Unknown";
			auto iter = g_mapModels.find(&instance);
			if (iter != g_mapModels.end()) {
				sCachedModelPath = iter->second;
			}

			Msg(VSP_LOG_PREFIX "[CModelRender::FindOrCreateStaticPropColorData][4][CRASH] ModelInstance_t: %x, m_pModel NULL, fix: %d, model path: %s!""\n", \
					&instance, g_CvarModelCrashFix.GetBool(), sCachedModelPath);
#else 
			Msg(VSP_LOG_PREFIX "[CModelRender::FindOrCreateStaticPropColorData][4][CRASH] ModelInstance_t: %x, m_pModel NULL, fix: %d!""\n", \
				&instance, g_CvarModelCrashFix.GetBool());
#endif
		}

		// For debug
		/*if (!g_CvarModelCrashFix.GetBool()) {
			return DETOUR_MEMBER_CALL(CModelRender__FindOrCreateStaticPropColorData)(handle); // call real function
		}*/

		return NULL; // skip real function
	}

#if MODEL_CRASH_DEBUG
	g_mapModels[&instance] = std::string(instance.m_pModel->szPathName);
#endif

	//Msg(VSP_LOG_PREFIX "instance.m_pModel->szPathName: %s""\n", instance.m_pModel->szPathName);
	
	return DETOUR_MEMBER_CALL(CModelRender__FindOrCreateStaticPropColorData)(handle); // call real function
}

bool CModelCrashFix::CreateDetour(HMODULE enginedll)
{
	size_t iSigSize = 0;
	uintptr_t CModelRender__FindOrCreateStaticPropColorData_pfn = UTIL_SignatureToAddress(enginedll, SIG_CMODELRENDER_CFINDORCREATESTATICPROPCOLORDATA, &iSigSize);
	if (CModelRender__FindOrCreateStaticPropColorData_pfn == NULL) {
		Error(VSP_LOG_PREFIX "Failed to find signature 'CModelRender::FindOrCreateStaticPropColorData'. Please contact the author""\n");

		return false;
	}

	m_DetourFindOrCreateStaticPropColorData = DETOUR_CREATE_MEMBER(CModelRender__FindOrCreateStaticPropColorData, (void *)CModelRender__FindOrCreateStaticPropColorData_pfn);
	if (m_DetourFindOrCreateStaticPropColorData == NULL) {
		Error(VSP_LOG_PREFIX "Could not obtain signature for 'CModelRender::FindOrCreateStaticPropColorData'""\n");

		return false;
	}

	m_DetourFindOrCreateStaticPropColorData->EnableDetour();

	Msg(VSP_LOG_PREFIX "[CModelRender::FindOrCreateStaticPropColorData] Enable detour. Detour ptr: %x, func ptr: %x, sig size: %d""\n", \
			m_DetourFindOrCreateStaticPropColorData, CModelRender__FindOrCreateStaticPropColorData_pfn, iSigSize);

	return true;
}

void CModelCrashFix::DestroyDetour()
{
	if (m_DetourFindOrCreateStaticPropColorData != NULL) {
		m_DetourFindOrCreateStaticPropColorData->DisableDetour();
		m_DetourFindOrCreateStaticPropColorData = NULL;
	}

#if MODEL_CRASH_DEBUG
	g_mapModels.clear();
#endif
}
