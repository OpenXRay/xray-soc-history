#include "stdafx.h"
#include "sh_shader.h"

CShader::CShader()
{
	dwFrame				= 0;
	Flags.iPriority		= 1;
	Flags.bLighting		= FALSE;
	Flags.bStrictB2F	= FALSE;
	Flags.bPixelShader	= FALSE;
}

CShader::~CShader()
{

}
