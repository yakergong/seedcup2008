#ifndef __TERISAI_API__
#define __TERISAI_API__

#ifdef TETRISAI_EXPORTS
#define TETRISAI_API __declspec(dllexport)
#else
#define TETRISAI_API __declspec(dllimport)
#endif

#endif