#pragma warning( disable : 4251 )

#ifdef COMPILE_SOLVER
#ifndef SOLVER_EXPORT
#define SOLVER_EXPORT 
//__declspec(dllexport)
#endif
#else
#ifndef SOLVER_EXPORT
#define SOLVER_EXPORT 
//__declspec(dllimport)
#endif
#endif