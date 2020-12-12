/****************************************************************************************/
/*  QKFRAME.H																			*/
/*                                                                                      */
/*  Author: Mike Sandige	                                                            */
/*  Description: Quaternion keyframe implementation.									*/
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
/* geQKFrame   (geQuaternion - Keyframe)
	This module handles interpolation for keyframes that contain a quaternion
	This is intended to support Path.c
	geTKArray supplies general support for a time-keyed array, and this supplements
	that support to include the specific time-keyed arrays:
	  An array of geQuaternion interpolated linearly
	  An array of geQuaternion with spherical linear interpolation (SLERP)
	  An array of geQuaternion with spherical quadrangle
		interpolation (SQUAD) as defined by:
	    Advanced Animation and Rendering Techniques by Alan Watt and Mark Watt

	These are phycially separated and have different base structures because
	the different interpolation techniques requre different additional data.

	The two lists are created with different creation calls,
	interpolated with different calls, but insertion and queries share a call.

	Quadrangle interpolation requires additional computation after changes are
	made to the keyframe list.  Call geQKFrame_SquadRecompute() to update the
	calculations.
*/
#include	"stdafx.h"

#include "vec3d.h"
#include "qkframe.h"

#define LINEAR_BLEND(a,b,t)  ( (t)*((b)-(a)) + (a) )
			// linear blend of a and b  0<t<1 where  t=0 ->a and t=1 ->b

typedef struct
{
	geTKArray_TimeType	Time;				// Time for this keyframe
	geQuaternion	Q;					// quaternion for this keyframe
}  QKeyframe;
	// This is the root structure that geQKFrame supports
	// all keyframe types must begin with this structure.  Time is first, so
	// that this structure can be manipulated by geTKArray

typedef struct
{
	QKeyframe Key;				// key values for this keyframe
}	geQKFrame_Linear;
	// keyframe data for linear interpolation
	// The structure includes no additional information.

typedef struct
{
	QKeyframe Key;				// key values for this keyframe
}	geQKFrame_Slerp;
	// keyframe data for spherical linear interpolation
	// The structure includes no additional information.

typedef struct
{
	QKeyframe Key;				// key values for this keyframe
	geQuaternion  QuadrangleCorner;
}	geQKFrame_Squad;
	// keyframe data for spherical quadratic interpolation


geTKArray *GENESISCC geQKFrame_LinearCreate()
	// creates a frame list for linear interpolation
{
	return geTKArray_Create(sizeof(geQKFrame_Linear) );
}


geTKArray *GENESISCC geQKFrame_SlerpCreate()
	// creates a frame list for spherical linear interpolation
{
	return geTKArray_Create(sizeof(geQKFrame_Slerp) );
}

geTKArray *GENESISCC geQKFrame_SquadCreate()
	// creates a frame list for spherical linear interpolation
{
	return geTKArray_Create(sizeof(geQKFrame_Squad) );
}


BOOL GENESISCC geQKFrame_Insert(
	geTKArray **KeyList,			// keyframe list to insert into
	geTKArray_TimeType Time,		// time of new keyframe
	const geQuaternion *Q,			// quaternion at new keyframe
	int *Index)						// index of new key
	// inserts a new keyframe with the given time and vector into the list.
{
	VERIFY( KeyList != NULL );
	VERIFY( *KeyList != NULL );
	VERIFY( Q != NULL );
	VERIFY(   sizeof(geQKFrame_Squad) == geTKArray_ElementSize(*KeyList)
	       || sizeof(geQKFrame_Slerp) == geTKArray_ElementSize(*KeyList)
		   || sizeof(geQKFrame_Linear) == geTKArray_ElementSize(*KeyList) );

	if (geTKArray_Insert(KeyList, Time, Index) == FALSE)
		{
//			geErrorLog_Add(ERR_QKARRAY_INSERT, NULL);
			return FALSE;
		}
	else
		{
			QKeyframe *KF;
			KF = (QKeyframe *)geTKArray_Element(*KeyList,*Index);
			KF->Q = *Q;
			return TRUE;
		}
}

void GENESISCC geQKFrame_Query(
	const geTKArray *KeyList,		// keyframe list
	int Index,						// index of frame to return
	geTKArray_TimeType *Time,		// time of the frame is returned
	geQuaternion *Q)					// vector from the frame is returned
	// returns the vector and the time at keyframe[index]
{
	QKeyframe *KF;
	VERIFY( KeyList != NULL );
	VERIFY( Time != NULL );
	VERIFY( Q != NULL );
	VERIFY( Index < geTKArray_NumElements(KeyList) );
	VERIFY( Index >= 0 );
	VERIFY(   sizeof(geQKFrame_Squad) == geTKArray_ElementSize(KeyList)
	       || sizeof(geQKFrame_Slerp) == geTKArray_ElementSize(KeyList)
		   || sizeof(geQKFrame_Linear) == geTKArray_ElementSize(KeyList) );

	KF = (QKeyframe *)geTKArray_Element(KeyList,Index);
	*Time = KF->Time;
	*Q    = KF->Q;
}

void GENESISCC geQKFrame_Modify(
	geTKArray *KeyList,				// keyframe list
	int Index,						// index of frame to change
	const geQuaternion *Q)			// vector for the new key
{
	QKeyframe *KF;
	VERIFY( KeyList != NULL );
	VERIFY( Q != NULL );
	VERIFY( Index < geTKArray_NumElements(KeyList) );
	VERIFY( Index >= 0 );
	VERIFY(   sizeof(geQKFrame_Squad) == geTKArray_ElementSize(KeyList)
	       || sizeof(geQKFrame_Slerp) == geTKArray_ElementSize(KeyList)
		   || sizeof(geQKFrame_Linear) == geTKArray_ElementSize(KeyList) );

	KF = (QKeyframe *)geTKArray_Element(KeyList,Index);
	KF->Q  = *Q;
}



void GENESISCC geQKFrame_LinearInterpolation(
	const void *KF1,		// pointer to first keyframe
	const void *KF2,		// pointer to second keyframe
	float T,				// 0 <= T <= 1   blending parameter
	void *Result)			// put the result in here (geQuaternion)
		// interpolates to get a vector between the two vectors at the two
		// keyframes where T==0 returns the vector for KF1
		// and T==1 returns the vector for KF2
		// interpolates linearly
{
	geQuaternion *Q1,*Q2;
	geQuaternion *QNew = (geQuaternion *)Result;

	VERIFY( Result != NULL );
	VERIFY( KF1 != NULL );
	VERIFY( KF2 != NULL );

	VERIFY( T >= (float)0.0f );
	VERIFY( T <= (float)1.0f );

	if ( KF1 == KF2 )
		{
			*QNew = ((geQKFrame_Linear *)KF1)->Key.Q;
			return;
		}

	Q1 = &( ((geQKFrame_Linear *)KF1)->Key.Q);
	Q2 = &( ((geQKFrame_Linear *)KF2)->Key.Q);

	QNew->X = LINEAR_BLEND(Q1->X,Q2->X,T);
	QNew->Y = LINEAR_BLEND(Q1->Y,Q2->Y,T);
	QNew->Z = LINEAR_BLEND(Q1->Z,Q2->Z,T);
	QNew->W = LINEAR_BLEND(Q1->W,Q2->W,T);
	if (geQuaternion_Normalize(QNew)==0.0f)
		{
			geQuaternion_SetNoRotation(QNew);
		}

}



void GENESISCC geQKFrame_SlerpInterpolation(
	const void *KF1,		// pointer to first keyframe
	const void *KF2,		// pointer to second keyframe
	float T,				// 0 <= T <= 1   blending parameter
	void *Result)			// put the result in here (geQuaternion)
		// interpolates to get a vector between the two vectors at the two
		// keyframes where T==0 returns the vector for KF1
		// and T==1 returns the vector for KF2
		// interpolates using spherical linear blending
{
	geQuaternion *Q1,*Q2;
	geQuaternion *QNew = (geQuaternion *)Result;

	VERIFY( Result != NULL );
	VERIFY( KF1 != NULL );
	VERIFY( KF2 != NULL );

	VERIFY( T >= (float)0.0f );
	VERIFY( T <= (float)1.0f );

	if ( KF1 == KF2 )
		{
			*QNew = ((geQKFrame_Slerp *)KF1)->Key.Q;
			return;
		}

	Q1 = &( ((geQKFrame_Slerp *)KF1)->Key.Q);
	Q2 = &( ((geQKFrame_Slerp *)KF2)->Key.Q);
	geQuaternion_SlerpNotShortest(Q1,Q2,T,QNew);
}




void GENESISCC geQKFrame_SquadInterpolation(
	const void *KF1,		// pointer to first keyframe
	const void *KF2,		// pointer to second keyframe
	float T,				// 0 <= T <= 1   blending parameter
	void *Result)			// put the result in here (geQuaternion)
		// interpolates to get a vector between the two vectors at the two
		// keyframes where T==0 returns the vector for KF1
		// and T==1 returns the vector for KF2
		// interpolates using spherical quadratic blending
{
	geQuaternion *Q1,*Q2;
	geQuaternion *QNew = (geQuaternion *)Result;

	VERIFY( Result != NULL );
	VERIFY( KF1 != NULL );
	VERIFY( KF2 != NULL );

	VERIFY( T >= (float)0.0f );
	VERIFY( T <= (float)1.0f );

	if ( KF1 == KF2 )
		{
			*QNew = ((geQKFrame_Squad *)KF1)->Key.Q;
			return;
		}

	Q1 = &( ((geQKFrame_Squad *)KF1)->Key.Q);
	Q2 = &( ((geQKFrame_Squad *)KF2)->Key.Q);

	{
		geQuaternion *A1,*B2;
		geQuaternion SL1,SL2;

		A1 = &( ((geQKFrame_Squad *)KF1)->QuadrangleCorner);
		B2 = &( ((geQKFrame_Squad *)KF2)->QuadrangleCorner);

		geQuaternion_SlerpNotShortest(Q1,   Q2,   T, &SL1);
				VERIFY( geQuaternion_IsUnit(&SL1) == TRUE);
		geQuaternion_SlerpNotShortest(A1,   B2,   T, &SL2);
				VERIFY( geQuaternion_IsUnit(&SL2) == TRUE);
		geQuaternion_SlerpNotShortest(&SL1, &SL2, (2.0f*T*(1.0f-T)), QNew);
				VERIFY( geQuaternion_IsUnit(QNew) == TRUE);
	}
}


static void GENESISCC geQKFrame_QuadrangleCorner(
	const geQuaternion *Q0,
	const geQuaternion *Q1,
	const geQuaternion *Q2,
	geQuaternion *Corner)
	// compute quadrangle corner for a keyframe containing Q1.
	//  Q0 and Q2 are the quaternions for the previous and next keyframes
	// corner is the newly computed quaternion
{
	geQuaternion Q1Inv,LnSum;

	VERIFY( Q0 != NULL );
	VERIFY( Q1 != NULL );
	VERIFY( Q2 != NULL );
	VERIFY( Corner != NULL );

	VERIFY( geQuaternion_IsUnit(Q1) == TRUE );

	Q1Inv.W = Q1->W;
	Q1Inv.X = -Q1->X;
	Q1Inv.Y = -Q1->Y;
	Q1Inv.Z = -Q1->Z;

	{
		geQuaternion Q1InvQ2, Q1InvQ0;
		geQuaternion Ln1,Ln2;

		geQuaternion_Multiply(&Q1Inv,Q2,&Q1InvQ2);
		geQuaternion_Multiply(&Q1Inv,Q0,&Q1InvQ0);
		geQuaternion_Ln(&Q1InvQ0,&Ln1);
		geQuaternion_Ln(&Q1InvQ2,&Ln2);
		geQuaternion_Add(&Ln1,&Ln2,&LnSum);
		geQuaternion_Scale(&LnSum,-0.25f,&LnSum);
	}

	geQuaternion_Exp(&LnSum,Corner);
	geQuaternion_Multiply(Q1,Corner,Corner);
}

static void GENESISCC geQKFrame_ChooseBestQuat(const geQuaternion *Q0,geQuaternion *Q1)
	// adjusts the sign of Q1:  to either Q1 or -Q1
	// adjusts Q1 such that Q1 is the 'closest' of the two choices to Q0.
{
	geQuaternion pLessQ,pPlusQ;
	float MagpLessQ,MagpPlusQ;

	VERIFY( Q0 != NULL );
	VERIFY( Q1 != NULL );

	geQuaternion_Add(Q0,Q1,&pPlusQ);
	geQuaternion_Subtract(Q0,Q1,&pLessQ);

	geQuaternion_Multiply(&pPlusQ,&pPlusQ,&pPlusQ);
	geQuaternion_Multiply(&pLessQ,&pLessQ,&pLessQ);

	MagpLessQ=   (pLessQ.W * pLessQ.W) + (pLessQ.X * pLessQ.X)
					  + (pLessQ.Y * pLessQ.Y) + (pLessQ.Z * pLessQ.Z);

	MagpPlusQ=   (pPlusQ.W * pPlusQ.W) + (pPlusQ.X * pPlusQ.X)
					  + (pPlusQ.Y * pPlusQ.Y) + (pPlusQ.Z * pPlusQ.Z);

	if (MagpLessQ >= MagpPlusQ)
		{
			Q1->X = -Q1->X;
			Q1->Y = -Q1->Y;
			Q1->Z = -Q1->Z;
			Q1->W = -Q1->W;
		}
}




void GENESISCC geQKFrame_SquadRecompute(
	int Looped,				// if keylist has the first key connected to last key
	geTKArray *KeyList)		// list of keys to recompute hermite values for
	// rebuild precomputed data for keyframe list.
{

	// compute the extra interpolation points at each keyframe
	// see Advanced Animation and Rendering Techniques
	//     by Alan Watt and Mark Watt, pg 366
	int i;
	geQKFrame_Squad *QList=NULL;
	int count;

	int Index0,Index1,Index2;
	VERIFY( KeyList != NULL );

	count = geTKArray_NumElements(KeyList);

	if (count > 0)
		{
			QList = (geQKFrame_Squad *)geTKArray_Element(KeyList,0);

			for (i =0; i< count-1; i++)
				{
					geQKFrame_ChooseBestQuat(&(QList[i].Key.Q),&(QList[i+1].Key.Q) );
				}
		}

	if (count<3)
		{
			Looped = 0;
			// cant compute 'slopes' without enough points to loop.
			// so treat path as non-looped.
		}
	for (i =0; i< count; i++)
		{
			Index0 = i-1;
			Index1 = i;
			Index2 = i+1;

			if (Index1 == 0)
				{
					if (Looped != TRUE)
						{
							Index0 = 0;
						}
					else
						{
							Index0 = count-2;
						}
				}

			if (Index2 == count)
				{
					if (Looped != TRUE)
						{
							Index2 = count-1;
						}
					else
						{
							Index2 = 1;
						}
				}

			if (( Looped != TRUE) && (Index1 == 0) )
				{
					geQuaternion_Copy(
						&(QList[i].Key.Q),
						&(QList[i].QuadrangleCorner) );
				}
			else if (( Looped != TRUE) && (Index1 == count-1))
				{
					geQuaternion_Copy(
						&(QList[i].Key.Q),
						&(QList[i].QuadrangleCorner) );
				}
			else
			{
				geQKFrame_QuadrangleCorner(
					&(QList[Index0].Key.Q),
					&(QList[Index1].Key.Q),
					&(QList[Index2].Key.Q),
					&(QList[i].QuadrangleCorner) );

			}
		}
}



void GENESISCC geQKFrame_SlerpRecompute(
	geTKArray *KeyList)		// list of keys to recompute hermite values for
	// rebuild precomputed data for keyframe list.
	// also make sure that each successive key is the 'closest' quaternion choice
	// to the previous one.
{

	int i;
	geQKFrame_Slerp *QList;
	int count;
	VERIFY( KeyList != NULL );

	count = geTKArray_NumElements(KeyList);

	if (count > 0)
		{
			QList = (geQKFrame_Slerp  *)geTKArray_Element(KeyList,0);
			for (i =0; i< count-1; i++)
				{
					geQKFrame_ChooseBestQuat(&(QList[i].Key.Q),&(QList[i+1].Key.Q) );
				}
		}
}


#define QKFRAME_LINEAR_ASCII_FILE 0x4C464B51	// 'QKFL'
#define QKFRAME_SLERP_ASCII_FILE 0x53464B51		// 'QKFS'
#define QKFRAME_SQUAD_ASCII_FILE 0x51464B51		// 'QKFQ'
#define CHECK_FOR_WRITE(uu) if(uu <= 0)     { /*geErrorLog_Add(ERR_PATH_FILE_WRITE, NULL);*/ return FALSE; }
#define CHECK_FOR_READ(uu, nn) if(uu != nn) { /*geErrorLog_Add(ERR_PATH_FILE_READ, NULL);*/  return FALSE; }

#define QKFRAME_KEYLIST_ID "Keys"
//------------------------------------------------------------------------
#define QKFRAME_HINGE_COMPRESSION 0x1
#define QKFRAME_LINEARTIME_COMPRESSION 0x2


#define HINGE_TOLERANCE (0.0001f)
#define LINEARTIME_TOLERANCE (0.0001f)

static BOOL GENESISCC geQKFrame_PathIsHinged(geTKArray *KeyList, float Tolerance)
{
	int i,Count;
	geVec3d Axis;
	geVec3d NextAxis;
	float Angle;
	geQKFrame_Linear* pLinear;

	VERIFY( KeyList != NULL );

	Count = geTKArray_NumElements(KeyList);

	if (Count<2)
		return FALSE;
	pLinear = (geQKFrame_Linear*)geTKArray_Element(KeyList, 0);
	if (geQuaternion_GetAxisAngle(&(pLinear->Key.Q),&Axis,&Angle)==FALSE)
		{
			return FALSE;
		}

	for (i=1; i<Count; i++)
		{
			pLinear = (geQKFrame_Linear*)geTKArray_Element(KeyList, i);
			if (geQuaternion_GetAxisAngle(&(pLinear->Key.Q),&NextAxis,&Angle)==FALSE)
				{
					return FALSE;
				}

			if (geVec3d_Compare(&Axis,&NextAxis,Tolerance) == FALSE)
				{
					return FALSE;
				}
		}
	return TRUE;
}


static int GENESISCC geQKFrame_DetermineCompressionType(geTKArray *KeyList)
{
	int Compression=0;
	int NumElements=0;

	VERIFY( KeyList != NULL );

	NumElements = geTKArray_NumElements(KeyList);

	if (NumElements>2)
		{
			if ( geTKArray_SamplesAreTimeLinear(KeyList,LINEARTIME_TOLERANCE) != FALSE )
				{
					Compression |= QKFRAME_LINEARTIME_COMPRESSION;
				}
		}


	if (NumElements>3)
		{
			 if ( geQKFrame_PathIsHinged(KeyList,HINGE_TOLERANCE)!=FALSE )
				{
					Compression |= QKFRAME_HINGE_COMPRESSION;
				}
		}

	return Compression;
}

uint32 GENESISCC geQKFrame_ComputeBlockSize(geTKArray *KeyList, int Compression)
{
	uint32 Size=0;
	int Count;
	VERIFY( KeyList != NULL );
	VERIFY( Compression < 0xFF);

	Count = geTKArray_NumElements(KeyList);

	Size += sizeof(uint32);		// flags
	Size += sizeof(uint32);		// count

	if (Compression & QKFRAME_LINEARTIME_COMPRESSION)
		{
			Size += sizeof(float) * 2;
		}
	else
		{
			Size += sizeof(float) * Count;
		}

	switch (Compression & (~QKFRAME_LINEARTIME_COMPRESSION) )
		{
			case 0:
				Size += sizeof(geQuaternion) * Count;
				break;
			case QKFRAME_HINGE_COMPRESSION:
				Size += (sizeof(float) * 3) + sizeof(float) * Count;
				break;
			default:
				VERIFY(0);
		}
	return Size;
}

geTKArray *GENESISCC geQKFrame_CreateFromStream(CStream* pStream, int *InterpolationType, int *Looping)
{
	uint32 u;
	int BlockSize;
	int Compression;
	int Count,i;
	int FieldSize;
	char *Block;
	float *Data;
	geTKArray *KeyList;
	geQKFrame_Linear* pLinear0;
	geQKFrame_Linear* pLinear;

	VERIFY( pStream != NULL );
	VERIFY( InterpolationType != NULL );
	VERIFY( Looping != NULL );

	pStream->Read(&BlockSize, sizeof(int));
//	if (geVFile_Read(pFile, &BlockSize, sizeof(int)) == FALSE)
//		{
//			geErrorLog_AddString(-1,"Failure to read binary QKFrame header", NULL);
//			return NULL;
//		}
	if (BlockSize<0)
		{
//			geErrorLog_AddString(-1,"Bad Blocksize", NULL);
			return NULL;
		}

	Block = (char*)malloc(BlockSize);
	pStream->Read(Block, BlockSize);
	u = *(uint32 *)Block;
	*InterpolationType = (u>>16)& 0xFF;
	Compression = (u>>8) & 0xFF;
	*Looping           = (u & 0x1);
	Count = *(((uint32 *)Block)+1);

	if (Compression > 0xFF)
		{
			_FREE(Block);
			//geErrorLog_AddString(-1,"Bad Compression Flag", NULL);
			return NULL;
		}
	switch (*InterpolationType)
		{
			case (QKFRAME_LINEAR):
				FieldSize = sizeof(geQKFrame_Linear);
				break;
			case (QKFRAME_SLERP):
				FieldSize = sizeof(geQKFrame_Slerp);
				break;
			case (QKFRAME_SQUAD):
				FieldSize = sizeof(geQKFrame_Squad);
				break;
			default:
				_FREE(Block);
				//geErrorLog_AddString(-1,"Bad InterpolationType", NULL);
				return NULL;
		}

	KeyList = geTKArray_CreateEmpty(FieldSize,Count);
	if (KeyList == NULL)
		{
			_FREE(Block);
			//geErrorLog_AddString(-1,"Failed to allocate tkarray", NULL);
			return NULL;
		}

	Data = (float *)(Block + sizeof(uint32)*2);

	pLinear0 = (geQKFrame_Linear*)geTKArray_Element(KeyList, 0);

	pLinear = pLinear0;

	if (Compression & QKFRAME_LINEARTIME_COMPRESSION)
		{
			float fi;
			float fCount = (float)Count;
			float Time,DeltaTime;
			Time = *(Data++);
			DeltaTime = *(Data++);
			for(fi=0.0f;fi<fCount;fi+=1.0f)
				{
					pLinear->Key.Time = Time + fi*DeltaTime;
					pLinear = (geQKFrame_Linear *)  ( ((char *)pLinear) + FieldSize );
				}
		}
	else
		{
			for(i=0;i<Count;i++)
				{
					pLinear->Key.Time = *(Data++);
					pLinear = (geQKFrame_Linear *)  ( ((char *)pLinear) + FieldSize );
				}
		}

	pLinear = pLinear0;

	if (Compression & QKFRAME_HINGE_COMPRESSION)
		{
			geVec3d Hinge;
			Hinge.X = *(Data++);
			Hinge.Y = *(Data++);
			Hinge.Z = *(Data++);

			for(i=0;i<Count;i++)
				{
					geQuaternion_SetFromAxisAngle(&(pLinear->Key.Q),&Hinge,*(Data++));
					pLinear = (geQKFrame_Linear *)  ( ((char *)pLinear) + FieldSize );
				}
		}
	else
		{
			for(i=0;i<Count;i++)
				{
					pLinear->Key.Q = *(geQuaternion *)Data;
					Data += 4;
					pLinear = (geQKFrame_Linear *)  ( ((char *)pLinear) + FieldSize );
				}
		}

	switch (*InterpolationType)
		{
			case (QKFRAME_LINEAR):
					break;
			case (QKFRAME_SLERP):
				geQKFrame_SlerpRecompute( KeyList);
					break;
			case (QKFRAME_SQUAD):
				geQKFrame_SquadRecompute( *Looping, KeyList);
					break;
			default:
				VERIFY(0);
		}
	_FREE(Block);
	return KeyList;
}
