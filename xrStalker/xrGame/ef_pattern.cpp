////////////////////////////////////////////////////////////////////////////
//	Module 		: ef_pattern.cpp
//	Created 	: 25.03.2002
//  Modified 	: 11.10.2002
//	Author		: Dmitriy Iassenev
//	Description : Pattern based evaluation functions trained by supervised learning
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ef_pattern.h"
#include "ef_primary.h"
#include "ai_space.h"
#include "ai_alife_templates.h"
#include "ai_debug.h"
#include "ef_storage.h"

CPatternFunction::CPatternFunction() : CBaseFunction()
{
	m_dwPatternCount		= 0;
	m_dwVariableCount		= 0;
	m_dwParameterCount		= 0;
	m_dwaVariableTypes		= 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes		= 0;
	m_tpPatterns			= 0;
	m_faParameters			= 0;
	m_dwaVariableValues		= 0;
}

CPatternFunction::CPatternFunction(LPCSTR caFileName, CEF_Storage *tpAI_DDD) : CBaseFunction()
{
	m_dwPatternCount		= 0;
	m_dwVariableCount		= 0;
	m_dwParameterCount		= 0;
	m_dwaVariableTypes		= 0;
	m_dwaAtomicFeatureRange = 0;
	m_dwaPatternIndexes		= 0;
	m_tpPatterns			= 0;
	m_faParameters			= 0;
	m_dwaVariableValues		= 0;
	vfLoadEF				(caFileName, tpAI_DDD);
}

CPatternFunction::~CPatternFunction()
{
	xr_free			(m_dwaVariableTypes);
	xr_free			(m_dwaAtomicFeatureRange);
	xr_free			(m_dwaPatternIndexes);
	for (u32 i=0; i<m_dwPatternCount; ++i)
		xr_free		(m_tpPatterns[i].dwaVariableIndexes);
	xr_free			(m_tpPatterns);
	xr_free			(m_faParameters);
	xr_free			(m_dwaVariableValues);
}

void CPatternFunction::vfLoadEF(LPCSTR caFileName, CEF_Storage *tpAI_DDD)
{
	string256		caPath;
	if (!FS.exist(caPath,"$game_ai$",caFileName)) {
		Msg			("! Evaluation function : File not found \"%s\"",caPath);
		R_ASSERT	(false);
		return;
	}
	
	IReader			*F = FS.r_open(caPath);
	F->r			(&m_tEFHeader,sizeof(SEFHeader));

	if (EFC_VERSION != m_tEFHeader.dwBuilderVersion) {
		FS.r_close	(F);
		Msg			("! Evaluation function (%s) : Not supported version of the Evaluation Function Contructor",caPath);
		R_ASSERT	(false);
		return;
	}

	F->r			(&m_dwVariableCount,sizeof(m_dwVariableCount));
	m_dwaAtomicFeatureRange = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	ZeroMemory		(m_dwaAtomicFeatureRange,m_dwVariableCount*sizeof(u32));
	u32				*m_dwaAtomicIndexes = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	ZeroMemory		(m_dwaAtomicIndexes,m_dwVariableCount*sizeof(u32));

	for (u32 i=0; i<m_dwVariableCount; ++i) {
		F->r(m_dwaAtomicFeatureRange + i,sizeof(u32));
		if (i)
			m_dwaAtomicIndexes[i] = m_dwaAtomicIndexes[i-1] + m_dwaAtomicFeatureRange[i-1];
	}

	m_dwaVariableTypes = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	F->r			(m_dwaVariableTypes,m_dwVariableCount*sizeof(u32));

	F->r			(&m_dwFunctionType,sizeof(u32));

	F->r			(&m_fMinResultValue,sizeof(float));
	F->r			(&m_fMaxResultValue,sizeof(float));

	F->r			(&m_dwPatternCount,sizeof(m_dwPatternCount));
	m_tpPatterns	= (SPattern *)xr_malloc(m_dwPatternCount*sizeof(SPattern));
	m_dwaPatternIndexes = (u32 *)xr_malloc(m_dwPatternCount*sizeof(u32));
	ZeroMemory		(m_dwaPatternIndexes,m_dwPatternCount*sizeof(u32));
	m_dwParameterCount = 0;
	for ( i=0; i<m_dwPatternCount; ++i) {
		if (i)
			m_dwaPatternIndexes[i] = m_dwParameterCount;
		F->r		(&(m_tpPatterns[i].dwCardinality),sizeof(m_tpPatterns[i].dwCardinality));
		m_tpPatterns[i].dwaVariableIndexes = (u32 *)xr_malloc(m_tpPatterns[i].dwCardinality*sizeof(u32));
		F->r		(m_tpPatterns[i].dwaVariableIndexes,m_tpPatterns[i].dwCardinality*sizeof(u32));
		u32			m_dwComplexity = 1;
		for (int j=0; j<(int)m_tpPatterns[i].dwCardinality; ++j)
			m_dwComplexity *= m_dwaAtomicFeatureRange[m_tpPatterns[i].dwaVariableIndexes[j]];
		m_dwParameterCount += m_dwComplexity;
	}
	
	m_faParameters	= (float *)xr_malloc(m_dwParameterCount*sizeof(float));
	F->r			(m_faParameters,m_dwParameterCount*sizeof(float));
	FS.r_close		(F);

	m_dwaVariableValues = (u32 *)xr_malloc(m_dwVariableCount*sizeof(u32));
	
	xr_free			(m_dwaAtomicIndexes);
    
	tpAI_DDD->m_fpaBaseFunctions[m_dwFunctionType] = this;
	
	_splitpath		(caPath,0,0,m_caName,0);

	// Msg			("* Evaluation function \"%s\" is successfully loaded",m_caName);
}

float CPatternFunction::ffEvaluate()
{
	float fResult = 0.0;
	for (u32 i=0; i<m_dwPatternCount; ++i)
		fResult += m_faParameters[dwfGetPatternIndex(m_dwaVariableValues,i)];
	return(fResult);
}

float CPatternFunction::ffGetValue()
{
	if (bfCheckForCachedResult())
		return(m_fLastValue);

	for (u32 i=0; i<m_dwVariableCount; ++i)
		m_dwaVariableValues[i] = ai().ef_storage().m_fpaBaseFunctions[m_dwaVariableTypes[i]]->dwfGetDiscreteValue(m_dwaAtomicFeatureRange[i]);

	
#ifdef DEBUG	
	if (psAI_Flags.test(aiFuncs)) {
		m_fLastValue = ffEvaluate();
		char caString[256];
		int j = sprintf(caString,"%32s (",m_caName);
		for ( i=0; i<m_dwVariableCount; ++i)
			j += sprintf(caString + j," %3d",m_dwaVariableValues[i] + 1);
		sprintf(caString + j,") = %7.2f",m_fLastValue);
		Msg("- %s",caString);
		return(m_fLastValue);
	}
#endif
	
	return(m_fLastValue = ffEvaluate());
}