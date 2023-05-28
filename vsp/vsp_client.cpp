#include "vsp_client.h"

//ICvar* g_pCvar = NULL;

EXPOSE_SINGLE_INTERFACE(VSPClient, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS);

bool VSPClient::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	/*g_pCVar = static_cast<ICvar*>(interfaceFactory(CVAR_INTERFACE_VERSION, NULL));
	if (g_pCVar == NULL) {
		Error(VSP_LOG_PREFIX "Couldn't retrieve interface \"" CVAR_INTERFACE_VERSION "\"\n");

		return false;
	}*/

	Msg(VSP_LOG_PREFIX "Client plugin loaded successfully. Version: \"" VSP_VERSION "\"\n");

	return true;
}

void VSPClient::Unload(void)
{
	// 
}

const char* VSPClient::GetPluginDescription(void)
{
	return "L4D2 SourceTV client fixes" VSP_VERSION ", A1m`";
}
