#ifndef _INCLUDE_GAMEDATA_VSP_H_
#define _INCLUDE_GAMEDATA_VSP_H_

#if defined PLATFORM_WINDOWS
	#define PLATFORM_LIB_EXT "dll"
#elif defined PLATFORM_LINUX
	#define PLATFORM_LIB_EXT "so"
#else
	#error This platform is not supported
#endif

#define CLIENT_MODULE_NAME "client." PLATFORM_LIB_EXT
#define ENGINE_MODULE_NAME "engine." PLATFORM_LIB_EXT

#if SOURCE_ENGINE == SE_LEFT4DEAD2

	#if defined PLATFORM_WINDOWS
		#include "l4d2win_gamedata.h"
	#elif defined PLATFORM_LINUX
		#include "l4d2lin_gamedata.h"
	#else
		#error This platform is not supported for game L4D2
	#endif

#elif SOURCE_ENGINE == SE_LEFT4DEAD

	#if defined PLATFORM_WINDOWS
		#include "l4d1win_gamedata.h"
	#else
		#error This platform is not supported for game L4D1
	#endif

#else
	#error Only games L4D1 and L4D2 are supported
#endif

#endif // _INCLUDE_GAMEDATA_VSP_H_