#pragma once

#ifdef SERVER_LIB_DLL
#define SERVER_LIB_DLL_EXPORT __declspec(dllexport)
#else
#define SERVER_LIB_DLL_EXPORT __declspec(dllimport)
#endif

