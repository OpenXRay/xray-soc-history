///////////////////////////////////////////////////////////////
// DummyArtifact.cpp
// DummyArtefact - �������� ��������
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DummyArtifact.h"
#include "PhysicsShell.h"


CDummyArtefact::CDummyArtefact(void) 
{
}

CDummyArtefact::~CDummyArtefact(void) 
{
}

void CDummyArtefact::Load(LPCSTR section) 
{
	inherited::Load(section);
}

void CDummyArtefact::UpdateCL() 
{
	inherited::UpdateCL();
}