///////////////////////////////////////////////////////////////
// ParticlesPlayer.cpp
// ��������� ��� ������������ ��������� �� �������
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "ParticlesPlayer.h"



CParticlesPlayer::CParticlesPlayer ()
{
}

CParticlesPlayer::~CParticlesPlayer ()
{
}


CParticlesObject* CParticlesPlayer::StartParticles(ref_str particles_name,
											 int bone_num, 
											 const Fvector& bone_pos,
											 const Fvector& dir,
											 u16 sender_id,
											 bool auto_remove)

{
	R_ASSERT(*particles_name);
	
	CParticlesObject* pParticlesObject = NULL;
	pParticlesObject = xr_new<CParticlesObject>(*particles_name, Sector(), false);

	SParticlesInfo* pParticlesInfo = xr_new<SParticlesInfo>();
	pParticlesInfo->particles_name = particles_name;
	pParticlesInfo->bone = bone_num;
	pParticlesInfo->bone_pos = bone_pos;
	pParticlesInfo->dir = dir;
	pParticlesInfo->sender_id = sender_id;
	pParticlesInfo->auto_remove = auto_remove;

	R_ASSERT3(!(pParticlesObject->IsLooped()&&auto_remove),"Can't attach looped particle system with auto-remove flag.", *particles_name);

	//�������� ����� ������ � map
	m_ParticlesInfoMap[pParticlesObject] = pParticlesInfo;
	
	//������ ������ ��������
	pParticlesObject->Play();
	UpdateParticlesPosition(pParticlesObject,pParticlesInfo);
		
	return pParticlesObject;
}


void CParticlesPlayer::StartParticlesOnAllBones(PARTICLES_PTR_VECTOR& particles_vector,
												ref_str particles_name,
												u16 sender_id,
												bool auto_remove)
{
	for(BONE_INFO_VECTOR_IT it = m_ParticlesBonesList.begin();
							it != m_ParticlesBonesList.end();
							it++)
	{
		CParticlesObject* pParticles = StartParticles(particles_name, (*it).index, (*it).offset, 
													  Fvector().set(0,1,0),sender_id, auto_remove);
		particles_vector.push_back(pParticles);
	}
}

void CParticlesPlayer::StopParticles(CParticlesObject* particles_object)
{
	PARTICLES_INFO_MAP_IT it = m_ParticlesInfoMap.find(particles_object);
	
	if(m_ParticlesInfoMap.end() == it) return;
	
	CParticlesObject* pParticlesObject = it->first;
	pParticlesObject->Stop();
}

void CParticlesPlayer::StopParticles(PARTICLES_PTR_VECTOR& particles_vector)
{
	for(PARTICLES_PTR_VECTOR_IT it = particles_vector.begin();
							    it != particles_vector.end();
							    it++)
	{
		StopParticles(*it);
	}
}


void CParticlesPlayer::StopParticles(u32 sender_id)
{
	for(PARTICLES_INFO_MAP_IT it = m_ParticlesInfoMap.begin();
							  it != m_ParticlesInfoMap.end();
							  it++)
	{
		CParticlesObject* pParticlesObject = it->first;
		SParticlesInfo* pParticlesInfo = it->second;

		if(pParticlesInfo->sender_id == sender_id)
		{
			pParticlesObject->Stop();
		}
	}
}
void CParticlesPlayer::StopParticles(ref_str particles_name)
{
	for(PARTICLES_INFO_MAP_IT it = m_ParticlesInfoMap.begin();
							  it != m_ParticlesInfoMap.end();
							  it++)
	{
		CParticlesObject* pParticlesObject = it->first;
		SParticlesInfo* pParticlesInfo = it->second;

		if(pParticlesInfo->particles_name == particles_name)
		{
			pParticlesObject->Stop();
		}
	}
}

void CParticlesPlayer::UpdateParticles()
{
	CParticlesObject* pPreviousObject = NULL;

	for(PARTICLES_INFO_MAP_IT it = m_ParticlesInfoMap.begin();
							  it != m_ParticlesInfoMap.end();
							  it++)
	{
		CParticlesObject* pParticlesObject = it->first;
		SParticlesInfo* pParticlesInfo = it->second;


		//�������� ������� ���������
		UpdateParticlesPosition(pParticlesObject,pParticlesInfo);


		//���������� �������, ������� ��� �������
		if(pParticlesInfo->auto_remove && !pParticlesObject->PSI_alive())
		{
			pParticlesObject->Stop();
		}

		if(!pParticlesObject->IsPlaying())
		{
			pParticlesObject->PSI_destroy();
			m_ParticlesInfoMap.erase(pParticlesObject);

			if(pPreviousObject == NULL)
				it = m_ParticlesInfoMap.begin();
			else
				it = m_ParticlesInfoMap.find(pPreviousObject);
		}

		//��������� ������, ���� ����� erase ����� ���� ����������� �����
		//�������������� it
		pPreviousObject = it->first;
	}
}

void CParticlesPlayer::UpdateParticlesPosition(CParticlesObject* pParticles,
											   const SParticlesInfo* pInfo)
{
	CObject				*object = dynamic_cast<CObject*>(this);
	VERIFY				(object);

	Fmatrix				  l_tMatrix;
	CBoneInstance	  	 &l_tBoneInstance = PKinematics(object->Visual())->LL_GetBoneInstance((u16)pInfo->bone);
	
	l_tMatrix.identity();
	l_tMatrix.translate_over(pInfo->bone_pos);
	l_tMatrix.k.normalize(pInfo->dir);
	Fvector::generate_orthonormal_basis(l_tMatrix.k, l_tMatrix.i, l_tMatrix.j);
	l_tMatrix.mulA(l_tBoneInstance.mTransform);
	l_tMatrix.mulA(object->XFORM());

	Fvector vel;
	PHGetLinearVell(vel);
	
	pParticles->UpdateParent(l_tMatrix, vel);
}