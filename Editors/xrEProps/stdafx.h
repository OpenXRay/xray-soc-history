//----------------------------------------------------
// file: stdafx.h
//----------------------------------------------------
#ifndef stdafxH
#define stdafxH

#pragma once   

#include <xrCore.h>

//refs
namespace CDB{
	class MODEL;
};

#include "xrEProps.h"
#include "FolderLib.h"                 

#define ENGINE_API
#define DLL_API		__declspec(dllimport)
#define ECORE_API	__declspec(dllexport)

#include "Defines.h"                 

// libs
#pragma comment		(lib,"xrSoundB.lib")
#pragma comment		(lib,"xrCoreB.lib")
#pragma comment		(lib,"EToolsB.lib")

#endif //stdafxH

