/****************************************************************************************/
/*  QUATERN.C                                                                           */
/*                                                                                      */
/*  Author: Mike Sandige                                                                */
/*  Description: Quaternion mathematical system implementation                          */
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
#include "basetype.h"
#include "quatern.h"


#ifndef NDEBUG
	static geQuaternion_MaximalAssertionMode = TRUE;
	#define geQuaternion_Assert if (geQuaternion_MaximalAssertionMode) VERIFY

	void GENESISCC geQuaternion_SetMaximalAssertionMode( BOOL Enable )
	{
		VERIFY( (Enable == TRUE) || (Enable == FALSE) );
		geQuaternion_MaximalAssertionMode = Enable;
	}
#else
	#define geQuaternion_Assert VERIFY
#endif

#define UNIT_TOLERANCE 0.001
	// Quaternion magnitude must be closer than this tolerance to 1.0 to be
	// considered a unit quaternion

#define QZERO_TOLERANCE 0.00001
	// quaternion magnitude must be farther from this tolerance to 0.0 to be
	// normalized

#define TRACE_QZERO_TOLERANCE 0.1
	// trace of matrix must be greater than this to be used for converting a matrix
	// to a quaternion.

#define AA_QZERO_TOLERANCE 0.0001


BOOL GENESISCC geQuaternion_IsValid(const geQuaternion *Q)
{
	if (Q == NULL)
		return FALSE;
	if ((Q->W * Q->W) < 0.0f)
		return FALSE;
	if ((Q->X * Q->X) < 0.0f)
		return FALSE;
	if ((Q->Y * Q->Y) < 0.0f)
		return FALSE;
	if ((Q->Z * Q->Z) < 0.0f)
		return FALSE;
	return TRUE;
}

void GENESISCC geQuaternion_Set(
	geQuaternion *Q, float W, float X, float Y, float Z)
{
	VERIFY( Q != NULL );

	Q->W = W;
	Q->X = X;
	Q->Y = Y;
	Q->Z = Z;
	VERIFY( geQuaternion_IsValid(Q) != FALSE );
}

void GENESISCC geQuaternion_SetVec3d(
	geQuaternion *Q, float W, const geVec3d *V)
{
	VERIFY( Q != NULL );
	VERIFY( geVec3d_IsValid(V) != FALSE );

	Q->W = W;
	Q->X = V->X;
	Q->Y = V->Y;
	Q->Z = V->Z;
}

void GENESISCC geQuaternion_Get(
	const geQuaternion *Q,
	float *W,
	float *X,
	float *Y,
	float *Z)
	// get quaternion components into W,X,Y,Z
{
	VERIFY( geQuaternion_IsValid(Q) != FALSE );
	VERIFY( W != NULL );
	VERIFY( X != NULL );
	VERIFY( Y != NULL );
	VERIFY( Z != NULL );

	*W = Q->W;
	*X = Q->X;
	*Y = Q->Y;
	*Z = Q->Z;
}

GENESISAPI void GENESISCC geQuaternion_SetFromAxisAngle(geQuaternion *Q, const geVec3d *Axis, float Theta)
	// set a quaternion from an axis and a rotation around the axis
{
	float sinTheta;
	VERIFY( Q != NULL);
	VERIFY( geVec3d_IsValid(Axis) != FALSE);
	VERIFY( (Theta * Theta) >= 0.0f );
	VERIFY( ( fabs(geVec3d_Length(Axis)-1.0f) < AA_QZERO_TOLERANCE) );

	Theta = Theta * (float)0.5f;
	Q->W     = (float) cos(Theta);
	sinTheta = (float) sin(Theta);
	Q->X = sinTheta * Axis->X;
	Q->Y = sinTheta * Axis->Y;
	Q->Z = sinTheta * Axis->Z;

	geQuaternion_Assert( geQuaternion_IsUnit(Q) == TRUE );
}


BOOL GENESISCC geQuaternion_GetAxisAngle(const geQuaternion *Q, geVec3d *Axis, float *Theta)
{
	float OneOverSinTheta;
	float HalfTheta;
	VERIFY( Q != NULL );
	VERIFY( Axis != NULL );
	VERIFY( Theta != NULL );
	geQuaternion_Assert( geQuaternion_IsUnit(Q) != FALSE );

	HalfTheta  = (float)acos( Q->W );
	if (HalfTheta>QZERO_TOLERANCE)
		{
			OneOverSinTheta = 1.0f / (float)sin( HalfTheta );
			Axis->X = OneOverSinTheta * Q->X;
			Axis->Y = OneOverSinTheta * Q->Y;
			Axis->Z = OneOverSinTheta * Q->Z;
			*Theta = 2.0f * HalfTheta;
			geQuaternion_Assert( geVec3d_IsValid(Axis) != FALSE );
			geQuaternion_Assert( (*Theta * *Theta) >= 0.0f);
			return TRUE;
		}
	else
		{
			Axis->X = Axis->Y = Axis->Z = 0.0f;
			*Theta = 0.0f;
			return FALSE;
		}
}


void GENESISCC geQuaternion_GetVec3d(
	const geQuaternion *Q,
	float *W,
	geVec3d *V)
	// get quaternion components into W and V
{
	VERIFY( geQuaternion_IsValid(Q) != FALSE );
	VERIFY( W != NULL );
	VERIFY( V != NULL );

	*W   = Q->W;
	V->X = Q->X;
	V->Y = Q->Y;
	V->Z = Q->Z;
}


void GENESISCC geQuaternion_FromMatrix(
	const geXForm3d		*M,
	      geQuaternion	*Q)
	// takes upper 3 by 3 portion of matrix (rotation sub matrix)
	// and generates a quaternion
{
	float trace,s;

	VERIFY( M != NULL );
	VERIFY( Q != NULL );
	geQuaternion_Assert( geXForm3d_IsOrthonormal(M)==TRUE );

	trace = M->AX + M->BY + M->CZ;
	if (trace > 0.0f)
		{
			s = (float)sqrt(trace + 1.0f);
			Q->W = s * 0.5f;
			s = 0.5f / s;

			Q->X = (M->CY - M->BZ) * s;
			Q->Y = (M->AZ - M->CX) * s;
			Q->Z = (M->BX - M->AY) * s;
		}
	else
		{
			int biggest;
			enum {A,E,I};
			if (M->AX > M->BY)
				{
					if (M->CZ > M->AX)
						biggest = I;
					else
						biggest = A;
				}
			else
				{
					if (M->CZ > M->AX)
						biggest = I;
					else
						biggest = E;
				}

			// in the unusual case the original trace fails to produce a good sqrt, try others...
			switch (biggest)
				{
				case A:
					s = (float)sqrt( M->AX - (M->BY + M->CZ) + 1.0);
					if (s > TRACE_QZERO_TOLERANCE)
						{
							Q->X = s * 0.5f;
							s = 0.5f / s;
							Q->W = (M->CY - M->BZ) * s;
							Q->Y = (M->AY + M->BX) * s;
							Q->Z = (M->AZ + M->CX) * s;
							break;
						}
							// I
							s = (float)sqrt( M->CZ - (M->AX + M->BY) + 1.0);
							if (s > TRACE_QZERO_TOLERANCE)
								{
									Q->Z = s * 0.5f;
									s = 0.5f / s;
									Q->W = (M->BX - M->AY) * s;
									Q->X = (M->CX + M->AZ) * s;
									Q->Y = (M->CY + M->BZ) * s;
									break;
								}
							// E
							s = (float)sqrt( M->BY - (M->CZ + M->AX) + 1.0);
							if (s > TRACE_QZERO_TOLERANCE)
								{
									Q->Y = s * 0.5f;
									s = 0.5f / s;
									Q->W = (M->AZ - M->CX) * s;
									Q->Z = (M->BZ + M->CY) * s;
									Q->X = (M->BX + M->AY) * s;
									break;
								}
							break;
				case E:
					s = (float)sqrt( M->BY - (M->CZ + M->AX) + 1.0);
					if (s > TRACE_QZERO_TOLERANCE)
						{
							Q->Y = s * 0.5f;
							s = 0.5f / s;
							Q->W = (M->AZ - M->CX) * s;
							Q->Z = (M->BZ + M->CY) * s;
							Q->X = (M->BX + M->AY) * s;
							break;
						}
							// I
							s = (float)sqrt( M->CZ - (M->AX + M->BY) + 1.0);
							if (s > TRACE_QZERO_TOLERANCE)
								{
									Q->Z = s * 0.5f;
									s = 0.5f / s;
									Q->W = (M->BX - M->AY) * s;
									Q->X = (M->CX + M->AZ) * s;
									Q->Y = (M->CY + M->BZ) * s;
									break;
								}
							// A
							s = (float)sqrt( M->AX - (M->BY + M->CZ) + 1.0);
							if (s > TRACE_QZERO_TOLERANCE)
								{
									Q->X = s * 0.5f;
									s = 0.5f / s;
									Q->W = (M->CY - M->BZ) * s;
									Q->Y = (M->AY + M->BX) * s;
									Q->Z = (M->AZ + M->CX) * s;
									break;
								}
					break;
				case I:
					s = (float)sqrt( M->CZ - (M->AX + M->BY) + 1.0);
					if (s > TRACE_QZERO_TOLERANCE)
						{
							Q->Z = s * 0.5f;
							s = 0.5f / s;
							Q->W = (M->BX - M->AY) * s;
							Q->X = (M->CX + M->AZ) * s;
							Q->Y = (M->CY + M->BZ) * s;
							break;
						}
							// A
							s = (float)sqrt( M->AX - (M->BY + M->CZ) + 1.0);
							if (s > TRACE_QZERO_TOLERANCE)
								{
									Q->X = s * 0.5f;
									s = 0.5f / s;
									Q->W = (M->CY - M->BZ) * s;
									Q->Y = (M->AY + M->BX) * s;
									Q->Z = (M->AZ + M->CX) * s;
									break;
								}
							// E
							s = (float)sqrt( M->BY - (M->CZ + M->AX) + 1.0);
							if (s > TRACE_QZERO_TOLERANCE)
								{
									Q->Y = s * 0.5f;
									s = 0.5f / s;
									Q->W = (M->AZ - M->CX) * s;
									Q->Z = (M->BZ + M->CY) * s;
									Q->X = (M->BX + M->AY) * s;
									break;
								}
					break;
				default:
					VERIFY(0);
				}
		}
	geQuaternion_Assert( geQuaternion_IsUnit(Q) == TRUE );
}

GENESISAPI void GENESISCC geQuaternion_ToMatrix(
	const geQuaternion	*Q,
		  geXForm3d		*M)
	// takes a unit quaternion and fills out an equivelant rotation
	// portion of a xform
{
	float X2,Y2,Z2;		//2*QX, 2*QY, 2*QZ
	float XX2,YY2,ZZ2;	//2*QX*QX, 2*QY*QY, 2*QZ*QZ
	float XY2,XZ2,XW2;	//2*QX*QY, 2*QX*QZ, 2*QX*QW
	float YZ2,YW2,ZW2;	// ...

	VERIFY( geQuaternion_IsValid(Q) != FALSE );
	VERIFY( M != NULL );
	geQuaternion_Assert( geQuaternion_IsUnit(Q) == TRUE );


	X2  = 2.0f * Q->X;
	XX2 = X2   * Q->X;
	XY2 = X2   * Q->Y;
	XZ2 = X2   * Q->Z;
	XW2 = X2   * Q->W;

	Y2  = 2.0f * Q->Y;
	YY2 = Y2   * Q->Y;
	YZ2 = Y2   * Q->Z;
	YW2 = Y2   * Q->W;

	Z2  = 2.0f * Q->Z;
	ZZ2 = Z2   * Q->Z;
	ZW2 = Z2   * Q->W;

	M->AX = 1.0f - YY2 - ZZ2;
	M->AY = XY2  - ZW2;
	M->AZ = XZ2  + YW2;

	M->BX = XY2  + ZW2;
	M->BY = 1.0f - XX2 - ZZ2;
	M->BZ = YZ2  - XW2;

	M->CX = XZ2  - YW2;
	M->CY = YZ2  + XW2;
	M->CZ = 1.0f - XX2 - YY2;

	M->Translation.X = M->Translation.Y = M->Translation.Z = 0.0f;

	geQuaternion_Assert( geXForm3d_IsOrthonormal(M)==TRUE );

}


#define EPSILON (0.00001)


void GENESISCC geQuaternion_Slerp(
	const geQuaternion		*Q0,
	const geQuaternion		*Q1,
	float					T,
	geQuaternion			*QT)
	// spherical interpolation between q0 and q1.   0<=t<=1
	// resulting quaternion is 'between' q0 and q1
	// with t==0 being all q0, and t==1 being all q1.
{
	float omega,cosom,sinom,Scale0,Scale1;
	geQuaternion QL;
	VERIFY( Q0 != NULL );
	VERIFY( Q1 != NULL );
	VERIFY( QT  != NULL );
	VERIFY( ( 0 <= T ) && ( T <= 1.0f ) );
	geQuaternion_Assert( geQuaternion_IsUnit(Q0) == TRUE );
	geQuaternion_Assert( geQuaternion_IsUnit(Q1) == TRUE );

	cosom =		(Q0->W * Q1->W) + (Q0->X * Q1->X)
			  + (Q0->Y * Q1->Y) + (Q0->Z * Q1->Z);

	if (cosom < 0)
		{
			cosom = -cosom;
			QL.X = -Q1->X;
			QL.Y = -Q1->Y;
			QL.Z = -Q1->Z;
			QL.W = -Q1->W;
		}
	else
		{
			QL = *Q1;
		}


	if ( (1.0f - cosom) > EPSILON )
		{
			omega  = (float) acos( cosom );
			sinom  = (float) sin( omega );
			Scale0 = (float) sin( (1.0f-T) * omega) / sinom;
			Scale1 = (float) sin( T*omega) / sinom;
		}
	else
		{
			// has numerical difficulties around cosom == 0
			// in this case degenerate to linear interpolation

			Scale0 = 1.0f - T;
			Scale1 = T;
		}


	QT-> X = Scale0 * Q0->X + Scale1 * QL.X;
	QT-> Y = Scale0 * Q0->Y + Scale1 * QL.Y;
	QT-> Z = Scale0 * Q0->Z + Scale1 * QL.Z;
	QT-> W = Scale0 * Q0->W + Scale1 * QL.W;
	geQuaternion_Assert( geQuaternion_IsUnit(QT) == TRUE );
}




void GENESISCC geQuaternion_SlerpNotShortest(
	const geQuaternion		*Q0,
	const geQuaternion		*Q1,
	float					T,
	geQuaternion			*QT)
	// spherical interpolation between q0 and q1.   0<=t<=1
	// resulting quaternion is 'between' q0 and q1
	// with t==0 being all q0, and t==1 being all q1.
{
	float omega,cosom,sinom,Scale0,Scale1;
	VERIFY( Q0 != NULL );
	VERIFY( Q1 != NULL );
	VERIFY( QT  != NULL );
	VERIFY( ( 0 <= T ) && ( T <= 1.0f ) );
	geQuaternion_Assert( geQuaternion_IsUnit(Q0) == TRUE );
	geQuaternion_Assert( geQuaternion_IsUnit(Q1) == TRUE );

	cosom =		(Q0->W * Q1->W) + (Q0->X * Q1->X)
			  + (Q0->Y * Q1->Y) + (Q0->Z * Q1->Z);
	if ( (1.0f + cosom) > EPSILON )
		{
			if ( (1.0f - cosom) > EPSILON )
				{
					omega  = (float) acos( cosom );
					sinom  = (float) sin( omega );
					// has numerical difficulties around cosom == nPI/2
					// in this case everything is up for grabs...
					//  ...degenerate to linear interpolation
					if (sinom < EPSILON)
						{
							Scale0 = 1.0f - T;
							Scale1 = T;
						}
					else
						{
							Scale0 = (float) sin( (1.0f-T) * omega) / sinom;
							Scale1 = (float) sin( T*omega) / sinom;
						}
				}
			else
				{
					// has numerical difficulties around cosom == 0
					// in this case degenerate to linear interpolation

					Scale0 = 1.0f - T;
					Scale1 = T;
				}
			QT-> X = Scale0 * Q0->X + Scale1 * Q1->X;
			QT-> Y = Scale0 * Q0->Y + Scale1 * Q1->Y;
			QT-> Z = Scale0 * Q0->Z + Scale1 * Q1->Z;
			QT-> W = Scale0 * Q0->W + Scale1 * Q1->W;
			//#pragma message (" ack:!!!!!!")
			//geQuaternionNormalize(QT);
			geQuaternion_Assert( geQuaternion_IsUnit(QT));
		}
	else
		{
			QT->X = -Q0->Y;
			QT->Y =  Q0->X;
			QT->Z = -Q0->W;
			QT->W =  Q0->Z;
			Scale0 = (float) sin( (1.0f - T) * (QUATERNION_PI*0.5) );
			Scale1 = (float) sin( T * (QUATERNION_PI*0.5) );
			QT-> X = Scale0 * Q0->X + Scale1 * QT->X;
			QT-> Y = Scale0 * Q0->Y + Scale1 * QT->Y;
			QT-> Z = Scale0 * Q0->Z + Scale1 * QT->Z;
			QT-> W = Scale0 * Q0->W + Scale1 * QT->W;
			geQuaternion_Assert( geQuaternion_IsUnit(QT));
		}
}

void GENESISCC geQuaternion_Multiply(
	const geQuaternion	*Q1,
	const geQuaternion	*Q2,
	geQuaternion		*Q)
	// multiplies q1 * q2, and places the result in q.
	// no failure. 	renormalization not automatic

{
	geQuaternion Q1L,Q2L;
	VERIFY( geQuaternion_IsValid(Q1) != FALSE );
	VERIFY( geQuaternion_IsValid(Q2) != FALSE );
	VERIFY( Q  != NULL );
	Q1L = *Q1;
	Q2L = *Q2;

	Q->W  =	(  (Q1L.W*Q2L.W) - (Q1L.X*Q2L.X)
			 - (Q1L.Y*Q2L.Y) - (Q1L.Z*Q2L.Z) );

	Q->X  =	(  (Q1L.W*Q2L.X) + (Q1L.X*Q2L.W)
			 + (Q1L.Y*Q2L.Z) - (Q1L.Z*Q2L.Y) );

	Q->Y  =	(  (Q1L.W*Q2L.Y) - (Q1L.X*Q2L.Z)
			 + (Q1L.Y*Q2L.W) + (Q1L.Z*Q2L.X) );

	Q->Z  = (  (Q1L.W*Q2L.Z) + (Q1L.X*Q2L.Y)
			 - (Q1L.Y*Q2L.X) + (Q1L.Z*Q2L.W) );
	geQuaternion_Assert( geQuaternion_IsValid(Q) != FALSE );

}


void GENESISCC geQuaternion_Rotate(
	const geQuaternion	*Q,
	const geVec3d         *V,
	geVec3d				*VRotated)
	// Rotates V by the quaternion Q, places the result in VRotated.
{
	VERIFY( geQuaternion_IsValid(Q) != FALSE );
	VERIFY( geVec3d_IsValid(V)  != FALSE );
	VERIFY( VRotated  != NULL );

	geQuaternion_Assert( geQuaternion_IsUnit(Q) == TRUE );

	{
		geQuaternion Qinv,QV,QRotated, QT;
		float zero;
		geQuaternion_SetVec3d(&QV ,0.0f,V);
		geQuaternion_Inverse (Q,&Qinv);
		geQuaternion_Multiply(Q,&QV,&QT);
		geQuaternion_Multiply(&QT,&Qinv,&QRotated);
		geQuaternion_GetVec3d(&QRotated,&zero,VRotated);
	}
}



BOOL GENESISCC geQuaternion_IsUnit(const geQuaternion *Q)
	// returns TRUE if Q is a unit geQuaternion.  FALSE otherwise.
{
	float magnitude;
	VERIFY( Q != NULL );

	magnitude  =   (Q->W * Q->W) + (Q->X * Q->X)
					  + (Q->Y * Q->Y) + (Q->Z * Q->Z);

	if (( magnitude < 1.0+UNIT_TOLERANCE ) && ( magnitude > 1.0-UNIT_TOLERANCE ))
		return TRUE;
	return FALSE;
}

float GENESISCC geQuaternion_Magnitude(const geQuaternion *Q)
	// returns Magnitude of Q.
{

	VERIFY( geQuaternion_IsValid(Q) != FALSE );
	return   (Q->W * Q->W) + (Q->X * Q->X)  + (Q->Y * Q->Y) + (Q->Z * Q->Z);
}


GENESISAPI float GENESISCC geQuaternion_Normalize(geQuaternion *Q)
	// normalizes Q to be a unit geQuaternion
{
	float magnitude,one_over_magnitude;
	VERIFY( geQuaternion_IsValid(Q) != FALSE );

	magnitude =   (float) sqrt ((Q->W * Q->W) + (Q->X * Q->X)
							  + (Q->Y * Q->Y) + (Q->Z * Q->Z));

	if (( magnitude < QZERO_TOLERANCE ) && ( magnitude > -QZERO_TOLERANCE ))
		{
			return 0.0f;
		}

	one_over_magnitude = 1.0f / magnitude;

	Q->W *= one_over_magnitude;
	Q->X *= one_over_magnitude;
	Q->Y *= one_over_magnitude;
	Q->Z *= one_over_magnitude;
	return magnitude;
}


GENESISAPI void GENESISCC geQuaternion_Copy(const geQuaternion *QSrc, geQuaternion *QDst)
	// copies quaternion QSrc into QDst
{
	VERIFY( geQuaternion_IsValid(QSrc) != FALSE );
	VERIFY( QDst != NULL );
	*QDst = *QSrc;
}

void GENESISCC geQuaternion_Inverse(const geQuaternion *Q, geQuaternion *QInv)
	// sets QInv to the inverse of Q.
{
	VERIFY( geQuaternion_IsValid(Q) != FALSE );
	VERIFY( QInv != NULL );

	QInv->W =  Q->W;
	QInv->X = -Q->X;
	QInv->Y = -Q->Y;
	QInv->Z = -Q->Z;
}


void GENESISCC geQuaternion_Add(
	const geQuaternion *Q1,
	const geQuaternion *Q2,
	geQuaternion *QSum)
	// QSum = Q1 + Q2  (result is not generally a unit quaternion!)
{
	VERIFY( geQuaternion_IsValid(Q1) != FALSE );
	VERIFY( geQuaternion_IsValid(Q2) != FALSE );
	VERIFY( QSum != NULL );
	QSum->W = Q1->W + Q2->W;
	QSum->X = Q1->X + Q2->X;
	QSum->Y = Q1->Y + Q2->Y;
	QSum->Z = Q1->Z + Q2->Z;
}

void GENESISCC geQuaternion_Subtract(
	const geQuaternion *Q1,
	const geQuaternion *Q2,
	geQuaternion *QSum)
	// QSum = Q1 - Q2  (result is not generally a unit quaternion!)
{
	VERIFY( geQuaternion_IsValid(Q1) != FALSE );
	VERIFY( geQuaternion_IsValid(Q2) != FALSE );
	VERIFY( QSum != NULL );
	QSum->W = Q1->W - Q2->W;
	QSum->X = Q1->X - Q2->X;
	QSum->Y = Q1->Y - Q2->Y;
	QSum->Z = Q1->Z - Q2->Z;
}


#define ZERO_EPSILON (0.0001f)
static int32 geQuaternion_XFormTable[]={1768710981,560296816};

void GENESISCC geQuaternion_Ln(
	const geQuaternion *Q,
	geQuaternion *LnQ)
	// ln(Q) for unit quaternion only!
{
	float Theta;
	geQuaternion QL;
	VERIFY( geQuaternion_IsValid(Q) != FALSE );
	VERIFY( LnQ != NULL );
	geQuaternion_Assert( geQuaternion_IsUnit(Q) == TRUE );

	if (Q->W < 0.0f)
		{
			QL.W = -Q->W;
			QL.X = -Q->X;
			QL.Y = -Q->Y;
			QL.Z = -Q->Z;
		}
	else
		{
			QL = *Q;
		}
	Theta    = (float)  acos( QL.W  );
	 //  0 < Theta < pi
	if (Theta< ZERO_EPSILON)
		{
			// lim(t->0) of t/sin(t) = 1, so:
			LnQ->W = 0.0f;
			LnQ->X = QL.X;
			LnQ->Y = QL.Y;
			LnQ->Z = QL.Z;
		}
	else
		{
			float Theta_Over_sin_Theta =  Theta / (float) sin ( Theta );
			LnQ->W = 0.0f;
			LnQ->X = Theta_Over_sin_Theta * QL.X;
			LnQ->Y = Theta_Over_sin_Theta * QL.Y;
			LnQ->Z = Theta_Over_sin_Theta * QL.Z;
		}
}

void GENESISCC geQuaternion_Exp(
	const geQuaternion *Q,
	geQuaternion *ExpQ)
	// exp(Q) for pure quaternion only!  (zero scalar part (W))
{
	float Theta;
	float sin_Theta_over_Theta;

	VERIFY( geQuaternion_IsValid(Q) != FALSE );
	VERIFY( ExpQ != NULL);
	VERIFY( Q->W == 0.0 );	//check a range?

	Theta = (float) sqrt(Q->X*Q->X  +  Q->Y*Q->Y  +  Q->Z*Q->Z);
	if (Theta > ZERO_EPSILON)
		{
			sin_Theta_over_Theta = (float) sin(Theta) / Theta;
		}
	else
		{
			sin_Theta_over_Theta = (float) 1.0f;
		}

	ExpQ->W   = (float) cos(Theta);
	ExpQ->X   = sin_Theta_over_Theta * Q->X;
	ExpQ->Y   = sin_Theta_over_Theta * Q->Y;
	ExpQ->Z   = sin_Theta_over_Theta * Q->Z;
}

void GENESISCC geQuaternion_Scale(
	const geQuaternion *Q,
	float Scale,
	geQuaternion *QScaled)
	// Q = Q * Scale  (result is not generally a unit quaternion!)
{
	VERIFY( geQuaternion_IsValid(Q) != FALSE );
	VERIFY( (Scale * Scale) >=0.0f );
	VERIFY( QScaled != NULL);

	QScaled->W = Q->W * Scale;
	QScaled->X = Q->X * Scale;
	QScaled->Y = Q->Y * Scale;
	QScaled->Z = Q->Z * Scale;
}

void GENESISCC geQuaternion_SetNoRotation(geQuaternion *Q)
	// sets Q to be a quaternion with no rotation (like an identity matrix)
{
	Q->W = 1.0f;
	Q->X = Q->Y = Q->Z = 0.0f;

	/* this is equivelant to:
		{
			geXForm3d M;
			geXForm3d_SetIdentity(&M);
			geQuaternionFromMatrix(&M,Q);
		}
	*/
}



BOOL GENESISCC geQuaternion_Compare( geQuaternion *Q1, geQuaternion *Q2, float Tolerance )
{
	VERIFY( geQuaternion_IsValid(Q1) != FALSE );
	VERIFY( geQuaternion_IsValid(Q2) != FALSE );
	VERIFY ( Tolerance >= 0.0 );

	if (	// they are the same but with opposite signs
			(		(fabs(Q1->X + Q2->X) <= Tolerance )
				&&  (fabs(Q1->Y + Q2->Y) <= Tolerance )
				&&  (fabs(Q1->Z + Q2->Z) <= Tolerance )
				&&  (fabs(Q1->W + Q2->W) <= Tolerance )
			)
		  ||  // they are the same with same signs
			(		(fabs(Q1->X - Q2->X) <= Tolerance )
				&&  (fabs(Q1->Y - Q2->Y) <= Tolerance )
				&&  (fabs(Q1->Z - Q2->Z) <= Tolerance )
				&&  (fabs(Q1->W - Q2->W) <= Tolerance )
			)
		)
		return TRUE;
	else
		return FALSE;



}
