#include "StdAfx.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "tri-colliderknoopc/dTriList.h"
#include "PHFracture.h"
#include "PHContactBodyEffector.h"
#include "MathUtils.h"
#include "PhysicsShellHolder.h"
#include "game_object_space.h"
#include "../skeletoncustom.h"
#include <../ode/src/util.h>

#ifdef DEBUG
#include "../StatGraph.h"
#include "PHDebug.h"
#endif

///////////////////////////////////////////////////////////////
#pragma warning(disable:4995)
#pragma warning(disable:4267)

#include "../ode/src/collision_kernel.h"

//for debug only
//#include <../ode\src\joint.h>
//struct dxSphere : public dxGeom {
//	dReal radius;		// sphere radius
//	dxSphere (dSpaceID space, dReal _radius);
//	void computeAABB();
//};
//#include <../ode\src\objects.h>
#pragma warning(default:4267)
#pragma warning(default:4995)
///////////////////////////////////////////////////////////////////

#include "ExtendedGeom.h"

#include "PHShell.h"
#include "PHElement.h"
#include "PHElementInline.h"
extern CPHWorld*				ph_world;




/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////Implementation for CPhysicsElement
CPHElement::CPHElement()																															//aux
{
	m_w_limit = default_w_limit;
	m_l_limit = default_l_limit;
	m_l_scale=default_l_scale;
	m_w_scale=default_w_scale;

	//push_untill=0;

	//temp_for_push_out=NULL;

	m_body=NULL;
	//bActive=false;
	//bActivating=false;
	m_flags.set(flActive,FALSE);
	m_flags.set(flActivating,FALSE);
	m_parent_element=NULL;
	m_shell=NULL;


	k_w=default_k_w;
	k_l=default_k_l;//1.8f;
	m_fratures_holder=NULL;
	//b_enabled_onstep=false;
	//m_flags.set(flEnabledOnStep,FALSE);
	m_flags.assign(0);
	mXFORM.identity();
	m_mass.setZero();
	m_mass_center.set(0,0,0);
	m_volume=0.f;
}

void CPHElement::add_Box		(const Fobb&		V)
{
	CPHGeometryOwner::add_Box(V);
}


void CPHElement::add_Sphere	(const Fsphere&	V)
{
	CPHGeometryOwner::add_Sphere(V);
}

void CPHElement::add_Cylinder	(const Fcylinder& V)
{
	CPHGeometryOwner::add_Cylinder(V);
}

void CPHElement::			build	(){

	m_body=dBodyCreate(0);//phWorld
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
	dBodyDisable(m_body);
	//dBodySetFiniteRotationMode(m_body,1);
	//dBodySetFiniteRotationAxis(m_body,0,0,0);
	VERIFY2(dMass_valide(&m_mass),"Element has bad mass");
	if(m_geoms.empty())
	{
		Fix();
	}
	else
	{
		VERIFY2(m_mass.mass>0.f,"Element has bad mass");
		dBodySetMass(m_body,&m_mass);
	}
#ifdef DEBUG
	if(!valid_pos(m_mass_center,phBoundaries))
	{
		Msg("m_mass_center: %f,%f,%f, seems to be invalid", m_mass_center.x,m_mass_center.y,m_mass_center.z);
		Msg("Level box: %f,%f,%f-%f,%f,%f,",phBoundaries.x1,phBoundaries.y1,phBoundaries.z1,phBoundaries.x2,phBoundaries.y2,phBoundaries.z2);
		Msg("Object: %s",PhysicsRefObject()->Name());
		Msg("Visual: %s",*(PhysicsRefObject()->cNameVisual()));
		VERIFY(0);
	}
#endif
	dBodySetPosition(m_body,m_mass_center.x,m_mass_center.y,m_mass_center.z);

	CPHDisablingTranslational::Reinit();
	///////////////////////////////////////////////////////////////////////////////////////
	CPHGeometryOwner::build();
	set_body(m_body);
}

void CPHElement::RunSimulation()
{
	//if(push_untill)
		//push_untill+=Device.dwTimeGlobal;

	if(m_group)
		dSpaceAdd(m_shell->dSpace(),(dGeomID)m_group);
	else
		if(!m_geoms.empty())(*m_geoms.begin())->add_to_space(m_shell->dSpace());
	if(!m_body->world) 
	{
		//dWorldAddBody(phWorld, m_body);
		m_shell->Island().AddBody(m_body);
	}
	dBodyEnable(m_body);
}

void CPHElement::destroy	()
{
	//dJointGroupDestroy(m_saved_contacts);
	CPHGeometryOwner::destroy();
	if(m_body)//&&m_body->world
	{
		if(m_body->world)m_shell->Island().RemoveBody(m_body);
		dBodyDestroy(m_body);
		m_body=NULL;
	}

	if(m_group){
		dGeomDestroy(m_group);
		m_group=NULL;
	}
}

void CPHElement::calculate_it_data(const Fvector& mc,float mas)
{
	float density=mas/m_volume;
	calculate_it_data_use_density(mc,density);
}

static float static_dencity;
void CPHElement::calc_it_fract_data_use_density(const Fvector& mc,float density)
{
	m_mass_center.set(mc);
	dMassSetZero(&m_mass);
	static_dencity=density;
	recursive_mass_summ(0,m_fratures_holder->m_fractures.begin());
}

dMass CPHElement::recursive_mass_summ(u16 start_geom,FRACTURE_I cur_fracture)
{
	dMass end_mass;
	dMassSetZero(&end_mass);
	GEOM_I i_geom=m_geoms.begin()+start_geom,	e=m_geoms.begin()+cur_fracture->m_start_geom_num;
	for(;i_geom!=e;++i_geom)(*i_geom)->add_self_mass(end_mass,m_mass_center,static_dencity);
	dMassAdd(&m_mass,&end_mass);
	start_geom=cur_fracture->m_start_geom_num;
	++cur_fracture;
	if(m_fratures_holder->m_fractures.end() != cur_fracture)
		cur_fracture->SetMassParts(m_mass,recursive_mass_summ(start_geom,cur_fracture));
	return end_mass;
}
void		CPHElement::	setDensity		(float M)
{
	calculate_it_data_use_density(get_mc_data(),M);
}

void		CPHElement::	setMass		(float M)
{

	calculate_it_data(get_mc_data(),M);
}

void		CPHElement::	setDensityMC		(float M,const Fvector& mass_center)
{
	m_mass_center.set(mass_center);
	calc_volume_data();
	calculate_it_data_use_density(mass_center,M);
}

void		CPHElement::	setMassMC		(float M,const Fvector& mass_center)
{
	m_mass_center.set(mass_center);
	calc_volume_data();
	calculate_it_data(mass_center,M);
}



void		CPHElement::Start()
{
	build();
	RunSimulation();
}

void		CPHElement::Deactivate()
{
	VERIFY(isActive());

	destroy();
	m_flags.set(flActive,FALSE);
	m_flags.set(flActivating,FALSE);
	//bActive=false;
	//bActivating=false;
	CKinematics* K=m_shell->PKinematics();
	if(K)
	{
		K->LL_GetBoneInstance(m_SelfID).reset_callback();
	}
}

void CPHElement::SetTransform(const Fmatrix &m0){
	VERIFY2(_valid(m0),"invalid_form_in_set_transform");
	Fvector mc;
	CPHGeometryOwner::get_mc_vs_transform(mc,m0);
#ifdef DEBUG
	if(!valid_pos(mc,phBoundaries))
	{
		Msg("mc: %f,%f,%f, seems to be invalid", mc.x,mc.y,mc.z);
		Msg("Level box: %f,%f,%f-%f,%f,%f,",phBoundaries.x1,phBoundaries.y1,phBoundaries.z1,phBoundaries.x2,phBoundaries.y2,phBoundaries.z2);
		Msg("Object: %s",PhysicsRefObject()->Name());
		Msg("Visual: %s",*(PhysicsRefObject()->cNameVisual()));
		VERIFY(0);
	}
#endif
	dBodySetPosition(m_body,mc.x,mc.y,mc.z);
	Fmatrix33 m33;
	m33.set(m0);
	dMatrix3 R;
	PHDynamicData::FMX33toDMX(m33,R);
	dBodySetRotation(m_body,R);
	CPHDisablingFull::Reinit();

	VERIFY2(dBodyGetPosition(m_body),"not valide safe position");
	VERIFY2(dBodyGetLinearVel(m_body),"not valide safe velocity");
	m_flags.set(flUpdate,TRUE);
}

void CPHElement::getQuaternion(Fquaternion& quaternion)
{
	if(!isActive()) return;
	const float* q=dBodyGetQuaternion(m_body);
	quaternion.set(-q[0],q[1],q[2],q[3]);
	VERIFY(_valid(quaternion));
}
void CPHElement::setQuaternion(const Fquaternion& quaternion)
{
	VERIFY(_valid(quaternion));
	if(!isActive()) return;
	dQuaternion q={-quaternion.w,quaternion.x,quaternion.y,quaternion.z};
	dBodySetQuaternion(m_body,q);
	CPHDisablingRotational::Reinit();
	m_flags.set(flUpdate,TRUE);
}
void CPHElement::GetGlobalPositionDynamic(Fvector* v)
{
	if(!isActive()) return;
	v->set((*(Fvector*)dBodyGetPosition(m_body)));
	VERIFY(_valid(*v));
}

void CPHElement::SetGlobalPositionDynamic(const Fvector& position)
{
	if(!isActive()) return;
	VERIFY(_valid(position));
#ifdef DEBUG
	if(!valid_pos(position,phBoundaries))
	{
		Msg("position: %f,%f,%f, seems to be invalid", position.x,position.y,position.z);
		Msg("Level box: %f,%f,%f-%f,%f,%f,",phBoundaries.x1,phBoundaries.y1,phBoundaries.z1,phBoundaries.x2,phBoundaries.y2,phBoundaries.z2);
		Msg("Object: %s",PhysicsRefObject()->Name());
		Msg("Visual: %s",*(PhysicsRefObject()->cNameVisual()));
		VERIFY(0);
	}
#endif
	dBodySetPosition(m_body,position.x,position.y,position.z);
	CPHDisablingTranslational::Reinit();
	m_flags.set(flUpdate,TRUE);
}

void CPHElement::TransformPosition(const Fmatrix &form)
{
	if(!isActive())return;
	VERIFY(_valid(form));
	R_ASSERT2(m_body,"body is not created");
	Fmatrix bm;
	PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body),dBodyGetPosition(m_body),bm);
	Fmatrix new_bm;
	new_bm.mul(form,bm);
	dMatrix3 dBM;
	PHDynamicData::FMXtoDMX(new_bm,dBM);
	dBodySetRotation(m_body,dBM);
#ifdef DEBUG
	if(!valid_pos(new_bm.c,phBoundaries))
	{
		Msg("new_bm.c: %f,%f,%f, seems to be invalid", new_bm.c.x,new_bm.c.y,new_bm.c.z);
		Msg("Level box: %f,%f,%f-%f,%f,%f,",phBoundaries.x1,phBoundaries.y1,phBoundaries.z1,phBoundaries.x2,phBoundaries.y2,phBoundaries.z2);
		Msg("Object: %s",PhysicsRefObject()->Name());
		Msg("Visual: %s",*(PhysicsRefObject()->cNameVisual()));
		VERIFY(0);
	}
#endif
	dBodySetPosition(m_body,new_bm.c.x,new_bm.c.y,new_bm.c.z);
	CPHDisablingFull::Reinit();
	m_body_interpolation.ResetPositions();
	m_body_interpolation.ResetRotations();
	m_flags.set(flUpdate,TRUE);
}
CPHElement::~CPHElement	()
{
	VERIFY(!isActive());
	DeleteFracturesHolder();

}

void CPHElement::Activate(const Fmatrix &transform,const Fvector& lin_vel,const Fvector& ang_vel,bool disable){
	VERIFY(!isActive());
	mXFORM.set(transform);
	Start();
	SetTransform(transform);

	dBodySetLinearVel(m_body,lin_vel.x,lin_vel.y,lin_vel.z);

	dBodySetAngularVel(m_body,ang_vel.x,ang_vel.y,ang_vel.z);
	VERIFY(dBodyStateValide(m_body));
//	dVectorSet(m_safe_position,dBodyGetPosition(m_body));
//	dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));
//	dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));

	m_body_interpolation.SetBody(m_body);

	if(disable) dBodyDisable(m_body);
	m_flags.set(flActive,TRUE);
	m_flags.set(flActivating,TRUE);
	CKinematics* K=m_shell->PKinematics();
	if(K)
	{
		K->LL_GetBoneInstance(m_SelfID).set_callback(bctPhysics,m_shell->GetBonesCallback(),static_cast<CPhysicsElement*>(this));
	}
}
void CPHElement::Activate(const Fmatrix &m0,float dt01,const Fmatrix &m2,bool disable){

	Fvector lvel,avel;
	lvel.set(m2.c.x-m0.c.x,m2.c.y-m0.c.y,m2.c.z-m0.c.z);
	avel.set(0.f,0.f,0.f);
	Activate(m0,lvel,avel,disable);

}


void CPHElement::Activate(bool disable){

	Fvector lvel,avel;
	lvel.set(0.f,0.f,0.f);
	avel.set(0.f,0.f,0.f);
	Activate(mXFORM,lvel,avel,disable);

}

void CPHElement::Activate(const Fmatrix& start_from,bool disable){
	VERIFY(_valid(start_from));
	Fmatrix globe;
	globe.mul_43(start_from,mXFORM);

	Fvector lvel,avel;
	lvel.set(0.f,0.f,0.f);
	avel.set(0.f,0.f,0.f);
	Activate(globe,lvel,avel,disable);

}

void CPHElement::Update(){
	if(!isActive()) return;
	if(m_flags.test(flActivating)) m_flags.set(flActivating,FALSE);
	if( !dBodyIsEnabled(m_body)&&!m_flags.test(flUpdate)/*!bUpdate*/) return;

	InterpolateGlobalTransform(&mXFORM);
	VERIFY2(_valid(mXFORM),"invalid position in update");
}

void CPHElement::PhTune(dReal step)
{
	if(!isActive()) return;
	CPHContactBodyEffector* contact_effector=
		(CPHContactBodyEffector*) dBodyGetData(m_body);
	if(contact_effector)contact_effector->Apply();
#ifdef DEBUG
	if(!valid_pos(cast_fv(dBodyGetPosition(m_body)),phBoundaries))
	{
		Msg("m0.c: %f,%f,%f, seems to be invalid", cast_fv(dBodyGetPosition(m_body)).x,cast_fv(dBodyGetPosition(m_body)).y,cast_fv(dBodyGetPosition(m_body)).z);
		Msg("Level box: %f,%f,%f-%f,%f,%f,",phBoundaries.x1,phBoundaries.y1,phBoundaries.z1,phBoundaries.x2,phBoundaries.y2,phBoundaries.z2);
		Msg("Object: %s",PhysicsRefObject()->Name());
		Msg("Visual: %s",*(PhysicsRefObject()->cNameVisual()));
		VERIFY(0);
	}
#endif
}
void CPHElement::PhDataUpdate(dReal step){

	if(! isActive())return;
#ifdef DEBUG
	if(!valid_pos(cast_fv(dBodyGetPosition(m_body)),phBoundaries))
	{
		Msg("body position: %f,%f,%f, seems to be invalid", cast_fv(dBodyGetPosition(m_body)).x,cast_fv(dBodyGetPosition(m_body)).y,cast_fv(dBodyGetPosition(m_body)).z);
		Msg("Level box: %f,%f,%f-%f,%f,%f,",phBoundaries.x1,phBoundaries.y1,phBoundaries.z1,phBoundaries.x2,phBoundaries.y2,phBoundaries.z2);
		Msg("Object: %s",PhysicsRefObject()->Name());
		Msg("Visual: %s",*(PhysicsRefObject()->cNameVisual()));
		VERIFY(0);
	}
#endif
	///////////////skip for disabled elements////////////////////////////////////////////////////////////
	//b_enabled_onstep=!!dBodyIsEnabled(m_body);
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phDbgDrawMassCenters))
	{
		DBG_DrawPoint(cast_fv(dBodyGetPosition(m_body)),0.03f,D3DCOLOR_XRGB(255,0,0));
	}
#endif
	
	m_flags.set(flEnabledOnStep,!!dBodyIsEnabled(m_body));
	if(!m_flags.test(flEnabledOnStep)/*!b_enabled_onstep*/) return;
	


	//////////////////////////////////base pointers/////////////////////////////////////////////////
	const dReal* linear_velocity	=	dBodyGetLinearVel(m_body)	;
	const dReal* angular_velocity	=	dBodyGetAngularVel(m_body)	;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////scale velocity///////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	VERIFY(dV_valid(linear_velocity));
#ifdef DEBUG
	if(!dV_valid(angular_velocity))
	{
		Msg("angular vel %f,%f,%f",angular_velocity[0],angular_velocity[1],angular_velocity[2]);
		Msg("linear vel %f,%f,%f",linear_velocity[0],linear_velocity[1],linear_velocity[2]);
		Msg("position  %f,%f,%f",dBodyGetPosition(m_body)[0],dBodyGetPosition(m_body)[1],dBodyGetPosition(m_body)[2]);
		Msg("quaternion  %f,%f,%f,%f",dBodyGetQuaternion(m_body)[0],dBodyGetQuaternion(m_body)[1],dBodyGetQuaternion(m_body)[2],dBodyGetQuaternion(m_body)[3]);
		Msg("matrix");
		Msg("x  %f,%f,%f",dBodyGetRotation(m_body)[0],dBodyGetRotation(m_body)[4],dBodyGetRotation(m_body)[8]);
		Msg("y  %f,%f,%f",dBodyGetRotation(m_body)[1],dBodyGetRotation(m_body)[5],dBodyGetRotation(m_body)[9]);
		Msg("z  %f,%f,%f",dBodyGetRotation(m_body)[2],dBodyGetRotation(m_body)[6],dBodyGetRotation(m_body)[10]);
		CPhysicsShellHolder* ph=PhysicsRefObject();
		Msg("name visual %s",*ph->cNameVisual());
		Msg("name obj %s",ph->Name());
		Msg("name section %s",*ph->cNameSect());
		VERIFY2(0,"bad angular velocity");
	}
#endif
	VERIFY(!fis_zero(m_l_scale));
	VERIFY(!fis_zero(m_w_scale));
	dBodySetLinearVel(
		m_body,
		linear_velocity[0]			/m_l_scale		,
		linear_velocity[1]			/m_l_scale		,
		linear_velocity[2]			/m_l_scale
		);
	dBodySetAngularVel(
		m_body,
		angular_velocity[0]			/m_w_scale		,
		angular_velocity[1]			/m_w_scale		,
		angular_velocity[2]			/m_w_scale
		);

	
	///////////////////scale changes values directly so get base values after it/////////////////////////
	/////////////////////////////base values////////////////////////////////////////////////////////////
	dReal linear_velocity_smag	=		dDOT(linear_velocity,linear_velocity);
	dReal linear_velocity_mag		=	_sqrt(linear_velocity_smag);

	dReal angular_velocity_smag	=	dDOT(angular_velocity,angular_velocity);
	dReal angular_velocity_mag	=	_sqrt(angular_velocity_smag);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////limit velocity & secure /////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	////////////////limit linear vel////////////////////////////////////////////////////////////////////////////////////////

	VERIFY(dV_valid(linear_velocity));
	if(linear_velocity_mag>m_l_limit)
	{
		dReal f=linear_velocity_mag/m_l_limit;
		linear_velocity_mag=m_l_limit;
		linear_velocity_smag=m_l_limit*m_l_limit;
		VERIFY(!fis_zero(f));
		dBodySetLinearVel(
			m_body,
			linear_velocity[0]/f,
			linear_velocity[1]/f,
			linear_velocity[2]/f
			);
	}
	////////////////secure position///////////////////////////////////////////////////////////////////////////////////
	const dReal* position=dBodyGetPosition(m_body);
	VERIFY(dV_valid(position));
	/////////////////limit & secure angular vel///////////////////////////////////////////////////////////////////////////////
	VERIFY(dV_valid(angular_velocity));

	if(angular_velocity_mag>m_w_limit)
	{
		dReal f=angular_velocity_mag/m_w_limit;
		angular_velocity_mag=m_w_limit;
		angular_velocity_smag=m_w_limit*m_w_limit;
		VERIFY(!fis_zero(f));
		dBodySetAngularVel(
			m_body,
			angular_velocity[0]/f,
			angular_velocity[1]/f,
			angular_velocity[2]/f);
	}

	////////////////secure rotation////////////////////////////////////////////////////////////////////////////////////////
	{

	VERIFY(dQ_valid(dBodyGetQuaternion(m_body)));


	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////disable///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if(dBodyIsEnabled(m_body)) Disabling();
	if(!dBodyIsEnabled(m_body)) return;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////air resistance/////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(!fis_zero(k_w))
		dBodyAddTorque(
		m_body,
		-angular_velocity[0]*k_w,
		-angular_velocity[1]*k_w,
		-angular_velocity[2]*k_w
		);

	dMass mass;
	dBodyGetMass(m_body,&mass);
	dReal l_air=linear_velocity_mag*k_l;//force/velocity !!!
	if(l_air>mass.mass/fixed_step) l_air=mass.mass/fixed_step;//validate

	if(!fis_zero(l_air))
		dBodyAddForce(
		m_body,
		-linear_velocity[0]*l_air,
		-linear_velocity[1]*l_air,
		-linear_velocity[2]*l_air
		);
	VERIFY(dBodyStateValide(m_body));
	VERIFY2(dV_valid(dBodyGetPosition(m_body)),"invalid body position");
	VERIFY2(dV_valid(dBodyGetQuaternion(m_body)),"invalid body rotation");
#ifdef DEBUG
	if(!valid_pos(cast_fv(dBodyGetPosition(m_body)),phBoundaries))
	{
		Msg("body position: %f,%f,%f, seems to be invalid", cast_fv(dBodyGetPosition(m_body)).x,cast_fv(dBodyGetPosition(m_body)).y,cast_fv(dBodyGetPosition(m_body)).z);
		Msg("Level box: %f,%f,%f-%f,%f,%f,",phBoundaries.x1,phBoundaries.y1,phBoundaries.z1,phBoundaries.x2,phBoundaries.y2,phBoundaries.z2);
		Msg("Object: %s",PhysicsRefObject()->Name());
		Msg("Visual: %s",*(PhysicsRefObject()->cNameVisual()));
		VERIFY(0);
	}
#endif
	UpdateInterpolation				();

}

void CPHElement::Enable()	{
	if(!isActive()) return;
	m_shell->EnableObject(0);
	if(dBodyIsEnabled(m_body)) return;
	dBodyEnable(m_body);
}

void CPHElement::Disable()	{

//	return;
	if(!isActive()||!dBodyIsEnabled(m_body)) return;
	FillInterpolation();

	dBodyDisable(m_body);
}

void CPHElement::ReEnable(){

	//dJointGroupEmpty(m_saved_contacts);
	//b_contacts_saved=false;

}

void	CPHElement::Freeze()
{
	if(!m_body) return;

	m_flags.set(flWasEnabledBeforeFreeze,!!dBodyIsEnabled(m_body));
	dBodyDisable(m_body);
}
void	CPHElement::UnFreeze()
{
	if(!m_body) return;
	if(m_flags.test(flWasEnabledBeforeFreeze)/*was_enabled_before_freeze*/)dBodyEnable(m_body);
}
void	CPHElement::applyImpulseVsMC(const Fvector& pos,const Fvector& dir, float val)
{
	if(!isActive()||m_flags.test(flFixed)) return;
	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	/////////////////////////////////////////////////////////////////////////
	Fvector impulse;
	val/=fixed_step;
	impulse.set(dir);
	impulse.mul(val);
	dBodyAddForceAtRelPos(m_body, impulse.x,impulse.y,impulse.z,pos.x, pos.y,pos.z);
	BodyCutForce(m_body,m_l_limit,m_w_limit);
	////////////////////////////////////////////////////////////////////////
}
void	CPHElement::applyImpulseVsGF(const Fvector& pos,const Fvector& dir, float val)
{
	VERIFY(_valid(pos)&&_valid(dir)&&_valid(val));
	if(!isActive()||m_flags.test(flFixed)) return;
	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	/////////////////////////////////////////////////////////////////////////
	Fvector impulse;
	val/=fixed_step;
	impulse.set(dir);
	impulse.mul(val);
	dBodyAddForceAtPos(m_body, impulse.x,impulse.y,impulse.z,pos.x, pos.y,pos.z);
	BodyCutForce(m_body,m_l_limit,m_w_limit);
	VERIFY(dBodyStateValide(m_body));
	////////////////////////////////////////////////////////////////////////
}
void	CPHElement::	applyImpulseTrace		(const Fvector& pos, const Fvector& dir, float val,u16 id)
{

	VERIFY(_valid(pos)&&_valid(dir)&&_valid(val));
	if(!isActive()||m_flags.test(flFixed)) return;
	Fvector body_pos;
	if(id!=BI_NONE)
	{
		if(id==m_SelfID)
		{
			body_pos.sub(pos,m_mass_center);
		}
		else
		{ 
			CKinematics* K=m_shell->PKinematics();
			if(K)
			{
				Fmatrix m;m.set(K->LL_GetTransform(m_SelfID));
				m.invert();m.mulB_43(K->LL_GetTransform(id));
				m.transform(body_pos,pos);
				body_pos.sub(m_mass_center);
			}
			else
			{
				body_pos.set(0.f,0.f,0.f);
			}
		}
	}
	else
	{
		body_pos.set(0.f,0.f,0.f);
	}
#ifdef DEBUG
	if(ph_dbg_draw_mask.test(phHitApplicationPoints))
	{
		DBG_OpenCashedDraw();
		Fvector dbg_position;dbg_position.set(body_pos);
		dMULTIPLY0_331 (cast_fp(dbg_position),dBodyGetRotation(m_body),cast_fp(body_pos));
		dbg_position.add(cast_fv(dBodyGetPosition(m_body)));
		DBG_DrawPoint(dbg_position,0.01f,D3DCOLOR_XRGB(255,255,255));
		DBG_DrawLine(cast_fv(dBodyGetPosition(m_body)),dbg_position,D3DCOLOR_XRGB(255,255,255));
		DBG_DrawLine(dbg_position,Fvector().add(dbg_position,Fvector().mul(dir,0.4f)),D3DCOLOR_XRGB(255,0,255));
		DBG_ClosedCashedDraw(10000);
	}
#endif	
	applyImpulseVsMC(body_pos,dir,val);
	if(m_fratures_holder)
	{
		///impulse.add(*((Fvector*)dBodyGetPosition(m_body)));
		Fvector impulse;impulse.set(dir);impulse.mul(val/fixed_step);
		m_fratures_holder->AddImpact(impulse,body_pos,m_shell->BoneIdToRootGeom(id));
	}
}
void CPHElement::applyImpact(const SPHImpact& I)
{
	Fvector pos;
	pos.add(I.point,m_mass_center);
	Fvector dir;
	dir.set(I.force);
	float val=I.force.magnitude();
	
	if(!fis_zero(val)&& GeomByBoneID(I.geom))
	{
		
		dir.mul(1.f/val);
		applyImpulseTrace(pos,dir,val,I.geom);
	}
	
}
void CPHElement::InterpolateGlobalTransform(Fmatrix* m){
	m_body_interpolation.InterpolateRotation(*m);
	m_body_interpolation.InterpolatePosition(m->c);
	//m->mulB_43(m_inverse_local_transform);
	MulB43InverceLocalForm(*m);
	//bUpdate=false;
	m_flags.set(flUpdate,FALSE);
	VERIFY(_valid(*m));
}
void CPHElement::GetGlobalTransformDynamic(Fmatrix* m)
{
	PHDynamicData::DMXPStoFMX(dBodyGetRotation(m_body),dBodyGetPosition(m_body),*m);
	MulB43InverceLocalForm(*m);
	//m->mulB_43(m_inverse_local_transform);
	//bUpdate=false;
	//m_flags.set(flUpdate,FALSE);
	VERIFY(_valid(*m));
}

void CPHElement::InterpolateGlobalPosition(Fvector* v){
	m_body_interpolation.InterpolatePosition(*v);
	VERIFY(_valid(*v));
	//v->add(m_inverse_local_transform.c);

}




void CPHElement::build(bool disable){

	if(isActive()) return;
	//bActive=true;
	//bActivating=true;
	m_flags.set(flActive,TRUE);
	m_flags.set(flActivating,TRUE);
	build();
	//	if(place_current_forms)
	{

		SetTransform(mXFORM);
	}

	m_body_interpolation.SetBody(m_body);
	//previous_f[0]=dInfinity;
	if(disable) dBodyDisable(m_body);


}

void CPHElement::RunSimulation(const Fmatrix& start_from)
{

	RunSimulation();
	//	if(place_current_forms)
	{
		Fmatrix globe;
		globe.mul(start_from,mXFORM);
		SetTransform(globe);
	}
	//dVectorSet(m_safe_position,dBodyGetPosition(m_body));
	//dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));
	//dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));

}



void CPHElement::StataticRootBonesCallBack(CBoneInstance* B)
{
	Fmatrix parent;
	VERIFY2( isActive(),"the element is not active");
	VERIFY(_valid(m_shell->mXFORM));
	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback resive 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c,phBoundaries));
	if(m_flags.test(flActivating))
	{
		//if(!dBodyIsEnabled(m_body))
		//	dBodyEnable(m_body);
		VERIFY(_valid(B->mTransform));
		if(m_shell->dSpace()->lock_count) return;
		mXFORM.set(B->mTransform);
		//m_start_time=Device.fTimeGlobal;
		Fmatrix global_transform;
		//if(m_parent_element)
		global_transform.mul_43(m_shell->mXFORM,mXFORM);
		SetTransform(global_transform);

		FillInterpolation();
		//bActivating=false;
		m_flags.set(flActivating,FALSE);
		if(!m_parent_element) 
		{
			m_shell->m_object_in_root.set(mXFORM);
			m_shell->m_object_in_root.invert();
			m_shell->SetNotActivating();
		}
		B->Callback_overwrite=TRUE;
		//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
		VERIFY_RMATRIX(B->mTransform);
		VERIFY(valid_pos(B->mTransform.c,phBoundaries));
		//return;
	}

	

	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c,phBoundaries));
	if( !m_shell->is_active() && !m_flags.test(flUpdate)/*!bUpdate*/ ) return;

	{
		InterpolateGlobalTransform(&mXFORM);
		parent.set			(m_shell->mXFORM);
		parent.invert		();
		mXFORM.mulA_43		(parent);
		B->mTransform.set	(mXFORM);
	}
	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c,phBoundaries));
	VERIFY2(_valid(B->mTransform),"Bones callback returns bad matrix");
	//else
	//{

	//	InterpolateGlobalTransform(&m_shell->mXFORM);
	//	mXFORM.identity();
	//	B->mTransform.set(mXFORM);
	//parent.set(B->mTransform);
	//parent.invert();
	//m_shell->mXFORM.mulB(parent);

	//}


}

void CPHElement::BonesCallBack(CBoneInstance* B)
{
	Fmatrix parent;
	VERIFY (isActive());
	VERIFY(_valid(m_shell->mXFORM));
	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback receive 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
#ifdef DEBUG
	if(!valid_pos(B->mTransform.c,phBoundaries))
	{
		Msg("Bone pos: %f,%f,%f, seems to be invalid", B->mTransform.c.x,B->mTransform.c.y,B->mTransform.c.z);
		Msg("Level box: %f,%f,%f-%f,%f,%f,",phBoundaries.x1,phBoundaries.y1,phBoundaries.z1,phBoundaries.x2,phBoundaries.y2,phBoundaries.z2);
		Msg("Object: %s",PhysicsRefObject()->Name());
		Msg("Visual: %s",*(PhysicsRefObject()->cNameVisual()));
		VERIFY(0);
	}
#endif
	
	if(m_flags.test(flActivating))
	{
		//if(!dBodyIsEnabled(m_body))
		//	dBodyEnable(m_body);
		VERIFY(_valid(B->mTransform));
		if(m_shell->dSpace()->lock_count) return;
		mXFORM.set(B->mTransform);
		//m_start_time=Device.fTimeGlobal;
		Fmatrix global_transform;
		//if(m_parent_element)
		global_transform.mul_43(m_shell->mXFORM, mXFORM);
		SetTransform(global_transform);

		FillInterpolation();
		//bActivating=false;
		m_flags.set(flActivating,FALSE);
		if(!m_parent_element) 
		{
			m_shell->m_object_in_root.set(mXFORM);
			m_shell->m_object_in_root.invert();
			m_shell->SetNotActivating();
		}
		B->Callback_overwrite=TRUE;
		//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
		VERIFY_RMATRIX(B->mTransform);
		VERIFY(valid_pos(B->mTransform.c,phBoundaries));
		return;
	}

	

	if( !m_shell->is_active() && !m_flags.test(flUpdate)/*!bUpdate*/) return;

	if(!m_parent_element)
	{
	
		m_shell->InterpolateGlobalTransform(&(m_shell->mXFORM));
		VERIFY(_valid(m_shell->mXFORM));
	}
	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c,phBoundaries));

	
	{
		InterpolateGlobalTransform(&mXFORM);
		parent.set			(m_shell->mXFORM);
		parent.invert		();
		mXFORM.mulA_43		(parent);
		B->mTransform.set	(mXFORM);
	}
	//VERIFY2(fsimilar(DET(B->mTransform),1.f,DET_CHECK_EPS),"Bones callback returns 0 matrix");
	VERIFY_RMATRIX(B->mTransform);
	VERIFY(valid_pos(B->mTransform.c,phBoundaries));
	VERIFY2(_valid(B->mTransform),"Bones callback returns bad matrix");
	//else
	//{

	//	InterpolateGlobalTransform(&m_shell->mXFORM);
	//	mXFORM.identity();
	//	B->mTransform.set(mXFORM);
	//parent.set(B->mTransform);
	//parent.invert();
	//m_shell->mXFORM.mulB(parent);

	//}


}

void CPHElement::set_PhysicsRefObject(CPhysicsShellHolder* ref_object)
{
	CPHGeometryOwner::set_PhysicsRefObject(ref_object);
}


void CPHElement::set_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	CPHGeometryOwner::set_ObjectContactCallback(callback);
}
void CPHElement::add_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	CPHGeometryOwner::add_ObjectContactCallback(callback);
}
void CPHElement::remove_ObjectContactCallback(ObjectContactCallbackFun* callback)
{
	CPHGeometryOwner::remove_ObjectContactCallback(callback);
}
ObjectContactCallbackFun* CPHElement::get_ObjectContactCallback()
{
	return CPHGeometryOwner::get_ObjectContactCallback();
}
void CPHElement::set_CallbackData(void * cd)
{
	CPHGeometryOwner::set_CallbackData(cd);
}
void*	CPHElement::					get_CallbackData						()
{
	return	CPHGeometryOwner::get_CallbackData();
}
void CPHElement::set_ContactCallback(ContactCallbackFun* callback)
{
	//push_untill=0;
	CPHGeometryOwner::set_ContactCallback(callback);
}

void CPHElement::SetMaterial(u16 m)
{
	CPHGeometryOwner::SetMaterial(m);
}

dMass*	CPHElement::getMassTensor()																						//aux
{
	return &m_mass;
}

void	CPHElement::setInertia(const dMass& M)
{
	m_mass=M;
	if(!isActive()||m_flags.test(flFixed))return;
	dBodySetMass(m_body,&M);
}

void	CPHElement::addInertia(const dMass& M)
{
	dMassAdd(&m_mass,&M);
	if(!isActive())return;
	dBodySetMass(m_body,&m_mass);
}
void CPHElement::get_LinearVel(Fvector& velocity)
{
	if(!isActive()||!dBodyIsEnabled(m_body))
	{
		velocity.set(0,0,0);
		return;
	}
	dVectorSet((dReal*)&velocity,dBodyGetLinearVel(m_body));
	
}
void CPHElement::get_AngularVel	(Fvector& velocity)
{
	if(!isActive()||!dBodyIsEnabled(m_body))
	{
		velocity.set(0,0,0);
		return;
	}
	dVectorSet((dReal*)&velocity,dBodyGetAngularVel(m_body));
}

void CPHElement::set_LinearVel			  (const Fvector& velocity)
{
	if(!isActive()||m_flags.test(flFixed)) return;
	VERIFY2(_valid(velocity),"not valid arqument velocity");
	float sq_mag=velocity.square_magnitude();
	if(sq_mag>m_l_limit*m_l_limit)
	{
		float mag=_sqrt(sq_mag);
		Fvector vel;vel.mul(velocity,m_l_limit/mag);
		dBodySetLinearVel(m_body,vel.x,vel.y,vel.z);
#ifdef DEBUG
		Msg(" CPHElement::set_LinearVel set velocity magnitude is too large %f",mag);
#endif
		
	}else
		dBodySetLinearVel(m_body,velocity.x,velocity.y,velocity.z);
	//dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));
}
void CPHElement::set_AngularVel			  (const Fvector& velocity)
{
	VERIFY(_valid(velocity));
	if(!isActive()||m_flags.test(flFixed)) return;
	float sq_mag=velocity.square_magnitude();
	if(sq_mag>m_w_limit*m_w_limit)
	{
		float mag=_sqrt(sq_mag);
		Fvector vel;vel.mul(velocity,m_w_limit/mag);
		dBodySetAngularVel(m_body,vel.x,vel.y,vel.z);
#ifdef DEBUG
		Msg("CPHElement::set_AngularVel set velocity magnitude is too large %f",mag);
#endif
		
	}else
		dBodySetAngularVel(m_body,velocity.x,velocity.y,velocity.z);
}

void	CPHElement::getForce(Fvector& force)
{
	if(!isActive()) return;
	force.set(*(Fvector*)dBodyGetForce(m_body));
	VERIFY(dBodyStateValide(m_body));
}
void	CPHElement::getTorque(Fvector& torque)
{
	if(!isActive()) return;
	torque.set(*(Fvector*)dBodyGetTorque(m_body));
	VERIFY(dBodyStateValide(m_body));
}
void	CPHElement::setForce(const Fvector& force)
{
	if(!isActive()||m_flags.test(flFixed)) return;
	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject(0);
	dBodySetForce(m_body,force.x,force.y,force.z);
	BodyCutForce(m_body,m_l_limit,m_w_limit);
	VERIFY(dBodyStateValide(m_body));
}
void	CPHElement::setTorque(const Fvector& torque)
{
	if(!isActive()||m_flags.test(flFixed)) return;
	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject(0);
	dBodySetTorque(m_body,torque.x,torque.y,torque.z);
	BodyCutForce(m_body,m_l_limit,m_w_limit);
	VERIFY(dBodyStateValide(m_body));
}

void	CPHElement::applyForce(const Fvector& dir, float val)															//aux
{
	applyForce				(dir.x*val,dir.y*val,dir.z*val);
}
void	CPHElement::applyForce(float x,float y,float z)																//called anywhere ph state influent
{
	VERIFY(_valid(x)&&_valid(y)&&_valid(z));
	if(!isActive())return;//hack??
	if(m_flags.test(flFixed)) return;
	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject(0);
	dBodyAddForce(m_body,x,y,z);
	BodyCutForce(m_body,m_l_limit,m_w_limit);
	VERIFY(dBodyStateValide(m_body));
}

void	CPHElement::applyImpulse(const Fvector& dir, float val)//aux
{														
	
	applyForce(dir.x*val/fixed_step,dir.y*val/fixed_step,dir.z*val/fixed_step);
}



void CPHElement::add_Shape(const SBoneShape& shape,const Fmatrix& offset)
{
	CPHGeometryOwner::add_Shape(shape,offset);
}

void CPHElement::add_Shape(const SBoneShape& shape)
{
	CPHGeometryOwner::add_Shape(shape);
}

#pragma todo(remake it using Geometry functions)

void CPHElement::add_Mass(const SBoneShape& shape,const Fmatrix& offset,const Fvector& mass_center,float mass,CPHFracture* fracture)
{

	dMass m;
	dMatrix3 DMatx;
	switch(shape.type) 
	{
	case SBoneShape::stBox	:
		{
			dMassSetBox(&m,1.f,shape.box.m_halfsize.x*2.f,shape.box.m_halfsize.y*2.f,shape.box.m_halfsize.z*2.f);
			dMassAdjust(&m,mass);
			Fmatrix box_transform;
			shape.box.xform_get(box_transform);
			PHDynamicData::FMX33toDMX(shape.box.m_rotate,DMatx);
			dMassRotate(&m,DMatx);
			dMassTranslate(&m,shape.box.m_translate.x-mass_center.x,shape.box.m_translate.y-mass_center.y,shape.box.m_translate.z-mass_center.z);
			break;
		}
	case SBoneShape::stSphere	:
		{
			shape.sphere;
			dMassSetSphere(&m,1.f,shape.sphere.R);
			dMassAdjust(&m,mass);
			dMassTranslate(&m,shape.sphere.P.x-mass_center.x,shape.sphere.P.y-mass_center.y,shape.sphere.P.z-mass_center.z);
			break;
		}


	case SBoneShape::stCylinder :
		{
			const Fvector& pos=shape.cylinder.m_center;
			Fvector l;
			l.sub(pos,mass_center);
			dMassSetCylinder(&m,1.f,2,shape.cylinder.m_radius,shape.cylinder.m_height);
			dMassAdjust(&m,mass);
			dMatrix3 DMatx;
			Fmatrix33 m33;
			m33.j.set(shape.cylinder.m_direction);
			Fvector::generate_orthonormal_basis(m33.j,m33.k,m33.i);
			PHDynamicData::FMX33toDMX(m33,DMatx);
			dMassRotate(&m,DMatx);
			dMassTranslate(&m,l.x,l.y,l.z);
			break;
		}

	case SBoneShape::stNone :
		break;
	default: NODEFAULT;
	}
	PHDynamicData::FMXtoDMX(offset,DMatx);
	dMassRotate(&m,DMatx);

	Fvector mc;
	offset.transform_tiny(mc,mass_center);
	//calculate _new mass_center 
	//new_mc=(m_mass_center*m_mass.mass+mc*mass)/(mass+m_mass.mass)
	Fvector tmp1;
	tmp1.set(m_mass_center);
	tmp1.mul(m_mass.mass);
	Fvector tmp2;
	tmp2.set(mc);
	tmp2.mul(mass);
	Fvector new_mc;
	new_mc.add(tmp1,tmp2);
	new_mc.mul(1.f/(mass+m_mass.mass));
	mc.sub(new_mc);
	dMassTranslate(&m,mc.x,mc.y,mc.z);
	m_mass_center.sub(new_mc);
	dMassTranslate(&m_mass,m_mass_center.x,m_mass_center.y,m_mass_center.z);
	if(m_fratures_holder)
	{
		m_fratures_holder->DistributeAdditionalMass(u16(m_geoms.size()-1),m);
	}
	if(fracture)
	{
		fracture->MassAddToSecond(m);
	}
	R_ASSERT2(dMass_valide(&m),"bad bone mass params");
	dMassAdd(&m_mass,&m);
	R_ASSERT2(dMass_valide(&m),"bad result mass params");
	m_mass_center.set(new_mc);
}

void CPHElement::set_BoxMass(const Fobb& box, float mass)
{
	dMassSetZero(&m_mass);
	m_mass_center.set(box.m_translate);
	const Fvector& hside=box.m_halfsize;
	dMassSetBox(&m_mass,1,hside.x*2.f,hside.y*2.f,hside.z*2.f);
	dMassAdjust(&m_mass,mass);
	dMatrix3 DMatx;
	PHDynamicData::FMX33toDMX(box.m_rotate,DMatx);
	dMassRotate(&m_mass,DMatx);

}

void CPHElement::calculate_it_data_use_density(const Fvector& mc,float density)
{
	dMassSetZero(&m_mass);
	GEOM_I i_geom=m_geoms.begin(),e=m_geoms.end();
	for(;i_geom!=e;++i_geom)(*i_geom)->add_self_mass(m_mass,mc,density);
	VERIFY2(dMass_valide(&m_mass),"non valide mass obtained!");
}

float CPHElement::getRadius()
{
	return CPHGeometryOwner::getRadius();
}

void CPHElement::set_DynamicLimits(float l_limit,float w_limit)
{
	m_l_limit=l_limit;
	m_w_limit=w_limit;
}

void CPHElement::set_DynamicScales(float l_scale/* =default_l_scale */,float w_scale/* =default_w_scale */)
{
	m_l_scale=l_scale;
	m_w_scale=w_scale;
}

void	CPHElement::set_DisableParams				(const SAllDDOParams& params)
{
	CPHDisablingFull::set_DisableParams(params);
}




void CPHElement::get_Extensions(const Fvector& axis,float center_prg,float& lo_ext, float& hi_ext)
{
	CPHGeometryOwner::get_Extensions(axis,center_prg,lo_ext,hi_ext);
}

const Fvector& CPHElement::mass_Center()
{
	VERIFY(dBodyStateValide(m_body));
	return *((const Fvector*)dBodyGetPosition(m_body));
}

CPhysicsShell* CPHElement::PhysicsShell()
{
	return smart_cast<CPhysicsShell*>(m_shell);
}

CPHShell* CPHElement::PHShell()
{
	return (m_shell);
}
void	CPHElement::SetShell(CPHShell* p)
{
	if(!m_body||!m_shell)
		{
			m_shell=p;
			return;
		}
	if(m_shell!=p)
	{
		m_shell->Island().RemoveBody(m_body);
		p->Island().AddBody(m_body);
		m_shell=p;
	}

}
void CPHElement::PassEndGeoms(u16 from,u16 to,CPHElement* dest)
{
	GEOM_I i_from=m_geoms.begin()+from,e=m_geoms.begin()+to;
	u16 shift=to-from;
	GEOM_I i=i_from;
	for(;i!=e;++i)
	{
		(*i)->remove_from_space(m_group);
		//(*i)->add_to_space(dest->m_group);
		//(*i)->set_body(dest->m_body);
		(*i)->set_body(0);
		u16& element_pos=(*i)->element_position();
		element_pos=element_pos-shift;
	}
	GEOM_I last=m_geoms.end();
	for(;i!=last;++i)
	{
		u16& element_pos=(*i)->element_position();
		element_pos=element_pos-shift;
	}

	dest->m_geoms.insert(dest->m_geoms.end(),i_from,e);
	dest->b_builded=true;
	m_geoms.erase(i_from,e);
}
void CPHElement::SplitProcess(ELEMENT_PAIR_VECTOR &new_elements)
{
	m_fratures_holder->SplitProcess(this,new_elements);
	if(!m_fratures_holder->m_fractures.size()) xr_delete(m_fratures_holder);
}
void CPHElement::DeleteFracturesHolder()
{
	xr_delete(m_fratures_holder);
}

void CPHElement::CreateSimulBase()
{
	m_body=dBodyCreate(0);
	m_shell->Island().AddBody(m_body);
	//m_saved_contacts=dJointGroupCreate (0);
	//b_contacts_saved=false;
	dBodyDisable(m_body);
	CPHGeometryOwner::CreateSimulBase();
}
void CPHElement::ReAdjustMassPositions(const Fmatrix &shift_pivot,float density)
{

	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i)
	{
		(*i)->move_local_basis(shift_pivot);
	}
	if(m_shell->PKinematics())
	{
		float mass;
		get_mc_kinematics(m_shell->PKinematics(),m_mass_center,mass);
		calculate_it_data(m_mass_center,mass);
	}
	else
	{
	
		setDensity(density);
	}

	dBodySetMass(m_body,&m_mass);
	//m_inverse_local_transform.identity();
	//m_inverse_local_transform.c.set(m_mass_center);
	//m_inverse_local_transform.invert();
	//dBodySetPosition(m_body,m_mass_center.x,m_mass_center.y,m_mass_center.z);
}
void CPHElement::ResetMass(float density)
{
	Fvector tmp,shift_mc;

	tmp.set(m_mass_center);


	setDensity(density);
	dBodySetMass(m_body,&m_mass);

	shift_mc.sub(m_mass_center,tmp);
	tmp.set(*(Fvector *)dBodyGetPosition(m_body));
	tmp.add(shift_mc);
	

	//bActivating = true;
	m_flags.set(flActivating,TRUE);

	CPHGeometryOwner::setPosition(m_mass_center);
}
void CPHElement::ReInitDynamics(const Fmatrix &shift_pivot,float density)
{
	VERIFY(_valid(shift_pivot)&&_valid(density));
	ReAdjustMassPositions(shift_pivot,density);
	GEOM_I i=m_geoms.begin(),e=m_geoms.end();
	for(;i!=e;++i)
	{
		(*i)->set_position(m_mass_center);
		(*i)->set_body(m_body);
		//if(object_contact_callback)geom.set_obj_contact_cb(object_contact_callback);
		//if(m_phys_ref_object) geom.set_ref_object(m_phys_ref_object);
		if(m_group)
		{
			(*i)->add_to_space((dSpaceID)m_group);
		}
	}	
}

void CPHElement::PresetActive()
{
	if(isActive()) return;

	CBoneInstance& B=m_shell->PKinematics()->LL_GetBoneInstance(m_SelfID);
	mXFORM.set(B.mTransform);
	//m_start_time=Device.fTimeGlobal;
	Fmatrix global_transform;
	global_transform.mul_43(m_shell->mXFORM, mXFORM);
	SetTransform(global_transform);

	if(!m_parent_element) 
	{
		m_shell->m_object_in_root.set(mXFORM);
		m_shell->m_object_in_root.invert();

	}
	//dVectorSet(m_safe_position,dBodyGetPosition(m_body));
	//dQuaternionSet(m_safe_quaternion,dBodyGetQuaternion(m_body));
	//dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));

	//////////////////////////////////////////////////////////////
	//initializing values for disabling//////////////////////////
	//////////////////////////////////////////////////////////////
	VERIFY(dBodyStateValide(m_body));
	m_body_interpolation.SetBody(m_body);
	FillInterpolation();
	//bActive=true;
	m_flags.set(flActive,TRUE);
	RunSimulation();
	VERIFY(dBodyStateValide(m_body));
}


bool CPHElement::isBreakable()
{
	return !!m_fratures_holder;
}
u16	CPHElement::setGeomFracturable(CPHFracture& fracture)
{
	if(!m_fratures_holder) m_fratures_holder=xr_new<CPHFracturesHolder>();
	return m_fratures_holder->AddFracture(fracture);
}

CPHFracture& CPHElement::Fracture(u16 num)
{
	R_ASSERT2(m_fratures_holder,"no fractures!");
	return m_fratures_holder->Fracture(num);
}
u16	CPHElement::numberOfGeoms()
{
	return CPHGeometryOwner::numberOfGeoms();
}


void CPHElement::cv2bone_Xfrom(const Fquaternion& q,const Fvector& pos, Fmatrix& xform)
{
	VERIFY2(_valid(q)&&_valid(pos),"cv2bone_Xfrom receive wrong data");
	xform.rotation(q);
	xform.c.set(pos);
	//xform.mulB(m_inverse_local_transform);
	MulB43InverceLocalForm(xform);
	VERIFY2(_valid(xform),"cv2bone_Xfrom returns wrong data");
}
void CPHElement::cv2obj_Xfrom(const Fquaternion& q,const Fvector& pos, Fmatrix& xform)
{
	
	cv2bone_Xfrom(q,pos,xform);
	xform.mulB_43(m_shell->m_object_in_root);
	VERIFY2(_valid(xform),"cv2obj_Xfrom returns wrong data");
}

void CPHElement::set_ApplyByGravity(bool flag)
{
	if(!isActive()||m_flags.test(flFixed)) return;
	dBodySetGravityMode(m_body,flag);
}
bool CPHElement::get_ApplyByGravity()
{
	return (!!dBodyGetGravityMode(m_body));
}

void	CPHElement::Fix()
{
	m_flags.set(flFixed,TRUE);
	FixBody(m_body);
}
void	CPHElement::ReleaseFixed()
{
	if(!isFixed())	return;
	m_flags.set(flFixed,FALSE);
	if(!isActive())return;
	dBodySetMass(m_body,&m_mass);
}
void CPHElement::applyGravityAccel				(const Fvector& accel)
{
	VERIFY(_valid(accel));
	if(m_flags.test(flFixed)) return;
	if( !dBodyIsEnabled(m_body)) dBodyEnable(m_body);
	m_shell->EnableObject(0);
	Fvector val;
	val.set(accel);
	val.mul(m_mass.mass);
	//ApplyGravityAccel(m_body,(const dReal*)(&accel));
	applyForce(val.x,val.y,val.z);
}

void CPHElement::CutVelocity(float l_limit,float a_limit)
{
	
	if(!isActive())return;
	VERIFY(_valid(l_limit)&&_valid(a_limit));
	dVector3 limitedl,limiteda,diffl,diffa;
	bool blimitl=dVectorLimit(dBodyGetLinearVel(m_body),l_limit,limitedl);
	bool blimita=dVectorLimit(dBodyGetAngularVel(m_body),a_limit,limiteda);
	if(blimitl||blimita)
	{
			dVectorSub(diffl,limitedl,dBodyGetLinearVel(m_body));
			dVectorSub(diffa,limiteda,dBodyGetAngularVel(m_body));
			dBodySetLinearVel(m_body,diffl[0],diffl[1],diffl[2]);
			dBodySetAngularVel(m_body,diffa[0],diffa[1],diffa[2]);
			dxStepBody(m_body,fixed_step);
			dBodySetLinearVel(m_body,limitedl[0],limitedl[1],limitedl[2]);
			dBodySetAngularVel(m_body,limiteda[0],limiteda[1],limiteda[2]);
	}
}
void CPHElement::ClearDestroyInfo()
{
	xr_delete(m_fratures_holder);
}
//bool CPHElement::CheckBreakConsistent()
//{
//	if(!m_fratures_holder) return true;
//	m_fratures_holder->m_fractures
//	m_fratures_holder->Fracture()
//}