#ifndef _INCLUDE_UTIL_VSP_H_
#define _INCLUDE_UTIL_VSP_H_

#include "includes/MemoryUtils/MemoryUtils.h" // HMODULE

#define PTR_NULL (uintptr_t)(void*)NULL

size_t UTIL_StringToSignature(const char* str, char buffer[], size_t maxlength);

uintptr_t UTIL_SignatureToAddress(const char* pModuleName, const char* sSig, size_t* retSigSize = NULL);

//extern MemoryUtils g_MemUtils;

#endif // _INCLUDE_UTIL_VSP_H_