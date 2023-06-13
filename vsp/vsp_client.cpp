#include "vsp_client.h"
#include "fixes/fixes.h"
#include "convar.h"

CSetParentFix g_ParentFix;

ICvar* g_pCvar = NULL;

EXPOSE_SINGLE_INTERFACE(VSPClient, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS);

bool VSPClient::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	g_pCVar = static_cast<ICvar*>(interfaceFactory(CVAR_INTERFACE_VERSION, NULL));
	if (g_pCVar == NULL) {
		Error(VSP_LOG_PREFIX "Couldn't retrieve interface \"" CVAR_INTERFACE_VERSION "\"\n");

		return false;
	}

	Msg(VSP_LOG_PREFIX "[ICvar] received interface: %x ""\n", g_pCVar);
	
	HMODULE clientdll = GetModuleHandle("client.dll");

	if (!g_ParentFix.CreateDetour(clientdll)) {
		return false;
	}

	ConVar_Register(0, this);
	Msg(VSP_LOG_PREFIX "Client plugin loaded successfully. Version: \"" VSP_VERSION "\"\n");
	
	return true;
}

void VSPClient::Unload(void)
{
	g_ParentFix.DestroyDetour();
	ConVar_Unregister();
}

bool VSPClient::RegisterConCommandBase(ConCommandBase* pVar)
{
	pVar->AddFlags(FCVAR_CLIENTDLL);

	// Unlink from plugin only list
	// Necessary because RegisterConCommandBase skips the command if it's next isn't null
	pVar->SetNext(0);

	// Link to engine's list instead
	g_pCVar->RegisterConCommand(pVar);
	return true;
}

const char* VSPClient::GetPluginDescription(void)
{
	return "L4D2 SourceTV client fixes" VSP_VERSION ", A1m`";
}
