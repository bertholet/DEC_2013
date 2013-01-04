#pragma warning( disable : 4251 )

#ifdef COMPILE_DECCORE
	#ifndef DECCORE_EXPORT
	#define DECCORE_EXPORT __declspec(dllexport)
	#endif
#else
	#ifndef DECCORE_EXPORT
	#define DECCORE_EXPORT __declspec(dllimport)
	#endif
#endif