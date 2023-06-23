#include "vsp_client.h"
#include "fixes/fixes.h"
#include "convar.h"
//#include "iplayerinfo.h"
#include "ienginetool.h"
#include "icliententitylist.h"
#include "wrappers.h"
//#include "itoolentity.h"
#include "cdll_int.h"

CNetPropsManager g_NetProps;

CSetParentFix g_ParentFix;
CHLTVCameraFix g_HltvCameraFix;
CModelCrashFix g_ModelCrashFix;

ICvar* g_pCvar = NULL;
//IPlayerInfoManager* g_pPlayerManager = NULL;
IClientEntityList* g_pClientEntityList = NULL;
IEngineToolWrapper* g_pEngineTool = NULL;
//IClientTools* g_pClientTools = NULL;
IBaseClientDLL* g_pClientDLL = NULL;

EXPOSE_SINGLE_INTERFACE(VSPClient, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS);

ConVar g_CvarSourceTVSendLocalTables("tv_send_local_data_tables", \
	"0", \
	FCVAR_HIDDEN | FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY, \
	"Send local data table to SourceTV? The server tells us that it sends data from local data tables."
);

bool VSPClient::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	g_pCVar = static_cast<ICvar*>(interfaceFactory(CVAR_INTERFACE_VERSION, NULL));
	if (g_pCVar == NULL) {
		Error(VSP_LOG_PREFIX "Couldn't retrieve interface \"" CVAR_INTERFACE_VERSION "\"\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[ICvar] received interface: %x ""\n", g_pCVar);

	g_pEngineTool = (IEngineToolWrapper*)interfaceFactory(VENGINETOOL_INTERFACE_VERSION, NULL);
	if (g_pEngineTool == NULL) {
		Error(VSP_LOG_PREFIX "Couldn't retrieve interface \"" VENGINETOOL_INTERFACE_VERSION "\"\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[IEngineTool] received interface: %x ""\n", g_pEngineTool);
	
	// Server side CGlobalVars (We neeed client side CGlobalVars)
	/*g_pPlayerManager = static_cast<IPlayerInfoManager*>(gameServerFactory(INTERFACEVERSION_PLAYERINFOMANAGER, NULL));
	if (g_pPlayerManager == NULL) {
		Error(VSP_LOG_PREFIX "Couldn't retrieve interface \"" INTERFACEVERSION_PLAYERINFOMANAGER "\"\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[IPlayerInfoManager] Received interface: %x ""\n", g_pPlayerManager);

	g_pGlobals = g_pPlayerManager->GetGlobalVars();
	if (g_pGlobals == NULL) {
		Error(VSP_LOG_PREFIX "Failed to get pointer to instance 'CGlobalVars'""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[CGlobalVars] Received instance: %x. Time: %f ""\n", g_pGlobals, g_pGlobals->realtime);*/

	CreateInterfaceFn gameClientFactory;
	g_pEngineTool->GetClientFactory(gameClientFactory);
	if (gameClientFactory == NULL) {
		Error(VSP_LOG_PREFIX "Failed to get client factory""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[gameClientFactory] Received interface: %x ""\n", gameClientFactory);

	g_pClientDLL = (IBaseClientDLL*)gameClientFactory(CLIENT_DLL_INTERFACE_VERSION, NULL);
	if (!g_pClientDLL) {
		Error(VSP_LOG_PREFIX "Couldn't retrieve interface \"" CLIENT_DLL_INTERFACE_VERSION "\"\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[IBaseClientDLL] Received interface: %x ""\n", g_pClientDLL);

	/*g_pClientTools = (IClientTools*)gameClientFactory(VCLIENTTOOLS_INTERFACE_VERSION, NULL);
	if (g_pClientTools == NULL) {
		Error(VSP_LOG_PREFIX "Couldn't retrieve interface \"" VCLIENTTOOLS_INTERFACE_VERSION "\"\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[IClientTools] Received interface: %x ""\n", g_pClientTools);*/

	g_pClientEntityList = (IClientEntityList*)gameClientFactory(VCLIENTENTITYLIST_INTERFACE_VERSION, NULL);
	if (g_pClientEntityList == NULL) {
		Error(VSP_LOG_PREFIX "Couldn't retrieve interface \"" VCLIENTENTITYLIST_INTERFACE_VERSION "\"\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[IClientEntityList] Received interface: %x ""\n", g_pClientEntityList);
	
	HMODULE clientdll = GetModuleHandle("client.dll");
	HMODULE enginedll = GetModuleHandle("engine.dll");
	
	if (!DumpClientNetClasses()) {
		return false;
	}

	if (!InitFunctions(clientdll)) {
		return false;
	}

	if (!InitClassInstances(clientdll)) {
		return false;
	}

	if (!InitOffsets()) {
		return false;
	}

	if (!g_ParentFix.CreateDetour(clientdll)) {
		return false;
	}

	if (!g_HltvCameraFix.CreateDetour(clientdll)) {
		return false;
	}

	if (!g_ModelCrashFix.CreateDetour(enginedll)) {
		return false;
	}

	ConVar_Register(0, this);

	Msg(VSP_LOG_PREFIX "Client plugin loaded successfully. Version: \"" VSP_VERSION "\"\n");
	
	return true;
}

bool DumpClientNetClasses()
{
	ClientClass* pClasses = g_pClientDLL->GetAllClasses();
	if (pClasses == NULL) {
		Error(VSP_LOG_PREFIX "Failed to get pointer to class  \"ClientClass\"\n");

		return false;
	}

	int iClassCount = g_NetProps.DumpClientClasses(pClasses);
	if (iClassCount < 1) {
		Error(VSP_LOG_PREFIX "Failed to save netprops dump!""\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[ClientClass] Get props: %d ""\n", iClassCount);

	return true;
}

void VSPClient::Unload()
{
	g_ParentFix.DestroyDetour();
	g_HltvCameraFix.DestroyDetour();
	g_ModelCrashFix.DestroyDetour();
	g_NetProps.Clear();

	ConVar_Unregister();
}

bool VSPClient::RegisterConCommandBase(ConCommandBase* pVar)
{
	//pVar->AddFlags(FCVAR_CLIENTDLL);

	// Unlink from plugin only list
	// Necessary because RegisterConCommandBase skips the command if it's next isn't null
	pVar->SetNext(0);

	// Link to engine's list instead
	g_pCVar->RegisterConCommand(pVar);

	return true;
}

const char* VSPClient::GetPluginDescription(void)
{
	return "L4D2 SourceTV client fixes. Authors: A1m`. Version: " VSP_VERSION;
}

#if VSP_DEBUG
CON_COMMAND(stv_check_globals, "Check 'CGlobalVars' Instance.")
{
	Msg(VSP_LOG_PREFIX "[CGlobalVars] Some properties are available only after connecting to the server!""\n");
	Msg(VSP_LOG_PREFIX "[CGlobalVars] Instance: %x. MaxClients: %d, Time: %f, frametime: %f !""\n", \
			g_pGlobals, g_pGlobals->maxClients, g_pGlobals->realtime, g_pGlobals->frametime);
}

CON_COMMAND(stv_check_gamerules, "Check 'CTerrorGameRules' Instance.")
{
	CTerrorGameRules* pGameRules = GetGameRules();
	if (pGameRules == NULL) {
		Msg(VSP_LOG_PREFIX "[CTerrorGameRules] Instance not found: %x. This class seems to be unavailable during map change!""\n", pGameRules);

		return;
	}

	Vector vecView = pGameRules->GetViewVectors()->m_vView;

	Msg(VSP_LOG_PREFIX "[CTerrorGameRules] Instance: %x. vecView: %f %f %f !""\n", \
			pGameRules, vecView.x, vecView.y, vecView.z);
}
#endif
