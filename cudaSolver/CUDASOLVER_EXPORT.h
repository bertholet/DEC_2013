#ifdef COMPILE_CUDASOLVER
	#ifndef CUDASOLVER_EXPORT
	#define CUDASOLVER_EXPORT __declspec(dllexport)
	#endif
#else
	#ifndef CUDASOLVER_EXPORT
	#define CUDASOLVER_EXPORT __declspec(dllimport)
	#endif
#endif