//----------------------------------------------------
// file: BuilderLTX.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"
#include "Scene.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "SceneObject.h"
#include "Sound.h"
#include "ELight.h"
#include "RPoint.h"
#include "Event.h"
#include "AITraffic.h"
#include "xr_ini.h"
//----------------------------------------------------

//----------------------------------------------------

void AppendDataToSection(CInifile* pIni, AnsiString& section, char* value){
    TStringList* ss = new TStringList();
    ss->SetText(value);
    AnsiString k,v,t;
    for (int j=0; j<ss->Count; j++){
        t = ss->Strings[j]; t.Trim();
        if (!t.Length()) continue;
        int delim = t.Pos("=");
        k = t.SubString(1,delim-1); k.Trim();
        v = t.SubString(delim+1,t.Length()-delim);
        if (!k.Length()&&!v.Length()){
            AnsiString temp;
            temp.sprintf("Error appended script data in section [%s]. Append skipped.",section);
            MessageDlg(temp, mtError, TMsgDlgButtons() << mbOK, 0);
            break;
        }
        pIni->WriteString(section.c_str(),k.c_str(),v.c_str());
    }
    delete ss;
}

bool SceneBuilder::BuildLTX(){
	bool bResult=true;
	int objcount = Scene.ObjCount();
	if( objcount <= 0 ) return true;

	char ltx_filename[MAX_PATH];
	strcpy( ltx_filename, "level.ltx" );
	m_LevelPath.Update( ltx_filename );

    if (FS.Exist(ltx_filename, false))
    	FS.MarkFile(ltx_filename);

	// -- defaults --
    CFS_Memory F;
	if( !Scene.m_LevelOp.m_BOPText.IsEmpty() )
		F.WstringZ( Scene.m_LevelOp.m_BOPText.c_str() );
	F.SaveTo(ltx_filename,0);

    CInifile* pIni = new CInifile(ltx_filename,false);

	Scene.WriteToLTX(pIni);

    // required strings
    pIni->WriteString("static_sounds",		"; sounds","");
    pIni->WriteString("sound_environment",	"; sound environment","");
    pIni->WriteString("mobileobjects",		"; mobile objects","");

	// -- mobile objects --
    ObjectIt i = Scene.FirstObj(OBJCLASS_SCENEOBJECT);
    ObjectIt _E = Scene.LastObj(OBJCLASS_SCENEOBJECT);
    for(;i!=_E;i++){
        CSceneObject *obj = (CSceneObject*)(*i);
        if( obj->IsDynamic() ){
    		pIni->WriteString("mobileobjects", obj->GetName(), obj->GetName() );
            if( !obj->GetReference()->GetClassScript().IsEmpty() ) AppendDataToSection(pIni, AnsiString(obj->GetName()), obj->GetReference()->GetClassScript().c_str());
            pIni->WriteVector(obj->GetName(),"position",obj->GetPosition());
			Fmatrix mRotate; mRotate.setHPB(obj->GetRotate().y, obj->GetRotate().x, obj->GetRotate().z);
            pIni->WriteVector(obj->GetName(),"direction",mRotate.k);
            pIni->WriteVector(obj->GetName(),"normal",mRotate.j);
//            temp.sprintf("%.3f,%.3f,%.3f", obj->GetScale().x, obj->GetScale().y, obj->GetScale().z);
//            pIni->WriteString(obj->GetName(),"scale",temp);
        }
	}

	// -- add event --
	i   = Scene.FirstObj(OBJCLASS_EVENT);
    _E  = Scene.LastObj(OBJCLASS_EVENT);
    for(;i!=_E;i++){
        CEvent *obj = (CEvent*)(*i);
        if( obj->eEventType==CEvent::eetBox ){
    		pIni->WriteString ("mobileobjects", obj->GetName(), obj->GetName() );
            pIni->WriteString(obj->GetName(),"class","EVENT");
            pIni->WriteString(obj->GetName(),"target_class",obj->sTargetClass.c_str());
            pIni->WriteString(obj->GetName(),"OnEnter",obj->sOnEnter.c_str());
            pIni->WriteString(obj->GetName(),"OnExit",obj->sOnExit.c_str());
            pIni->WriteVector(obj->GetName(),"position",obj->GetPosition());
			Fmatrix mRotate; mRotate.setHPB(obj->GetRotate().y, obj->GetRotate().x, obj->GetRotate().z);
            pIni->WriteVector(obj->GetName(),"direction",mRotate.k);
            pIni->WriteVector(obj->GetName(),"normal",mRotate.j);
            pIni->WriteVector(obj->GetName(),"scale",obj->GetScale());
            pIni->WriteString(obj->GetName(),"execute_once",obj->bExecuteOnce?"on":"off");
        }
	}

	// -- add static sounds --
	if (Scene.ObjCount(OBJCLASS_SOUND)) {
		i = Scene.FirstObj(OBJCLASS_SOUND);
        _E  = Scene.LastObj(OBJCLASS_SOUND);
        for(;i!=_E;i++){
            CSound *s = (CSound *)(*i);
            if (strlen(s->m_fName)==0){
            	ELog.DlgMsg(mtError,"*ERROR: Sound '%s'. Not assigned wave file!", s->GetName());
                bResult=false;
                goto end_ltx_build;
            }
            AnsiString ln; ln.sprintf("%s,%f,%f,%f",s->m_fName,s->m_Position.x,s->m_Position.y,s->m_Position.z);
            pIni->WriteString("static_sounds",s->GetName(),ln.c_str());
		}
	}

	// -- add flares --
	if (Scene.ObjCount(OBJCLASS_LIGHT)) {
        AnsiString suns;
		i = Scene.FirstObj(OBJCLASS_LIGHT);
        _E  = Scene.LastObj(OBJCLASS_LIGHT);
        for(;i!=_E;i++){
            CLight *l = (CLight *)(*i);
            if (l->m_D3D.type==D3DLIGHT_DIRECTIONAL){
	            if (suns.Length()) suns += ", ";
    	        suns += l->GetName();
           	    pIni->WriteColor	(l->GetName(), "sun_color", 		l->m_D3D.diffuse.get());
               	pIni->WriteVector	(l->GetName(), "sun_dir", 			l->m_D3D.direction);
               	pIni->WriteString	(l->GetName(), "gradient", 			l->m_LensFlare.m_Flags.bGradient?"on":"off");
               	pIni->WriteString	(l->GetName(), "source", 			l->m_LensFlare.m_Flags.bSource?"on":"off");
               	pIni->WriteString	(l->GetName(), "flares", 			l->m_LensFlare.m_Flags.bFlare?"on":"off");
                pIni->WriteFloat	(l->GetName(), "gradient_density", 	l->m_LensFlare.m_fGradientDensity);
                pIni->WriteString	(l->GetName(), "source_texture", 	l->m_LensFlare.m_Source.texture);
                pIni->WriteFloat	(l->GetName(), "source_radius", 	l->m_LensFlare.m_Source.fRadius);
                AnsiString FT=""; AnsiString FR=""; AnsiString FO=""; AnsiString FP="";
                AnsiString T="";
                int i=l->m_LensFlare.m_Flares.size();
                for (CEditFlare::FlareIt it = l->m_LensFlare.m_Flares.begin(); it!=l->m_LensFlare.m_Flares.end(); it++,i--){
                	FT += it->texture;
                	T.sprintf("%.3f",it->fRadius); 	FR += T;
                	T.sprintf("%.3f",it->fOpacity);	FO += T;
                	T.sprintf("%.3f",it->fPosition);FP += T;
                    if (i>1){FT+=","; FR+=","; FO+=","; FP+=",";}
                }
               	pIni->WriteString	(l->GetName(), "flare_textures",	FT.c_str());
               	pIni->WriteString	(l->GetName(), "flare_radius",		FR.c_str());
               	pIni->WriteString	(l->GetName(), "flare_opacity",		FO.c_str());
               	pIni->WriteString	(l->GetName(), "flare_position",	FP.c_str());
            }
		}
        if (suns.Length()) pIni->WriteString("environment", "suns", suns.c_str());
	}

	// -- add respawn point --
	if (Scene.ObjCount(OBJCLASS_RPOINT)) {
		i = Scene.FirstObj(OBJCLASS_RPOINT);
        _E  = Scene.LastObj(OBJCLASS_RPOINT);
        AnsiString temp;
        for(;i!=_E;i++){
            CRPoint *rpt = (CRPoint *)(*i);
            if (CRPoint::etPlayer==rpt->m_Type){
	            temp.sprintf("%.3f,%.3f,%.3f,%d,%.3f", rpt->m_Position.x,rpt->m_Position.y,rpt->m_Position.z,rpt->m_dwTeamID,rpt->m_fHeading);
    	        pIni->WriteString("respawn_point",rpt->GetName(),temp.c_str());
            }
		}
	}

	// -- add ai-traffic --
	if (Scene.ObjCount(OBJCLASS_AITPOINT)) {
		i = Scene.FirstObj(OBJCLASS_AITPOINT);
        _E  = Scene.LastObj(OBJCLASS_AITPOINT);
        AnsiString temp;
        for(;i!=_E;i++){
        	CAITPoint* P = (CAITPoint*)(*i);
            temp.sprintf("%.3f,%.3f,%.3f", P->m_Position.x,P->m_Position.y,P->m_Position.z);
            for (ObjectIt it=P->m_Links.begin(); it!=P->m_Links.end(); it++)
	            temp += AnsiString(", ") + AnsiString((*it)->GetName());
            pIni->WriteString("ai_traffic",P->GetName(),temp.c_str());
		}
	}


end_ltx_build:
    delete pIni;

	return bResult;
}

