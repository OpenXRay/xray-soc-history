#include "stdafx.h"
#include "xr_func.h"

//----------------------------------------------------------------------
// Function	:
// Purpose	:
//----------------------------------------------------------------------
LPSTR							_ChangeFileExt			( LPCSTR name, LPCSTR ext, LPSTR dest)
{
	strcpy						(dest, name);
    char						*sTmpName = dest;

    // Find the extension
    while((sTmpName[0] != '.') && sTmpName[0] )	sTmpName ++;
    strcpy						(sTmpName, ext);
	return						dest;
}

LPSTR							_GetFileName			( LPCSTR full, LPSTR name )
{
	strcpy						( name, full );
	char *ext					= name;
    // Find the extension
    while((*ext != '.') && *ext )	ext++;
	*ext						= 0;
	return						name;
}
/*
LPSTR							_GetFileExt				( LPCSTR name, LPSTR ext )
{
	char *temp;
	ext[0]						= 0;
    if (temp = _tcsrchr(name, _T('.'))) strcpy			( ext, temp );
	return						temp;
}

//----------------------------------------------------------------------
// Function	:	GetTheta()
// Purpose	:	Returns angle between vectors (gradus)
// Note		:	vectors must be normalized
//----------------------------------------------------------------------
float   						_GetTheta( Fvector* vector1, Fvector* vector2 )
{
	float   					angle					= 0;
	float   					vector_dest				= VECTOR_RANGE(vector1, vector2);

//	angle						= 180 - RADIAN_2_GRADUS(asin( vector_dest / 2 )) * 2;
	angle						= 180 - RADIAN_2_GRADUS(asin( vector_dest / 2 )) * 2;

	return angle;
}

//----------------------------------------------------------------------
// Function	:	GetTheta2()
// Purpose	:	Returns angle between vectors (gradus)
//----------------------------------------------------------------------
float   						_GetThetaCos( Fvector* v1, Fvector* v2 )
{
	float   					angle					= 0;
	float   					cosa					= 0;

	cosa	=	v1->dotproduct(*v2)/			//(v1->x*v2->x + v1->y*v2->y + v1->z*v2->z) /
				(v1->magnitude() * v2->magnitude());		//D3DVAL(sqrt((SQR(v1->x) + SQR(v1->y) + SQR(v1->z))*(SQR(v2->x) + SQR(v2->y) + SQR(v2->z))));

	angle						= -cosa;

	return angle;
}
*/
