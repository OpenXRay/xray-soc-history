//////////////////////////////////////////////////////////////////////
// ShootingObject.h: ��������� ��� ��������� ���������� �������� 
//					 (������ � ���������� �������) 	
//					 ������������ ����� �����, ������ ��������
//////////////////////////////////////////////////////////////////////

#pragma once

#include "GameObject.h"
#include "ShootingHitEffector.h"

class CCartridge;
#define WEAPON_MATERIAL_NAME "objects\\bullet"



class CShootingObject
{
private:
	typedef CGameObject inherited;
protected: //���� ������ ���� ������� �� ������
	CShootingObject(void);
	virtual ~CShootingObject(void);

	void	reinit	();
	void	reload	(LPCSTR section) {};
	void	Load	(LPCSTR section);

	//������� �������� ���� � �������� ��� �������� 
	//������������ ��� ������������� ���� ��� RayPick
	float		m_fCurrentFireDist;
	float		m_fCurrentHitPower;
	float		m_fCurrentHitImpulse;
	float		m_fCurrentWallmarkSize;
	CCartridge*	m_pCurrentCartridge;
	Fvector		m_vCurrentShootDir;
	Fvector		m_vCurrentShootPos;
	Fvector		m_vEndPoint;
	//ID ��������� ������� ���������� ��������
	u16			m_iCurrentParentID;
	//��� ���������� ����
	ALife::EHitType m_eCurrentHitType;


//////////////////////////////////////////////////////////////////////////
// Fire Params
//////////////////////////////////////////////////////////////////////////
protected:
	virtual void			LoadFireParams		(LPCSTR section, LPCSTR prefix);

protected:
	float					fTimeToFire;
	int						iHitPower;
	float					fHitImpulse;

	//�������� ������ ���� �� ������
	float					m_fStartBulletSpeed;
	//������������ ���������� ��������
	float					fireDistance;

	//����������� �� ����� ��������
	float					fireDispersionBase;

	//��������
	float					tracerHeadSpeed;
	float					tracerTrailCoeff;
	float					tracerStartLength;
	float					tracerWidth;
	u32						tracerFrame;

protected:
	//��� ���������, ���� ��� ����� ����������� ������� ������������� 
	//������
	float					m_fMinRadius;
	float					m_fMaxRadius;


//////////////////////////////////////////////////////////////////////////
// Lights
//////////////////////////////////////////////////////////////////////////
protected:
	Fcolor					light_base_color;
	float					light_base_range;
	Fcolor					light_build_color;
	float					light_build_range;
	IRender_Light*			light_render;
	float					light_var_color;
	float					light_var_range;
	float					light_lifetime;
	u32						light_frame;
	float					light_time;
	//��������� ��������� �� ����� ��������
	bool					m_bShotLight;
protected:
	void					Light_Start			();
	void					Light_Render		(Fvector& P);

	virtual	void			LoadLights			(LPCSTR section, LPCSTR prefix);
	
	
//////////////////////////////////////////////////////////////////////////
// ����������� �������
//////////////////////////////////////////////////////////////////////////
protected:
	//������� ������������� �������
	virtual const Fmatrix&	XFORM()	 const = 0;
	virtual IRender_Sector*	Sector() = 0;
	virtual const Fvector&	CurrentFirePoint()	= 0;
	virtual const Fvector&	CurrentFirePoint2() = 0;
		

	////////////////////////////////////////////////
	//����� ������� ��� ������ � ���������� ������
	virtual void			StartParticles		(CParticlesObject*& pParticles, LPCSTR particles_name, const Fvector& pos, const Fvector& vel = zero_vel, bool auto_remove_flag = false);
	virtual void			StopParticles		(CParticlesObject*& pParticles);
	virtual void			UpdateParticles		(CParticlesObject*& pParticles, const Fvector& pos, const  Fvector& vel = zero_vel);

	virtual	void			LoadShellParticles	(LPCSTR section, LPCSTR prefix);
	virtual	void			LoadFlameParticles	(LPCSTR section, LPCSTR prefix);
	
	////////////////////////////////////////////////
	//������������� ������� ��� ���������
	//�������� ����
	virtual void			StartFlameParticles	();
	virtual void			StopFlameParticles	();
	virtual void			UpdateFlameParticles();
	//��� ������� ������
	virtual void			StartFlameParticles2();
	virtual void			StopFlameParticles2	();
	virtual void			UpdateFlameParticles2();

	//�������� ����
	virtual void			StartSmokeParticles	(const Fvector& play_pos,
												 const Fvector& parent_vel);
	//�������� �����
	virtual void			OnShellDrop			(const Fvector& play_pos,
												 const Fvector& parent_vel);
protected:
	//��� ��������� ��� �����
	ref_str				m_sShellParticles;
	Fvector				vShellPoint;

protected:
	//��� ��������� ��� ����
	ref_str				m_sFlameParticlesCurrent;
	//��� �������� 1� � 2� ����� ��������
	ref_str				m_sFlameParticles;
	ref_str				m_sFlameParticles2;

	//������ ��������� ����
	CParticlesObject*	m_pFlameParticles;

	//������ ��������� ��� �������� �� 2-�� ������
	CParticlesObject*	m_pFlameParticles2;

	//��� ��������� ��� ����
	ref_str				m_sSmokeParticlesCurrent;
	ref_str				m_sSmokeParticles;

};