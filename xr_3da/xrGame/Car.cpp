#include "stdafx.h"
#include "ParticlesObject.h"
#include "Physics.h"
#include "car.h"
#include "hudmanager.h"
#include "cameralook.h"
#include "camerafirsteye.h"
#include "Actor.h"
#define   _USE_MATH_DEFINES
#include "math.h"

static float car_snd_volume=1.f;

BONE_P_MAP CCar::bone_map=BONE_P_MAP();

extern CPHWorld*	ph_world;

CCar::CCar(void)
{
	
	active_camera	= 0;
	camera[ectFirst]= xr_new<CCameraFirstEye>	(this, pSettings, "car_firsteye_cam",	CCameraBase::flRelativeLink|CCameraBase::flPositionRigid); 
	camera[ectFirst]->tag	= ectFirst;
	camera[ectChase]= xr_new<CCameraLook>		(this, pSettings, "car_look_cam",		CCameraBase::flRelativeLink); 
	camera[ectChase]->tag	= ectChase;
	camera[ectFree]	= xr_new<CCameraLook>		(this, pSettings, "car_free_cam",		0); 
	camera[ectFree]->tag	= ectFree;
	OnCameraChange(ectFirst);

	m_repairing		=false;
	m_owner			=NULL;

	///////////////////////////////
	//////////////////////////////
	/////////////////////////////
	b_wheels_limited=false;
	b_engine_on=false;
	e_state_steer=idle;
	e_state_drive=neutral;
	m_current_gear_ratio=dInfinity;
	rsp=false;lsp=false;fwp=false;bkp=false;brp=false;
	///////////////////////////////
	//////////////////////////////
	/////////////////////////////
}

CCar::~CCar(void)
{
	xr_delete			(camera[0]);
	xr_delete			(camera[1]);
	xr_delete			(camera[2]);
	snd_engine.destroy	();
	ClearExhausts();
}



void __stdcall  CCar::cb_Steer(CBoneInstance* B)
{
	CCar*	C			= dynamic_cast<CCar*>	(static_cast<CObject*>(B->Callback_Param));
	Fmatrix m;

	C->m_steer_angle=C->m_steering_wheels.begin()->GetSteerAngle()*0.1f+C->m_steer_angle*0.9f;
	m.rotateZ(C->m_steer_angle);

	B->mTransform.mulB	(m);
}

// Core events
void	CCar::Load					( LPCSTR section )
{
	inherited::Load					(section);
	car_snd_volume  				= pSettings->r_float(section,"snd_volume");
	snd_engine.create				(TRUE,"car\\car1");


}

BOOL	CCar::net_Spawn				(LPVOID DC)
{
	CSE_Abstract			*e		= (CSE_Abstract*)(DC);
	CSE_ALifeItemCar		*po		= dynamic_cast<CSE_ALifeItemCar*>(e);
	R_ASSERT(po);
	cNameVisual_set					(po->get_visual());
	BOOL R							= inherited::net_Spawn	(DC);

	setEnabled						(TRUE);
	setVisible						(TRUE);


	ParseDefinitions				();//parse ini filling in m_driving_wheels,m_steering_wheels,m_breaking_wheels
	CreateSkeleton					();//creates m_pPhysicsShell & fill in bone_map
	Init							();//inits m_driving_wheels,m_steering_wheels,m_breaking_wheels values using recieved in ParceDefinitions & from bone_map
	m_ident=ph_world->AddObject(dynamic_cast<CPHObject*>(this));

	m_pPhysicsShell->set_PhysicsRefObject(this);

	return R;
}

void	CCar::net_Destroy()
{
	inherited::net_Destroy();
	if(m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		m_pPhysicsShell->ZeroCallbacks();
		xr_delete(m_pPhysicsShell);
	}
	ClearExhausts();
	m_wheels_map.clear();
	m_steering_wheels.clear();
	m_driving_wheels.clear();
	m_exhausts.clear();
	m_breaking_wheels.clear();
	m_doors.clear();
	ph_world->RemoveObject(m_ident);
	CKinematics* pKinematics=PKinematics(Visual());
	CInifile* ini = pKinematics->LL_UserData();
	if(ini->line_exist("car_definition","steer"))
		pKinematics->LL_GetInstance(pKinematics->LL_BoneID(ini->r_string("car_definition","steer"))).set_callback(0,0);
}
#ifdef DEBUG
void	CCar::shedule_Update		(u32 dt)
{
	inherited::shedule_Update(dt);

	//if()
	

}
#endif

void	CCar::UpdateCL				( )
{
	inherited::UpdateCL();


#ifdef DEBUG
	if(m_pPhysicsShell&&m_owner)
	{
		Fvector v;
		m_pPhysicsShell->get_LinearVel(v);
		string32 s;
		sprintf(s,"speed, %f km/hour",v.magnitude()/1000.f*3600.f);
		HUD().pFontSmall->SetColor(D3DCOLOR_RGBA(0xff,0xff,0xff,0xff));
		HUD().pFontSmall->OutSet	(120,530);
		HUD().pFontSmall->OutNext(s);
		HUD().pFontSmall->OutNext("Transmission num:      [%d]",m_current_transmission_num);
		HUD().pFontSmall->OutNext("gear ratio:			  [%3.2f]",m_current_gear_ratio);
		HUD().pFontSmall->OutNext		("Power:      [%3.2f]",m_current_engine_power/(0.8f*1000.f));
		HUD().pFontSmall->OutNext		("rpm:      [%3.2f]",m_current_rpm/(1.f/60.f*2.f*M_PI));
		HUD().pFontSmall->OutNext		("wheel torque:      [%3.2f]",RefWheelCurTorque());
		HUD().pFontSmall->OutNext		("engine torque:      [%3.2f]",EngineCurTorque());
		//HUD().pFontSmall->OutNext("Vel Magnitude: [%3.2f]",ph_Movement.GetVelocityMagnitude());
		//HUD().pFontSmall->OutNext("Vel Actual:    [%3.2f]",ph_Movement.GetVelocityActual());
	}
#endif
	//	Log("UpdateCL",Device.dwFrame);
	//XFORM().set(m_pPhysicsShell->mXFORM);
	m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());
	// Camera
	if (m_owner&&IsMyCamera())				
		cam_Update	(Device.fTimeDelta);

	Fvector lin_vel;
	m_pPhysicsShell->get_LinearVel(lin_vel);
	// Sound
	Fvector		C,V;
	Center	(C);
	V.set		(lin_vel);
	
	UpdateSound();
	if(m_owner)
	{
		m_pPhysicsShell->InterpolateGlobalTransform(&m_owner->XFORM());
		m_owner->XFORM().mulB	(m_sits_transforms[0]);

		if(m_owner->IsMyCamera()) 
			cam_Update	(Device.fTimeDelta);
	}

	UpdateExhausts();
}

void	CCar::renderable_Render				( )
{
	inherited::renderable_Render			();
}

void	CCar::net_Export			(NET_Packet& P)
{
}

void	CCar::net_Import			(NET_Packet& P)
{
}

void	CCar::OnHUDDraw				(CCustomHUD* hud)
{
#ifdef DEBUG
	Fvector velocity;
	m_pPhysicsShell->get_LinearVel(velocity);
	HUD().pFontSmall->SetColor		(0xffffffff);
	HUD().pFontSmall->OutSet		(120,530);
	HUD().pFontSmall->OutNext		("Position:      [%3.2f, %3.2f, %3.2f]",VPUSH(Position()));
	HUD().pFontSmall->OutNext		("Velocity:      [%3.2f]",velocity.magnitude());

	
#endif
}

void CCar::Hit(float P,Fvector &dir,CObject *who,s16 element,Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	if(m_pPhysicsShell)		m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);
}

void CCar::detach_Actor()
{
	if(!m_owner) return;
	m_owner->setVisible(1);
	m_owner=NULL;
	NeutralDrive();
	///Break();
}

bool CCar::attach_Actor(CActor* actor)
{
	if(m_owner) return false;
	m_owner=actor;

	CKinematics* K	= PKinematics(Visual());
	CInifile* ini	= K->LL_UserData();
	int id;
	if(ini->line_exist("car_definition","driver_place"))
		id=K->LL_BoneID(ini->r_string("car_definition","driver_place"));
	else
	{	
		m_owner->setVisible(0);
		id=K->LL_BoneRoot();
	}
	CBoneInstance& instance=K->LL_GetInstance				(id);
	m_sits_transforms.push_back(instance.mTransform);

	//CBoneData&	bone_data=K->LL_GetData(id);
	//Fmatrix driver_pos_tranform;
	//driver_pos_tranform.setHPB(bone_data.bind_hpb.x,bone_data.bind_hpb.y,bone_data.bind_hpb.z);
	//driver_pos_tranform.c.set(bone_data.bind_translate);
	//m_sits_transforms.push_back(driver_pos_tranform);
	return true;
}


bool CCar::is_Door(int id,xr_map<int,SDoor>::iterator& i)
{
	i	= m_doors.find(id);
	if (i == m_doors.end()) 
	{
		return false;
	}
	else
	{
		if(i->second.joint)//temp for fake doors
			return true;
		else
			return false;
	}
}

bool CCar::Enter(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos)
{
	xr_map<int,SDoor>::iterator i,e;

	i=m_doors.begin();e=m_doors.end();
	Fvector enter_pos;
	enter_pos.add(pos,foot_pos);
	enter_pos.mul(0.5f);
	for(;i!=e;i++)
	{
		if(i->second.CanEnter(pos,dir,enter_pos)) return true;
	}
	return false;
}

bool CCar::Exit(const Fvector& pos,const Fvector& dir)
{
	xr_map<int,SDoor>::iterator i,e;

	i=m_doors.begin();e=m_doors.end();
	for(;i!=e;i++)
	{
		if(i->second.CanExit(pos,dir)) 
		{	
			i->second.GetExitPosition(m_exit_position);
			return true;
		}
	}
	return false;

}

void CCar::ParseDefinitions()
{
	bone_map.clear();

	CKinematics* pKinematics=PKinematics(Visual());
	bone_map.insert(mk_pair(pKinematics->LL_BoneRoot(),physicsBone()));
	CInifile* ini = pKinematics->LL_UserData();
	if(! ini) return;

	m_camera_position			= ini->r_fvector3("car_definition","camera_pos");
	///////////////////////////car definition///////////////////////////////////////////////////
	fill_wheel_vector			(ini->r_string	("car_definition","driving_wheels"),m_driving_wheels);
	fill_wheel_vector			(ini->r_string	("car_definition","steering_wheels"),m_steering_wheels);
	fill_wheel_vector			(ini->r_string	("car_definition","breaking_wheels"),m_breaking_wheels);
	fill_exhaust_vector			(ini->r_string	("car_definition","exhausts"),m_exhausts);
	fill_doors_map				(ini->r_string	("car_definition","doors"),m_doors);

	///////////////////////////car properties///////////////////////////////


	m_max_power			=		ini->r_float("car_definition","engine_power");
	m_max_power			*=		(0.8f*1000.f);

	m_max_rpm			=		ini->r_float("car_definition","max_engine_rpm");
	m_max_rpm			*=		(1.f/60.f*2.f*M_PI);


	m_min_rpm			=		ini->r_float("car_definition","idling_engine_rpm");
	m_min_rpm			*=		(1.f/60.f*2.f*M_PI);

	m_power_rpm			=		ini->r_float("car_definition","max_power_rpm");
	m_power_rpm			*=		(1.f/60.f*2.f*M_PI);//
	
	m_torque_rpm		=		ini->r_float("car_definition","max_torque_rpm");
	m_torque_rpm		*=		(1.f/60.f*2.f*M_PI);//

	b_auto_switch_transmission= !!ini->r_bool("car_definition","auto_transmission");
	m_auto_switch_rpm.set(ini->r_fvector2("car_definition","auto_transmission_rpm"));
	m_auto_switch_rpm.mul((1.f/60.f*2.f*M_PI));

	InitParabola		();

	m_axle_friction		=		ini->r_float("car_definition","axle_friction");
	m_steering_speed	=		ini->r_float("car_definition","steering_speed");
	m_break_torque		=		ini->r_float("car_definition","break_torque");
	m_hand_break_torque	=		ini->r_float("car_definition","break_torque");

	/////////////////////////transmission////////////////////////////////////////////////////////////////////////
	float main_gear_ratio=ini->r_float("car_definition","main_gear_ratio");
	
	R_ASSERT2(ini->section_exist("transmission_gear_ratio"),"no section transmission_gear_ratio");
	m_gear_ratious.push_back(-ini->r_float("transmission_gear_ratio","R")*main_gear_ratio);
	string32 rat_num;
	for(int i=1;true;i++)
	{
		sprintf(rat_num,"N%d",i);
		if(!ini->line_exist("transmission_gear_ratio",rat_num)) break;
		m_gear_ratious.push_back(ini->r_float("transmission_gear_ratio",rat_num)*main_gear_ratio);
	}
	///////////////////////////////steer/////////////////////////////////////////////////////////////////



}

void CCar::CreateSkeleton()
{

	if (!Visual()) return;
	CKinematics* K= PKinematics(Visual());
	K->PlayCycle("idle");
	K->Calculate();

	//CInifile* ini=K->LL_UserData();
	//K->LL_GetInstance				(K->LL_BoneID(ini->r_string("car_definition","steer"))).set_callback			(cb_Steer,this);
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(K,&bone_map);
	m_pPhysicsShell->set_PhysicsRefObject(this);
	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->Activate(true);
	m_pPhysicsShell->SetAirResistance(0.f,0.f);
}

void CCar::Init()
{
	//get reference wheel radius
	CKinematics* pKinematics=PKinematics(Visual());
	CInifile* ini = pKinematics->LL_UserData();
	SWheel& ref_wheel=m_wheels_map.find(pKinematics->LL_BoneID(ini->r_string("car_definition","reference_wheel")))->second;
	if(ini->line_exist("car_definition","steer"))
		pKinematics->LL_GetInstance(pKinematics->LL_BoneID(ini->r_string("car_definition","steer"))).set_callback(cb_Steer,this);
	ref_wheel.Init();
	m_ref_radius=ref_wheel.radius;

	b_engine_on=false;
	b_clutch   =false;
	b_starting =false;
	b_stalling =false;
	m_root_transform.set(bone_map.find(pKinematics->LL_BoneRoot())->second.element->mXFORM);
	m_current_transmission_num=0;
	m_pPhysicsShell->set_DynamicScales(1.f,1.f);

	{
		xr_map<int,SWheel>::iterator i,e;
		i=m_wheels_map.begin();
		e=m_wheels_map.end();
		for(;i!=e;i++)
			i->second.Init();
	}

	{
		xr_vector<SWheelDrive>::iterator i,e;
		i=m_driving_wheels.begin();
		e=m_driving_wheels.end();
		for(;i!=e;i++)
			i->Init();
	}

	{
		xr_vector<SWheelBreak>::iterator i,e;
		i=m_breaking_wheels.begin();
		e=m_breaking_wheels.end();
		for(;i!=e;i++)
			i->Init();
	}

	{
		xr_vector<SWheelSteer>::iterator i,e;
		i=m_steering_wheels.begin();
		e=m_steering_wheels.end();
		for(;i!=e;i++)
			i->Init();
	}

	{
		xr_vector<SExhaust>::iterator i,e;
		i=m_exhausts.begin();
		e=m_exhausts.end();
		for(;i!=e;i++)
			i->Init();
	}

{
	xr_map<int,SDoor>::iterator i,e;
	i=m_doors.begin();
	e=m_doors.end();
	for(;i!=e;i++)
		i->second.Init();
}
Break();
Transmision(1);
}

void CCar::Revert()
{
	//if(!bActive) return;
	//dBodyAddForce(Bodies[0], 0, 2*9000, 0);
	//dBodyAddRelTorque(Bodies[0], 300, 0, 0);

	m_pPhysicsShell->applyForce(0,40.f*m_pPhysicsShell->getMass(),0);
}

void CCar::NeutralDrive()
{

	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;i++)
		i->Neutral();
	e_state_drive=neutral;
}
void CCar::Unbreak()
{
	xr_vector<SWheelBreak>::iterator i,e;
	i=m_breaking_wheels.begin();
	e=m_breaking_wheels.end();
	for(;i!=e;i++)
		i->Neutral();
	if(e_state_drive==drive) 
		Drive();
}
void CCar::Drive()
{
	
	if(!(b_clutch&&b_engine_on)) return;
	m_pPhysicsShell->Enable();
	m_current_rpm=EngineDriveSpeed();
	m_current_engine_power=EnginePower();
	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;i++)
		i->Drive();
	e_state_drive=drive;

}

void CCar::StartEngine()
{
PlayExhausts();
snd_engine.play_at_pos			(this,Position(),TRUE);
b_engine_on=true;
}
void CCar::StopEngine()
{

StopExhausts();
NeutralDrive();//set zero speed
UpdatePower();//set engine friction;
b_engine_on=false;
}

void CCar::Stall()
{
	StopEngine();
	b_stalling=true;
	m_dwStallTime=Device.dwTimeGlobal;
}
void CCar::ReleasePedals()
{
	Clutch();
	NeutralDrive();//set zero speed
	UpdatePower();//set engine friction;
}

void CCar::SwitchEngine()
{
if(b_engine_on) StopEngine();
else			StartEngine();
}
void CCar::Clutch()
{
b_clutch=true;
}

void CCar::Unclutch()
{
b_clutch=false;
}

void CCar::Starter()
{
b_starting=true;
m_dwStartTime=Device.dwTimeGlobal;
}
void CCar::UpdatePower()
{
	m_current_rpm=EngineDriveSpeed();
	m_current_engine_power=EnginePower();

	if(b_auto_switch_transmission) 
	{
		if(m_current_rpm<m_auto_switch_rpm.x) TransmisionDown();
		if(m_current_rpm>m_auto_switch_rpm.y) TransmisionUp();
	}

	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;i++)
		i->UpdatePower();

	if(brp)
		Break();
}

void CCar::SteerRight()
{
	b_wheels_limited=true;  //no need to limit wheels when stiring
	m_pPhysicsShell->Enable();
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;i++)
		i->SteerRight();
	e_state_steer=right;

}
void CCar::SteerLeft()
{
	b_wheels_limited=true; //no need to limit wheels when stiring
	m_pPhysicsShell->Enable();
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;i++)
		i->SteerLeft();
	e_state_steer=left;
}

void CCar::SteerIdle()
{
	b_wheels_limited=false;
	m_pPhysicsShell->Enable();
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;i++)
		i->SteerIdle();
	e_state_steer=idle;
}

void CCar::LimitWheels()
{
	if(b_wheels_limited) return;
	b_wheels_limited=true;
	xr_vector<SWheelSteer>::iterator i,e;
	i=m_steering_wheels.begin();
	e=m_steering_wheels.end();
	for(;i!=e;i++)
		i->Limit();
}
void CCar::Break()
{
	xr_vector<SWheelBreak>::iterator i,e;
	i=m_breaking_wheels.begin();
	e=m_breaking_wheels.end();
	for(;i!=e;i++)
		i->Break();
}

void CCar::PressRight()
{
	if(lsp)
	{
		if(!fwp)SteerIdle();
	}
	else
		SteerRight();
	rsp=true;
}
void CCar::PressLeft()
{
	if(rsp)
	{
		if(!fwp)SteerIdle();
	}
	else
		SteerLeft();
	lsp=true;
}
void CCar::PressForward()
{
	if(bkp) 
	{	
		Unclutch();
		NeutralDrive();
	}
	else 
	{
		Clutch();
		if(m_current_transmission_num==0) Transmision(1);
		if(m_current_transmission_num==1||m_current_transmission_num==0)Starter();
		Drive();
	}
	fwp=true;
}
void CCar::PressBack()
{
	if(fwp) 
	{
		Unclutch();
		NeutralDrive();
	}
	else 
	{
		Clutch();
		Transmision(0);
		if(m_current_transmission_num==1||m_current_transmission_num==0)Starter();
		Drive();
	}
	bkp=true;
}
void CCar::PressBreaks()
{
	Break();
	brp=true;
}

void CCar::ReleaseRight()
{
	if(lsp)
		SteerLeft();
	else
		SteerIdle();
	rsp=false;
}
void CCar::ReleaseLeft()
{
	if(rsp)
		SteerRight();
	else
		SteerIdle();
	lsp=false;
}
void CCar::ReleaseForward()
{
	if(bkp)
	{
		Clutch();
		Transmision(0);
		if(m_current_transmission_num==1||m_current_transmission_num==0)Starter();
		Drive();
	}
	else
	{
		Unclutch();
		NeutralDrive();
	}

	fwp=false;
}
void CCar::ReleaseBack()
{
	if(fwp)
	{
		Clutch();
		if(m_current_transmission_num==0) Transmision(1);
		if(m_current_transmission_num==1||m_current_transmission_num==0) Starter();
		Drive();
	}
	else
	{
		Unclutch();
		NeutralDrive();
	}
	bkp=false;
}
void CCar::ReleaseBreaks()
{
	Unbreak();
	brp=false;
}

void CCar::Transmision(size_t num)
{

	if(num==0||num==1)Starter();
	if(num<m_gear_ratious.size())
	{
		m_current_transmission_num=num;
		m_current_gear_ratio=m_gear_ratious[num];
		m_current_rpm=m_torque_rpm;
	}
}
void CCar::CircleSwitchTransmission()
{
	if(m_current_transmission_num==0)return;
	m_current_transmission_num++;
	m_current_transmission_num=m_current_transmission_num%m_gear_ratious.size();
	m_current_transmission_num==0 ? m_current_transmission_num++ : m_current_transmission_num;
	Transmision(m_current_transmission_num);
	Drive();
}

void CCar::TransmisionUp()
{
	if(m_current_transmission_num==0)return;
	m_current_transmission_num++;
	size_t max_transmition_num=m_gear_ratious.size()-1;
	m_current_transmission_num>max_transmition_num ? m_current_transmission_num=max_transmition_num :m_current_transmission_num;
	Transmision(m_current_transmission_num);
	Drive();
}

void CCar::TransmisionDown()
{
	if(m_current_transmission_num==0)return;
	m_current_transmission_num--;
	m_current_transmission_num<1 ? m_current_transmission_num=1 : m_current_transmission_num;
	Transmision(m_current_transmission_num);
	Drive();
}
void CCar::InitParabola()
{
	//float t1=(m_power_rpm-m_torque_rpm);
	//float t2=m_max_power/m_power_rpm;
	//m_c = t2* (3.f*m_power_rpm - 4.f*m_torque_rpm)/t1/2.f;
	//t2/=m_power_rpm;
	//m_a = -t2/t1/2.f;
	//m_b = t2*m_torque_rpm/t1;


	//m_c = m_max_power* (3.f*m_power_rpm - 4.f*m_torque_rpm)/(m_power_rpm-m_torque_rpm)/2.f/m_power_rpm;
	//m_a = -m_max_power/(m_power_rpm-m_torque_rpm)/m_power_rpm/m_power_rpm/2.f;
	//m_b = m_max_power*m_torque_rpm/(m_power_rpm-m_torque_rpm)/m_power_rpm/m_power_rpm;




	m_a=expf((m_power_rpm - m_torque_rpm)/(2.f*m_power_rpm))*m_max_power/m_power_rpm;
	m_b=m_torque_rpm;
	m_c=-(1.41421356237309504880f*_sqrt(m_power_rpm*(m_power_rpm - m_torque_rpm)));


}
void CCar::PhTune(dReal step)
{
	if(m_repairing)Revert();
	LimitWheels();

	if(fwp||bkp)
	{	
		UpdatePower();
		if(b_engine_on&&!b_starting && m_current_rpm<m_min_rpm)Stall();
	}

	for (int k=0; k<(int)m_doors_update.size(); k++){
		SDoor* D = m_doors_update[k];
		if (!D->update)
		{
			m_doors_update.erase(m_doors_update.begin()+k);
			k--;
		}
		else
		{
			D->Update();
		}
	}
}


void CCar::PlayExhausts()
{
	if(b_engine_on) return;
	xr_vector<SExhaust>::iterator i,e;
	i=m_exhausts.begin();
	e=m_exhausts.end();
	for(;i!=e;i++)
		i->Play();

}

void CCar::StopExhausts()
{
	if(!b_engine_on) return;
	xr_vector<SExhaust>::iterator i,e;
	i=m_exhausts.begin();
	e=m_exhausts.end();
	for(;i!=e;i++)
		i->Stop();
}

void CCar::UpdateExhausts()
{
	if(!b_engine_on) return;
	xr_vector<SExhaust>::iterator i,e;
	i=m_exhausts.begin();
	e=m_exhausts.end();
	for(;i!=e;i++)
		i->Update();
}

void CCar::ClearExhausts()
{
	xr_vector<SExhaust>::iterator i,e;
	i=m_exhausts.begin();
	e=m_exhausts.end();
	for(;i!=e;i++)
		i->Clear();
}

bool CCar::Use(int id,const Fvector& pos,const Fvector& dir,const Fvector& foot_pos)
{
	xr_map<int,SDoor>::iterator i;
 //

	if(!m_owner)
	{
		if(Enter(pos,dir,foot_pos)) return true;
	}



	
	ICollisionForm::RayPickResult result;
	if (collidable.model->_RayPick	(result,pos, dir, 3.f, 0)) // CDB::OPT_ONLYFIRST CDB::OPT_ONLYNEAREST
	{
		int y=result.r_count();
		for (int k=0; k<y; k++)
		{
			ICollisionForm::RayPickResult::Result* R = result.r_begin()+k;
			if(is_Door(R->element,i)) 
			{
				i->second.Use();
				return false;

			}
		}
	}
	
if(m_owner)return Exit(pos,dir);

return false;
	
}

float CCar::Parabola(float rpm)
{
	//float rpm_2=rpm*rpm;
	//float value=(m_a*rpm_2*rpm_2*rpm_2+m_b*rpm_2+m_c)*rpm_2;
	float ex=(rpm-m_b)/m_c;
	float value=m_a*expf(-ex*ex)*rpm;
	if(value<0.f) return 0.f;
	return value;
}

float CCar::EnginePower()
{

	if(m_current_rpm<m_min_rpm)
	{
		if(b_starting) return Parabola(m_min_rpm);
	
	}
	else
	{
		if(b_starting&&Device.dwTimeGlobal-m_dwStartTime>1000) b_starting=false;
	}

	return Parabola(m_current_rpm);



}

float CCar::EngineDriveSpeed()
{
	float wheel_speed,drive_speed=dInfinity;
	xr_vector<SWheelDrive>::iterator i,e;
	i=m_driving_wheels.begin();
	e=m_driving_wheels.end();
	for(;i!=e;i++)
	{
		wheel_speed=i->ASpeed();
		if(wheel_speed<drive_speed)drive_speed=wheel_speed;
	}
	if(drive_speed<dInfinity) return dFabs(drive_speed*m_current_gear_ratio);
	else					  return 0.f;
}

void CCar::UpdateSound()
{
	//float		velocity						= V.magnitude();
	float		scale							= 0.5f+m_current_rpm/m_torque_rpm/2.f; clamp(scale,0.5f,1.f);

	snd_engine.set_position			(Position());
	snd_engine.set_frequency		(scale);
	snd_engine.set_volume			(car_snd_volume);
	StallSound();
}

void CCar::StallSound()
{
	
	if(!b_stalling||b_engine_on) return;
	u32 time_passed=Device.dwTimeGlobal-m_dwStallTime;
	if(time_passed>2500) 
	{
		snd_engine.stop();
		b_stalling=false;
		return;
	}
	snd_engine.set_volume(car_snd_volume*1000.f/time_passed);
	
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCar::SExhaust::~SExhaust()
{
	if(p_pgobject)xr_delete(p_pgobject);
}

void CCar::SExhaust::Init()
{
	pelement=(bone_map.find(bone_id))->second.element;
	CKinematics* K=PKinematics(pcar->Visual());
	CBoneData&	bone_data=K->LL_GetData(bone_id);
	transform.setXYZi(bone_data.bind_xyz);
	transform.c.set(bone_data.bind_translate);
	transform.mulA(pcar->XFORM());
	Fmatrix element_transform;
	pelement->InterpolateGlobalTransform(&element_transform);
	element_transform.invert();
	transform.mulA(element_transform);
	p_pgobject=xr_new<CParticlesObject>("vehiclefx\\exhaust_1",pcar->Sector(),false);
	p_pgobject->SetTransform(pcar->XFORM());
}

void CCar::SExhaust::Update()
{
	Fmatrix global_transform;
	pelement->InterpolateGlobalTransform(&global_transform);
	global_transform.mulB(transform);
	dVector3 res;
	Fvector	 res_vel;
	dBodyGetRelPointVel(pelement->get_body(),transform.c.x,transform.c.y,transform.c.z,res);
	Memory.mem_copy (&res_vel,res,sizeof(Fvector));
	p_pgobject->UpdateParent(global_transform,res_vel);
}

void CCar::SExhaust::Clear()
{
	xr_delete(p_pgobject);
}

void CCar::SExhaust::Play()
{
	p_pgobject->Play();
}

void CCar::SExhaust::Stop()
{
	p_pgobject->Stop();
}

#undef   _USE_MATH_DEFINES