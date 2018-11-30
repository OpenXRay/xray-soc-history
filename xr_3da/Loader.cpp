// Loader.cpp: implementation of the CLoader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Loader.h"
#include "xrLevel.h"
#include "xr_creator.h"
#include "fstaticrender.h"
#include "x_ray.h"
#include "xr_ini.h"
#include "xr_trims.h"
#include "xr_sndman.h"
#include "3dsoundrender.h"
 
CLoader*	pLoader = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLoader::CLoader()
{

}

CLoader::~CLoader()
{
}
 
BOOL CLoader::Load(char *LDesc, CInifile *INI)
{
}

