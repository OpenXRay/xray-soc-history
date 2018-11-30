/****************************************************************************************/
/*  VEC3D.H                                                                             */
/*                                                                                      */
/*  Author:                                                                             */
/*  Description: 3D Vector interface                                                    */
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
/*  Genesis3D Version 1.1 released November 15, 1999                                 */
/*  Copyright (C) 1999 WildTangent, Inc. All Rights Reserved           */
/*                                                                                      */
/****************************************************************************************/
#ifndef GE_VEC3D_H
#define GE_VEC3D_H

#include "basetype.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	float X, Y, Z;
} geVec3d;

#ifndef NDEBUG
GENESISAPI	float GENESISCC   geVec3d_GetElement(geVec3d *V, int Index);
#else
	#define geVec3d_GetElement(Vector,Index)  (* ((&((Vector)->X)) +  (Index) ))
#endif

GENESISAPI void GENESISCC		geVec3d_Set(geVec3d *V, float X, float Y, float Z);
GENESISAPI void GENESISCC		geVec3d_Get(const geVec3d *V, float *X, float *Y, float *Z);

GENESISAPI float GENESISCC	geVec3d_DotProduct(const geVec3d *V1, const geVec3d *V2);
GENESISAPI void GENESISCC		geVec3d_CrossProduct(const geVec3d *V1, const geVec3d *V2, geVec3d *VResult);
GENESISAPI BOOL GENESISCC	geVec3d_Compare(const geVec3d *V1, const geVec3d *V2,float tolarance);
GENESISAPI float GENESISCC	geVec3d_Normalize(geVec3d *V1);
GENESISAPI BOOL GENESISCC 	geVec3d_IsNormalized(const geVec3d *V);
GENESISAPI void GENESISCC		geVec3d_Scale(const geVec3d *VSrc, float Scale, geVec3d *VDst);
GENESISAPI float GENESISCC	geVec3d_Length(const geVec3d *V1);
GENESISAPI float GENESISCC	geVec3d_LengthSquared(const geVec3d *V1);
GENESISAPI void GENESISCC		geVec3d_Subtract(const geVec3d *V1, const geVec3d *V2, geVec3d *V1MinusV2);
GENESISAPI void GENESISCC		geVec3d_Add(const geVec3d *V1, const geVec3d *V2,  geVec3d *VSum);
GENESISAPI void GENESISCC		geVec3d_Copy(const geVec3d *Vsrc, geVec3d *Vdst);
GENESISAPI void GENESISCC		geVec3d_Clear(geVec3d *V);
GENESISAPI void GENESISCC		geVec3d_Inverse(geVec3d *V);
GENESISAPI void GENESISCC		geVec3d_MA(geVec3d *V1, float Scale, const geVec3d *V2, geVec3d *V1PlusV2Scaled);
GENESISAPI void GENESISCC		geVec3d_AddScaled(const geVec3d *V1, const geVec3d *V2, float Scale, geVec3d *V1PlusV2Scaled);

GENESISAPI float GENESISCC		geVec3d_DistanceBetween(const geVec3d *V1, const geVec3d *V2);	// returns length of V1-V2

GENESISAPI BOOL GENESISCC geVec3d_IsValid(const geVec3d *V);

#ifdef __cplusplus
}
#endif

#endif

