/****************************************************************************************/
/*  STRBLOCK.C																			*/
/*                                                                                      */
/*  Author: Mike Sandige	                                                            */
/*  Description: String block implementation.											*/
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
// strblock.c
//   a list of strings implemented as a single block of memory for fast
//   loading.  The 'Data' Field is interpreted as an array of integer
//   offsets relative to the beginning of the data field.  After the int list
//   is the packed string data.  Since no additional allocations are needed
//   this object can be file loaded as one block.

#include "strblock.h"

#define STRBLOCK_MAX_STRINGLEN 255

typedef struct geStrBlock
{
	int Count;
	geStrBlock *SanityCheck;
	union
		{
			int IntArray[1];		// char offset into CharArray for string[n]
			char CharArray[1];
		} Data;

} geStrBlock;


int GENESISCC geStrBlock_GetChecksum(const geStrBlock *SB)
{
	int Count;
	int Len;
	int i,j;
	const char *Str;
	int Checksum=0;
	VERIFY( SB != NULL );

	Count = geStrBlock_GetCount(SB);
	for (i=0; i<Count; i++)
		{
			Str = geStrBlock_GetString(SB,i);
			VERIFY(Str!=NULL);
			Len = strlen(Str);
			for (j=0; j<Len; j++)
				 {
					Checksum += (int)Str[j];
				}
			Checksum = Checksum*3;
		}
	return Checksum;
}

geStrBlock *GENESISCC geStrBlock_Create(void)
{
	geStrBlock *SB;

	SB = (geStrBlock*)malloc(sizeof(geStrBlock));

	SB->Count=0;
	SB->SanityCheck = SB;
	return SB;
}


void GENESISCC geStrBlock_Destroy(geStrBlock **SB)
{
	VERIFY( (*SB)->SanityCheck == (*SB) );
	VERIFY(  SB != NULL );
	VERIFY( *SB != NULL );
	_FREE( *SB );
	*SB = NULL;
}


static int GENESISCC geStrBlock_BlockSize(const geStrBlock *B)
{
	int Offset;
	const char *LastStr;
	VERIFY( B != NULL );
	VERIFY( B->SanityCheck == B );

	if ( B->Count == 0 )
		return 0;
	Offset = B->Data.IntArray[B->Count-1];
	LastStr = &(B->Data.CharArray[Offset]);

	return strlen(LastStr) + 1 + Offset;
}


void GENESISCC geStrBlock_Delete(geStrBlock **ppSB,int Nth)
{
	int BlockSize;
	int StringLen;
	int CloseSize;
	const char *String;
	VERIFY(  ppSB  != NULL );
	VERIFY( *ppSB  != NULL );
	VERIFY( Nth >=0 );
	VERIFY( Nth < (*ppSB)->Count );
	VERIFY( (*ppSB)->SanityCheck == (*ppSB) );

	String = geStrBlock_GetString(*ppSB,Nth);
	VERIFY( String != NULL );
	StringLen = strlen(String) + 1;

	BlockSize = geStrBlock_BlockSize(*ppSB);

	{
		geStrBlock *B = *ppSB;
		char *ToBeReplaced;
		char *Replacement=NULL;
		int i;
		ToBeReplaced = &((*ppSB)->Data.CharArray[(*ppSB)->Data.IntArray[Nth]]);
		if (Nth< (*ppSB)->Count-1)
			Replacement  = &((*ppSB)->Data.CharArray[(*ppSB)->Data.IntArray[Nth+1]]);
		for (i=Nth+1,CloseSize = 0; i<(*ppSB)->Count ; i++)
			{
				CloseSize += strlen(&((*ppSB)->Data.CharArray[(*ppSB)->Data.IntArray[i]])) +1;
				B->Data.IntArray[i] -= StringLen;
			}
		for (i=0; i<(*ppSB)->Count ; i++)
			{
				B->Data.IntArray[i] -= sizeof(int);
			}
		// crunch out Nth string
		if (Nth< (*ppSB)->Count-1)
			memmove(ToBeReplaced,Replacement,CloseSize);
		// crunch out Nth index
		memmove(&(B->Data.IntArray[Nth]),
				&(B->Data.IntArray[Nth+1]),
				BlockSize - ( sizeof(int) *  (Nth+1) ) );

	}

	{
		geStrBlock * NewgeStrBlock;

		NewgeStrBlock = (geStrBlock *)realloc( *ppSB,
			BlockSize				// size of data block
			+ sizeof(geStrBlock)		// size of strblock structure
			- StringLen				// size of dying string
			- sizeof(int) );		// size of new index to string
		if ( NewgeStrBlock != NULL )
			{
				*ppSB = NewgeStrBlock;
				(*ppSB)->SanityCheck = NewgeStrBlock;
			}
	}

	(*ppSB)->Count--;
}


#if 0
	// as of yet un needed.  and this is untested
BOOL GENESISCC geStrBlock_SetString(geStrBlock **ppSB, int Index, const char *String)
{
	VERIFY(  ppSB  != NULL );
	VERIFY( *ppSB  != NULL );
	VERIFY( Index >=0 );
	VERIFY( Index < (*ppSB)->Count );
	VERIFY( String != NULL );

	if (geStrBlock_Insert(ppSB,Index,String) == FALSE)
		{
			geErrorLog_Add(ERR_STRBLOCK_ENOMEM, NULL);
			return FALSE;
		}

	geStrBlock_Delete(ppSB,Index);
	return TRUE;
}
#endif

#if 0
	// as of yet un needed.  and this is untested
BOOL GENESISCC geStrBlock_Insert(geStrBlock **ppSB,int InsertAfterIndex,const char *String)
{
	int BlockSize;
	int StringLen;
	int MoveSize;

	VERIFY(  ppSB  != NULL );
	VERIFY( *ppSB  != NULL );
	VERIFY( InsertAfterIndex >=-1);
	VERIFY( InsertAfterIndex < (*ppSB)->Count );
	VERIFY( (*ppSB)->SanityCheck == (*ppSB) );
	VERIFY( String != NULL );

	if (InsertAfterIndex == (*ppSB)->Count - 1)
		{
			if (geStrBlock_Append(ppSB,String)==FALSE)
				{
					geErrorLog_Add(-1, NULL);
					return FALSE;
				}
			return TRUE;
		}

	StringLen = strlen(String) + 1;

	BlockSize = geStrBlock_BlockSize(*ppSB);

	{
		geStrBlock * NewgeStrBlock;

		NewgeStrBlock = realloc( *ppSB,
			BlockSize				// size of data block
			+ sizeof(geStrBlock)	// size of strblock structure
			+ StringLen				// size of new string
			+ sizeof(int) );		// size of new index to string
		if ( NewgeStrBlock != NULL )
			{
				*ppSB = NewgeStrBlock;
				(*ppSB)->SanityCheck = NewgeStrBlock;
			}
	}



	{
		geStrBlock *B = *ppSB;
		char *Chars = B->Data.CharArray;
		int  *Table = B->Data.IntArray;
		char *MoveFrom;
		char *MoveTo=NULL;
		int i;

		MoveFrom  = &(Chars[Table[InsertAfterIndex+1]]);

		MoveTo = MoveFrom + StringLen;

		for (i=InsertAfterIndex+1,MoveSize = 0; i<B->Count ; i++)
			{
				MoveSize += strlen(&(Chars[Table[i]])) +1;
				Table[i] += StringLen;
			}
		for (i=0; i<(*ppSB)->Count ; i++)
			{
				Table[i] += sizeof(int);
			}
		// make room for string
		memmove(MoveFrom,MoveTo,MoveSize);
		// make room for new index
		memmove(&(Table[InsertAfterIndex+1]),
				&(Table[InsertAfterIndex+2]),
				BlockSize - ( sizeof(int) *  (InsertAfterIndex+2) ) );
		Table[InsertAfterIndex+1] = Table[InsertAfterIndex]
						+ strlen(&(Chars[Table[InsertAfterIndex]])) +1;

	}


	(*ppSB)->Count++;
	return TRUE;


}
#endif

BOOL GENESISCC geStrBlock_FindString(const geStrBlock* pSB, const char* String, int* pIndex)
{
	int i;
	int Count;
	const char *Str;

	VERIFY(pSB != NULL);
	VERIFY(String != NULL);
	VERIFY(pIndex != NULL);
	VERIFY( pSB->SanityCheck == pSB );

	Count = geStrBlock_GetCount(pSB);
	for (i=0; i<Count; i++)
	{
		Str = geStrBlock_GetString(pSB,i);
		if(strcmp(String, Str) == 0)
		{
			*pIndex = i;
			return TRUE;
		}
	}
	return FALSE;
}


BOOL GENESISCC geStrBlock_Append(geStrBlock **ppSB,const char *String)
{
	int BlockSize;
	VERIFY(  ppSB  != NULL );
	VERIFY( *ppSB  != NULL );
	VERIFY( String != NULL );
	VERIFY( (*ppSB)->SanityCheck == (*ppSB) );

	if (strlen(String)>=STRBLOCK_MAX_STRINGLEN)
		{
//			geErrorLog_Add(ERR_STRBLOCK_STRLEN, NULL);
			return FALSE;
		}

	BlockSize = geStrBlock_BlockSize(*ppSB);

	{
		geStrBlock * NewgeStrBlock;

		NewgeStrBlock = (geStrBlock *)realloc( *ppSB,
			BlockSize				// size of data block
			+ sizeof(geStrBlock)		// size of strblock structure
			+ strlen(String) + 1		// size of new string
			+ sizeof(int) );		// size of new index to string
		if ( NewgeStrBlock == NULL )
			{
//				geErrorLog_Add(ERR_STRBLOCK_ENOMEM, NULL);
				return FALSE;
			}
		*ppSB = NewgeStrBlock;
		(*ppSB)->SanityCheck = NewgeStrBlock;
	}

	{
		geStrBlock *B = *ppSB;
		int i;
		for (i=0; i<B->Count; i++)
			{
				B->Data.IntArray[i] += sizeof(int);
			}
		if (B->Count > 0)
			{
				memmove(&(B->Data.IntArray[B->Count+1]),
						&(B->Data.IntArray[B->Count]),
						BlockSize - sizeof(int) * B->Count);
			}
		B->Data.IntArray[B->Count] = BlockSize + sizeof(int);
		strcpy(&(B->Data.CharArray[B->Data.IntArray[B->Count]]),String);
	}
	(*ppSB)->Count++;
	return TRUE;
}


const char *GENESISCC geStrBlock_GetString(const geStrBlock *SB, int Index)
{
	VERIFY( SB != NULL );
	VERIFY( Index >= 0 );
	VERIFY( Index < SB->Count );
	VERIFY( SB->SanityCheck == SB );
	return &(SB->Data.CharArray[SB->Data.IntArray[Index]]);
}

int GENESISCC geStrBlock_GetCount(const geStrBlock *SB)
{
	VERIFY( SB != NULL);
	VERIFY( SB->SanityCheck == SB );
	return SB->Count;
}


#define CHECK_FOR_WRITE(uu) if(uu <= 0) { geErrorLog_Add( ERR_STRBLOCK_FILE_WRITE, NULL ); return FALSE; }

#define STRBLOCK_ASCII_FILE_TYPE 0x4B4C4253	// 'SBLK'
#define STRBLOCK_BIN_FILE_TYPE 0x424B4253	// 'SBKB'
#define STRBLOCK_FILE_VERSION 0x00F0		// Restrict version to 16 bits

#define STRBLOCK_STRINGARRAY_ID		"Strings"
#define STRBLOCK_NUM_ASCII_IDS       1 	// Keep this up to date

typedef struct
{
	int Count;
	uint32 Size;
} geStrBlock_BinaryFileHeader;

geStrBlock* GENESISCC geStrBlock_CreateFromStream(CStream* pStream)
{
	uint32 u;

	VERIFY( pStream );

	pStream->Read(&u, sizeof(u));
//	if(geVFile_Read(pFile, &u, sizeof(u)) == FALSE)
//	{
//		geErrorLog_Add( ERR_STRBLOCK_FILE_READ , NULL);
//		return NULL;
//	}

	if(u == STRBLOCK_ASCII_FILE_TYPE)
	{
	}
	else
	{
		if (u!=STRBLOCK_BIN_FILE_TYPE)	return NULL;

		geStrBlock *SB;
		geStrBlock_BinaryFileHeader Header;

		pStream->Read(&Header,sizeof(geStrBlock_BinaryFileHeader));
//		if (geVFile_Read(pFile, &Header,sizeof(geStrBlock_BinaryFileHeader)) == FALSE)
//			{
	//			geErrorLog_Add( ERR_STRBLOCK_FILE_READ , NULL);
//				return NULL;
//			}

		SB = (geStrBlock *)malloc( sizeof(geStrBlock) + Header.Size );
		if( SB == NULL )
			{
	//			geErrorLog_Add(ERR_STRBLOCK_ENOMEM, NULL);
				return NULL;
			}
		SB->SanityCheck = SB;
		SB->Count = Header.Count;

		pStream->Read(&(SB->Data),Header.Size);
//		if (geVFile_Read(pFile, &(SB->Data),Header.Size) == FALSE)
//			{
	//			geErrorLog_Add( ERR_STRBLOCK_FILE_READ , NULL);
//				return NULL;
//			}
		return SB;
	}

//	geErrorLog_Add( ERR_STRBLOCK_FILE_PARSE , NULL);
	return NULL;
}

