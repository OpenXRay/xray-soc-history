#ifndef ShaderTypesH
#define ShaderTypesH

#pragma once

#include "fixedvector.h"

const DWORD			sh_STAGE_MAX	= 8;
const DWORD			sh_PASS_MAX		= 8;

typedef char		sh_name			[64];

class ENGINE_API	CTexture;
class ENGINE_API	CShader;

typedef svector<CTexture*,sh_STAGE_MAX>				tex_vector;
typedef svector<tex_vector,sh_PASS_MAX>				tex_handles;
#endif
