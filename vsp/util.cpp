#include "util.h"
#include <cstdio>
#include <cstring>
#include "tier0/dbg.h"

//https://github.com/alliedmodders/sourcemod/blob/237db0504c7a59e394828446af3e8ca3d53ef647/extensions/sdktools/vglobals.cpp#L149
size_t UTIL_StringToSignature(const char* str, char buffer[], size_t maxlength)
{
	size_t real_bytes = 0;
	size_t length = strlen(str);

	for (size_t i = 0; i < length; i++) {
		if (real_bytes >= maxlength) {
			break;
		}

		buffer[real_bytes++] = (unsigned char)str[i];
		if (str[i] == '\\' && str[i + 1] == 'x') {
			if (i + 3 >= length) {
				continue;
			}

			/* Get the hex part */
			char s_byte[3];
			int r_byte;
			s_byte[0] = str[i + 2];
			s_byte[1] = str[i + 3];
			s_byte[2] = '\n';
			/* Read it as an integer */
			sscanf(s_byte, "%x", &r_byte);
			/* Save the value */
			buffer[real_bytes - 1] = (unsigned char)r_byte;
			/* Adjust index */
			i += 3;
		}
	}

	return real_bytes;
}

uintptr_t UTIL_SignatureToAddress(const char* pModuleName, const char* sSig, size_t* retSigSize /*= NULL*/)
{
	char sSignatureAddress[256];

#ifdef PLATFORM_WINDOWS
	HMODULE pModule = GetModuleHandle(pModuleName);
	if (pModule == NULL) {
		Error("Unable to open module %s""\n", pModuleName);

		return uintptr_t(NULL);
	}
#elif defined PLATFORM_LINUX
	void* pModule = dlopen(pModuleName, RTLD_NOW);
	if (pModule == NULL) {
		Error("Unable to open module %s""\n", pModuleName);

		return uintptr_t(NULL);
	}
#else
	#error This platform is not supported
#endif

	size_t iSigSize = UTIL_StringToSignature(sSig, sSignatureAddress, sizeof(sSignatureAddress));

	if (retSigSize != NULL) {
		*retSigSize = iSigSize;
	}

	uintptr_t pFunction = uintptr_t(g_MemUtils.FindPattern(pModule, sSignatureAddress, iSigSize));

#ifdef PLATFORM_LINUX
	dlclose(pModule);
#endif

	return pFunction;
}
