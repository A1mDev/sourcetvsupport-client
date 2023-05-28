#include "plugin_vsp.h"

//ICvar* g_pCvar = NULL;

EXPOSE_SINGLE_INTERFACE(VSPPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS);

bool VSPPlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	/*g_pCVar = static_cast<ICvar*>(interfaceFactory(CVAR_INTERFACE_VERSION, NULL));
	if (g_pCVar == NULL) {
		Error(LOG_PREFIX "Couldn't retrieve interface \"" CVAR_INTERFACE_VERSION "\"\n");

		return false;
	}

	return true;*/
}

const char* L4DTickRate::GetPluginDescription(void)
{
	return "L4D2 SourceTV client fixes" VERSION ", A1m`";
}