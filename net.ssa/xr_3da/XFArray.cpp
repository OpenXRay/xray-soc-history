/****************************************************************************************/
/*  XFARRAY.C																			*/
/*                                                                                      */
/*  Author: Mike Sandige	                                                            */
/*  Description: Array of transforms implementation.									*/
/*                                                                                      */
/*  The contents of this file are subject to the Genesis3D Public License               */
/*  Version 1.01 (the "License"); you may not use this file except in                   */
/*  compliance with the License. You may obtain a copy of the License at                */
/*  http://www.genesis3d.com                                                            */
/*                                                                                      */
/*  Software distributed under the License is distributed on an "AS IS"                 */
/*  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See                */
/*  the License for the specific language governing rights and limitations              */
/*  under the License.                                                                  */
/*                                                                                      */
/*  The Original Code is Genesis3D, released March 25, 1999.                            */
/*Genesis3D Version 1.1 released November 15, 1999                            */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/****************************************************************************************/
#include	"stdafx.h"
#include "XFArray.h"


typedef struct geXFArray
{
	int		 TransformCount;
	geXForm3d *TransformArray;
} geXFArray;

geXFArray *GENESISCC geXFArray_Create(int Size)
{
	geXFArray *XFA;

	VERIFY( Size > 0 );

	XFA = (geXFArray*)malloc( sizeof(geXFArray) );
	if (XFA == NULL)
		{
//			geErrorLog_Add( ERR_XFARRAY_ENOMEM , NULL);
			return NULL;
		}
	XFA->TransformArray = (geXForm3d*)malloc(sizeof(geXForm3d)*Size);
	XFA->TransformCount = Size;
	{
		geXForm3d X;
		geXForm3d_SetIdentity(&X);
		geXFArray_SetAll(XFA,&X);
	}
	return XFA;
}

void GENESISCC geXFArray_Destroy( geXFArray **XFA )
{
	VERIFY( XFA != NULL );
	VERIFY( *XFA != NULL );
	VERIFY( (*XFA)->TransformCount > 0 );
	VERIFY( (*XFA)->TransformArray != NULL );

	(*XFA)->TransformCount = -1;
	_FREE( (*XFA)->TransformArray);
	(*XFA)->TransformArray = NULL;
	_FREE( (*XFA) );
	(*XFA) = NULL;
}

geXForm3d *GENESISCC geXFArray_GetElements(const geXFArray *XFA, int *Size)
{
	VERIFY( XFA != NULL );
	VERIFY( Size != NULL );
	VERIFY( XFA->TransformCount > 0 );
	VERIFY( XFA->TransformArray != NULL );

	*Size = XFA->TransformCount;
	return XFA->TransformArray;
}

void GENESISCC geXFArray_SetAll(geXFArray *XFA, const geXForm3d *Matrix)
{
	VERIFY( XFA != NULL );
	VERIFY( Matrix != NULL );
	VERIFY( XFA->TransformCount > 0 );
	VERIFY( XFA->TransformArray != NULL );
	{
		int i;
		geXForm3d *X;
		for (i=0,X=XFA->TransformArray; i<XFA->TransformCount; i++,X++)
			{
				*X = *Matrix;
			}
	}
}
