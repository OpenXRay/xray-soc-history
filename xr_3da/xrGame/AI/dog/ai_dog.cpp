#include "stdafx.h"
#include "ai_dog.h"


CAI_Dog::CAI_Dog()
{
	stateRest			= xr_new<CBitingRest>		(this);
	stateAttack			= xr_new<CBitingAttack>		(this, false);
	stateEat			= xr_new<CBitingEat>		(this, true);
	stateHide			= xr_new<CBitingHide>		(this);
	stateDetour			= xr_new<CBitingDetour>		(this);
	statePanic			= xr_new<CBitingPanic>		(this, false);
	stateExploreNDE		= xr_new<CBitingExploreNDE>	(this);
	
	CurrentState		= stateRest;

	Init();
}

CAI_Dog::~CAI_Dog()
{
	xr_delete(stateRest);
	xr_delete(stateAttack);
	xr_delete(stateEat);
	xr_delete(stateHide);
	xr_delete(stateDetour);
	xr_delete(statePanic);
	xr_delete(stateExploreNDE);
}


void CAI_Dog::Init()
{
	inherited::Init();

	CurrentState					= stateRest;
	CurrentState->Reset				();

	Bones.Reset();
}


void CAI_Dog::StateSelector()
{	
	VisionElem ve;

	if (C && H && I)			SetState(statePanic);
	else if (C && H && !I)		SetState(statePanic);
	else if (C && !H && I)		SetState(statePanic);
	else if (C && !H && !I) 	SetState(statePanic);
	else if (D && H && I)		SetState(stateAttack);
	else if (D && H && !I)		SetState(stateAttack);  //���� ����������� � ������ ������
	else if (D && !H && I)		SetState(statePanic);
	else if (D && !H && !I) 	SetState(stateHide);	// ����� ����������� ����� �������
	else if (E && H && I)		SetState(stateAttack); 
	else if (E && H && !I)  	SetState(stateAttack);  //���� ����������� � ������ ������
	else if (E && !H && I) 		SetState(stateDetour); 
	else if (E && !H && !I)		SetState(stateDetour); 
	else if (F && H && I) 		SetState(stateAttack); 		
	else if (F && H && !I)  	SetState(stateAttack); 
	else if (F && !H && I)  	SetState(stateDetour); 
	else if (F && !H && !I) 	SetState(stateHide);
	else if (A && !K && !H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);  // ����� ������� ����, �� �� ����, ���� �� ��������		(ExploreDNE)
	else if (A && !K && H)		SetState(stateExploreNDE);  //SetState(stateExploreDNE);	//SetState(stateExploreDE);	// ����� ������� ����, �� �� ����, ���� ��������			(ExploreDE)		
	else if (B && !K && !H)		SetState(stateExploreNDE);	// ����� �� ������� ����, �� �� ����, ���� �� ��������	(ExploreNDNE)
	else if (B && !K && H)		SetState(stateExploreNDE);	// ����� �� ������� ����, �� �� ����, ���� ��������		(ExploreNDE)
	else if (GetCorpse(ve) && (ve.obj->m_fFood > 1) && ((GetSatiety() < 0.85f) || flagEatNow))	
		SetState(stateEat);
	else						SetState(stateRest); 
}


void __stdcall CAI_Dog::BoneCallback(CBoneInstance *B)
{
	CAI_Dog*	this_class = dynamic_cast<CAI_Dog*> (static_cast<CObject*>(B->Callback_Param));
	this_class->Bones.Update(B, Level().timeServer());
}

BOOL CAI_Dog::net_Spawn (LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return(FALSE);

	// define animation set
	MotionMan.AddAnim(eAnimStandIdle,		"stand_idle_",			-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimLieIdle,			"lie_idle_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimSleep,			"lie_idle_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, m_fsWalkFwdNormal,		m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimRun,				"stand_run_",			-1,	m_fsRunFwdNormal,		m_fsRunAngular,				PS_STAND);
	//...
	MotionMan.AddAnim(eAnimCheckCorpse,		"stand_idle_",			-1,	0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimEat,				"stand_eat_",			-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimStandLieDown,	"stand_lie_down_",		-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimLieStandUp,		"lie_stand_up_",		-1, 0,						0,							PS_LIE);
	//...
	//MotionMan.AddAnim(eAnimSitStandUp,		"sit_stand_up_",		-1, 0,						0,							PS_SIT);
	//MotionMan.AddAnim(eAnimSitLieDown,		"sit_lie_down_",		-1, 0,						0,							PS_SIT);
	MotionMan.AddAnim(eAnimLieSitUp,		"lie_sit_up_",			-1, 0,						0,							PS_LIE);
	MotionMan.AddAnim(eAnimStandSitDown,	"stand_sit_down_",		-1, 0,						0,							PS_STAND);	
	MotionMan.AddAnim(eAnimAttack,			"stand_attack_",		 1, 0,						m_fsRunAngular,				PS_STAND);
	MotionMan.AddAnim(eAnimStandDamaged,	"stand_damaged_",		-1, 0,						0,							PS_STAND);
	MotionMan.AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, m_fsDrag,				m_fsWalkAngular,			PS_STAND);
	//...
	MotionMan.AddAnim(eAnimSteal,			"stand_drag_",			-1, m_fsSteal,				m_fsWalkAngular,			PS_STAND);

	MotionMan.AddAnim(eAnimJumpStart,		"jump1_",				 0, 0,						m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimJumpFly,			"jump1_",				 1, 0,						m_fsWalkAngular,			PS_STAND);
	MotionMan.AddAnim(eAnimJumpFinish,		"jump1_",				 2, 0,						m_fsWalkAngular,			PS_STAND);


	// define transitions
	// order : 1. [anim -> anim]	2. [anim->state]	3. [state -> anim]		4. [state -> state]
	MotionMan.AddTransition_S2S(PS_STAND,	PS_LIE,		eAnimStandLieDown,		false);
	MotionMan.AddTransition_S2S(PS_LIE,		PS_STAND,	eAnimLieStandUp,		false);
	//MotionMan.AddTransition_S2S(PS_SIT,		PS_STAND,	eAnimSitStandUp,		false);
	//MotionMan.AddTransition_S2S(PS_SIT,		PS_LIE,		eAnimSitLieDown,		false);
	MotionMan.AddTransition_S2S(PS_LIE,		PS_SIT,		eAnimLieSitUp,			false);
	MotionMan.AddTransition_S2S(PS_STAND,	PS_SIT,		eAnimStandSitDown,		false);

	// define links from Action to animations
	MotionMan.LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	MotionMan.LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	MotionMan.LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_WALK_BKWD,		eAnimWalkFwd);
	MotionMan.LinkAction(ACT_RUN,			eAnimRun);
	MotionMan.LinkAction(ACT_EAT,			eAnimEat);
	MotionMan.LinkAction(ACT_SLEEP,			eAnimSleep);
	MotionMan.LinkAction(ACT_REST,			eAnimLieIdle);
	MotionMan.LinkAction(ACT_DRAG,			eAnimDragCorpse);
	MotionMan.LinkAction(ACT_ATTACK,		eAnimAttack);
	MotionMan.LinkAction(ACT_STEAL,			eAnimSteal);
	MotionMan.LinkAction(ACT_LOOK_AROUND,	eAnimStandIdle);

	Fvector center;
	center.set		(0.f,0.f,0.f);

	MotionMan.AA_PushAttackAnim(eAnimAttack, 0, 700,	800,	center,		2.0f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 1, 600,	800,	center,		2.5f, m_fHitPower, 0.f, 0.f);
	MotionMan.AA_PushAttackAnim(eAnimAttack, 2, 600,	700,	center,		1.5f, m_fHitPower, 0.f, 0.f);

	int bone1		= PKinematics(Visual())->LL_BoneID("bone01");
	PKinematics(Visual())->LL_GetInstance(u16(bone1)).set_callback(BoneCallback,this);

	// Bones settings
	Bones.Reset();
	Bones.AddBone(GetBone(bone1), AXIS_Y); 

	MotionMan.AddJump(eAnimJumpStart, eAnimJumpFly, eAnimJumpFinish, 10.f);

	return TRUE;
}

void CAI_Dog::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_CHECK_CORPSE) == ASP_CHECK_CORPSE) {
		MotionMan.Seq_Add(eAnimCheckCorpse);
		MotionMan.Seq_Switch();
	}
}

void CAI_Dog::OnSoundPlay()
{
	if (!Bones.IsActive()) Bones.SetMotion(GetBone("bone01"),AXIS_Y, PI_DIV_6, PI_MUL_2, 1);
}
