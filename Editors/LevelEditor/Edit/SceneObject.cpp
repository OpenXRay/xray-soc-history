//----------------------------------------------------
// file: CEditObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SceneObject.h"
#include "UI_Main.h"          
#include "bottombar.h"
#include "scene.h"
#include "d3dutils.h"
#include "library.h"
#include "EditMesh.h"
#include "motion.h"

#define BLINK_TIME 300.f

//----------------------------------------------------
CSceneObject::CSceneObject( char *name ):CCustomObject(){
	Construct	();
	Name		= name;
}

CSceneObject::CSceneObject( ):CCustomObject(){
	Construct	();
}

void CSceneObject::Construct(){
	ClassID		= OBJCLASS_SCENEOBJECT;

	m_pRefs 	= 0;
    m_ObjVer.reset();

    m_Center.set(0,0,0);
    m_fRadius = 0;
    m_iBlinkTime = 0;

    m_ActiveOMotion = 0;
}

CSceneObject::~CSceneObject(){
	Lib.RemoveEditObject(m_pRefs);
    // object motions
    for(OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end();o_it++)_DELETE(*o_it);
    m_OMotions.clear();
    m_ActiveOMotion = 0;
}

//----------------------------------------------------
void CSceneObject::Select(BOOL flag)
{
	inherited::Select(flag);
    if (flag) m_iBlinkTime=Device.dwTimeGlobal+BLINK_TIME+Device.dwTimeDelta;
}

//----------------------------------------------------
void CSceneObject::GetFaceWorld(CEditableMesh* M, int idx, Fvector* verts){
	const Fvector* PT[3];
	M->GetFacePT(idx, PT);
	_Transform().transform_tiny(verts[0],*PT[0]);
    _Transform().transform_tiny(verts[1],*PT[1]);
	_Transform().transform_tiny(verts[2],*PT[2]);
}

int CSceneObject::GetFaceCount(){
	return m_pRefs?m_pRefs->GetFaceCount():0;
}

int CSceneObject::GetSurfFaceCount(const char* surf_name){
	return m_pRefs?m_pRefs->GetSurfFaceCount(surf_name):0;
}

int CSceneObject::GetVertexCount(){
	return m_pRefs?m_pRefs->GetVertexCount():0;
}

void CSceneObject::OnUpdateTransform(){
	inherited::OnUpdateTransform();
    // update bounding volume
    Fbox BB; GetBox			(BB);
    BB.getsphere			(m_Center,m_fRadius);
}

bool CSceneObject::GetBox( Fbox& box ){
	if (!m_pRefs) return false;
    box.xform(m_pRefs->GetBox(),_Transform());
	return true;
}

bool CSceneObject::GetUTBox( Fbox& box ){
	if (!m_pRefs) return false;
    box.set(m_pRefs->GetBox());
	return true;
}

bool __inline CSceneObject::IsRender(){
	if (!m_pRefs) return false;
    bool bRes = Device.m_Frustum.testSphere(m_Center,m_fRadius);
    if(bRes&&fraBottomBar->miDrawObjectAnimPath->Checked) RenderAnimation();
    return bRes;
}

void CSceneObject::Render(int priority, bool strictB2F){
	inherited::Render(priority,strictB2F);
    if (!m_pRefs) return;
    Scene.TurnLightsForObject(this);
	m_pRefs->Render(_Transform(), priority, strictB2F);
    if ((1==priority)&&(false==strictB2F)){
        if (Selected()){
            if (m_iBlinkTime>(int)Device.dwTimeGlobal){
            	DWORD c=D3DCOLOR_ARGB(iFloor(sqrtf(float(m_iBlinkTime-Device.dwTimeGlobal)/BLINK_TIME)*48),255,255,255);
                RenderSelection(c);
                UI.RedrawScene();
            }
            Device.SetShader(Device.m_WireShader);
            Device.SetTransform(D3DTS_WORLD,_Transform());
            DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
            DU::DrawSelectionBox(m_pRefs->GetBox(),&clr);
        }
    }
}

void CSceneObject::RenderSingle(){
	if (!m_pRefs) return;
	m_pRefs->RenderSingle(_Transform());
}

void CSceneObject::RenderAnimation(){
	if (!m_pRefs) return;
	m_pRefs->RenderAnimation(_Transform());
}

void CSceneObject::RenderBones(){
	if (!m_pRefs) return;
	m_pRefs->RenderBones(_Transform());
}

void CSceneObject::RenderEdge(CEditableMesh* mesh, DWORD color){
	if (!m_pRefs) return;
    if (Device.m_Frustum.testSphere(m_Center,m_fRadius))
		m_pRefs->RenderEdge(_Transform(), mesh, color);
}

void CSceneObject::RenderSelection(DWORD color){
	if (!m_pRefs) return;
	m_pRefs->RenderSelection(_Transform(),0,color);
}

bool CSceneObject::FrustumPick(const CFrustum& frustum){
	if (!m_pRefs) return false;
    if(Device.m_Frustum.testSphere(m_Center,m_fRadius))
		return m_pRefs->FrustumPick(frustum, _Transform());
    return false;
}

bool CSceneObject::SpherePick(const Fvector& center, float radius){
	if (!m_pRefs) return false;
	float R=radius+m_fRadius;
    float dist_sqr=center.distance_to_sqr(m_Center);
    if (dist_sqr<R*R) return true;
    return false;
}

bool CSceneObject::RayPick(float& dist, Fvector& S, Fvector& D, SRayPickInfo* pinf){
	if (!m_pRefs) return false;
    if (Device.m_Frustum.testSphere(m_Center,m_fRadius))
		if (m_pRefs&&m_pRefs->RayPick(dist, S, D, _ITransform(), pinf)){
        	if (pinf) pinf->s_obj = this;
            return true;
        }
	return false;
}

bool CSceneObject::BoxPick(const Fbox& box, SBoxPickInfoVec& pinf){
	if (!m_pRefs) return false;
	return m_pRefs->BoxPick(this, box, _ITransform(), pinf);
}

void CSceneObject::PivotScale(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount ){
    if (IsDynamic()){
    	ELog.Msg(mtError,"Dynamic object %s - can't scale.", Name);
        return;
    }
	inherited::PivotScale(prev_inv,current,amount);
}

void CSceneObject::Scale( Fvector& amount ){
    if (IsDynamic()){
    	ELog.Msg(mtError,"Dynamic object %s - can't scale.", Name);
        return;
    }
	inherited::Scale(amount);
}

void CSceneObject::GetFullTransformToWorld( Fmatrix& m ){
    m.set( _Transform() );
}

void CSceneObject::GetFullTransformToLocal( Fmatrix& m ){
    m.invert(_Transform());
}


CEditableObject* CSceneObject::SetReference(LPCSTR ref_name)
{
	Lib.RemoveEditObject(m_pRefs);
	m_pRefs	= (ref_name&&ref_name[0])?Lib.CreateEditObject(ref_name):0;
    return m_pRefs;
}

void CSceneObject::OnFrame(){
	inherited::OnFrame();
	if (!m_pRefs) return;
	if (m_pRefs) m_pRefs->OnFrame();
/*
	if (IsOMotionActive()){
    	float fps = m_ActiveOMotion->FPS();
	    float min_t=(float)m_ActiveOMotion->FrameStart()/fps;
    	float max_t=(float)m_ActiveOMotion->FrameEnd()/fps;

        Fvector T,r;
        FvectorVec v;
        DWORD clr=0xffffffff;
        for (float t=min_t; t<max_t; t+=0.1f){
	        m_ActiveOMotion->Evaluate(t,T,r);
            v.push_back(T);
        }

        Device.SetShader		(Device.m_WireShader);
        Device.SetTransform		(D3DTS_WORLD,parent);
        DU::DrawPrimitiveL		(D3DPT_LINESTRIP,v.size()-1,v.begin(),v.size(),clr,true,false);
    }
    if (IsOMotionActive()){
///    	ELog.DlgMsg(mtError,"TODO: CEditableObject::RTL_Update");
        Fvector R,T,r;
        m_ActiveOMotion->Evaluate(m_OMParam.Frame(),T,r);
        R.set(r.y,r.x,r.z);
        T.add(vPosition);
        R.add(vRotate);
        UpdateTransform(T, R, vScale);
        m_OMParam.Update(dT);
    }
*/
}
//S	SetActiveOMotion(0,false);

//----------------------------------------------------
// Object motion
//----------------------------------------------------
void CSceneObject::SetActiveOMotion(COMotion* mot, bool upd_t){
	m_ActiveOMotion=mot;
    if (m_ActiveOMotion) m_OMParam.Set(m_ActiveOMotion,true);
    UI.RedrawScene();
}

COMotion* CSceneObject::FindOMotionByName	(const char* name, const COMotion* Ignore){
    OMotionVec& lst = m_OMotions;
    for(OMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((Ignore!=(*m))&&(stricmp((*m)->Name(),name)==0)) return (*m);
    return 0;
}

void CSceneObject::RemoveOMotion(const char* name){
    OMotionVec& lst = m_OMotions;
    for(OMotionIt m=lst.begin(); m!=lst.end(); m++)
        if ((stricmp((*m)->Name(),name)==0)){
        	if (m_ActiveOMotion==*m) SetActiveOMotion(0);
            _DELETE(*m);
        	lst.erase(m);
            break;
        }
}

bool CSceneObject::RenameOMotion(const char* old_name, const char* new_name){
	if (stricmp(old_name,new_name)==0) return true;
    if (FindOMotionByName(new_name)) return false;
	COMotion* M = FindOMotionByName(old_name); VERIFY(M);
    M->SetName(new_name);
    return true;
}

void CSceneObject::GenerateOMotionName(char* buffer, const char* start_name, const COMotion* M){
	strcpy(buffer,start_name);
    int idx = 0;
	while(FindOMotionByName(buffer,M)){
		sprintf( buffer, "%s_%d", start_name, idx );
    	idx++;
    }
    strlwr(buffer);
}

COMotion* CSceneObject::AppendOMotion(const char* fname){
	COMotion* M = LoadOMotion(fname);
    if (M) m_OMotions.push_back(M);
    return M;
}

void CSceneObject::ClearOMotions(){
	SetActiveOMotion(0);
    for(OMotionIt m_it=m_OMotions.begin(); m_it!=m_OMotions.end();m_it++)_DELETE(*m_it);
    m_OMotions.clear();
}

void CSceneObject::LoadOMotions(const char* fname){
	CFileStream F(fname);
    ClearOMotions();
    // object motions
    m_OMotions.resize(F.Rdword());
	SetActiveOMotion(0);
    for (OMotionIt m_it=m_OMotions.begin(); m_it!=m_OMotions.end(); m_it++){
        *m_it = new COMotion();
        (*m_it)->Load(F);
    }
}

void CSceneObject::SaveOMotions(const char* fname){
	CFS_Memory F;
    F.Wdword		(m_OMotions.size());
    for (OMotionIt m_it=m_OMotions.begin(); m_it!=m_OMotions.end(); m_it++) (*m_it)->Save(F);
    F.SaveTo		(fname,0);
}


