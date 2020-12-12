/****************************************************************************************/
/*  VKFRAME.C																			*/
/*                                                                                      */
/*  Author: Mike Sandige	                                                            */
/*  Description: Vector keyframe implementation.										*/
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
/* geVKFrame  (Vector-Keyframe)
	This module handles interpolation for keyframes that contain a vector (a geVec3d)
	This is intended to support Path.c
	geTKArray supplies general support for a time-keyed array, and this supplements
	that support to include the two specific time-keyed arrays:
	  An array of geVec3d interpolated linearly
	  An array of geVec3d interpolated with hermite blending
	These are phycially separated and have different base structures because:
		linear blending requires less data.
		future blending might require more data.
	The two types of lists are created with different creation calls,
	interpolated with different calls, but insertion and queries share a call.

	Hermite interpolation requires additional computation after changes are
	made to the keyframe list.  Call geVKFrame_HermiteRecompute() to update the
	calculations.
*/
#include	"stdafx.h"

#include "vec3d.h"
#include "vkframe.h"

#define LINEAR_BLEND(a,b,t)  ( (t)*((b)-(a)) + (a) )
			// linear blend of a and b  0<t<1 where  t=0 ->a and t=1 ->b

typedef struct
{
	geTKArray_TimeType	Time;		// Time for this keyframe
	geVec3d		V;					// vector for this keyframe
}  geVKFrame;
	// This is the root structure that geVKFrame supports
	// all keyframe types must begin with this structure.  Time is first, so
	// that this structure can be manipulated by geTKArray

typedef struct
{
	geVKFrame Key;					// key values for this keyframe
	geVec3d		SDerivative;		// Hermite Derivative (Incoming)
	geVec3d		DDerivative;		// Hermite Derivative (Outgoing)
}	geVKFrame_Hermite;
	// keyframe data for hermite blending
	// The structure includes computed derivative information.

typedef struct
{
	geVKFrame Key;				// key values for this keyframe
}	geVKFrame_Linear;
	// keyframe data for linear interpolation
	// The structure includes no additional information.

geTKArray *GENESISCC geVKFrame_LinearCreate(void)
	// creates a frame list for linear interpolation
{
	return geTKArray_Create(sizeof(geVKFrame_Linear) );
}


geTKArray *GENESISCC geVKFrame_HermiteCreate(void)
	// creates a frame list for hermite interpolation
{
	return geTKArray_Create(sizeof(geVKFrame_Hermite) );
}


BOOL GENESISCC geVKFrame_Insert(
	geTKArray **KeyList,			// keyframe list to insert into
	geTKArray_TimeType Time,		// time of new keyframe
	const geVec3d *V,				// vector at new keyframe
	int *Index)					// index of new key
	// inserts a new keyframe with the given time and vector into the list.
{
	VERIFY( KeyList != NULL );
	VERIFY( *KeyList != NULL );
	VERIFY( V != NULL );
	VERIFY(   sizeof(geVKFrame_Hermite) == geTKArray_ElementSize(*KeyList)
	       || sizeof(geVKFrame_Linear) == geTKArray_ElementSize(*KeyList) );

	if (geTKArray_Insert(KeyList, Time, Index) == FALSE)
		{
//			//geErrorLog_Add(ERR_VKARRAY_INSERT, NULL);
			return FALSE;
		}
	else
		{
			geVKFrame *KF;
			KF = (geVKFrame *)geTKArray_Element(*KeyList,*Index);
			KF->V = *V;
			return TRUE;
		}
}

void GENESISCC geVKFrame_Query(
	const geTKArray *KeyList,		// keyframe list
	int Index,						// index of frame to return
	geTKArray_TimeType *Time,		// time of the frame is returned
	geVec3d *V)						// vector from the frame is returned
	// returns the vector and the time at keyframe[index]
{
	geVKFrame *KF;
	VERIFY( KeyList != NULL );
	VERIFY( Time != NULL );
	VERIFY( V != NULL );
	VERIFY( Index < geTKArray_NumElements(KeyList) );
	VERIFY( Index >= 0 );
	VERIFY(   sizeof(geVKFrame_Hermite) == geTKArray_ElementSize(KeyList)
	       || sizeof(geVKFrame_Linear) == geTKArray_ElementSize(KeyList) );

	KF = (geVKFrame *)geTKArray_Element(KeyList,Index);
	*Time = KF->Time;
	*V    = KF->V;
}


void GENESISCC geVKFrame_Modify(
	geTKArray *KeyList,				// keyframe list
	int Index,						// index of frame to change
	const geVec3d *V)				// vector for the key
	// chganes the vector at keyframe[index]
{
	geVKFrame *KF;
	VERIFY( KeyList != NULL );
	VERIFY( V != NULL );
	VERIFY( Index < geTKArray_NumElements(KeyList) );
	VERIFY( Index >= 0 );
	VERIFY(   sizeof(geVKFrame_Hermite) == geTKArray_ElementSize(KeyList)
	       || sizeof(geVKFrame_Linear) == geTKArray_ElementSize(KeyList) );

	KF = (geVKFrame *)geTKArray_Element(KeyList,Index);
	KF->V = *V;
}


void GENESISCC geVKFrame_LinearInterpolation(
	const void *KF1,		// pointer to first keyframe
	const void *KF2,		// pointer to second keyframe
	float T,				// 0 <= T <= 1   blending parameter
	void *Result)			// put the result in here (geVec3d)
		// interpolates to get a vector between the two vectors at the two
		// keyframes where T==0 returns the vector for KF1
		// and T==1 returns the vector for KF2
		// interpolates linearly
{
	geVec3d *Vec1,*Vec2;
	geVec3d *VNew = (geVec3d *)Result;

	VERIFY( Result != NULL );
	VERIFY( KF1 != NULL );
	VERIFY( KF2 != NULL );

	VERIFY( T >= (float)0.0f );
	VERIFY( T <= (float)1.0f );

	if ( KF1 == KF2 )
		{
			*VNew = ((geVKFrame_Linear *)KF1)->Key.V;
			return;
		}

	Vec1 = &( ((geVKFrame_Linear *)KF1)->Key.V);
	Vec2 = &( ((geVKFrame_Linear *)KF2)->Key.V);

	VNew->X = LINEAR_BLEND(Vec1->X,Vec2->X,T);
	VNew->Y = LINEAR_BLEND(Vec1->Y,Vec2->Y,T);
	VNew->Z = LINEAR_BLEND(Vec1->Z,Vec2->Z,T);
}



void GENESISCC geVKFrame_HermiteInterpolation(
	const void *KF1,		// pointer to first keyframe
	const void *KF2,		// pointer to second keyframe
	float T,				// 0 <= T <= 1   blending parameter
	void *Result)			// put the result in here (geVec3d)
		// interpolates to get a vector between the two vectors at the two
		// keyframes where T==0 returns the vector for KF1
		// and T==1 returns the vector for KF2
		// interpolates using 'hermite' blending
{
	geVec3d *Vec1,*Vec2;
	geVec3d *VNew = (geVec3d *)Result;

	VERIFY( Result != NULL );
	VERIFY( KF1 != NULL );
	VERIFY( KF2 != NULL );

	VERIFY( T >= (float)0.0f );
	VERIFY( T <= (float)1.0f );

	if ( KF1 == KF2 )
		{
			*VNew = ((geVKFrame_Hermite *)KF1)->Key.V;
			return;
		}

	Vec1 = &( ((geVKFrame_Hermite *)KF1)->Key.V);
	Vec2 = &( ((geVKFrame_Hermite *)KF2)->Key.V);

	{
		float	t2;			// T sqaured
		float	t3;			// T cubed
		float   H1,H2,H3,H4;	// hermite basis function coefficients

		t2 = T * T;
		t3 = t2 * T;

		H2 = -(t3 + t3) + t2*3.0f;
		H1 = 1.0f - H2;
		H4 = t3 - t2;
		H3 = H4 - t2 + T;   //t3 - 2.0f * t2 + t;

		geVec3d_Scale(Vec1,H1,VNew);
		geVec3d_AddScaled(VNew,Vec2,H2,VNew);
		geVec3d_AddScaled(VNew,&( ((geVKFrame_Hermite *)KF1)->DDerivative),H3,VNew);
		geVec3d_AddScaled(VNew,&( ((geVKFrame_Hermite *)KF2)->SDerivative),H4,VNew);
	}
}


void GENESISCC geVKFrame_HermiteRecompute(
	int Looped,				 // if keylist has the first key connected to last key
	BOOL ZeroDerivative,// if each key should have a zero derivatives (good for 2 point S curves)
	geTKArray *KeyList)		 // list of keys to recompute hermite values for
	// rebuild precomputed data for keyframe list.
{
	// compute the incoming and outgoing derivatives at each keyframe
	int i;
	geVec3d V0,V1,V2;
	float Time0, Time1, Time2, N0, N1, N0N1;
	geVKFrame_Hermite *TK;
	geVKFrame_Hermite *Vector= NULL;
	int count;
	int Index0,Index1,Index2;

	VERIFY( KeyList != NULL );
	VERIFY( sizeof(geVKFrame_Hermite) == geTKArray_ElementSize(KeyList) );


	// Compute derivatives at the keyframe points:
	// The derivative is the average of the source chord p[i]-p[i-1]
	// and the destination chord p[i+1]-p[i]
	//     (where i is Index1 in this function)
	//  D = 1/2 * ( p[i+1]-p[i-1] ) = 1/2 *( (p[i+1]-p[i]) + (p[i]-p[i-1]) )
	//  The very first and last chords are simply the
	// destination and source derivative.
	//   These 'averaged' D's are adjusted for variences in the time scale
	// between the Keyframes.  To do this, the derivative at each keyframe
	// is split into two parts, an incoming ('source' DS)
	// and an outgoing ('destination' DD) derivative.
	// DD[i] = DD[i] * 2 * N[i]  / ( N[i-1] + N[i] )
	// DS[i] = DS[i] * 2 * N[i-1]/ ( N[i-1] + N[i] )
	//    where N[i] is time between keyframes i and i+1
	// Since the chord dealt with on a given chord between key[i] and key[i+1], only
	// one of the derivates are needed for each keyframe.  For key[i] the outgoing
	// derivative at is needed (DD[i]).  For key[i+1], the incoming derivative
	// is needed (DS[i+1])   ( note that  (1/2) * 2 = 1 )
	count = geTKArray_NumElements(KeyList);
	if (count > 0)
		{
			Vector = (geVKFrame_Hermite *)geTKArray_Element(KeyList,0);
		}

	if (ZeroDerivative!=FALSE)
		{	// in this case, just bang all derivatives to zero.
			for (i =0; i< count; i++)
				{
					TK = &(Vector[i]);
					geVec3d_Clear(&(TK->DDerivative));
					geVec3d_Clear(&(TK->SDerivative));
				}
			return;
		}

	if (count < 3)
		{
			Looped = FALSE;
			// cant compute slopes without a closed loop:
			// so compute slopes as if it is not closed.
		}
	for (i =0; i< count; i++)
		{
			TK = &(Vector[i]);
			Index0 = i-1;
			Index1 = i;
			Index2 = i+1;

			Time1 = Vector[Index1].Key.Time;
			if (Index1 == 0)
				{
					if (Looped != TRUE)
						{
							Index0 = 0;
							Time0 = Vector[Index0].Key.Time;
						}
					else
						{
							Index0 = count-2;
							Time0 = Time1 - (Vector[count-1].Key.Time - Vector[count-2].Key.Time);
						}
				}
			else
				{
					Time0 = Vector[Index0].Key.Time;
				}


			if (Index2 == count)
				{
					if (Looped != TRUE)
						{
							Index2 = count-1;
							Time2 = Vector[Index2].Key.Time;
						}
					else
						{
							Index2 = 1;
							Time2 = Time1 + (Vector[1].Key.Time - Vector[0].Key.Time);
						}
				}
			else
				{
					Time2 = Vector[Index2].Key.Time;
				}

			V0 = Vector[Index0].Key.V;
			V1 = Vector[Index1].Key.V;
			V2 = Vector[Index2].Key.V;

			N0    = (Time1 - Time0);
			N1    = (Time2 - Time1);
			N0N1  = N0 + N1;

			if (( Looped != TRUE) && (Index1 == 0) )
				{
					geVec3d_Subtract(&V2,&V1,&(TK->SDerivative));
					geVec3d_Copy( &(TK->SDerivative), &(TK->DDerivative));
				}
			else if (( Looped != TRUE) && (Index1 == count-1))
				{
					geVec3d_Subtract(&V1,&V0,&(TK->SDerivative));
					geVec3d_Copy( &(TK->SDerivative), &(TK->DDerivative));
				}
			else
			{
				geVec3d Slope;
				geVec3d_Subtract(&V2,&V0,&Slope);
				geVec3d_Scale(&Slope, (N1 / N0N1), &(TK->DDerivative));
				geVec3d_Scale(&Slope, (N0 / N0N1), &(TK->SDerivative));
			}
		}
}


#define VKFRAME_LINEAR_ASCII_FILE 0x4C464B56	// 'VKFL'
#define VKFRAME_HERMITE_ASCII_FILE 0x48464B56	// 'VKFH'
#define CHECK_FOR_READ(uu, nn) if(uu != nn) { /*geErrorLog_Add(ERR_PATH_FILE_READ, NULL);*/  return FALSE; }
#define CHECK_FOR_WRITE(uu) if (uu <= 0)    { /*geErrorLog_Add(ERR_PATH_FILE_WRITE, NULL);*/ return FALSE; }

#define VKFRAME_KEYLIST_ID "Keys"
#define LINEARTIME_TOLERANCE (0.0001f)
#define VKFRAME_LINEARTIME_COMPRESSION 0x2

uint32 GENESISCC geVKFrame_ComputeBlockSize(geTKArray *KeyList, int Compression)
{
	uint32 Size=0;
	int Count;

	VERIFY( KeyList != NULL );
	VERIFY( Compression < 0xFF);

	Count = geTKArray_NumElements(KeyList);

	Size += sizeof(uint32);		// flags
	Size += sizeof(uint32);		// count

	if (Compression & VKFRAME_LINEARTIME_COMPRESSION)
		{
			Size += sizeof(float) * 2;
		}
	else
		{
			Size += sizeof(float) * Count;
		}

	Size += sizeof(geVec3d) * Count;
	return Size;
}

geTKArray *GENESISCC geVKFrame_CreateFromStream(CStream* pStream, int *InterpolationType, int *Looping)
{
	uint32 u;
	int BlockSize;
	int Compression;
	int Count,i;
	int FieldSize;
	char *Block;
	float *Data;
	geTKArray *KeyList;
	geVKFrame_Linear* pLinear0;
	geVKFrame_Linear* pLinear;

	VERIFY( pStream != NULL );
	VERIFY( InterpolationType != NULL );
	VERIFY( Looping != NULL );

	pStream->Read(&BlockSize, sizeof(int));
//	if (geVFile_Read(pFile, &BlockSize, sizeof(int)) == FALSE)
//		{
//			//geErrorLog_AddString(-1,"Failure to read binary VKFrame header", NULL);
//			return NULL;
//		}
	if (BlockSize<0)
		{
			//geErrorLog_AddString(-1,"Bad Blocksize", NULL);
			return NULL;
		}

	Block = (char*)malloc(BlockSize);
	pStream->Read(Block, BlockSize);
//	if(geVFile_Read(pFile, Block, BlockSize) == FALSE)
//		{
//			//geErrorLog_AddString(-1,"Failure to read binary VKFrame header", NULL);
//			return NULL;
//		}
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
			case (VKFRAME_LINEAR):
					FieldSize = sizeof(geVKFrame_Linear);
					break;
			case (VKFRAME_HERMITE):
			case (VKFRAME_HERMITE_ZERO_DERIV):
					FieldSize = sizeof(geVKFrame_Hermite);
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

	pLinear0 = (geVKFrame_Linear*)geTKArray_Element(KeyList, 0);

	pLinear = pLinear0;

	if (Compression & VKFRAME_LINEARTIME_COMPRESSION)
		{
			float fi;
			float fCount = (float)Count;
			float Time,DeltaTime;
			Time = *(Data++);
			DeltaTime = *(Data++);
			for(fi=0.0f;fi<fCount;fi+=1.0f)
				{
					pLinear->Key.Time = Time + fi*DeltaTime;
					pLinear = (geVKFrame_Linear *)  ( ((char *)pLinear) + FieldSize );
				}
		}
	else
		{
			for(i=0;i<Count;i++)
				{
					pLinear->Key.Time = *(Data++);
					pLinear = (geVKFrame_Linear *)  ( ((char *)pLinear) + FieldSize );
				}
		}

	pLinear = pLinear0;
	for(i=0;i<Count;i++)
		{
			pLinear->Key.V = *(geVec3d *)Data;
			Data += 3;
			pLinear = (geVKFrame_Linear *)  ( ((char *)pLinear) + FieldSize );
		}

	switch (*InterpolationType)
		{
			case (VKFRAME_LINEAR):
					break;
			case (VKFRAME_HERMITE):
				geVKFrame_HermiteRecompute(	*Looping, FALSE, KeyList);
					break;
			case (VKFRAME_HERMITE_ZERO_DERIV):
				geVKFrame_HermiteRecompute(	*Looping, TRUE, KeyList);
					break;
			default:
				VERIFY(0);
		}
	_FREE(Block);
	return KeyList;
}
